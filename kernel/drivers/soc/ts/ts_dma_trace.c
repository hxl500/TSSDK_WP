/*
 * Copyright (C) 2022 Tsingmicro, Inc.
 *
 * Author: evan yang <yangyihuan@tsingmicro.com>
 *
 * This file is released under the GPLv2.
 * See the file COPYING for more details.
 */

#include <linux/cma.h>
#include <linux/dma-mapping.h>
#include <linux/slab.h>
#include <linux/ts_dma_trace.h>
#include <linux/seq_file.h>
#include <linux/slab.h>

#define DMA_TRACE_HASH_SIZE 128ULL
#define DMA_TRACE_HASH_FN_SHIFT 13
#define DMA_TRACE_HASH_FN_MASK (DMA_TRACE_HASH_SIZE - 1)

#define DMA_TRACE_PREALLOC_DMA_TRACE_ENTRIES 640

typedef bool (*match_fn)(struct dma_trace_entry *, struct dma_trace_entry *);

/* Hash list to save the allocated dma addresses */
static struct dma_trace_hash_bucket dma_trace_entry_hash[DMA_TRACE_HASH_SIZE];
/* List of pre-allocated dma_trace_entry's */
static LIST_HEAD(free_entries);
/* Lock for the list above */
static DEFINE_SPINLOCK(free_entries_lock);

/* Global disable flag - will be set in case of an error */
static bool global_disable __read_mostly;

/* Early initialization disable flag, set at the end of dma_trace_init */
static bool dma_trace_initialized __read_mostly;

/* List of vb_entry's */
static LIST_HEAD(cma_sys_entries);

/* Lock for the list above */
static DEFINE_SPINLOCK(cma_sys_entries_lock);

static inline bool dma_trace_disabled(void)
{
	return global_disable || !dma_trace_initialized;
}

static u32 num_free_entries;
static u32 min_free_entries;

/* number of preallocated entries */
static u32 req_entries;

/*
 * Hash related functions
 *
 * Every dma_alloc_coherent request is saved into a struct dma_trace_entry. To
 * have quick access to these structs they are stored into a hash.
 */
static int hash_fn(struct dma_trace_entry *entry)
{
	/*
	 * Hash function is based on the dma address.
	 * We use bits 13-19 here as the index into the hash
	 */
	return (entry->dev_addr >> DMA_TRACE_HASH_FN_SHIFT) &
	       DMA_TRACE_HASH_FN_MASK;
}

/*
 * Request exclusive access to a hash bucket for a given dma_trace_entry.
 */
static struct dma_trace_hash_bucket *
get_hash_bucket(struct dma_trace_entry *entry, unsigned long *flags)
	__acquires(&dma_trace_entry_hash[idx].lock)
{
	int idx = hash_fn(entry);
	unsigned long __flags;

	spin_lock_irqsave(&dma_trace_entry_hash[idx].lock, __flags);
	*flags = __flags;
	return &dma_trace_entry_hash[idx];
}

/*
 * Give up exclusive access to the hash bucket
 */
static void put_hash_bucket(struct dma_trace_hash_bucket *bucket,
			    unsigned long *flags) __releases(&bucket->lock)
{
	unsigned long __flags = *flags;

	spin_unlock_irqrestore(&bucket->lock, __flags);
}

static bool exact_match(struct dma_trace_entry *a, struct dma_trace_entry *b)
{
	return (a->dev_addr == b->dev_addr);
}

/*
 * Search a given entry in the hash bucket list
 */
static struct dma_trace_entry *
__hash_bucket_find(struct dma_trace_hash_bucket *bucket,
		   struct dma_trace_entry *ref, match_fn match)
{
	struct dma_trace_entry *entry;

	list_for_each_entry(entry, &bucket->list, list) {
		if (!match(ref, entry))
			continue;
		break;
	}

	return entry;
}

static struct dma_trace_entry *
bucket_find_exact(struct dma_trace_hash_bucket *bucket,
		  struct dma_trace_entry *ref)
{
	return __hash_bucket_find(bucket, ref, exact_match);
}

/*
 * Add an entry to a hash bucket
 */
static void hash_bucket_add(struct dma_trace_hash_bucket *bucket,
			    struct dma_trace_entry *entry)
{
	list_add_tail(&entry->list, &bucket->list);
}

/*
 * Remove entry from a hash bucket list
 */
static void hash_bucket_del(struct dma_trace_entry *entry)
{
	list_del(&entry->list);
}

/*
 * Dump alloc entries for trace purposes
 */
void get_dma_trace_hash_array(struct dma_trace_hash_bucket **dma_trace_hash,
			      int *size)
{
	*dma_trace_hash = (struct dma_trace_hash_bucket *)&dma_trace_entry_hash;
	*size = DMA_TRACE_HASH_SIZE;
}
EXPORT_SYMBOL(get_dma_trace_hash_array);

/*
 * cma size convert to KB
 */
unsigned long get_cma_size(void)
{
	return totalcma_pages << (PAGE_SHIFT - 10);
}
EXPORT_SYMBOL(get_cma_size);

/* Wrapper function for adding an entry to the hash. */
static void add_dma_trace_entry(struct dma_trace_entry *entry)
{
	struct dma_trace_hash_bucket *bucket;
	unsigned long flags;

	bucket = get_hash_bucket(entry, &flags);
	hash_bucket_add(bucket, entry);
	put_hash_bucket(bucket, &flags);
}

static struct dma_trace_entry *__dma_trace_entry_alloc(void)
{
	struct dma_trace_entry *entry;

	entry = list_entry(free_entries.next, struct dma_trace_entry, list);
	list_del(&entry->list);
	memset(entry, 0, sizeof(*entry));

	num_free_entries -= 1;
	if (num_free_entries < min_free_entries)
		min_free_entries = num_free_entries;

	return entry;
}

/* struct dma_trace_entry allocator */
static struct dma_trace_entry *dma_trace_entry_alloc(void)
{
	struct dma_trace_entry *entry;
	unsigned long flags;

	spin_lock_irqsave(&free_entries_lock, flags);

	if (list_empty(&free_entries)) {
		global_disable = true;
		spin_unlock_irqrestore(&free_entries_lock, flags);
		pr_err("ts dma trace alloc out of memory - disabling\n");
		return NULL;
	}

	entry = __dma_trace_entry_alloc();

	spin_unlock_irqrestore(&free_entries_lock, flags);

	return entry;
}

static void dma_trace_entry_free(struct dma_trace_entry *entry)
{
	unsigned long flags;

	/* add no used entry to beginning of the list */
	spin_lock_irqsave(&free_entries_lock, flags);
	list_add(&entry->list, &free_entries);
	num_free_entries += 1;
	entry->size = 0;
	entry->dev_addr = 0;
	spin_unlock_irqrestore(&free_entries_lock, flags);
}

/*
 * dma trace init code
 *
 * The init code does two things:
 *   1. Initialize core data structures
 *   2. Preallocate a given number of dma_trace_entry structs
 */

static int prealloc_memory(u32 num_entries)
{
	struct dma_trace_entry *entry, *next_entry;
	int i;

	for (i = 0; i < num_entries; ++i) {
		entry = kzalloc(sizeof(*entry), GFP_KERNEL);
		if (!entry)
			goto out_err;

		list_add_tail(&entry->list, &free_entries);
	}

	num_free_entries = num_entries;
	min_free_entries = num_entries;

	pr_info("ts dma trace preallocated %d debug entries\n", num_entries);

	return 0;

out_err:

	list_for_each_entry_safe(entry, next_entry, &free_entries, list) {
		list_del(&entry->list);
		kfree(entry);
	}

	return -ENOMEM;
}

void dma_trace_init(u32 num_entries)
{
	int i;

	for (i = 0; i < DMA_TRACE_HASH_SIZE; ++i) {
		INIT_LIST_HEAD(&dma_trace_entry_hash[i].list);
		spin_lock_init(&dma_trace_entry_hash[i].lock);
	}

	if (req_entries)
		num_entries = req_entries;

	if (prealloc_memory(num_entries) != 0) {
		pr_err("ts dma trace alloc out of memory error - disabled\n");
		global_disable = true;

		return;
	}

	dma_trace_initialized = true;

	pr_info("ts dma trace enabled !\n");
}

static int __init dma_trace_do_init(void)
{
	dma_trace_init(DMA_TRACE_PREALLOC_DMA_TRACE_ENTRIES);
	return 0;
}
fs_initcall(dma_trace_do_init);

static void remove_dma_trace_entry(struct dma_trace_entry *ref)
{
	struct dma_trace_entry *entry;
	struct dma_trace_hash_bucket *bucket;
	unsigned long flags;

	bucket = get_hash_bucket(ref, &flags);
	entry = bucket_find_exact(bucket, ref);

	if (!entry) {
		put_hash_bucket(bucket, &flags);
		pr_err("%s can not find entry in bucket\n", __func__);
		return;
	}

	//if (ref->size != entry->size)
	//	pr_err("%s size not equal ref->size=%llu, entry->size=%llu\n",  __func__, ref->size, entry->size);

	if (entry->size != 0 && entry->dev_addr != 0) {
		hash_bucket_del(entry);
		dma_trace_entry_free(entry);
	}

	put_hash_bucket(bucket, &flags);
}

void trace_dma_alloc_coherent(struct device *dev, size_t size,
			      dma_addr_t dma_addr, void *virt)
{
	struct dma_trace_entry *entry;

	if (unlikely(dma_trace_disabled()))
		return;

	if (unlikely(virt == NULL))
		return;

	entry = dma_trace_entry_alloc();
	if (!entry)
		return;

	entry->size = size;
	entry->dev_addr = dma_addr;
	strcpy(entry->dev_name, dev->driver->name);

	add_dma_trace_entry(entry);
}
EXPORT_SYMBOL(trace_dma_alloc_coherent);

void trace_dma_free_coherent(struct device *dev, size_t size, void *virt,
			     dma_addr_t addr)
{
	struct dma_trace_entry ref = {
		.dev_addr = addr,
		.size = size,
	};

	if (unlikely(dma_trace_disabled()))
		return;

	remove_dma_trace_entry(&ref);
}
EXPORT_SYMBOL(trace_dma_free_coherent);

void cma_dma_show_info(struct seq_file *m, unsigned int *kb_size)
{
	int idx, size;
	unsigned int size_kb = 0;
	unsigned long flags;
	struct dma_trace_hash_bucket *hash_bucket_array;
	get_dma_trace_hash_array(&hash_bucket_array, &size);
	if (NULL == hash_bucket_array || 0 == size) {
		pr_err("hash_bucket_array or size is null\n");
	}

	for (idx = 0; idx < size; idx++) {
		struct dma_trace_hash_bucket *bucket = (struct dma_trace_hash_bucket *)(hash_bucket_array + idx);
		struct dma_trace_entry *entry;

		if (NULL == bucket)
			continue;

		spin_lock_irqsave(&bucket->lock, flags);

		list_for_each_entry(entry, &bucket->list, list) {
			if (NULL == entry)
				continue;
			seq_printf(m, "   |-MMB: " TS_MMB_FMT "\n", TS_MMB_FMT_ARG(entry));
			size_kb += PAGE_ALIGN(entry->size) / SZ_1K;
		}

		spin_unlock_irqrestore(&bucket->lock, flags);
	}
	*kb_size = size_kb;
}
EXPORT_SYMBOL(cma_dma_show_info);

/* show cma sys info by entry list ,contain paddr, size, name */

void cma_sys_show_info(struct seq_file *m, unsigned long *kb_size)
{
	unsigned long size_kb = 0;
	struct cma_sys_entry *entry, *next_entry;

	spin_lock(&cma_sys_entries_lock);
	list_for_each_entry_safe(entry, next_entry, &cma_sys_entries, list) {
		if (entry) {
			size_kb += PAGE_ALIGN(entry->size) / SZ_1K;
			seq_printf(m, "   |-MMB: " TS_SYS_MMB_FMT "\n", TS_SYS_MMB_FMT_ARG(entry));
		}
		*kb_size = size_kb;
	}
	spin_unlock(&cma_sys_entries_lock);
}
EXPORT_SYMBOL(cma_sys_show_info);

/* add cma sys entry list by ion_ioc_alloc , and set entry paddr, size */

void cma_sys_entry_list_add(unsigned long paddr, size_t size, char *name)
{
	struct cma_sys_entry *entry = NULL;

	entry = kzalloc(sizeof(struct cma_sys_entry), GFP_KERNEL);
	entry->paddr = paddr;
	entry->size = size;
	strcpy(entry->user_name, name);
	pr_debug("cma_sys_entry_add_list paddr=%lx, size=%lld, user_name=%s\n", entry->paddr, entry->size, entry->user_name);
	spin_lock(&cma_sys_entries_lock);
	list_add(&entry->list, &cma_sys_entries);
	spin_unlock(&cma_sys_entries_lock);
}
EXPORT_SYMBOL(cma_sys_entry_list_add);

/* delete cma sys entry list by ion cma distroy */

void cma_sys_entry_list_del(unsigned long paddr)
{
	struct cma_sys_entry *entry, *next_entry;
	spin_lock(&cma_sys_entries_lock);
	pr_debug("cma_sys_entry_del_list paddr=%lx\n", paddr);
	list_for_each_entry_safe(entry, next_entry, &cma_sys_entries, list) {
			if (entry->paddr == paddr) {
				list_del(&entry->list); // Remove from the list first
				spin_unlock(&cma_sys_entries_lock); // Unlock before potentially blocking operations
				kfree(entry); // Free memory after removing from the list
				return;
			}
	}
	spin_unlock(&cma_sys_entries_lock);
}
EXPORT_SYMBOL(cma_sys_entry_list_del);

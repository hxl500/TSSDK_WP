/*
 * Copyright (C) 2022 Tsingmicro, Inc.
 *
 * Author: evan yang <yangyihuan@tsingmicro.com>
 *
 * This file is released under the GPLv2.
 * See the file COPYING for more details.
 */

#ifndef __TS_DMA_TRACE_H
#define __TS_DMA_TRACE_H

/**
 * struct dma_trace_entry - track a dma_alloc_coherent mapping
 * @list: node on pre-allocated free_entries list
 * @dev_name: device name of dma_alloc_coherent caller
 * @dev_addr: physical address of dma_alloc_coherent
 * @size: length of the mapping
 */
struct dma_trace_entry {
	struct list_head list;
	char dev_name[20];
	u64 dev_addr;
	u64 size;
};

struct cma_sys_entry {
       struct list_head list;
       char user_name[20];
       uintptr_t paddr;
       uintptr_t vaddr;
       u64 size;
};

struct dma_trace_hash_bucket {
	struct list_head list;
	spinlock_t lock;
} ____cacheline_aligned_in_smp;

#define MMZ_GRAIN PAGE_SIZE
#define mmz_align2(x, g) ((((x) + (g)-1) / (g)) * (g))
#define mmz_grain_align(x) mmz_align2(x, MMZ_GRAIN)

#define TS_SYS_MMB_FMT "phys(0x%08lX, 0x%08llX), length=%lluKB,  name=\"%s\""
#define TS_SYS_MMB_FMT_ARG(entry)                                                                                           \
	(entry)->paddr, mmz_grain_align((entry)->paddr + (entry)->size) - 1, PAGE_ALIGN((entry)->size) / SZ_1K, (entry)->user_name

#define TS_MMB_FMT "phys(0x%08llX, 0x%08llX), length=%lluKB,  name=\"%s\""
#define TS_MMB_FMT_ARG(p)                                                                                              \
	(p)->dev_addr, mmz_grain_align((p)->dev_addr + (p)->size) - 1, PAGE_ALIGN((p)->size) / SZ_1K, (p)->dev_name

#ifdef CONFIG_TS_DMA_TRACE

extern void trace_dma_alloc_coherent(struct device *dev, size_t size,
				     dma_addr_t dma_addr, void *virt);

extern void trace_dma_free_coherent(struct device *dev, size_t size, void *virt,
				    dma_addr_t addr);

extern void
get_dma_trace_hash_array(struct dma_trace_hash_bucket **dma_trace_hash,
			 int *size);

extern unsigned long get_cma_size(void);
extern void cma_dma_show_info(struct seq_file *m, unsigned int *kb_size);

extern void cma_sys_show_info(struct seq_file *m, unsigned long *kb_size);
extern void cma_sys_entry_list_add(unsigned long paddr, size_t size, char *name);
extern void cma_sys_entry_list_del(unsigned long paddr);

#else  /* CONFIG_TS_DMA_TRACE */

static inline void trace_dma_alloc_coherent(struct device *dev, size_t size,
					    dma_addr_t dma_addr, void *virt)
{
}

static inline void trace_dma_free_coherent(struct device *dev, size_t size,
					   void *virt, dma_addr_t addr)
{
}

static inline void
get_dma_trace_hash_array(struct dma_trace_hash_bucket **dma_trace_hash,
			 int *size)
{
}

static inline unsigned long get_cma_size(void)
{
	return 0;
}

static inline void cma_dma_show_info(struct seq_file *m, unsigned int *kb_size)
{
}

static inline void cma_sys_show_info(struct seq_file *m, unsigned long *kb_size)
{
}

static inline void cma_sys_entry_list_add(unsigned long paddr, size_t size, char *name)
{
}

static inline void cma_sys_entry_list_del(unsigned long paddr)
{
}

#endif /* CONFIG_TS_DMA_TRACE */

#endif /* __TS_DMA_TRACE_H */

// SPDX-License-Identifier: GPL-2.0
/*
 *  linux/drivers/char/mem.c
 *
 *  Copyright (C) 1991, 1992  Linus Torvalds
 *
 *  Added devfs support.
 *    Jan-11-1998, C. Scott Ananian <cananian@alumni.princeton.edu>
 *  Shared /dev/zero mmapping support, Feb 2000, Kanoj Sarcar <kanoj@sgi.com>
 */

#include <linux/mm.h>
#include <linux/miscdevice.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <linux/mman.h>
#include <linux/random.h>
#include <linux/init.h>
#include <linux/raw.h>
#include <linux/tty.h>
#include <linux/capability.h>
#include <linux/ptrace.h>
#include <linux/device.h>
#include <linux/highmem.h>
#include <linux/backing-dev.h>
#include <linux/shmem_fs.h>
#include <linux/splice.h>
#include <linux/pfn.h>
#include <linux/export.h>
#include <linux/io.h>
#include <linux/uio.h>
#include <linux/major.h>

#include <linux/uaccess.h>

#ifdef CONFIG_IA64
# include <linux/efi.h>
#endif

#include <linux/cma.h>

static loff_t memory_lseek(struct file *file, loff_t offset, int orig);
static ssize_t read_mem(struct file *file, char __user *buf,
			size_t count, loff_t *ppos);
static ssize_t write_mem(struct file *file, const char __user *buf,
			 size_t count, loff_t *ppos);
static int mmap_mem(struct file *file, struct vm_area_struct *vma);
static int open_port(struct inode *inode, struct file *filp);

#define open_mem	open_port

static struct file_operations __maybe_unused mem_fops = {
	.llseek		= memory_lseek,
	.read		= read_mem,
	.write		= write_mem,
	.mmap		= mmap_mem,
	.open		= open_mem,
#ifndef CONFIG_MMU
	.get_unmapped_area = get_unmapped_area_mem,
	.mmap_capabilities = memory_mmap_capabilities,
#endif
};

static struct memdev {
	const char *name;
	umode_t mode;
	const struct file_operations *fops;
	fmode_t fmode;
	unsigned int minor;
} devlist[] = {
#ifdef CONFIG_DEVMEM
	[0] = {"mmzmem", 0, &mem_fops, FMODE_UNSIGNED_OFFSET, 0},
	[1] = {"mmzmem_ram", 0, &mem_fops, FMODE_UNSIGNED_OFFSET, 1},
#endif
};

static inline unsigned long size_inside_page(unsigned long start,
					     unsigned long size)
{
	unsigned long sz;

	sz = PAGE_SIZE - (start & (PAGE_SIZE - 1));

	return min(sz, size);
}

#ifndef ARCH_HAS_VALID_PHYS_ADDR_RANGE
static inline int valid_phys_addr_range(phys_addr_t addr, size_t count)
{
	return addr + count <= __pa(high_memory);
}

static inline int valid_mmap_phys_addr_range(unsigned long pfn, size_t size)
{
	return 1;
}
#endif

#ifdef CONFIG_STRICT_DEVMEM
int valid_cma_range_cb(struct cma *cma, void *data)
{
	unsigned long pfn = *(unsigned long *)data;

	if (pfn < (cma_get_base(cma) + cma_get_size(cma)))
		return 1;

	return 0;
}

static inline int valid_cma_range(unsigned long pfn)
{
	int ret;

	ret = cma_for_each_area(valid_cma_range_cb, &pfn);

	return ret;
}

static inline int page_is_allowed(unsigned long pfn)
{
	if (valid_cma_range(pfn))
		return 1;

	return devmem_is_allowed(pfn);
}
static inline int range_is_allowed(unsigned long pfn, unsigned long size)
{
	u64 from = ((u64)pfn) << PAGE_SHIFT;
	u64 to = from + size;
	u64 cursor = from;

	while (cursor < to) {
		if (!page_is_allowed(pfn))
			return 0;
		cursor += PAGE_SIZE;
		pfn++;
	}
	return 1;
}
#else
static inline int page_is_allowed(unsigned long pfn)
{
	return 1;
}
static inline int range_is_allowed(unsigned long pfn, unsigned long size)
{
	return 1;
}
#endif

#ifndef unxlate_dev_mem_ptr
#define unxlate_dev_mem_ptr unxlate_dev_mem_ptr
void __weak unxlate_dev_mem_ptr(phys_addr_t phys, void *addr)
{
}
#endif

static inline bool should_stop_iteration(void)
{
	if (need_resched())
		cond_resched();
	return fatal_signal_pending(current);
}

/*
 * This funcion reads the *physical* memory. The f_pos points directly to the
 * memory location.
 */
static ssize_t read_mem(struct file *file, char __user *buf,
			size_t count, loff_t *ppos)
{
	phys_addr_t p = *ppos;
	ssize_t read, sz;
	void *ptr;
	char *bounce;
	int err;

	if (p != *ppos)
		return 0;

	if (!valid_phys_addr_range(p, count))
		return -EFAULT;
	read = 0;
#ifdef __ARCH_HAS_NO_PAGE_ZERO_MAPPED
	/* we don't have page 0 mapped on sparc and m68k.. */
	if (p < PAGE_SIZE) {
		sz = size_inside_page(p, count);
		if (sz > 0) {
			if (clear_user(buf, sz))
				return -EFAULT;
			buf += sz;
			p += sz;
			count -= sz;
			read += sz;
		}
	}
#endif

	bounce = kmalloc(PAGE_SIZE, GFP_KERNEL);
	if (!bounce)
		return -ENOMEM;

	while (count > 0) {
		unsigned long remaining;
		int allowed, probe;

		sz = size_inside_page(p, count);

		err = -EPERM;
		allowed = page_is_allowed(p >> PAGE_SHIFT);
		if (!allowed)
			goto failed;

		err = -EFAULT;
		if (allowed == 2) {
			/* Show zeros for restricted memory. */
			remaining = clear_user(buf, sz);
		} else {
			/*
			 * On ia64 if a page has been mapped somewhere as
			 * uncached, then it must also be accessed uncached
			 * by the kernel or data corruption may occur.
			 */
			ptr = xlate_dev_mem_ptr(p);
			if (!ptr)
				goto failed;

			probe = probe_kernel_read(bounce, ptr, sz);
			unxlate_dev_mem_ptr(p, ptr);
			if (probe)
				goto failed;

			remaining = copy_to_user(buf, bounce, sz);
		}

		if (remaining)
			goto failed;

		buf += sz;
		p += sz;
		count -= sz;
		read += sz;
		if (should_stop_iteration())
			break;
	}
	kfree(bounce);

	*ppos += read;
	return read;

failed:
	kfree(bounce);
	return err;
}

static ssize_t write_mem(struct file *file, const char __user *buf,
			 size_t count, loff_t *ppos)
{
	phys_addr_t p = *ppos;
	ssize_t written, sz;
	unsigned long copied;
	void *ptr;

	if (p != *ppos)
		return -EFBIG;

	if (!valid_phys_addr_range(p, count))
		return -EFAULT;

	written = 0;

#ifdef __ARCH_HAS_NO_PAGE_ZERO_MAPPED
	/* we don't have page 0 mapped on sparc and m68k.. */
	if (p < PAGE_SIZE) {
		sz = size_inside_page(p, count);
		/* Hmm. Do something? */
		buf += sz;
		p += sz;
		count -= sz;
		written += sz;
	}
#endif

	while (count > 0) {
		int allowed;

		sz = size_inside_page(p, count);

		allowed = page_is_allowed(p >> PAGE_SHIFT);
		if (!allowed)
			return -EPERM;

		/* Skip actual writing when a page is marked as restricted. */
		if (allowed == 1) {
			/*
			 * On ia64 if a page has been mapped somewhere as
			 * uncached, then it must also be accessed uncached
			 * by the kernel or data corruption may occur.
			 */
			ptr = xlate_dev_mem_ptr(p);
			if (!ptr) {
				if (written)
					break;
				return -EFAULT;
			}

			copied = copy_from_user(ptr, buf, sz);
			unxlate_dev_mem_ptr(p, ptr);
			if (copied) {
				written += sz - copied;
				if (written)
					break;
				return -EFAULT;
			}
		}

		buf += sz;
		p += sz;
		count -= sz;
		written += sz;
		if (should_stop_iteration())
			break;
	}

	*ppos += written;
	return written;
}

int __weak phys_mem_access_prot_allowed(struct file *file,
	unsigned long pfn, unsigned long size, pgprot_t *vma_prot)
{
	return 1;
}

#ifndef __HAVE_PHYS_MEM_ACCESS_PROT

/*
 * Architectures vary in how they handle caching for addresses
 * outside of main memory.
 *
 */
#ifdef pgprot_noncached
static int uncached_access(struct file *file, phys_addr_t addr)
{
#if defined(CONFIG_IA64)
	/*
	 * On ia64, we ignore O_DSYNC because we cannot tolerate memory
	 * attribute aliases.
	 */
	return !(efi_mem_attributes(addr) & EFI_MEMORY_WB);
#else
	/*
	 * Accessing memory above the top the kernel knows about or through a
	 * file pointer
	 * that was marked O_DSYNC will be done non-cached.
	 */
	if (file->f_flags & O_DSYNC)
		return 1;

	return addr >= __pa(high_memory);
#endif
}
#endif

static pgprot_t phys_mem_access_prot(struct file *file, unsigned long pfn,
				     unsigned long size, pgprot_t vma_prot)
{
#ifdef pgprot_noncached
	phys_addr_t offset = pfn << PAGE_SHIFT;

	if (uncached_access(file, offset))
		return pgprot_noncached(vma_prot);
#endif
	return vma_prot;
}
#endif

#ifndef CONFIG_MMU
static unsigned long get_unmapped_area_mem(struct file *file,
					   unsigned long addr,
					   unsigned long len,
					   unsigned long pgoff,
					   unsigned long flags)
{
	if (!valid_mmap_phys_addr_range(pgoff, len))
		return (unsigned long) -EINVAL;
	return pgoff << PAGE_SHIFT;
}

/* permit direct mmap, for read, write or exec */
static unsigned int memory_mmap_capabilities(struct file *file)
{
	return NOMMU_MAP_DIRECT |
		NOMMU_MAP_READ | NOMMU_MAP_WRITE | NOMMU_MAP_EXEC;
}

/* can't do an in-place private mapping if there's no MMU */
static inline int private_mapping_ok(struct vm_area_struct *vma)
{
	return vma->vm_flags & VM_MAYSHARE;
}
#else

static inline int private_mapping_ok(struct vm_area_struct *vma)
{
	return 1;
}
#endif

static const struct vm_operations_struct mmap_mem_ops = {
#ifdef CONFIG_HAVE_IOREMAP_PROT
	.access = generic_access_phys
#endif
};

static int mmap_mem(struct file *file, struct vm_area_struct *vma)
{
	size_t size = vma->vm_end - vma->vm_start;
	phys_addr_t offset = (phys_addr_t)vma->vm_pgoff << PAGE_SHIFT;
	struct memdev *dev = file->private_data;

	/* It's illegal to wrap around the end of the physical address space. */
	if (offset + (phys_addr_t)size - 1 < offset)
		return -EINVAL;

	if (!valid_mmap_phys_addr_range(vma->vm_pgoff, size))
		return -EINVAL;

	if (!private_mapping_ok(vma))
		return -EINVAL;

	if (dev->minor != 1) {
		if (!range_is_allowed(vma->vm_pgoff, size))
			return -EPERM;
	}

	if (!phys_mem_access_prot_allowed(file, vma->vm_pgoff, size,
						&vma->vm_page_prot))
		return -EINVAL;

	// default using cache
	if ((dev->minor == 1) && !(file->f_flags & O_DSYNC)) {
		;
	} else {
		vma->vm_page_prot = phys_mem_access_prot(file, vma->vm_pgoff,
							 size,
							 vma->vm_page_prot);
	}

	vma->vm_ops = &mmap_mem_ops;

	/* Remap-pfn-range will mark the range VM_IO */
	if (remap_pfn_range(vma,
			    vma->vm_start,
			    vma->vm_pgoff,
			    size,
			    vma->vm_page_prot)) {
		return -EAGAIN;
	}

	return 0;
}


/*
 * The memory devices use the full 32/64 bits of the offset, and so we cannot
 * check against negative addresses: they are ok. The return value is weird,
 * though, in that case (0).
 *
 * also note that seeking relative to the "end of file" isn't supported:
 * it has no meaning, so it returns -EINVAL.
 */
static loff_t memory_lseek(struct file *file, loff_t offset, int orig)
{
	loff_t ret;

	inode_lock(file_inode(file));
	switch (orig) {
	case SEEK_CUR:
		offset += file->f_pos;
	case SEEK_SET:
		/* to avoid userland mistaking f_pos=-9 as -EBADF=-9 */
		if ((unsigned long long)offset >= -MAX_ERRNO) {
			ret = -EOVERFLOW;
			break;
		}
		file->f_pos = offset;
		ret = file->f_pos;
		force_successful_syscall_return();
		break;
	default:
		ret = -EINVAL;
	}
	inode_unlock(file_inode(file));
	return ret;
}

static int open_port(struct inode *inode, struct file *filp)
{
	return capable(CAP_SYS_RAWIO) ? 0 : -EPERM;
}



static int memory_open(struct inode *inode, struct file *filp)
{
	int minor;
	struct memdev *dev;

	minor = iminor(inode);
	if (minor >= ARRAY_SIZE(devlist))
		return -ENXIO;

	dev = &devlist[minor];
	if (!dev->fops)
		return -ENXIO;

	filp->f_op = dev->fops;
	filp->f_mode |= dev->fmode;
	filp->private_data = dev;

	if (dev->fops->open)
		return dev->fops->open(inode, filp);

	return 0;
}

static const struct file_operations memory_fops = {
	.open = memory_open,
	.llseek = noop_llseek,
};

static struct cdev mmz_cdev;

static char *mem_devnode(struct device *dev, umode_t *mode)
{
	if (mode && devlist[MINOR(dev->devt)].mode)
		*mode = devlist[MINOR(dev->devt)].mode;
	return NULL;
}

static struct class *mem_class;
#define MMZMEM_DEV_NUM 2
static int __init ts_mem_init(void)
{
	int ret;
	dev_t devno = 0;

#if defined(TS_MMZMEM_MAJOR)
	devno = MKDEV(TS_MMZMEM_MAJOR, 0);
#endif

	if (devno) {
		ret = register_chrdev_region(devno, MMZMEM_DEV_NUM, "mmzmem");
	} else {
		ret = alloc_chrdev_region(&devno, 0, MMZMEM_DEV_NUM, "mmzmem");
	}
	if (ret < 0) {
		pr_err("%s alloc_chrdev_region err\n", __func__);
		return ret;
	}

	cdev_init(&mmz_cdev, &memory_fops);
	mmz_cdev.owner = THIS_MODULE;
	ret = cdev_add(&mmz_cdev, devno, MMZMEM_DEV_NUM);
	if (ret) {
		pr_err("cdev_add err.\r\n");
		return ret;
	}

	mem_class = class_create(THIS_MODULE, "mmzmem");
	if (IS_ERR(mem_class))
		return PTR_ERR(mem_class);
	mem_class->devnode = mem_devnode;

	device_create(mem_class, NULL, MKDEV(MAJOR(devno), 0), NULL, devlist[0].name);
	device_create(mem_class, NULL, MKDEV(MAJOR(devno), 1), NULL, devlist[1].name);
	return 0;
}

fs_initcall(ts_mem_init);


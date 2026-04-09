/***************************************************************************
 *                      COPYRIGHT NOTICE
 *               Copyright 2023 Tsingmicro, Inc.
 *                     All rights reserved.
 ***************************************************************************/
#define pr_fmt(fmt)	"cache_op: " fmt

#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/uaccess.h>
#include <linux/dma-mapping.h>
#include <linux/miscdevice.h>
#include <linux/cache.h>
#include <asm/cacheflush.h>
#ifdef __arm__
#include <asm/glue-cache.h>
#endif
#ifdef __aarch64__
#include <asm/uaccess.h>
#endif
#include <soc/ts/cache_op_uapi.h>

#define DEV_NAME "cache_op"


#define MAX_MEMCOPY_SIZE 0x10000000
#define MAX_ELEM_SIZE 0x100
#define MAX_SEG_SIZE 0x10000
#define MAX_LINE_SIZE 0x10000

#ifdef __arm__
#define dmac_map_area			__glue(_CACHE, _dma_map_area)
#define dmac_unmap_area 		__glue(_CACHE, _dma_unmap_area)
extern void dmac_map_area(const void *, size_t, int);
extern void dmac_unmap_area(const void *, size_t, int);
#endif

#ifdef __aarch64__
extern void __dma_map_area(const void *, size_t, int);
#endif


static int cache_op_mem_cache_clean(unsigned long arg)
{
	int ret = 0;
	struct cache_operation  ca_op;
	uintptr_t clean_pa;
	uintptr_t clean_va;
	uintptr_t clean_sz;

	ret = copy_from_user((char *)&ca_op, (u32 __user *)arg,
		sizeof(struct cache_operation));
	if (unlikely(ret)) {
		pr_err("%s %d ca_op copy error %d.\n", __func__, __LINE__, ret);
		ret = -1; goto err;
	}

	if ((ca_op.paddr % L1_CACHE_BYTES != 0)
		|| (ca_op.vaddr % L1_CACHE_BYTES != 0)
		|| (ca_op.size % L1_CACHE_BYTES != 0))
		pr_debug("%s %d addr or size not align\n",  __func__, __LINE__);

	clean_pa = L1_CACHE_ALIGN(ca_op.paddr);
	clean_va = L1_CACHE_ALIGN(ca_op.vaddr);
	clean_sz = ca_op.paddr - clean_pa + ca_op.size;
	clean_sz = round_up(clean_sz, SMP_CACHE_BYTES);
	pr_debug("clean_pa %lx clean_va %lx sz %lx\n", clean_pa, clean_va,
		clean_sz);

#ifdef __arm__
	dmac_map_area((const void *)clean_va, clean_sz, DMA_TO_DEVICE);
	outer_clean_range(clean_pa, clean_pa + clean_sz);
#endif

#ifdef __aarch64__
	__dma_map_area((const void *)clean_va, clean_sz, DMA_TO_DEVICE);
#endif

	return 0;

err:
	pr_err("%s %d error%d.\n", __func__, __LINE__, ret);
	return ret;
}

static int cache_op_mem_cache_inv(unsigned long arg)
{
	int ret = 0;
	struct cache_operation ca_op;
	uintptr_t inv_pa;
	uintptr_t inv_va;
	uintptr_t inv_sz;

	ret = copy_from_user((char *)&ca_op, (u32 __user *)arg,
		sizeof(struct cache_operation));
	if (unlikely(ret)) {
		pr_err("%s %d ca_op copy error %d.\n", __func__, __LINE__, ret);
		ret = -1; goto err;
	}

	if ((ca_op.paddr % L1_CACHE_BYTES != 0)
		|| (ca_op.vaddr % L1_CACHE_BYTES != 0)
		|| (ca_op.size % L1_CACHE_BYTES != 0))
		pr_debug("%s %d addr or size not align\n",  __func__, __LINE__);

	inv_pa = L1_CACHE_ALIGN(ca_op.paddr);
	inv_va = L1_CACHE_ALIGN(ca_op.vaddr);
	inv_sz = ca_op.paddr - inv_pa + ca_op.size;
	inv_sz = round_up(inv_sz, SMP_CACHE_BYTES);
	pr_debug("inv_pa %lx inv_va %lx sz %lx\n", inv_pa, inv_va, inv_sz);

#ifdef __arm__
	dmac_map_area((const void *)inv_va, inv_sz, DMA_FROM_DEVICE);
	outer_inv_range(inv_pa, inv_pa + inv_sz);
#endif
#ifdef __aarch64__
	uaccess_enable_not_uao();
	__dma_map_area((const void *)inv_va, inv_sz, DMA_FROM_DEVICE);
	uaccess_disable_not_uao();
#endif

	return 0;

err:
	pr_err("%s %d error%d.\n", __func__, __LINE__, ret);
	return ret;
}


static long cache_op_mem_ioctl(struct file *file, unsigned int cmd,
			  unsigned long arg)
{
	int ret;

	switch (cmd) {
	case CACHE_OP_IOC_CLEAN_CACHE:
		ret = cache_op_mem_cache_clean(arg);
		break;
	case CACHE_OP_IOC_INVALIDATE_CACHE:
		ret = cache_op_mem_cache_inv(arg);
		break;
	default:
		pr_info("%s %d unknow cmd %x\n", __func__, __LINE__, cmd);
		break;
	}

	return ret;
}

static int cache_op_mem_open(struct inode *inode, struct file *file)
{
	return 0;
}

static int cache_op_mem_close(struct inode *inode, struct file *file)
{
	return 0;
}

const static struct file_operations cache_op_dev_fops = {
	.owner = THIS_MODULE,
	.open = cache_op_mem_open,
	.release = cache_op_mem_close,
	.unlocked_ioctl = cache_op_mem_ioctl,
};

static struct miscdevice cache_op_misc = {
#if defined(TS_CACHE_OP_MINOR)
	.minor = TS_CACHE_OP_MINOR,
#else
	.minor = MISC_DYNAMIC_MINOR,
#endif
	.name = DEV_NAME,
	.fops = &cache_op_dev_fops,
};

static int __init ts_cache_op_init(void)
{
	int ret;

	ret = misc_register(&cache_op_misc);
	if (ret) {
		pr_err("fail to register cache_op %d\n", ret);
		return ret;
	}

	pr_info("cache operation device success.\n");
	return ret;
}
late_initcall(ts_cache_op_init);


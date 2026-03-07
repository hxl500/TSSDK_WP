/***************************************************************************
 *                      COPYRIGHT NOTICE
 *               Copyright 2022 Tsingmicro, Inc.
 *                     All rights reserved.
 ***************************************************************************/

#ifndef __TS_CACHE_OP_UAPI_H__
#define __TS_CACHE_OP_UAPI_H__

#include <linux/ioctl.h>

/* /dev/cache_op */
#define CACHE_OP_DEV_NAME "/dev/cache_op"

struct cache_operation;


/*
 * command for ioctl()
 * CLEAN_CACHE: cache -> memory, using before dma read and after cpu write
 * INVALIDATE_CACHE: invadate data in cache, using dma write and before cpu read
 */
#define CACHE_OP_IOC_MAGIC 'c'

#define CACHE_OP_IOC_CLEAN_CACHE	_IOW(CACHE_OP_IOC_MAGIC, 1, struct cache_operation)
#define CACHE_OP_IOC_INVALIDATE_CACHE	_IOW(CACHE_OP_IOC_MAGIC, 2, struct cache_operation)

/*
 * cache operation unit is cache-line:
 * paddr, vaddr and size should align with cache-line.
 *
 * cache-line:
 *     = 64 Bytes, arch32 system
 *     = 128 Bytes, arch64 system
 */
struct cache_operation {
	uintptr_t paddr;
	uintptr_t vaddr;

	uint32_t size;		/* unit: Bytes */
};

#endif

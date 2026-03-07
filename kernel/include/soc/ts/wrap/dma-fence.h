/***************************************************************************
 *                      COPYRIGHT NOTICE
 *               Copyright 2022 Tsingmicro, Inc.
 *                     All rights reserved.
 ***************************************************************************/

#ifndef __TS_WRAP_DMA_FENCE_H__
#define __TS_WRAP_DMA_FENCE_H__

#include <linux/dma-fence.h>
#include <linux/dma-fence-array.h>

struct dma_fence *ts_dma_fence_get(struct dma_fence *fence);
void ts_dma_fence_put(struct dma_fence *fence);
bool ts_dma_fence_is_signaled(struct dma_fence *fence);

struct dma_fence_array *ts_to_dma_fence_array(struct dma_fence *fence);
struct dma_fence **ts_dma_fence_array_get_fences(struct dma_fence_array *array);
unsigned int ts_dma_fence_array_get_num_fences(struct dma_fence_array *array);

#endif


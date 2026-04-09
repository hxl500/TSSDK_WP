#include <linux/module.h>
#include <soc/ts/wrap/dma-fence.h>


struct dma_fence *ts_dma_fence_get(struct dma_fence *fence)
{
	return dma_fence_get(fence);
}
EXPORT_SYMBOL(ts_dma_fence_get);

void ts_dma_fence_put(struct dma_fence *fence)
{
	dma_fence_put(fence);
}
EXPORT_SYMBOL(ts_dma_fence_put);


bool ts_dma_fence_is_signaled(struct dma_fence *fence)
{
	return dma_fence_is_signaled(fence);
}
EXPORT_SYMBOL(ts_dma_fence_is_signaled);

struct dma_fence_array *ts_to_dma_fence_array(struct dma_fence *fence)
{
	return to_dma_fence_array(fence);
}
EXPORT_SYMBOL(ts_to_dma_fence_array);

struct dma_fence **ts_dma_fence_array_get_fences(struct dma_fence_array *array)
{
	return array->fences;
}
EXPORT_SYMBOL(ts_dma_fence_array_get_fences);

unsigned int ts_dma_fence_array_get_num_fences(struct dma_fence_array *array)
{
	return array->num_fences;
}
EXPORT_SYMBOL(ts_dma_fence_array_get_num_fences);


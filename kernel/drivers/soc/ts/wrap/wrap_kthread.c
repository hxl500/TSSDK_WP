#include <linux/module.h>
#include <soc/ts/wrap/kthread.h>
#include <linux/slab.h>

struct kthread_worker *ts_alloc_st_kthread_worker(void)
{
	return kzalloc(sizeof(struct kthread_worker), GFP_KERNEL);
}
EXPORT_SYMBOL_GPL(ts_alloc_st_kthread_worker);


#include <linux/module.h>
#include <soc/ts/wrap/semaphore.h>
#include <linux/slab.h>

struct semaphore *ts_alloc_st_semaphore(void)
{
	return kzalloc(sizeof(struct semaphore), GFP_KERNEL);
}
EXPORT_SYMBOL_GPL(ts_alloc_st_semaphore);

void ts_sema_init(struct semaphore *sem, int val)
{
	sema_init(sem, val);
}
EXPORT_SYMBOL_GPL(ts_sema_init);



#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/slab.h>


struct mutex *ts_alloc_st_mutex(void)
{
	return kzalloc(sizeof(struct mutex), GFP_KERNEL);
}
EXPORT_SYMBOL(ts_alloc_st_mutex);

size_t ts_get_st_mutex_size(void)
{
	return sizeof(struct mutex);
}
EXPORT_SYMBOL(ts_get_st_mutex_size);


void ts_mutex_init(struct mutex *lock)
{
	mutex_init(lock);
}
EXPORT_SYMBOL(ts_mutex_init);


void ts_mutex_lock(struct mutex *lock)
{
	mutex_lock(lock);
}
EXPORT_SYMBOL(ts_mutex_lock);

void ts_mutex_unlock(struct mutex *lock)
{
	mutex_unlock(lock);
}
EXPORT_SYMBOL(ts_mutex_unlock);

int ts_mutex_lock_interruptible(struct mutex *lock)
{
	return mutex_lock_interruptible(lock);
}
EXPORT_SYMBOL(ts_mutex_lock_interruptible);


void ts_mutex_destroy(struct mutex *lock)
{
	mutex_destroy(lock);
}
EXPORT_SYMBOL(ts_mutex_destroy);


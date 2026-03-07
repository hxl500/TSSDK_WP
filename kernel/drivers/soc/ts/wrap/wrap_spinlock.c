#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/printk.h>
#include <linux/spinlock.h>
#include <linux/slab.h>


spinlock_t *ts_alloc_st_spinlock(void)
{
	return kzalloc(sizeof(spinlock_t), GFP_KERNEL);
}
EXPORT_SYMBOL_GPL(ts_alloc_st_spinlock);

size_t ts_get_st_spinlock_size(void)
{
	return sizeof(spinlock_t);
}
EXPORT_SYMBOL(ts_get_st_spinlock_size);

void ts_spin_lock_init(spinlock_t *lock)
{
	spin_lock_init(lock);
}
EXPORT_SYMBOL(ts_spin_lock_init);


void ts_spin_lock(spinlock_t *lock)
{
	spin_lock(lock);
}
EXPORT_SYMBOL(ts_spin_lock);

int ts_spin_trylock(spinlock_t *lock)
{
	return spin_trylock(lock);
}
EXPORT_SYMBOL(ts_spin_trylock);


void ts_spin_unlock(spinlock_t *lock)
{
	spin_unlock(lock);
}
EXPORT_SYMBOL(ts_spin_unlock);

/*
 * attention: we can not use #define, so flag type is pointer
 */
void ts_spin_lock_irqsave(spinlock_t *lock, unsigned long *flags)
{
	unsigned long f;

	spin_lock_irqsave(lock, f);
	*flags = f;
}
EXPORT_SYMBOL(ts_spin_lock_irqsave);

/*
 * attention: we can not use #define, so flag type is pointer
 */
void ts_spin_unlock_irqrestore(spinlock_t *lock, unsigned long *flags)
{
	unsigned long f;

	f = *flags;
	spin_unlock_irqrestore(lock, f);
}
EXPORT_SYMBOL(ts_spin_unlock_irqrestore);


void ts_raw_spin_lock_irqsave(raw_spinlock_t *lock, unsigned long *flags)
{
	raw_spin_lock_irqsave(lock, *flags);
}
EXPORT_SYMBOL(ts_raw_spin_lock_irqsave);

void ts_raw_spin_unlock_irqrestore(raw_spinlock_t *lock, unsigned long *flags)
{
	raw_spin_unlock_irqrestore(lock, *flags);
}
EXPORT_SYMBOL(ts_raw_spin_unlock_irqrestore);


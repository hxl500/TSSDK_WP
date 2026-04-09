/***************************************************************************
 *                      COPYRIGHT NOTICE
 *               Copyright 2022 Tsingmicro, Inc.
 *                     All rights reserved.
 ***************************************************************************/

#ifndef __TS_WRAP_SPINLOCK_H__
#define __TS_WRAP_SPINLOCK_H__

#include <linux/spinlock.h>

spinlock_t *ts_alloc_st_spinlock(void);
size_t ts_get_st_spinlock_size(void);

void ts_spin_lock_init(spinlock_t *lock);
void ts_spin_lock(spinlock_t *lock);
int ts_spin_trylock(spinlock_t *lock);
void ts_spin_unlock(spinlock_t *lock);

/*
 * attention: we can not use #define, so flag type is pointer
 */
void ts_spin_lock_irqsave(spinlock_t *lock, unsigned long *flags);
void ts_spin_unlock_irqrestore(spinlock_t *lock, unsigned long *flags);

void ts_raw_spin_lock_irqsave(raw_spinlock_t *lock, unsigned long *flags);
void ts_raw_spin_unlock_irqrestore(raw_spinlock_t *lock, unsigned long *flags);


#endif


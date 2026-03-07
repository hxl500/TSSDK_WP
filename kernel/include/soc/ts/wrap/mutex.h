/***************************************************************************
 *                      COPYRIGHT NOTICE
 *               Copyright 2022 Tsingmicro, Inc.
 *                     All rights reserved.
 ***************************************************************************/

#ifndef __TS_WRAP_MUTEX_H__
#define __TS_WRAP_MUTEX_H__

#include <linux/mutex.h>

struct mutex *ts_alloc_st_mutex(void);
size_t ts_get_st_mutex_size(void);

void ts_mutex_init(struct mutex *lock);
void ts_mutex_lock(struct mutex *lock);
void ts_mutex_unlock(struct mutex *lock);
int ts_mutex_lock_interruptible(struct mutex *lock);
void ts_mutex_destroy(struct mutex *lock);

#endif


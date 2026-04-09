/***************************************************************************
 *                      COPYRIGHT NOTICE
 *               Copyright 2022 Tsingmicro, Inc.
 *                     All rights reserved.
 ***************************************************************************/

#ifndef __TS_WRAP_SEMAPHORE_H__
#define __TS_WRAP_SEMAPHORE_H__

#include <linux/semaphore.h>

struct semaphore *ts_alloc_st_semaphore(void);
void ts_sema_init(struct semaphore *sem, int val);


#endif


/***************************************************************************
 *                      COPYRIGHT NOTICE
 *               Copyright 2022 Tsingmicro, Inc.
 *                     All rights reserved.
 ***************************************************************************/

#ifndef __TS_WRAP_KTHREAD_H__
#define __TS_WRAP_KTHREAD_H__

#include <linux/kthread.h>

struct kthread_worker *ts_alloc_st_kthread_worker(void);


#endif


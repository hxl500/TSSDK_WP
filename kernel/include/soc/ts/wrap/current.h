/***************************************************************************
 *                      COPYRIGHT NOTICE
 *               Copyright 2022 Tsingmicro, Inc.
 *                     All rights reserved.
 ***************************************************************************/

#ifndef __TS_WRAP_CURRENT_H__
#define __TS_WRAP_CURRENT_H__

#include <linux/sched.h>
#include <linux/mm_types.h>
#include <linux/rwsem.h>


pid_t ts_current_get_pid(void);
struct mm_struct *ts_current_get_mm(void);
struct rw_semaphore *ts_current_get_mm_mmap_sem(void);
pid_t ts_current_get_tgid(void);
#endif


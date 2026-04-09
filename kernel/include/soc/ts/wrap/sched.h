/***************************************************************************
 *                      COPYRIGHT NOTICE
 *               Copyright 2022 Tsingmicro, Inc.
 *                     All rights reserved.
 ***************************************************************************/

#ifndef __TS_WRAP_SCHED_H__
#define __TS_WRAP_SCHED_H__

#include <linux/sched.h>

void ts__set_current_state(volatile long state_value);
char *ts_task_struct_get_comm(struct task_struct *task);
int ts_task_struct_get_comm_sz(struct task_struct *task);



#endif


/***************************************************************************
 *                      COPYRIGHT NOTICE
 *               Copyright 2022 Tsingmicro, Inc.
 *                     All rights reserved.
 ***************************************************************************/

#ifndef __TS_WRAP_WORKQUEUE_H__
#define __TS_WRAP_WORKQUEUE_H__

#include <linux/workqueue.h>

wait_queue_head_t *ts_alloc_st_wq_head(void);

void ts_INIT_WORK(struct work_struct *work, work_func_t func);
void ts_INIT_DELAYED_WORK(struct delayed_work *work, work_func_t func);


bool ts_queue_work(struct workqueue_struct *wq, struct work_struct *work);
bool ts_queue_delayed_work(struct workqueue_struct *wq,
				      struct delayed_work *dwork,
				      unsigned long delay);
bool ts_mod_delayed_work(struct workqueue_struct *wq,
				    struct delayed_work *dwork,
				    unsigned long delay);
bool ts_schedule_work(struct work_struct *work);
bool ts_schedule_delayed_work(struct delayed_work *dwork,
					 unsigned long delay);

struct workqueue_struct *ts_create_singlethread_workqueue(char *name);

#endif


/***************************************************************************
 *                      COPYRIGHT NOTICE
 *               Copyright 2022 Tsingmicro, Inc.
 *                     All rights reserved.
 ***************************************************************************/

#ifndef __TS_WRAP_WAIT_H__
#define __TS_WRAP_WAIT_H__

#include <linux/wait.h>
#include <linux/list.h>
#include <linux/stddef.h>
#include <linux/spinlock.h>

#include <asm/current.h>
#include <uapi/linux/wait.h>


wait_queue_head_t *ts_alloc_wait_queue_head(void);
void ts_free_wait_queue_head(wait_queue_head_t *wq_head);
void ts_init_waitqueue_head(wait_queue_head_t *wq_head);
/* alloc and initial */
wait_queue_head_t *ts_DECLARE_WAIT_QUEUE_HEAD(void);


/*
 * wq_head: global var, not point
 * might_sleep(); influenced by CONFIG_, not must exist
 */
#define ts_wait_event_interruptible(wq_head, condition)				\
({										\
	int __ret = 0;								\
	if (!(condition))							\
		__ret = __wait_event_interruptible(wq_head, condition);		\
	__ret;									\
})

#define ts_wait_event_timeout(wq_head, condition, timeout)				\
({										\
	long __ret = timeout;							\
	if (!___wait_cond_timeout(condition))					\
		__ret = __wait_event_timeout(wq_head, condition, timeout);	\
	__ret;									\
})

#define ts_wait_event_interruptible_timeout(wq_head, condition, timeout)				\
({										\
	long __ret = timeout;							\
	if (!___wait_cond_timeout(condition))					\
		__ret = __wait_event_interruptible_timeout(wq_head, condition, timeout);	\
	__ret;									\
})

#define ts_wait_event_interruptible_hrtimeout(wq_head, condition, timeout)				\
({										\
	long __ret = 0;							\
	if (!(condition))						\
		__ret = __wait_event__hrtimeout(wq_head, condition, timeout,TASK_INTERRUPTIBLE);	\
	__ret;									\
})

#endif

/***************************************************************************
 *                      COPYRIGHT NOTICE
 *               Copyright 2022 Tsingmicro, Inc.
 *                     All rights reserved.
 ***************************************************************************/

#ifndef __TS_WRAP_RCU_H__
#define __TS_WRAP_RCU_H__

void ts_rcu_read_lock(void);
void ts_rcu_read_unlock(void);
void ts_list_del_rcu(struct list_head *entry);
void ts_list_add_tail_rcu(struct list_head *new, struct list_head *head);

#endif


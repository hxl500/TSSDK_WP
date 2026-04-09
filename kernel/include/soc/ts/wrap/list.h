/***************************************************************************
 *                      COPYRIGHT NOTICE
 *               Copyright 2022 Tsingmicro, Inc.
 *                     All rights reserved.
 ***************************************************************************/

#ifndef __TS_WRAP_LIST_H__
#define __TS_WRAP_LIST_H__

#include <linux/list.h>

void ts_INIT_LIST_HEAD(struct list_head *list);

void ts_list_add(struct list_head *new, struct list_head *head);
void ts_list_add_tail(struct list_head *new, struct list_head *head);
void ts_list_del(struct list_head *entry);
void ts_list_del_init(struct list_head *entry);
int ts_list_empty(const struct list_head *head);
int ts_hlist_empty(const struct hlist_head *head);



#endif


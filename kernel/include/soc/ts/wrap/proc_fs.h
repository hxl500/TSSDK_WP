/***************************************************************************
 *                      COPYRIGHT NOTICE
 *               Copyright 2022 Tsingmicro, Inc.
 *                     All rights reserved.
 ***************************************************************************/

#ifndef __TS_WRAP_PROC_FS_H__
#define __TS_WRAP_PROC_FS_H__

#include <linux/proc_fs.h>

struct proc_dir_entry *ts_proc_mkdir(const char *dir, struct proc_dir_entry *entry);

struct proc_dir_entry *ts_proc_create(const char *name, umode_t mode, struct proc_dir_entry *parent, const struct file_operations *proc_fops);

void ts_proc_remove(struct proc_dir_entry *entry);

void ts_remove_proc_entry(const char *dir, struct proc_dir_entry *entry);

#endif


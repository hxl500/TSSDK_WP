/***************************************************************************
 *                      COPYRIGHT NOTICE
 *               Copyright 2022 Tsingmicro, Inc.
 *                     All rights reserved.
 ***************************************************************************/

#ifndef __TS_WRAP_FILE_H__
#define __TS_WRAP_FILE_H__

#include <linux/fs.h>

void ts_file_set_priv(struct file *filp, void *priv);
void *ts_file_get_priv(struct file *filp);
fmode_t ts_file_get_fmode(struct file *filp);
loff_t *ts_file_get_f_pos_ptr(struct file *filp);

#endif


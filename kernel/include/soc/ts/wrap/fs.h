/***************************************************************************
 *                      COPYRIGHT NOTICE
 *               Copyright 2022 Tsingmicro, Inc.
 *                     All rights reserved.
 ***************************************************************************/

#ifndef __TS_WRAP_FS_H__
#define __TS_WRAP_FS_H__

#include <linux/fs.h>

void *ts_inode_get_priv(struct inode *inode);
void *ts_file_get_private_data(struct file *file);
void ts_file_set_private_data(struct file *file, void *priv_data);


#endif


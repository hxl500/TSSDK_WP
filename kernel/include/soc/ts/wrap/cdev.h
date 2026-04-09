/***************************************************************************
 *                      COPYRIGHT NOTICE
 *               Copyright 2022 Tsingmicro, Inc.
 *                     All rights reserved.
 ***************************************************************************/

#ifndef __TS_WRAP_CDEV_H__
#define __TS_WRAP_CDEV_H__

#include <linux/cdev.h>

struct cdev *ts_alloc_st_cdev(void);

void ts_cdev_set_owner(struct cdev *cdev, struct module *owner);



#endif


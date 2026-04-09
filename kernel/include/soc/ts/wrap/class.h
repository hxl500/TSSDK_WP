/***************************************************************************
 *                      COPYRIGHT NOTICE
 *               Copyright 2022 Tsingmicro, Inc.
 *                     All rights reserved.
 ***************************************************************************/

#ifndef __TS_WRAP_CLASS_H__
#define __TS_WRAP_CLASS_H__

#include <linux/device.h>

struct class *ts_class_create(struct module *owner, const char *name);

#endif


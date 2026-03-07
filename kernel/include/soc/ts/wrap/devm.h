/***************************************************************************
 *                      COPYRIGHT NOTICE
 *               Copyright 2022 Tsingmicro, Inc.
 *                     All rights reserved.
 ***************************************************************************/

#ifndef __TS_WRAP_DEVM_H__
#define __TS_WRAP_DEVM_H__

#include <linux/reset.h>

void *ts_devm_kzalloc(struct device *dev, size_t size, gfp_t gfp);
void ts_devm_kfree(struct device *dev, void *name);

#endif


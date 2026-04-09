/***************************************************************************
 *                      COPYRIGHT NOTICE
 *               Copyright 2022 Tsingmicro, Inc.
 *                     All rights reserved.
 ***************************************************************************/

#ifndef __TS_WRAP_DEVICE_H__
#define __TS_WRAP_DEVICE_H__

#include <linux/device.h>

void ts_dev_set_drvdata(struct device *dev, void *data);
void *ts_dev_get_drvdata(const struct device *dev);



#endif


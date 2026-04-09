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
void *ts_dev_get_platdata(const struct device *dev);
struct device_node *ts_of_dev_get_node_dev(struct device *dev);
const char *ts_dev_name(const struct device *dev);
void ts_dev_set_dma_mask_ptr(struct device *dev, void *dma_mask);

#endif


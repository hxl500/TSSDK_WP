/***************************************************************************
 *                      COPYRIGHT NOTICE
 *               Copyright 2022 Tsingmicro, Inc.
 *                     All rights reserved.
 ***************************************************************************/

#ifndef __TS_WRAP_OF_DEVICE_H__
#define __TS_WRAP_OF_DEVICE_H__

#include <linux/of_device.h>

int ts_of_dma_configure(struct device *dev, struct device_node *np);

#endif


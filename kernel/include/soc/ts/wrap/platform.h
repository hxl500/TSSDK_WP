/***************************************************************************
 *                      COPYRIGHT NOTICE
 *               Copyright 2022 Tsingmicro, Inc.
 *                     All rights reserved.
 ***************************************************************************/

#ifndef __TS_WRAP_PLATFORM_H__
#define __TS_WRAP_PLATFORM_H__

#include <linux/platform_device.h>


void *ts_platform_get_dev_pm_domain(const struct platform_device *pdev);


struct device_node *ts_of_get_dev_node_plat(struct platform_device *pdev);

void *ts_platform_get_drvdata(const struct platform_device *pdev);
void ts_platform_set_drvdata(struct platform_device *pdev, void *data);


#endif


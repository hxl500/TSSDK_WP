/***************************************************************************
 *                      COPYRIGHT NOTICE
 *               Copyright 2022 Tsingmicro, Inc.
 *                     All rights reserved.
 ***************************************************************************/

#ifndef __TS_WRAP_RESET_H__
#define __TS_WRAP_RESET_H__

#include <linux/reset.h>

struct reset_control *ts_devm_reset_control_get(
				struct device *dev, const char *id);

struct reset_control *ts_devm_reset_control_get_optional(
				 struct device *dev, const char *id);

struct reset_control *ts_devm_reset_control_get_optional_exclusive(
				 struct device *dev, const char *id);


#endif


/***************************************************************************
 *                      COPYRIGHT NOTICE
 *               Copyright 2022 Tsingmicro, Inc.
 *                     All rights reserved.
 ***************************************************************************/

#ifndef __TS_WRAP_I2C_H__
#define __TS_WRAP_I2C_H__

#include <linux/i2c.h>

struct device_node *ts_of_get_dev_node_i2c_c(struct i2c_client *client);

void *ts_i2c_get_clientdata(const struct i2c_client *dev);


#endif


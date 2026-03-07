// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2022-2022 Tsingmicro, Inc.
 *
 * I2C access driver for injoinic ip6103 PMIC
 * Based on the TPS65912 driver
 */

#ifndef __LINUX_MFD_IP6103_H
#define __LINUX_MFD_IP6103_H

#include <linux/device.h>
#include <linux/regmap.h>

/* List of registers for IP6103 */

#define IP6103_DCDC0_VOLT_MASK		0x7f
#define IP6103_DCDC123_VOLT_MASK	0x7f
#define IP6103_DCDC_CTL_REG			0x20

#define IP6103_DCDC0_OP				0x21
#define IP6103_DCDC1_OP				0x28
#define IP6103_DCDC2_OP				0x2f
#define IP6103_DCDC3_OP				0x36


#define IP6103_LDO_VOLT_MASK		0x7f
#define IP6103_LDO_CTL_REG			0x41

#define IP6103_LDO0_OP				0x42
#define IP6103_LDO1_OP				0x44
#define IP6103_LDO2_OP				0x42
#define IP6103_LDO4_OP				0x4a
#define IP6103_LDO5_OP				0x4c
#define IP6103_LDO6_OP				0x4e


/* GPIO Register field definitions */
#define IP6103_REG_GPIO_0		0x83
#define IP6103_REG_GPIO_1		0x83
#define IP6103_REG_GPIO_2		0x82

/*
 * struct ip6103 - state holder for the ip6103 driver
 *
 * Device data may be used to access the IP6103 chip
 */
struct ip6103 {
	struct device *dev;
	struct regmap *regmap;
};

struct ip6103_gpio_reg_cfg {
	u8 reg;
	u8 oen; //output en
	u8 ien; // intput en
	u8 data;
};

#endif /*  __LINUX_MFD_IP6103_H */

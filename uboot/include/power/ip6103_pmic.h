// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2022-2022 Tsingmicro, Inc.
 */

#ifndef __IP6103_H_
#define __IP6103_H_

#include <power/pmic.h>

/* I2C device address for pmic ip6103 */
#define IP6103_DEFAULT_I2C_ADDR_VAL 0x60
#define IP6103_DEFAULT_I2C_ADDR	(IP6103_DEFAULT_I2C_ADDR_VAL >> 1)
#define IP6103_NUM_OF_REGS 0xFF
#define IP6103_LDO_NUM	7
#define IP6103_DCDC_NUM	4

#define IP6103_LDO0_REG     0x42
#define IP6103_LDOEN_REG    0x41
#define IP6103_DCDC_CTL_REG 0x20
#define IP6103_DCDC0_REG    0x21
#define IP6103_DCDC2_REG    0x2f
#define IP6103_LDO2_REG     0x46
#define IP6103_READ_I2C_ADDR_REG 0xfe
/* Drivers name */
#define IP6103_LDO_DRIVER	"ip6103_ldo"
#define IP6103_DCDC_DRIVER	"ip6103_dcdc"

enum {
	OPMODE_OFF = 0,
	OPMODE_ON,
};

#ifdef CONFIG_POWER
int ip6103_set_ldo_voltage(struct pmic *p, int ldo, ulong uV);
int ip6103_set_ldo_mode(struct pmic *p, int ldo, char opmode);
int ip6103_set_dcdc_voltage(struct pmic *p, int dcdc, ulong uV);
int ip6103_set_dcdc_mode(struct pmic *p, int dcdc, char opmode);
#endif

#define IP6103_LDO_VOLT_MAX_HEX		0x6c
#define IP6103_LDO_VOLT_MASK		0x7f
#define IP6103_LDO_MODE_MASK		0x7f
#define IP6103_LDO_MODE_OFF		(0x00 << 0x06)
#define IP6103_LDO_MODE_LPM		(0x01 << 0x06)
#define IP6103_LDO_MODE_STANDBY	(0x01 << 0x06)
#define IP6103_LDO_MODE_STANDBY_LPM	(0x02 << 0x06)
#define IP6103_LDO_MODE_ON		(0x03 << 0x06)
#define IP6103_DCDC012_VOLT_MAX_HEX	0x7f
#define IP6103_DCDC012_VOLT_MASK	0x7f
#define IP6103_DCDC3_VOLT_MAX_HEX	0x68
#define IP6103_DCDC3_VOLT_MASK		0x7e    //reg 0x36 bit[6:1]
#define IP6103_DCDC_MODE_MASK		0xf
#define IP6103_DCDC_MODE_SHIFT_1	0x00
#define IP6103_DCDC_MODE_SHIFT_2	0x04
#define IP6103_DCDC_MODE_OFF		0x00
#define IP6103_DCDC_MODE_STANDBY	0x01
#define IP6103_DCDC_MODE_LPM		0x02
#define IP6103_DCDC_MODE_ON		0x03

/* For regulator hex<->volt conversion */
#define IP6103_LDO_UV_LMIN		700000 /* Lower minimum LDO uV value */
#define IP6103_LDO_UV_HMIN		2600000 /* Higher minimum LDO uV value */
#define IP6103_LDO_UV_LSTEP		25000 /* uV lower value step */
#define IP6103_LDO_UV_HSTEP		100000 /* uV higher value step */
#define IP6103_DCDC_UV_LMIN		600000 /* Lower minimun DCDC value */
#define IP6103_DCDC_UV_HMIN		2200000 /* Higher minimun DCDC value */
#define IP6103_DCDC_UV_STEP		12500  /* uV value step */

/* DCDC0 0.9 volt value */
#define IP6103_DCDC0_0_9V	0x18
/* DCDC2 0.9 volt value */
#define IP6103_DCDC2_0_9V	0x18
/* LDO2 1.8 volt value */
#define IP6103_LDO2_1_8V	0x2c
#endif /* __IP6103_PMIC_H_ */

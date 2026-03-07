// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2022-2022 Tsingmicro, Inc.
 */

#include <common.h>
#include <fdtdec.h>
#include <errno.h>
#include <dm.h>
#include <i2c.h>
#include <power/pmic.h>
#include <power/regulator.h>
#include <power/ip6103_pmic.h>
#include <log.h>

#define MODE(_id, _val, _name) { \
	.id = _id, \
	.register_value = _val, \
	.name = _name, \
}

static struct dm_regulator_mode ip6103_ldo_mode_onoff[] = {
	MODE(OPMODE_OFF, IP6103_LDO_MODE_OFF, "OFF"),
	MODE(OPMODE_ON, IP6103_LDO_MODE_ON, "ON"),
};

static struct dm_regulator_mode ip6103_dcdc_mode_onoff[] = {
	MODE(OPMODE_OFF, IP6103_DCDC_MODE_OFF, "OFF"),
	MODE(OPMODE_ON, IP6103_DCDC_MODE_ON, "ON"),
};

static const char ip6103_dcdc_out[] = {
	0x21, 0x28, 0x2f, 0x36
};

static int ip6103_dcdc_volt2hex(int dcdc, int uV)
{
	int hex = 0;
	int hex_max = 0;

	switch (dcdc) {
	case 0:
	case 1:
	case 2:
		/* hex = (uV - 600000) / 12500; */
		hex = (uV - IP6103_DCDC_UV_LMIN) / IP6103_DCDC_UV_STEP;
		hex_max = IP6103_DCDC012_VOLT_MAX_HEX;
		break;
	default:
		/* dcdc3
		 * hex = (uV - 2200000) / 12500.
		 */
		hex = (uV - IP6103_DCDC_UV_HMIN) / IP6103_DCDC_UV_STEP;
		hex_max = IP6103_DCDC3_VOLT_MAX_HEX;
		break;
	}

	if (hex >= 0 && hex <= hex_max)
		return hex;

	pr_err("Value: %d uV is wrong for DCDC%d", uV, dcdc);
	return -EINVAL;
}

static int ip6103_dcdc_hex2volt(int dcdc, int hex)
{
	unsigned uV = 0;
	int hex_max = 0;

	if (hex < 0)
		goto bad_hex;

	switch (dcdc) {
	case 0:
	case 1:
	case 2:
		hex_max = IP6103_DCDC012_VOLT_MAX_HEX;
		if (hex > hex_max)
			goto bad_hex;

		/* uV = hex * 12500 + 600000; */
		uV = hex * IP6103_DCDC_UV_STEP + IP6103_DCDC_UV_LMIN;
		break;
	default:
		hex_max = IP6103_DCDC3_VOLT_MAX_HEX;
		if (hex > hex_max)
			goto bad_hex;
		/* dcdc3 */
		/* uV = hex * 12500 + 2200000; */
		uV = hex * IP6103_DCDC_UV_STEP + IP6103_DCDC_UV_HMIN;
		break;
	}

	return uV;

bad_hex:
	pr_err("Value: %#x is wrong for DCDC%d", hex, dcdc);
	return -EINVAL;
}

static int ip6103_ldo_volt2hex(int ldo, int uV)
{
	int hex = 0;

	switch (ldo) {
	case 0:
	case 1:
	case 2:
	case 4:
	case 5:
	case 6:
		hex = (uV - IP6103_LDO_UV_LMIN) / IP6103_LDO_UV_LSTEP;
		/* hex = (uV - 700000) / 25000; */
		break;
	default:
		hex = (uV - IP6103_LDO_UV_HMIN) / IP6103_LDO_UV_HSTEP;
		/* hex = (uV - 2600000) / 100000; */
	}

	if (hex >= 0 && hex <= IP6103_LDO_VOLT_MAX_HEX)
		return hex;

	pr_err("Value: %d uV is wrong for LDO%d", uV, ldo);
	return -EINVAL;
}

static int ip6103_ldo_hex2volt(int ldo, int hex)
{
	unsigned int uV = 0;

	if (hex > IP6103_LDO_VOLT_MAX_HEX)
		goto bad_hex;

	switch (ldo) {
	case 0:
	case 1:
	case 2:
	case 4:
	case 5:
	case 6:
		/* uV = hex * 25000 + 700000; */
		uV = hex * IP6103_LDO_UV_LSTEP + IP6103_LDO_UV_LMIN;
		break;
	default:
		/* uV = hex * 100000 + 2600000; */
		uV = hex * IP6103_LDO_UV_HSTEP + IP6103_LDO_UV_HMIN;
	}

	return uV;

bad_hex:
	pr_err("Value: %#x is wrong for ldo%d", hex, ldo);
	return -EINVAL;
}

static int ip6103_ldo_hex2mode(int ldo, int hex)
{
	if (hex > IP6103_LDO_MODE_MASK)
		return -EINVAL;

	if ((ldo >= 0) && (ldo < IP6103_LDO_NUM)) {
		uint8_t mode = hex & (1 << ldo);
		if (mode) {
			return OPMODE_ON;
		} else {
			return OPMODE_OFF;
		}
	}
	return -EINVAL;
}

static int ip6103_dcdc_hex2mode(int dcdc, int hex)
{
	if (hex > IP6103_DCDC_MODE_MASK)
		return -EINVAL;

	if ((dcdc >= 0) && (dcdc < IP6103_DCDC_NUM)) {
		uint8_t mode = hex & (1 << dcdc);

		if (mode) {
			return OPMODE_ON;
		} else {
			return OPMODE_OFF;
		}
	}
	return -EINVAL;
}

static int ip6103_dcdc_modes(int dcdc, struct dm_regulator_mode **modesp)
{
	int ret = -EINVAL;

	if (dcdc >= IP6103_DCDC_NUM)
		return ret;

	*modesp = ip6103_dcdc_mode_onoff;
	ret = ARRAY_SIZE(ip6103_dcdc_mode_onoff);

	return ret;
}

static int ip6103_ldo_modes(int ldo, struct dm_regulator_mode **modesp,
				struct udevice *dev)
{
	int ret = -EINVAL;

	if (ldo >= IP6103_LDO_NUM)
		return ret;

	*modesp = ip6103_ldo_mode_onoff;
	ret = ARRAY_SIZE(ip6103_ldo_mode_onoff);

	return ret;
}

static int ip6103_ldo_val(struct udevice *dev, int op, int *uV)
{
	unsigned int adr;
	unsigned char val;
	int hex, ldo, ret;

	if (op == PMIC_OP_GET)
		*uV = 0;

	ldo = dev->driver_data;
	if (ldo < 0 || ldo >= IP6103_LDO_NUM) {
		pr_err("Wrong ldo number: %d", ldo);
		return -EINVAL;
	}

	adr = IP6103_LDO0_REG + ldo * 2;

	ret = pmic_read(dev->parent, adr, &val, 1);
	if (ret)
		return ret;

	if (op == PMIC_OP_GET) {
		val &= IP6103_LDO_VOLT_MASK;
		ret = ip6103_ldo_hex2volt(ldo, val);
		if (ret < 0)
			return ret;
		*uV = ret;
		return 0;
	}

	hex = ip6103_ldo_volt2hex(ldo, *uV);
	if (hex < 0)
		return hex;

	val &= ~IP6103_LDO_VOLT_MASK;
	val |= hex;
	ret = pmic_write(dev->parent, adr, &val, 1);

	return ret;
}

static int ip6103_dcdc_val(struct udevice *dev, int op, int *uV)
{
	unsigned int mask, adr;
	unsigned char val;
	int hex, dcdc, ret;

	dcdc = dev->driver_data;
	if (dcdc < 0 || dcdc >= IP6103_DCDC_NUM) {
		pr_err("Wrong dcdc number: %d", dcdc);
		return -EINVAL;
	}

	if (op == PMIC_OP_GET)
		*uV = 0;

	adr = ip6103_dcdc_out[dcdc];

	/* mask */
	switch (dcdc) {
	case 0:
	case 1:
	case 2:
		mask = IP6103_DCDC012_VOLT_MASK;
		break;
	default:
		mask = IP6103_DCDC3_VOLT_MASK;
		break;
	}

	ret = pmic_read(dev->parent, adr, &val, 1);
	if (ret)
		return ret;

	if (op == PMIC_OP_GET) {
		val &= mask;
		ret = ip6103_dcdc_hex2volt(dcdc, val);
		if (ret < 0)
			return ret;
		*uV = ret;
		return 0;
	}

	hex = ip6103_dcdc_volt2hex(dcdc, *uV);
	if (hex < 0)
		return hex;

	val &= ~mask;
	val |= hex;
	ret = pmic_write(dev->parent, adr, &val, 1);

	return ret;
}

static int ip6103_ldo_mode(struct udevice *dev, int op, int *opmode)
{
	unsigned int adr, mode;
	unsigned char val;
	int ldo, ret;

	if (op == PMIC_OP_GET)
		*opmode = -EINVAL;

	ldo = dev->driver_data;
	if (ldo < 0 || ldo >= IP6103_LDO_NUM) {
		pr_err("Wrong ldo number: %d", ldo);
		return -EINVAL;
	}

	adr = IP6103_LDOEN_REG;

	ret = pmic_read(dev->parent, adr, &val, 1);
	if (ret)
		return ret;

	if (op == PMIC_OP_GET) {
		ret = ip6103_ldo_hex2mode(ldo, val & IP6103_LDO_MODE_MASK);
		if (ret < 0)
			return ret;
		*opmode = ret;
		return 0;
	}

	/* mode */
	switch (*opmode) {
	case OPMODE_OFF:
		mode &= ~(1 << ldo);
		break;
	case OPMODE_ON:
		mode |= (1 << ldo);
		break;
	default:
		mode = 0xff;
	}

	if (mode == 0xff) {
		pr_err("Wrong mode: %d for ldo%d", *opmode, ldo);
		return -EINVAL;
	}

	val &= ~(1 << ldo);
	val |= mode;
	ret = pmic_write(dev->parent, adr, &val, 1);

	return ret;
}

static int ip6103_ldo_enable(struct udevice *dev, int op, bool *enable)
{
	int ret, on_off;

	if (op == PMIC_OP_GET) {
		ret = ip6103_ldo_mode(dev, op, &on_off);
		if (ret)
			return ret;

		switch (on_off) {
		case OPMODE_OFF:
			*enable = false;
			break;
		case OPMODE_ON:
			*enable = true;
			break;
		default:
			return -EINVAL;
		}
	} else if (op == PMIC_OP_SET) {
		if (*enable)
			on_off = OPMODE_ON;
		else
			on_off = OPMODE_OFF;

		ret = ip6103_ldo_mode(dev, op, &on_off);
		if (ret)
			return ret;
	}

	return 0;
}

static int ip6103_dcdc_mode(struct udevice *dev, int op, int *opmode)
{
	unsigned int adr, mode;
	unsigned char val;
	int dcdc, ret;

	dcdc = dev->driver_data;
	if (dcdc < 0 || dcdc >= IP6103_DCDC_NUM) {
		pr_err("Wrong dcdc number: %d", dcdc);
		return -EINVAL;
	}

	adr = IP6103_DCDC_CTL_REG;

	ret = pmic_read(dev->parent, adr, &val, 1);
	if (ret)
		return ret;

	if (op == PMIC_OP_GET) {
		ret = ip6103_dcdc_hex2mode(dcdc, val & IP6103_DCDC_MODE_MASK);
		if (ret < 0)
			return ret;
		*opmode = ret;
		return 0;
	}

	/* mode */
	switch (*opmode) {
	case OPMODE_OFF:
		mode &= ~(1 << dcdc);
		break;
	case OPMODE_ON:
		mode |= (1 << dcdc);
		break;
	default:
		mode = 0xff;
	}

	if (mode == 0xff) {
		pr_err("Wrong mode: %d for dcdc: %d\n", *opmode, dcdc);
		return -EINVAL;
	}

	val &= ~(1 << dcdc);
	val |= mode;
	ret = pmic_write(dev->parent, adr, &val, 1);

	return ret;
}

static int ip6103_dcdc_enable(struct udevice *dev, int op, bool *enable)
{
	int ret, on_off;

	if (op == PMIC_OP_GET) {
		ret = ip6103_dcdc_mode(dev, op, &on_off);
		if (ret)
			return ret;

		switch (on_off) {
		case OPMODE_OFF:
			*enable = false;
			break;
		case OPMODE_ON:
			*enable = true;
			break;
		default:
			return -EINVAL;
		}
	} else if (op == PMIC_OP_SET) {
		if (*enable)
			on_off = OPMODE_ON;
		else
			on_off = OPMODE_OFF;

		ret = ip6103_dcdc_mode(dev, op, &on_off);
		if (ret)
			return ret;
	}

	return 0;
}

static int ip6103_ldo_probe(struct udevice *dev)
{
	struct dm_regulator_uclass_platdata *uc_pdata;
	debug("%s: '%s'\n", __func__, dev->name);
	uc_pdata = dev_get_uclass_platdata(dev);

	uc_pdata->type = REGULATOR_TYPE_LDO;
	uc_pdata->mode_count = ip6103_ldo_modes(dev->driver_data,
						  &uc_pdata->mode, dev);

	return 0;
}

static int ldo_get_value(struct udevice *dev)
{
	int uV;
	int ret;

	ret = ip6103_ldo_val(dev, PMIC_OP_GET, &uV);
	if (ret)
		return ret;

	return uV;
}

static int ldo_set_value(struct udevice *dev, int uV)
{
	return ip6103_ldo_val(dev, PMIC_OP_SET, &uV);
}

static int ldo_get_enable(struct udevice *dev)
{
	bool enable = false;
	int ret;

	ret = ip6103_ldo_enable(dev, PMIC_OP_GET, &enable);
	if (ret)
		return ret;

	return enable;
}

static int ldo_set_enable(struct udevice *dev, bool enable)
{
	return ip6103_ldo_enable(dev, PMIC_OP_SET, &enable);
}

static int ldo_get_mode(struct udevice *dev)
{
	int mode;
	int ret;

	ret = ip6103_ldo_mode(dev, PMIC_OP_GET, &mode);
	if (ret)
		return ret;

	return mode;
}

static int ldo_set_mode(struct udevice *dev, int mode)
{
	return ip6103_ldo_mode(dev, PMIC_OP_SET, &mode);
}

static int ip6103_dcdc_probe(struct udevice *dev)
{
	struct dm_regulator_uclass_platdata *uc_pdata;
	debug("%s: '%s'\n", __func__, dev->name);
	uc_pdata = dev_get_uclass_platdata(dev);

	uc_pdata->type = REGULATOR_TYPE_OTHER;
	uc_pdata->mode_count = ip6103_dcdc_modes(dev->driver_data,
						   &uc_pdata->mode);

	return 0;
}

static int dcdc_get_value(struct udevice *dev)
{
	int uV;
	int ret;

	ret = ip6103_dcdc_val(dev, PMIC_OP_GET, &uV);
	if (ret)
		return ret;

	return uV;
}

static int dcdc_set_value(struct udevice *dev, int uV)
{
	return ip6103_dcdc_val(dev, PMIC_OP_SET, &uV);
}

static int dcdc_get_enable(struct udevice *dev)
{
	bool enable = false;
	int ret;

	ret = ip6103_dcdc_enable(dev, PMIC_OP_GET, &enable);
	if (ret)
		return ret;

	return enable;
}

static int dcdc_set_enable(struct udevice *dev, bool enable)
{
	return ip6103_dcdc_enable(dev, PMIC_OP_SET, &enable);
}

static int dcdc_get_mode(struct udevice *dev)
{
	int mode;
	int ret;

	ret = ip6103_dcdc_mode(dev, PMIC_OP_GET, &mode);
	if (ret)
		return ret;

	return mode;
}

static int dcdc_set_mode(struct udevice *dev, int mode)
{
	return ip6103_dcdc_mode(dev, PMIC_OP_SET, &mode);
}

static const struct dm_regulator_ops ip6103_ldo_ops = {
	.get_value  = ldo_get_value,
	.set_value  = ldo_set_value,
	.get_enable = ldo_get_enable,
	.set_enable = ldo_set_enable,
	.get_mode   = ldo_get_mode,
	.set_mode   = ldo_set_mode,
};

U_BOOT_DRIVER(ip6103_ldo) = {
	.name = IP6103_LDO_DRIVER,
	.id = UCLASS_REGULATOR,
	.ops = &ip6103_ldo_ops,
	.probe = ip6103_ldo_probe,
};

static const struct dm_regulator_ops ip6103_dcdc_ops = {
	.get_value  = dcdc_get_value,
	.set_value  = dcdc_set_value,
	.get_enable = dcdc_get_enable,
	.set_enable = dcdc_set_enable,
	.get_mode   = dcdc_get_mode,
	.set_mode   = dcdc_set_mode,
};

U_BOOT_DRIVER(ip6103_dcdc) = {
	.name = IP6103_DCDC_DRIVER,
	.id = UCLASS_REGULATOR,
	.ops = &ip6103_dcdc_ops,
	.probe = ip6103_dcdc_probe,
};

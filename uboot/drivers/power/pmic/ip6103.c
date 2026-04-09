// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2022-2022 Tsingmicro, Inc.
 */
// #define DEBUG
#include <common.h>
#include <fdtdec.h>
#include <errno.h>
#include <dm.h>
#include <i2c.h>
#include <log.h>
#include <power/pmic.h>
#include <power/regulator.h>
#include <power/ip6103_pmic.h>

static const struct pmic_child_info pmic_children_info[] = {
	{ .prefix = "LDO", .driver = IP6103_LDO_DRIVER },
	{ .prefix = "DCDC", .driver = IP6103_DCDC_DRIVER },
	{ },
};

static int ip6103_reg_count(struct udevice *dev)
{
	return IP6103_NUM_OF_REGS;
}

static int ip6103_write(struct udevice *dev, uint reg, const uint8_t *buff,
			  int len)
{
	debug("%s:reg:0x%x val:%x len:%d\n", __func__, reg, *buff, len);
	if (dm_i2c_write(dev, reg, buff, len)) {
		pr_err("write error to device: %p register: %#x!\n", dev, reg);
		return -EIO;
	}

	return 0;
}

static int ip6103_read(struct udevice *dev, uint reg, uint8_t *buff, int len)
{
	if (dm_i2c_read(dev, reg, buff, len)) {
		pr_err("read error from device: %p register: %#x!\n", dev, reg);
		return -EIO;
	}
	debug("%s:reg:0x%x val:%x len:%d\n", __func__, reg, *buff, len);
	return 0;
}

static int set_some_registers(struct udevice *dev)
{
	int ret;
	unsigned int adr;
	unsigned char val;

	val = IP6103_DCDC0_0_9V;
	ret = pmic_write(dev, IP6103_DCDC0_REG, &val, 1);
	if (ret)
		return ret;

	val = IP6103_DCDC2_0_9V;
	ret = pmic_write(dev, IP6103_DCDC2_REG, &val, 1);

	adr = IP6103_DCDC_CTL_REG;
	ret = pmic_read(dev, adr, &val, 1);
	if (ret)
		return ret;

	val |= (1 << 0) | (1 << 2);// enable dcdc0 and 2
	ret = pmic_write(dev, adr, &val, 1);

	val = IP6103_LDO2_1_8V;
	ret = pmic_write(dev, IP6103_LDO2_REG, &val, 1);

	adr = IP6103_LDOEN_REG;
	ret = pmic_read(dev, adr, &val, 1);
	val |= (1 << 2);// enable ldo2
	ret = pmic_write(dev, adr, &val, 1);

	adr = 0x83;
	val = 0x2c;//LOSC use gpio func,gpio 0 1 out high
	ret = pmic_write(dev, adr, &val, 1);

	adr = 0x82;
	val = 0x5;//use gpio 2 func,gpio 2 out high
	ret = pmic_write(dev, adr, &val, 1);

	if (ret)
		return ret;

	return 0;
}

static int ip6103_bind(struct udevice *dev)
{
	ofnode regulators_node;
	int children;
	// int ret;
	// ret = i2c_get_chip_for_busnum(0, IP6103_DEFAULT_I2C_ADDR, 1, &dev);
	// if (ret) {
	//	printf("%s: Cannot find udev for a bus\n", __func__);
	//	return -EINVAL;
	// }
	// set_some_registers();
	regulators_node = dev_read_subnode(dev, "voltage-regulators");
	if (!ofnode_valid(regulators_node)) {
		debug("%s: %s regulators subnode not found!\n", __func__,
			  dev->name);
		return -ENXIO;
	}

	debug("%s: '%s' - found regulators subnode\n", __func__, dev->name);

	children = pmic_bind_children(dev, regulators_node, pmic_children_info);
	if (!children)
		debug("%s: %s - no child found\n", __func__, dev->name);

	/* Always return success for this device */
	return 0;
}

static int ip6103_probe(struct udevice *dev)
{
	int ret;
	unsigned int adr;
	unsigned char val;

	debug("%s: '%s'\n", __func__, dev->name);
	adr = IP6103_READ_I2C_ADDR_REG;
	ret = pmic_read(dev, adr, &val, 1);
	if (ret)
		return ret;
	if (val != IP6103_DEFAULT_I2C_ADDR_VAL) {
		pr_err("error default i2c addr val :%x!", val);
		return -EINVAL;
	}

	ret = set_some_registers(dev);
	if (ret) {
		pr_err("error set_some_registers,ret:%d!", ret);
		return ret;
	}
	return 0;
}

static struct dm_pmic_ops ip6103_ops = {
	.reg_count = ip6103_reg_count,
	.read = ip6103_read,
	.write = ip6103_write,
};

static const struct udevice_id ip6103_ids[] = {
	{ .compatible = "injoinic,ip6103" },
	{ }
};

U_BOOT_DRIVER(pmic_ip6103) = {
	.name = "ip6103_pmic",
	.id = UCLASS_PMIC,
	.of_match = ip6103_ids,
	.bind = ip6103_bind,
	.probe = ip6103_probe,
	.ops = &ip6103_ops,
};

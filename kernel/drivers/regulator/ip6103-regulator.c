// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2022-2022 Tsingmicro, Inc.
 *
 * Regulator driver for injoinic ip6103 PMIC
 * Based on the IP6103 driver
 */
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/regulator/driver.h>
#include <linux/regulator/machine.h>
#include <linux/mfd/ip6103.h>
#include <linux/regulator/userspace-consumer.h>

enum ip6103_regulators { DCDC0, DCDC1, DCDC2, DCDC3, LDO0, LDO1, LDO2, LDO4,
	LDO5, LDO6};

#define IP6103_REGULATOR(_name, _id, _of_match, _ops, _n_v, _vr, _vm, \
	_er, _em, _lr) \
	[_id] = {							\
		.name			= _name,			\
		.of_match		= _of_match,			\
		.regulators_node	= "regulators",			\
		.id			= _id,				\
		.ops			= &_ops,			\
		.n_voltages		= _n_v,				\
		.type			= REGULATOR_VOLTAGE,		\
		.owner			= THIS_MODULE,			\
		.vsel_reg		= _vr,				\
		.vsel_mask		= _vm,				\
		.enable_reg		= _er,				\
		.enable_mask	= _em,			\
		.volt_table		= NULL,				\
		.linear_ranges		= _lr,				\
		.n_linear_ranges	= ARRAY_SIZE(_lr),		\
	}

static const struct regulator_linear_range ip6103_dcdc012_ranges[] = {
	REGULATOR_LINEAR_RANGE(600000, 0x0, 0x7f, 12500),
};

static const struct regulator_linear_range ip6103_dcdc3_ranges[] = {
	REGULATOR_LINEAR_RANGE(2200000, 0x0, 0x68, 12500),
};

static const struct regulator_linear_range ip6103_ldo_ranges[] = {
	REGULATOR_LINEAR_RANGE(700000, 0x0, 0x6c, 25000),
};

/* Operations permitted on DCDCx */
static struct regulator_ops ip6103_ops_dcdc = {
	.is_enabled		= regulator_is_enabled_regmap,
	.enable			= regulator_enable_regmap,
	.disable		= regulator_disable_regmap,
	.get_voltage_sel	= regulator_get_voltage_sel_regmap,
	.set_voltage_sel	= regulator_set_voltage_sel_regmap,
	.list_voltage		= regulator_list_voltage_linear_range,
};

/* Operations permitted on LDOx */
static struct regulator_ops ip6103_ops_ldo = {
	.is_enabled		= regulator_is_enabled_regmap,
	.enable			= regulator_enable_regmap,
	.disable		= regulator_disable_regmap,
	.get_voltage_sel	= regulator_get_voltage_sel_regmap,
	.set_voltage_sel	= regulator_set_voltage_sel_regmap,
	.list_voltage		= regulator_list_voltage_linear_range,
	.map_voltage		= regulator_map_voltage_linear_range,
};

static const struct regulator_desc regulators[] = {
	IP6103_REGULATOR("DCDC0", DCDC0, "DCDC0", ip6103_ops_dcdc, 0x7f,
			   IP6103_DCDC0_OP, IP6103_DCDC0_VOLT_MASK, IP6103_DCDC_CTL_REG,
			   BIT(0), ip6103_dcdc012_ranges),
	IP6103_REGULATOR("DCDC1", DCDC1, "DCDC1", ip6103_ops_dcdc, 0x7f,
			   IP6103_DCDC1_OP, IP6103_DCDC123_VOLT_MASK, IP6103_DCDC_CTL_REG,
			   BIT(1), ip6103_dcdc012_ranges),
	IP6103_REGULATOR("DCDC2", DCDC2, "DCDC2", ip6103_ops_dcdc, 0x7f,
			   IP6103_DCDC2_OP, IP6103_DCDC123_VOLT_MASK, IP6103_DCDC_CTL_REG,
			   BIT(2), ip6103_dcdc012_ranges),
	IP6103_REGULATOR("DCDC3", DCDC3, "DCDC3", ip6103_ops_dcdc, 0x7f,
			   IP6103_DCDC3_OP, IP6103_DCDC123_VOLT_MASK, IP6103_DCDC_CTL_REG,
			   BIT(3), ip6103_dcdc3_ranges),
	IP6103_REGULATOR("LDO0", LDO0, "LDO0", ip6103_ops_ldo, 0x6c,
			   IP6103_LDO0_OP, IP6103_LDO_VOLT_MASK, IP6103_LDO_CTL_REG,
			   BIT(0), ip6103_ldo_ranges),
	IP6103_REGULATOR("LDO1", LDO1, "LDO1", ip6103_ops_ldo, 0x6c,
			   IP6103_LDO0_OP, IP6103_LDO_VOLT_MASK, IP6103_LDO_CTL_REG,
			   BIT(1), ip6103_ldo_ranges),
	IP6103_REGULATOR("LDO2", LDO2, "LDO2", ip6103_ops_ldo, 0x6c,
			   IP6103_LDO2_OP, IP6103_LDO_VOLT_MASK, IP6103_LDO_CTL_REG,
			   BIT(2), ip6103_ldo_ranges),
	IP6103_REGULATOR("LDO4", LDO4, "LDO4", ip6103_ops_ldo, 0x6c,
			   IP6103_LDO4_OP, IP6103_LDO_VOLT_MASK, IP6103_LDO_CTL_REG,
			   BIT(4), ip6103_ldo_ranges),
	IP6103_REGULATOR("LDO5", LDO5, "LDO5", ip6103_ops_ldo, 0x6c,
			   IP6103_LDO5_OP, IP6103_LDO_VOLT_MASK, IP6103_LDO_CTL_REG,
			   BIT(5), ip6103_ldo_ranges),
	IP6103_REGULATOR("LDO6", LDO6, "LDO6", ip6103_ops_ldo, 0x6c,
			   IP6103_LDO6_OP, IP6103_LDO_VOLT_MASK, IP6103_LDO_CTL_REG,
			   BIT(6), ip6103_ldo_ranges),

};

#if IS_ENABLED(CONFIG_REGULATOR_USERSPACE_CONSUMER)

#define USERSPACE_CONSUMER(_name, _id) \
static struct regulator_bulk_data consumer_supply_##_name = { \
	.supply		= #_name, \
}; static struct regulator_userspace_consumer_data consumer_data_##_name = { \
	.name		= #_name, \
	.num_supplies	= 1, \
	.supplies	= &consumer_supply_##_name, \
}; static struct platform_device userspace_consumer_##_name = { \
	.name		= "reg-userspace-consumer", \
	.id		= _id, \
	.dev		= { \
		.platform_data = &consumer_data_##_name, \
	}, \
}

USERSPACE_CONSUMER(DCDC0, 0);
USERSPACE_CONSUMER(DCDC2, 1);
USERSPACE_CONSUMER(LDO2, 2);
USERSPACE_CONSUMER(LDO6, 3);

#define PLATFORM_DEVICE(_name, _id) \
	[_id] = &userspace_consumer_##_name

static struct platform_device *ip6103_userspace_consumers[] = {
	PLATFORM_DEVICE(DCDC0, 0),
	PLATFORM_DEVICE(DCDC2, 1),
	PLATFORM_DEVICE(LDO2, 2),
	PLATFORM_DEVICE(LDO6, 3),
};
#endif

#if IS_ENABLED(CONFIG_REGULATOR_VIRTUAL_CONSUMER)

#define VIRT_CONSUMER(_name, _id) \
static struct platform_device virt_consumer_##_name = { \
	.name = "reg-virt-consumer", \
	.id = _id, \
	.dev = { \
		.platform_data = #_name, /* Passed to driver */ \
	}, \
}

VIRT_CONSUMER(DCDC0, 0);
VIRT_CONSUMER(DCDC2, 1);
VIRT_CONSUMER(LDO2, 2);
VIRT_CONSUMER(LDO6, 3);

#undef PLATFORM_DEVICE
#define PLATFORM_DEVICE(_name, _id) \
	[_id] = &virt_consumer_##_name

static struct platform_device *ip6103_virt_consumer[] = {
	PLATFORM_DEVICE(DCDC0, 0),
	PLATFORM_DEVICE(DCDC2, 1),
	PLATFORM_DEVICE(LDO2, 2),
	PLATFORM_DEVICE(LDO6, 3),
};
#endif

static int ip6103_regulator_probe(struct platform_device *pdev)
{
	struct ip6103 *ip = dev_get_drvdata(pdev->dev.parent);
	struct regulator_config config = { };
	struct regulator_dev *rdev;
	int i, ret;

	platform_set_drvdata(pdev, ip);

	config.dev = &pdev->dev;
	config.driver_data = ip;
	config.dev->of_node = ip->dev->of_node;
	config.regmap = ip->regmap;

	for (i = 0; i < ARRAY_SIZE(regulators); i++) {
		rdev = devm_regulator_register(&pdev->dev, &regulators[i],
						   &config);
		if (IS_ERR(rdev)) {
			dev_err(ip->dev, "failed to register %s regulator\n",
				pdev->name);
			return PTR_ERR(rdev);
		}
	}
#if IS_ENABLED(CONFIG_REGULATOR_USERSPACE_CONSUMER)
	ret = platform_add_devices(ip6103_userspace_consumers,
		ARRAY_SIZE(ip6103_userspace_consumers));
	if (ret) {
		dev_err(ip->dev, "platform_add_devices error: %d\n", ret);
		return ret;
	}
#endif

#if IS_ENABLED(CONFIG_REGULATOR_VIRTUAL_CONSUMER)
	ret = platform_add_devices(ip6103_virt_consumer,
		ARRAY_SIZE(ip6103_virt_consumer));
	if (ret) {
		dev_err(ip->dev, "platform_add_devices error: %d\n", ret);
		return ret;
	}
#endif
	return 0;
}

static const struct platform_device_id ip6103_regulator_id_table[] = {
	{ "ip6103-regulator", },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(platform, ip6103_regulator_id_table);

static struct platform_driver ip6103_regulator_driver = {
	.driver = {
		.name = "ip6103-regulator",
	},
	.probe = ip6103_regulator_probe,
	.id_table = ip6103_regulator_id_table,
};
module_platform_driver(ip6103_regulator_driver);

MODULE_DESCRIPTION("IP6103 voltage regulator driver");
MODULE_LICENSE("GPL v2");

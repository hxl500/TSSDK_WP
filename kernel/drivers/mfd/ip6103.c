// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2022-2022 Tsingmicro, Inc.
 *
 * I2C access driver for injoinic ip6103 PMIC
 * Based on the TPS65912 driver
 */

#include <linux/i2c.h>
#include <linux/module.h>
#include <linux/regmap.h>
#include <linux/mfd/core.h>
#include <linux/mfd/ip6103.h>
#include <linux/of.h>

static const struct mfd_cell ip6103_cells[] = {
	{ .name = "ip6103-regulator", },
	{
		.name = "ip6103-gpio",
		.of_compatible = "injoinic,ip6103-gpio",
	},
};

static struct regmap_config ip6103_regmap_config = {
	.reg_bits = 8,
	.val_bits = 8,
	.cache_type = REGCACHE_RBTREE,
	.max_register	= 0xfe,
};

static int ip6103_i2c_probe(struct i2c_client *client,
				  const struct i2c_device_id *ids)
{
	struct ip6103 *ip;
	int ret;

	ip = devm_kzalloc(&client->dev, sizeof(*ip), GFP_KERNEL);
	if (!ip)
		return -ENOMEM;

	i2c_set_clientdata(client, ip);
	ip->dev = &client->dev;

	ip->regmap = devm_regmap_init_i2c(client, &ip6103_regmap_config);
	if (IS_ERR(ip->regmap)) {
		dev_err(ip->dev, "Failed to initialize register map\n");
		return PTR_ERR(ip->regmap);
	}
	ret = mfd_add_devices(ip->dev, PLATFORM_DEVID_AUTO, ip6103_cells,
				  ARRAY_SIZE(ip6103_cells), NULL, 0, NULL);
	if (ret) {
		dev_err(ip->dev, "Failed to add mfd devices\n");
		return ret;
	}

	return 0;
}

#ifdef CONFIG_OF
static const struct of_device_id ip6103_i2c_of_match_table[] = {
	{ .compatible = "injoinic,ip6103", },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, ip6103_i2c_of_match_table);
#endif

static const struct i2c_device_id ip6103_i2c_id_table[] = {
	{ "ip6103", 0 },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(i2c, ip6103_i2c_id_table);

static struct i2c_driver ip6103_i2c_driver = {
	.driver		= {
		.name	= "ip6103",
		.of_match_table = of_match_ptr(ip6103_i2c_of_match_table),
	},
	.probe		= ip6103_i2c_probe,
	.id_table       = ip6103_i2c_id_table,
};
module_i2c_driver(ip6103_i2c_driver);

MODULE_DESCRIPTION("IP6103 Interface Driver");
MODULE_LICENSE("GPL v2");

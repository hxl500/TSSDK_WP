// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2022-2022 Tsingmicro, Inc.
 *
 * gpio driver for injoinic ip6103 PMIC
 * Based on the lp3943 driver
 */

#include <linux/bitops.h>
#include <linux/err.h>
#include <linux/gpio.h>
#include <linux/gpio/driver.h>
#include <linux/i2c.h>
#include <linux/mfd/ip6103.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/regmap.h>

enum ip6103_gpios {
	IP6103_GPIO0,
	IP6103_GPIO1,
	IP6103_GPIO2,
	IP6103_MAX_GPIO,
};

struct ip6103_gpio {
	struct gpio_chip chip;
	struct ip6103 *ip6103;
};

#define REG_CFG(_index, _reg, _oen, _ien, _data) \
	[_index] = { \
		.reg = (_reg), \
		.oen = (_oen), \
		.ien = (_ien), \
		.data = (_data), \
	}

static const struct ip6103_gpio_reg_cfg gpio_reg_cfg[] = {
	REG_CFG(IP6103_GPIO0, IP6103_REG_GPIO_0, BIT(2), BIT(1), BIT(0)),
	REG_CFG(IP6103_GPIO1, IP6103_REG_GPIO_1, BIT(5), BIT(4), BIT(3)),
	REG_CFG(IP6103_GPIO2, IP6103_REG_GPIO_2, BIT(2), BIT(1), BIT(0)),
};

static int ip6103_gpio_direction_input(struct gpio_chip *chip, unsigned int offset)
{
	struct ip6103_gpio *ip6103_gpio = gpiochip_get_data(chip);
	struct ip6103 *ip6103 = ip6103_gpio->ip6103;
	const struct ip6103_gpio_reg_cfg *cfg = &gpio_reg_cfg[offset];
	int ret;
	unsigned int val;

	if (offset >= chip->ngpio)
		return -EINVAL;

	ret = regmap_read(ip6103->regmap, cfg->reg, &val);
	if (ret < 0) {
		dev_err(ip6103->dev, "regmap_read error: %d\n", ret);
		return ret;
	}

	val = (val & (~cfg->oen)) | cfg->ien;

	return regmap_write(ip6103->regmap, cfg->reg, val);
}

static int ip6103_gpio_direction_output(struct gpio_chip *chip, unsigned int offset,
					int value)
{
	struct ip6103_gpio *ip6103_gpio = gpiochip_get_data(chip);
	struct ip6103 *ip6103 = ip6103_gpio->ip6103;
	const struct ip6103_gpio_reg_cfg *cfg = &gpio_reg_cfg[offset];
	int ret;
	unsigned int val;

	if (offset >= chip->ngpio)
		return -EINVAL;

	ret = regmap_read(ip6103->regmap, cfg->reg, &val);
	if (ret < 0) {
		dev_err(ip6103->dev, "regmap_read error: %d\n", ret);
		return ret;
	}

	val = (val & (~cfg->ien)) | cfg->oen;

	return regmap_write(ip6103->regmap, cfg->reg, val);
}

static int ip6103_gpio_get_direction(struct gpio_chip *chip, unsigned int offset)
{
	struct ip6103_gpio *ip6103_gpio = gpiochip_get_data(chip);
	struct ip6103 *ip6103 = ip6103_gpio->ip6103;
	const struct ip6103_gpio_reg_cfg *cfg = &gpio_reg_cfg[offset];
	int ret;
	unsigned int val;

	if (offset >= chip->ngpio)
		return -EINVAL;

	ret = regmap_read(ip6103->regmap, cfg->reg, &val);

	if (ret < 0) {
		dev_err(ip6103->dev, "regmap_read error: %d\n", ret);
		return ret;
	}

	if (val & cfg->ien)
		return 1;

	return 0;

}

static int ip6103_gpio_get(struct gpio_chip *chip, unsigned int offset)
{
	struct ip6103_gpio *ip6103_gpio = gpiochip_get_data(chip);
	struct ip6103 *ip6103 = ip6103_gpio->ip6103;
	const struct ip6103_gpio_reg_cfg *cfg = &gpio_reg_cfg[offset];
	int ret;
	unsigned int val;

	if (offset >= chip->ngpio)
		return -EINVAL;

	ret = regmap_read(ip6103->regmap, cfg->reg, &val);
	if (ret < 0) {
		dev_err(ip6103->dev, "regmap_read error: %d\n", ret);
		return ret;
	}

	return !!(val & cfg->data);
}

static void ip6103_gpio_set(struct gpio_chip *chip, unsigned int offset, int value)
{
	struct ip6103_gpio *ip6103_gpio = gpiochip_get_data(chip);
	struct ip6103 *ip6103 = ip6103_gpio->ip6103;
	const struct ip6103_gpio_reg_cfg *cfg = &gpio_reg_cfg[offset];
	u8 data;

	if (offset >= chip->ngpio)
		return;

	if (value)
		data = cfg->data;
	else
		data = ~cfg->data;

	regmap_update_bits(ip6103->regmap, cfg->reg, cfg->data, data);
}

static const struct gpio_chip ip6103_gpio_chip = {
	.label			= "ip6103",
	.owner			= THIS_MODULE,
	.get			= ip6103_gpio_get,
	.set			= ip6103_gpio_set,
	.direction_input	= ip6103_gpio_direction_input,
	.direction_output	= ip6103_gpio_direction_output,
	.get_direction	= ip6103_gpio_get_direction,
	.base			= -1,
	.ngpio			= IP6103_MAX_GPIO,
	.can_sleep		= 1,
};

static int ip6103_gpio_probe(struct platform_device *pdev)
{
	struct ip6103 *ip6103 = dev_get_drvdata(pdev->dev.parent);
	struct ip6103_gpio *ip6103_gpio;
	int ret;

	ip6103_gpio = devm_kzalloc(&pdev->dev, sizeof(*ip6103_gpio),
				GFP_KERNEL);
	if (!ip6103_gpio)
		return -ENOMEM;

	ip6103_gpio->ip6103 = ip6103;
	ip6103_gpio->chip = ip6103_gpio_chip;
	ip6103_gpio->chip.parent = &pdev->dev;

	/* LOSC use gpio func,gpio 0 1 out high */
	ret = regmap_write(ip6103->regmap, 0x83, 0x2d);
	if (ret < 0) {
		dev_err(ip6103->dev, "regmap_write error: %d\n", ret);
		return ret;
	}
	/* use gpio 2 func,gpio 2 out low */
	ret = regmap_write(ip6103->regmap, 0x82, 0x4);
	if (ret < 0) {
		dev_err(ip6103->dev, "regmap_write error: %d\n", ret);
		return ret;
	}

	platform_set_drvdata(pdev, ip6103_gpio);

	return devm_gpiochip_add_data(&pdev->dev, &ip6103_gpio->chip,
					  ip6103_gpio);
}

static const struct of_device_id ip6103_gpio_of_match[] = {
	{ .compatible = "injoinic,ip6103-gpio", },
	{ }
};
MODULE_DEVICE_TABLE(of, ip6103_gpio_of_match);

static struct platform_driver ip6103_gpio_driver = {
	.probe = ip6103_gpio_probe,
	.driver = {
		.name = "ip6103-gpio",
		.of_match_table = ip6103_gpio_of_match,
	},
};
module_platform_driver(ip6103_gpio_driver);

MODULE_DESCRIPTION("IP6103 GPIO driver");
MODULE_ALIAS("platform:ip6103-gpio");
MODULE_LICENSE("GPL v2");

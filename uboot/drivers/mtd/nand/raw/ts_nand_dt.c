/*
 *
 * Copyright (C) 2018 Toradex
 * Copyright 2019 NXP
 * Copyright 2022 TsingMicro
 *
 * Authors:
 * Stefan Agner <liuyangyang@tsingmicro.com>
 *
 * Based on ts_nand_dt.c
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <dm.h>
#include <linux/io.h>
#include <linux/ioport.h>
#include <linux/printk.h>
#include <clk.h>
#include <linux/delay.h>

#include "ts_nand.h"

struct ts_nand_dt_data {
	unsigned int revision;
	unsigned int max_ecc_strength_supported;
};

static const struct ts_nand_dt_data ts_nand_tx536_data = {
	.revision = 0100,
	.max_ecc_strength_supported = 48,
};

static const struct udevice_id ts_nand_dt_ids[] = {
	{
		.compatible = "ts,tx536-nand",
		.data = (unsigned long)&ts_nand_tx536_data,
	},

	{ /* sentinel */ }
};

int ts_nand_clk_cfg(struct udevice *dev)
{
	struct clk clk;
	uint32_t rate;
	int ret;

	ret = clk_get_by_index(dev, 0, &clk);
	if (ret) {
		pr_debug("get clk 0 failed\n");
		//return ret;  ignor error on FPGA board
	} else {
		pr_debug("get clk 0 %s, id %ld\n", clk.dev->name, clk.id);
	}
	ret = clk_enable(&clk);
	if (ret) {
		pr_debug("enable clk 0 failed\n");
		//return ret;  ignor error on FPGA board
	}

	ret = clk_set_rate(&clk, TS_NAND_MAX_CLK);
	if (ret) {
		pr_debug("set clk %ld rate failed\n", clk.id);
		//return ret;  ignor error on FPGA board
	}

	rate = clk_get_rate(&clk);
	if ((int)rate <= 0) {
		pr_debug("get clk %ld rate failed\n", clk.id);
		//return -EINVAL;  ignor error on FPGA board
	}
	pr_debug("rate %d\n", rate);
	clk_free(&clk);
	return 0;
}

static int ts_nand_dt_probe(struct udevice *dev)
{
	struct ts_nand_info *info = dev_get_priv(dev);
	const struct ts_nand_dt_data *data;
	struct resource res;
	int ret;

	//read enable flag.
	info->nand_ctrl.config.enabled = dev_read_enabled(dev);
	if (!info->nand_ctrl.config.enabled)
		return -1;

	data = (void *)dev_get_driver_data(dev);
	if (data)
		info->max_ecc_strength_supported = data->max_ecc_strength_supported;

	info->dev = dev;

	ret = dev_read_resource_byname(dev, "ts-nand", &res);
	if (ret)
		return ret;

	ts_nand_clk_cfg(dev);
	info->nand_ctrl.regs = devm_ioremap(dev, res.start, resource_size(&res));

	//TODO enable clock...
	if (IS_ENABLED(CONFIG_CLK) && IS_ENABLED(CONFIG_CLK_TS)) {
		/* Assigned clock already set clock */
		struct clk ts_clk;

		ret = clk_get_by_name(dev, "nfc_clkb", &ts_clk);
		if (ret < 0) {
			pr_debug("Can't get clk_b clk: %d\n", ret);
			//return ret;
		}

		ret = clk_enable(&ts_clk);
		if (ret < 0) {
			pr_debug("Can't enable nfc_clkb clk: %d\n", ret);
			//return ret;
		}

		info->nand_ctrl.clk_rate = clk_get_rate(&ts_clk);
		if ((int)info->nand_ctrl.clk_rate <= 0) {
			pr_debug("get clk %ld rate failed\n", ts_clk.id);
			//return -EINVAL;  ignor error on FPGA board
			info->nand_ctrl.clk_rate = TS_NAND_MAX_CLK;
		}
		dev_info(&dev, "clock rate %lld\n", info->nand_ctrl.clk_rate);

	}

	ret = reset_get_bulk(dev, &info->resets);
	if (ret)
		dev_warn(dev, "Can't get reset: %d\n", ret);
	else {
		reset_assert_bulk(&info->resets);
		udelay(5);
		reset_deassert_bulk(&info->resets);
		udelay(5);
	}

	return ts_nand_init_ctrl(info);
}

static int ts_nand_remove(struct udevice *dev)
{
	struct ts_nand_info *info = dev_get_priv(dev);

	return reset_release_bulk(&info->resets);
}

U_BOOT_DRIVER(ts_nand_dt) = {
	.name = "ts-nand-dt",
	.id = UCLASS_MTD,
	.of_match = ts_nand_dt_ids,
	.priv_auto_alloc_size = sizeof(struct ts_nand_info),
	.probe = ts_nand_dt_probe,
	.remove = ts_nand_remove,
	.flags = DM_FLAG_OS_PREPARE,
};

void board_nand_init(void)
{
	struct udevice *dev;
	int ret;

	ret = uclass_get_device_by_driver(UCLASS_MTD,
					  DM_GET_DRIVER(ts_nand_dt),
					  &dev);
	if (ret && ret != -ENODEV)
		pr_err("Failed to initialize TS NAND controller. (error %d)\n",
		       ret);
}

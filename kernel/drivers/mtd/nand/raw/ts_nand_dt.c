/*
 * Copyright 2022 TsingMicro
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */
#include <linux/clk.h>
#include <linux/err.h>
#include <linux/io.h>
#include <linux/ioport.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/reset.h>
#include <linux/delay.h>

#include <linux/printk.h>
#include "ts_nand.h"

struct ts_nand_dt_data {
	unsigned int revision;
	unsigned int max_ecc_strength_supported;
};

static const struct ts_nand_dt_data ts_nand_tx536_data = {
	.revision = 0100,
	.max_ecc_strength_supported = 48,
};


struct ts_dt {
	struct ts_nand_info	ts;
	struct clk		*clk;
};

static const struct of_device_id ts_nand_dt_ids[] = {
	{
		.compatible = "ts,tx536-nand",
		.data = &ts_nand_tx536_data,
	},

	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, ts_nand_dt_ids);

static int ts_dt_probe(struct platform_device *pdev)
{
	struct resource *res;
	struct ts_dt *dt;
	struct device *dev = &pdev->dev;
	const struct ts_nand_dt_data *data;
	struct ts_nand_info *ts_nand;
	struct reset_control *rstc;
	int ret;
	//TODO enable reset...

	dt = devm_kzalloc(&pdev->dev, sizeof(*dt), GFP_KERNEL);
	if (!dt)
		return -ENOMEM;
	ts_nand = &dt->ts;

	data = of_device_get_match_data(&pdev->dev); //get device *data.
	if (data) {
		ts_nand->revision = data->revision;
		ts_nand->max_ecc_strength_supported =
			data->max_ecc_strength_supported;
	}

	ts_nand->dev = &pdev->dev;
	ts_nand->irq = platform_get_irq(pdev, 0);
	if (ts_nand->irq < 0) {
		dev_err(&pdev->dev, "no irq defined\n");
		return ts_nand->irq;
	}

	res = platform_get_resource_byname(pdev, IORESOURCE_MEM, "ts-nand");
	ts_nand->nand_ctrl.regs = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(ts_nand->nand_ctrl.regs))
		return PTR_ERR(ts_nand->nand_ctrl.regs);

	dt->clk = devm_clk_get(&pdev->dev, NULL);
	if (IS_ERR(dt->clk)) {
		dev_err(&pdev->dev, "Cannot claim NFC clock.\n");
		return PTR_ERR(dt->clk);
	}
#if defined(__UBOOT__)
	pr_info("clock %s.\n", __clk_get_name(dt->clk));
#endif
	ret = clk_prepare_enable(dt->clk);
	if (ret) {
		dev_err(&pdev->dev, "Cannot enable NFC clock.\n");
		return ret;
	}
	ts_nand->nand_ctrl.clk_rate = clk_get_rate(dt->clk);
	if ((int)ts_nand->nand_ctrl.clk_rate <= 0) {
#if defined(__UBOOT__)
		pr_debug("get clk %ld rate failed\n", dt->clk.id);
#endif
		//return -EINVAL;  ignor error on FPGA board
		ts_nand->nand_ctrl.clk_rate = TS_NAND_MAX_CLK;
	}
	dev_info(&pdev->dev, "clock rate %lld\n", ts_nand->nand_ctrl.clk_rate);

	/* Obtain NFC reset control */
	rstc = devm_reset_control_get_optional_exclusive(dev, "nfc_global");
	if (IS_ERR(rstc)) {
		ret = PTR_ERR(rstc);
		dev_err(dev, "Cannot get NFC reset.\n");
		goto probe_reset_failed;
	}

	reset_control_assert(rstc);
	udelay(5);
	reset_control_deassert(rstc);

	ret = ts_nand_init_ctrl(ts_nand);
	if (ret)
		goto out_disable_clk;

	platform_set_drvdata(pdev, dt);
	return 0;

probe_reset_failed:
out_disable_clk:
	clk_disable_unprepare(dt->clk);

	return ret;
}

static int ts_dt_remove(struct platform_device *pdev)
{
	struct ts_dt *dt = platform_get_drvdata(pdev);

	ts_nand_remove(&dt->ts);
	clk_disable_unprepare(dt->clk);

	return 0;
}

static struct platform_driver ts_dt_driver = {
	.probe		= ts_dt_probe,
	.remove		= ts_dt_remove,
	.driver		= {
		.name	= "ts-nand-dt",
		.of_match_table	= ts_nand_dt_ids,
	},
};

module_platform_driver(ts_dt_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Yang Liu, <liuyangyang@tsingmicro.com>");
MODULE_DESCRIPTION("DT driver for TS NAND controller");

#if defined(__UBOOT__)
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
#endif

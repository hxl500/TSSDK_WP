// SPDX-License-Identifier: GPL-2.0-only
/*
 * Memory-mapped interface driver for DW SPI Core
 *
 * Copyright (c) 2010, Octasic semiconductor.
 */

#include <linux/clk.h>
#include <linux/err.h>
#include <linux/platform_device.h>
#include <linux/pm_runtime.h>
#include <linux/slab.h>
#include <linux/spi/spi.h>
#include <linux/scatterlist.h>
#include <linux/mfd/syscon.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_platform.h>
#include <linux/acpi.h>
#include <linux/property.h>
#include <linux/regmap.h>
#include <linux/reset.h>
#include <linux/clk-provider.h>

#include "spi-dw.h"

#define DRIVER_NAME "dw_spi_mmio"

struct dw_spi_mmio {
	struct dw_spi  dws;
	struct clk     *clk;
	struct clk     *pclk;
	void           *priv;
	struct reset_control *rstc;
};

static int dw_spi_dw_apb_init(struct platform_device *pdev,
			      struct dw_spi_mmio *dwsmmio)
{
	dw_spi_dma_setup_generic(&dwsmmio->dws);

	return 0;
}

static int dw_spi_mmio_probe(struct platform_device *pdev)
{
	int (*init_func)(struct platform_device *pdev,
			 struct dw_spi_mmio *dwsmmio);
	struct dw_spi_mmio *dwsmmio;
	struct resource *mem;
	struct dw_spi *dws;
	int ret;
	int num_cs;

	dwsmmio = devm_kzalloc(&pdev->dev, sizeof(struct dw_spi_mmio),
			GFP_KERNEL);
	if (!dwsmmio)
		return -ENOMEM;

	dws = &dwsmmio->dws;

	/* Get basic io resource and map it */
	mem = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (IS_ERR(mem))
		return PTR_ERR(mem);

	dws->regs = devm_ioremap_resource(&pdev->dev, mem);
	if (IS_ERR(dws->regs)) {
		dev_err(&pdev->dev, "SPI region map failed\n");
		return PTR_ERR(dws->regs);
	}
	dws->paddr = mem->start;
	dws->irq = platform_get_irq(pdev, 0);
	if (dws->irq < 0) {
		dev_err(&pdev->dev, "no irq resource?\n");
		return dws->irq; /* -ENXIO */
	}

	dwsmmio->clk = devm_clk_get(&pdev->dev, NULL);
	if (IS_ERR(dwsmmio->clk))
		return PTR_ERR(dwsmmio->clk);

	pr_info("clock %s, rate %lu\n",
		__clk_get_name(dwsmmio->clk),
		clk_get_rate(dwsmmio->clk));

	ret = clk_prepare_enable(dwsmmio->clk);
	if (ret)
		return ret;

	/* Optional clock needed to access the registers */
	dwsmmio->pclk = devm_clk_get(&pdev->dev, "pclk");
	if (IS_ERR(dwsmmio->pclk)) {
		ret = PTR_ERR(dwsmmio->pclk);
		goto out_clk;
	}
	pr_info("pclk %s, rate %lu\n",
		__clk_get_name(dwsmmio->pclk),
		clk_get_rate(dwsmmio->pclk));

	ret = clk_prepare_enable(dwsmmio->pclk);
	if (ret)
		goto out_clk;

	/* find an optional reset controller */
	dwsmmio->rstc = devm_reset_control_get_optional_exclusive(&pdev->dev,
		"spi");
	if (IS_ERR(dwsmmio->rstc)) {
		ret = PTR_ERR(dwsmmio->rstc);
		goto out_clk;
	}
	reset_control_deassert(dwsmmio->rstc);

	dws->bus_num = pdev->id;

	dws->max_freq = clk_get_rate(dwsmmio->clk);

	pr_info("max_freq fix to %u\n", dws->max_freq);

	device_property_read_u32(&pdev->dev, "reg-io-width",
		&dws->reg_io_width);

	num_cs = 4;

	device_property_read_u32(&pdev->dev, "num-cs", &num_cs);

	dws->num_cs = num_cs;

	init_func = of_device_get_match_data(&pdev->dev);
	if (init_func) {
		ret = init_func(pdev, dwsmmio);
		if (ret)
			goto out;
	}

	pm_runtime_enable(&pdev->dev);

	ret = dw_spi_add_host(&pdev->dev, dws);
	if (ret)
		goto out;

	platform_set_drvdata(pdev, dwsmmio);
	return 0;

out:
	pm_runtime_disable(&pdev->dev);
	clk_disable_unprepare(dwsmmio->pclk);
out_clk:
	clk_disable_unprepare(dwsmmio->clk);
	reset_control_assert(dwsmmio->rstc);
	return ret;
}

static int dw_spi_mmio_remove(struct platform_device *pdev)
{
	struct dw_spi_mmio *dwsmmio = platform_get_drvdata(pdev);

	dw_spi_remove_host(&dwsmmio->dws);
	pm_runtime_disable(&pdev->dev);
	clk_disable_unprepare(dwsmmio->pclk);
	clk_disable_unprepare(dwsmmio->clk);
	reset_control_assert(dwsmmio->rstc);

	return 0;
}

static const struct of_device_id dw_spi_mmio_of_match[] = {
	{ .compatible = "snps,dw-apb-ssi", .data = dw_spi_dw_apb_init},
	{ /* end of table */}
};
MODULE_DEVICE_TABLE(of, dw_spi_mmio_of_match);

#ifdef CONFIG_ACPI
static const struct acpi_device_id dw_spi_mmio_acpi_match[] = {
	{"HISI0173", (kernel_ulong_t)dw_spi_dw_apb_init},
	{},
};
MODULE_DEVICE_TABLE(acpi, dw_spi_mmio_acpi_match);
#endif

static struct platform_driver dw_spi_mmio_driver = {
	.probe		= dw_spi_mmio_probe,
	.remove		= dw_spi_mmio_remove,
	.driver		= {
		.name	= DRIVER_NAME,
		.of_match_table = dw_spi_mmio_of_match,
		.acpi_match_table = ACPI_PTR(dw_spi_mmio_acpi_match),
	},
};
module_platform_driver(dw_spi_mmio_driver);

MODULE_AUTHOR("Jean-Hugues Deschenes <jean-hugues.deschenes@octasic.com>");
MODULE_DESCRIPTION("Memory-mapped I/O interface driver for DW SPI Core");
MODULE_LICENSE("GPL v2");

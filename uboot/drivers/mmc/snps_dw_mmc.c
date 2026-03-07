// SPDX-License-Identifier: GPL-2.0+
/*
 * Synopsys DesignWare Multimedia Card Interface driver
 * extensions used in various Synopsys ARC devboards.
 *
 * Copyright (C) 2019 Synopsys
 * Author: Eugeniy Paltsev <Eugeniy.Paltsev@synopsys.com>
 */
//#define DEBUG 1

#include "dm/ofnode.h"
#include <common.h>
#include <clk.h>
#include <dm.h>
#include <dwmmc.h>
#include <errno.h>
#include <fdtdec.h>
#include <dm/device_compat.h>
#include <linux/libfdt.h>
#include <linux/err.h>
#include <malloc.h>
#include <asm/gpio.h>

#define DEBUG 0xff



#define CLOCK_MIN		400000	/*  400 kHz */
#define FIFO_MIN		8
#define FIFO_MAX		4096

struct snps_dwmci_plat {
	struct mmc_config	cfg;
	struct mmc		mmc;
};

struct snps_dwmci_priv_data {
	struct dwmci_host	host;
	u32			f_max;
};

 int snps_dwmmc_clk_setup(struct udevice *dev)
{
#if CONFIG_ARCH_TS_TX5336
	struct snps_dwmci_priv_data *priv = dev_get_priv(dev);
	struct dwmci_host *host = &priv->host;

	struct clk clk_ciu, clk_biu;
    struct clk clk_ciu_parent;
    struct clk clk_ciu_gate;
    u32 ciu_rate;
	int ret;

	printf("dev 0x%p, size 0x%zx, name %s, ofnode %lx, %s\n", dev, sizeof(struct udevice),
		dev->name, dev->node.of_offset, ofnode_get_name(dev->node));

	// host->bus_hz = 200000000;   //need by ON_FPGA 

	ret = clk_get_by_name(dev, "ciu_mux", &clk_ciu);
	if (ret)
		goto clk_err;

	ret = clk_get_by_name(dev, "ciu_mux", &clk_ciu_gate);
	if (ret)
		goto clk_err;

	ret = clk_enable(&clk_ciu_gate);
	if (ret)
		goto clk_err;


    debug("got clk mux\n");
    ret = clk_get_by_name(dev, "ciu_div", &clk_ciu_parent);
	if (ret)
		goto clk_err_parent;
    debug("got ciu div\n");

    ret = clk_set_parent(&clk_ciu, &clk_ciu_parent);
	if (ret)
		goto clk_err_parent;
    debug("set clk ciu parent with ciu_div\n");

    dev_read_u32(dev,"core-clk-rate" , &ciu_rate);
    debug("ofnode clk:%u\n", ciu_rate);
    ret = clk_set_rate(&clk_ciu_parent, ciu_rate);

	if (IS_ERR_VALUE(ret))
		return ret;

	ret = clk_enable(&clk_ciu);
	if (ret && ret != -ENOSYS && ret != -ENOTSUPP)
	    goto clk_err_ciu;

	host->bus_hz = clk_get_rate(&clk_ciu);
	if (host->bus_hz < CLOCK_MIN) {
		ret = -EINVAL;
		goto clk_err_ciu_dis;
	}
    printf("set bus_hz with ciu_clk with:%d\n", host->bus_hz);

	ret = clk_enable(&clk_ciu);
	if (ret && ret != -ENOSYS && ret != -ENOTSUPP)
		goto clk_err_biu;

	ret = clk_get_by_name(dev, "biu", &clk_biu);
	if (ret)
		goto clk_err_ciu_dis;

    debug("got biu\n");

	ret = clk_enable(&clk_biu);
	if (ret && ret != -ENOSYS && ret != -ENOTSUPP)
		goto clk_err_biu;

    debug("mmc ahb clk:%ld\n",clk_get_rate(&clk_biu));

	return 0;

clk_err_biu:
	clk_free(&clk_biu);
clk_err_ciu:
	clk_free(&clk_ciu);
clk_err_ciu_dis:
	clk_disable(&clk_biu);
clk_err_parent:
	clk_free(&clk_ciu_parent);
clk_err:
	dev_err(dev, "failed to setup clocks, ret %d\n", ret);

	return ret;
#else
	struct snps_dwmci_priv_data *priv = dev_get_priv(dev);
	struct dwmci_host *host = &priv->host;

	struct clk clk_ciu, clk_siu, clk_diu, clk_hiu;
	int ret;

	debug("dev 0x%p, size 0x%x, name %s, ofnode %lx, %s\r\n", dev, sizeof(struct udevice),
		dev->name, dev->node.of_offset, ofnode_get_name(dev->node));

	ret = clk_get_by_name(dev, "ciu", &clk_ciu);
	if (ret)
		goto clk_err;

	ret = clk_enable(&clk_ciu);
	if (ret && ret != -ENOSYS && ret != -ENOTSUPP)
		goto clk_err_ciu;

	host->bus_hz = clk_get_rate(&clk_ciu);
	if (host->bus_hz < CLOCK_MIN) {
		ret = -EINVAL;
		goto clk_err_ciu_dis;
	}

	ret = clk_get_by_name(dev, "siu", &clk_siu);
	if (ret)
		goto clk_err_ciu_dis;

	ret = clk_enable(&clk_siu);
	if (ret && ret != -ENOSYS && ret != -ENOTSUPP)
		goto clk_err_siu;

	ret = clk_get_by_name(dev, "diu", &clk_diu);
	if (ret)
		goto clk_err_siu_dis;

	ret = clk_enable(&clk_diu);
	if (ret && ret != -ENOSYS && ret != -ENOTSUPP)
		goto clk_err_diu;

	/*hclk enable*/
	ret = clk_get_by_name(dev, "hiu", &clk_hiu);
	if (ret)
		goto clk_err_diu_dis;

	ret = clk_enable(&clk_hiu);
	if (ret && ret != -ENOSYS && ret != -ENOTSUPP)
		goto clk_err_hiu;

	return 0;

clk_err_hiu:
	clk_free(&clk_hiu);
clk_err_diu_dis:
	clk_disable(&clk_diu);
clk_err_diu:
	clk_free(&clk_diu);
clk_err_siu_dis:
	clk_disable(&clk_siu);
clk_err_siu:
	clk_free(&clk_siu);
clk_err_ciu_dis:
	clk_disable(&clk_ciu);
clk_err_ciu:
	clk_free(&clk_ciu);
clk_err:
	dev_err(dev, "failed to setup clocks, ret %d\n", ret);

	return ret;
#endif

}

static int snps_dwmmc_ofdata_to_platdata(struct udevice *dev)
{
	struct snps_dwmci_priv_data *priv = dev_get_priv(dev);
	struct dwmci_host *host = &priv->host;
	u32 fifo_depth;
	int ret;


	host->ioaddr = dev_read_addr_ptr(dev);

	/*
	 * If fifo-depth is unset don't set fifoth_val - we will try to
	 * auto detect it.
	 */
	ret = dev_read_u32(dev, "fifo-depth", &fifo_depth);
	if (!ret) {
		if (fifo_depth < FIFO_MIN || fifo_depth > FIFO_MAX)
			return -EINVAL;

		host->fifoth_val = MSIZE(0x2) |
				   RX_WMARK(fifo_depth / 2 - 1) |
				   TX_WMARK(fifo_depth / 2);
	}

	host->buswidth = dev_read_u32_default(dev, "bus-width", 4);
	if (host->buswidth != 1 && host->buswidth != 4 && host->buswidth != 8)
		return -EINVAL;

	/*
	 * If max-frequency is unset don't set priv->f_max - we will use
	 * host->bus_hz in probe() instead.
	 */
	ret = dev_read_u32(dev, "max-frequency", &priv->f_max);
	if (!ret && priv->f_max < CLOCK_MIN)
		return -EINVAL;

	host->fifo_mode = dev_read_bool(dev, "fifo-mode");
	host->name = dev->name;
	host->dev_index = 0;
	host->priv = priv;

	return 0;
}

int snps_dwmmc_getcd(struct udevice *dev)
{
	struct snps_dwmci_priv_data *priv = dev_get_priv(dev);
	struct dwmci_host *host = &priv->host;

	return !(dwmci_readl(host, DWMCI_CDETECT) & 1);
}

 void sd_pwr_gpio_init(struct udevice *dev)
{
	struct gpio_desc desc;
	int ret;

	ret = gpio_request_by_name_nodev(dev->node, "sd-pwr-gpios", 0, &desc, GPIOD_IS_OUT | GPIOD_IS_OUT_ACTIVE);
	if (ret) {
		pr_debug("gpio_request_by_name_nodev not provided %d\n", ret);
	}

	if (dm_gpio_is_valid(&desc)) {

		ret = dm_gpio_set_value(&desc, 0);
		if (ret < 0) {
			pr_err("dm_gpio_set_value failed: %d\n", ret);
		}
	}
}


struct dm_mmc_ops snps_dwmci_dm_ops;

static int snps_dwmmc_probe(struct udevice *dev)
{

#ifdef CONFIG_BLK
	struct snps_dwmci_plat *plat = dev_get_platdata(dev);
#endif
	struct mmc_uclass_priv *upriv = dev_get_uclass_priv(dev);
	struct snps_dwmci_priv_data *priv = dev_get_priv(dev);
	struct dwmci_host *host = &priv->host;
	unsigned int clock_max;
	int ret;
	
	/* Extend generic 'dm_dwmci_ops' with our 'getcd' implementation */
	memcpy(&snps_dwmci_dm_ops, &dm_dwmci_ops, sizeof(struct dm_mmc_ops));
	snps_dwmci_dm_ops.get_cd = snps_dwmmc_getcd;

	/* SD card power GPIO pull down */
	sd_pwr_gpio_init(dev);
	ret = snps_dwmmc_clk_setup(dev);
#if 0	
	if (ret)
		return ret;
#else
	printk("--snps_dwmmc_clk_setup ret %d, ignored---\n", ret);
#endif
	if (!priv->f_max)
		clock_max = host->bus_hz;
	else
		clock_max = min_t(unsigned int, host->bus_hz, priv->f_max);

#ifdef CONFIG_BLK
	dwmci_setup_cfg(&plat->cfg, host, clock_max, CLOCK_MIN);
	host->mmc = &plat->mmc;
#else
	ret = add_dwmci(host, clock_max, CLOCK_MIN);
	if (ret)
		return ret;
#endif
	host->mmc->priv = &priv->host;
	upriv->mmc = host->mmc;
	host->mmc->dev = dev;

	return dwmci_probe(dev);
}

static int snps_dwmmc_bind(struct udevice *dev)
{
	// printk("---snps_dwmmc_bind---\r\n");
#ifdef CONFIG_BLK
	struct snps_dwmci_plat *plat = dev_get_platdata(dev);
	int ret;

	ret = dwmci_bind(dev, &plat->mmc, &plat->cfg);
	if (ret)
		return ret;
#endif
	return 0;
}

static const struct udevice_id snps_dwmmc_ids[] = {
	{ .compatible = "snps,dw-mshc" },
	{ }
};

U_BOOT_DRIVER(snps_dwmmc_drv) = {
	.name				= "snps_dw_mmc",
	.id				= UCLASS_MMC,
	.of_match			= snps_dwmmc_ids,
	.ofdata_to_platdata		= snps_dwmmc_ofdata_to_platdata,
	.ops				= &snps_dwmci_dm_ops,
	.bind				= snps_dwmmc_bind,
	.probe				= snps_dwmmc_probe,
	.priv_auto_alloc_size		= sizeof(struct snps_dwmci_priv_data),
	.platdata_auto_alloc_size	= sizeof(struct snps_dwmci_plat),
};

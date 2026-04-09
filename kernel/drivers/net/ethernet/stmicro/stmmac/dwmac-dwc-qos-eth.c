/*
 * Synopsys DWC Ethernet Quality-of-Service v4.10a linux driver
 *
 * Copyright (C) 2016 Joao Pinto <jpinto@synopsys.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <linux/io.h>
#include <linux/of.h>
#include <linux/of_platform.h>
#include <linux/of_address.h>
#include <linux/clk.h>
#include <linux/clk-provider.h>
#include <linux/device.h>
#include <linux/gpio/consumer.h>
#include <linux/ethtool.h>
#include <linux/iopoll.h>
#include <linux/ioport.h>
#include <linux/module.h>
#include <linux/of_device.h>
#include <linux/of_net.h>
#include <linux/mfd/syscon.h>
#include <linux/platform_device.h>
#include <linux/reset.h>
#include <linux/stmmac.h>
#include <linux/regmap.h>

#include "stmmac_platform.h"
#include "dwmac4.h"

struct tegra_eqos {
	struct device *dev;
	void __iomem *regs;

	struct reset_control *rst;
	struct clk *clk_master;
	struct clk *clk_slave;
	struct clk *clk_tx;
	struct clk *clk_rx;

	struct gpio_desc *reset;
};

struct ts_crgu_speed {
	const char *desc;
	u32 reg;
	u32 mask;
	u32 shift;
	u32 sync;
};

struct ts_crgu_mode {
	const char *desc;
	u32 reg;
	u32 mask;
	u32 shift;
	u32 sync;
	u32 tx_clk_oe_bit;
};

struct ts_sys {
	const char *desc;
	u32 reg;
	u32 mask;
	u32 shift;
};

struct ts_eqos {
	int dev_id;
	struct device *dev;
	struct regmap *sysctl;
	struct regmap *clkctl;
	void __iomem *ioaddr;

	struct reset_control *rst;
	struct gpio_desc *reset;
	struct ts_sys *fix_sys;
};

static int dwc_eth_dwmac_config_dt(struct platform_device *pdev,
				   struct plat_stmmacenet_data *plat_dat)
{
	struct device_node *np = pdev->dev.of_node;
	u32 burst_map = 0;
	u32 bit_index = 0;
	u32 a_index = 0;

	if (!plat_dat->axi) {
		plat_dat->axi = kzalloc(sizeof(struct stmmac_axi), GFP_KERNEL);

		if (!plat_dat->axi)
			return -ENOMEM;
	}

	plat_dat->axi->axi_lpi_en = of_property_read_bool(np, "snps,en-lpi");
	if (of_property_read_u32(np, "snps,write-requests",
				 &plat_dat->axi->axi_wr_osr_lmt)) {
		/**
		 * Since the register has a reset value of 1, if property
		 * is missing, default to 1.
		 */
		plat_dat->axi->axi_wr_osr_lmt = 1;
	} else {
		/**
		 * If property exists, to keep the behavior from dwc_eth_qos,
		 * subtract one after parsing.
		 */
		plat_dat->axi->axi_wr_osr_lmt--;
	}

	if (of_property_read_u32(np, "snps,read-requests",
				 &plat_dat->axi->axi_rd_osr_lmt)) {
		/**
		 * Since the register has a reset value of 1, if property
		 * is missing, default to 1.
		 */
		plat_dat->axi->axi_rd_osr_lmt = 1;
	} else {
		/**
		 * If property exists, to keep the behavior from dwc_eth_qos,
		 * subtract one after parsing.
		 */
		plat_dat->axi->axi_rd_osr_lmt--;
	}
	of_property_read_u32(np, "snps,burst-map", &burst_map);

	/* converts burst-map bitmask to burst array */
	for (bit_index = 0; bit_index < 7; bit_index++) {
		if (burst_map & (1 << bit_index)) {
			switch (bit_index) {
			case 0:
			plat_dat->axi->axi_blen[a_index] = 4; break;
			case 1:
			plat_dat->axi->axi_blen[a_index] = 8; break;
			case 2:
			plat_dat->axi->axi_blen[a_index] = 16; break;
			case 3:
			plat_dat->axi->axi_blen[a_index] = 32; break;
			case 4:
			plat_dat->axi->axi_blen[a_index] = 64; break;
			case 5:
			plat_dat->axi->axi_blen[a_index] = 128; break;
			case 6:
			plat_dat->axi->axi_blen[a_index] = 256; break;
			default:
			break;
			}
			a_index++;
		}
	}

	/* dwc-qos needs GMAC4, AAL, TSO and PMT */
	plat_dat->has_gmac4 = 1;
	plat_dat->dma_cfg->aal = 1;
	plat_dat->tso_en = 1;
	plat_dat->pmt = 1;

	return 0;
}

static void *dwc_qos_probe(struct platform_device *pdev,
			   struct plat_stmmacenet_data *plat_dat,
			   struct stmmac_resources *stmmac_res)
{
	int err;

	plat_dat->stmmac_clk = devm_clk_get(&pdev->dev, "apb_pclk");
	if (IS_ERR(plat_dat->stmmac_clk)) {
		dev_err(&pdev->dev, "apb_pclk clock not found.\n");
		return ERR_CAST(plat_dat->stmmac_clk);
	}

	err = clk_prepare_enable(plat_dat->stmmac_clk);
	if (err < 0) {
		dev_err(&pdev->dev, "failed to enable apb_pclk clock: %d\n",
			err);
		return ERR_PTR(err);
	}

	plat_dat->pclk = devm_clk_get(&pdev->dev, "phy_ref_clk");
	if (IS_ERR(plat_dat->pclk)) {
		dev_err(&pdev->dev, "phy_ref_clk clock not found.\n");
		err = PTR_ERR(plat_dat->pclk);
		goto disable;
	}

	err = clk_prepare_enable(plat_dat->pclk);
	if (err < 0) {
		dev_err(&pdev->dev, "failed to enable phy_ref clock: %d\n",
			err);
		goto disable;
	}
	return NULL;

disable:
	clk_disable_unprepare(plat_dat->stmmac_clk);
	return ERR_PTR(err);
}

static int dwc_qos_remove(struct platform_device *pdev)
{
	struct net_device *ndev = platform_get_drvdata(pdev);
	struct stmmac_priv *priv = netdev_priv(ndev);

	clk_disable_unprepare(priv->plat->pclk);
	clk_disable_unprepare(priv->plat->stmmac_clk);

	return 0;
}

#define SDMEMCOMPPADCTRL 0x8800
#define  SDMEMCOMPPADCTRL_PAD_E_INPUT_OR_E_PWRD BIT(31)

#define AUTO_CAL_CONFIG 0x8804
#define  AUTO_CAL_CONFIG_START BIT(31)
#define  AUTO_CAL_CONFIG_ENABLE BIT(29)

#define AUTO_CAL_STATUS 0x880c
#define  AUTO_CAL_STATUS_ACTIVE BIT(31)

static void tegra_eqos_fix_speed(void *priv, unsigned int speed)
{
	struct tegra_eqos *eqos = priv;
	unsigned long rate = 125000000;
	bool needs_calibration = false;
	u32 value;
	int err;

	switch (speed) {
	case SPEED_1000:
		needs_calibration = true;
		rate = 125000000;
		break;

	case SPEED_100:
		needs_calibration = true;
		rate = 25000000;
		break;

	case SPEED_10:
		rate = 2500000;
		break;

	default:
		dev_err(eqos->dev, "invalid speed %u\n", speed);
		break;
	}

	if (needs_calibration) {
		/* calibrate */
		value = stmmac_readl(eqos->regs + SDMEMCOMPPADCTRL);
		value |= SDMEMCOMPPADCTRL_PAD_E_INPUT_OR_E_PWRD;
		stmmac_writel(value, eqos->regs + SDMEMCOMPPADCTRL);

		udelay(1);

		value = stmmac_readl(eqos->regs + AUTO_CAL_CONFIG);
		value |= AUTO_CAL_CONFIG_START | AUTO_CAL_CONFIG_ENABLE;
		stmmac_writel(value, eqos->regs + AUTO_CAL_CONFIG);

		err = stmmac_readl_poll_timeout_atomic(eqos->regs + AUTO_CAL_STATUS,
						value,
						value & AUTO_CAL_STATUS_ACTIVE,
						1, 10);
		if (err < 0) {
			dev_err(eqos->dev, "calibration did not start\n");
			goto failed;
		}

		err = stmmac_readl_poll_timeout_atomic(eqos->regs + AUTO_CAL_STATUS,
						value,
						(value & AUTO_CAL_STATUS_ACTIVE) == 0,
						20, 200);
		if (err < 0) {
			dev_err(eqos->dev, "calibration didn't finish\n");
			goto failed;
		}

	failed:
		value = stmmac_readl(eqos->regs + SDMEMCOMPPADCTRL);
		value &= ~SDMEMCOMPPADCTRL_PAD_E_INPUT_OR_E_PWRD;
		stmmac_writel(value, eqos->regs + SDMEMCOMPPADCTRL);
	} else {
		value = stmmac_readl(eqos->regs + AUTO_CAL_CONFIG);
		value &= ~AUTO_CAL_CONFIG_ENABLE;
		stmmac_writel(value, eqos->regs + AUTO_CAL_CONFIG);
	}

	err = clk_set_rate(eqos->clk_tx, rate);
	if (err < 0)
		dev_err(eqos->dev, "failed to set TX rate: %d\n", err);
}

static int tegra_eqos_init(struct platform_device *pdev, void *priv)
{
	struct tegra_eqos *eqos = priv;
	unsigned long rate;
	u32 value;

	rate = clk_get_rate(eqos->clk_slave);

	value = (rate / 1000000) - 1;
	stmmac_writel(value, eqos->regs + GMAC_1US_TIC_COUNTER);

	return 0;
}

static void *tegra_eqos_probe(struct platform_device *pdev,
			      struct plat_stmmacenet_data *data,
			      struct stmmac_resources *res)
{
	struct tegra_eqos *eqos;
	int err;

	eqos = devm_kzalloc(&pdev->dev, sizeof(*eqos), GFP_KERNEL);
	if (!eqos) {
		err = -ENOMEM;
		goto error;
	}

	eqos->dev = &pdev->dev;
	eqos->regs = res->addr;

	eqos->clk_master = devm_clk_get(&pdev->dev, "master_bus");
	if (IS_ERR(eqos->clk_master)) {
		err = PTR_ERR(eqos->clk_master);
		goto error;
	}

	err = clk_prepare_enable(eqos->clk_master);
	if (err < 0)
		goto error;

	eqos->clk_slave = devm_clk_get(&pdev->dev, "slave_bus");
	if (IS_ERR(eqos->clk_slave)) {
		err = PTR_ERR(eqos->clk_slave);
		goto disable_master;
	}

	data->stmmac_clk = eqos->clk_slave;

	err = clk_prepare_enable(eqos->clk_slave);
	if (err < 0)
		goto disable_master;

	eqos->clk_rx = devm_clk_get(&pdev->dev, "rx");
	if (IS_ERR(eqos->clk_rx)) {
		err = PTR_ERR(eqos->clk_rx);
		goto disable_slave;
	}

	err = clk_prepare_enable(eqos->clk_rx);
	if (err < 0)
		goto disable_slave;

	eqos->clk_tx = devm_clk_get(&pdev->dev, "tx");
	if (IS_ERR(eqos->clk_tx)) {
		err = PTR_ERR(eqos->clk_tx);
		goto disable_rx;
	}

	err = clk_prepare_enable(eqos->clk_tx);
	if (err < 0)
		goto disable_rx;

	eqos->reset = devm_gpiod_get(&pdev->dev, "phy-reset", GPIOD_OUT_HIGH);
	if (IS_ERR(eqos->reset)) {
		err = PTR_ERR(eqos->reset);
		goto disable_tx;
	}

	usleep_range(2000, 4000);
	gpiod_set_value(eqos->reset, 0);

	eqos->rst = devm_reset_control_get(&pdev->dev, "eqos");
	if (IS_ERR(eqos->rst)) {
		err = PTR_ERR(eqos->rst);
		goto reset_phy;
	}

	err = reset_control_assert(eqos->rst);
	if (err < 0)
		goto reset_phy;

	usleep_range(2000, 4000);

	err = reset_control_deassert(eqos->rst);
	if (err < 0)
		goto reset_phy;

	usleep_range(2000, 4000);

	data->fix_mac_speed = tegra_eqos_fix_speed;
	data->init = tegra_eqos_init;
	data->bsp_priv = eqos;

	err = tegra_eqos_init(pdev, eqos);
	if (err < 0)
		goto reset;

out:
	return eqos;

reset:
	reset_control_assert(eqos->rst);
reset_phy:
	gpiod_set_value(eqos->reset, 1);
disable_tx:
	clk_disable_unprepare(eqos->clk_tx);
disable_rx:
	clk_disable_unprepare(eqos->clk_rx);
disable_slave:
	clk_disable_unprepare(eqos->clk_slave);
disable_master:
	clk_disable_unprepare(eqos->clk_master);
error:
	eqos = ERR_PTR(err);
	goto out;
}

static int tegra_eqos_remove(struct platform_device *pdev)
{
	struct tegra_eqos *eqos = get_stmmac_bsp_priv(&pdev->dev);

	reset_control_assert(eqos->rst);
	gpiod_set_value(eqos->reset, 1);
	clk_disable_unprepare(eqos->clk_tx);
	clk_disable_unprepare(eqos->clk_rx);
	clk_disable_unprepare(eqos->clk_slave);
	clk_disable_unprepare(eqos->clk_master);

	return 0;
}

static int ts_eqos_init(struct platform_device *pdev, void *priv)
{
	unsigned long rate;
	u32 value;
	struct ts_eqos *eqos = priv;

	//rate = clk_get_rate(eqos->clk_slave);
	rate = 150 * 1000000;
	value = (rate / 1000000) - 1;
	stmmac_writel(value, eqos->ioaddr + GMAC_1US_TIC_COUNTER);

	return 0;
}

static int ts_eqos_fix_mode(struct platform_device *pdev,
				struct plat_stmmacenet_data *data,
			    void *priv)
{
	return 0;
}

static int eqos_get_dev_id(const char *name)
{
	char*q;
	char *p = strchr(name , '.');

	q = p + 9;

	if((p == NULL) || (q == NULL)) return 0;

	if((*q > '9') || (*q < '0'))
		return 0;
	else
		return (*q - '0');
}

static void ts_tx536_eqos_clk(struct platform_device *pdev,
				struct plat_stmmacenet_data *data,
			    void *priv)
{
	struct ts_eqos *eqos = priv;
	unsigned int clken;
	unsigned int clkmode;

	switch (data->interface) {
	case PHY_INTERFACE_MODE_RGMII:
	case PHY_INTERFACE_MODE_RGMII_ID:
	case PHY_INTERFACE_MODE_RGMII_RXID:
	case PHY_INTERFACE_MODE_RGMII_TXID:
		clken =  BIT(0) | BIT(6) | BIT(8) | BIT(9);
		clkmode = (0x3 << 0) | BIT(8) | BIT(13);
		break;
	case PHY_INTERFACE_MODE_MII:
		break;
	case PHY_INTERFACE_MODE_GMII:
		break;
	case PHY_INTERFACE_MODE_RMII:
		break;
	default:
		dev_err(&pdev->dev, "bad phy mode %d\n", data->interface);
		return;
	}

	regmap_write(eqos->clkctl, 0x140, clken);
	regmap_write(eqos->clkctl, 0x144, clkmode);

}

static void ts_tx536_eqos_fix_speed(void *priv, unsigned int speed)
{
	struct ts_eqos *eqos = priv;
	unsigned int val;
	unsigned int mac_speed;

	switch (speed) {
	case SPEED_1000:
		mac_speed = 0x2;
		break;

	case SPEED_100:
		mac_speed = 0x1;
		break;

	case SPEED_10:
		mac_speed = 0x0;
		break;

	default:
		dev_err(eqos->dev, "invalid speed %u\n", speed);
		break;
	}

	regmap_read(eqos->clkctl, 0x144, &val);
	val &= (~(0x3 << 4));
	val |= (mac_speed << 0x04) | BIT(8);

	regmap_write(eqos->clkctl, 0x144, val);

	return;
}

#define TX5336_GMAC_RMII	0x4
#define TX5336_GMAC_RGMII	0x1

typedef union
{
	struct
	{
		unsigned int we_clock_en:		1;//[0:0]
		unsigned int reserved0:			1;//[1:1]
		unsigned int gmac_rxclk_en:		1;//[2:2]
		unsigned int reserved1:			1;//[3:3]
		unsigned int gmac_ref_clk_sel:	1;//[4:4]
		unsigned int rmii_clk_from_ephy_pad_en:	1;//[5:5]
		unsigned int rmii_clk_from_pll_en:	1;//[6:6]
		unsigned int ptp_ref_clk_en:	1;//[7:7]
		unsigned int ephy_ref_clk_en:	1;//[8:8]
		unsigned int pll0_d4_gmac_tx_clk_en:1;//[9:9]
		unsigned int ref_rmii_txrx_clk_en:	1;//[10:10]
		unsigned int ref_rmii_txrx_clk_div_num:	5;//[11:15]
		unsigned int pll0_d4_ephy_ref_clk_div_num:	8;//[16:23]
		unsigned int pll0_d4_gmac_tx_clk_div_num:	8;//[24:31]
	};
	unsigned int u32;
} GMAC_CFG1_U;

static void ts_tx5336_eqos_clk(struct platform_device *pdev,
				struct plat_stmmacenet_data *data,
			    void *priv)
{

	struct ts_eqos *eqos = priv;
	unsigned int eth_mac_mode;
	unsigned int value = 0;
	GMAC_CFG1_U val = {0};

	switch (data->interface) {
	case PHY_INTERFACE_MODE_RGMII:
	case PHY_INTERFACE_MODE_RGMII_ID:
	case PHY_INTERFACE_MODE_RGMII_RXID:
	case PHY_INTERFACE_MODE_RGMII_TXID:
		eth_mac_mode = TX5336_GMAC_RGMII;
		val.we_clock_en = 1;
		val.gmac_rxclk_en = 1;
		val.gmac_ref_clk_sel = 0;
		val.rmii_clk_from_ephy_pad_en = 0;
		val.rmii_clk_from_pll_en = 0;
		val.ptp_ref_clk_en = 1;
		val.ephy_ref_clk_en =1;
		val.pll0_d4_gmac_tx_clk_en = 1;
		val.ref_rmii_txrx_clk_div_num = 0x1;
		val.pll0_d4_ephy_ref_clk_div_num = 0x3;//1000Mbps
		val.pll0_d4_gmac_tx_clk_div_num = 0x3; //1000Mbps

		regmap_read(eqos->clkctl, 0xcc, &value);
		value = (value & 0xffff0000) | (0x1d << 8) | 0x13;
		  //modify rx delay to 0x1f, tx delay to 0x10, lyy 20240621
		regmap_write(eqos->clkctl, 0xcc, value);
		break;
	case PHY_INTERFACE_MODE_RMII:
		eth_mac_mode = TX5336_GMAC_RMII;
		val.we_clock_en = 1;
		val.gmac_rxclk_en = 0;
		val.gmac_ref_clk_sel = 1;
		val.rmii_clk_from_ephy_pad_en = 1;
		val.rmii_clk_from_pll_en = 0;
		val.ptp_ref_clk_en = 1;
		val.ephy_ref_clk_en = 0;
		val.pll0_d4_gmac_tx_clk_en = 0;
		val.ref_rmii_txrx_clk_en = 1;
		val.ref_rmii_txrx_clk_div_num = 1;
		val.pll0_d4_ephy_ref_clk_div_num = 0x9;//100Mbps
		val.pll0_d4_gmac_tx_clk_div_num = 0x13; //100Mbps

		regmap_write(eqos->clkctl, 0x12c, 0x313115);//chipoutclk1 output 25MHZ
		break;
	default:
		dev_err(&pdev->dev, "bad phy mode %d\n", data->interface);
		return;
	}
	value = (eth_mac_mode << 12);
	/*gmax axi clk pll0_d2/(4+ 1)*/
	value |= (0x4 << 8);
	/*gmac axi & ahb enable */
	value |= BIT(3) | BIT(2);

	regmap_write(eqos->clkctl, 0xc4, value | BIT(0));
	regmap_write(eqos->clkctl, 0xc8, val.u32);
}

static void ts_tx5336_eqos_fix_speed(void *priv, unsigned int speed)
{
	struct ts_eqos *eqos = priv;
	GMAC_CFG1_U val;
	unsigned int mac_mode;

	regmap_read(eqos->clkctl, 0xc4, &mac_mode);
	regmap_read(eqos->clkctl, 0xc8, &val.u32);

	mac_mode = (mac_mode >> 12) & 0x7;
	if((TX5336_GMAC_RGMII != mac_mode) && (TX5336_GMAC_RMII != mac_mode)) {
		dev_err(eqos->dev, "invalid mac_mode %u\n", mac_mode);
		return;
	}

	switch (speed) {
	case SPEED_1000:
		if(TX5336_GMAC_RGMII == mac_mode) {
			val.pll0_d4_gmac_tx_clk_div_num = 0x3;
			val.pll0_d4_ephy_ref_clk_div_num = 0x3;
		} else {
			;;
		}
		break;

	case SPEED_100:
		if(TX5336_GMAC_RGMII == mac_mode) {
			val.pll0_d4_gmac_tx_clk_div_num = 0x13;
			val.pll0_d4_ephy_ref_clk_div_num = 0x13;
		}  else if(TX5336_GMAC_RMII == mac_mode) {
			val.ref_rmii_txrx_clk_div_num = 1;
			val.pll0_d4_ephy_ref_clk_div_num = 0x9;//100Mbps
			val.pll0_d4_gmac_tx_clk_div_num = 0x13; //100Mbps
		} else {
			;;
		}
		break;

	case SPEED_10:
		if(TX5336_GMAC_RGMII == mac_mode) {
			val.pll0_d4_gmac_tx_clk_div_num = 0xc7;
			val.pll0_d4_ephy_ref_clk_div_num = 0xc7;
		} else if(TX5336_GMAC_RMII == mac_mode) {
			val.ref_rmii_txrx_clk_div_num = 0x13;
			val.pll0_d4_ephy_ref_clk_div_num = 0x9;//10Mbps
			val.pll0_d4_gmac_tx_clk_div_num = 0xc7; //10Mbps
		} else {
			;;
		}
		break;

	default:
		dev_err(eqos->dev, "invalid speed %u\n", speed);
		break;
	}

	val.we_clock_en = 1;
	regmap_write(eqos->clkctl, 0xc8, val.u32);

	return;
}

static void ts_tx5215_eqos_clk(struct platform_device *pdev,
				struct plat_stmmacenet_data *data,
			    void *priv)
{
	struct ts_eqos *eqos = priv;
	unsigned int clken;
	unsigned int clkcfg;

	switch (data->interface) {
	case PHY_INTERFACE_MODE_RGMII:
	case PHY_INTERFACE_MODE_RGMII_ID:
	case PHY_INTERFACE_MODE_RGMII_RXID:
	case PHY_INTERFACE_MODE_RGMII_TXID:
		break;
	case PHY_INTERFACE_MODE_MII:
		break;
	case PHY_INTERFACE_MODE_GMII:
		break;
	case PHY_INTERFACE_MODE_RMII:
		clken = BIT(4) | BIT(5) | BIT(8) | BIT(9);
		clkcfg = (0x1 << 0) | BIT(8) | BIT(13) | (0x1 << 16) | BIT(19);
		break;
	default:
		dev_err(&pdev->dev, "bad phy mode %d\n", data->interface);
		return;
	}

	regmap_write(eqos->clkctl, eqos->dev_id == 0 ? 0xe0 : 0x3e0, clken);
	regmap_write(eqos->clkctl, eqos->dev_id == 0 ? 0xe4 : 0x3e4, clkcfg);

}

#ifdef CONFIG_GMAC_PHY_SPI_CLK_INIT
#define GMAC_PHY_10M_INIT_NEED          1
#else
#define GMAC_PHY_10M_INIT_NEED          0
#endif

/* LLI == Linked List Item; a.k.a. DMA block descriptor */
typedef struct {
	/* values that are not changed by hardware */
	u32		sar;
	u32		dar;
	u32		llp;		/* chain to next lli */
	u32		ctllo;
	/* values that may get written back: */
	u32		ctlhi;
	/* sstat and dstat can snapshot peripheral register state.
	 * silicon config may discard either or both...
	 */
	u32		sstat;
	u32		dstat;
}dw_ahb_dma_lli;

static dw_ahb_dma_lli *g_dma_lli = NULL;
#define USED_SRAM_ADDR (0xF4000100)

static void ts_tx5112_dma_init(void __iomem *dma_base)
{
	int i;
	dw_ahb_dma_lli *last_lli = NULL;

	stmmac_writel((0x1 << 5), dma_base + 0x98);
	stmmac_writel((0x1 << 1) | (0x1 << 11), dma_base + 0x98 + 0x4);

	g_dma_lli = (dw_ahb_dma_lli *)ioremap(USED_SRAM_ADDR, 0x100);
	for(i = 0; i < 3; i++) {
		g_dma_lli[i].sar = 0xF4000000UL + i * 0x10;
		g_dma_lli[i].dar = 0xf244d060;
		g_dma_lli[i].llp = (u32)(USED_SRAM_ADDR +
						(i+1)*sizeof(dw_ahb_dma_lli)) | 0x1;
		g_dma_lli[i].ctllo = 0x1a102121;
		g_dma_lli[i].ctlhi = 0xf;
		last_lli = &g_dma_lli[i];
	#if 0
		pr_err("lli%d 0x%p\r\n", i, last_lli);
		pr_err("sar_lo      = 0x%x\r\n", last_lli->sar);
		pr_err("dar_lo      = 0x%x\r\n", last_lli->dar);
		pr_err("llp_lo      = 0x%x\r\n", last_lli->llp);
		pr_err("ctl_lo      = 0x%x\r\n", last_lli->ctllo);
		pr_err("ctl_hi      = 0x%x\r\n", last_lli->ctlhi);
		pr_err("----------------\r\n");
	#endif
	}

	last_lli->llp = (u32)USED_SRAM_ADDR | 0x1;

	stmmac_writel((0x1), dma_base + 0x398);
	stmmac_writel(((u32)USED_SRAM_ADDR | 0x1), dma_base + 0x68);

	stmmac_writel((1 << 27) | (1 << 28), dma_base + 0x70);
	stmmac_writel(0, dma_base + 0x74);
	stmmac_writel((0x1 <<9) | (0x1 << 1), dma_base + 0x3a0);
}

static void ts_tx5112_spi_init(void __iomem *spi_base)
{
	u32 val;
	void __iomem *crgu_base = NULL;

	crgu_base = ioremap(0xf1004000, 0x1000);
	if (!crgu_base) {
		pr_err("%s crgu_base is null\r\n", __func__);
		return;
	}
	val = stmmac_readl(crgu_base + 0xa0);
	val &= ~(0x1f << 16);
	val |= (0x4 << 16) | (1 << 8) | (1 << 3) | BIT(0);
	stmmac_writel(val, crgu_base + 0xa0);
	stmmac_writel(val, crgu_base + 0xa0);

	stmmac_writel((0x7 << 16) | (0x0 << 0) | (0x1 << 8), spi_base + 0x0);
	stmmac_writel((0x8 << 0), spi_base + 0x14);

	stmmac_writel((0x1 << 1), spi_base + 0x4c);
	stmmac_writel((0x10 << 0), spi_base + 0x50);
	stmmac_writel((0xf << 0), spi_base + 0x54);

	stmmac_writel((0x1 << 0), spi_base + 0x10);
	stmmac_writel((0x1 << 0), spi_base + 0x8);
}

int ts_tx5112_spi_dma_clk(void)
{
	void __iomem *dma_base = NULL;
	void __iomem *spi_base = NULL;
	void __iomem *crgu_base = NULL;
	u32 val;

	crgu_base = ioremap(0xf1004000, 0x1000);
	if (!crgu_base) {
		pr_err("%s crgu_base is null\r\n", __func__);
		return -ENODEV;
	}

	dma_base = ioremap(0xf2100000, 0x1000);
	if (!dma_base) {
		pr_err("dma_base is null\r\n");
		return -ENODEV;
	}

	spi_base = ioremap(0xf244d000, 0x1000);
	if (!spi_base) {
		pr_err("spi_base is null\r\n");
		return -ENODEV;
	}

	/* reset, because uboot already working */
	val = stmmac_readl(crgu_base + 0x4c);
	val |= (1 << 16) | (1 << 1);
	stmmac_writel(val, crgu_base + 0x4c);

	val = stmmac_readl(crgu_base + 0xa0);
	val |= (1 << 5) | (1 <<6) | (1 << 1);
	stmmac_writel(val, crgu_base + 0xa0);

	mdelay(10);
	val = stmmac_readl(crgu_base + 0x4c);
	val &= ~(1 << 16);
	val |= (1 << 1);
	stmmac_writel(val, crgu_base + 0x4c);

	val = stmmac_readl(crgu_base + 0xa0);
	val &= ~((1 << 5) | (1 <<6));
	val |= (1 << 1);
	stmmac_writel(val, crgu_base + 0xa0);

	//pr_err("dma_base = 0x%p, spi_base = 0x%p\r\n", dma_base, spi_base);
	ts_tx5112_dma_init(dma_base);
	ts_tx5112_spi_init(spi_base);

	return 0;
}

static void ts_tx5112_eqos_clk(struct platform_device *pdev,
				struct plat_stmmacenet_data *data,
			    void *priv)
{
	struct ts_eqos *eqos = priv;
	unsigned int clkcfg;

	switch (data->interface) {
	case PHY_INTERFACE_MODE_RGMII:
	case PHY_INTERFACE_MODE_RGMII_ID:
	case PHY_INTERFACE_MODE_RGMII_RXID:
	case PHY_INTERFACE_MODE_RGMII_TXID:
		clkcfg = (0x2 << 16) | (0x3 << 9) | BIT(2) | BIT(6) | BIT(8) | BIT(14);
		break;
	case PHY_INTERFACE_MODE_MII:
		break;
	case PHY_INTERFACE_MODE_GMII:
		break;
	case PHY_INTERFACE_MODE_RMII:
		clkcfg = (0x1 << 9) | BIT(4) | BIT(5) | BIT(6) | BIT(7) | BIT(8) | BIT(14) | BIT(16);
		break;
	default:
		dev_err(&pdev->dev, "bad phy mode %d\n", data->interface);
		return;
	}

	regmap_write(eqos->clkctl, 0x74, clkcfg | BIT(0));
	regmap_write(eqos->clkctl, 0x74, clkcfg | BIT(0));
#if GMAC_PHY_10M_INIT_NEED
	ts_tx5112_spi_dma_clk();
#endif
}

static void ts_tx5215_eqos_fix_speed(void *priv, unsigned int speed)
{
	struct ts_eqos *eqos = priv;
	unsigned int val;
	unsigned int mac_speed;

	switch (speed) {
	case SPEED_1000:
		mac_speed = 0x2;
		break;

	case SPEED_100:
		mac_speed = 0x1;
		break;

	case SPEED_10:
		mac_speed = 0x0;
		break;

	default:
		dev_err(eqos->dev, "invalid speed %u\n", speed);
		break;
	}

	regmap_read(eqos->clkctl, eqos->dev_id == 0 ? 0xe4 : 0x3e4, &val);
	val &= (~(0x3 << 4));
	val |= (mac_speed << 0x04) | BIT(8);
	regmap_write(eqos->clkctl, eqos->dev_id == 0 ? 0xe4 : 0x3e4, val);

	return;
}

static void ts_tx5112_eqos_fix_speed(void *priv, unsigned int speed)
{
	struct ts_eqos *eqos = priv;
	unsigned int val;
	unsigned int mac_speed;
#if GMAC_PHY_10M_INIT_NEED
	void __iomem *iomux_base = NULL;
	iomux_base = ioremap(0xf0000000, 0x1000);
#endif

	switch (speed) {
	case SPEED_1000:
		mac_speed = 0x2;
		break;

	case SPEED_100:
		mac_speed = 0x1;
	/*mac_txclk -> spi_clk*/
	#if GMAC_PHY_10M_INIT_NEED
		val = stmmac_readl(iomux_base + 0x52c);
		val &= 0xffffff0f;
		stmmac_writel(val, iomux_base + 0x52c);
	#endif
		break;

	case SPEED_10:
		mac_speed = 0x0;
	/*mac_txclk -> spi_clk*/
	#if GMAC_PHY_10M_INIT_NEED
		val = stmmac_readl(iomux_base + 0x52c);
		val &= 0xffffff0f;
		val |= 0x50;
		stmmac_writel(val, iomux_base + 0x52c);
	#endif
		break;

	default:
		dev_err(eqos->dev, "invalid speed %u\n", speed);
		break;
	}

	regmap_read(eqos->clkctl, 0x74, &val);
	val &= (~(0x3 << 11));
	val |= (mac_speed << 11);
	regmap_write(eqos->clkctl, 0x74, val | BIT(15));

	return;
}

static void *ts_tx536_eqos_probe(struct platform_device *pdev,
			      struct plat_stmmacenet_data *data,
			      struct stmmac_resources *res)
{

	struct ts_eqos *eqos;
	int err;

	eqos = devm_kzalloc(&pdev->dev, sizeof(*eqos), GFP_KERNEL);
	if (!eqos) {
		err = -ENOMEM;
		goto error;
	}

	eqos->dev = &pdev->dev;
	eqos->ioaddr = res->addr;

	eqos->dev_id = eqos_get_dev_id(pdev->name);

	eqos->clkctl = syscon_regmap_lookup_by_phandle(pdev->dev.of_node, "ts,clkctl");
	if (IS_ERR(eqos->clkctl)) {
		eqos->clkctl = NULL;
		pr_err("%s: Unable to get clkctl\n", __func__);
		goto error;
	}

	ts_tx536_eqos_clk(pdev, data, eqos);

	eqos->reset = devm_gpiod_get(&pdev->dev, "phy-reset", GPIOD_OUT_HIGH);
	if (!IS_ERR(eqos->reset)) {
		usleep_range(2000, 4000);
		gpiod_set_value(eqos->reset, 0);
	}
	usleep_range(50000, 60000);
	
	data->fix_mac_speed = ts_tx536_eqos_fix_speed;
	data->init = ts_eqos_init;

	data->bsp_priv = eqos;
	err = ts_eqos_init(pdev, eqos);
	if (err < 0)
		goto error;

	err = ts_eqos_fix_mode(pdev, data, eqos);
	if (err < 0)
		goto error;

out:
	return eqos;

error:
	dev_err(&pdev->dev, "func %s line %d\n", __func__, __LINE__);
	eqos = ERR_PTR(err);
	goto out;

	return NULL;
}

static void *ts_tx5336_eqos_probe(struct platform_device *pdev,
			      struct plat_stmmacenet_data *data,
			      struct stmmac_resources *res)
{

	struct ts_eqos *eqos;
	int err;

	eqos = devm_kzalloc(&pdev->dev, sizeof(*eqos), GFP_KERNEL);
	if (!eqos) {
		err = -ENOMEM;
		goto error;
	}

	eqos->dev = &pdev->dev;
	eqos->ioaddr = res->addr;

	eqos->dev_id = eqos_get_dev_id(pdev->name);
	eqos->clkctl = syscon_regmap_lookup_by_phandle(pdev->dev.of_node, "ts,clkctl");
	if (IS_ERR(eqos->clkctl)) {
		eqos->clkctl = NULL;
		pr_err("%s: Unable to get clkctl\n", __func__);
		goto error;
	}

	ts_tx5336_eqos_clk(pdev, data, eqos);

	eqos->reset = devm_gpiod_get(&pdev->dev, "phy-reset", GPIOD_OUT_HIGH);

	if (!IS_ERR(eqos->reset)) {
		usleep_range(2000, 4000);
		gpiod_set_value(eqos->reset, 0);
	}
	usleep_range(50000, 60000);

	data->fix_mac_speed = ts_tx5336_eqos_fix_speed;

	data->init = ts_eqos_init;

	data->bsp_priv = eqos;
	err = ts_eqos_init(pdev, eqos);
	if (err < 0)
		goto error;

	err = ts_eqos_fix_mode(pdev, data, eqos);
	if (err < 0)
		goto error;

out:
	return eqos;

error:
	dev_err(&pdev->dev, "func %s line %d\r\n", __func__, __LINE__);
	eqos = ERR_PTR(err);
	goto out;

	return NULL;
}

static void *ts_tx5215_eqos_probe(struct platform_device *pdev,
			      struct plat_stmmacenet_data *data,
			      struct stmmac_resources *res)
{

	struct ts_eqos *eqos;
	int err;

	eqos = devm_kzalloc(&pdev->dev, sizeof(*eqos), GFP_KERNEL);
	if (!eqos) {
		err = -ENOMEM;
		goto error;
	}

	eqos->dev = &pdev->dev;
	eqos->ioaddr = res->addr;
	eqos->dev_id = eqos_get_dev_id(pdev->name);

	eqos->clkctl = syscon_regmap_lookup_by_phandle(pdev->dev.of_node, "ts,clkctl");
	if (IS_ERR(eqos->clkctl)) {
		eqos->clkctl = NULL;
		pr_err("%s: Unable to get clkctl\n", __func__);
		goto error;
	}

	ts_tx5215_eqos_clk(pdev, data, eqos);

	eqos->reset = devm_gpiod_get(&pdev->dev, "phy-reset", GPIOD_OUT_HIGH);
	if (!IS_ERR(eqos->reset)) {
		usleep_range(10 * 1000, 20 * 1000);
		gpiod_set_value(eqos->reset, 0);
	}

	data->fix_mac_speed = ts_tx5215_eqos_fix_speed;
	data->init = ts_eqos_init;

	data->bsp_priv = eqos;
	err = ts_eqos_init(pdev, eqos);
	if (err < 0)
		goto error;

	err = ts_eqos_fix_mode(pdev, data, eqos);
	if (err < 0)
		goto error;

out:
	return eqos;

error:
	dev_err(&pdev->dev, "func %s line %d\n", __func__, __LINE__);
	eqos = ERR_PTR(err);
	goto out;

	return NULL;
}

static void *ts_tx5112_eqos_probe(struct platform_device *pdev,
			      struct plat_stmmacenet_data *data,
			      struct stmmac_resources *res)
{

	struct ts_eqos *eqos;
	int err;

	eqos = devm_kzalloc(&pdev->dev, sizeof(*eqos), GFP_KERNEL);
	if (!eqos) {
		err = -ENOMEM;
		goto error;
	}

	eqos->dev = &pdev->dev;
	eqos->ioaddr = res->addr;

	eqos->clkctl = syscon_regmap_lookup_by_phandle(pdev->dev.of_node, "ts,clkctl");
	if (IS_ERR(eqos->clkctl)) {
		eqos->clkctl = NULL;
		pr_err("%s: Unable to get clkctl\n", __func__);
		goto error;
	}

	ts_tx5112_eqos_clk(pdev, data, eqos);
	ts_tx5112_eqos_fix_speed(eqos,SPEED_100);//fixbug speed

	eqos->reset = devm_gpiod_get(&pdev->dev, "phy-reset", GPIOD_OUT_HIGH);
	if (!IS_ERR(eqos->reset)) {
		usleep_range(10*1000, 20*1000);
		gpiod_set_value(eqos->reset, 0);
	}

	data->fix_mac_speed = ts_tx5112_eqos_fix_speed;
	data->init = ts_eqos_init;

	data->bsp_priv = eqos;
	err = ts_eqos_init(pdev, eqos);
	if (err < 0)
		goto error;

	err = ts_eqos_fix_mode(pdev, data, eqos);
	if (err < 0)
		goto error;

out:
	return eqos;

error:
	dev_err(&pdev->dev, "func %s line %d\n", __func__, __LINE__);
	eqos = ERR_PTR(err);
	goto out;

	return NULL;
}

static int ts_eqos_remove(struct platform_device *pdev)
{
	return 0;
}

struct dwc_eth_dwmac_data {
	void *(*probe)(struct platform_device *pdev,
		       struct plat_stmmacenet_data *data,
		       struct stmmac_resources *res);
	int (*remove)(struct platform_device *pdev);
};

static const struct dwc_eth_dwmac_data dwc_qos_data = {
	.probe = dwc_qos_probe,
	.remove = dwc_qos_remove,
};

static const struct dwc_eth_dwmac_data tegra_eqos_data = {
	.probe = tegra_eqos_probe,
	.remove = tegra_eqos_remove,
};

static const struct dwc_eth_dwmac_data ts_tx536_eqos_data = {
	.probe = ts_tx536_eqos_probe,
	.remove = ts_eqos_remove,
};

static const struct dwc_eth_dwmac_data ts_tx5336_eqos_data = {
	.probe = ts_tx5336_eqos_probe,
	.remove = ts_eqos_remove,
};

static const struct dwc_eth_dwmac_data ts_tx5215_eqos_data = {
	.probe = ts_tx5215_eqos_probe,
	.remove = ts_eqos_remove,
};

static const struct dwc_eth_dwmac_data ts_tx5112_eqos_data = {
	.probe = ts_tx5112_eqos_probe,
	.remove = ts_eqos_remove,
};

static int dwc_eth_dwmac_probe(struct platform_device *pdev)
{
	const struct dwc_eth_dwmac_data *data;
	struct plat_stmmacenet_data *plat_dat;
	struct stmmac_resources stmmac_res;
	struct resource *res;
	void *priv;
	int ret;

	data = of_device_get_match_data(&pdev->dev);

	memset(&stmmac_res, 0, sizeof(struct stmmac_resources));

	/**
	 * Since stmmac_platform supports name IRQ only, basic platform
	 * resource initialization is done in the glue logic.
	 */
	stmmac_res.irq = platform_get_irq(pdev, 0);
	if (stmmac_res.irq < 0) {
		if (stmmac_res.irq != -EPROBE_DEFER)
			dev_err(&pdev->dev,
				"IRQ configuration information not found\n");

		return stmmac_res.irq;
	}
	stmmac_res.wol_irq = stmmac_res.irq;

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	stmmac_res.addr = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(stmmac_res.addr))
		return PTR_ERR(stmmac_res.addr);

	plat_dat = stmmac_probe_config_dt(pdev, &stmmac_res.mac);
	if (IS_ERR(plat_dat))
		return PTR_ERR(plat_dat);

	priv = data->probe(pdev, plat_dat, &stmmac_res);
	if (IS_ERR(priv)) {
		ret = PTR_ERR(priv);
		dev_err(&pdev->dev, "failed to probe subdriver: %d\n", ret);
		goto remove_config;
	}

	ret = dwc_eth_dwmac_config_dt(pdev, plat_dat);
	if (ret)
		goto remove;

	ret = stmmac_dvr_probe(&pdev->dev, plat_dat, &stmmac_res);
	if (ret)
		goto remove;

	return ret;

remove:
	data->remove(pdev);
remove_config:
	stmmac_remove_config_dt(pdev, plat_dat);

	return ret;
}

static int dwc_eth_dwmac_remove(struct platform_device *pdev)
{
	struct net_device *ndev = platform_get_drvdata(pdev);
	struct stmmac_priv *priv = netdev_priv(ndev);
	const struct dwc_eth_dwmac_data *data;
	int err;

	data = of_device_get_match_data(&pdev->dev);

	err = stmmac_dvr_remove(&pdev->dev);
	if (err < 0)
		dev_err(&pdev->dev, "failed to remove platform: %d\n", err);

	err = data->remove(pdev);
	if (err < 0)
		dev_err(&pdev->dev, "failed to remove subdriver: %d\n", err);

	stmmac_remove_config_dt(pdev, priv->plat);

	return err;
}

static const struct of_device_id dwc_eth_dwmac_match[] = {
	{ .compatible = "snps,dwc-qos-ethernet-4.10", .data = &dwc_qos_data },
	{ .compatible = "nvidia,tegra186-eqos", .data = &tegra_eqos_data },
	{ .compatible = "ts-tx536,dwc-qos-ethernet-4.10", .data = &ts_tx536_eqos_data },
	{ .compatible = "ts-tx5336,dwc-qos-ethernet-4.10", .data = &ts_tx5336_eqos_data },
	{ .compatible = "ts-tx5215,dwc-qos-ethernet-4.10", .data = &ts_tx5215_eqos_data },
	{ .compatible = "ts-tx5112,dwc-qos-ethernet-4.10", .data = &ts_tx5112_eqos_data },
	{ }
};
MODULE_DEVICE_TABLE(of, dwc_eth_dwmac_match);

static struct platform_driver dwc_eth_dwmac_driver = {
	.probe  = dwc_eth_dwmac_probe,
	.remove = dwc_eth_dwmac_remove,
	.driver = {
		.name           = "dwc-eth-dwmac",
		.pm             = &stmmac_pltfr_pm_ops,
		.of_match_table = dwc_eth_dwmac_match,
	},
};
module_platform_driver(dwc_eth_dwmac_driver);

MODULE_AUTHOR("Joao Pinto <jpinto@synopsys.com>");
MODULE_DESCRIPTION("Synopsys DWC Ethernet Quality-of-Service v4.10a driver");
MODULE_LICENSE("GPL v2");

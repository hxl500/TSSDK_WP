// SPDX-License-Identifier: GPL-2.0+
/*
 * (C) Copyright 2014 - 2019 Xilinx, Inc.
 * Michal Simek <michal.simek@xilinx.com>
 */

#include <common.h>
#include <env.h>
#include <log.h>
#include <asm/sections.h>
#include <asm/io.h>
#include <asm/arch/hardware.h>
#include <dm/uclass.h>
#include <i2c.h>
#include <linux/sizes.h>
#include "board.h"
#include <phy.h>


#define CRGU_BASE				0xF038A000UL  /* clock generating unit */
#define GMAC_CFG0				0xc4
#define GMAC_CFG1				0xc8
#define GMAC_CFG2				0xcc
#define TX5336_GMAC_RMII		0x4
#define TX5336_GMAC_RGMII		0x1
#define IOMUX_A_BASE			0xF0389000UL
#define IOMUX_B_BASE			0xF0389100UL
#define CHIP_OCLK1_CFG			0x12c

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

void cpu_secondary_init_r(void)
{
	printf("cpu_secondary_init_r bypass\n");
	return;
}

/* gmac: mac interface config and clk config, set mac speed */

int dev_eth_txclk_set_rate(int devid, u32 rate)
{
	u32 mac_mode;
	GMAC_CFG1_U val;

	mac_mode = readl(CRGU_BASE + GMAC_CFG0);
	val.u32 = readl(CRGU_BASE + GMAC_CFG1);
	/* disable the clock first */

	mac_mode = (mac_mode >> 12) & 0x7;
	if((TX5336_GMAC_RGMII != mac_mode) && (TX5336_GMAC_RMII != mac_mode)) {
		pr_err("[error]invalid mac_mode %d\n", mac_mode);
		return -EINVAL;
	}

	switch (rate) {
	case 125000000:
		if(TX5336_GMAC_RGMII == mac_mode) {
			val.pll0_d4_gmac_tx_clk_div_num = 0x3;
			val.pll0_d4_ephy_ref_clk_div_num = 0x3;
		} else {
			;;
		}
		break;
	case 25000000:
		if(TX5336_GMAC_RGMII == mac_mode) {
			val.pll0_d4_gmac_tx_clk_div_num = 0x13;
			val.pll0_d4_ephy_ref_clk_div_num = 0x13;
		} else if(TX5336_GMAC_RMII == mac_mode) {
			val.ref_rmii_txrx_clk_div_num = 1;
			val.pll0_d4_ephy_ref_clk_div_num = 0x9;//100Mbps
			val.pll0_d4_gmac_tx_clk_div_num = 0x13; //100Mbps
		} else {
			;;
		}
		break;
	case 2500000:
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
		return -EINVAL;
	}

	val.we_clock_en = 1;

	writel(val.u32, CRGU_BASE + GMAC_CFG1);

	if(TX5336_GMAC_RGMII == mac_mode) {
		writel(0x801f13, CRGU_BASE + GMAC_CFG2); //modify tx delay, add by lyy 20240620
	}

	return 0;
}
int dev_eth_start_clks_ts(int index,
			     phy_interface_t interface_type);

int dev_eth_interface_init(int devid,
			     phy_interface_t interface_type)
{
	u32 val = 0;
	u32 eth_mac_mode;

	switch (interface_type) {
	case PHY_INTERFACE_MODE_RMII:
		eth_mac_mode = TX5336_GMAC_RMII;
		writel(0x313115, CRGU_BASE + CHIP_OCLK1_CFG); //chipoutclk output 25MHZ
		debug("%s: PHY_INTERFACE_MODE_RMII\n", __func__);
		break;
	case PHY_INTERFACE_MODE_RGMII:
	case PHY_INTERFACE_MODE_RGMII_ID:
	case PHY_INTERFACE_MODE_RGMII_RXID:
	case PHY_INTERFACE_MODE_RGMII_TXID:
		eth_mac_mode = TX5336_GMAC_RGMII;
		debug("%s: PHY_INTERFACE_MODE_RGMII\n", __func__);
		break;
	default:
		debug("%s: Do not manage %d interface\n",
		      __func__, interface_type);
		/* Do not manage others interfaces */
		return -EINVAL;
	}
	val = (eth_mac_mode << 12);
	/*gmax axi clk pll0_d2/(4+ 1)*/
	val |= (0x4 << 8);
	/*gmac axi & ahb enable */
	val |= BIT(3) | BIT(2) | BIT(0);
	writel(val , CRGU_BASE + GMAC_CFG0);

	/*improve io driver strength*/
	writel(0xff, IOMUX_A_BASE + 0x4);
	writel(0x7f, IOMUX_B_BASE + 0x4);
	//writel(0xff, IOMUX_A_BASE + 0x8);
	//writel(0x7f, IOMUX_B_BASE + 0x8);

	return 0;
}

int dev_eth_start_clks_ts(int index,
			     phy_interface_t interface_type)
{
	GMAC_CFG1_U val = {0};

	switch (interface_type) {
	case PHY_INTERFACE_MODE_RMII:
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
		break;
	case PHY_INTERFACE_MODE_RGMII:
	case PHY_INTERFACE_MODE_RGMII_ID:
	case PHY_INTERFACE_MODE_RGMII_RXID:
	case PHY_INTERFACE_MODE_RGMII_TXID:
		val.we_clock_en = 1;
		val.gmac_rxclk_en = 1;
		val.gmac_ref_clk_sel = 0;
		val.rmii_clk_from_ephy_pad_en = 0;
		val.rmii_clk_from_pll_en = 0;
		val.ptp_ref_clk_en = 1;
		val.ephy_ref_clk_en =1;
		val.pll0_d4_gmac_tx_clk_en = 1;
		val.ref_rmii_txrx_clk_div_num = 1;
		val.pll0_d4_ephy_ref_clk_div_num = 0x13;//100Mbps
		val.pll0_d4_gmac_tx_clk_div_num = 0x13; //100Mbps
		break;
	default:
		debug("%s: Do not manage %d interface\n",
		      __func__, interface_type);
		/* Do not manage others interfaces */
		return -EINVAL;
	}
	writel(val.u32, CRGU_BASE + GMAC_CFG1);

	return 0;
}

void dev_eth_stop_clks_ts(int index,
			     phy_interface_t interface_type)
{
//	writel(0, CRGU_BASE + GMAC_CFG1);

	return;
}


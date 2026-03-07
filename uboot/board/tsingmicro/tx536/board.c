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

int board_late_init_xilinx(void)
{
	u32 ret = 0;
	phys_size_t bootm_size = gd->ram_size;

	if (CONFIG_IS_ENABLED(ARCH_ZYNQ))
		bootm_size = min(bootm_size, (phys_size_t)(SZ_512M + SZ_256M));

	ret |= env_set_hex("script_offset_f", CONFIG_BOOT_SCRIPT_OFFSET);

	ret |= env_set_addr("bootm_low", (void *)gd->ram_base);
	ret |= env_set_addr("bootm_size", (void *)bootm_size);

	if (ret)
		printf("%s: Saving run time variables FAILED\n", __func__);

	return 0;
}

#define _MCU_CRGU_MCU_BUS_CLK_CFG1 	(MCU_CLKG_RSTGG_BASE+0X24)
#define _MCU_CRGU_MCU_BUS_CLK_CFG2	(MCU_CLKG_RSTGG_BASE+0X28)
#define _MCU_CRGU_MCU_BUS_CLK_EN0	(MCU_CLKG_RSTGG_BASE+0X30)
#define _MCU_CRGU_MCU_BUS_CLK_EN1	(MCU_CLKG_RSTGG_BASE+0X34)

#define _CK804_SRAM_CODE_BADDR		0xF1230000
#define _CK804_SRAM_DATA_BADDR		0xF1240000

#define _SMU_CPU_BOOT_RST_CTL		(SMU_BASE+0X34)

#define CK804_BOOT_CFG0				0x400
#define CK804_BOOT_CFG1				0x404
#define CK804_BOOT_CFG2				0x408
#define CK804_BOOT_CFG3				0x40c
#define CK804_BOOT_CFG4				0x410

#define CGU_BASE					0xF0D9A000UL  /* clock generating unit */
#define GMAC_MAC_CLK_EN				0x140
#define GMAC_MAC_CLK_MODE			0x144
#define GMAC_CFG					0x34

#define GRF_BASE					0xF128C000UL

#define PADCTRL_BASE				0xF0D95000UL  /* pad ctrl unit */
#define GMII_TXCLK_IOCFG			0x10c
#define GMII_TXEN_IOCFG				0x110
#define GMII_TXD0_IOCFG				0x118
#define GMII_TXD1_IOCFG				0x11c
#define GMII_TXD2_IOCFG				0x120
#define GMII_TXD3_IOCFG				0x124

#define GMAC_EPHY_REF_CLK_EN		BIT(9)
#define GMAC_PTP_REF_CLK_EN			BIT(8)
#define GMAC_GMII_TX_CLK_EN			BIT(6)
#define GMAC_RMII_TX_CLK_EN			BIT(5)
#define GMAC_RMII_CLK_EN			BIT(4)
#define GMAC_MII_TXCLK_EN			BIT(1)
#define GMAC_MII_RXCLK_EN			BIT(0)

#define GMAC_TX_CLK_OE				BIT(13)

void cpu_secondary_init_r(void)
{
	u32 val;
	/* enable_mcu_ram_aclk_en_and_sys_reg_pclk_en */
	val = readl(_MCU_CRGU_MCU_BUS_CLK_EN1);
	writel(val | (1 << 0) | (1 << 4), _MCU_CRGU_MCU_BUS_CLK_EN1);

#if 0
	/* setting_itcm_dtcm_and_mask */
	writel(_CK804_SRAM_DATA_BADDR >> 12, SMU_BASE + CK804_DAHB_BASE);
	writel(0xFFFF0, SMU_BASE + CK804_DAHB_MASK);
	writel(_CK804_SRAM_CODE_BADDR >> 12, SMU_BASE + CK804_IAHB_BASE);
	writel(0xFFFF0, SMU_BASE + CK804_IAHB_MASK);
#endif

	/* reset ck804 */
	writel(1 << 0, _SMU_CPU_BOOT_RST_CTL);
	/* run always */
	//writel(SGRF_BASE + 0x404, SMU_BASE + CK804_BOOT_CFG0);
	/* release_reset_of_ck804 */
	writel(1 << 1, _SMU_CPU_BOOT_RST_CTL);

	//return;
}

/* gmac: mac interface config and clk config, set mac speed */

int dev_eth_txclk_set_rate(int devid, u32 rate)
{
	u32 val;
	int mac_speed;

	/* disable the clock first */

	switch (rate) {
	case 125000000:
		mac_speed = 0x2;
		break;
	case 25000000:
		mac_speed = 0x1;
		break;
	case 2500000:
		mac_speed = 0x0;
		break;
	default:
		return -EINVAL;
	}

	val = readl(CGU_BASE + GMAC_MAC_CLK_MODE);
	val &= ~(0x3 << 4);
	val |= (mac_speed << 4) | BIT(8);
	writel(val, CGU_BASE + GMAC_MAC_CLK_MODE);

	return 0;
}

int dev_eth_interface_init(int devid,
			     phy_interface_t interface_type)
{
	u32 val, i;
	int eth_mac_mode;
	int ephy_ifsel_mode;

	switch (interface_type) {
	case PHY_INTERFACE_MODE_MII:
		eth_mac_mode = 0x0;
		ephy_ifsel_mode = 0x0;
		break;
	case PHY_INTERFACE_MODE_GMII:
		eth_mac_mode = 0x2 | BIT(13);
		ephy_ifsel_mode = 0x0;
		debug("%s: PHY_INTERFACE_MODE_GMII\n", __func__);
		break;
	case PHY_INTERFACE_MODE_RMII:
		eth_mac_mode = 0x1;
		ephy_ifsel_mode = 0x4;
		debug("%s: PHY_INTERFACE_MODE_RMII\n", __func__);
		break;
	case PHY_INTERFACE_MODE_RGMII:
	case PHY_INTERFACE_MODE_RGMII_ID:
	case PHY_INTERFACE_MODE_RGMII_RXID:
	case PHY_INTERFACE_MODE_RGMII_TXID:
		eth_mac_mode = 0x3 | BIT(13);
		ephy_ifsel_mode = 0x1;
		debug("%s: PHY_INTERFACE_MODE_RGMII\n", __func__);
		break;
	default:
		debug("%s: Do not manage %d interface\n",
		      __func__, interface_type);
		/* Do not manage others interfaces */
		return -EINVAL;
	}

	val = readl(CGU_BASE + GMAC_MAC_CLK_MODE);
	val &= ~0x3;
	val |= eth_mac_mode;
	writel(val, CGU_BASE + GMAC_MAC_CLK_MODE);
	val = readl(CGU_BASE + GMAC_MAC_CLK_MODE);
	val |= BIT(8);
	writel(val, CGU_BASE + GMAC_MAC_CLK_MODE);

	/*ephy_ifsel cfg*/
	writel(ephy_ifsel_mode, GRF_BASE + GMAC_CFG);

	writel(0x4040, PADCTRL_BASE + GMII_TXCLK_IOCFG);
	writel(0x4040, PADCTRL_BASE + GMII_TXEN_IOCFG);
	writel(0x4040, PADCTRL_BASE + GMII_TXD0_IOCFG);
	writel(0x4040, PADCTRL_BASE + GMII_TXD1_IOCFG);
	writel(0x4040, PADCTRL_BASE + GMII_TXD2_IOCFG);
	writel(0x4040, PADCTRL_BASE + GMII_TXD3_IOCFG);

	for (i = 0; i < 27; i++)
		writel(0x4040, PADCTRL_BASE + 0x108 + i*4); //108 ~ 170


	return 0;
}

int dev_eth_start_clks_ts(int index,
			     phy_interface_t interface_type)
{
	u32 val;

	switch (interface_type) {
	case PHY_INTERFACE_MODE_MII:
		break;
	case PHY_INTERFACE_MODE_GMII:
		break;
	case PHY_INTERFACE_MODE_RMII:
		break;
	case PHY_INTERFACE_MODE_RGMII:
	case PHY_INTERFACE_MODE_RGMII_ID:
	case PHY_INTERFACE_MODE_RGMII_RXID:
	case PHY_INTERFACE_MODE_RGMII_TXID:
		val = GMAC_MII_RXCLK_EN | GMAC_GMII_TX_CLK_EN | GMAC_PTP_REF_CLK_EN;
		break;
	default:
		debug("%s: Do not manage %d interface\n",
		      __func__, interface_type);
		/* Do not manage others interfaces */
		return -EINVAL;
	}

	writel(val, CGU_BASE + GMAC_MAC_CLK_EN);

	return 0;
}

void dev_eth_stop_clks_ts(int index,
			     phy_interface_t interface_type)
{
	writel(0, CGU_BASE + GMAC_MAC_CLK_EN);

	return;
}


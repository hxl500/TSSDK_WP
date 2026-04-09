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
#include <env_internal.h>
#include <tx52xx.h>

#define CRGU_BASE				0xF1004000UL
#define GMAC_ACLK_CFG			0x098
#define GMAC_MAC_CLK_EN			0x0e0
#define GMAC_MAC_CLK_CFG		0x0e4
#define GMAC_TXCLK_DELAY		0x0e8
#define GMAC_RXCLK_DELAY		0x0ec

#define GMAC1_ACLK_CFG			0x398
#define GMAC1_MAC_CLK_EN		0x3e0
#define GMAC1_MAC_CLK_CFG		0x3e4
#define GMAC1_TXCLK_DELAY		0x3e8
#define GMAC1_RXCLK_DELAY		0x3ec

#define GMAC_EPHY_REF_CLK_EN	BIT(9)
#define GMAC_PTP_REF_CLK_EN		BIT(8)
#define GMAC_GMII_TX_CLK_EN		BIT(6)
#define GMAC_RMII_TX_CLK_EN		BIT(5)
#define GMAC_RMII_CLK_EN		BIT(4)
#define GMAC_MII_TXCLK_EN		BIT(1)
#define GMAC_MII_RXCLK_EN		BIT(0)

#define GMAC_SPEED_10M			0x0
#define GMAC_SPEED_100M			0x1
#define GMAC_SPEED_1G			0x2

#define SYS_BASE				0xF2440000UL

#define GMAC_MII				0x0
#define GMAC_RMII				0x1
#define GMAC_GMII				0x2
#define GMAC_RGMII				0x3

#define GMAC_CFG				0x4

void cpu_secondary_init_r(void)
{
	pr_debug("%s: bypass\n", __func__);
	//return;
}

int board_early_init_f(void)
{
	return 0;
}

static int multi_boot(void)
{
	pr_debug("Multiboot bypass\n");
	return 0;
}

int board_init(void)
{
#if (CONFIG_TS_HASH | CONFIG_TS_SKE | CONFIG_TS_PKE)
	int ret = 0;
	struct udevice *dev;
#endif

	pr_debug("EL Level:\tEL%d\n", 0); //current_el()

	multi_boot();

#if CONFIG_TS_HASH

	ret = uclass_get_device_by_driver(UCLASS_MISC, DM_GET_DRIVER(ts_hash),
						  &dev);
	if (ret)
		pr_err("%s get ts hash err %d\n", __func__, ret);

#endif

#if CONFIG_TS_SKE
	ret = uclass_get_device_by_driver(UCLASS_MISC,
					  DM_GET_DRIVER(ts_ske),
					  &dev);
	if (ret)
		pr_err("%s get ts ske err %d\n", __func__, ret);
#endif

#if CONFIG_TS_PKE
	ret = uclass_get_device_by_driver(UCLASS_MISC,
					  DM_GET_DRIVER(ts_pke),
					  &dev);
	if (ret)
		pr_err("%s get ts pke err %d\n", __func__, ret);
#endif
	return 0;
}

int board_early_init_r(void)
{
	printf("%s bypass\n", __func__);
	return 0;
}

#if !defined(CONFIG_SYS_SDRAM_BASE) && !defined(CONFIG_SYS_SDRAM_SIZE)
int dram_init_banksize(void)
{
	int ret;

	ret = fdtdec_setup_memory_banksize();
	if (ret)
		return ret;

	mem_map_fill();

	return 0;
}
int dram_init(void)
{
	if (fdtdec_setup_mem_size_base() != 0)
		return -EINVAL;

	return 0;
}
#else
int dram_init_banksize(void)
{
#if defined(CONFIG_NR_DRAM_BANKS)
	gd->bd->bi_dram[0].start = CONFIG_SYS_SDRAM_BASE;
	gd->bd->bi_dram[0].size = get_effective_memsize();
#endif

	mem_map_fill();

	return 0;
}

int dram_init(void)
{
	gd->ram_size = get_ram_size((void *)CONFIG_SYS_SDRAM_BASE,
				    CONFIG_SYS_SDRAM_SIZE);

	return 0;
}
#endif

void reset_cpu(ulong addr)
{
}

int checkboard(void)
{
	puts("Board: Tsingmicro board\n");
	return 0;
}

int dev_eth_txclk_set_rate(int devid, u32 rate)
{
	u32 val;
	int mac_speed;
	u32 offset;
	/* disable the clock first */
	if (devid == 1)
		offset = GMAC1_MAC_CLK_CFG;
	else
		offset = GMAC_MAC_CLK_CFG;

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

	val = readl(CRGU_BASE + offset);
	val &= ~(0x3 << 4);
	val |= (mac_speed << 4) | BIT(8);
	writel(val, CRGU_BASE + offset);

	return 0;
}

int dev_eth_clk_bus_init(int devid)
{
	if (devid == 0) {
		writel(0x3c1, CRGU_BASE + 0x98);
		setbits_32(CRGU_BASE + 0x9c, BIT(7));
		setbits_32(CRGU_BASE + 0xA0, BIT(6));
	} else if (devid == 1) {
		writel(0xfc1, CRGU_BASE + 0x398);
		setbits_32(CRGU_BASE + 0x9c, BIT(8));
		setbits_32(CRGU_BASE + 0xA0, BIT(9));
	} else {
		printf("error devid %d\r\n", devid);
		return -1;
	}

	return 0;
}
int dev_eth_interface_init(int devid,
			     phy_interface_t interface_type)
{
	u32 val;
	u32 eth_mac_mode;
	u32 ephy_ifsel_mode;
	u32 offset;

	dev_eth_clk_bus_init(devid);

	if (devid == 1)
		offset = GMAC1_MAC_CLK_CFG;
	else
		offset = GMAC_MAC_CLK_CFG;

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
		eth_mac_mode = 0x1 | BIT(8) | BIT(13) | (0x1 << 16) | BIT(19);
		ephy_ifsel_mode = 0x4;
		debug("%s: PHY_INTERFACE_MODE_RMII\n", __func__);
		break;
	case PHY_INTERFACE_MODE_RGMII:
	case PHY_INTERFACE_MODE_RGMII_ID:
	case PHY_INTERFACE_MODE_RGMII_RXID:
	case PHY_INTERFACE_MODE_RGMII_TXID:
		eth_mac_mode = 0x3 | BIT(13) | BIT(8);
		ephy_ifsel_mode = 0x1;
		debug("%s: PHY_INTERFACE_MODE_RGMII\n", __func__);
		break;
	default:
		debug("%s: Do not manage %d interface\n",
		      __func__, interface_type);
		/* Do not manage others interfaces */
		return -EINVAL;
	}

	writel(eth_mac_mode, CRGU_BASE + offset);

	/*ephy_ifsel cfg*/
	val = readl(SYS_BASE + GMAC_CFG);
	if (devid == 0) {
		val &= ~0x3;
	} else if (devid == 1) {
		ephy_ifsel_mode <<= 4;
		val &= ~(0x3 << 4);
	} else {
		debug("error:%s: devid 0x%x\n", __func__, devid);
		return -EINVAL;
	}
	val |= ephy_ifsel_mode;
	writel(val, SYS_BASE + GMAC_CFG);

#if defined(CONFIG_PHY_MOTORCOMM_NOXTAL) || defined(PHY_MOTORCOMM_RMII2)
	offset = (devid == 0) ? GMAC_ACLK_CFG : GMAC1_ACLK_CFG;
	val = readl(CRGU_BASE + offset);
	val |= (0x3 << 10);//ephy_clk use pad not pll0
	writel(val, CRGU_BASE + offset);
#endif

	return 0;
}

int dev_eth_start_clks_ts(int index,
			     phy_interface_t interface_type)
{
	u32 val;
	u32 offset;

	if (index == 1)
		offset = GMAC1_MAC_CLK_EN;
	else
		offset = GMAC_MAC_CLK_EN;

	switch (interface_type) {
	case PHY_INTERFACE_MODE_MII:
		break;
	case PHY_INTERFACE_MODE_GMII:
		break;
	case PHY_INTERFACE_MODE_RMII:
		val = GMAC_RMII_CLK_EN | GMAC_RMII_TX_CLK_EN | GMAC_PTP_REF_CLK_EN | GMAC_EPHY_REF_CLK_EN;
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

	writel(val, CRGU_BASE + offset);

	return 0;
}

void dev_eth_stop_clks_ts(int index,
			     phy_interface_t interface_type)
{
#if 0
	u32 offset;

	if (index == 1)
		offset = GMAC1_MAC_CLK_EN;
	else
		offset = GMAC_MAC_CLK_EN;

	writel(0, CRGU_BASE + offset);
#endif
	//return;
}

enum boot_sel ts_get_boot_sel(void)
{
	return (uint8_t)(readl(SMU_CHIP_STATUS) & BOOT_MODE_MASK);
}

int boot_select_is_emmc(void)
{
	uint32_t boot_sel = ts_get_boot_sel();

	switch (boot_sel) {

	case BOOT_SEL_QSPI_FLASH_18:
	case BOOT_SEL_QSPI_FLASH_33:
	case BOOT_SEL_QSPI_FLASH_1L_XIP:
	case BOOT_SEL_QSPI_FLASH_4L_XIP:
	case BOOT_SEL_QSPI_NAND_18:
	case BOOT_SEL_QSPI_NAND_33:
	break;

	case BOOT_SEL_EMMC:
		return 1;
	break;

	case BOOT_SEL_UART:
	break;
	}

	return 0;
}


int boot_select_is_ospi(void)
{
	uint32_t boot_sel = ts_get_boot_sel();

	switch (boot_sel) {

	case BOOT_SEL_QSPI_FLASH_18:
	case BOOT_SEL_QSPI_FLASH_33:
	case BOOT_SEL_QSPI_FLASH_1L_XIP:
	case BOOT_SEL_QSPI_FLASH_4L_XIP:
	case BOOT_SEL_QSPI_NAND_18:
	case BOOT_SEL_QSPI_NAND_33:
		return 1;
	break;

	case BOOT_SEL_EMMC:
	break;

	case BOOT_SEL_UART:
	break;
	}

	return 0;
}



enum env_location env_get_location(enum env_operation op, int prio)
{
	uint32_t boot_sel = ts_get_boot_sel();

	if (prio)
		return ENVL_UNKNOWN;

	switch (boot_sel) {

	case BOOT_SEL_QSPI_FLASH_18:
	case BOOT_SEL_QSPI_FLASH_33:
	case BOOT_SEL_QSPI_FLASH_1L_XIP:
	case BOOT_SEL_QSPI_FLASH_4L_XIP:
	if (CONFIG_IS_ENABLED(ENV_IS_IN_SPI_FLASH))
		return ENVL_SPI_FLASH;
	else
		return ENVL_NOWHERE;
	break;

	case BOOT_SEL_QSPI_NAND_18:
	case BOOT_SEL_QSPI_NAND_33:
	if (CONFIG_IS_ENABLED(ENV_IS_IN_UBI))
		return ENVL_UBI;
	else
		return ENVL_NAND;
		//return ENVL_NOWHERE; //todo...
	break;

	case BOOT_SEL_EMMC:
	if (CONFIG_IS_ENABLED(ENV_IS_IN_MMC))
		return ENVL_MMC;
	else if (CONFIG_IS_ENABLED(ENV_IS_IN_EXT4))
		return ENVL_EXT4;
	else
		return ENVL_NOWHERE;

	break;

	case BOOT_SEL_UART:
	default:
		return ENVL_NOWHERE;
	break;
	}

	return ENVL_NOWHERE;
}

#if 0
int mmc_get_env_dev(void)
{
	uint32_t boot_sel = ts_get_boot_sel();

	switch (boot_sel) {
	case BOOT_SEL_EMMC:
		return 0;
	break;

	case BOOT_SEL_QSPI_FLASH_18:
	case BOOT_SEL_QSPI_FLASH_33:
	case BOOT_SEL_QSPI_FLASH_1L_XIP:
	case BOOT_SEL_QSPI_FLASH_4L_XIP:
	case BOOT_SEL_QSPI_NAND_18:
	case BOOT_SEL_QSPI_NAND_33:
	case BOOT_SEL_UART:
	default:
		return CONFIG_SYS_MMC_ENV_DEV;
	break;
	}

	return CONFIG_SYS_MMC_ENV_DEV;
}
#endif

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
#include <tx51xx.h>
#include <dma-uclass.h>
#include <linux/io.h>

#ifdef CONFIG_GMAC_PHY_SPI_CLK_INIT
#define GMAC_PHY_10M_INIT_NEED          1
#else
#define GMAC_PHY_10M_INIT_NEED          0
#endif


#define CRGU_BASE				0xF1004000UL
#define IOMUX_BASE				0xF0000000UL
#define IOMUX_PUR				0x510
#define IOMUX_DOWN				0x514
#define IOMUX_FUNC				0x52c
#define BUS_CLK_AXI_EN			0x048
#define BUS_CLK_AHB_EN			0x04C
#define GMAC_ACLK_CFG			0x070
#define GMAC_MAC_CLK			0x074
#define GMAC_TXCLK_DELAY		0x078
#define GMAC_RXCLK_DELAY		0x07c

#define SPI_SSICLK_CFG			0x0a0

#define GMAC_MAC_SPEED_UPD		BIT(15)
#define GMAC_TX_CLK_OE			BIT(14)
#define GMAC_EPHY_REF_CLK_EN	BIT(8)
#define GMAC_PTP_REF_CLK_EN		BIT(7)
#define GMAC_GMII_TX_CLK_EN		BIT(6)
#define GMAC_RMII_TX_CLK_EN		BIT(5)
#define GMAC_RMII_CLK_EN		BIT(4)
#define GMAC_MII_TXCLK_EN		BIT(3)
#define GMAC_MII_RXCLK_EN		BIT(2)
#define GMAC_WE_GMAC_CLK		BIT(0)

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
#if (CONFIG_TS_SKE)
	int ret = 0;
	struct udevice *dev;
#endif

	pr_debug("EL Level:\tEL%d\n", 0); //current_el()

	multi_boot();

#if CONFIG_TS_SKE
	ret = uclass_get_device_by_driver(UCLASS_MISC,
					  DM_GET_DRIVER(ts_ske),
					  &dev);
	if (ret)
		pr_err("%s get ts ske err %d\n", __func__, ret);
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
} dw_ahb_dma_lli;

dw_ahb_dma_lli *g_dma_lli;
static void ts_dma_init(void __iomem *dma_base)
{
	int i;
	dw_ahb_dma_lli *last_lli = NULL;

	writel((0x1 << 5), dma_base + 0x98);
	writel((0x1 << 1) | (0x1 << 11), dma_base + 0x98 + 0x4);

	g_dma_lli = (dw_ahb_dma_lli *)ioremap(0xF4000100UL, 0x100);
	for (i = 0; i < 3; i++) {
		g_dma_lli[i].sar = 0xF4000000UL + i * 0x10;
		g_dma_lli[i].dar = 0xf244d060;
		g_dma_lli[i].llp = (u32)(0xF4000100 + (i+1)*sizeof(dw_ahb_dma_lli)) | 0x1;
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

	last_lli->llp = (u32)0xF4000100 | 0x1;
#if 0
	pr_err("-------- last --------\r\n");
	pr_err("sar_lo		= 0x%x\r\n", last_lli->sar);
	pr_err("dar_lo		= 0x%x\r\n", last_lli->dar);
	pr_err("llp_lo		= 0x%x\r\n", last_lli->llp);
	pr_err("ctl_lo		= 0x%x\r\n", last_lli->ctllo);
	pr_err("ctl_hi		= 0x%x\r\n", last_lli->ctlhi);
	pr_err("---------22-------\r\n");
#endif
	writel((0x1), dma_base + 0x398);
	writel(((u32)0xF4000100 | 0x1), dma_base + 0x68);

	writel((1 << 27) | (1 << 28), dma_base + 0x70);
	writel(0, dma_base + 0x74);

	writel((0x1 << 9) | (0x1 << 1), dma_base + 0x3a0);
}

static void ts_spi_init(void __iomem *spi_base)
{
	u32 val;

	val = readl(CRGU_BASE + SPI_SSICLK_CFG);
	val &= ~(0x1f << 16);
	val |= (0x4 << 16) | (1 << 8) | (1 << 3) | BIT(0);
	writel(val, CRGU_BASE + SPI_SSICLK_CFG);
	writel(val, CRGU_BASE + SPI_SSICLK_CFG);

	writel((0x7 << 16) | (0x0 << 0) | (0x1 << 8), spi_base + 0x0);
	writel((0x8 << 0), spi_base + 0x14);

	writel((0x1 << 1),  spi_base + 0x4c);
	writel((0x10 << 0), spi_base + 0x50);
	writel((0xf << 0), spi_base + 0x54);

	writel((0x1 << 0), spi_base + 0x10);
	writel((0x1 << 0),  spi_base + 0x8);
}

int spi_dma_clk(void)
{
	void __iomem *dma_base = NULL;
	void __iomem *spi_base = NULL;

	dma_base = ioremap(0xf2100000, 0x1000);
	if (!dma_base) {
		printf("dma_base is null\r\n");
		return -ENODEV;
	}

	spi_base = ioremap(0xf244d000, 0x1000);
	if (!spi_base) {
		printf("spi_base is null\r\n");
		return -ENODEV;
	}

	//printf("dma_base = 0x%p, spi_base = 0x%p\r\n", dma_base, spi_base);
	ts_dma_init(dma_base);
	ts_spi_init(spi_base);

	return 0;
}

int dev_eth_txclk_set_rate(int devid, u32 rate)
{
	u32 val;
	int mac_speed;

	if (devid > 0)
		return -1;
	/* disable the clock first */
	switch (rate) {
	case 125000000:
		mac_speed = 0x2;
		break;
	case 25000000:
		mac_speed = 0x1;
	/*mac_txclk -> spi_clk*/
	#if GMAC_PHY_10M_INIT_NEED
		val = readl(IOMUX_BASE + IOMUX_FUNC);
		val &= 0xffffff0f;
		writel(val, IOMUX_BASE + IOMUX_FUNC);
	#endif
		break;
	case 2500000:
		mac_speed = 0x0;//bug
	/*mac_txclk -> spi_clk*/
	#if GMAC_PHY_10M_INIT_NEED
		val = readl(IOMUX_BASE + IOMUX_FUNC);
		val &= 0xffffff0f;
		val |= 0x50;
		writel(val, IOMUX_BASE + IOMUX_FUNC);
	#endif
		break;
	default:
		return -EINVAL;
	}

	val = readl(CRGU_BASE + GMAC_MAC_CLK);
	val &= ~(0x3 << 11);
	val |= (mac_speed << 11) | GMAC_MAC_SPEED_UPD;

	writel(val, CRGU_BASE + GMAC_MAC_CLK);
	writel(val, CRGU_BASE + GMAC_MAC_CLK);

	return 0;
}

int dev_eth_clk_bus_init(int devid)
{
	u32 val;

	setbits_32(CRGU_BASE + BUS_CLK_AXI_EN, BIT(5) | BIT(0));
	setbits_32(CRGU_BASE + BUS_CLK_AXI_EN, BIT(5) | BIT(0));
	setbits_32(CRGU_BASE + BUS_CLK_AHB_EN, BIT(8) | BIT(0));
	setbits_32(CRGU_BASE + BUS_CLK_AHB_EN, BIT(8) | BIT(0));

	writel((0x1 << 16) | BIT(8) | BIT(0), CRGU_BASE + GMAC_ACLK_CFG);
	writel((0x1 << 16) | BIT(8) | BIT(0), CRGU_BASE + GMAC_ACLK_CFG);

	val = readl(CRGU_BASE + 0xf4);
	val &= ~(0xf << 20);
	val |= (0x8 << 20) | BIT(3) | BIT(0);
	writel(val, CRGU_BASE + 0xf4);

	return 0;
}

int dev_eth_interface_init(int devid,
			     phy_interface_t interface_type)
{
	u32 val;
	int ephy_ifsel_mode;

	if (devid > 0)
		return -1;

	dev_eth_clk_bus_init(devid);

	switch (interface_type) {
	case PHY_INTERFACE_MODE_MII:
		ephy_ifsel_mode = 0x0;
		break;
	case PHY_INTERFACE_MODE_GMII:
		ephy_ifsel_mode = 0x0;
		debug("%s: PHY_INTERFACE_MODE_GMII\n", __func__);
		break;
	case PHY_INTERFACE_MODE_RMII:
		/*kernel phy clk need*/
		val = GMAC_MAC_SPEED_UPD | GMAC_WE_GMAC_CLK | (0x1 << 9) | GMAC_GMII_TX_CLK_EN | GMAC_RMII_TX_CLK_EN \
		| GMAC_TX_CLK_OE | GMAC_RMII_CLK_EN | GMAC_EPHY_REF_CLK_EN | GMAC_PTP_REF_CLK_EN;

		writel(val, CRGU_BASE + GMAC_MAC_CLK);
		writel(val, CRGU_BASE + GMAC_MAC_CLK);
		dev_eth_txclk_set_rate(devid, 25000000);//default 100m provide 25MHZ to phy
		ephy_ifsel_mode = 0x4;
		debug("%s: PHY_INTERFACE_MODE_RMII\n", __func__);
		break;
	case PHY_INTERFACE_MODE_RGMII:
	case PHY_INTERFACE_MODE_RGMII_ID:
	case PHY_INTERFACE_MODE_RGMII_RXID:
	case PHY_INTERFACE_MODE_RGMII_TXID:
		ephy_ifsel_mode = 0x1;
		debug("%s: PHY_INTERFACE_MODE_RGMII\n", __func__);
		break;
	default:
		debug("%s: Do not manage %d interface\n",
		      __func__, interface_type);
		/* Do not manage others interfaces */
		return -EINVAL;
	}

	val = readl(CRGU_BASE + GMAC_ACLK_CFG);
	val &= ~(0x1 << 10);
	val |= (1 << 10);
	writel(val | BIT(0), CRGU_BASE + GMAC_ACLK_CFG);

	//mac_rxdv pull down
	val = readl(IOMUX_BASE + IOMUX_PUR);
	val &= ~(0x1 << 5);
	writel(val, IOMUX_BASE + IOMUX_PUR);
	val = readl(IOMUX_BASE + IOMUX_DOWN);
	val |= (0x1 << 5);
	writel(val | BIT(0), IOMUX_BASE + IOMUX_DOWN);

	/*ephy_ifsel cfg*/
	writel(ephy_ifsel_mode, SYS_BASE + GMAC_CFG);

#if	GMAC_PHY_10M_INIT_NEED
	printf("spi dma clk init\r\n");
	spi_dma_clk();
#if 0
	val = readl(IOMUX_BASE + IOMUX_FUNC);
	val &= 0xffffff0f;
	val |= 0x50;
	writel(val, IOMUX_BASE + IOMUX_FUNC);
#endif
#endif
	return 0;
}

int dev_eth_start_clks_ts(int devid,
			     phy_interface_t interface_type)
{
	u32 val;

	if (devid > 0)
		return -1;

	val = readl(CRGU_BASE + GMAC_MAC_CLK);
	val &= ~(0x1 << 5);

	switch (interface_type) {
	case PHY_INTERFACE_MODE_MII:
		break;
	case PHY_INTERFACE_MODE_GMII:
		break;
	case PHY_INTERFACE_MODE_RMII:
		val |=  GMAC_RMII_TX_CLK_EN;
		break;
	case PHY_INTERFACE_MODE_RGMII:
	case PHY_INTERFACE_MODE_RGMII_ID:
	case PHY_INTERFACE_MODE_RGMII_RXID:
	case PHY_INTERFACE_MODE_RGMII_TXID:
		val |= GMAC_MII_RXCLK_EN | GMAC_GMII_TX_CLK_EN \
			| GMAC_EPHY_REF_CLK_EN | GMAC_TX_CLK_OE;
		break;
	default:
		debug("%s: Do not manage %d interface\n",
		      __func__, interface_type);
		/* Do not manage others interfaces */
		return -EINVAL;
	}

	writel(val | GMAC_WE_GMAC_CLK | GMAC_MAC_SPEED_UPD, CRGU_BASE + GMAC_MAC_CLK);
	writel(val | GMAC_WE_GMAC_CLK | GMAC_MAC_SPEED_UPD, CRGU_BASE + GMAC_MAC_CLK);
	return 0;
}

void dev_eth_stop_clks_ts(int devid,
			     phy_interface_t interface_type)
{
	if (devid > 0)
		return;
//	writel(0, CRGU_BASE + GMAC_MAC_CLK);
//	return;
}

void dev_eth_fix_clk(int devid)
{
	u32 val = readl(CRGU_BASE + GMAC_MAC_CLK);
	writel(val | GMAC_MAC_SPEED_UPD, CRGU_BASE + GMAC_MAC_CLK);
	printf("update eth eqos clk\r\n");
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

	//return 0;
	return 2; //alwys init mmc
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

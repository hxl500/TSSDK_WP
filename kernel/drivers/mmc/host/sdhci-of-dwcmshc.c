// SPDX-License-Identifier: GPL-2.0
/*
 * Driver for Synopsys DesignWare Cores Mobile Storage Host Controller
 *
 * Copyright (C) 2018 Synaptics Incorporated
 *
 * Author: Jisheng Zhang <jszhang@kernel.org>
 */

#include <linux/clk.h>
#include <linux/dma-mapping.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/sizes.h>
#include <linux/clk-provider.h>
#include <linux/delay.h>
#include <linux/gpio.h>
#include <linux/gpio/consumer.h>
#include <linux/preempt.h>
#include <asm/io_lock.h>
#include "sdhci-pltfm.h"

/* phy reg */
#define DW_PHY_CNFG			0x300
#define DW_PHY_CNFG_SN_MASK		0x00F00000
#define DW_PHY_CNFG_SN_SHITF	20
#define DW_PHY_CNFG_SP_MASK		0x000F0000
#define DW_PHY_CNFG_SP_SHITF	16
#define DW_PHY_CNFG_PWRGOOD		BIT(1)
#define DW_PHY_CNFG_RSTN		BIT(0)

#define DW_PHY_CMDPAD_CNFG		0x304
#define DW_PHY_DATAPAD_CNFG		0x306
#define DW_PHY_CLKPAD_CNFG		0x308
#define DW_PHY_STBPAD_CNFG		0x30A
#define DW_PHY_RSTNPAD_CNFG		0x30C
#define DW_PHY_PAD_TXSLEW_N_MASK	0x1E00
#define DW_PHY_PAD_TXSLEW_N_SHIFT	9
#define DW_PHY_PAD_TXSLEW_P_MASK	0x01E0
#define DW_PHY_PAD_TXSLEW_P_SHIFT	5
#define DW_PHY_PAD_WEEKPULL_MASK	0x18
#define DW_PHY_PAD_WEEKPULL_SHIFT	3
#define DW_PHY_PAD_RXSEL_MASK		0x7
#define DW_PHY_PAD_RXSEL_SHIFT		0

#define DW_PHY_PAD_BEST_TXSLEW_N_33	(3)
#define DW_PHY_PAD_BEST_TXSLEW_P_33	(3)
#define DW_PHY_PAD_BEST_TXSLEW_N_18	(3)
#define DW_PHY_PAD_BEST_TXSLEW_P_18	(3)

#define DW_PHY_PULL_OFF			(0)
#define DW_PHY_PULL_UP			(1)
#define DW_PHY_PULL_DOWN		(2)

#define DW_PHY_PAD_SN_50_OHM_33		(8)
#define DW_PHY_PAD_SP_50_OHM_33		(9)
#define DW_PHY_PAD_SN_50_OHM_18		(8)
#define DW_PHY_PAD_SP_50_OHM_18		(8)

#define DW_PHY_COMMDL_CNFG			0x31C
#define DW_PHY_SDCLKDL_CNFG			0x31D
#define DW_PHY_SDCLKDL_UPDATE_DC		BIT(4)
#define	DW_PHY_SDCLKDL_INSRC_CCLK_TX		0
#define	DW_PHY_SDCLKDL_INSRC_CCLK_TX_SHIFT	2
#define	DW_PHY_SDCLKDL_BYPASS_EN		BIT(1)
#define	DW_PHY_SDCLKDL_EXTDLY_EN		BIT(0)
#define DW_PHY_SDCLKDL_EXTDLY_EN_SHITF		0

#define DW_PHY_SDCLKDL_DC			0x31E
#define DW_PHY_SMPLDL_CNFG			0x320
#define DW_PHY_SMPLDL_IN_OV			BIT(4)
#define DW_PHY_SMPLDL_IN_OV_SHITF		4
#define	DW_PHY_SMPLDL_INSRC_DL1OUT		2
#define	DW_PHY_SMPLDL_INSRC_CCLK_TX_SHIFT	2
#define	DW_PHY_SMPLDL_BYPASS_EN			BIT(1)
#define	DW_PHY_SMPLDL_EXTDLY_EN			BIT(0)

#define DW_PHY_EXTDLY_EN		1
#define DW_PHY_EXTDLY_DIS		0
#define DW_PHY_SMPL_INSEL_OV		1	/* use SMLPDL_CNFG.INPSEL_CNFG */
#define DW_PHY_SMPL_INSEL_OV_HOST	0	/* controller diver config */

#define AT_CTRL_R			0x540
#define AT_CTRL_SW_TUNE_EN		BIT(4)
#define AT_STAT_R			0x544
#define AT_STAT_CENTER_PH_CODE(code)	((code) & 0xFF)

struct dw_phy_cfg {
	u32 valid:1;

	/* delay line, 22 bits */
	u32 SDCLKDL_CNFG_INSEL:2;
	u32 SDCLKDL_CNFG_EXTDLY_EN:1;
	u32 SDCLKDL_DC:7;
	u32 SMPLDL_CNFG_INSEL_OV:1;
	u32 SMPLDL_CNFG_INSEL:2;
	u32 SMPLDL_CNFG_EXTDLY_EN:1;
	u32 AT_STAT_CENTER_PH_CODE:8; /* not use */

	/* pad, 26 bits */
	u32 PAD_SN:4;
	u32 PAD_SP:4;
	u32 TXSLEW_CTRL_N:4;
	u32 TXSLEW_CTRL_P:4;
	u32 WEAKPULL_EN:2;
	u32 WEAKPULL_EN_CLK:2;
	u32 RXSEL:3;
	u32 RXSEL_CLK:3;
};

enum DW_PHY_RCV_TYPE {
	DW_PHY_RCV_TYPE_OFF = 0,
	DW_PHY_RCV_TYPE_18_SCHMITT,
	DW_PHY_RCV_TYPE_33_SCHMITT,
	DW_PHY_RCV_TYPE_12_SCHMITT,
	DW_PHY_RCV_TYPE_18_COMPATATOR_05,			/* Vth = 0.5*VDDIO */
	DW_PHY_RCV_TYPE_12_COMPATATOR,
	DW_PHY_RCV_TYPE_18_COMPATATOR_0527_0472,	/* VIH = 0.527*VDDIO; VIL = 0.472*VDDIO */
	DW_PHY_RCV_TYPE_INTER_LP
};

#define HOST_CNT_MAX                   2
#define SNPS_MMC_MAX_BLK_COUNT         4 /* read/write maximum 4 blocks once */

/* DWCMSHC specific Mode Select value */
#define DWCMSHC_CTRL_HS400		0x7

#define BOUNDARY_OK(addr, len) \
	((addr | (SZ_128M - 1)) == ((addr + len - 1) | (SZ_128M - 1)))

struct dwcmshc_priv {
	struct clk	*bus_clk;
};

unsigned int g_dev_cnt;  /* default to 0 */
struct sdhci_host *g_dwc_host[HOST_CNT_MAX] = {NULL, NULL};

/*
 * If DMA addr spans 128MB boundary, we split the DMA transfer into two
 * so that each DMA transfer doesn't exceed the boundary.
 */
static void dwcmshc_adma_write_desc(struct sdhci_host *host, void *desc,
				    dma_addr_t addr, int len, unsigned int cmd)
{
	int tmplen, offset;

	if (likely(!len || BOUNDARY_OK(addr, len))) {
		sdhci_adma_write_desc(host, desc, addr, len, cmd);
		return;
	}

	offset = addr & (SZ_128M - 1);
	tmplen = SZ_128M - offset;
	sdhci_adma_write_desc(host, desc, addr, tmplen, cmd);

	addr += tmplen;
	len -= tmplen;
	sdhci_adma_write_desc(host, desc, addr, len, cmd);
}

static void dwcmshc_set_uhs_signaling(struct sdhci_host *host,
				      unsigned int timing)
{
	u16 ctrl_2;

	ctrl_2 = sdhci_readw(host, SDHCI_HOST_CONTROL2);
	/* Select Bus Speed Mode for host */
	ctrl_2 &= ~SDHCI_CTRL_UHS_MASK;
	if ((timing == MMC_TIMING_MMC_HS200) ||
	    (timing == MMC_TIMING_UHS_SDR104))
		ctrl_2 |= SDHCI_CTRL_UHS_SDR104;
	else if (timing == MMC_TIMING_UHS_SDR12)
		ctrl_2 |= SDHCI_CTRL_UHS_SDR12;
	else if ((timing == MMC_TIMING_UHS_SDR25) ||
		 (timing == MMC_TIMING_MMC_HS))
		ctrl_2 |= SDHCI_CTRL_UHS_SDR25;
	else if (timing == MMC_TIMING_UHS_SDR50)
		ctrl_2 |= SDHCI_CTRL_UHS_SDR50;
	else if ((timing == MMC_TIMING_UHS_DDR50) ||
		 (timing == MMC_TIMING_MMC_DDR52))
		ctrl_2 |= SDHCI_CTRL_UHS_DDR50;
	else if (timing == MMC_TIMING_MMC_HS400)
		ctrl_2 |= DWCMSHC_CTRL_HS400;

	sdhci_writew(host, ctrl_2, SDHCI_HOST_CONTROL2);
}

#ifdef CONFIG_ARCH_TS_TX536
static unsigned int sdhci_rw1;
static unsigned int sdhci_rw2;
module_param(sdhci_rw1, uint, 0644);
module_param(sdhci_rw2, uint, 0644);

static inline void dwcmshc_writel(struct sdhci_host *host, u32 val, int reg)
{
	if (in_irq()) {
		sdhci_rw1++;
		writel_lock(val, host->ioaddr + reg);
	} else {
		sdhci_rw2++;
		writel_lock_irqsave(val, host->ioaddr + reg);
	}
}

static inline void dwcmshc_writew(struct sdhci_host *host, u16 val, int reg)
{
	if (in_irq()) {
		sdhci_rw1++;
		writew_lock(val, host->ioaddr + reg);
	} else {
		sdhci_rw2++;
		writew_lock_irqsave(val, host->ioaddr + reg);
	}
}

static inline void dwcmshc_writeb(struct sdhci_host *host, u8 val, int reg)
{
	if (in_irq()) {
		sdhci_rw1++;
		writeb_lock(val, host->ioaddr + reg);
	} else {
		sdhci_rw2++;
		writeb_lock_irqsave(val, host->ioaddr + reg);
	}
}

static inline u32 dwcmshc_readl(struct sdhci_host *host, int reg)
{
	if (in_irq()) {
		sdhci_rw1++;
		return readl_lock(host->ioaddr + reg);
	} else {
		sdhci_rw2++;
		return readl_lock_irqsave(host->ioaddr + reg);
	}
}

static inline u16 dwcmshc_readw(struct sdhci_host *host, int reg)
{
	if (in_irq()) {
		sdhci_rw1++;
		return readw_lock(host->ioaddr + reg);
	} else {
		sdhci_rw2++;
		return readw_lock_irqsave(host->ioaddr + reg);
	}
}

static inline u8 dwcmshc_readb(struct sdhci_host *host, int reg)
{
	if (in_irq()) {
		sdhci_rw1++;
		return readb_lock(host->ioaddr + reg);
	} else {
		sdhci_rw2++;
		return readb_lock_irqsave(host->ioaddr + reg);
	}
}
#endif
static const struct sdhci_ops sdhci_dwcmshc_ops = {
#ifdef CONFIG_ARCH_TS_TX536
	.read_l = dwcmshc_readl,
	.read_w = dwcmshc_readw,
	.read_b = dwcmshc_readb,
	.write_l = dwcmshc_writel,
	.write_w = dwcmshc_writew,
	.write_b = dwcmshc_writeb,
#endif
	.set_clock		= sdhci_set_clock,
	.set_bus_width		= sdhci_set_bus_width,
	.set_uhs_signaling	= dwcmshc_set_uhs_signaling,
	.get_max_clock		= sdhci_pltfm_clk_get_max_clock,
	.reset			= sdhci_reset,
	.adma_write_desc	= dwcmshc_adma_write_desc,
};

static const struct sdhci_pltfm_data sdhci_dwcmshc_pdata = {
	.ops = &sdhci_dwcmshc_ops,
	.quirks = SDHCI_QUIRK_CAP_CLOCK_BASE_BROKEN,
};

static ssize_t dwcmshc_dbg_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	struct sdhci_host *host = NULL;
	static int host_index;

	if (g_dev_cnt == 0) {
		pr_err("g_dev_cnt is 0\n");
		return 0;
	}

	if (host_index > HOST_CNT_MAX) {
		pr_err("invalid host index %d\n", host_index);
		return 0;
	}

	host = g_dwc_host[host_index];
	if (host == NULL)
		pr_err("host%d is NULL\n", host_index);
	else
		sdhci_dumpregs(host);

	host_index = (host_index + 1) % g_dev_cnt;
	return 0;
}
static ssize_t dwcmshc_dbg_store(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t count)
{
	return count;
}

static DEVICE_ATTR(dwcmshcdbg, S_IRUGO | S_IWUSR,
	dwcmshc_dbg_show, dwcmshc_dbg_store);

static struct attribute *dwcmshcdbg_attrs[] = {
	&dev_attr_dwcmshcdbg.attr,
	NULL,
};

static struct attribute_group dwcmshcdbg_attr_group = {
	.attrs = dwcmshcdbg_attrs,
};
struct kobject *dwcmshc_node_device ;  /* default to NULL */

static int dwcmshc_create_sysfs(struct sdhci_host *host)
{
	int ret;

	if (g_dev_cnt < HOST_CNT_MAX) {
		g_dwc_host[g_dev_cnt] = host;
		g_dev_cnt++;
	}

	if (dwcmshc_node_device != NULL)
		return 0;

	dwcmshc_node_device = kobject_create_and_add("dwc_mshc", NULL);
	if (dwcmshc_node_device == NULL) {
		pr_err("create sysfs node failed\n");
		return -ENOMEM;
	}

	ret = sysfs_create_group(dwcmshc_node_device, &dwcmshcdbg_attr_group);
	if (ret) {
		pr_err("sysfs_create_group failed, ret %d\n", ret);
		kobject_put(dwcmshc_node_device);
		dwcmshc_node_device = NULL;
	}

	return ret;
}

static void dwcmshc_remove_sysfs(void)
{
	if (dwcmshc_node_device != NULL) {
		sysfs_remove_group(dwcmshc_node_device, &dwcmshcdbg_attr_group);
		kobject_put(dwcmshc_node_device);
		dwcmshc_node_device = NULL;
	}
}

#ifdef CONFIG_ARCH_TS
void sdhci_phy_init(struct sdhci_host *host)
{
	struct dw_phy_cfg tmpcfg = {0};
	struct dw_phy_cfg *cfg = &tmpcfg;
	u32 retry = 1000;
	u32 phy_cnfg;
	u16 pad_cfg;
	u8 sdclkdl_cnfg, sdclkdl_dc, smpldl_cnfg;

	/* process phy config data */
	if (host->dev_type == MMC_IS_EMMC) {
		cfg->SDCLKDL_CNFG_INSEL = DW_PHY_SDCLKDL_INSRC_CCLK_TX;
		cfg->SDCLKDL_CNFG_EXTDLY_EN = DW_PHY_EXTDLY_EN;
		cfg->SDCLKDL_DC = 64;
		cfg->SMPLDL_CNFG_INSEL_OV = 0;
		cfg->SMPLDL_CNFG_INSEL = DW_PHY_SMPLDL_INSRC_DL1OUT;
		cfg->SMPLDL_CNFG_EXTDLY_EN = DW_PHY_EXTDLY_EN;
		cfg->AT_STAT_CENTER_PH_CODE = 64;

		cfg->PAD_SN = DW_PHY_PAD_SN_50_OHM_18;
		cfg->PAD_SP = DW_PHY_PAD_SP_50_OHM_18;
		cfg->TXSLEW_CTRL_N = DW_PHY_PAD_BEST_TXSLEW_N_18;
		cfg->TXSLEW_CTRL_P = DW_PHY_PAD_BEST_TXSLEW_P_18;
		cfg->WEAKPULL_EN = DW_PHY_PULL_UP;
		cfg->WEAKPULL_EN_CLK = DW_PHY_PULL_OFF;
		cfg->RXSEL = DW_PHY_RCV_TYPE_18_SCHMITT;
		cfg->RXSEL_CLK = DW_PHY_RCV_TYPE_OFF;
	} else {
		cfg->SDCLKDL_CNFG_INSEL = DW_PHY_SDCLKDL_INSRC_CCLK_TX;
		cfg->SDCLKDL_CNFG_EXTDLY_EN = DW_PHY_EXTDLY_EN;
		cfg->SDCLKDL_DC = 64;
		cfg->SMPLDL_CNFG_INSEL_OV = 0;
		cfg->SMPLDL_CNFG_INSEL = DW_PHY_SMPLDL_INSRC_DL1OUT;
		cfg->SMPLDL_CNFG_EXTDLY_EN = DW_PHY_EXTDLY_DIS;
		cfg->AT_STAT_CENTER_PH_CODE = 64;

		cfg->PAD_SN = DW_PHY_PAD_SN_50_OHM_33;
		cfg->PAD_SP = DW_PHY_PAD_SP_50_OHM_33;
		cfg->TXSLEW_CTRL_N = DW_PHY_PAD_BEST_TXSLEW_N_33;
		cfg->TXSLEW_CTRL_P = DW_PHY_PAD_BEST_TXSLEW_P_33;
		cfg->WEAKPULL_EN = DW_PHY_PULL_UP;
		cfg->WEAKPULL_EN_CLK = DW_PHY_PULL_OFF;
		cfg->RXSEL = DW_PHY_RCV_TYPE_33_SCHMITT;
		cfg->RXSEL_CLK = DW_PHY_RCV_TYPE_OFF;
	}

	/* pad config */
	pad_cfg = (cfg->TXSLEW_CTRL_N << DW_PHY_PAD_TXSLEW_N_SHIFT)
		| (cfg->TXSLEW_CTRL_P << DW_PHY_PAD_TXSLEW_P_SHIFT)
		| (cfg->WEAKPULL_EN << DW_PHY_PAD_WEEKPULL_SHIFT)
		| (cfg->RXSEL << DW_PHY_PAD_RXSEL_SHIFT);
	sdhci_writew(host, pad_cfg, DW_PHY_CMDPAD_CNFG);
	sdhci_writew(host, pad_cfg, DW_PHY_DATAPAD_CNFG);
	pad_cfg = (cfg->TXSLEW_CTRL_N << DW_PHY_PAD_TXSLEW_N_SHIFT)
		| (cfg->TXSLEW_CTRL_P << DW_PHY_PAD_TXSLEW_P_SHIFT)
		| (cfg->WEAKPULL_EN_CLK << DW_PHY_PAD_WEEKPULL_SHIFT)
		| (cfg->RXSEL_CLK << DW_PHY_PAD_RXSEL_SHIFT);
	sdhci_writew(host, pad_cfg, DW_PHY_CLKPAD_CNFG);
	pad_cfg =  (cfg->TXSLEW_CTRL_N << DW_PHY_PAD_TXSLEW_N_SHIFT)
		| (cfg->TXSLEW_CTRL_P << DW_PHY_PAD_TXSLEW_P_SHIFT)
		| (DW_PHY_PULL_DOWN << DW_PHY_PAD_WEEKPULL_SHIFT)
		| (cfg->RXSEL << DW_PHY_PAD_RXSEL_SHIFT);
	sdhci_writew(host, pad_cfg, DW_PHY_STBPAD_CNFG);

	if (host->dev_type == MMC_IS_EMMC) {
		pad_cfg = (cfg->TXSLEW_CTRL_N << DW_PHY_PAD_TXSLEW_N_SHIFT)
			| (cfg->TXSLEW_CTRL_P << DW_PHY_PAD_TXSLEW_P_SHIFT)
			| (cfg->WEAKPULL_EN << DW_PHY_PAD_WEEKPULL_SHIFT)
			| (cfg->RXSEL << DW_PHY_PAD_RXSEL_SHIFT);
		sdhci_writew(host, pad_cfg, DW_PHY_RSTNPAD_CNFG);
	}

	/*
	 * delay line config
	 * sample_dl_128_cell:
	 *	HOST_CTRL2_R.SAMPLE_CLK_SEL = 0
	 *	AT_CRTL_R.SW_TUNE_EN = 1
	 *	AT_STAT_R.CENTER_PH_CODE = 64 (2.5ns)
	 */
	//sdhci_writew(host, 0, SDHCI_HOST_CONTROL2);
	sdhci_writeb(host, 0, DW_PHY_COMMDL_CNFG);
	sdclkdl_cnfg = DW_PHY_SDCLKDL_UPDATE_DC
		| (cfg->SDCLKDL_CNFG_INSEL << DW_PHY_SDCLKDL_INSRC_CCLK_TX_SHIFT)
		| (cfg->SDCLKDL_CNFG_EXTDLY_EN << DW_PHY_SDCLKDL_EXTDLY_EN_SHITF);
	sdhci_writeb(host, sdclkdl_cnfg, DW_PHY_SDCLKDL_CNFG);
	sdclkdl_dc = cfg->SDCLKDL_DC;
	sdhci_writeb(host, sdclkdl_dc, DW_PHY_SDCLKDL_DC);
	sdclkdl_cnfg &= ~DW_PHY_SDCLKDL_UPDATE_DC;
	sdhci_writeb(host, sdclkdl_cnfg, DW_PHY_SDCLKDL_CNFG);

	smpldl_cnfg = (cfg->SMPLDL_CNFG_INSEL_OV << DW_PHY_SMPLDL_IN_OV_SHITF)
		| (cfg->SMPLDL_CNFG_INSEL << DW_PHY_SMPLDL_INSRC_CCLK_TX_SHIFT);
		/*| DW_PHY_SMPLDL_EXTDLY_EN;*/
	sdhci_writeb(host, smpldl_cnfg, DW_PHY_SMPLDL_CNFG);

#if 0	/* SW TUNE will make signal sample_cclk_sel to 1, and then smpldl INSEL will be 3*/
	at_ctrl_r = AT_CTRL_SW_TUNE_EN;
	sdhci_writel(host, at_ctrl_r, AT_CTRL_R);
	at_stat_r = AT_STAT_CENTER_PH_CODE(cfg->AT_STAT_CENTER_PH_CODE);
	sdhci_writel(host, at_stat_r, AT_STAT_R);
#endif
	/* check phy power good */
	do {
		if (retry > 0) {
			retry--;
			udelay(200);
		} else {
			pr_err("[%s] check phy power good timeout\n", __func__);
			break;
		}

		phy_cnfg = sdhci_readl(host, DW_PHY_CNFG);
	} while ((phy_cnfg & DW_PHY_CNFG_PWRGOOD) != DW_PHY_CNFG_PWRGOOD);

	/* de-assert phy reset */
	phy_cnfg |= DW_PHY_CNFG_RSTN
		   | (cfg->PAD_SN << DW_PHY_CNFG_SN_SHITF)
		   | (cfg->PAD_SP << DW_PHY_CNFG_SP_SHITF);
	sdhci_writel(host, phy_cnfg, DW_PHY_CNFG);
}

static unsigned int sdhci_err1;
static unsigned int sdhci_err2;
module_param(sdhci_err1, uint, 0644);
module_param(sdhci_err2, uint, 0644);

/*
 * USI3_SCLK(bit0, GPIO PB13) and USI3_SD0(bit1, GPIO PB14) control power supply for SD card:
 *		00: power off
 *		01: bus 1.8V (may not need)
 *		10: bus 3.3V
 */
void sdhci_pwr_init(struct sdhci_host *host)
{
	struct gpio_desc *pb13;
	struct gpio_desc *pb14;

	if (host->dev_type == MMC_IS_EMMC)
		return;

	/* get gpio desc */
	pb13 = devm_gpiod_get_index(host->mmc->parent, "sdpwr", 0, GPIOD_OUT_HIGH);
	if (IS_ERR(pb13)) {
		//pr_err("Error getting GPIO pb13\n");
		sdhci_err1++;
		return;
	}

	pb14 = devm_gpiod_get_index(host->mmc->parent, "sdpwr", 1, GPIOD_OUT_HIGH);
	if (IS_ERR(pb14)) {
		//pr_err("Error getting GPIO pb14\n");
		sdhci_err2++;
		return;
	}
	/* power off */
	gpiod_set_value_cansleep(pb13, 0);
	gpiod_set_value_cansleep(pb14, 0);

	udelay(2000);
	/* power on */
	gpiod_set_value_cansleep(pb14, 1);
}

#endif

static int dwcmshc_probe(struct platform_device *pdev)
{
	struct sdhci_pltfm_host *pltfm_host;
	struct sdhci_host *host;
	struct dwcmshc_priv *priv;
	int err;
	u32 extra;

	host = sdhci_pltfm_init(pdev, &sdhci_dwcmshc_pdata,
				sizeof(struct dwcmshc_priv));
	if (IS_ERR(host))
		return PTR_ERR(host);

	/*
	 * extra adma table cnt for cross 128M boundary handling.
	 */
	extra = DIV_ROUND_UP_ULL(dma_get_required_mask(&pdev->dev), SZ_128M);
	if (extra > SDHCI_MAX_SEGS)
		extra = SDHCI_MAX_SEGS;
	host->adma_table_cnt += extra;

	pltfm_host = sdhci_priv(host);
	priv = sdhci_pltfm_priv(pltfm_host);

	pltfm_host->clk = devm_clk_get(&pdev->dev, "core");
	if (IS_ERR(pltfm_host->clk)) {
		err = PTR_ERR(pltfm_host->clk);
		dev_err(&pdev->dev, "failed to get core clk: %d\n", err);
		goto free_pltfm;
	}
	pr_info("core clock %s\n", __clk_get_name(pltfm_host->clk));

	err = clk_prepare_enable(pltfm_host->clk);
	if (err)
		goto free_pltfm;

	priv->bus_clk = devm_clk_get(&pdev->dev, "bus");
	if (!IS_ERR(priv->bus_clk))
		clk_prepare_enable(priv->bus_clk);

	pr_info("bus clock %s\n", __clk_get_name(priv->bus_clk));

	err = mmc_of_parse(host->mmc);
	if (err)
		goto err_clk;

	sdhci_get_of_property(pdev);

	err = sdhci_add_host(host);
	if (err)
		goto err_clk;

#ifdef CONFIG_ON_FPGA
	host->mmc->max_blk_count = SNPS_MMC_MAX_BLK_COUNT;
#endif

	dwcmshc_create_sysfs(host);
	return 0;

err_clk:
	clk_disable_unprepare(pltfm_host->clk);
	clk_disable_unprepare(priv->bus_clk);
free_pltfm:
	sdhci_pltfm_free(pdev);
	return err;
}

static int dwcmshc_remove(struct platform_device *pdev)
{
	struct sdhci_host *host = platform_get_drvdata(pdev);
	struct sdhci_pltfm_host *pltfm_host = sdhci_priv(host);
	struct dwcmshc_priv *priv = sdhci_pltfm_priv(pltfm_host);

	dwcmshc_remove_sysfs();

	sdhci_remove_host(host, 0);

	clk_disable_unprepare(pltfm_host->clk);
	clk_disable_unprepare(priv->bus_clk);

	sdhci_pltfm_free(pdev);

	return 0;
}

#ifdef CONFIG_PM_SLEEP
static int dwcmshc_suspend(struct device *dev)
{
	struct sdhci_host *host = dev_get_drvdata(dev);
	struct sdhci_pltfm_host *pltfm_host = sdhci_priv(host);
	struct dwcmshc_priv *priv = sdhci_pltfm_priv(pltfm_host);
	int ret;

	ret = sdhci_suspend_host(host);
	if (ret)
		return ret;

	clk_disable_unprepare(pltfm_host->clk);
	if (!IS_ERR(priv->bus_clk))
		clk_disable_unprepare(priv->bus_clk);

	return ret;
}

static int dwcmshc_resume(struct device *dev)
{
	struct sdhci_host *host = dev_get_drvdata(dev);
	struct sdhci_pltfm_host *pltfm_host = sdhci_priv(host);
	struct dwcmshc_priv *priv = sdhci_pltfm_priv(pltfm_host);
	int ret;

	ret = clk_prepare_enable(pltfm_host->clk);
	if (ret)
		return ret;

	if (!IS_ERR(priv->bus_clk)) {
		ret = clk_prepare_enable(priv->bus_clk);
		if (ret)
			return ret;
	}

	return sdhci_resume_host(host);
}
#endif

static SIMPLE_DEV_PM_OPS(dwcmshc_pmops, dwcmshc_suspend, dwcmshc_resume);

static const struct of_device_id sdhci_dwcmshc_dt_ids[] = {
	{ .compatible = "snps,dwcmshc-sdhci" },
	{}
};
MODULE_DEVICE_TABLE(of, sdhci_dwcmshc_dt_ids);

static struct platform_driver sdhci_dwcmshc_driver = {
	.driver	= {
		.name	= "sdhci-dwcmshc",
		.probe_type = PROBE_PREFER_ASYNCHRONOUS,
		.of_match_table = sdhci_dwcmshc_dt_ids,
		.pm = &dwcmshc_pmops,
	},
	.probe	= dwcmshc_probe,
	.remove	= dwcmshc_remove,
};
module_platform_driver(sdhci_dwcmshc_driver);

MODULE_DESCRIPTION("SDHCI platform driver for Synopsys DWC MSHC");
MODULE_AUTHOR("Jisheng Zhang <jszhang@kernel.org>");
MODULE_LICENSE("GPL v2");

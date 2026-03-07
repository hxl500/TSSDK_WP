/*****************************************************************************
* Copyright (C) 2021,Tsing Micro Technology Co., Ltd,All rights reserved.
* FileName     : snps_sdhci.c
* Author       :
* Data         : 2021-12-27
* Description  : snps dwc_mshc sdhci driver
*
*****************************************************************************/
#include <common.h>
#include <clk.h>
#include <dm.h>
#include <malloc.h>
#include <reset.h>
#include <errno.h>
#include <sdhci.h>
#include <linux/delay.h>

#ifdef CONFIG_ON_FPGA
#define SNPS_SDHC_MAX_CLK        20000000  /* from FPGA */
#else
#define SNPS_SDHC_MAX_CLK        200000000  /* 800MHz from cpu_pll_d2, or 24MHz from OSC */
#endif
#define SNPS_SDHC_MIN_CLK          400000

/* read/write maximum 4 blocks once */
#define SNPS_MMC_MAX_BLK_COUNT          4

struct snps_sdhci_plat {
	struct mmc_config cfg;
	struct mmc mmc;
};

DECLARE_GLOBAL_DATA_PTR;

#if 1  /* to be optimized */
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
	uint32_t  valid:1;

	/* delay line, 22 bits */
	uint32_t  SDCLKDL_CNFG_INSEL:2;
	uint32_t  SDCLKDL_CNFG_EXTDLY_EN:1;
	uint32_t  SDCLKDL_DC:7;
	uint32_t  SMPLDL_CNFG_INSEL_OV:1;
	uint32_t  SMPLDL_CNFG_INSEL:2;
	uint32_t  SMPLDL_CNFG_EXTDLY_EN:1;
	uint32_t  AT_STAT_CENTER_PH_CODE:8;	/* not use */

	/* pad, 26 bits */
	uint32_t  PAD_SN:4;
	uint32_t  PAD_SP:4;
	uint32_t  TXSLEW_CTRL_N:4;
	uint32_t  TXSLEW_CTRL_P:4;
	uint32_t  WEAKPULL_EN:2;
	uint32_t  WEAKPULL_EN_CLK:2;
	uint32_t  RXSEL:3;
	uint32_t  RXSEL_CLK:3;
};

enum DW_PHY_RCV_TYPE
{
	DW_PHY_RCV_TYPE_OFF = 0,
	DW_PHY_RCV_TYPE_18_SCHMITT,
	DW_PHY_RCV_TYPE_33_SCHMITT,
	DW_PHY_RCV_TYPE_12_SCHMITT,
	DW_PHY_RCV_TYPE_18_COMPATATOR_05,			/* Vth = 0.5*VDDIO */
	DW_PHY_RCV_TYPE_12_COMPATATOR,
	DW_PHY_RCV_TYPE_18_COMPATATOR_0527_0472,	/* VIH = 0.527*VDDIO; VIL = 0.472*VDDIO */
	DW_PHY_RCV_TYPE_INTER_LP
};

void sdhci_phy_init(struct sdhci_host *host)
{
	struct dw_phy_cfg tmpcfg = {0};
	struct dw_phy_cfg *cfg = &tmpcfg;
	uint16_t pad_cfg;
	uint32_t phy_cnfg;
	uint8_t sdclkdl_cnfg, sdclkdl_dc, smpldl_cnfg;
	u32 dev;

	if (host->ioaddr == (void *)0xf0d70000)
		dev = 0;
	else
		dev = 1;

	debug("[%s] dev %d\n", __func__, dev);

	/* process phy config data */
	//if (dw_params.mmc_dev_type == MMC_IS_EMMC) {
	if (dev == 0) {
		debug("[%s] emmc br\n", __func__);
		cfg->SDCLKDL_CNFG_INSEL = DW_PHY_SDCLKDL_INSRC_CCLK_TX;
		cfg->SDCLKDL_CNFG_EXTDLY_EN = DW_PHY_EXTDLY_EN;
		cfg->SDCLKDL_DC = 64;
		cfg->SMPLDL_CNFG_INSEL_OV = 0;
		cfg->SMPLDL_CNFG_INSEL = DW_PHY_SMPLDL_INSRC_DL1OUT;
		cfg->SMPLDL_CNFG_EXTDLY_EN = DW_PHY_EXTDLY_DIS;
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
		debug("[%s] sd br\n", __func__);
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

	//if (dw_params.mmc_dev_type == MMC_IS_EMMC) {
	if (dev == 0) {
		pad_cfg = (cfg->TXSLEW_CTRL_N << DW_PHY_PAD_TXSLEW_N_SHIFT)
			| (cfg->TXSLEW_CTRL_P << DW_PHY_PAD_TXSLEW_P_SHIFT)
			| (cfg->WEAKPULL_EN << DW_PHY_PAD_WEEKPULL_SHIFT)
			| (cfg->RXSEL << DW_PHY_PAD_RXSEL_SHIFT);
		sdhci_writew(host, pad_cfg, DW_PHY_RSTNPAD_CNFG);
	}

	/*
	 * delay line config
	 * sample_dl_128_cell:
	 * 	HOST_CTRL2_R.SAMPLE_CLK_SEL = 0
	 * 	AT_CRTL_R.SW_TUNE_EN = 1
	 * 	AT_STAT_R.CENTER_PH_CODE = 64 (2.5ns)
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
	sdhci_writeb(host, smpldl_cnfg, DW_PHY_SMPLDL_CNFG);

#if 0	/* SW TUNE will make signal sample_cclk_sel to 1, and then smpldl INSEL will be 3*/
	at_ctrl_r = AT_CTRL_SW_TUNE_EN;
	sdhci_writel(host, at_ctrl_r, AT_CTRL_R);
	at_stat_r = AT_STAT_CENTER_PH_CODE(cfg->AT_STAT_CENTER_PH_CODE);
	sdhci_writel(host, at_stat_r, AT_STAT_R);
#endif
	/* check phy power good */
	u32 retry = 1000;
	do {
		
		if (retry > 0) {
			retry--;
			udelay(200);
		} else {
			debug("[%s] retry failed\n", __func__);
			break;
		}

		phy_cnfg = sdhci_readl(host, DW_PHY_CNFG);
	} while ((phy_cnfg & DW_PHY_CNFG_PWRGOOD) != DW_PHY_CNFG_PWRGOOD);

	debug("[%s] retry %u, phy_cnfg 0x%x\n", __func__, retry, phy_cnfg);

	/* de-assert phy reset */
	phy_cnfg |= DW_PHY_CNFG_RSTN
		   | (cfg->PAD_SN << DW_PHY_CNFG_SN_SHITF)
		   | (cfg->PAD_SP << DW_PHY_CNFG_SP_SHITF);
	sdhci_writel(host, phy_cnfg, DW_PHY_CNFG);
	debug("[%s] wr phy_cnfg 0x%x to DW_PHY_CNFG(0x300)\n", __func__, phy_cnfg);
}
#endif


#if defined(CONFIG_ARCH_TS_TX536) || defined (CONFIG_ARCH_TS_TX5336)/* to be replaced by pinctrl */
/*
 * pad controller
 */
#define TS_PAD_CTRL_BASE			ULL(0xF0D95000)

#define PAD_CTRL_USI3_SCLK_OFF			ULL(0xB4)
#define PAD_CTRL_USI3_SD0_OFF			ULL(0xB8)
#define PAD_CTRL_FUNC3_GPIO				3
#define PAD_CTRL_NAND_DS_SHIFT			12
#define PAD_CTRL_NAND_DS(ds)			((ds) << PAD_CTRL_NAND_DS_SHIFT)
#define PAD_CTRL_PU						BIT(6)


#define ARM_TF_GPIO_DIR_OUT		0
#define ARM_TF_GPIO_DIR_IN		1

#define ARM_TF_GPIO_LEVEL_LOW		0
#define ARM_TF_GPIO_LEVEL_HIGH		1

#define GPIO_DIR_OUT		ARM_TF_GPIO_DIR_OUT
#define GPIO_DIR_IN		ARM_TF_GPIO_DIR_IN

#define GPIO_LEVEL_LOW		ARM_TF_GPIO_LEVEL_LOW
#define GPIO_LEVEL_HIGH		ARM_TF_GPIO_LEVEL_HIGH

enum sdcard_pwr_ctrl
{
	SD_PWR_OFF = 0,
	SD_PWR_1P8 = 1,
	SD_PWR_3P3 = 2,
};

static inline uint32_t mmio_read_32(uintptr_t addr)
{
	return readl((uint32_t *)addr);
}

static inline void mmio_write_32(uintptr_t addr, uint32_t value)
{
	writel(value, (unsigned char *)addr);
}

#define GPIO_NUM_PER_BANK			32
#define GPIO_NUM_PER_BANK_SHIFT		5
#define GPIO_BANK_ADDR_LEN			0x400

#define GPIO_GET_BANK(gpio)			((gpio) >> GPIO_NUM_PER_BANK_SHIFT)
#define GPIO_GET_BANK_BASE(gpio)	(ULL(0xF0D94000) + GPIO_GET_BANK(gpio) * GPIO_BANK_ADDR_LEN)
#define GPIO_GET_REG_SHIFT(gpio)	((gpio) & 0x1F)

#define GPIO_DATA_REG_OFF			(0)
#define GPIO_DIR_REG_OFF			(4)

static void ts_gpio_set_direction(int gpio, int direction)
{
	uint32_t val;
	uintptr_t regN = GPIO_GET_BANK_BASE(gpio) + GPIO_DIR_REG_OFF;
	int shift = GPIO_GET_REG_SHIFT(gpio);

	val = mmio_read_32(regN);
	switch (direction) {
	case GPIO_DIR_IN:
		val &= ~(1 << shift);
		break;
	case GPIO_DIR_OUT:
		val |= (1 << shift);
		break;
	}
	mmio_write_32(regN, val);
}

static void ts_gpio_set_value(int gpio, int value)
{
	uint32_t val;
	uintptr_t regN = GPIO_GET_BANK_BASE(gpio) + GPIO_DATA_REG_OFF;
	int shift = GPIO_GET_REG_SHIFT(gpio);

	val = mmio_read_32(regN);
	switch (value) {
	case GPIO_LEVEL_LOW:
		val &= ~(1 << shift);
		break;
	case GPIO_LEVEL_HIGH:
		val |= (1 << shift);
		break;
	}
	mmio_write_32(regN, val);
}

/*
 * USI3_SCLK(bit0, GPIO PB13) and USI3_SD0(bit1, GPIO PB14) control power supply for SD card:
 *		00: power off
 *		01: bus 1.8V (may not need)
 *		10: bus 3.3V
 */
#define SDIO1_PWR0_GPIO			(13 + 32)	/* PORTB[13] */
#define SDIO1_PWR1_GPIO			(14 + 32)	/* PORTB[14] */
void ts_sd_pwr_ctrl(enum sdcard_pwr_ctrl ctrl)
{
	debug("[%s] enter ctrl %d\n", __func__, ctrl);

	switch (ctrl) {
	case SD_PWR_1P8:
		ts_gpio_set_value(SDIO1_PWR0_GPIO, GPIO_LEVEL_HIGH);
		ts_gpio_set_value(SDIO1_PWR1_GPIO, GPIO_LEVEL_LOW);
		break;
	case SD_PWR_3P3:
		ts_gpio_set_value(SDIO1_PWR0_GPIO, GPIO_LEVEL_LOW);
		ts_gpio_set_value(SDIO1_PWR1_GPIO, GPIO_LEVEL_HIGH);
		break;
	case SD_PWR_OFF:
	default:
		ts_gpio_set_value(SDIO1_PWR0_GPIO, GPIO_LEVEL_LOW);
		ts_gpio_set_value(SDIO1_PWR1_GPIO, GPIO_LEVEL_LOW);
		break;
	}
	ts_gpio_set_direction(SDIO1_PWR0_GPIO, GPIO_DIR_OUT);
	ts_gpio_set_direction(SDIO1_PWR1_GPIO, GPIO_DIR_OUT);

	/* pad config */
	mmio_write_32(TS_PAD_CTRL_BASE + PAD_CTRL_USI3_SCLK_OFF,
		PAD_CTRL_FUNC3_GPIO | PAD_CTRL_NAND_DS(1) | PAD_CTRL_PU);
	mmio_write_32(TS_PAD_CTRL_BASE + PAD_CTRL_USI3_SD0_OFF,
		PAD_CTRL_FUNC3_GPIO | PAD_CTRL_NAND_DS(1) | PAD_CTRL_PU);
}

void sdhci_pwr_init(struct sdhci_host *host)
{
	u32 dev;

	if (host->ioaddr == (void *)0xf0d70000)
		dev = 0;
	else
		dev = 1;

	debug("[%s] dev %d\n", __func__, dev);

	if (dev == 0)
		return;

	/* power-on may control by gpio */
	debug("[%s] SD_PWR_OFF\n", __func__);
	ts_sd_pwr_ctrl(SD_PWR_OFF);
	udelay(2000);
	debug("[%s] SD_PWR_3P3\n", __func__);
	ts_sd_pwr_ctrl(SD_PWR_3P3);
}
#endif


int snps_sdhci_clk_cfg(struct udevice *dev)
{
	struct clk clk_ciu, clk_biu;
	u32 rate;
	int ret;

#ifdef CONFIG_ON_FPGA
	return 0;
#endif

	ret = clk_get_by_index(dev, 0, &clk_biu);
	if (ret) {
		debug("get clk 0 failed\n");
		goto clk_err;
	} else {
		debug("get clk 0 %s, id %ld\n", clk_biu.dev->name, clk_biu.id);
	}

	ret = clk_enable(&clk_biu);
	if (ret) {
		debug("enable clk 0 failed\n");
		goto clk_err;
	}

	ret = clk_get_by_index(dev, 1, &clk_ciu);
	if (ret) {
		debug("get clk 1 failed\n");
		goto clk_err;
	} else {
		debug("get clk 1 %s, id %ld\n", clk_ciu.dev->name, clk_ciu.id);
	}


	rate = clk_get_rate(&clk_ciu);
	if ((int)rate <= 0) {
		debug("get clk %ld rate failed\n", clk_ciu.id);
		goto clk_err;
	}
	debug("rate %d\n", rate);

	if (rate > SNPS_SDHC_MAX_CLK) {
		clk_set_rate(&clk_ciu, SNPS_SDHC_MAX_CLK);
	}

	ret = clk_enable(&clk_ciu);
	if (ret) {
		debug("enable clk 1 failed\n");
		goto clk_err;
	}

	
	clk_free(&clk_ciu);
	return 0;

clk_err:
	dev_err(dev, "failed to setup clocks, ret %d\n", ret);
	return 0;
}

void snps_sdhci_reset_cfg(struct udevice *dev)
{
	int ret;

	struct reset_ctl_bulk rst_bulk;
	ret = reset_get_bulk(dev, &rst_bulk);
	if (ret)
		dev_warn(dev, "Can't get reset: %d\n", ret);
	else{
		reset_assert_bulk(&rst_bulk);
		udelay(2);
		reset_deassert_bulk(&rst_bulk);
	}
}

static int snps_sdhci_probe(struct udevice *dev)
{
	struct mmc_uclass_priv *upriv = dev_get_uclass_priv(dev);
	struct snps_sdhci_plat *plat = dev_get_platdata(dev);
	struct sdhci_host *host = dev_get_priv(dev);
	u32 max_clk;
	u32 min_clk;
	int ret;
	void *ioaddr;
	fdt_addr_t iosize = 0;

	max_clk = ofnode_read_u32_default(dev->node, "sdio-max-frequency",
							SNPS_SDHC_MAX_CLK);
	min_clk = ofnode_read_u32_default(dev->node, "sdio-min-frequency",
							SNPS_SDHC_MIN_CLK);
	debug("max_clk %d, min_clk %d\n", max_clk, min_clk);

	ioaddr = (void *)dev_read_addr_size_index(dev, 0, &iosize);
// #if defined(CONFIG_ARCH_TS_TX536)
// 	debug("ioaddr %p, iosize 0x%llx\n", ioaddr, iosize);
// #else
// 	debug("ioaddr %p, iosize 0x%lx\n", ioaddr, iosize);
// #endif
	snps_sdhci_clk_cfg(dev);
	snps_sdhci_reset_cfg(dev);

	host->name = dev->name;
	host->ioaddr = ioaddr;

	host->quirks = 0;
#if 0
	host->bus_width = fdtdec_get_int(gd->fdt_blob, dev_of_offset(dev),
					 "bus-width", 4);
#endif


	host->max_clk = max_clk;
	host->mmc = &plat->mmc;
	host->mmc->dev = dev;

	ret = sdhci_setup_cfg(&plat->cfg, host, 0, min_clk);
	debug("sdhci_setup_cfg ret %d\n", ret);
	if (ret)
		return ret;

	/* modify b_max on Haps */
#ifdef CONFIG_ON_FPGA
	plat->cfg.b_max = SNPS_MMC_MAX_BLK_COUNT;
#endif
	debug("plat->cfg.b_max %d\n", plat->cfg.b_max);

	host->mmc->priv = host;
	upriv->mmc = host->mmc;

	ret = sdhci_probe(dev);
	debug("sdhci_probe ret %d\n\n", ret);
	if (ret)
		debug("sdhci_probe failed, ret %d\n", ret);

	(void)iosize;
	return ret;
}

static int snps_sdhci_bind(struct udevice *dev)
{
	struct snps_sdhci_plat *plat = dev_get_platdata(dev);
	return sdhci_bind(dev, &plat->mmc, &plat->cfg);
}

static const struct udevice_id snps_sdhci_ids[] = {
	{ .compatible = "snps,dwcmshc-sdhci" },
	{ }
};

U_BOOT_DRIVER(snps_sdhci_drv) = {
	.name		= "snps_sdhci",
	.id		    = UCLASS_MMC,
	.of_match	= snps_sdhci_ids,
	.ops		= &sdhci_ops,
	.bind		= snps_sdhci_bind,
	.probe		= snps_sdhci_probe,
	.priv_auto_alloc_size = sizeof(struct sdhci_host),
	.platdata_auto_alloc_size = sizeof(struct snps_sdhci_plat),
};

// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2022-2022 Tsingmicro, Inc.
 */

#define LOG_CATEGORY UCLASS_RESET

#include <common.h>
#include <dm.h>
#include <dm/device_compat.h>
#include <reset-uclass.h>
#include <asm/io.h>
#include <dt-bindings/reset/ts-tx5215-resets.h>

/* TOP_RGU*/
#define CPU_RST_CTL				0x800
#define SDHC0_RST_CTL			0x820
#define SDHC1_RST_CTL			0x824
#define PERI_DMA0_RST_CTL		0x830
#define PERI_DMA1_RST_CTL		0x834
#define MEM_DMA_RST_CTL			0x838
#define GMAC_RST_CTL			0x840
#define OCRAM_RST_CTL			0x848
#define CRYPTO_RST_CTL			0x850
#define OSPI_RST_CTL			0x858
#define USB_RST_CTL				0x85C
#define I2C_RST_CTL0			0x870
#define I2C_RST_CTL1			0x874
#define UART_RST_CTL0			0x880
#define UART_RST_CTL1			0x884
#define USI_RST_CTL				0x890
#define I2S0_RST_CTL			0x894
#define PDM_RST_CTL				0x898
#define ADC_RST_CTL				0x89C
#define SPI_RST_CTL				0x8A0
#define OTPC_RST_CTL			0x8A4
#define PWM_RST_CTL				0x8A8
#define IOMUX_RST_CTL			0x8AC
#define WDT_RST_CTL				0x8B0
#define TMR_RST_CTL				0x8B4
#define SYSREG_RST_CTL			0x8B8
#define BOOTROOM_RST_CTL		0x8BC
#define AUD_CODEC_RST_CTL		0x8C0
#define DISP_SYS_RST_CTL		0x8D0
#define NEMADC_RST_CTL			0x8D4
#define DVP_RST_CTL				0x8D8
#define CVE_RST_CTL				0x8DC
#define AMR_RST_CTL				0x8E0
#define PD_VI_RST_CTL			0x900
#define ISP_RST_CTL				0x904
#define VPE_RST_CTL				0x908
#define HDR_RST_CTL				0x90C
#define MIPI_RX_RST_CTL			0x910
#define PD_DDR_RST_CTL			0x920
#define UMCTL_RST_CTL			0x924
#define DDR_PHY_RST_CTL			0x928
#define PD_AI_RST_CTL			0x930
#define RNE_RST_CTL				0x934
#define PD_VPU_RST_CTL			0x940
#define VPU_RST_CTL				0x944
#define INT_COL_RST_CTL			0x948


#define RESET_LIST \
	RESET(TS_CPU_C0RST, CPU_RST_CTL, 0) \
	RESET(TS_CPU_C1RST, CPU_RST_CTL, 1) \
	RESET(TS_CPU_DBG_RST, CPU_RST_CTL, 4) \
	RESET(TS_SDHC0_HRST, SDHC0_RST_CTL, 0) \
	RESET(TS_SDHC0_CRST, SDHC0_RST_CTL, 1) \
	RESET(TS_SDHC1_HRST, SDHC1_RST_CTL, 0) \
	RESET(TS_SDHC1_CRST, SDHC1_RST_CTL, 1) \
	RESET(TS_PERI_DMA0_HRST, PERI_DMA0_RST_CTL, 0) \
	RESET(TS_PERI_DMA1_HRST, PERI_DMA1_RST_CTL, 0) \
	RESET(TS_MEM_DMA_ARST, MEM_DMA_RST_CTL, 0) \
	RESET(TS_GMAC_ARST, GMAC_RST_CTL, 0) \
	RESET(TS_GMAC_HRST, GMAC_RST_CTL, 1) \
	RESET(TS_GMAC1_ARST, GMAC_RST_CTL, 2) \
	RESET(TS_GMAC1_HRST, GMAC_RST_CTL, 3) \
	RESET(TS_OCRAM_ARST, OCRAM_RST_CTL, 0) \
	RESET(TS_PKE_RST, CRYPTO_RST_CTL, 0) \
	RESET(TS_SKE_RST, CRYPTO_RST_CTL, 1) \
	RESET(TS_HASH_RST, CRYPTO_RST_CTL, 2) \
	RESET(TS_SPACC_RST, CRYPTO_RST_CTL, 3) \
	RESET(TS_TRNG_RST, CRYPTO_RST_CTL, 4) \
	RESET(TS_OSPI_PRST, OSPI_RST_CTL, 0) \
	RESET(TS_OSPI_HRST, OSPI_RST_CTL, 1) \
	RESET(TS_OSPI_REF_RST, OSPI_RST_CTL, 2) \
	RESET(TS_USB2C_HRST, USB_RST_CTL, 0) \
	RESET(TS_USB_PHY_RST, USB_RST_CTL, 1) \
	RESET(TS_I2C0_PRST, I2C_RST_CTL0, 0) \
	RESET(TS_I2C0_ICRST, I2C_RST_CTL0, 1) \
	RESET(TS_I2C1_PRST, I2C_RST_CTL0, 8) \
	RESET(TS_I2C1_ICRST, I2C_RST_CTL0, 9) \
	RESET(TS_I2C2_PRST, I2C_RST_CTL1, 0) \
	RESET(TS_I2C2_ICRST, I2C_RST_CTL1, 1) \
	RESET(TS_I2C3_PRST, I2C_RST_CTL1, 8) \
	RESET(TS_I2C3_ICRST, I2C_RST_CTL1, 9) \
	RESET(TS_UART0_PRST, UART_RST_CTL0, 0) \
	RESET(TS_UART0_SRST, UART_RST_CTL0, 1) \
	RESET(TS_UART1_PRST, UART_RST_CTL0, 8) \
	RESET(TS_UART1_SRST, UART_RST_CTL0, 9) \
	RESET(TS_UART2_PRST, UART_RST_CTL1, 0) \
	RESET(TS_UART2_SRST, UART_RST_CTL1, 1) \
	RESET(TS_USI0_PRST, USI_RST_CTL, 0) \
	RESET(TS_USI1_PRST, USI_RST_CTL, 8) \
	RESET(TS_I2S0_PRST, I2S0_RST_CTL, 0) \
	RESET(TS_I2S0_MRST, I2S0_RST_CTL, 1) \
	RESET(TS_PDM_PRST, PDM_RST_CTL, 0) \
	RESET(TS_PDM_MRST, PDM_RST_CTL, 1) \
	RESET(TS_ADC_PRST, ADC_RST_CTL, 0) \
	RESET(TS_SPI_PRST, SPI_RST_CTL, 0) \
	RESET(TS_SPI_SSI_RST, SPI_RST_CTL, 1) \
	RESET(TS_OTPC_PRST, OTPC_RST_CTL, 0) \
	RESET(TS_PWM_PRST, PWM_RST_CTL, 0) \
	RESET(TS_IOMUX_PRST, IOMUX_RST_CTL, 0) \
	RESET(TS_WDT_PRST, WDT_RST_CTL, 0) \
	RESET(TS_WDT_TICK_RST, WDT_RST_CTL, 1) \
	RESET(TS_TIMER_PRST, TMR_RST_CTL, 0) \
	RESET(TS_TMR_T1RST, TMR_RST_CTL, 1) \
	RESET(TS_TMR_T2RST, TMR_RST_CTL, 2) \
	RESET(TS_TMR_T3RST, TMR_RST_CTL, 3) \
	RESET(TS_TMR_T4RST, TMR_RST_CTL, 4) \
	RESET(TS_TMR_T5RST, TMR_RST_CTL, 5) \
	RESET(TS_TMR_T6RST, TMR_RST_CTL, 6) \
	RESET(TS_TMR_T7RST, TMR_RST_CTL, 7) \
	RESET(TS_TMR_T8RST, TMR_RST_CTL, 8) \
	RESET(TS_SYS_REG_RST, SYSREG_RST_CTL, 0) \
	RESET(TS_GPIO_A_PRST, SYSREG_RST_CTL, 1) \
	RESET(TS_GPIO_B_RST, SYSREG_RST_CTL, 2) \
	RESET(TS_BOOTM_HRST, BOOTROOM_RST_CTL, 0) \
	RESET(TS_AUD_CODEC_RST, AUD_CODEC_RST_CTL, 0) \
	RESET(TS_DISP_SYS_RST, DISP_SYS_RST_CTL, 0) \
	RESET(TS_NEMADC_HRST, NEMADC_RST_CTL, 0) \
	RESET(TS_DVP_HRST, DVP_RST_CTL, 0) \
	RESET(TS_DVP_ARST, DVP_RST_CTL, 1) \
	RESET(TS_DVP_PXRST, DVP_RST_CTL, 2) \
	RESET(TS_CVE_HRST, CVE_RST_CTL, 0) \
	RESET(TS_CVE_ARST, CVE_RST_CTL, 1) \
	RESET(TS_CVE_CRST, CVE_RST_CTL, 2) \
	RESET(TS_AMR_HRST, AMR_RST_CTL, 0) \
	RESET(TS_AMR_ARST, AMR_RST_CTL, 1) \
	RESET(TS_AMR_CRST, AMR_RST_CTL, 2) \
	RESET(TS_VI_SYS_RST, PD_VI_RST_CTL, 0) \
	RESET(TS_ISP_HRST, ISP_RST_CTL, 0) \
	RESET(TS_ISP_ARST, ISP_RST_CTL, 1) \
	RESET(TS_ISP_SRST, ISP_RST_CTL, 2) \
	RESET(TS_VPE_HRST, VPE_RST_CTL, 0) \
	RESET(TS_VPE_ARST, VPE_RST_CTL, 1) \
	RESET(TS_VPE_CRST, VPE_RST_CTL, 2) \
	RESET(TS_VPE_ISP_RST, VPE_RST_CTL, 3) \
	RESET(TS_HDR_HRST, HDR_RST_CTL, 0) \
	RESET(TS_HDR_ARST, HDR_RST_CTL, 1) \
	RESET(TS_HDR_SRST, HDR_RST_CTL, 2) \
	RESET(TS_MIPI_RX0_PRST, MIPI_RX_RST_CTL, 0) \
	RESET(TS_MIPI_RX1_PRST1, MIPI_RX_RST_CTL, 1) \
	RESET(TS_MIPI_PHY_PRST, MIPI_RX_RST_CTL, 2) \
	RESET(TS_DDR_SYS_RST, PD_DDR_RST_CTL, 0) \
	RESET(TS_UMCTL_PRST, UMCTL_RST_CTL, 0) \
	RESET(TS_MCTL_P0_ARST, UMCTL_RST_CTL, 1) \
	RESET(TS_MCTL_P1_ARST, UMCTL_RST_CTL, 2) \
	RESET(TS_MCTL_P2_ARST, UMCTL_RST_CTL, 3) \
	RESET(TS_MCTL_P3_ARST, UMCTL_RST_CTL, 4) \
	RESET(TS_MCTL_P4_ARST, UMCTL_RST_CTL, 5) \
	RESET(TS_MCTL_P5_ARST, UMCTL_RST_CTL, 6) \
	RESET(TS_DDRC_CORE_RST, UMCTL_RST_CTL, 7) \
	RESET(TS_DDR_PHY_PRST, DDR_PHY_RST_CTL, 0) \
	RESET(TS_DDR_PHY_RST, DDR_PHY_RST_CTL, 1) \
	RESET(TS_AI_SYS_RST, PD_AI_RST_CTL, 0) \
	RESET(TS_RNE_HRST, RNE_RST_CTL, 0) \
	RESET(TS_RNE_ARST, RNE_RST_CTL, 1) \
	RESET(TS_RNE_CRST, RNE_RST_CTL, 2) \
	RESET(TS_VPU_SYS_RST, PD_VPU_RST_CTL, 0) \
	RESET(TS_VPU_HRST, VPU_RST_CTL, 0) \
	RESET(TS_VPU_ARST, VPU_RST_CTL, 1) \
	RESET(TS_INT_COL_RST, INT_COL_RST_CTL, 0)

#define _RESETIFY(id) TS_RST_##id
#define RESETIFY(id) _RESETIFY(id)

enum ts_rst_ids {
#define RESET(id, ...) RESETIFY(id),
	RESET_LIST
#undef RESET
	TS_RST_NONE,
};

struct ts_rst_params {
	u32 id;
	u32 off;
	u8 bit_idx;
};

static const struct ts_rst_params ts_resets[] = {
#define RESET(_id, _off, _idx) \
	[RESETIFY(_id)] = { \
		.id = (_id), \
		.off = (_off), \
		.bit_idx = (_idx), \
	},
	RESET_LIST
#undef RESET
};

#undef RESET_LIST

#define SET_BIT(data, bit)		((data) | (0x1 << (bit)))
#define CLEAR_BIT(data, bit)		((data) & (~(0x1 << (bit))))

enum ts_pm_reset_action {
	PM_RESET_ACTION_RELEASE = 0,
	PM_RESET_ACTION_ASSERT  = 1,
	PM_RESET_ACTION_PULSE   = 2,
};

struct tx5215_reset_priv {
	u32 nr_reset;
	void __iomem *top_rgu_base;
};

static const struct ts_rst_params *ts_reset_search_by_id(u32 reset)
{
	const struct ts_rst_params *para = NULL;
	u32 i;

	for (i = 0; i < ARRAY_SIZE(ts_resets); i++) {
		if (ts_resets[i].id == reset) {
			para = &ts_resets[i];
			break;
		}
	}
	return para;
}

static int tx5215_pm_reset_assert(struct reset_ctl *rst,
				const enum ts_pm_reset_action assert_flag)
{
	struct tx5215_reset_priv *priv = dev_get_priv(rst->dev);
	const struct ts_rst_params *para = NULL;
	const u32 reset = rst->id;
	void __iomem *addr;
	u32 val;

	debug("--%s id %d, flag %d--\n", __func__, reset, assert_flag);
	para = ts_reset_search_by_id(reset);
	if (!para)
		return -ENOENT;

	addr = priv->top_rgu_base + para->off;
	val = readl(addr);
	debug("--reg %p, bit %d--\n", addr, para->bit_idx);
	debug("--ori val 0x%x--\n", val);
	if (assert_flag == PM_RESET_ACTION_ASSERT)
		val = SET_BIT(val, para->bit_idx);
	else if (assert_flag == PM_RESET_ACTION_RELEASE)
		val = CLEAR_BIT(val, para->bit_idx);
	debug("--new val 0x%x--\n", val);
	writel(val, addr);
	return 0;
}

static int tx5215_reset_assert(struct reset_ctl *rst)
{
	dev_dbg(rst->dev, "%s(rst=%p) (id=%lu)\n", __func__, rst, rst->id);
	return tx5215_pm_reset_assert(rst, PM_RESET_ACTION_ASSERT);
}

static int tx5215_reset_deassert(struct reset_ctl *rst)
{
	debug("%s(rst=%p) (id=%lu)\n", __func__, rst, rst->id);
	return tx5215_pm_reset_assert(rst, PM_RESET_ACTION_RELEASE);
}

static int tx5215_reset_request(struct reset_ctl *rst)
{
	struct tx5215_reset_priv *priv = dev_get_priv(rst->dev);

	debug("%s(rst=%p) (id=%lu) (nr_reset=%d)\n", __func__,
		rst, rst->id, priv->nr_reset);

	if (rst->id >= priv->nr_reset)
		return -EINVAL;

	return 0;
}

static int tx5215_reset_free(struct reset_ctl *rst)
{
	struct tx5215_reset_priv *priv = dev_get_priv(rst->dev);

	debug("%s(rst=%p) (id=%lu) (nr_reset=%d)\n", __func__,
		rst, rst->id, priv->nr_reset);
	return 0;
}

static bool probed;
static int tx5215_reset_probe(struct udevice *dev)
{
	struct tx5215_reset_priv *priv = dev_get_priv(dev);
	fdt_addr_t addr;

	debug("---%s enter---\n", __func__);

	if (probed)
		return -ENOTSUPP;

	addr = dev_read_addr_index(dev, 0);
	if (addr == FDT_ADDR_T_NONE) {
		printf("%s get addr0 failed", __func__);
		return -EINVAL;
	}
	priv->top_rgu_base = (void *)addr;

	pr_debug("--top_rgu %p--\n", priv->top_rgu_base);

	priv->nr_reset = TS_RST_MAX;
	probed = true;
	return 0;
}

const struct reset_ops tx5215_reset_ops = {
	.request = tx5215_reset_request,
	.rfree = tx5215_reset_free,
	.rst_assert = tx5215_reset_assert,
	.rst_deassert = tx5215_reset_deassert,
};

static const struct udevice_id tx5215_reset_ids[] = {
	{ .compatible = "ts,tx5215-reset" },
	{ }
};

U_BOOT_DRIVER(ts_tx5215_reset) = {
	.name		= "ts_tx5215_reset",
	.id		= UCLASS_RESET,
	.of_match	= tx5215_reset_ids,
	.ops		= &tx5215_reset_ops,
	.probe		= tx5215_reset_probe,
	.priv_auto_alloc_size	= sizeof(struct tx5215_reset_priv),
};

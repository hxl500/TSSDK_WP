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
#include <dt-bindings/reset/ts-tx5336-resets.h>

/* TOP_RGU*/
#define CPU_CFG0				0x030
#define TOP_RST_CFG				0x040
#define PERIPH0_CFG1			0x04c
#define PERIPH1_CFG1			0x054
#define PERIPH2_CFG1			0x05c
#define QSPI_CFG				0x060
#define AUD_AHB_CFG				0x064
#define SPI_CFG0				0x074
#define SPI_CFG1				0x078
#define TIMER_CFG0				0x07c
#define TIMER_CFG1				0x080
#define TIMER_CFG2				0x084
#define TIMER_CFG3				0x088
#define I2C_CFG0				0x090
#define I2C_CFG1				0x094
#define I2C_CFG2				0x098
#define I2C_CFG3				0x09c
#define UART_CFG4				0x0b0
#define I2S_CFG					0x0b4
#define SDHC_CFG0				0x0b8
#define SDHC_CFG1				0x0bc
#define USB_CFG					0x0c0
#define GMAC_CFG0				0x0c4
#define UMCTL_CFG0				0x0d0
#define UMCTL_CFG1				0x0d4
#define UMCTL_CFG2				0x0d8
#define UMCTL_CFG3				0x0dc
#define RNE_CFG0				0x0e0
#define RNE_CFG1				0x0e4
#define VI_CFG					0x0ec
#define ISP_CFG					0x0f0
#define VPE_CFG					0x0f4
#define DOF_CFG					0x0f8
#define EBD_CFG					0x0fc
#define MIPI_RX_CFG				0x100
#define CV_CFG					0x104
#define CRYPTO_CFG				0x108
#define AMR_CFG					0x10c
#define CVE_CFG					0x110
#define BIS_CFG					0x114
#define GDC_CFG					0x118
#define DPU_CFG					0x11c
#define MIPI_TX_CFG				0x120
#define ADMA_CFG				0x124
#define PDM_CFG					0x138
#define PDM_CFG1				0x13c
#define VPU_CFG1				0x144


#define TOP_RGU			0

#define RESET_LIST \
		RESET(TS_GIC_RST,			TOP_RGU, CPU_CFG0,		0x6000) \
		RESET(TS_CPU_DAP_SYS_RST,	TOP_RGU, CPU_CFG0,		0x42) \
		RESET(TS_CPU_DAP_DEBUG_RST, TOP_RGU, CPU_CFG0,		0x22) \
		RESET(TS_CV_SYS_ALL_RST,	TOP_RGU, TOP_RST_CFG,	0x10000020) \
		RESET(TS_VPU_SYS_ALL_RST,	TOP_RGU, TOP_RST_CFG,	0x1000010) \
		RESET(TS_RNE_SYS_ALL_RST,	TOP_RGU, TOP_RST_CFG,	0x100008) \
		RESET(TS_DDR_SYS_ALL_RST,	TOP_RGU, TOP_RST_CFG,	0x10004) \
		RESET(TS_VI_SYS_ALL_RST,	TOP_RGU, TOP_RST_CFG,	0x1002) \
		RESET(TS_WDT_T_RST,			TOP_RGU, PERIPH0_CFG1,	0x80000002) \
		RESET(TS_WDT_APB_RST,		TOP_RGU, PERIPH0_CFG1,	0x40000002) \
		RESET(TS_GPIO_APB_RST,		TOP_RGU, PERIPH0_CFG1,	0x20000002) \
		RESET(TS_SYSREG_APB_RST,	TOP_RGU, PERIPH0_CFG1,	0x8000002) \
		RESET(TS_IOMUX_APB_RST,		TOP_RGU, PERIPH0_CFG1,	0x4000002) \
		RESET(TS_GPADC_APB_RST,		TOP_RGU, PERIPH0_CFG1,	0x2000002) \
		RESET(TS_OTPC_APB_RST,		TOP_RGU, PERIPH0_CFG1,	0x1000002) \
		RESET(TS_USI1_APB_RST,		TOP_RGU, PERIPH1_CFG1,	0x40002) \
		RESET(TS_USI0_APB_RST,		TOP_RGU, PERIPH1_CFG1,	0x20002) \
		RESET(TS_PWM_APB_RST,		TOP_RGU, PERIPH1_CFG1,	0x10002) \
		RESET(TS_HDMA1_AHB_RST,		TOP_RGU, PERIPH2_CFG1,	0x202) \
		RESET(TS_HDMA0_AHB_RST,		TOP_RGU, PERIPH2_CFG1,	0x102) \
		RESET(TS_QSPI_REF_RST,		TOP_RGU, QSPI_CFG,		0x402) \
		RESET(TS_QSPI_AHB_RST,		TOP_RGU, QSPI_CFG,		0x202) \
		RESET(TS_QSPI_APB_RST,		TOP_RGU, QSPI_CFG,		0x102) \
		RESET(TS_AUD_CODEC_AHB_RST, TOP_RGU, AUD_AHB_CFG,	0xa) \
		RESET(TS_SPI1_CORE_RST,		TOP_RGU, SPI_CFG0,		0x20020000) \
		RESET(TS_SPI1_APB_RST,		TOP_RGU, SPI_CFG0,		0x10020000) \
		RESET(TS_SPI0_CORE_RST,		TOP_RGU, SPI_CFG0,		0x2002) \
		RESET(TS_SPI0_APB_RST,		TOP_RGU, SPI_CFG0,		0x1002) \
		RESET(TS_SPI2_CORE_RST,		TOP_RGU, SPI_CFG1,		0x2002) \
		RESET(TS_SPI2_APB_RST,		TOP_RGU, SPI_CFG1,		0x1002) \
		RESET(TS_TIMER0_5_RST,		TOP_RGU, TIMER_CFG0,	0x4002) \
		RESET(TS_TIMER0_4_RST,		TOP_RGU, TIMER_CFG0,	0x2002) \
		RESET(TS_TIMER0_3_RST,		TOP_RGU, TIMER_CFG0,	0x1002) \
		RESET(TS_TIMER0_2_RST,		TOP_RGU, TIMER_CFG0,	0x802) \
		RESET(TS_TIMER0_1_RST,		TOP_RGU, TIMER_CFG0,	0x402) \
		RESET(TS_TIMER0_APB_RST,	TOP_RGU, TIMER_CFG0,	0x102) \
		RESET(TS_TIMER0_8_RST,		TOP_RGU, TIMER_CFG1,	0x402) \
		RESET(TS_TIMER0_7_RST,		TOP_RGU, TIMER_CFG1,	0x202) \
		RESET(TS_TIMER0_6_RST,		TOP_RGU, TIMER_CFG1,	0x102) \
		RESET(TS_TIMER1_5_RST,		TOP_RGU, TIMER_CFG2,	0x4002) \
		RESET(TS_TIMER1_4_RST,		TOP_RGU, TIMER_CFG2,	0x2002) \
		RESET(TS_TIMER1_3_RST,		TOP_RGU, TIMER_CFG2,	0x1002) \
		RESET(TS_TIMER1_2_RST,		TOP_RGU, TIMER_CFG2,	0x802) \
		RESET(TS_TIMER1_1_RST,		TOP_RGU, TIMER_CFG2,	0x402) \
		RESET(TS_TIMER1_APB_RST,	TOP_RGU, TIMER_CFG2,	0x102) \
		RESET(TS_TIMER1_8_RST,		TOP_RGU, TIMER_CFG3,	0x402) \
		RESET(TS_TIMER1_7_RST,		TOP_RGU, TIMER_CFG3,	0x202) \
		RESET(TS_TIMER1_6_RST,		TOP_RGU, TIMER_CFG3,	0x102) \
		RESET(TS_I2C1_CORE_RST,		TOP_RGU, I2C_CFG0,		0x420000) \
		RESET(TS_I2C1_APB_RST,		TOP_RGU, I2C_CFG0,		0x220000) \
		RESET(TS_I2C0_CORE_RST,		TOP_RGU, I2C_CFG0,		0x42) \
		RESET(TS_I2C0_APB_RST,		TOP_RGU, I2C_CFG0,		0x22) \
		RESET(TS_I2C3_CORE_RST,		TOP_RGU, I2C_CFG1,		0x420000) \
		RESET(TS_I2C3_APB_RST,		TOP_RGU, I2C_CFG1,		0x220000) \
		RESET(TS_I2C2_CORE_RST,		TOP_RGU, I2C_CFG1,		0x42) \
		RESET(TS_I2C2_APB_RST,		TOP_RGU, I2C_CFG1,		0x22) \
		RESET(TS_I2C5_CORE_RST,		TOP_RGU, I2C_CFG2,		0x420000) \
		RESET(TS_I2C5_APB_RST,		TOP_RGU, I2C_CFG2,		0x220000) \
		RESET(TS_I2C4_CORE_RST,		TOP_RGU, I2C_CFG2,		0x42) \
		RESET(TS_I2C4_APB_RST,		TOP_RGU, I2C_CFG2,		0x22) \
		RESET(TS_I2C6_CORE_RST,		TOP_RGU, I2C_CFG3,		0x42) \
		RESET(TS_I2C6_APB_RST,		TOP_RGU, I2C_CFG3,		0x22) \
		RESET(TS_UART3_CORE_RST,	TOP_RGU, UART_CFG4,		0x202) \
		RESET(TS_UART3_APB_RST,		TOP_RGU, UART_CFG4,		0x102) \
		RESET(TS_UART2_CORE_RST,	TOP_RGU, UART_CFG4,		0x82) \
		RESET(TS_UART2_APB_RST,		TOP_RGU, UART_CFG4,		0x42) \
		RESET(TS_UART1_CORE_RST,	TOP_RGU, UART_CFG4,		0x22) \
		RESET(TS_UART1_APB_RST,		TOP_RGU, UART_CFG4,		0x12) \
		RESET(TS_UART0_CORE_RST,	TOP_RGU, UART_CFG4,		0xa) \
		RESET(TS_UART0_APB_RST,		TOP_RGU, UART_CFG4,		0x6) \
		RESET(TS_I2S_CORE_RST,		TOP_RGU, I2S_CFG,		0x42) \
		RESET(TS_I2S_APB_RST,		TOP_RGU, I2S_CFG,		0x22) \
		RESET(TS_SDHCI0_AHB_RST,	TOP_RGU, SDHC_CFG0,		0x12) \
		RESET(TS_SDHCI1_AHB_RST,	TOP_RGU, SDHC_CFG1,		0x12) \
		RESET(TS_USB_PHY_RST,		TOP_RGU, USB_CFG,		0x42) \
		RESET(TS_USB_AHB_RST,		TOP_RGU, USB_CFG,		0x22) \
		RESET(TS_GMAC_AXI_RST,		TOP_RGU, GMAC_CFG0,		0x22) \
		RESET(TS_GMAC_AHB_RST,		TOP_RGU, GMAC_CFG0,		0x12) \
		RESET(TS_DDRPHY_RST,		TOP_RGU, UMCTL_CFG0,	0x8002) \
		RESET(TS_DDRC_CORE_RST,		TOP_RGU, UMCTL_CFG0,	0x4002) \
		RESET(TS_DDRPHY_APB_RST,	TOP_RGU, UMCTL_CFG0,	0x2002) \
		RESET(TS_DDRC_APB_RST,		TOP_RGU, UMCTL_CFG0,	0x120000) \
		RESET(TS_DDR_PORT1_RST,		TOP_RGU, UMCTL_CFG1,	0x402) \
		RESET(TS_DDR_PORT0_RST,		TOP_RGU, UMCTL_CFG1,	0x12) \
		RESET(TS_DDR_PORT3_RST,		TOP_RGU, UMCTL_CFG2,	0x402) \
		RESET(TS_DDR_PORT2_RST,		TOP_RGU, UMCTL_CFG2,	0x12) \
		RESET(TS_DDR_PORT5_RST,		TOP_RGU, UMCTL_CFG3,	0x402) \
		RESET(TS_DDR_PORT4_RST,		TOP_RGU, UMCTL_CFG3,	0x12) \
		RESET(TS_RNE_CORE_RST,		TOP_RGU, RNE_CFG0,		0x402) \
		RESET(TS_RNE_AXI_RST,		TOP_RGU, RNE_CFG0,		0x202) \
		RESET(TS_RNE_AHB_RST,		TOP_RGU, RNE_CFG0,		0x102) \
		RESET(TS_RNE_MEMBUF_RST,	TOP_RGU, RNE_CFG1,		0x1002) \
		RESET(TS_VI_APB_BUS_RST,	TOP_RGU, VI_CFG,		0x82) \
		RESET(TS_ISP_CORE_RST,		TOP_RGU, ISP_CFG,		0x82) \
		RESET(TS_ISP_AXI_RST,		TOP_RGU, ISP_CFG,		0x42) \
		RESET(TS_ISP_AHB_RST,		TOP_RGU, ISP_CFG,		0x22) \
		RESET(TS_VPE_CORE_RST,		TOP_RGU, VPE_CFG,		0x802) \
		RESET(TS_VPE_ISP_RST,		TOP_RGU, VPE_CFG,		0x402) \
		RESET(TS_VPE_AXI_RST,		TOP_RGU, VPE_CFG,		0x202) \
		RESET(TS_VPE_AHB_RST,		TOP_RGU, VPE_CFG,		0x102) \
		RESET(TS_DOF_CORE_RST,		TOP_RGU, DOF_CFG,		0x402) \
		RESET(TS_DOF_AXI_RST,		TOP_RGU, DOF_CFG,		0x202) \
		RESET(TS_DOF_APB_RST,		TOP_RGU, DOF_CFG,		0x102) \
		RESET(TS_EBD_CORE_RST,		TOP_RGU, EBD_CFG,		0x402) \
		RESET(TS_EBD_AXI_RST,		TOP_RGU, EBD_CFG,		0x202) \
		RESET(TS_EBD_AHB_RST,		TOP_RGU, EBD_CFG,		0x102) \
		RESET(TS_MIPI_RXPHY1_APB_RST,	TOP_RGU, MIPI_RX_CFG, 0x200002) \
		RESET(TS_MIPI_RXPHY0_APB_RST,	TOP_RGU, MIPI_RX_CFG, 0x100002) \
		RESET(TS_MIPI_CSI3_APB_RST,	TOP_RGU, MIPI_RX_CFG,	0x80002) \
		RESET(TS_MIPI_CSI2_APB_RST,	TOP_RGU, MIPI_RX_CFG,	0x40002) \
		RESET(TS_MIPI_CSI1_APB_RST,	TOP_RGU, MIPI_RX_CFG,	0x20002) \
		RESET(TS_MIPI_CSI0_APB_RST,	TOP_RGU, MIPI_RX_CFG,	0x10002) \
		RESET(TS_BOOTROM_AHB_RST,	TOP_RGU, CV_CFG,		0x402) \
		RESET(TS_CV_APB_BUS_RST,	TOP_RGU, CV_CFG,		0x202) \
		RESET(TS_TRNG_RST,			TOP_RGU, CRYPTO_CFG,	0x1002) \
		RESET(TS_PKE_RST,			TOP_RGU, CRYPTO_CFG,	0x802) \
		RESET(TS_SPACC_RST,			TOP_RGU, CRYPTO_CFG,	0x402) \
		RESET(TS_SKE_RST,			TOP_RGU, CRYPTO_CFG,	0x202) \
		RESET(TS_HASH_RST,			TOP_RGU, CRYPTO_CFG,	0x102) \
		RESET(TS_AMR_CORE_RST,		TOP_RGU, AMR_CFG,		0x82) \
		RESET(TS_AMR_AXI_RST,		TOP_RGU, AMR_CFG,		0x42) \
		RESET(TS_AMR_AHB_RST,		TOP_RGU, AMR_CFG,		0x22) \
		RESET(TS_CVE_CORE_RST,		TOP_RGU, CVE_CFG,		0x82) \
		RESET(TS_CVE_AXI_RST,		TOP_RGU, CVE_CFG,		0x42) \
		RESET(TS_CVE_AHB_RST,		TOP_RGU, CVE_CFG,		0x22) \
		RESET(TS_BIS_CORE_RST,		TOP_RGU, BIS_CFG,		0x402) \
		RESET(TS_BIS_AXI_RST,		TOP_RGU, BIS_CFG,		0x202) \
		RESET(TS_BIS_APB_RST,		TOP_RGU, BIS_CFG,		0x102) \
		RESET(TS_GDC_CORE_RST,		TOP_RGU, GDC_CFG,		0x402) \
		RESET(TS_GDC_AXI_RST,		TOP_RGU, GDC_CFG,		0x202) \
		RESET(TS_GDC_APB_RST,		TOP_RGU, GDC_CFG,		0x102) \
		RESET(TS_DPU_ALL_RST,		TOP_RGU, DPU_CFG,		0x22) \
		RESET(TS_MIPI_TXPHY_APB_RST,	TOP_RGU, MIPI_TX_CFG,	0x82) \
		RESET(TS_MIPI_DSI_APB_RST,	TOP_RGU, MIPI_TX_CFG,	0x42) \
		RESET(TS_ADMA_AXI_RST,		TOP_RGU, ADMA_CFG,		0x42) \
		RESET(TS_ADMA_AHB_RST,		TOP_RGU, ADMA_CFG,		0x22) \
		RESET(TS_PDM1_M_RST,		TOP_RGU, PDM_CFG,		0x220000) \
		RESET(TS_PDM1_APB_RST,		TOP_RGU, PDM_CFG,		0x120000) \
		RESET(TS_PDM0_M_RST,		TOP_RGU, PDM_CFG,		0x22) \
		RESET(TS_PDM0_APB_RST,		TOP_RGU, PDM_CFG,		0x12) \
		RESET(TS_PDM3_M_RST,		TOP_RGU, PDM_CFG1,		0x220000) \
		RESET(TS_PDM3_APB_RST,		TOP_RGU, PDM_CFG1,		0x120000) \
		RESET(TS_PDM2_M_RST,		TOP_RGU, PDM_CFG1,		0x22) \
		RESET(TS_PDM2_APB_RST,		TOP_RGU, PDM_CFG1,		0x12) \
		RESET(TS_VPU_CORE_RST,		TOP_RGU, VPU_CFG1,		0x12)


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
	u32 rgu;
	u32 off;
	u32 bit_idx;
};

static const struct ts_rst_params ts_resets[] = {
#define RESET(_id, _rgu, _off, _idx) \
	[RESETIFY(_id)] = { \
		.id = (_id), \
		.rgu = (_rgu), \
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

struct ts_reset_priv {
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

static int ts_pm_reset_assert(struct reset_ctl *rst,
				const enum ts_pm_reset_action assert_flag)
{
	struct ts_reset_priv *priv = dev_get_priv(rst->dev);
	const struct ts_rst_params *para = NULL;
	const u32 reset = rst->id;
	void __iomem *addr;
	u32 val, bit, index, count;

	debug("--%s id %d, flag %d--\n", __func__, reset, assert_flag);
	para = ts_reset_search_by_id(reset);
	if (!para)
		return -ENOENT;

	addr = priv->top_rgu_base + para->off;
	val = readl(addr);
	debug("--reg %p, bit %d--\n", addr, para->bit_idx);
	debug("--ori val 0x%x--\n", val);

	bit = para->bit_idx;
	index = 0;
	count = 0;

	while (bit) {
		if (bit & 0x1) {
			count++;
			if (assert_flag == PM_RESET_ACTION_ASSERT)
				val = SET_BIT(val, index);
			else if (assert_flag == PM_RESET_ACTION_RELEASE) {
				if (count == 1)
					val = SET_BIT(val, index);
				else
					val = CLEAR_BIT(val, index);
			}
		}
		index++;
		bit >>= 1;
	}
	debug("--new val 0x%x--\n", val);
	writel(val, addr);
	return 0;
}

static int ts_reset_assert(struct reset_ctl *rst)
{
	dev_dbg(rst->dev, "%s(rst=%p) (id=%lu)\n", __func__, rst, rst->id);
	return ts_pm_reset_assert(rst, PM_RESET_ACTION_ASSERT);
}

static int ts_reset_deassert(struct reset_ctl *rst)
{
	debug("%s(rst=%p) (id=%lu)\n", __func__, rst, rst->id);
	return ts_pm_reset_assert(rst, PM_RESET_ACTION_RELEASE);
}

static int ts_reset_request(struct reset_ctl *rst)
{
	struct ts_reset_priv *priv = dev_get_priv(rst->dev);

	debug("%s(rst=%p) (id=%lu) (nr_reset=%d)\n", __func__,
		rst, rst->id, priv->nr_reset);

	if (rst->id >= priv->nr_reset)
		return -EINVAL;

	return 0;
}

static int ts_reset_free(struct reset_ctl *rst)
{
	struct ts_reset_priv *priv = dev_get_priv(rst->dev);

	debug("%s(rst=%p) (id=%lu) (nr_reset=%d)\n", __func__,
		rst, rst->id, priv->nr_reset);
	return 0;
}

static bool probed;
static int ts_reset_probe(struct udevice *dev)
{
	struct ts_reset_priv *priv = dev_get_priv(dev);
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

	printf("--top_rgu %p--\n", priv->top_rgu_base);

	priv->nr_reset = TS_RST_MAX;
	probed = true;
	return 0;
}

const struct reset_ops ts_reset_ops = {
	.request = ts_reset_request,
	.rfree = ts_reset_free,
	.rst_assert = ts_reset_assert,
	.rst_deassert = ts_reset_deassert,
};

static const struct udevice_id ts_reset_ids[] = {
	{ .compatible = "ts,tx5336-reset" },
	{ }
};

U_BOOT_DRIVER(ts_ts_reset) = {
	.name		= "ts_tx5336_reset",
	.id		= UCLASS_RESET,
	.of_match	= ts_reset_ids,
	.ops		= &ts_reset_ops,
	.probe		= ts_reset_probe,
	.priv_auto_alloc_size	= sizeof(struct ts_reset_priv),
};

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
#include <dt-bindings/reset/ts-tx5112-resets.h>

/* TOP_RGU*/
#define CPU_RST_CFG             0x030
#define BUS_RST_CFG             0x040
#define AXI_RET_CFG             0x048
#define AHB_RST_CFG             0x04c
#define APB_RST_CFG             0x054
#define SDHC0_RST_CFG		    0x060
#define SDHC1_RST_CFG		    0x064
#define OSPI_RST_CFG		    0x068
#define USB_RST_CFG			    0x06c
#define GMAC_RST_CFG		    0x070
#define I2C_RST_CFG0		    0x080
#define I2C_RST_CFG1		    0x084
#define UART_RST_CFG		    0x09c
#define SPI_RST_CFG			    0x0a0
#define TMR_RST_CFG0			0x0a4
#define TMR_RST_CFG1		    0x0a8
#define TMR_RST_CFG2			0x0ac
#define I2S_RST_CFG			    0x0b0
#define PDM_RST_CFG			    0x0b4
#define MCTL_RST_CFG		    0x0d4
#define VI_BUS_RST_CFG		    0x0d8
#define ISP_RST_CFG		        0x0dc
#define VPE_RST_CFG			    0x0e0
#define AMR_RST_CFG			    0x0e8
#define RNE_RST_CFG			    0x0ec
#define VPU_RST_CFG			    0x0f0

#define RESET_LIST \
	RESET(TS_CPU_DAP_DBG_RST, CPU_RST_CFG, 0x22) \
	RESET(TS_CPU_DAP_SYS_RST, CPU_RST_CFG, 0x42) \
	RESET(TS_CPU_SOC_DBG_RST, CPU_RST_CFG, 0x82) \
	RESET(TS_CPU_L2_RST, CPU_RST_CFG, 0x102) \
	RESET(TS_CPU_DBG_RST, CPU_RST_CFG, 0x202) \
	RESET(TS_CPU_CORE_RST, CPU_RST_CFG, 0x402) \
	RESET(TS_BUS_PD_VI_SYS_RST, BUS_RST_CFG, 0x1002) \
	RESET(TS_BUS_PD_DDR_SYS_RST, BUS_RST_CFG, 0x10004) \
	RESET(TS_BUS_PD_AI_SYS_RST, BUS_RST_CFG, 0x100008) \
	RESET(TS_BUS_PD_VPU_SYS_RST, BUS_RST_CFG, 0x1000010) \
	RESET(TS_AXI_MEM_DMA_ARST, AXI_RET_CFG, 0x10002) \
	RESET(TS_AXI_OCRAM_ARST, AXI_RET_CFG, 0x20002) \
	RESET(TS_AXI_SKE_RST, AXI_RET_CFG, 0x40002) \
	RESET(TS_AXI_UMCTL_PRST, AXI_RET_CFG, 0x80002) \
	RESET(TS_AXI_MCTL_P0_ARST, AXI_RET_CFG, 0x100002) \
	RESET(TS_AXI_MCTL_P1_ARST, AXI_RET_CFG, 0x200002) \
	RESET(TS_AXI_MCTL_P2_ARST, AXI_RET_CFG, 0x400002) \
	RESET(TS_AXI_MCTL_P3_ARST, AXI_RET_CFG, 0x800002) \
	RESET(TS_AXI_DDRC_CORE_RST, AXI_RET_CFG, 0x1000002) \
	RESET(TS_AHB_PERI_DMA0_HRST, AHB_RST_CFG, 0x10002) \
	RESET(TS_AHB_PERI_DMA1_HRST, AHB_RST_CFG, 0x20002) \
	RESET(TS_AHB_BOOTM_HRST, AHB_RST_CFG, 0x40002) \
	RESET(TS_AHB_AUD_CODEC_RST, AHB_RST_CFG, 0x80002) \
	RESET(TS_APB_USI0_PRST, APB_RST_CFG, 0x10001) \
	RESET(TS_APB_USI1_PRST, APB_RST_CFG, 0x20002) \
	RESET(TS_APB_ADC_PRST, APB_RST_CFG, 0x40004) \
	RESET(TS_APB_GPIO_PRST, APB_RST_CFG, 0x80008) \
	RESET(TS_APB_WDT_PRST, APB_RST_CFG, 0x100010) \
	RESET(TS_APB_PWM_PRST, APB_RST_CFG, 0x200020) \
	RESET(TS_APB_OTPC_PRST, APB_RST_CFG, 0x400040) \
	RESET(TS_APB_GPIO_B_PRST, APB_RST_CFG, 0x800080) \
	RESET(TS_APB_GPIO_A_PRST, APB_RST_CFG, 0x1000100) \
	RESET(TS_APB_SYS_REG_PRST, APB_RST_CFG, 0x2000200) \
	RESET(TS_APB_WDT_TICK_RST, APB_RST_CFG, 0x4000400) \
	RESET(TS_SDHC0_RST, SDHC0_RST_CFG, 0x62) \
	RESET(TS_SDHC1_RST, SDHC1_RST_CFG, 0x62) \
	RESET(TS_OSPI_REF_RST, OSPI_RST_CFG, 0x22) \
	RESET(TS_OSPI_HRST, OSPI_RST_CFG, 0x42) \
	RESET(TS_OSPI_PRST, OSPI_RST_CFG, 0x82) \
	RESET(TS_USB2C_HRST, USB_RST_CFG, 0x22) \
	RESET(TS_USB_PHY_RST, USB_RST_CFG, 0x42) \
	RESET(TS_GMAC_RST, GMAC_RST_CFG, 0x62) \
	RESET(TS_I2C0_RST, I2C_RST_CFG0, 0x32) \
	RESET(TS_I2C1_RST, I2C_RST_CFG0, 0xc2) \
	RESET(TS_I2C2_RST,I2C_RST_CFG1, 0x32) \
	RESET(TS_UART0_RST, UART_RST_CFG, 0xe) \
	RESET(TS_UART1_RST, UART_RST_CFG, 0x32) \
	RESET(TS_SPI_RST, SPI_RST_CFG, 0x62) \
	RESET(TS_TIMER_PRST, TMR_RST_CFG0, 0x12) \
	RESET(TS_TMR_T1RST, TMR_RST_CFG0, 0x22) \
	RESET(TS_TMR_T2RST, TMR_RST_CFG0, 0x42) \
	RESET(TS_TMR_T3RST, TMR_RST_CFG0, 0x82) \
	RESET(TS_TMR_T4RST, TMR_RST_CFG0, 0x102) \
	RESET(TS_TMR_T5RST, TMR_RST_CFG1, 0x22) \
	RESET(TS_TMR_T6RST, TMR_RST_CFG1, 0x42) \
	RESET(TS_TMR_T7RST, TMR_RST_CFG2, 0x22) \
	RESET(TS_TMR_T8RST, TMR_RST_CFG2, 0x42) \
	RESET(TS_I2S0_RST, I2S_RST_CFG, 0x62) \
	RESET(TS_PDM_RST, PDM_RST_CFG, 0x62) \
	RESET(TS_DDR_PHY_PRST, MCTL_RST_CFG, 0x202) \
	RESET(TS_DDR_PHY_RST, MCTL_RST_CFG, 0x402) \
	RESET(TS_HDR_HRST, VI_BUS_RST_CFG, 0x20002) \
	RESET(TS_HDR_ARST, VI_BUS_RST_CFG, 0x40002) \
	RESET(TS_HDR_SRST, VI_BUS_RST_CFG, 0x80002) \
	RESET(TS_MIPI_RX0_PRST, VI_BUS_RST_CFG, 0x100002) \
	RESET(TS_MIPI_RX1_PRST, VI_BUS_RST_CFG, 0x200002) \
	RESET(TS_MIPI_PHY_PRST, VI_BUS_RST_CFG, 0x400002) \
	RESET(TS_ISP_SRST, ISP_RST_CFG, 0x22) \
	RESET(TS_ISP_ARST, ISP_RST_CFG, 0x42) \
	RESET(TS_ISP_HRST, ISP_RST_CFG, 0x82) \
	RESET(TS_VPE_CRST, VPE_RST_CFG, 0x22) \
	RESET(TS_VPE_ARST, VPE_RST_CFG, 0x42) \
	RESET(TS_VPE_HRST, VPE_RST_CFG, 0x82) \
	RESET(TS_VPE_ISP_RST, VPE_RST_CFG, 0x102) \
	RESET(TS_AMR_RST, AMR_RST_CFG, 0xe2) \
	RESET(TS_RNE_CRST, RNE_RST_CFG, 0x82) \
	RESET(TS_RNE_ARST, RNE_RST_CFG, 0x102) \
	RESET(TS_RNE_HRST, RNE_RST_CFG, 0x202) \
	RESET(TS_VPU_ARST, VPU_RST_CFG, 0x102) \
	RESET(TS_VPU_HRST, VPU_RST_CFG, 0x202) \
	RESET(TS_H265_CCLK_RST, VPU_RST_CFG, 0x402) \
	RESET(TS_H264_CCLK_RST, VPU_RST_CFG, 0x802)

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
	u32 bit_idx;
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

struct tx5112_reset_priv {
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

static int tx5112_pm_reset_assert(struct reset_ctl *rst,
				const enum ts_pm_reset_action assert_flag)
{
	struct tx5112_reset_priv *priv = dev_get_priv(rst->dev);
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

	/*if (assert_flag == PM_RESET_ACTION_ASSERT)
		val = SET_BIT(val, para->bit_idx);
	else if (assert_flag == PM_RESET_ACTION_RELEASE)
		val = CLEAR_BIT(val, para->bit_idx);*/

	bit = para->bit_idx;
	index = 0;
	count = 0;
	
	while (bit) {
		if (bit & 0x1) {
			count++;
			if (assert_flag == PM_RESET_ACTION_ASSERT)
				val = SET_BIT(val, index);
			else if(assert_flag == PM_RESET_ACTION_RELEASE) {
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

static int tx5112_reset_assert(struct reset_ctl *rst)
{
	dev_dbg(rst->dev, "%s(rst=%p) (id=%lu)\n", __func__, rst, rst->id);
	return tx5112_pm_reset_assert(rst, PM_RESET_ACTION_ASSERT);
}

static int tx5112_reset_deassert(struct reset_ctl *rst)
{
	debug("%s(rst=%p) (id=%lu)\n", __func__, rst, rst->id);
	return tx5112_pm_reset_assert(rst, PM_RESET_ACTION_RELEASE);
}

static int tx5112_reset_request(struct reset_ctl *rst)
{
	struct tx5112_reset_priv *priv = dev_get_priv(rst->dev);

	debug("%s(rst=%p) (id=%lu) (nr_reset=%d)\n", __func__,
		rst, rst->id, priv->nr_reset);

	if (rst->id >= priv->nr_reset)
		return -EINVAL;

	return 0;
}

static int tx5112_reset_free(struct reset_ctl *rst)
{
	struct tx5112_reset_priv *priv = dev_get_priv(rst->dev);

	debug("%s(rst=%p) (id=%lu) (nr_reset=%d)\n", __func__,
		rst, rst->id, priv->nr_reset);
	return 0;
}

static bool probed;
static int tx5112_reset_probe(struct udevice *dev)
{
	struct tx5112_reset_priv *priv = dev_get_priv(dev);
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

const struct reset_ops tx5112_reset_ops = {
	.request = tx5112_reset_request,
	.rfree = tx5112_reset_free,
	.rst_assert = tx5112_reset_assert,
	.rst_deassert = tx5112_reset_deassert,
};

static const struct udevice_id tx5112_reset_ids[] = {
	{ .compatible = "ts,tx5112-reset" },
	{ }
};

U_BOOT_DRIVER(ts_tx5112_reset) = {
	.name		= "ts_tx5112_reset",
	.id		= UCLASS_RESET,
	.of_match	= tx5112_reset_ids,
	.ops		= &tx5112_reset_ops,
	.probe		= tx5112_reset_probe,
	.priv_auto_alloc_size	= sizeof(struct tx5112_reset_priv),
};

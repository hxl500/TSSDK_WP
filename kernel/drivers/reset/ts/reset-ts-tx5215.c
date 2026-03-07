// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2022-2022 Tsingmicro, Inc.
 */
#include <linux/kernel.h>
#include <linux/io.h>
#include <linux/of.h>
#include <linux/module.h>
#include <linux/of_device.h>
#include <linux/platform_device.h>
#include <linux/reset-controller.h>
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

#define TOP_RGU			0

#define RESET_LIST \
	RESET(TS_CPU_C0RST, TOP_RGU, CPU_RST_CTL, 0x1) \
	RESET(TS_CPU_C1RST, TOP_RGU, CPU_RST_CTL, 0x2) \
	RESET(TS_CPU_DBG_RST, TOP_RGU, CPU_RST_CTL, 0x10) \
	RESET(TS_SDHC0_RST, TOP_RGU, SDHC0_RST_CTL, 0x3) \
	RESET(TS_SDHC1_RST, TOP_RGU, SDHC1_RST_CTL, 0x3) \
	RESET(TS_PERI_DMA0_HRST, TOP_RGU, PERI_DMA0_RST_CTL, 0x1) \
	RESET(TS_PERI_DMA1_HRST, TOP_RGU, PERI_DMA1_RST_CTL, 0x1) \
	RESET(TS_MEM_DMA_ARST, TOP_RGU, MEM_DMA_RST_CTL, 1) \
	RESET(TS_GMAC_RST, TOP_RGU, GMAC_RST_CTL, 0x3) \
	RESET(TS_GMAC1_RST, TOP_RGU, GMAC_RST_CTL, 0xc) \
	RESET(TS_OCRAM_ARST, TOP_RGU, OCRAM_RST_CTL, 0x1) \
	RESET(TS_PKE_RST, TOP_RGU, CRYPTO_RST_CTL, 0x1) \
	RESET(TS_SKE_RST, TOP_RGU, CRYPTO_RST_CTL, 0x2) \
	RESET(TS_HASH_RST, TOP_RGU, CRYPTO_RST_CTL, 0x4) \
	RESET(TS_SPACC_RST, TOP_RGU, CRYPTO_RST_CTL, 0x8) \
	RESET(TS_TRNG_RST, TOP_RGU, CRYPTO_RST_CTL, 0x10) \
	RESET(TS_OSPI_PRST, TOP_RGU, OSPI_RST_CTL, 0x1) \
	RESET(TS_OSPI_HRST, TOP_RGU, OSPI_RST_CTL, 0x2) \
	RESET(TS_OSPI_REF_RST, TOP_RGU, OSPI_RST_CTL, 0x4) \
	RESET(TS_USB2C_HRST, TOP_RGU, USB_RST_CTL, 0x1) \
	RESET(TS_USB_PHY_RST, TOP_RGU, USB_RST_CTL, 0x3) \
	RESET(TS_I2C0_RST, TOP_RGU, I2C_RST_CTL0, 0x3) \
	RESET(TS_I2C1_RST, TOP_RGU, I2C_RST_CTL0, 0x300) \
	RESET(TS_I2C2_RST, TOP_RGU, I2C_RST_CTL1, 0x3) \
	RESET(TS_I2C3_RST, TOP_RGU, I2C_RST_CTL1, 0x300) \
	RESET(TS_UART0_RST, TOP_RGU, UART_RST_CTL0, 0x3) \
	RESET(TS_UART1_RST, TOP_RGU, UART_RST_CTL0, 0x300) \
	RESET(TS_UART2_RST, TOP_RGU, UART_RST_CTL1, 0x3) \
	RESET(TS_USI0_PRST, TOP_RGU, USI_RST_CTL, 0x1) \
	RESET(TS_USI1_PRST, TOP_RGU, USI_RST_CTL, 0x100) \
	RESET(TS_I2S0_RST, TOP_RGU, I2S0_RST_CTL, 0x3) \
	RESET(TS_PDM_RST, TOP_RGU, PDM_RST_CTL, 0x3) \
	RESET(TS_ADC_PRST, TOP_RGU, ADC_RST_CTL, 0x1) \
	RESET(TS_SPI_RST, TOP_RGU, SPI_RST_CTL, 0x3) \
	RESET(TS_OTPC_PRST, TOP_RGU, OTPC_RST_CTL, 0x1) \
	RESET(TS_PWM_PRST, TOP_RGU, PWM_RST_CTL, 0x1) \
	RESET(TS_IOMUX_PRST, TOP_RGU, IOMUX_RST_CTL, 0x1) \
	RESET(TS_WDT_PRST, TOP_RGU, WDT_RST_CTL, 0x1) \
	RESET(TS_WDT_TICK_RST, TOP_RGU, WDT_RST_CTL, 0x2) \
	RESET(TS_TIMER_PRST, TOP_RGU, TMR_RST_CTL, 0x1) \
	RESET(TS_TMR_T1RST, TOP_RGU, TMR_RST_CTL, 0x2) \
	RESET(TS_TMR_T2RST, TOP_RGU, TMR_RST_CTL, 0x4) \
	RESET(TS_TMR_T3RST, TOP_RGU, TMR_RST_CTL, 0x8) \
	RESET(TS_TMR_T4RST, TOP_RGU, TMR_RST_CTL, 0x10) \
	RESET(TS_TMR_T5RST, TOP_RGU, TMR_RST_CTL, 0x20) \
	RESET(TS_TMR_T6RST, TOP_RGU, TMR_RST_CTL, 0x40) \
	RESET(TS_TMR_T7RST, TOP_RGU, TMR_RST_CTL, 0x80) \
	RESET(TS_TMR_T8RST, TOP_RGU, TMR_RST_CTL, 0x100) \
	RESET(TS_SYS_REG_RST, TOP_RGU, SYSREG_RST_CTL, 0x1) \
	RESET(TS_GPIO_A_PRST, TOP_RGU, SYSREG_RST_CTL, 0x2) \
	RESET(TS_GPIO_B_RST, TOP_RGU, SYSREG_RST_CTL, 0x4) \
	RESET(TS_BOOTM_HRST, TOP_RGU, BOOTROOM_RST_CTL, 0x1) \
	RESET(TS_AUD_CODEC_RST, TOP_RGU, AUD_CODEC_RST_CTL, 0x1) \
	RESET(TS_DISP_SYS_RST, TOP_RGU, DISP_SYS_RST_CTL, 0x1) \
	RESET(TS_NEMADC_HRST, TOP_RGU, NEMADC_RST_CTL, 0x1) \
	RESET(TS_DVP_HRST, TOP_RGU, DVP_RST_CTL, 0x1) \
	RESET(TS_DVP_ARST, TOP_RGU, DVP_RST_CTL, 0x2) \
	RESET(TS_DVP_PXRST, TOP_RGU, DVP_RST_CTL, 0x4) \
	RESET(TS_CVE_RST, TOP_RGU, CVE_RST_CTL, 0x7) \
	RESET(TS_AMR_RST, TOP_RGU, AMR_RST_CTL, 0x7) \
	RESET(TS_VI_SYS_RST, TOP_RGU, PD_VI_RST_CTL, 0x1) \
	RESET(TS_ISP_HRST, TOP_RGU, ISP_RST_CTL, 0x1) \
	RESET(TS_ISP_ARST, TOP_RGU, ISP_RST_CTL, 0x2) \
	RESET(TS_ISP_SRST, TOP_RGU, ISP_RST_CTL, 0x4) \
	RESET(TS_VPE_HRST, TOP_RGU, VPE_RST_CTL, 0x1) \
	RESET(TS_VPE_ARST, TOP_RGU, VPE_RST_CTL, 0x2) \
	RESET(TS_VPE_CRST, TOP_RGU, VPE_RST_CTL, 0x4) \
	RESET(TS_VPE_ISP_RST, TOP_RGU, VPE_RST_CTL, 0x8) \
	RESET(TS_HDR_HRST, TOP_RGU, HDR_RST_CTL, 0x1) \
	RESET(TS_HDR_ARST, TOP_RGU, HDR_RST_CTL, 0x2) \
	RESET(TS_HDR_SRST, TOP_RGU, HDR_RST_CTL, 0x4) \
	RESET(TS_MIPI_RX0_PRST, TOP_RGU, MIPI_RX_RST_CTL, 0x1) \
	RESET(TS_MIPI_RX1_PRST1, TOP_RGU, MIPI_RX_RST_CTL, 0x2) \
	RESET(TS_MIPI_PHY_PRST, TOP_RGU, MIPI_RX_RST_CTL, 0x4) \
	RESET(TS_DDR_SYS_RST, TOP_RGU, PD_DDR_RST_CTL, 0x1) \
	RESET(TS_UMCTL_PRST, TOP_RGU, UMCTL_RST_CTL, 0x1) \
	RESET(TS_MCTL_P0_ARST, TOP_RGU, UMCTL_RST_CTL, 0x2) \
	RESET(TS_MCTL_P1_ARST, TOP_RGU, UMCTL_RST_CTL, 0x4) \
	RESET(TS_MCTL_P2_ARST, TOP_RGU, UMCTL_RST_CTL, 0x8) \
	RESET(TS_MCTL_P3_ARST, TOP_RGU, UMCTL_RST_CTL, 0x10) \
	RESET(TS_MCTL_P4_ARST, TOP_RGU, UMCTL_RST_CTL, 0x20) \
	RESET(TS_MCTL_P5_ARST, TOP_RGU, UMCTL_RST_CTL, 0x40) \
	RESET(TS_DDRC_CORE_RST, TOP_RGU, UMCTL_RST_CTL, 0x80) \
	RESET(TS_DDR_PHY_PRST, TOP_RGU, DDR_PHY_RST_CTL, 0x1) \
	RESET(TS_DDR_PHY_RST, TOP_RGU, DDR_PHY_RST_CTL, 0x2) \
	RESET(TS_AI_SYS_RST, TOP_RGU, PD_AI_RST_CTL, 0x1) \
	RESET(TS_RNE_HRST, TOP_RGU, RNE_RST_CTL, 0x1) \
	RESET(TS_RNE_ARST, TOP_RGU, RNE_RST_CTL, 0x2) \
	RESET(TS_RNE_CRST, TOP_RGU, RNE_RST_CTL, 0x4) \
	RESET(TS_VPU_SYS_RST, TOP_RGU, PD_VPU_RST_CTL, 0x1) \
	RESET(TS_VPU_HRST, TOP_RGU, VPU_RST_CTL, 0x1) \
	RESET(TS_VPU_ARST, TOP_RGU, VPU_RST_CTL, 0x2) \
	RESET(TS_INT_COL_RST, TOP_RGU, INT_COL_RST_CTL, 0x1)

#define _RESETIFY(id) TS_RST_##id
#define RESETIFY(id) _RESETIFY(id)

enum tsm_rst_ids {
#define RESET(id, ...) RESETIFY(id),
	RESET_LIST
#undef RESET
	TS_RST_NONE,
};

struct tsm_rst_params {
	u32 id;
	u32 rgu;
	u32 off;
	u32 bit_idx;
};

static const struct tsm_rst_params tsm_resets[] = {
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

struct tx5215_reset_controller {
	struct reset_controller_dev rst;
	void __iomem *top_rgu_base;
};

#define to_tx5215_reset_controller(_rst) \
	container_of(_rst, struct tx5215_reset_controller, rst)

const struct tsm_rst_params *tsm_reset_search_by_id(u32 reset)
{
	const struct tsm_rst_params *para = NULL;
	u32 i;

	for (i = 0; i < ARRAY_SIZE(tsm_resets); i++) {
		if (tsm_resets[i].id == reset) {
			para = &tsm_resets[i];
			break;
		}
	}
	return para;
}

static int tx5215_reset_program_hw(struct reset_controller_dev *rcdev,
				   unsigned long idx, bool assert)
{
	struct tx5215_reset_controller *rc = to_tx5215_reset_controller(rcdev);
	unsigned long temp;
	u32 bit, index;
	const struct tsm_rst_params *para = NULL;
	void __iomem *addr;

	//pr_info("--%s id %lu, flag %d--\n", __func__, idx, assert);
	para = tsm_reset_search_by_id(idx);
	if (!para)
		return -ENOENT;

	addr = (para->rgu == TOP_RGU) ? rc->top_rgu_base : 0;
	if (addr == 0)
		return -ENOENT;

	addr += para->off;

	temp = readl(addr);

	//pr_info("--reg %p, bit %x--\n", addr, para->bit_idx);
	//pr_info("--ori val 0x%lx--\n", temp);

	bit = para->bit_idx;
	index = 0;
	while (bit) {
		if (bit & 0x1) {
			if (assert)
				set_bit(index, &temp);
			else
				clear_bit(index, &temp);
		}
		index++;
		bit >>= 1;
	}

	//pr_info("--new val 0x%lx--\n", temp);
	writel(temp, addr);
	return 0;
}

static int tx5215_reset_assert(struct reset_controller_dev *rcdev,
			       unsigned long idx)
{
	//pr_info("%s(rst=%p) (id=%lu)\n", __func__, rcdev, idx);
	return tx5215_reset_program_hw(rcdev, idx, true);
}

static int tx5215_reset_deassert(struct reset_controller_dev *rcdev,
				 unsigned long idx)
{
	//pr_info("%s(rst=%p) (id=%lu)\n", __func__, rcdev, idx);
	return tx5215_reset_program_hw(rcdev, idx, false);
}

static int tx5215_reset_dev(struct reset_controller_dev *rcdev,
			    unsigned long idx)
{
	int err;

	//pr_info("%s(rst=%p) (id=%lu)\n", __func__, rcdev, idx);
	err = tx5215_reset_assert(rcdev, idx);
	if (err)
		return err;

	return tx5215_reset_deassert(rcdev, idx);
}

static struct reset_control_ops tx5215_reset_ops = {
	.reset    = tx5215_reset_dev,
	.assert   = tx5215_reset_assert,
	.deassert = tx5215_reset_deassert,
};

static int tx5215_reset_probe(struct platform_device *pdev)
{
	struct tx5215_reset_controller *rc;
	struct device *dev = &pdev->dev;
	struct resource *mem;

	rc = devm_kzalloc(dev, sizeof(*rc), GFP_KERNEL);
	if (!rc)
		return -ENOMEM;

	mem = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	//pr_info("--mem0 start 0x%lx--\n", (unsigned long)mem->start);
	rc->top_rgu_base = devm_ioremap_resource(&pdev->dev, mem);
	if (IS_ERR(rc->top_rgu_base))
		return PTR_ERR(rc->top_rgu_base);

	rc->rst.owner = THIS_MODULE;
	rc->rst.ops = &tx5215_reset_ops;
	rc->rst.of_node = pdev->dev.of_node;
	rc->rst.nr_resets = TS_RST_MAX;
	rc->rst.of_reset_n_cells = 1;

	return reset_controller_register(&rc->rst);
}

static const struct of_device_id tx5215_reset_match[] = {
	{ .compatible = "ts,tx5215-reset", },
	{},
};
MODULE_DEVICE_TABLE(of, tx5215_reset_match);

static struct platform_driver tx5215_reset_driver = {
	.probe = tx5215_reset_probe,
	.driver = {
		.name = "tx5215-reset",
		.of_match_table = tx5215_reset_match,
	},
};

static int __init tx5215_reset_init(void)
{
	return platform_driver_register(&tx5215_reset_driver);
}
arch_initcall(tx5215_reset_init);

MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:tx5215-reset");
MODULE_DESCRIPTION("TS tx5215 Reset Driver");

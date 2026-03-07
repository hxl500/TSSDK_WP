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

#define TOP_RGU			0

#define RESET_LIST \
	RESET(TS_CPU_DAP_DBG_RST, TOP_RGU, CPU_RST_CFG, 0x22) \
	RESET(TS_CPU_DAP_SYS_RST, TOP_RGU, CPU_RST_CFG, 0x42) \
	RESET(TS_CPU_SOC_DBG_RST, TOP_RGU, CPU_RST_CFG, 0x82) \
	RESET(TS_CPU_L2_RST, TOP_RGU, CPU_RST_CFG, 0x102) \
	RESET(TS_CPU_DBG_RST, TOP_RGU, CPU_RST_CFG, 0x202) \
	RESET(TS_CPU_CORE_RST, TOP_RGU, CPU_RST_CFG, 0x402) \
	RESET(TS_BUS_PD_VI_SYS_RST, TOP_RGU, BUS_RST_CFG, 0x1002) \
	RESET(TS_BUS_PD_DDR_SYS_RST, TOP_RGU, BUS_RST_CFG, 0x10004) \
	RESET(TS_BUS_PD_AI_SYS_RST, TOP_RGU, BUS_RST_CFG, 0x100008) \
	RESET(TS_BUS_PD_VPU_SYS_RST, TOP_RGU, BUS_RST_CFG, 0x1000010) \
	RESET(TS_AXI_MEM_DMA_ARST, TOP_RGU, AXI_RET_CFG, 0x10002) \
	RESET(TS_AXI_OCRAM_ARST, TOP_RGU, AXI_RET_CFG, 0x20002) \
	RESET(TS_AXI_SKE_RST, TOP_RGU, AXI_RET_CFG, 0x40002) \
	RESET(TS_AXI_UMCTL_PRST, TOP_RGU, AXI_RET_CFG, 0x80002) \
	RESET(TS_AXI_MCTL_P0_ARST, TOP_RGU, AXI_RET_CFG, 0x100002) \
	RESET(TS_AXI_MCTL_P1_ARST, TOP_RGU, AXI_RET_CFG, 0x200002) \
	RESET(TS_AXI_MCTL_P2_ARST, TOP_RGU, AXI_RET_CFG, 0x400002) \
	RESET(TS_AXI_MCTL_P3_ARST, TOP_RGU, AXI_RET_CFG, 0x800002) \
	RESET(TS_AXI_DDRC_CORE_RST, TOP_RGU, AXI_RET_CFG, 0x1000002) \
	RESET(TS_AHB_PERI_DMA0_HRST, TOP_RGU, AHB_RST_CFG, 0x10002) \
	RESET(TS_AHB_PERI_DMA1_HRST, TOP_RGU, AHB_RST_CFG, 0x20002) \
	RESET(TS_AHB_BOOTM_HRST, TOP_RGU, AHB_RST_CFG, 0x40002) \
	RESET(TS_AHB_AUD_CODEC_RST, TOP_RGU, AHB_RST_CFG, 0x80002) \
	RESET(TS_APB_USI0_PRST, TOP_RGU, APB_RST_CFG, 0x10001) \
	RESET(TS_APB_USI1_PRST, TOP_RGU, APB_RST_CFG, 0x20002) \
	RESET(TS_APB_ADC_PRST, TOP_RGU, APB_RST_CFG, 0x40004) \
	RESET(TS_APB_GPIO_PRST, TOP_RGU, APB_RST_CFG, 0x80008) \
	RESET(TS_APB_WDT_PRST, TOP_RGU, APB_RST_CFG, 0x100010) \
	RESET(TS_APB_PWM_PRST, TOP_RGU, APB_RST_CFG, 0x200020) \
	RESET(TS_APB_OTPC_PRST, TOP_RGU, APB_RST_CFG, 0x400040) \
	RESET(TS_APB_GPIO_B_PRST, TOP_RGU, APB_RST_CFG, 0x800080) \
	RESET(TS_APB_GPIO_A_PRST, TOP_RGU, APB_RST_CFG, 0x1000100) \
	RESET(TS_APB_SYS_REG_PRST, TOP_RGU, APB_RST_CFG, 0x2000200) \
	RESET(TS_APB_WDT_TICK_RST, TOP_RGU, APB_RST_CFG, 0x4000400) \
	RESET(TS_SDHC0_RST, TOP_RGU, SDHC0_RST_CFG, 0x62) \
	RESET(TS_SDHC1_RST, TOP_RGU, SDHC1_RST_CFG, 0x62) \
	RESET(TS_OSPI_REF_RST, TOP_RGU, OSPI_RST_CFG, 0x22) \
	RESET(TS_OSPI_HRST, TOP_RGU, OSPI_RST_CFG, 0x42) \
	RESET(TS_OSPI_PRST, TOP_RGU, OSPI_RST_CFG, 0x82) \
	RESET(TS_USB2C_HRST, TOP_RGU, USB_RST_CFG, 0x42) \
	RESET(TS_USB_PHY_RST, TOP_RGU, USB_RST_CFG, 0x62) \
	RESET(TS_GMAC_RST, TOP_RGU, GMAC_RST_CFG, 0x62) \
	RESET(TS_I2C0_RST, TOP_RGU, I2C_RST_CFG0, 0x32) \
	RESET(TS_I2C1_RST, TOP_RGU, I2C_RST_CFG0, 0xc2) \
	RESET(TS_I2C2_RST, TOP_RGU, I2C_RST_CFG1, 0x32) \
	RESET(TS_UART0_RST, TOP_RGU, UART_RST_CFG, 0xe) \
	RESET(TS_UART1_RST, TOP_RGU, UART_RST_CFG, 0x32) \
	RESET(TS_SPI_RST, TOP_RGU, SPI_RST_CFG, 0x62) \
	RESET(TS_TIMER_PRST, TOP_RGU, TMR_RST_CFG0, 0x12) \
	RESET(TS_TMR_T1RST, TOP_RGU, TMR_RST_CFG0, 0x22) \
	RESET(TS_TMR_T2RST, TOP_RGU, TMR_RST_CFG0, 0x42) \
	RESET(TS_TMR_T3RST, TOP_RGU, TMR_RST_CFG0, 0x82) \
	RESET(TS_TMR_T4RST, TOP_RGU, TMR_RST_CFG0, 0x102) \
	RESET(TS_TMR_T5RST, TOP_RGU, TMR_RST_CFG1, 0x22) \
	RESET(TS_TMR_T6RST, TOP_RGU, TMR_RST_CFG1, 0x42) \
	RESET(TS_TMR_T7RST, TOP_RGU, TMR_RST_CFG2, 0x22) \
	RESET(TS_TMR_T8RST, TOP_RGU, TMR_RST_CFG2, 0x42) \
	RESET(TS_I2S0_RST, TOP_RGU, I2S_RST_CFG, 0x62) \
	RESET(TS_PDM_RST, TOP_RGU, PDM_RST_CFG, 0x62) \
	RESET(TS_DDR_PHY_PRST, TOP_RGU, MCTL_RST_CFG, 0x202) \
	RESET(TS_DDR_PHY_RST, TOP_RGU, MCTL_RST_CFG, 0x402) \
	RESET(TS_HDR_HRST, TOP_RGU, VI_BUS_RST_CFG, 0x20002) \
	RESET(TS_HDR_ARST, TOP_RGU, VI_BUS_RST_CFG, 0x40002) \
	RESET(TS_HDR_SRST, TOP_RGU, VI_BUS_RST_CFG, 0x80002) \
	RESET(TS_MIPI_RX0_PRST, TOP_RGU, VI_BUS_RST_CFG, 0x100002) \
	RESET(TS_MIPI_RX1_PRST, TOP_RGU, VI_BUS_RST_CFG, 0x200002) \
	RESET(TS_MIPI_PHY_PRST, TOP_RGU, VI_BUS_RST_CFG, 0x400002) \
	RESET(TS_ISP_SRST, TOP_RGU, ISP_RST_CFG, 0x22) \
	RESET(TS_ISP_ARST, TOP_RGU, ISP_RST_CFG, 0x42) \
	RESET(TS_ISP_HRST, TOP_RGU, ISP_RST_CFG, 0x82) \
	RESET(TS_VPE_CRST, TOP_RGU, VPE_RST_CFG, 0x22) \
	RESET(TS_VPE_ARST, TOP_RGU, VPE_RST_CFG, 0x42) \
	RESET(TS_VPE_HRST, TOP_RGU, VPE_RST_CFG, 0x82) \
	RESET(TS_VPE_ISP_RST, TOP_RGU, VPE_RST_CFG, 0x102) \
	RESET(TS_AMR_RST, TOP_RGU, AMR_RST_CFG, 0xe2) \
	RESET(TS_RNE_CRST, TOP_RGU, RNE_RST_CFG, 0x82) \
	RESET(TS_RNE_ARST, TOP_RGU, RNE_RST_CFG, 0x102) \
	RESET(TS_RNE_HRST, TOP_RGU, RNE_RST_CFG, 0x202) \
	RESET(TS_VPU_ARST, TOP_RGU, VPU_RST_CFG, 0x102) \
	RESET(TS_VPU_HRST, TOP_RGU, VPU_RST_CFG, 0x202) \
	RESET(TS_H265_CCLK_RST, TOP_RGU, VPU_RST_CFG, 0xd02) \
	RESET(TS_H264_CCLK_RST, TOP_RGU, VPU_RST_CFG, 0x802)

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

struct tx5112_reset_controller {
	struct reset_controller_dev rst;
	void __iomem *top_rgu_base;
};

#define to_tx5112_reset_controller(_rst) \
	container_of(_rst, struct tx5112_reset_controller, rst)

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

static int tx5112_reset_program_hw(struct reset_controller_dev *rcdev,
				   unsigned long idx, bool assert)
{
	struct tx5112_reset_controller *rc = to_tx5112_reset_controller(rcdev);
	unsigned long temp;
	u32 bit, index, count;
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
	count = 0;
	while (bit) {
		if (bit & 0x1) {
			count++;
			if (assert)
				set_bit(index, &temp);
			else {
				if (count == 1)
					set_bit(index, &temp);
				else
					clear_bit(index, &temp);
			}
		}
		index++;
		bit >>= 1;
	}

	//pr_info("--new val 0x%lx--\n", temp);
	writel(temp, addr);
	return 0;
}

static int tx5112_reset_assert(struct reset_controller_dev *rcdev,
			       unsigned long idx)
{
	//pr_info("%s(rst=%p) (id=%lu)\n", __func__, rcdev, idx);
	return tx5112_reset_program_hw(rcdev, idx, true);
}

static int tx5112_reset_deassert(struct reset_controller_dev *rcdev,
				 unsigned long idx)
{
	//pr_info("%s(rst=%p) (id=%lu)\n", __func__, rcdev, idx);
	return tx5112_reset_program_hw(rcdev, idx, false);
}

static int tx5112_reset_dev(struct reset_controller_dev *rcdev,
			    unsigned long idx)
{
	int err;

	//pr_info("%s(rst=%p) (id=%lu)\n", __func__, rcdev, idx);
	err = tx5112_reset_assert(rcdev, idx);
	if (err)
		return err;

	return tx5112_reset_deassert(rcdev, idx);
}

static struct reset_control_ops tx5112_reset_ops = {
	.reset    = tx5112_reset_dev,
	.assert   = tx5112_reset_assert,
	.deassert = tx5112_reset_deassert,
};

static int tx5112_reset_probe(struct platform_device *pdev)
{
	struct tx5112_reset_controller *rc;
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
	rc->rst.ops = &tx5112_reset_ops;
	rc->rst.of_node = pdev->dev.of_node;
	rc->rst.nr_resets = TS_RST_MAX;
	rc->rst.of_reset_n_cells = 1;

	return reset_controller_register(&rc->rst);
}

static const struct of_device_id tx5112_reset_match[] = {
	{ .compatible = "tsm,tx5112-reset", },
	{},
};
MODULE_DEVICE_TABLE(of, tx5112_reset_match);

static struct platform_driver tx5112_reset_driver = {
	.probe = tx5112_reset_probe,
	.driver = {
		.name = "tx5112-reset",
		.of_match_table = tx5112_reset_match,
	},
};

static int __init tx5112_reset_init(void)
{
	return platform_driver_register(&tx5112_reset_driver);
}
arch_initcall(tx5112_reset_init);

MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:tx5112-reset");
MODULE_DESCRIPTION("TS tx5112 Reset Driver");

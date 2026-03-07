// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2022-2022 Tsingmicro, Inc.
 */
#include <asm/io.h>
#include <dt-bindings/clock/ts-tx5112-clk.h>
#include <dm.h>
#include <log.h>
#include <mapmem.h>
#include "ts_pll.h"
#include "ts_clk.h"

/* TOP_CGU */
#define PLL0_CFG0			0x000
#define PLL0_CFG1			0x004
#define PLL1_CFG0			0x010
#define PLL1_CFG1			0x014
#define PLL2_CFG0			0x020
#define PLL2_CFG1			0x024
#define PLL_STATUS			0x02C

#define CPU_CLK_CFG0		0x030
#define CPU_CLK_CFG1		0x034
#define BUS_CLK_CFG0		0x040
#define BUS_CLK_CFG1		0x044
#define BUS_CLK_AXI_EN		0x048
#define BUS_CLK_AHB_EN		0x04C
#define BUS_CLK_APB_EN		0x050

#define SD_CLK_OFFSET		0x05C
#define SDHC0_CCLK_CFG		0x060
#define SDHC1_CCLK_CFG		0x064

#define OSPI_RCLK_CFG		0x068
#define USB_PHY_CLK_CFG		0x06C
#define GMAC_ACLK_CFG		0x070
#define GMAC_MAC_CLK_CFG	0x074
#define GMAC_TXCLK_DLY		0x078
#define GMAC_RXCLK_DLY		0x07C

#define I2C_ICCLK_CFG0		0x080
#define I2C_ICCLK_CFG1		0x084
#define UART0_SCLK_CFG		0x090
#define UART1_SCLK_CFG		0x094
#define SPI_SSICLK_CFG		0x0A0
#define TMR_TCLK_CFG0		0x0A4
#define TMR_TCLK_CFG1		0x0A8
#define TMR_TCLK_CFG2		0x0AC
#define I2S0_MCLK_CFG		0x0B0
#define PDM_MCLK_CFG		0x0B4
#define AUD_DAC_CLK_CFG0	0x0B8
#define AUD_DAC_CLK_CFG1	0x0BC
#define AUD_ADC_CLK_CFG		0x0D0
#define MCTL_ACLK_CFG0		0x0D4
#define VI_BUS_CLK_EN		0x0D8
#define ISP_SCLK_CFG		0x0DC
#define VPE_CCLK_CFG		0x0E0
#define MIPI_CLK_CFG		0x0E4
#define AMR_CCLK_CFG		0x0E8
#define RNE_CCLK_CFG		0x0EC
#define VPU_CLK_CFG			0x0F0
#define CHIP_OCLK_CFG		0x0F4


/* All methods are delegated to CCF clocks */

/* Parents for muxed clocks */
static const char * const cpu_cclk_sels[] = { &osc_in[0], "pll0" };
static const char * const top_axi_ahb_apb_clk_sels[] = { &osc_in[0], "pll0" };
static const char * const sd0_cclk_sels[] = { &osc_in[0], "pll0" };
static const char * const sd0_cclk_smpl_sels[] = { "sd0_cclk_aligned",
	"sd0_cclk_shift_1_10", "sd0_cclk_shift_2_10", "sd0_cclk_shift_3_10",
	"sd0_cclk_shift_4_10", "sd0_cclk_shift_5_10", "sd0_cclk_shift_6_10",
	"sd0_cclk_shift_7_10", "sd0_cclk_shift_8_10", "sd0_cclk_shift_9_10" };
static const char * const sd0_cclk_drv_sels[] = { "sd0_cclk_aligned",
	"sd0_cclk_shift_1_10", "sd0_cclk_shift_2_10", "sd0_cclk_shift_3_10",
	"sd0_cclk_shift_4_10", "sd0_cclk_shift_5_10", "sd0_cclk_shift_6_10",
	"sd0_cclk_shift_7_10", "sd0_cclk_shift_8_10", "sd0_cclk_shift_9_10" };
static const char * const sd1_cclk_sels[] = { &osc_in[0], "pll0" };
static const char * const sd1_cclk_smpl_sels[] = { "sd1_cclk_aligned",
	"sd1_cclk_shift_1_10", "sd1_cclk_shift_2_10", "sd1_cclk_shift_3_10",
	"sd1_cclk_shift_4_10", "sd1_cclk_shift_5_10", "sd1_cclk_shift_6_10",
	"sd1_cclk_shift_7_10", "sd1_cclk_shift_8_10", "sd1_cclk_shift_9_10" };
static const char * const sd1_cclk_drv_sels[] = { "sd1_cclk_aligned",
	"sd1_cclk_shift_1_10", "sd1_cclk_shift_2_10", "sd1_cclk_shift_3_10",
	"sd1_cclk_shift_4_10", "sd1_cclk_shift_5_10", "sd1_cclk_shift_6_10",
	"sd1_cclk_shift_7_10", "sd1_cclk_shift_8_10", "sd1_cclk_shift_9_10" };
static const char * const ospi_ref_clk_sels[] = { &osc_in[0], "pll0" };
static const char * const i2c_icclk_sels[] = { &osc_in[0], "pll0" };
static const char * const uart_sclk_sels[] = { &osc_in[0], "pll0_d4" };
static const char * const spi_ssiiclk_sels[] = { &osc_in[0], "pll0" };
static const char * const rne_cclk_sels[] = { "pll0", "pll1" };
static const char * const h265_cclk_sels[] = { "pll0", "pll1" };
static const char * const chip_out_clk_sels[] = {
	"pll0_d8", "pll1_d8", &osc_in[0] };

/*
 * All parameters for different sub-clocks are collected into parameter arrays.
 * These parameters are then initialized by the clock which uses them during
 * probe. To save space, ids are automatically generated for each sub-clock by
 * using an enum. Instead of storing a parameter struct for each clock, even for
 * those clocks which don't use a particular type of sub-clock, we can just
 * store the parameters for the clocks which need them.
 *
 * So why do it like this? Arranging all the sub-clocks together makes it very
 * easy to find bugs in the code.
 */

#define DIV(id, off, shift, width, we, sync) DIV_FLAGS(id, off, shift, width, we, sync, 0)
#define DIV_LIST \
	DIV(TS_CLK_TX5112_CPU_CCLK, CPU_CLK_CFG0, 16, 4, 0, SYNC_INVALID) \
	DIV(TS_CLK_TX5112_CPU_ACLK, CPU_CLK_CFG1, 24, 1, 0, SYNC_INVALID) \
	DIV(TS_CLK_TX5112_CPU_DBG_PCLK, CPU_CLK_CFG1, 16, 7, 0, SYNC_INVALID) \
	DIV(TS_CLK_TX5112_CORE_APB_CLK, BUS_CLK_CFG1, 24, 7, 0, SYNC_INVALID) \
	DIV(TS_CLK_TX5112_CORE_AHB_CLK, BUS_CLK_CFG1, 16, 7, 0, SYNC_INVALID) \
	DIV(TS_CLK_TX5112_CORE_AXI_CLK, BUS_CLK_CFG1, 8, 4, 0, SYNC_INVALID) \
	DIV(TS_CLK_TX5112_SD0_CCLK, SDHC0_CCLK_CFG, 16, 6, 0, SYNC_INVALID) \
	DIV(TS_CLK_TX5112_SD1_CCLK, SDHC1_CCLK_CFG, 16, 6, 0, SYNC_INVALID) \
	DIV(TS_CLK_TX5112_OSPI_REF_CLK, OSPI_RCLK_CFG, 16, 4, 0, SYNC_INVALID) \
	DIV(TS_CLK_TX5112_I2C1_ICCLK, I2C_ICCLK_CFG0, 24, 5, 0, SYNC_INVALID) \
	DIV(TS_CLK_TX5112_I2C0_ICCLK, I2C_ICCLK_CFG0, 16, 5, 0, SYNC_INVALID) \
	DIV(TS_CLK_TX5112_I2C2_ICCLK, I2C_ICCLK_CFG1, 16, 5, 0, SYNC_INVALID) \
	DIV(TS_CLK_TX5112_UART0_SCLK, UART0_SCLK_CFG, 0, 28, WE_INVALID, SYNC_INVALID) \
	DIV(TS_CLK_TX5112_UART1_SCLK, UART1_SCLK_CFG, 0, 28, WE_INVALID, SYNC_INVALID) \
	DIV(TS_CLK_TX5112_SPI_SSICLK, SPI_SSICLK_CFG, 16, 5, 0, SYNC_INVALID) \
	DIV(TS_CLK_TX5112_TMR_T4CLK, TMR_TCLK_CFG0, 24, 6, 0, SYNC_INVALID) \
	DIV(TS_CLK_TX5112_TMR_T3CLK, TMR_TCLK_CFG0, 16, 6, 0, SYNC_INVALID) \
	DIV(TS_CLK_TX5112_TMR_T6CLK, TMR_TCLK_CFG1, 22, 10, 0, SYNC_INVALID) \
	DIV(TS_CLK_TX5112_TMR_T5CLK, TMR_TCLK_CFG1, 12, 10, 0, SYNC_INVALID) \
	DIV(TS_CLK_TX5112_TMR_T8CLK, TMR_TCLK_CFG2, 22, 10, 0, SYNC_INVALID) \
	DIV(TS_CLK_TX5112_TMR_T7CLK, TMR_TCLK_CFG2, 12, 10, 0, SYNC_INVALID) \
	DIV(TS_CLK_TX5112_I2S0_MCLK, I2S0_MCLK_CFG, 21, 11, 0, SYNC_INVALID) \
	DIV(TS_CLK_TX5112_I2S0_OCLK, I2S0_MCLK_CFG, 12, 8, 0, SYNC_INVALID) \
	DIV(TS_CLK_TX5112_PDM_MCLK, PDM_MCLK_CFG, 16, 9, 0, SYNC_INVALID) \
	DIV(TS_CLK_TX5112_AUD_DAC_PBCLK, AUD_DAC_CLK_CFG0, 20, 12, 0, SYNC_INVALID) \
	DIV(TS_CLK_TX5112_AUD_DAC_CCLK, AUD_DAC_CLK_CFG0, 8, 12, 0, SYNC_INVALID) \
	DIV(TS_CLK_TX5112_ED_TMR_STRB, AUD_DAC_CLK_CFG1, 14, 18, 0, SYNC_INVALID) \
	DIV(TS_CLK_TX5112_AUD_ADC_CCLK, AUD_ADC_CLK_CFG, 8, 12, 0, SYNC_INVALID) \
	DIV(TS_CLK_TX5112_MCTL_P3_ACLK, MCTL_ACLK_CFG0, 28, 4, 0, SYNC_INVALID) \
	DIV(TS_CLK_TX5112_MCTL_P2_ACLK, MCTL_ACLK_CFG0, 24, 4, 0, SYNC_INVALID) \
	DIV(TS_CLK_TX5112_MCTL_P1_ACLK, MCTL_ACLK_CFG0, 20, 4, 0, SYNC_INVALID) \
	DIV(TS_CLK_TX5112_MCTL_P0_ACLK, MCTL_ACLK_CFG0, 16, 4, 0, SYNC_INVALID) \
	DIV(TS_CLK_TX5112_VI_DDR_ACLK, MCTL_ACLK_CFG0, 12, 4, 0, SYNC_INVALID) \
	DIV(TS_CLK_TX5112_AI_DDR_ACLK, RNE_CCLK_CFG, 16, 5, 0, SYNC_INVALID) \
	DIV(TS_CLK_TX5112_ISP_SCLK, ISP_SCLK_CFG, 16, 4, 0, SYNC_INVALID) \
	DIV(TS_CLK_TX5112_VPE_CCLK, VPE_CCLK_CFG, 16, 4, 0, SYNC_INVALID) \
	DIV(TS_CLK_TX5112_MIPI_TXCLKESC, MIPI_CLK_CFG, 16, 10, 0, SYNC_INVALID) \
	DIV(TS_CLK_TX5112_AMR_CCLK, AMR_CCLK_CFG, 16, 5, 0, SYNC_INVALID) \
	DIV(TS_CLK_TX5112_RNE_ACLK, RNE_CCLK_CFG, 24, 4, 0, SYNC_INVALID) \
	DIV(TS_CLK_TX5112_RNE_CCLK, RNE_CCLK_CFG, 16, 4, 0, SYNC_INVALID) \
	DIV(TS_CLK_TX5112_H265_CCLK, VPU_CLK_CFG, 22, 6, 0, SYNC_INVALID) \
	DIV(TS_CLK_TX5112_VPU_ACLK, VPU_CLK_CFG, 16, 6, 0, SYNC_INVALID) \

#define _DIVIFY(id) TS_CLK_TX5112_DIV_##id
#define DIVIFY(id) _DIVIFY(id)

enum ts_div_ids {
#define DIV_FLAGS(id, ...) DIVIFY(id),
	DIV_LIST
#undef DIV_FLAGS
	TS_CLK_DIV_NONE,
};

static const struct ts_div_params ts_tx5112_divs[] = {
#define DIV_FLAGS(id, _off, _shift, _width, _we, _sync, _flags) \
	[DIVIFY(id)] = { \
		.off = (_off), \
		.shift = (_shift), \
		.width = (_width), \
		.we = (_we), \
		.sync = (_sync), \
		.flags = (_flags), \
	},
	DIV_LIST
#undef DIV_FLAGS
};

#undef DIV
#undef DIV_LIST

#define GATE_LIST \
	GATE(TS_CLK_TX5112_SKE_CLK, BUS_CLK_AXI_EN, 4, 0) \
	GATE(TS_CLK_TX5112_OCRAM_ACLK, BUS_CLK_AXI_EN, 3, 0) \
	GATE(TS_CLK_TX5112_MEM_DMA_ACLK, BUS_CLK_AXI_EN, 2, 0) \
	GATE(TS_CLK_TX5112_AUD_CODEC_HCLK, BUS_CLK_AHB_EN, 10, 0) \
	GATE(TS_CLK_TX5112_OSPI_HCLK, BUS_CLK_AHB_EN, 9, 0) \
	GATE(TS_CLK_TX5112_GMAC_HCLK, BUS_CLK_AHB_EN, 8, 0) \
	GATE(TS_CLK_TX5112_USB2C_HCLK, BUS_CLK_AHB_EN, 7, 0) \
	GATE(TS_CLK_TX5112_SDHC1_HCLK, BUS_CLK_AHB_EN, 6, 0) \
	GATE(TS_CLK_TX5112_SDHC0_HCLK, BUS_CLK_AHB_EN, 5, 0) \
	GATE(TS_CLK_TX5112_PERI_DMA1_HCLK, BUS_CLK_AHB_EN, 4, 0) \
	GATE(TS_CLK_TX5112_PERI_DMA0_HCLK, BUS_CLK_AHB_EN, 3, 0) \
	GATE(TS_CLK_TX5112_BOOTROM_HCLK, BUS_CLK_AHB_EN, 2, 0) \
	GATE(TS_CLK_TX5112_GPIO_B_PCLK, BUS_CLK_APB_EN, 31, 4) \
	GATE(TS_CLK_TX5112_GPIO_A_PCLK, BUS_CLK_APB_EN, 30, 4) \
	GATE(TS_CLK_TX5112_PDM_PCLK, BUS_CLK_APB_EN, 29, 3) \
	GATE(TS_CLK_TX5112_SYS_REG_PCLK, BUS_CLK_APB_EN, 27, 9) \
	GATE(TS_CLK_TX5112_OTPC_PCLK, BUS_CLK_APB_EN, 26, 9) \
	GATE(TS_CLK_TX5112_OSPI_PCLK, BUS_CLK_APB_EN, 25, 8) \
	GATE(TS_CLK_TX5112_PWM_PCLK, BUS_CLK_APB_EN, 24, 7) \
	GATE(TS_CLK_TX5112_TMR_PCLK, BUS_CLK_APB_EN, 23, 6) \
	GATE(TS_CLK_TX5112_WDT_PCLK, BUS_CLK_APB_EN, 22, 5) \
	GATE(TS_CLK_TX5112_GPIO_PCLK, BUS_CLK_APB_EN, 21, 4) \
	GATE(TS_CLK_TX5112_ADC_PCLK, BUS_CLK_APB_EN, 20, 3) \
	GATE(TS_CLK_TX5112_I2S0_PCLK, BUS_CLK_APB_EN, 19, 3) \
	GATE(TS_CLK_TX5112_USI1_PCLK, BUS_CLK_APB_EN, 18, 2) \
	GATE(TS_CLK_TX5112_USI0_PCLK, BUS_CLK_APB_EN, 17, 2) \
	GATE(TS_CLK_TX5112_UART1_PCLK, BUS_CLK_APB_EN, 15, 1) \
	GATE(TS_CLK_TX5112_UART0_PCLK, BUS_CLK_APB_EN, 14, 1) \
	GATE(TS_CLK_TX5112_SD0_CCLK_SMPL, SDHC0_CCLK_CFG, 12, 0) \
	GATE(TS_CLK_TX5112_SD0_CCLK_DRV, SDHC0_CCLK_CFG, 11, 0) \
	GATE(TS_CLK_TX5112_SD0_CCLK_SMP_EDGE, SDHC0_CCLK_CFG, 10, 0) \
	GATE(TS_CLK_TX5112_SD0_CCLK_DRV_EDGE, SDHC0_CCLK_CFG, 9, 0) \
	GATE(TS_CLK_TX5112_SD0_CCLK, SDHC0_CCLK_CFG, 2, 0) \
	GATE(TS_CLK_TX5112_SD1_CCLK_SMPL, SDHC1_CCLK_CFG, 12, 0) \
	GATE(TS_CLK_TX5112_SD1_CCLK_DRV, SDHC1_CCLK_CFG, 11, 0) \
	GATE(TS_CLK_TX5112_SD1_CCLK_SMP_EDGE, SDHC1_CCLK_CFG, 10, 0) \
	GATE(TS_CLK_TX5112_SD1_CCLK_DRV_EDGE, SDHC1_CCLK_CFG, 9, 0) \
	GATE(TS_CLK_TX5112_SD1_CCLK, SDHC1_CCLK_CFG, 2, 0) \
	GATE(TS_CLK_TX5112_OSPI_REF_CLK, OSPI_RCLK_CFG, 2, 0) \
	GATE(TS_CLK_TX5112_USB_REF_CLK, USB_PHY_CLK_CFG, 2, 0) \
	GATE(TS_CLK_TX5112_I2C1_PCLK, I2C_ICCLK_CFG0, 3, 0) \
	GATE(TS_CLK_TX5112_I2C0_PCLK, I2C_ICCLK_CFG0, 2, 0) \
	GATE(TS_CLK_TX5112_I2C2_PCLK, I2C_ICCLK_CFG1, 3, 0) \
	GATE(TS_CLK_TX5112_SPI_PCLK, SPI_SSICLK_CFG, 3, 0) \
	GATE(TS_CLK_TX5112_TMR_T4CLK, TMR_TCLK_CFG0, 3, 0) \
	GATE(TS_CLK_TX5112_TMR_T3CLK, TMR_TCLK_CFG0, 2, 0) \
	GATE(TS_CLK_TX5112_TMR_T6CLK, TMR_TCLK_CFG1, 3, 0) \
	GATE(TS_CLK_TX5112_TMR_T5CLK, TMR_TCLK_CFG1, 2, 0) \
	GATE(TS_CLK_TX5112_TMR_T8CLK, TMR_TCLK_CFG2, 3, 0) \
	GATE(TS_CLK_TX5112_TMR_T7CLK, TMR_TCLK_CFG2, 2, 0) \
	GATE(TS_CLK_TX5112_I2S0_OCLK_O, I2S0_MCLK_CFG, 10, 0) \
	GATE(TS_CLK_TX5112_I2S0_OCLK, I2S0_MCLK_CFG, 3, 0) \
	GATE(TS_CLK_TX5112_I2S0_MCLK, I2S0_MCLK_CFG, 2, 0) \
	GATE(TS_CLK_TX5112_PDM_MCLK, PDM_MCLK_CFG, 2, 0) \
	GATE(TS_CLK_TX5112_AUD_DAC_PBCLK_INV, AUD_DAC_CLK_CFG0, 7, 0) \
	GATE(TS_CLK_TX5112_AUD_DAC_CCLK, AUD_DAC_CLK_CFG0, 3, 0) \
	GATE(TS_CLK_TX5112_AUD_DAC_PBCLK, AUD_DAC_CLK_CFG0, 2, 0) \
	GATE(TS_CLK_TX5112_AUD_TMR_STRB, AUD_DAC_CLK_CFG1, 2, 0) \
	GATE(TS_CLK_TX5112_AUD_ADC_CCLK, AUD_ADC_CLK_CFG, 2, 0) \
	GATE(TS_CLK_TX5112_VI_DDR_ACLK, MCTL_ACLK_CFG0, 11, 0) \
	GATE(TS_CLK_TX5112_AI_DDR_ACLK, RNE_CCLK_CFG, 3, 0) \
	GATE(TS_CLK_TX5112_DDRC_CORE_CLK, MCTL_ACLK_CFG0, 8, 0) \
	GATE(TS_CLK_TX5112_DDR_PHY_PCLK, MCTL_ACLK_CFG0, 7, 0) \
	GATE(TS_CLK_TX5112_UMCTL_PCLK, MCTL_ACLK_CFG0, 6, 0) \
	GATE(TS_CLK_TX5112_MCTL_P3_ACLK, MCTL_ACLK_CFG0, 5, 0) \
	GATE(TS_CLK_TX5112_MCTL_P2_ACLK, MCTL_ACLK_CFG0, 4, 0) \
	GATE(TS_CLK_TX5112_MCTL_P1_ACLK, MCTL_ACLK_CFG0, 3, 0) \
	GATE(TS_CLK_TX5112_MCTL_P0_ACLK, MCTL_ACLK_CFG0, 2, 0) \
	GATE(TS_CLK_TX5112_HDR_SCLK, VI_BUS_CLK_EN, 16, 0) \
	GATE(TS_CLK_TX5112_VPE_ISP_CLK, VI_BUS_CLK_EN, 15, 0) \
	GATE(TS_CLK_TX5112_MIPI_RX1_PIXCLK, VI_BUS_CLK_EN, 14, 0) \
	GATE(TS_CLK_TX5112_MIPI_RX0_PIXCLK1, VI_BUS_CLK_EN, 13, 0) \
	GATE(TS_CLK_TX5112_MIPI_RX0_PIXCLK0, VI_BUS_CLK_EN, 12, 0) \
	GATE(TS_CLK_TX5112_MIPI_RX1_PCLK, VI_BUS_CLK_EN, 10, 0) \
	GATE(TS_CLK_TX5112_MIPI_RX0_PCLK, VI_BUS_CLK_EN, 9, 0) \
	GATE(TS_CLK_TX5112_HDR_HCLK, VI_BUS_CLK_EN, 8, 0) \
	GATE(TS_CLK_TX5112_VPE_HCLK, VI_BUS_CLK_EN, 7, 0) \
	GATE(TS_CLK_TX5112_VI_CFG_HCLK, VI_BUS_CLK_EN, 6, 0) \
	GATE(TS_CLK_TX5112_HDR_ACLK, VI_BUS_CLK_EN, 5, 0) \
	GATE(TS_CLK_TX5112_VPE_ACLK, VI_BUS_CLK_EN, 4, 0) \
	GATE(TS_CLK_TX5112_ISP_ACLK, VI_BUS_CLK_EN, 3, 0) \
	GATE(TS_CLK_TX5112_ISP_SCLK, ISP_SCLK_CFG, 2, 0) \
	GATE(TS_CLK_TX5112_VPE_CCLK, VPE_CCLK_CFG, 2, 0) \
	GATE(TS_CLK_TX5112_MIPI_TXCLKESC, MIPI_CLK_CFG, 2, 0) \
	GATE(TS_CLK_TX5112_AMR_HCLK, AMR_CCLK_CFG, 4, 0) \
	GATE(TS_CLK_TX5112_AMR_ACLK, AMR_CCLK_CFG, 3, 0) \
	GATE(TS_CLK_TX5112_AMR_CCLK, AMR_CCLK_CFG, 2, 0) \
	GATE(TS_CLK_TX5112_RNE_CLK, RNE_CCLK_CFG, 6, 0) \
	GATE(TS_CLK_TX5112_AI_ACLK, RNE_CCLK_CFG, 5, 0) \
	GATE(TS_CLK_TX5112_RNE_HCLK, RNE_CCLK_CFG, 4, 0) \
	GATE(TS_CLK_TX5112_RNE_ACLK, RNE_CCLK_CFG, 3, 0) \
	GATE(TS_CLK_TX5112_RNE_CCLK, RNE_CCLK_CFG, 2, 0) \
	GATE(TS_CLK_TX5112_VPU_HCLK, VPU_CLK_CFG, 7, 0) \
	GATE(TS_CLK_TX5112_H265_CCLK, VPU_CLK_CFG, 6, 0) \
	GATE(TS_CLK_TX5112_H265_PCLK, VPU_CLK_CFG, 5, 0) \
	GATE(TS_CLK_TX5112_H265_ACLK, VPU_CLK_CFG, 4, 0) \
	GATE(TS_CLK_TX5112_H264_PCLK, VPU_CLK_CFG, 3, 0) \
	GATE(TS_CLK_TX5112_H264_ACLK, VPU_CLK_CFG, 2, 0) \
	GATE(TS_CLK_TX5112_CHIP_OCLK_I3, CHIP_OCLK_CFG, 5, 0) \
	GATE(TS_CLK_TX5112_CHIP_OCLK_I2, CHIP_OCLK_CFG, 4, 0) \
	GATE(TS_CLK_TX5112_CHIP_OCLK_I1, CHIP_OCLK_CFG, 3, 0) \
	GATE(TS_CLK_TX5112_CHIP_OCLK_I0, CHIP_OCLK_CFG, 2, 0)

#define _GATEIFY(id) TS_CLK_TX5112_GATE_##id
#define GATEIFY(id) _GATEIFY(id)

enum ts_gate_ids {
#define GATE(id, ...) GATEIFY(id),
	GATE_LIST
#undef GATE
	TS_CLK_GATE_NONE,
};

static const struct ts_gate_params ts_tx5112_gates[] = {
#define GATE(id, _off, _idx, _we) \
	[GATEIFY(id)] = { \
		.off = (_off), \
		.bit_idx = (_idx), \
		.we = (_we), \
	},
	GATE_LIST
#undef GATE
};

#undef GATE_LIST

#define MUX(id, reg, shift, width, we, sync) \
	MUX_PARENTS(id, generic_sels, reg, shift, width, we, sync)
#define MUX_LIST \
	MUX_PARENTS(TS_CLK_TX5112_CPU_CCLK, cpu_cclk_sels, CPU_CLK_CFG0, 11, 1, 0, SYNC_INVALID) \
	MUX_PARENTS(TS_CLK_TX5112_AXI_AHB_APB_CLK, top_axi_ahb_apb_clk_sels, BUS_CLK_CFG0, 8, 1, 0, SYNC_INVALID) \
	MUX_PARENTS(TS_CLK_TX5112_CORE_APB_CLK, top_axi_ahb_apb_clk_sels, BUS_CLK_CFG0, 8, 1, 0, SYNC_INVALID) \
	MUX_PARENTS(TS_CLK_TX5112_CORE_AHB_CLK, top_axi_ahb_apb_clk_sels, BUS_CLK_CFG0, 8, 1, 0, SYNC_INVALID) \
	MUX_PARENTS(TS_CLK_TX5112_CORE_AXI_CLK, top_axi_ahb_apb_clk_sels, BUS_CLK_CFG0, 8, 1, 0, SYNC_INVALID) \
	MUX_PARENTS(TS_CLK_TX5112_SD0_CCLK, sd0_cclk_sels, SDHC0_CCLK_CFG, 8, 1, 0, SYNC_INVALID) \
	MUX_PARENTS(TS_CLK_TX5112_SD0_CCLK_SMPL, sd0_cclk_smpl_sels, SD_CLK_OFFSET, 20, 4, WE_INVALID, SYNC_INVALID) \
	MUX_PARENTS(TS_CLK_TX5112_SD0_CCLK_DRV, sd0_cclk_drv_sels, SD_CLK_OFFSET, 16, 4, WE_INVALID, SYNC_INVALID) \
	MUX_PARENTS(TS_CLK_TX5112_SD1_CCLK, sd1_cclk_sels, SDHC1_CCLK_CFG, 8, 1, 0, SYNC_INVALID) \
	MUX_PARENTS(TS_CLK_TX5112_SD1_CCLK_SMPL, sd1_cclk_smpl_sels, SD_CLK_OFFSET, 28, 4, WE_INVALID, SYNC_INVALID) \
	MUX_PARENTS(TS_CLK_TX5112_SD1_CCLK_DRV, sd1_cclk_drv_sels, SD_CLK_OFFSET, 24, 4, WE_INVALID, SYNC_INVALID) \
	MUX_PARENTS(TS_CLK_TX5112_OSPI_REF_CLK, ospi_ref_clk_sels, OSPI_RCLK_CFG, 8, 1, 0, SYNC_INVALID) \
	MUX_PARENTS(TS_CLK_TX5112_I2C1_ICCLK, i2c_icclk_sels, I2C_ICCLK_CFG0, 10, 1, 0, SYNC_INVALID) \
	MUX_PARENTS(TS_CLK_TX5112_I2C0_ICCLK, i2c_icclk_sels, I2C_ICCLK_CFG0, 8, 1, 0, SYNC_INVALID) \
	MUX_PARENTS(TS_CLK_TX5112_I2C2_ICCLK, i2c_icclk_sels, I2C_ICCLK_CFG1, 8, 1, 0, SYNC_INVALID) \
	MUX_PARENTS(TS_CLK_TX5112_UART0_SCLK, uart_sclk_sels, UART0_SCLK_CFG, 31, 1, 0, SYNC_INVALID) \
	MUX_PARENTS(TS_CLK_TX5112_UART1_SCLK, uart_sclk_sels, UART1_SCLK_CFG, 31, 1, 0, SYNC_INVALID) \
	MUX_PARENTS(TS_CLK_TX5112_SPI_SSICLK, spi_ssiiclk_sels, SPI_SSICLK_CFG, 8, 1, 0, SYNC_INVALID) \
	MUX_PARENTS(TS_CLK_TX5112_RNE_CCLK, rne_cclk_sels, RNE_CCLK_CFG, 11, 1, 0, SYNC_INVALID) \
	MUX_PARENTS(TS_CLK_TX5112_RNE_CLK, rne_cclk_sels, RNE_CCLK_CFG, 10, 1, 0, SYNC_INVALID) \
	MUX_PARENTS(TS_CLK_TX5112_H265_CCLK, h265_cclk_sels, VPU_CLK_CFG, 12, 1, 0, SYNC_INVALID) \
	MUX_PARENTS(TS_CLK_TX5112_CHIP_OCLK_I0, chip_out_clk_sels, CHIP_OCLK_CFG, 16, 2, 0, SYNC_INVALID) \
	MUX_PARENTS(TS_CLK_TX5112_CHIP_OCLK_I1, chip_out_clk_sels, CHIP_OCLK_CFG, 20, 2, 0, SYNC_INVALID) \
	MUX_PARENTS(TS_CLK_TX5112_CHIP_OCLK_I2, chip_out_clk_sels, CHIP_OCLK_CFG, 24, 2, 0, SYNC_INVALID) \
	MUX_PARENTS(TS_CLK_TX5112_CHIP_OCLK_I3, chip_out_clk_sels, CHIP_OCLK_CFG, 28, 2, 0, SYNC_INVALID) \

#define _MUXIFY(id) TS_CLK_TX5112_MUX_##id
#define MUXIFY(id) _MUXIFY(id)

enum ts_mux_ids {
#define MUX_PARENTS(id, ...) MUXIFY(id),
	MUX_LIST
#undef MUX_PARENTS
	TS_CLK_MUX_NONE,
};


static const struct ts_mux_params ts_tx5112_muxes[] = {
#define MUX_PARENTS(id, parents, _off, _shift, _width, _we, _sync) \
	[MUXIFY(id)] = { \
		.parent_names = (const char * const *)(parents), \
		.num_parents = ARRAY_SIZE(parents), \
		.off = (_off), \
		.shift = (_shift), \
		.width = (_width), \
		.we = (_we), \
		.sync = (_sync), \
	},
	MUX_LIST
#undef MUX_PARENTS
};

#undef MUX
#undef MUX_LIST


static struct ts_pll_params ts_tx5112_plls[] = {
#define PLL(_name, _id, _off, _shift, _width) { \
	.name = _name, \
	.id = _id, \
	.off = (_off), \
	.lock_off = PLL_STATUS, \
	.shift = (_shift), \
	.width = (_width), \
}
	[0] = PLL("pll0", TS_CLK_TX5112_PLL0, PLL0_CFG0, 0, 1),
	[1] = PLL("pll1", TS_CLK_TX5112_PLL1, PLL1_CFG0, 1, 1),
	[2] = PLL("pll2", TS_CLK_TX5112_PLL2, PLL2_CFG0, 2, 1),
#undef PLL
};

#define COMP(id) \
	COMP_FULL(id, MUXIFY(id), DIVIFY(id), GATEIFY(id))
#define COMP_NOMUX(id) \
	COMP_FULL(id, TS_CLK_MUX_NONE, DIVIFY(id), GATEIFY(id))
#define COMP_NOGATE(id) \
	COMP_FULL(id, MUXIFY(id), DIVIFY(id), TS_CLK_GATE_NONE)
#define COMP_NODIV(id) \
	COMP_FULL(id, MUXIFY(id), TS_CLK_DIV_NONE, GATEIFY(id))
#define COMP_MUX_ONLY(id) \
	COMP_FULL(id, MUXIFY(id), TS_CLK_DIV_NONE, TS_CLK_GATE_NONE)
#define COMP_DIV_ONLY(id) \
	COMP_FULL(id, TS_CLK_MUX_NONE, DIVIFY(id), TS_CLK_GATE_NONE)
#define COMP_GATE_ONLY(id) \
	COMP_FULL(id, TS_CLK_MUX_NONE, TS_CLK_DIV_NONE, GATEIFY(id))
#define COMP_LIST \
	COMP_NOGATE(TS_CLK_TX5112_CPU_CCLK) \
	COMP_DIV_ONLY(TS_CLK_TX5112_CPU_ACLK) \
	COMP_DIV_ONLY(TS_CLK_TX5112_CPU_DBG_PCLK) \
	COMP_NOGATE(TS_CLK_TX5112_CORE_APB_CLK) \
	COMP_NOGATE(TS_CLK_TX5112_CORE_AHB_CLK) \
	COMP_NOGATE(TS_CLK_TX5112_CORE_AXI_CLK) \
	COMP_GATE_ONLY(TS_CLK_TX5112_SKE_CLK) \
	COMP_GATE_ONLY(TS_CLK_TX5112_OCRAM_ACLK) \
	COMP_GATE_ONLY(TS_CLK_TX5112_MEM_DMA_ACLK) \
	COMP_GATE_ONLY(TS_CLK_TX5112_AUD_CODEC_HCLK) \
	COMP_GATE_ONLY(TS_CLK_TX5112_OSPI_HCLK) \
	COMP_GATE_ONLY(TS_CLK_TX5112_USB2C_HCLK) \
	COMP_GATE_ONLY(TS_CLK_TX5112_SDHC1_HCLK) \
	COMP_GATE_ONLY(TS_CLK_TX5112_SDHC0_HCLK) \
	COMP_GATE_ONLY(TS_CLK_TX5112_PERI_DMA1_HCLK) \
	COMP_GATE_ONLY(TS_CLK_TX5112_PERI_DMA0_HCLK) \
	COMP_GATE_ONLY(TS_CLK_TX5112_BOOTROM_HCLK) \
	COMP_GATE_ONLY(TS_CLK_TX5112_GPIO_B_PCLK) \
	COMP_GATE_ONLY(TS_CLK_TX5112_GPIO_A_PCLK) \
	COMP_GATE_ONLY(TS_CLK_TX5112_PDM_PCLK) \
	COMP_GATE_ONLY(TS_CLK_TX5112_SYS_REG_PCLK) \
	COMP_GATE_ONLY(TS_CLK_TX5112_OTPC_PCLK) \
	COMP_GATE_ONLY(TS_CLK_TX5112_OSPI_PCLK) \
	COMP_GATE_ONLY(TS_CLK_TX5112_PWM_PCLK) \
	COMP_GATE_ONLY(TS_CLK_TX5112_TMR_PCLK) \
	COMP_GATE_ONLY(TS_CLK_TX5112_WDT_PCLK) \
	COMP_GATE_ONLY(TS_CLK_TX5112_GPIO_PCLK) \
	COMP_GATE_ONLY(TS_CLK_TX5112_ADC_PCLK) \
	COMP_GATE_ONLY(TS_CLK_TX5112_I2S0_PCLK) \
	COMP_GATE_ONLY(TS_CLK_TX5112_USI1_PCLK) \
	COMP_GATE_ONLY(TS_CLK_TX5112_USI0_PCLK) \
	COMP_GATE_ONLY(TS_CLK_TX5112_UART1_PCLK) \
	COMP_GATE_ONLY(TS_CLK_TX5112_UART0_PCLK) \
	COMP(TS_CLK_TX5112_SD0_CCLK) \
	COMP_NODIV(TS_CLK_TX5112_SD0_CCLK_SMPL) \
	COMP_NODIV(TS_CLK_TX5112_SD0_CCLK_DRV) \
	COMP_GATE_ONLY(TS_CLK_TX5112_SD0_CCLK_SMP_EDGE) \
	COMP_GATE_ONLY(TS_CLK_TX5112_SD0_CCLK_DRV_EDGE) \
	COMP(TS_CLK_TX5112_SD1_CCLK) \
	COMP_NODIV(TS_CLK_TX5112_SD1_CCLK_SMPL) \
	COMP_NODIV(TS_CLK_TX5112_SD1_CCLK_DRV) \
	COMP_GATE_ONLY(TS_CLK_TX5112_SD1_CCLK_SMP_EDGE) \
	COMP_GATE_ONLY(TS_CLK_TX5112_SD1_CCLK_DRV_EDGE) \
	COMP(TS_CLK_TX5112_OSPI_REF_CLK) \
	COMP_NOGATE(TS_CLK_TX5112_I2C1_ICCLK) \
	COMP_NOGATE(TS_CLK_TX5112_I2C0_ICCLK) \
	COMP_GATE_ONLY(TS_CLK_TX5112_I2C1_PCLK) \
	COMP_GATE_ONLY(TS_CLK_TX5112_I2C0_PCLK) \
	COMP_NOGATE(TS_CLK_TX5112_I2C2_ICCLK) \
	COMP_GATE_ONLY(TS_CLK_TX5112_I2C2_PCLK) \
	COMP_NOGATE(TS_CLK_TX5112_UART0_SCLK) \
	COMP_NOGATE(TS_CLK_TX5112_UART1_SCLK) \
	COMP_NOGATE(TS_CLK_TX5112_SPI_SSICLK) \
	COMP_GATE_ONLY(TS_CLK_TX5112_SPI_PCLK) \
	COMP_NOMUX(TS_CLK_TX5112_TMR_T4CLK) \
	COMP_NOMUX(TS_CLK_TX5112_TMR_T3CLK) \
	COMP_NOMUX(TS_CLK_TX5112_TMR_T6CLK) \
	COMP_NOMUX(TS_CLK_TX5112_TMR_T5CLK) \
	COMP_NOMUX(TS_CLK_TX5112_TMR_T8CLK) \
	COMP_NOMUX(TS_CLK_TX5112_TMR_T7CLK) \
	COMP_NOMUX(TS_CLK_TX5112_I2S0_OCLK) \
	COMP_NOMUX(TS_CLK_TX5112_I2S0_MCLK) \
	COMP_GATE_ONLY(TS_CLK_TX5112_I2S0_OCLK_O) \
	COMP_NOMUX(TS_CLK_TX5112_PDM_MCLK) \
	COMP_NOMUX(TS_CLK_TX5112_AUD_DAC_PBCLK) \
	COMP_NOMUX(TS_CLK_TX5112_AUD_DAC_CCLK) \
	COMP_GATE_ONLY(TS_CLK_TX5112_AUD_DAC_PBCLK_INV) \
	COMP_DIV_ONLY(TS_CLK_TX5112_ED_TMR_STRB) \
	COMP_GATE_ONLY(TS_CLK_TX5112_AUD_TMR_STRB) \
	COMP_NOMUX(TS_CLK_TX5112_AUD_ADC_CCLK) \
	COMP_NOMUX(TS_CLK_TX5112_MCTL_P3_ACLK) \
	COMP_NOMUX(TS_CLK_TX5112_MCTL_P2_ACLK) \
	COMP_NOMUX(TS_CLK_TX5112_MCTL_P1_ACLK) \
	COMP_NOMUX(TS_CLK_TX5112_MCTL_P0_ACLK) \
	COMP_NOMUX(TS_CLK_TX5112_VI_DDR_ACLK) \
	COMP_NOMUX(TS_CLK_TX5112_AI_DDR_ACLK) \
	COMP_GATE_ONLY(TS_CLK_TX5112_DDRC_CORE_CLK) \
	COMP_GATE_ONLY(TS_CLK_TX5112_DDR_PHY_PCLK) \
	COMP_GATE_ONLY(TS_CLK_TX5112_UMCTL_PCLK) \
	COMP_GATE_ONLY(TS_CLK_TX5112_HDR_SCLK) \
	COMP_GATE_ONLY(TS_CLK_TX5112_VPE_ISP_CLK) \
	COMP_GATE_ONLY(TS_CLK_TX5112_MIPI_RX1_PIXCLK) \
	COMP_GATE_ONLY(TS_CLK_TX5112_MIPI_RX0_PIXCLK1) \
	COMP_GATE_ONLY(TS_CLK_TX5112_MIPI_RX0_PIXCLK0) \
	COMP_GATE_ONLY(TS_CLK_TX5112_MIPI_RX1_PCLK) \
	COMP_GATE_ONLY(TS_CLK_TX5112_MIPI_RX0_PCLK) \
	COMP_GATE_ONLY(TS_CLK_TX5112_HDR_HCLK) \
	COMP_GATE_ONLY(TS_CLK_TX5112_VPE_HCLK) \
	COMP_GATE_ONLY(TS_CLK_TX5112_VI_CFG_HCLK) \
	COMP_GATE_ONLY(TS_CLK_TX5112_HDR_ACLK) \
	COMP_GATE_ONLY(TS_CLK_TX5112_VPE_ACLK) \
	COMP_GATE_ONLY(TS_CLK_TX5112_ISP_ACLK) \
	COMP_NOMUX(TS_CLK_TX5112_ISP_SCLK) \
	COMP_NOMUX(TS_CLK_TX5112_VPE_CCLK) \
	COMP_NOMUX(TS_CLK_TX5112_MIPI_TXCLKESC) \
	COMP_NOMUX(TS_CLK_TX5112_AMR_CCLK) \
	COMP_GATE_ONLY(TS_CLK_TX5112_AMR_HCLK) \
	COMP_GATE_ONLY(TS_CLK_TX5112_AMR_ACLK) \
	COMP_NOMUX(TS_CLK_TX5112_RNE_ACLK) \
	COMP(TS_CLK_TX5112_RNE_CCLK) \
	COMP_NODIV(TS_CLK_TX5112_RNE_CLK) \
	COMP_GATE_ONLY(TS_CLK_TX5112_AI_ACLK) \
	COMP_GATE_ONLY(TS_CLK_TX5112_RNE_HCLK) \
	COMP(TS_CLK_TX5112_H265_CCLK) \
	COMP_DIV_ONLY(TS_CLK_TX5112_VPU_ACLK) \
	COMP_GATE_ONLY(TS_CLK_TX5112_VPU_HCLK) \
	COMP_GATE_ONLY(TS_CLK_TX5112_H265_PCLK) \
	COMP_GATE_ONLY(TS_CLK_TX5112_H265_ACLK) \
	COMP_GATE_ONLY(TS_CLK_TX5112_H264_PCLK) \
	COMP_GATE_ONLY(TS_CLK_TX5112_H264_ACLK) \
	COMP_NODIV(TS_CLK_TX5112_CHIP_OCLK_I3) \
	COMP_NODIV(TS_CLK_TX5112_CHIP_OCLK_I2) \
	COMP_NODIV(TS_CLK_TX5112_CHIP_OCLK_I1) \
	COMP_NODIV(TS_CLK_TX5112_CHIP_OCLK_I0)

#define _COMPIFY(id) TS_CLK_COMP_##id
#define COMPIFY(id) _COMPIFY(id)

enum ts_comp_ids {
#define COMP_FULL(id, ...) COMPIFY(id),
	COMP_LIST
#undef COMP_FULL
};

static const struct ts_comp_params ts_tx5112_comps[] = {
#define COMP_FULL(id, _mux, _div, _gate) \
	[COMPIFY(id)] = { \
		.mux = (_mux), \
		.div = (_div), \
		.gate = (_gate), \
	},
	COMP_LIST
#undef COMP_FULL
};

#undef COMP
#undef COMP_ID
#undef COMP_NOMUX
#undef COMP_NOMUX_ID
#undef COMP_LIST

static int clk_tx5112_is_frac_divider(const char *name)
{
	u32 i;
	char *frac_div_arr[] = {"uart0_sclk", "uart1_sclk"};

	for (i = 0; i < ARRAY_SIZE(frac_div_arr); i++) {
		if (!strncmp(frac_div_arr[i], name, strlen(frac_div_arr[i])))
			return true;
	}
	return false;
}

static struct clk_div_table oclk_div_table[] = {
	{ 0, 1 }, { 1, 3 }, { 2, 5 }, { 3, 9 }
};
struct clk_div_table *clk_tx5112_get_div_table(const char *name)
{
	u32 i;
	char *selftable_div_arr[] = {"chip_oclk_i0", "chip_oclk_i1",
		"chip_oclk_i2", "chip_oclk_i3"};

	for (i = 0; i < ARRAY_SIZE(selftable_div_arr); i++) {
		if (!strncmp(selftable_div_arr[i], name, strlen(selftable_div_arr[i])))
			return oclk_div_table;
	}
	return NULL;
}


/* Create all sub-clocks, and then register the composite clock */
struct clk *clk_tx5112_register_comp(const struct ts_comp_params *params,
					void *base, const char *name,
					const char *parent)
{
	const char *const *parent_names;
	int num_parents;
	struct clk *comp;
	const struct clk_ops *mux_ops;
	const struct clk_ops *div_ops;
	const struct clk_ops *gate_ops;
	struct clk_mux *mux;
	struct clk_gate *gate;
	void *div;
	struct clk *clk;
	u32 div_id = 0;
	struct clk_div_table *table;

	if (params->mux == TS_CLK_MUX_NONE) {
		if (!parent) {
			//printf("params->mux:%d div:%d gate:%d \n",params->mux, params->div, params->gate);
			//printf("MUX_NONE with parent not NULL\n");
			return ERR_PTR(-EINVAL);
		}

		mux_ops = NULL;
		mux = NULL;
		parent_names = &parent;
		num_parents = 1;
	} else {
		mux_ops = &clk_mux_ops;
		mux = ts_create_mux(&ts_tx5112_muxes[params->mux], base);
		if (!mux) {
			printf("create clk %s mux %d failed\n", name, params->mux);
			return ERR_PTR(-ENOMEM);
		}

		parent_names = mux->parent_names;
		num_parents = mux->num_parents;
	}

	if (params->div == TS_CLK_DIV_NONE) {
		div_ops = NULL;
		div = NULL;
	} else {
		div_id = params->div;
		if (clk_tx5112_is_frac_divider(name)) {
			div_ops = &clk_fractional_divider_ops;
			div = (struct clk_fractional_divider *)ts_create_fractional_div(&ts_tx5112_divs[div_id], base);
			clk = &((struct clk_fractional_divider *)div)->clk;
		} else {
			div_ops = &clk_divider_ops;
			table = clk_tx5112_get_div_table(name);
			div = (struct clk_divider *)ts_create_div(&ts_tx5112_divs[div_id], base, table);
			clk = &((struct clk_divider *)div)->clk;
		}

		if (!div) {
			printf("create clk %s div %d failed\n", name, div_id);
			comp = ERR_PTR(-ENOMEM);
			goto cleanup_mux;
		}
	}

	if (params->gate == TS_CLK_GATE_NONE) {
		gate_ops = NULL;
		gate = NULL;
	} else {
		gate_ops = &clk_gate_ops;
		gate = ts_create_gate(&ts_tx5112_gates[params->gate], base);
		if (!gate) {
			printf("create clk %s gate %d failed\n", name, params->gate);
			comp = ERR_PTR(-ENOMEM);
			goto cleanup_div;
		}
	}

	comp = clk_register_composite(NULL, name, parent_names, num_parents,
					(mux ? &mux->clk : NULL), mux_ops,
					(div ? clk : NULL), div_ops,
					(gate ? &gate->clk : NULL), gate_ops, CLK_GET_RATE_NOCACHE);
	if (IS_ERR(comp)) {
		printf("register composite %s failed\n", name);
		goto cleanup_gate;
	}
	return comp;

cleanup_gate:
	if (gate)
		free(gate);
cleanup_div:
	if (div)
		free(div);
cleanup_mux:
	if (mux)
		free(mux);
	return comp;
}

static int clk_tx5112_register_plls(struct ts_clk_params *paras)
{
	void *top_base = paras->top_base;
	void __iomem *reg;
	void __iomem *lock;
	int i;

	for (i = 0; i < ARRAY_SIZE(ts_tx5112_plls); i++) {
		struct ts_pll_params *params = &ts_tx5112_plls[i];
		reg = top_base + params->off;
		lock = top_base + params->lock_off;

		clk_dm(params->id,
			ts_register_pll(params->name, &osc_in[0], reg, lock,
				params->shift, params->width, PLL_REG_MODE_COMPACT));
	}

	/* fix-frequency clocks for plls */
	clk_dm(TS_CLK_TX5112_PLL0_D2,
		ts_clk_fixed_divider("pll0_d2", "pll0", 2));
	clk_dm(TS_CLK_TX5112_PLL0_D4,
		ts_clk_fixed_divider("pll0_d4", "pll0", 4));
	clk_dm(TS_CLK_TX5112_PLL0_D5,
		ts_clk_fixed_divider("pll0_d5", "pll0", 5));
	clk_dm(TS_CLK_TX5112_PLL0_D8,
		ts_clk_fixed_divider("pll0_d8", "pll0", 8));
	clk_dm(TS_CLK_TX5112_PLL0_D10,
		ts_clk_fixed_divider("pll0_d10", "pll0", 10));
	clk_dm(TS_CLK_TX5112_PLL0_D27,
		ts_clk_fixed_divider("pll0_d27", "pll0", 27));
	clk_dm(TS_CLK_TX5112_PLL1_D3,
		ts_clk_fixed_divider("pll1_d3", "pll1", 3));
	clk_dm(TS_CLK_TX5112_PLL1_D4,
		ts_clk_fixed_divider("pll1_d4", "pll1", 4));
	clk_dm(TS_CLK_TX5112_PLL1_D6,
		ts_clk_fixed_divider("pll1_d6", "pll1", 6));

#if 0
	/* divider "div0_codec_dac_cclk" is parent of comp "aud_dac_cclk"), register here */
	const struct ts_comp_params *params;
	params = &ts_tx5112_comps[DIVIFY(TS_CLK_TX5112_DIV0_CODEC_DAC_CCLK)];
	clk_dm(TS_CLK_TX5112_DIV0_CODEC_DAC_CCLK,
		clk_tx5112_register_comp(params, top_base, "div0_codec_dac_cclk", "pll1"));

	/* divider "div0_codec_adc_cclk" is parent of comp "aud_adc_cclk"), register here */
	params = &ts_tx5112_comps[DIVIFY(TS_CLK_TX5112_DIV0_CODEC_ADC_CCLK)];
	clk_dm(TS_CLK_TX5112_DIV0_CODEC_ADC_CCLK,
		clk_tx5112_register_comp(params, top_base, "div0_codec_adc_cclk", "pll1"));
#endif
	return 0;
}

static int clk_tx5112_register_comps(struct ts_clk_params *paras)
{
	void *top_base = paras->top_base;

#define REGISTER_COMP(id, name) \
		clk_dm(id, \
			clk_tx5112_register_comp(&ts_tx5112_comps[COMPIFY(id)], \
							top_base, name, NULL))

	//printf("--TS_CLK_MUX_NONE  %d--\n", TS_CLK_MUX_NONE);
	//printf("--TS_CLK_DIV_NONE  %d--\n", TS_CLK_DIV_NONE);
	//printf("--TS_CLK_GATE_NONE %d--\n", TS_CLK_GATE_NONE);

	REGISTER_COMP(TS_CLK_TX5112_CPU_CCLK, "cpu_cclk");

	REGISTER_COMP(TS_CLK_TX5112_CORE_AHB_CLK, "core_ahb_clk");
	REGISTER_COMP(TS_CLK_TX5112_CORE_AXI_CLK, "core_axi_clk");
	REGISTER_COMP(TS_CLK_TX5112_CORE_APB_CLK, "core_apb_clk");

	REGISTER_COMP(TS_CLK_TX5112_SD0_CCLK, "sd0_cclk");
	clk_dm(TS_CLK_TX5112_SD0_CLK_SMP_SHIFT_ALIEND,
		ts_clk_fixed_divider("sd0_cclk_aligned", "sd0_cclk", 1));
	clk_dm(TS_CLK_TX5112_SD0_CLK_SMP_SHIFT_1_10,
		ts_clk_fixed_divider("sd0_cclk_shift_1_10", "sd0_cclk", 1));
	clk_dm(TS_CLK_TX5112_SD0_CLK_SMP_SHIFT_2_10,
		ts_clk_fixed_divider("sd0_cclk_shift_2_10", "sd0_cclk", 1));
	clk_dm(TS_CLK_TX5112_SD0_CLK_SMP_SHIFT_3_10,
		ts_clk_fixed_divider("sd0_cclk_shift_3_10", "sd0_cclk", 1));
	clk_dm(TS_CLK_TX5112_SD0_CLK_SMP_SHIFT_4_10,
		ts_clk_fixed_divider("sd0_cclk_shift_4_10", "sd0_cclk", 1));
	clk_dm(TS_CLK_TX5112_SD0_CLK_SMP_SHIFT_5_10,
		ts_clk_fixed_divider("sd0_cclk_shift_5_10", "sd0_cclk", 1));
	clk_dm(TS_CLK_TX5112_SD0_CLK_SMP_SHIFT_6_10,
		ts_clk_fixed_divider("sd0_cclk_shift_6_10", "sd0_cclk", 1));
	clk_dm(TS_CLK_TX5112_SD0_CLK_SMP_SHIFT_7_10,
		ts_clk_fixed_divider("sd0_cclk_shift_7_10", "sd0_cclk", 1));
	clk_dm(TS_CLK_TX5112_SD0_CLK_SMP_SHIFT_8_10,
		ts_clk_fixed_divider("sd0_cclk_shift_8_10", "sd0_cclk", 1));
	clk_dm(TS_CLK_TX5112_SD0_CLK_SMP_SHIFT_9_10,
		ts_clk_fixed_divider("sd0_cclk_shift_9_10", "sd0_cclk", 1));
	REGISTER_COMP(TS_CLK_TX5112_SD0_CCLK_SMPL, "sd0_cclk_smpl");
	REGISTER_COMP(TS_CLK_TX5112_SD0_CCLK_DRV, "sd0_cclk_drv");

	REGISTER_COMP(TS_CLK_TX5112_SD1_CCLK, "sd1_cclk");
	clk_dm(TS_CLK_TX5112_SD1_CLK_SMP_SHIFT_ALIEND,
		ts_clk_fixed_divider("sd1_cclk_aligned", "sd1_cclk", 1));
	clk_dm(TS_CLK_TX5112_SD1_CLK_SMP_SHIFT_1_10,
		ts_clk_fixed_divider("sd1_cclk_shift_1_10", "sd1_cclk", 1));
	clk_dm(TS_CLK_TX5112_SD1_CLK_SMP_SHIFT_2_10,
		ts_clk_fixed_divider("sd1_cclk_shift_2_10", "sd1_cclk", 1));
	clk_dm(TS_CLK_TX5112_SD1_CLK_SMP_SHIFT_3_10,
		ts_clk_fixed_divider("sd1_cclk_shift_3_10", "sd1_cclk", 1));
	clk_dm(TS_CLK_TX5112_SD1_CLK_SMP_SHIFT_4_10,
		ts_clk_fixed_divider("sd1_cclk_shift_4_10", "sd1_cclk", 1));
	clk_dm(TS_CLK_TX5112_SD1_CLK_SMP_SHIFT_5_10,
		ts_clk_fixed_divider("sd1_cclk_shift_5_10", "sd1_cclk", 1));
	clk_dm(TS_CLK_TX5112_SD1_CLK_SMP_SHIFT_6_10,
		ts_clk_fixed_divider("sd1_cclk_shift_6_10", "sd1_cclk", 1));
	clk_dm(TS_CLK_TX5112_SD1_CLK_SMP_SHIFT_7_10,
		ts_clk_fixed_divider("sd1_cclk_shift_7_10", "sd1_cclk", 1));
	clk_dm(TS_CLK_TX5112_SD1_CLK_SMP_SHIFT_8_10,
		ts_clk_fixed_divider("sd1_cclk_shift_8_10", "sd1_cclk", 1));
	clk_dm(TS_CLK_TX5112_SD1_CLK_SMP_SHIFT_9_10,
		ts_clk_fixed_divider("sd1_cclk_shift_9_10", "sd1_cclk", 1));
	REGISTER_COMP(TS_CLK_TX5112_SD1_CCLK_SMPL, "sd1_cclk_smpl");
	REGISTER_COMP(TS_CLK_TX5112_SD1_CCLK_DRV, "sd1_cclk_drv");

	REGISTER_COMP(TS_CLK_TX5112_OSPI_REF_CLK, "ospi_ref_clk");
	REGISTER_COMP(TS_CLK_TX5112_I2C1_ICCLK, "i2c1_icclk");
	REGISTER_COMP(TS_CLK_TX5112_I2C0_ICCLK, "i2c0_icclk");
	REGISTER_COMP(TS_CLK_TX5112_I2C2_ICCLK, "i2c2_icclk");
	REGISTER_COMP(TS_CLK_TX5112_UART0_SCLK, "uart0_sclk");
	REGISTER_COMP(TS_CLK_TX5112_UART1_SCLK, "uart1_sclk");
	REGISTER_COMP(TS_CLK_TX5112_SPI_SSICLK, "spi_ssiclk");
	REGISTER_COMP(TS_CLK_TX5112_RNE_CCLK, "rne_cclk");
	REGISTER_COMP(TS_CLK_TX5112_RNE_CCLK, "rne_clk");


	REGISTER_COMP(TS_CLK_TX5112_H265_CCLK, "h265_cclk");
	REGISTER_COMP(TS_CLK_TX5112_CHIP_OCLK_I0, "chip_oclk_i0");
	REGISTER_COMP(TS_CLK_TX5112_CHIP_OCLK_I1, "chip_oclk_i1");
	REGISTER_COMP(TS_CLK_TX5112_CHIP_OCLK_I2, "chip_oclk_i2");
	REGISTER_COMP(TS_CLK_TX5112_CHIP_OCLK_I3, "chip_oclk_i3");

#undef REGISTER_COMP

	return 0;
}

static int clk_tx5112_register_comps_without_mux(struct ts_clk_params *paras)
{
	void *top_base = paras->top_base;

	/* Dividing clocks, no mux */
#define REGISTER_COMP_NOMUX(id, name, parent) \
		clk_dm(id, \
			clk_tx5112_register_comp(&ts_tx5112_comps[COMPIFY(id)], \
				top_base, name, parent))

	REGISTER_COMP_NOMUX(TS_CLK_TX5112_TMR_T8CLK, "tmr_t8clk", "pll0_d2");
	REGISTER_COMP_NOMUX(TS_CLK_TX5112_TMR_T7CLK, "tmr_t7clk", "pll0_d2");
	REGISTER_COMP_NOMUX(TS_CLK_TX5112_TMR_T6CLK, "tmr_t6clk", "pll0_d2");
	REGISTER_COMP_NOMUX(TS_CLK_TX5112_TMR_T5CLK, "tmr_t5clk", "pll0_d2");
	REGISTER_COMP_NOMUX(TS_CLK_TX5112_TMR_T4CLK, "tmr_t4clk", &osc_in[0]);
	REGISTER_COMP_NOMUX(TS_CLK_TX5112_TMR_T3CLK, "tmr_t3clk", &osc_in[0]);
	REGISTER_COMP_NOMUX(TS_CLK_TX5112_I2S0_OCLK, "i2s0_oclk", "pll1");
	REGISTER_COMP_NOMUX(TS_CLK_TX5112_I2S0_MCLK, "i2s0_mclk", "pll1");
	REGISTER_COMP_NOMUX(TS_CLK_TX5112_PDM_MCLK, "pdm_mclk", "pll1");
	REGISTER_COMP_NOMUX(TS_CLK_TX5112_AUD_DAC_PBCLK, "aud_dac_pbclk", "pll1");
	REGISTER_COMP_NOMUX(TS_CLK_TX5112_AUD_DAC_CCLK, "aud_dac_cclk", "pll1");
	REGISTER_COMP_NOMUX(TS_CLK_TX5112_AUD_ADC_CCLK, "aud_adc_cclk", "pll1");
	REGISTER_COMP_NOMUX(TS_CLK_TX5112_MCTL_P3_ACLK, "mctl_p3_aclk", "pll0");
	REGISTER_COMP_NOMUX(TS_CLK_TX5112_MCTL_P2_ACLK, "mctl_p2_aclk", "pll0");
	REGISTER_COMP_NOMUX(TS_CLK_TX5112_MCTL_P1_ACLK, "mctl_p1_aclk", "pll0");
	REGISTER_COMP_NOMUX(TS_CLK_TX5112_MCTL_P0_ACLK, "mctl_p0_aclk", "pll0");
	REGISTER_COMP_NOMUX(TS_CLK_TX5112_VI_DDR_ACLK, "vi_ddr_aclk", "pll0");
	REGISTER_COMP_NOMUX(TS_CLK_TX5112_ISP_SCLK, "isp_sclk", "pll0");
	REGISTER_COMP_NOMUX(TS_CLK_TX5112_VPE_CCLK, "vpe_cclk", "pll0");
	REGISTER_COMP_NOMUX(TS_CLK_TX5112_MIPI_TXCLKESC, "mipi_txclkesc", "pll0_d2");
	REGISTER_COMP_NOMUX(TS_CLK_TX5112_AMR_CCLK, "amr_cclk", "pll0");
	REGISTER_COMP_NOMUX(TS_CLK_TX5112_RNE_ACLK, "rne_aclk", "pll0");

#undef REGISTER_COMP_NOMUX
	return 0;
}


static int clk_tx5112_register_dividers(struct ts_clk_params *paras)
{
	void *top_base = paras->top_base;

#define REGISTER_DIV(id, name, parent) \
		clk_dm(id, \
			clk_tx5112_register_comp(&ts_tx5112_comps[COMPIFY(id)], \
				top_base, name, parent))

	REGISTER_DIV(TS_CLK_TX5112_CPU_ACLK, "cpu_aclk", "cpu_cclk");
	REGISTER_DIV(TS_CLK_TX5112_CPU_DBG_PCLK, "cpu_dbg_pclk", "cpu_cclk");
	REGISTER_DIV(TS_CLK_TX5112_CORE_APB_CLK, "core_apb_clk", "pll0");
	REGISTER_DIV(TS_CLK_TX5112_CORE_AXI_CLK, "core_axi_clk", "pll0");
	REGISTER_DIV(TS_CLK_TX5112_CORE_AHB_CLK, "core_ahb_clk", "pll0");
	REGISTER_DIV(TS_CLK_TX5112_ED_TMR_STRB, "aud_ed_tmr_strb", "pll1");
	REGISTER_DIV(TS_CLK_TX5112_VPU_ACLK, "vpu_aclk", "pll0");

#undef REGISTER_DIV
	return 0;
}

static int clk_tx5112_register_gates(struct ts_clk_params *paras)
{
	void *top_base = paras->top_base;

#define REGISTER_GATE(id, name, parent) \
		clk_dm(id, \
			clk_tx5112_register_comp(&ts_tx5112_comps[COMPIFY(id)], \
				top_base, name, parent))

	REGISTER_GATE(TS_CLK_TX5112_SKE_CLK, "ske_axi_clk", "core_axi_clk");
	REGISTER_GATE(TS_CLK_TX5112_OCRAM_ACLK, "ocram_aclk", "core_axi_clk");
	REGISTER_GATE(TS_CLK_TX5112_MEM_DMA_ACLK, "mem_dma_aclk", "core_axi_clk");
	//REGISTER_GATE(TS_CLK_TX5112_AUD_CODEC_HCLK, "aud_codec_hclk", "top_ahb_clk");
	REGISTER_GATE(TS_CLK_TX5112_OSPI_HCLK, "ospi_hclk", "core_ahb_clk");
	REGISTER_GATE(TS_CLK_TX5112_USB2C_HCLK, "usb2c_hclk", "core_ahb_clk");
	REGISTER_GATE(TS_CLK_TX5112_SDHC1_HCLK, "sdhc1_hclk", "core_ahb_clk");
	REGISTER_GATE(TS_CLK_TX5112_SDHC0_HCLK, "sdhc0_hclk", "core_ahb_clk");
	REGISTER_GATE(TS_CLK_TX5112_PERI_DMA1_HCLK, "peri_dma1_hclk", "core_ahb_clk");
	REGISTER_GATE(TS_CLK_TX5112_PERI_DMA0_HCLK, "peri_dma0_hclk", "core_ahb_clk");
	REGISTER_GATE(TS_CLK_TX5112_BOOTROM_HCLK, "bootrom_hclk", "core_ahb_clk");

	REGISTER_GATE(TS_CLK_TX5112_GPIO_B_PCLK, "gpio_b_pclk", "core_apb_clk");
	REGISTER_GATE(TS_CLK_TX5112_GPIO_A_PCLK, "gpio_a_pclk", "core_apb_clk");
	REGISTER_GATE(TS_CLK_TX5112_PDM_PCLK, "pdm_pclk", "core_apb_clk");
	REGISTER_GATE(TS_CLK_TX5112_SYS_REG_PCLK, "sys_reg_pclk", "core_apb_clk");
	REGISTER_GATE(TS_CLK_TX5112_OTPC_PCLK, "otpc_pclk", "core_apb_clk");
	REGISTER_GATE(TS_CLK_TX5112_OSPI_PCLK, "ospi_pclk", "core_apb_clk");
	REGISTER_GATE(TS_CLK_TX5112_PWM_PCLK, "pwm_pclk", "core_apb_clk");
	REGISTER_GATE(TS_CLK_TX5112_TMR_PCLK, "tmr_pclk", "core_apb_clk");
	REGISTER_GATE(TS_CLK_TX5112_WDT_PCLK, "wdt_pclk", "core_apb_clk");
	REGISTER_GATE(TS_CLK_TX5112_GPIO_PCLK, "gpio_pclk", "core_apb_clk");
	REGISTER_GATE(TS_CLK_TX5112_ADC_PCLK, "adc_pclk", "core_apb_clk");
	REGISTER_GATE(TS_CLK_TX5112_I2S0_PCLK, "i2s0_pclk", "core_apb_clk");
	REGISTER_GATE(TS_CLK_TX5112_USI1_PCLK, "usi1_pclk", "core_apb_clk");
	REGISTER_GATE(TS_CLK_TX5112_USI0_PCLK, "usi0_pclk", "core_apb_clk");
	REGISTER_GATE(TS_CLK_TX5112_UART1_PCLK, "uart1_pclk", "core_apb_clk");
	REGISTER_GATE(TS_CLK_TX5112_UART0_PCLK, "uart0_pclk", "core_apb_clk");

	REGISTER_GATE(TS_CLK_TX5112_I2C2_PCLK, "i2c2_pclk", "core_apb_clk");
	REGISTER_GATE(TS_CLK_TX5112_I2C1_PCLK, "i2c1_pclk", "core_apb_clk");
	REGISTER_GATE(TS_CLK_TX5112_I2C0_PCLK, "i2c0_pclk", "core_apb_clk");
	REGISTER_GATE(TS_CLK_TX5112_SPI_PCLK, "spi_pclk", "core_apb_clk");
	REGISTER_GATE(TS_CLK_TX5112_AUD_DAC_PBCLK_INV, "aud_dac_pbclk_inv", "pll1");
	REGISTER_GATE(TS_CLK_TX5112_AUD_TMR_STRB, "aud_tmr_strb", "pll1");

	REGISTER_GATE(TS_CLK_TX5112_DDRC_CORE_CLK, "ddrc_core_clk", "pll0");//parent uncertain
	REGISTER_GATE(TS_CLK_TX5112_DDR_PHY_PCLK, "ddr_phy_pclk", "pll0");//parent uncertain
	REGISTER_GATE(TS_CLK_TX5112_UMCTL_PCLK, "umctl_pclk", "core_apb_clk");//parent uncertain
	REGISTER_GATE(TS_CLK_TX5112_HDR_SCLK, "hdr_sclk", "isp_sclk");
	REGISTER_GATE(TS_CLK_TX5112_VPE_ISP_CLK, "vpe_isp_clk", "isp_sclk");
	REGISTER_GATE(TS_CLK_TX5112_MIPI_RX1_PIXCLK, "mipi_rx1_pixclk", "isp_sclk");
	REGISTER_GATE(TS_CLK_TX5112_MIPI_RX0_PIXCLK1, "mipi_rx0_pixclk1", "isp_sclk");
	REGISTER_GATE(TS_CLK_TX5112_MIPI_RX0_PIXCLK0, "mipi_rx0_pixclk0", "isp_sclk");
	REGISTER_GATE(TS_CLK_TX5112_MIPI_RX1_PCLK, "mipi_rx1_pclk", "vi_apb_clk");
	REGISTER_GATE(TS_CLK_TX5112_MIPI_RX0_PCLK, "mipi_rx0_pclk", "vi_apb_clk");
	REGISTER_GATE(TS_CLK_TX5112_HDR_HCLK, "hdr_hclk", "isp_sclk");
	REGISTER_GATE(TS_CLK_TX5112_VPE_HCLK, "vpe_hclk", "isp_sclk");
	REGISTER_GATE(TS_CLK_TX5112_VI_CFG_HCLK, "vi_cfg_hclk", "isp_sclk");
	REGISTER_GATE(TS_CLK_TX5112_HDR_ACLK, "hdr_aclk", "vi_ddr_aclk");
	REGISTER_GATE(TS_CLK_TX5112_VPE_ACLK, "vpe_aclk", "vi_ddr_aclk");
	REGISTER_GATE(TS_CLK_TX5112_ISP_ACLK, "isp_aclk", "vi_ddr_aclk");
	REGISTER_GATE(TS_CLK_TX5112_AMR_HCLK, "amr_hclk", "ai_ahb_clk");
	REGISTER_GATE(TS_CLK_TX5112_AMR_ACLK, "amr_aclk", "ai_ddr_aclk");
	REGISTER_GATE(TS_CLK_TX5112_RNE_ACLK, "rne_aclk", "ai_ddr_aclk");
	REGISTER_GATE(TS_CLK_TX5112_RNE_HCLK, "rne_hclk", "core_ahb_clk");
	REGISTER_GATE(TS_CLK_TX5112_VPU_HCLK, "vpu_hclk", "core_ahb_clk");
	REGISTER_GATE(TS_CLK_TX5112_H265_PCLK, "h265_pclk", "vpu_aclk");
	REGISTER_GATE(TS_CLK_TX5112_H265_ACLK, "h265_aclk", "vpu_aclk");
	REGISTER_GATE(TS_CLK_TX5112_H264_PCLK, "h264_pclk", "vpu_aclk");
	REGISTER_GATE(TS_CLK_TX5112_H264_ACLK, "h264_aclk", "vpu_aclk");

#undef REGISTER_GATE
	return 0;
}

int ts_clk_init_funcs(struct ts_clk_reg_funcs *pfuncs)
{
	if (!pfuncs)
		return -EINVAL;

	pfuncs->register_plls = clk_tx5112_register_plls;
	pfuncs->register_comps = clk_tx5112_register_comps;
	pfuncs->register_comps_without_mux = clk_tx5112_register_comps_without_mux;
	pfuncs->register_dividers = clk_tx5112_register_dividers;
	pfuncs->register_gates = clk_tx5112_register_gates;
	return 0;
}

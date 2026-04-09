// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2022-2022 Tsingmicro, Inc.
 */
#include <asm/io.h>
#include <dt-bindings/clock/ts-tx5215-clk.h>
#include <dm.h>
#include <log.h>
#include <mapmem.h>
#include "ts_pll.h"
#include "ts_clk.h"

/* TOP_CGU */
#define PLL0_CFG0			0x000
#define PLL0_CFG1			0x004
#define PLL0_CFG2			0x008
#define PLL1_CFG0			0x010
#define PLL1_CFG1			0x014
#define PLL1_CFG2			0x018
#define PLL2_CFG0			0x020
#define PLL2_CFG1			0x024
#define PLL2_CFG2			0x028
#define PLL_STATUS			0x030

#define CPU_CLK_CFG0		0x080
#define CPU_CLK_CFG1		0x084
#define BUS_CLK_CFG0		0x090
#define BUS_CLK_CFG1		0x094

#define GMAC_ACLK_CFG		0x098
#define BUS_CLK_EN0			0x09C
#define BUS_CLK_EN1			0x0A0
#define BUS_CLK_EN2			0x0A4
#define BUS_CLK_EN3			0x0A8

#define SDHC0_CCLK_CFG0		0x0B0
#define SDHC0_CCLK_CFG1		0x0B4
#define SDHC1_CCLK_CFG0		0x0C0
#define SDHC1_CCLK_CFG1		0x0C4

#define OSPI_RCLK_CFG		0x0D0
#define USB_PHY_CLK_CFG		0x0D4
#define GMAC_MAC_CLK_EN		0x0E0
#define GMAC_MAC_CLK_CFG	0x0E4
#define GMAC_TXCLK_DLY		0x0E8
#define GMAC_RXCLK_DLY		0x0EC

#define I2C_ICCLK_CFG0		0x0F0
#define I2C_ICCLK_CFG1		0x0F4
#define UART0_SCLK_CFG		0x100
#define UART1_SCLK_CFG		0x104
#define UART2_SCLK_CFG		0x108
#define SPI_SSICLK_CFG		0x110
#define TMR_TCLK_CFG0		0x120
#define TMR_TCLK_CFG1		0x124
#define TMR_TCLK_EN			0x128
#define I2S0_MCLK_CFG		0x130
#define PDM_MCLK_CFG		0x138
#define AUD_DAC_CLK_CFG0	0x140
#define AUD_DAC_CLK_CFG1	0x144
#define AUD_ADC_CLK_CFG		0x148

#define MCTL_ACLK_CFG0		0x160
#define MCTL_ACLK_CFG1		0x164
#define MCTL_CLK_EN			0x168

#define VI_DDR_ACLK_CFG		0x180
#define VI_BUS_CLK_EN		0x184
#define ISP_SCLK_CFG		0x188
#define VPE_CCLK_CFG		0x18C
#define MIPI_CLK_CFG		0x190
#define CV_DDR_ACLK_CFG		0x1A0
#define DISP_DDR_ACLK_CFG	0x1A4
#define DISP_BUS_CLK_EN		0x1A8
#define NEMADC_CCLK_CFG		0x1AC
#define CVE_CCLK_CFG		0x1B0
#define AMR_CCLK_CFG		0x1B4
#define RNE_BUS_CLK_EN		0x1D0
#define RNE_CCLK_CFG		0x1D4
#define VPU_CLK_CFG			0x200
#define VPU_CLK_EN			0x204
#define CHIP_OCLK_CFG		0x208


/* All methods are delegated to CCF clocks */

/* Parents for muxed clocks */
static const char * const cpu_cclk_sels[] = { &osc_in[0], "pll0" };
static const char * const cpu_jtag_clk_sels[] = { &osc_in[0], "pll0" };
static const char * const top_ahb_clk_sels[] = { &osc_in[0], "pll0" };
static const char * const top_axi_clk_sels[] = { &osc_in[0], "pll0" };
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
static const char * const ospi_ref_clk_sels[] = { &osc_in[0], "pll1" };
static const char * const usb_phy_clk_sels[] = { &osc_in[0], "pll0_d40" };
static const char * const i2c_icclk_sels[] = { &osc_in[0], "pll1" };
static const char * const uart_sclk_sels[] = { &osc_in[0], "pll1" };
static const char * const spi_ssiiclk_sels[] = { &osc_in[0], "pll1" };
static const char * const rne_cclk_sels[] = { "pll0", "pll1" };
static const char * const chip_oclk_sels[] = { "pll0_d8", "pll1_d4", &osc_in[0], "forbidden"};

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

#define DIV(id, off, shift, width, we, sync, flags) DIV_FLAGS(id, off, shift, width, we, sync, flags)
#define DIV_LIST \
	DIV(TS_CLK_TX5215_CPU_CCLK, CPU_CLK_CFG0, 0, 4, 6, 7, 0) \
	DIV(TS_CLK_TX5215_CPU_JTAG_CLK, CPU_CLK_CFG1, 16, 3, 22, 23, CLK_DIVIDER_MUl_TWO) \
	DIV(TS_CLK_TX5215_CPU_ACLK, CPU_CLK_CFG1, 8, 2, 10, 11, 0) \
	DIV(TS_CLK_TX5215_CPU_DBG_PCLK, CPU_CLK_CFG1, 0, 4, 6, 7, 0) \
	DIV(TS_CLK_TX5215_TOP_AHB_CLK, BUS_CLK_CFG1, 8, 4, 14, 15, 0) \
	DIV(TS_CLK_TX5215_TOP_AXI_CLK, BUS_CLK_CFG1, 0, 4, 6, 7, 0) \
	DIV(TS_CLK_TX5215_PERI_APB_CLK, BUS_CLK_CFG1, 16, 2, 22, 23, 0) \
	DIV(TS_CLK_TX5215_SD0_CCLK_DIV1, SDHC0_CCLK_CFG0, 0, 5, 6, 7, CLK_DIVIDER_MUl_TWO) \
	DIV(TS_CLK_TX5215_SD0_CCLK_DIV2, SDHC0_CCLK_CFG0, 16, 4, 6, 7, 0) \
	DIV(TS_CLK_TX5215_SD1_CCLK_DIV1, SDHC1_CCLK_CFG0, 0, 5, 6, 7, CLK_DIVIDER_MUl_TWO) \
	DIV(TS_CLK_TX5215_SD1_CCLK_DIV2, SDHC1_CCLK_CFG0, 16, 4, 6, 7, 0) \
	DIV(TS_CLK_TX5215_OSPI_REF_CLK, OSPI_RCLK_CFG, 0, 4, 6, 7, CLK_DIVIDER_MUl_TWO) \
	DIV(TS_CLK_TX5215_I2C1_ICCLK, I2C_ICCLK_CFG0, 16, 5, 22, 23, 0) \
	DIV(TS_CLK_TX5215_I2C0_ICCLK, I2C_ICCLK_CFG0, 0, 5, 6, 7, 0) \
	DIV(TS_CLK_TX5215_I2C3_ICCLK, I2C_ICCLK_CFG1, 16, 5, 22, 23, 0) \
	DIV(TS_CLK_TX5215_I2C2_ICCLK, I2C_ICCLK_CFG1, 0, 5, 6, 7, 0) \
	DIV(TS_CLK_TX5215_UART0_SCLK, UART0_SCLK_CFG, 0, 28, WE_INVALID, 28, 0) \
	DIV(TS_CLK_TX5215_UART1_SCLK, UART1_SCLK_CFG, 0, 28, WE_INVALID, 28, 0) \
	DIV(TS_CLK_TX5215_UART2_SCLK, UART2_SCLK_CFG, 0, 28, WE_INVALID, 28, 0) \
	DIV(TS_CLK_TX5215_SPI_SSICLK, SPI_SSICLK_CFG, 0, 5, 6, 7, 0) \
	DIV(TS_CLK_TX5215_TMR_T8CLK, TMR_TCLK_CFG1, 8, 6, 14, 15, 0) \
	DIV(TS_CLK_TX5215_TMR_T7CLK, TMR_TCLK_CFG1, 0, 6, 6, 7, 0) \
	DIV(TS_CLK_TX5215_TMR_T6CLK, TMR_TCLK_CFG0, 24, 6, 30, 31, 0) \
	DIV(TS_CLK_TX5215_TMR_T5CLK, TMR_TCLK_CFG0, 16, 6, 22, 23, 0) \
	DIV(TS_CLK_TX5215_TMR_T4CLK, TMR_TCLK_CFG0, 8, 6, 14, 15, 0) \
	DIV(TS_CLK_TX5215_TMR_T3CLK, TMR_TCLK_CFG0, 0, 6, 6, 7, 0) \
	DIV(TS_CLK_TX5215_I2S0_OCLK, I2S0_MCLK_CFG, 0, 8, 8, 9, 0) \
	DIV(TS_CLK_TX5215_I2S0_MCLK, I2S0_MCLK_CFG, 12, 4, 18, 19, 0) \
	DIV(TS_CLK_TX5215_PDM_MCLK, PDM_MCLK_CFG, 0, 9, 10, 11, 0) \
	DIV(TS_CLK_TX5215_DIV0_CODEC_DAC_CCLK, AUD_DAC_CLK_CFG0, 0, 8, 8, 9, 0) \
	DIV(TS_CLK_TX5215_DIV1_CODEC_DAC_CCLK, AUD_DAC_CLK_CFG0, 12, 6, 20, 21, 0) \
	DIV(TS_CLK_TX5215_AUD_DAC_PBCLK, AUD_DAC_CLK_CFG0, 24, 6, 30, 31, 0) \
	DIV(TS_CLK_TX5215_DIV0_CODEC_ADC_CCLK, AUD_ADC_CLK_CFG, 0, 8, 8, 9, 0) \
	DIV(TS_CLK_TX5215_DIV1_CODEC_ADC_CCLK, AUD_ADC_CLK_CFG, 12, 6, 20, 21, 0) \
	DIV(TS_CLK_TX5215_MCTL_P5_ACLK, MCTL_ACLK_CFG1, 8, 4, 14, 15, 0) \
	DIV(TS_CLK_TX5215_MCTL_P4_ACLK, MCTL_ACLK_CFG1, 0, 4, 6, 7, 0) \
	DIV(TS_CLK_TX5215_MCTL_P3_ACLK, MCTL_ACLK_CFG0, 24, 4, 30, 31, 0) \
	DIV(TS_CLK_TX5215_MCTL_P2_ACLK, MCTL_ACLK_CFG0, 16, 4, 22, 23, 0) \
	DIV(TS_CLK_TX5215_MCTL_P1_ACLK, MCTL_ACLK_CFG0, 8, 4, 14, 15, 0) \
	DIV(TS_CLK_TX5215_MCTL_P0_ACLK, MCTL_ACLK_CFG0, 0, 4, 6, 7, 0) \
	DIV(TS_CLK_TX5215_VI_DDR_ACLK, VI_DDR_ACLK_CFG, 0, 4, 6, 7, 0) \
	DIV(TS_CLK_TX5215_ISP_SCLK, ISP_SCLK_CFG, 0, 4, 6, 7, 0) \
	DIV(TS_CLK_TX5215_VPE_CCLK, VPE_CCLK_CFG, 0, 4, 6, 7, 0) \
	DIV(TS_CLK_TX5215_MIPI_TXCLKESC, MIPI_CLK_CFG, 0, 6, 6, 7, 0) \
	DIV(TS_CLK_TX5215_CV_DDR_ACLK, CV_DDR_ACLK_CFG, 0, 4, 6, 7, 0) \
	DIV(TS_CLK_TX5215_DISP_DDR_ACLK, DISP_DDR_ACLK_CFG, 0, 4, 6, 7, 0) \
	DIV(TS_CLK_TX5215_NEMADC_CCLK, NEMADC_CCLK_CFG, 0, 6, 6, 7, 0) \
	DIV(TS_CLK_TX5215_CVE_CCLK, CVE_CCLK_CFG, 0, 5, 6, 7, 0) \
	DIV(TS_CLK_TX5215_AMR_CCLK, AMR_CCLK_CFG, 0, 5, 6, 7, 0) \
	DIV(TS_CLK_TX5215_RNE_CCLK, RNE_CCLK_CFG, 0, 6, 6, 7, 0) \
	DIV(TS_CLK_TX5215_VPU_ACLK, VPU_CLK_CFG, 0, 6, 6, 7, 0) \
	DIV(TS_CLK_TX5215_CHIP_OCLK_I0, CHIP_OCLK_CFG, 6, 2, WE_INVALID, 1, 0) \
	DIV(TS_CLK_TX5215_CHIP_OCLK_I1, CHIP_OCLK_CFG, 14, 2, WE_INVALID, 9, 0) \
	DIV(TS_CLK_TX5215_CHIP_OCLK_I2, CHIP_OCLK_CFG, 22, 2, WE_INVALID, 17, 0) \
	DIV(TS_CLK_TX5215_CHIP_OCLK_I3, CHIP_OCLK_CFG, 30, 2, WE_INVALID, 25, 0)

#define _DIVIFY(id) TS_CLK_TX5215_DIV_##id
#define DIVIFY(id) _DIVIFY(id)

enum ts_div_ids {
#define DIV_FLAGS(id, ...) DIVIFY(id),
	DIV_LIST
#undef DIV_FLAGS
	TS_CLK_DIV_NONE,
};

static const struct ts_div_params ts_tx5215_divs[] = {
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
	GATE(TS_CLK_TX5215_TRNG_CLK, BUS_CLK_EN0, 6) \
	GATE(TS_CLK_TX5215_PKE_CLK, BUS_CLK_EN0, 5) \
	GATE(TS_CLK_TX5215_HASH_CLK, BUS_CLK_EN0, 4) \
	GATE(TS_CLK_TX5215_SPACC_CLK, BUS_CLK_EN0, 3) \
	GATE(TS_CLK_TX5215_SKE_CLK, BUS_CLK_EN0, 2) \
	GATE(TS_CLK_TX5215_OCRAM_ACLK, BUS_CLK_EN0, 1) \
	GATE(TS_CLK_TX5215_MEM_DMA_ACLK, BUS_CLK_EN0, 0) \
	GATE(TS_CLK_TX5215_AUD_CODEC_HCLK, BUS_CLK_EN1, 8) \
	GATE(TS_CLK_TX5215_OSPI_HCLK, BUS_CLK_EN1, 7) \
	GATE(TS_CLK_TX5215_GMAC_HCLK, BUS_CLK_EN1, 6) \
	GATE(TS_CLK_TX5215_USB2C_HCLK, BUS_CLK_EN1, 5) \
	GATE(TS_CLK_TX5215_SDHC1_HCLK, BUS_CLK_EN1, 4) \
	GATE(TS_CLK_TX5215_SDHC0_HCLK, BUS_CLK_EN1, 3) \
	GATE(TS_CLK_TX5215_PERI_DMA1_HCLK, BUS_CLK_EN1, 2) \
	GATE(TS_CLK_TX5215_PERI_DMA0_HCLK, BUS_CLK_EN1, 1) \
	GATE(TS_CLK_TX5215_BOOTROM_HCLK, BUS_CLK_EN1, 0) \
	GATE(TS_CLK_TX5215_I2S0_PCLK, BUS_CLK_EN2, 20) \
	GATE(TS_CLK_TX5215_USI1_PCLK, BUS_CLK_EN2, 17) \
	GATE(TS_CLK_TX5215_USI0_PCLK, BUS_CLK_EN2, 16) \
	GATE(TS_CLK_TX5215_UART2_PCLK, BUS_CLK_EN2, 10) \
	GATE(TS_CLK_TX5215_UART1_PCLK, BUS_CLK_EN2, 9) \
	GATE(TS_CLK_TX5215_UART0_PCLK, BUS_CLK_EN2, 8) \
	GATE(TS_CLK_TX5215_I2C3_PCLK, BUS_CLK_EN2, 3) \
	GATE(TS_CLK_TX5215_I2C2_PCLK, BUS_CLK_EN2, 2) \
	GATE(TS_CLK_TX5215_I2C1_PCLK, BUS_CLK_EN2, 1) \
	GATE(TS_CLK_TX5215_I2C0_PCLK, BUS_CLK_EN2, 0) \
	GATE(TS_CLK_TX5215_GPIO_B_PCLK, BUS_CLK_EN3, 11) \
	GATE(TS_CLK_TX5215_GPIO_A_PCLK, BUS_CLK_EN3, 10) \
	GATE(TS_CLK_TX5215_PDM_PCLK, BUS_CLK_EN3, 9) \
	GATE(TS_CLK_TX5215_SPI_PCLK, BUS_CLK_EN3, 8) \
	GATE(TS_CLK_TX5215_SYS_REG_PCLK, BUS_CLK_EN3, 7) \
	GATE(TS_CLK_TX5215_OTPC_PCLK, BUS_CLK_EN3, 6) \
	GATE(TS_CLK_TX5215_OSPI_PCLK, BUS_CLK_EN3, 5) \
	GATE(TS_CLK_TX5215_PWM_PCLK, BUS_CLK_EN3, 4) \
	GATE(TS_CLK_TX5215_TMR_PCLK, BUS_CLK_EN3, 3) \
	GATE(TS_CLK_TX5215_WDT_PCLK, BUS_CLK_EN3, 2) \
	GATE(TS_CLK_TX5215_GPIO_PCLK, BUS_CLK_EN3, 1) \
	GATE(TS_CLK_TX5215_ADC_PCLK, BUS_CLK_EN3, 0) \
	GATE(TS_CLK_TX5215_SD0_CCLK_DIV2, SDHC0_CCLK_CFG0, 12) \
	GATE(TS_CLK_TX5215_SD0_CCLK_SMPL, SDHC0_CCLK_CFG0, 14) \
	GATE(TS_CLK_TX5215_SD0_CCLK_DRV, SDHC0_CCLK_CFG0, 13) \
	GATE(TS_CLK_TX5215_SD1_CCLK_DIV2, SDHC1_CCLK_CFG0, 12) \
	GATE(TS_CLK_TX5215_SD1_CCLK_SMPL, SDHC1_CCLK_CFG0, 14) \
	GATE(TS_CLK_TX5215_SD1_CCLK_DRV, SDHC1_CCLK_CFG0, 13) \
	GATE(TS_CLK_TX5215_OSPI_REF_CLK, OSPI_RCLK_CFG, 12) \
	GATE(TS_CLK_TX5215_USB_PHY_CLK, USB_PHY_CLK_CFG, 4) \
	GATE(TS_CLK_TX5215_TMR_T8CLK, TMR_TCLK_EN, 5) \
	GATE(TS_CLK_TX5215_TMR_T7CLK, TMR_TCLK_EN, 4) \
	GATE(TS_CLK_TX5215_TMR_T6CLK, TMR_TCLK_EN, 3) \
	GATE(TS_CLK_TX5215_TMR_T5CLK, TMR_TCLK_EN, 2) \
	GATE(TS_CLK_TX5215_TMR_T4CLK, TMR_TCLK_EN, 1) \
	GATE(TS_CLK_TX5215_TMR_T3CLK, TMR_TCLK_EN, 0) \
	GATE(TS_CLK_TX5215_I2S0_OCLK, I2S0_MCLK_CFG, 20) \
	GATE(TS_CLK_TX5215_I2S0_MCLK, I2S0_MCLK_CFG, 21) \
	GATE(TS_CLK_TX5215_PDM_MCLK, PDM_MCLK_CFG, 16) \
	GATE(TS_CLK_TX5215_DIV1_CODEC_DAC_CCLK, AUD_DAC_CLK_CFG1, 18) \
	GATE(TS_CLK_TX5215_AUD_DAC_PBCLK, AUD_DAC_CLK_CFG1, 17) \
	GATE(TS_CLK_TX5215_DIV1_CODEC_ADC_CCLK, AUD_ADC_CLK_CFG, 24) \
	GATE(TS_CLK_TX5215_DDRC_CORE_CLK, MCTL_CLK_EN, 12) \
	GATE(TS_CLK_TX5215_DDR_PHY_PCLK, MCTL_CLK_EN, 9) \
	GATE(TS_CLK_TX5215_UMCTL_PCLK, MCTL_CLK_EN, 8) \
	GATE(TS_CLK_TX5215_MCTL_P5_ACLK, MCTL_CLK_EN, 5) \
	GATE(TS_CLK_TX5215_MCTL_P4_ACLK, MCTL_CLK_EN, 4) \
	GATE(TS_CLK_TX5215_MCTL_P3_ACLK, MCTL_CLK_EN, 3) \
	GATE(TS_CLK_TX5215_MCTL_P2_ACLK, MCTL_CLK_EN, 2) \
	GATE(TS_CLK_TX5215_MCTL_P1_ACLK, MCTL_CLK_EN, 1) \
	GATE(TS_CLK_TX5215_MCTL_P0_ACLK, MCTL_CLK_EN, 0) \
	GATE(TS_CLK_TX5215_VI_CFG_HCLK, VI_BUS_CLK_EN, 8) \
	GATE(TS_CLK_TX5215_MIPI_PHY_PCLK, VI_BUS_CLK_EN, 13) \
	GATE(TS_CLK_TX5215_MIPI_RX1_PCLK, VI_BUS_CLK_EN, 12) \
	GATE(TS_CLK_TX5215_MIPI_RX0_PCLK, VI_BUS_CLK_EN, 11) \
	GATE(TS_CLK_TX5215_HDR_HCLK, VI_BUS_CLK_EN, 10) \
	GATE(TS_CLK_TX5215_VPE_HCLK, VI_BUS_CLK_EN, 9) \
	GATE(TS_CLK_TX5215_HDR_ACLK, VI_BUS_CLK_EN, 3) \
	GATE(TS_CLK_TX5215_VPE_ACLK, VI_BUS_CLK_EN, 2) \
	GATE(TS_CLK_TX5215_ISP_ACLK, VI_BUS_CLK_EN, 1) \
	GATE(TS_CLK_TX5215_VI_DDR_ACLK, VI_BUS_CLK_EN, 0) \
	GATE(TS_CLK_TX5215_ISP_SCLK, ISP_SCLK_CFG, 8) \
	GATE(TS_CLK_TX5215_MIPI_RX1_PIXCLK, ISP_SCLK_CFG, 13) \
	GATE(TS_CLK_TX5215_MIPI_RX0_PIXCLK1, ISP_SCLK_CFG, 12) \
	GATE(TS_CLK_TX5215_MIPI_RX0_PIXCLK0, ISP_SCLK_CFG, 11) \
	GATE(TS_CLK_TX5215_HDR_SCLK, ISP_SCLK_CFG, 10) \
	GATE(TS_CLK_TX5215_VPE_ISP_CLK, ISP_SCLK_CFG, 9) \
	GATE(TS_CLK_TX5215_VPE_CCLK, VPE_CCLK_CFG, 8) \
	GATE(TS_CLK_TX5215_MIPI_TXCLKESC, MIPI_CLK_CFG, 8) \
	GATE(TS_CLK_TX5215_CV_DDR_ACLK, DISP_BUS_CLK_EN, 1) \
	GATE(TS_CLK_TX5215_DISP_DDR_ACLK, DISP_BUS_CLK_EN, 0) \
	GATE(TS_CLK_TX5215_DVP_HCLK, DISP_BUS_CLK_EN, 11) \
	GATE(TS_CLK_TX5215_CVE_HCLK, DISP_BUS_CLK_EN, 10) \
	GATE(TS_CLK_TX5215_AMR_HCLK, DISP_BUS_CLK_EN, 9) \
	GATE(TS_CLK_TX5215_DISP_CFG_HCLK, DISP_BUS_CLK_EN, 8) \
	GATE(TS_CLK_TX5215_NEMADC_HCLK, DISP_BUS_CLK_EN, 5) \
	GATE(TS_CLK_TX5215_DVP_ACLK, DISP_BUS_CLK_EN, 4) \
	GATE(TS_CLK_TX5215_CVE_ACLK, DISP_BUS_CLK_EN, 3) \
	GATE(TS_CLK_TX5215_AMR_ACLK, DISP_BUS_CLK_EN, 2) \
	GATE(TS_CLK_TX5215_NEMADC_CCLK, NEMADC_CCLK_CFG, 8) \
	GATE(TS_CLK_TX5215_CVE_CCLK, CVE_CCLK_CFG, 8) \
	GATE(TS_CLK_TX5215_AMR_CCLK, AMR_CCLK_CFG, 8) \
	GATE(TS_CLK_TX5215_RNE_HCLK, RNE_BUS_CLK_EN, 1) \
	GATE(TS_CLK_TX5215_RNE_ACLK, RNE_BUS_CLK_EN, 0) \
	GATE(TS_CLK_TX5215_RNE_CCLK, RNE_CCLK_CFG, 8) \
	GATE(TS_CLK_TX5215_VPU_HCLK, VPU_CLK_EN, 8) \
	GATE(TS_CLK_TX5215_H265_CCLK, VPU_CLK_EN, 6) \
	GATE(TS_CLK_TX5215_H265_PCLK, VPU_CLK_EN, 5) \
	GATE(TS_CLK_TX5215_H265_ACLK, VPU_CLK_EN, 4) \
	GATE(TS_CLK_TX5215_H264_PCLK, VPU_CLK_EN, 1) \
	GATE(TS_CLK_TX5215_H264_ACLK, VPU_CLK_EN, 0) \
	GATE(TS_CLK_TX5215_CHIP_OCLK_I0, CHIP_OCLK_CFG, 0) \
	GATE(TS_CLK_TX5215_CHIP_OCLK_I1, CHIP_OCLK_CFG, 8) \
	GATE(TS_CLK_TX5215_CHIP_OCLK_I2, CHIP_OCLK_CFG, 16) \
	GATE(TS_CLK_TX5215_CHIP_OCLK_I3, CHIP_OCLK_CFG, 24)

#define _GATEIFY(id) TS_CLK_TX5215_GATE_##id
#define GATEIFY(id) _GATEIFY(id)

enum ts_gate_ids {
#define GATE(id, ...) GATEIFY(id),
	GATE_LIST
#undef GATE
	TS_CLK_GATE_NONE,
};

static const struct ts_gate_params ts_tx5215_gates[] = {
#define GATE(id, _off, _idx) \
	[GATEIFY(id)] = { \
		.off = (_off), \
		.bit_idx = (_idx), \
	},
	GATE_LIST
#undef GATE
};

#undef GATE_LIST

#define MUX(id, reg, shift, width, we, sync) \
	MUX_PARENTS(id, generic_sels, reg, shift, width, we, sync)
#define MUX_LIST \
	MUX_PARENTS(TS_CLK_TX5215_CPU_CCLK, cpu_cclk_sels, CPU_CLK_CFG0, 8, 1, WE_INVALID, SYNC_INVALID) \
	MUX_PARENTS(TS_CLK_TX5215_CPU_JTAG_CLK, cpu_jtag_clk_sels, CPU_CLK_CFG1, 24, 1, WE_INVALID, SYNC_INVALID) \
	MUX_PARENTS(TS_CLK_TX5215_TOP_AHB_CLK, top_ahb_clk_sels, BUS_CLK_CFG0, 0, 1, WE_INVALID, SYNC_INVALID) \
	MUX_PARENTS(TS_CLK_TX5215_TOP_AXI_CLK, top_axi_clk_sels, BUS_CLK_CFG0, 0, 1, WE_INVALID, SYNC_INVALID) \
	MUX_PARENTS(TS_CLK_TX5215_SD0_CCLK_DIV1, sd0_cclk_sels, SDHC0_CCLK_CFG0, 8, 1, WE_INVALID, SYNC_INVALID) \
	MUX_PARENTS(TS_CLK_TX5215_SD0_CCLK_SMPL, sd0_cclk_smpl_sels, SDHC0_CCLK_CFG1, 8, 4, 15, SYNC_INVALID) \
	MUX_PARENTS(TS_CLK_TX5215_SD0_CCLK_DRV, sd0_cclk_drv_sels, SDHC0_CCLK_CFG1, 0, 4, 7, SYNC_INVALID) \
	MUX_PARENTS(TS_CLK_TX5215_SD1_CCLK_DIV1, sd1_cclk_sels, SDHC1_CCLK_CFG0, 8, 1, WE_INVALID, SYNC_INVALID) \
	MUX_PARENTS(TS_CLK_TX5215_SD1_CCLK_SMPL, sd1_cclk_smpl_sels, SDHC1_CCLK_CFG1, 8, 4, 15, SYNC_INVALID) \
	MUX_PARENTS(TS_CLK_TX5215_SD1_CCLK_DRV, sd1_cclk_drv_sels, SDHC1_CCLK_CFG1, 0, 4, 7, SYNC_INVALID) \
	MUX_PARENTS(TS_CLK_TX5215_OSPI_REF_CLK, ospi_ref_clk_sels, OSPI_RCLK_CFG, 8, 1, WE_INVALID, SYNC_INVALID) \
	MUX_PARENTS(TS_CLK_TX5215_USB_PHY_CLK, usb_phy_clk_sels, USB_PHY_CLK_CFG, 0, 1, WE_INVALID, SYNC_INVALID) \
	MUX_PARENTS(TS_CLK_TX5215_I2C1_ICCLK, i2c_icclk_sels, I2C_ICCLK_CFG0, 24, 1, WE_INVALID, SYNC_INVALID) \
	MUX_PARENTS(TS_CLK_TX5215_I2C0_ICCLK, i2c_icclk_sels, I2C_ICCLK_CFG0, 8, 1, WE_INVALID, SYNC_INVALID) \
	MUX_PARENTS(TS_CLK_TX5215_I2C3_ICCLK, i2c_icclk_sels, I2C_ICCLK_CFG1, 24, 1, WE_INVALID, SYNC_INVALID) \
	MUX_PARENTS(TS_CLK_TX5215_I2C2_ICCLK, i2c_icclk_sels, I2C_ICCLK_CFG1, 8, 1, WE_INVALID, SYNC_INVALID) \
	MUX_PARENTS(TS_CLK_TX5215_UART0_SCLK, uart_sclk_sels, UART0_SCLK_CFG, 31, 1, WE_INVALID, SYNC_INVALID) \
	MUX_PARENTS(TS_CLK_TX5215_UART1_SCLK, uart_sclk_sels, UART1_SCLK_CFG, 31, 1, WE_INVALID, SYNC_INVALID) \
	MUX_PARENTS(TS_CLK_TX5215_UART2_SCLK, uart_sclk_sels, UART2_SCLK_CFG, 31, 1, WE_INVALID, SYNC_INVALID) \
	MUX_PARENTS(TS_CLK_TX5215_SPI_SSICLK, spi_ssiiclk_sels, SPI_SSICLK_CFG, 8, 1, WE_INVALID, SYNC_INVALID) \
	MUX_PARENTS(TS_CLK_TX5215_RNE_CCLK, rne_cclk_sels, RNE_CCLK_CFG, 12, 1, WE_INVALID, SYNC_INVALID) \
	MUX_PARENTS(TS_CLK_TX5215_CHIP_OCLK_I0, chip_oclk_sels, CHIP_OCLK_CFG, 4, 2, WE_INVALID, SYNC_INVALID) \
	MUX_PARENTS(TS_CLK_TX5215_CHIP_OCLK_I1, chip_oclk_sels, CHIP_OCLK_CFG, 12, 2, WE_INVALID, SYNC_INVALID) \
	MUX_PARENTS(TS_CLK_TX5215_CHIP_OCLK_I2, chip_oclk_sels, CHIP_OCLK_CFG, 20, 2, WE_INVALID, SYNC_INVALID) \
	MUX_PARENTS(TS_CLK_TX5215_CHIP_OCLK_I3, chip_oclk_sels, CHIP_OCLK_CFG, 28, 2, WE_INVALID, SYNC_INVALID)

#define _MUXIFY(id) TS_CLK_TX5215_MUX_##id
#define MUXIFY(id) _MUXIFY(id)

enum ts_mux_ids {
#define MUX_PARENTS(id, ...) MUXIFY(id),
	MUX_LIST
#undef MUX_PARENTS
	TS_CLK_MUX_NONE,
};


static const struct ts_mux_params ts_tx5215_muxes[] = {
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


static struct ts_pll_params ts_tx5215_plls[] = {
#define PLL(_name, _id, _off, _shift, _width) { \
	.name = _name, \
	.id = _id, \
	.off = (_off), \
	.lock_off = PLL_STATUS, \
	.shift = (_shift), \
	.width = (_width), \
}
	[0] = PLL("pll0", TS_CLK_TX5215_PLL0, PLL0_CFG0, 0, 1),
	[1] = PLL("pll1", TS_CLK_TX5215_PLL1, PLL1_CFG0, 1, 1),
	[2] = PLL("pll2", TS_CLK_TX5215_PLL2, PLL2_CFG0, 2, 1),
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
#define COMP_DIV_ONLY(id) \
	COMP_FULL(id, TS_CLK_MUX_NONE, DIVIFY(id), TS_CLK_GATE_NONE)
#define COMP_GATE_ONLY(id) \
	COMP_FULL(id, TS_CLK_MUX_NONE, TS_CLK_DIV_NONE, GATEIFY(id))
#define COMP_LIST \
	COMP_NOGATE(TS_CLK_TX5215_CPU_CCLK) \
	COMP_NOGATE(TS_CLK_TX5215_CPU_JTAG_CLK) \
	COMP_NOGATE(TS_CLK_TX5215_TOP_AHB_CLK) \
	COMP_NOGATE(TS_CLK_TX5215_TOP_AXI_CLK) \
	COMP_NOGATE(TS_CLK_TX5215_SD0_CCLK_DIV1) \
	COMP_NOMUX(TS_CLK_TX5215_SD0_CCLK_DIV2) \
	COMP_NODIV(TS_CLK_TX5215_SD0_CCLK_SMPL) \
	COMP_NODIV(TS_CLK_TX5215_SD0_CCLK_DRV) \
	COMP_NOGATE(TS_CLK_TX5215_SD1_CCLK_DIV1) \
	COMP_NOMUX(TS_CLK_TX5215_SD1_CCLK_DIV2) \
	COMP_NODIV(TS_CLK_TX5215_SD1_CCLK_SMPL) \
	COMP_NODIV(TS_CLK_TX5215_SD1_CCLK_DRV) \
	COMP(TS_CLK_TX5215_OSPI_REF_CLK) \
	COMP_NODIV(TS_CLK_TX5215_USB_PHY_CLK) \
	COMP_NOGATE(TS_CLK_TX5215_I2C1_ICCLK) \
	COMP_NOGATE(TS_CLK_TX5215_I2C0_ICCLK) \
	COMP_NOGATE(TS_CLK_TX5215_I2C3_ICCLK) \
	COMP_NOGATE(TS_CLK_TX5215_I2C2_ICCLK) \
	COMP_NOGATE(TS_CLK_TX5215_UART0_SCLK) \
	COMP_NOGATE(TS_CLK_TX5215_UART1_SCLK) \
	COMP_NOGATE(TS_CLK_TX5215_UART2_SCLK) \
	COMP_NOGATE(TS_CLK_TX5215_SPI_SSICLK) \
	COMP_NOMUX(TS_CLK_TX5215_TMR_T8CLK) \
	COMP_NOMUX(TS_CLK_TX5215_TMR_T7CLK) \
	COMP_NOMUX(TS_CLK_TX5215_TMR_T6CLK) \
	COMP_NOMUX(TS_CLK_TX5215_TMR_T5CLK) \
	COMP_NOMUX(TS_CLK_TX5215_TMR_T4CLK) \
	COMP_NOMUX(TS_CLK_TX5215_TMR_T3CLK) \
	COMP_NOMUX(TS_CLK_TX5215_I2S0_OCLK) \
	COMP_NOMUX(TS_CLK_TX5215_I2S0_MCLK) \
	COMP_NOMUX(TS_CLK_TX5215_PDM_MCLK) \
	COMP_NOMUX(TS_CLK_TX5215_DIV1_CODEC_DAC_CCLK) \
	COMP_NOMUX(TS_CLK_TX5215_AUD_DAC_PBCLK) \
	COMP_NOMUX(TS_CLK_TX5215_DIV1_CODEC_ADC_CCLK) \
	COMP_NOMUX(TS_CLK_TX5215_MCTL_P5_ACLK) \
	COMP_NOMUX(TS_CLK_TX5215_MCTL_P4_ACLK) \
	COMP_NOMUX(TS_CLK_TX5215_MCTL_P3_ACLK) \
	COMP_NOMUX(TS_CLK_TX5215_MCTL_P2_ACLK) \
	COMP_NOMUX(TS_CLK_TX5215_MCTL_P1_ACLK) \
	COMP_NOMUX(TS_CLK_TX5215_MCTL_P0_ACLK) \
	COMP_NOMUX(TS_CLK_TX5215_VI_DDR_ACLK) \
	COMP_NOMUX(TS_CLK_TX5215_ISP_SCLK) \
	COMP_NOMUX(TS_CLK_TX5215_VPE_CCLK) \
	COMP_NOMUX(TS_CLK_TX5215_MIPI_TXCLKESC) \
	COMP_NOMUX(TS_CLK_TX5215_CV_DDR_ACLK) \
	COMP_NOMUX(TS_CLK_TX5215_DISP_DDR_ACLK) \
	COMP_NOMUX(TS_CLK_TX5215_NEMADC_CCLK) \
	COMP_NOMUX(TS_CLK_TX5215_CVE_CCLK) \
	COMP_NOMUX(TS_CLK_TX5215_AMR_CCLK) \
	COMP(TS_CLK_TX5215_RNE_CCLK) \
	COMP(TS_CLK_TX5215_CHIP_OCLK_I0) \
	COMP(TS_CLK_TX5215_CHIP_OCLK_I1) \
	COMP(TS_CLK_TX5215_CHIP_OCLK_I2) \
	COMP(TS_CLK_TX5215_CHIP_OCLK_I3) \
	COMP_DIV_ONLY(TS_CLK_TX5215_CPU_ACLK) \
	COMP_DIV_ONLY(TS_CLK_TX5215_CPU_DBG_PCLK) \
	COMP_DIV_ONLY(TS_CLK_TX5215_PERI_APB_CLK) \
	COMP_DIV_ONLY(TS_CLK_TX5215_DIV0_CODEC_DAC_CCLK) \
	COMP_DIV_ONLY(TS_CLK_TX5215_DIV0_CODEC_ADC_CCLK) \
	COMP_DIV_ONLY(TS_CLK_TX5215_VPU_ACLK) \
	COMP_GATE_ONLY(TS_CLK_TX5215_TRNG_CLK) \
	COMP_GATE_ONLY(TS_CLK_TX5215_PKE_CLK) \
	COMP_GATE_ONLY(TS_CLK_TX5215_HASH_CLK) \
	COMP_GATE_ONLY(TS_CLK_TX5215_SPACC_CLK) \
	COMP_GATE_ONLY(TS_CLK_TX5215_SKE_CLK) \
	COMP_GATE_ONLY(TS_CLK_TX5215_OCRAM_ACLK) \
	COMP_GATE_ONLY(TS_CLK_TX5215_MEM_DMA_ACLK) \
	COMP_GATE_ONLY(TS_CLK_TX5215_AUD_CODEC_HCLK) \
	COMP_GATE_ONLY(TS_CLK_TX5215_OSPI_HCLK) \
	COMP_GATE_ONLY(TS_CLK_TX5215_USB2C_HCLK) \
	COMP_GATE_ONLY(TS_CLK_TX5215_SDHC1_HCLK) \
	COMP_GATE_ONLY(TS_CLK_TX5215_SDHC0_HCLK) \
	COMP_GATE_ONLY(TS_CLK_TX5215_PERI_DMA1_HCLK) \
	COMP_GATE_ONLY(TS_CLK_TX5215_PERI_DMA0_HCLK) \
	COMP_GATE_ONLY(TS_CLK_TX5215_BOOTROM_HCLK) \
	COMP_GATE_ONLY(TS_CLK_TX5215_I2S0_PCLK) \
	COMP_GATE_ONLY(TS_CLK_TX5215_USI1_PCLK) \
	COMP_GATE_ONLY(TS_CLK_TX5215_USI0_PCLK) \
	COMP_GATE_ONLY(TS_CLK_TX5215_UART2_PCLK) \
	COMP_GATE_ONLY(TS_CLK_TX5215_UART1_PCLK) \
	COMP_GATE_ONLY(TS_CLK_TX5215_UART0_PCLK) \
	COMP_GATE_ONLY(TS_CLK_TX5215_I2C3_PCLK) \
	COMP_GATE_ONLY(TS_CLK_TX5215_I2C2_PCLK) \
	COMP_GATE_ONLY(TS_CLK_TX5215_I2C1_PCLK) \
	COMP_GATE_ONLY(TS_CLK_TX5215_I2C0_PCLK) \
	COMP_GATE_ONLY(TS_CLK_TX5215_GPIO_B_PCLK) \
	COMP_GATE_ONLY(TS_CLK_TX5215_GPIO_A_PCLK) \
	COMP_GATE_ONLY(TS_CLK_TX5215_PDM_PCLK) \
	COMP_GATE_ONLY(TS_CLK_TX5215_SPI_PCLK) \
	COMP_GATE_ONLY(TS_CLK_TX5215_SYS_REG_PCLK) \
	COMP_GATE_ONLY(TS_CLK_TX5215_OTPC_PCLK) \
	COMP_GATE_ONLY(TS_CLK_TX5215_OSPI_PCLK) \
	COMP_GATE_ONLY(TS_CLK_TX5215_PWM_PCLK) \
	COMP_GATE_ONLY(TS_CLK_TX5215_TMR_PCLK) \
	COMP_GATE_ONLY(TS_CLK_TX5215_WDT_PCLK) \
	COMP_GATE_ONLY(TS_CLK_TX5215_GPIO_PCLK) \
	COMP_GATE_ONLY(TS_CLK_TX5215_ADC_PCLK) \
	COMP_GATE_ONLY(TS_CLK_TX5215_DDRC_CORE_CLK) \
	COMP_GATE_ONLY(TS_CLK_TX5215_DDR_PHY_PCLK) \
	COMP_GATE_ONLY(TS_CLK_TX5215_UMCTL_PCLK) \
	COMP_GATE_ONLY(TS_CLK_TX5215_VI_CFG_HCLK) \
	COMP_GATE_ONLY(TS_CLK_TX5215_MIPI_PHY_PCLK) \
	COMP_GATE_ONLY(TS_CLK_TX5215_MIPI_RX1_PCLK) \
	COMP_GATE_ONLY(TS_CLK_TX5215_MIPI_RX0_PCLK) \
	COMP_GATE_ONLY(TS_CLK_TX5215_HDR_HCLK) \
	COMP_GATE_ONLY(TS_CLK_TX5215_VPE_HCLK) \
	COMP_GATE_ONLY(TS_CLK_TX5215_HDR_ACLK) \
	COMP_GATE_ONLY(TS_CLK_TX5215_VPE_ACLK) \
	COMP_GATE_ONLY(TS_CLK_TX5215_ISP_ACLK) \
	COMP_GATE_ONLY(TS_CLK_TX5215_MIPI_RX1_PIXCLK) \
	COMP_GATE_ONLY(TS_CLK_TX5215_MIPI_RX0_PIXCLK1) \
	COMP_GATE_ONLY(TS_CLK_TX5215_MIPI_RX0_PIXCLK0) \
	COMP_GATE_ONLY(TS_CLK_TX5215_HDR_SCLK) \
	COMP_GATE_ONLY(TS_CLK_TX5215_VPE_ISP_CLK) \
	COMP_GATE_ONLY(TS_CLK_TX5215_DVP_HCLK) \
	COMP_GATE_ONLY(TS_CLK_TX5215_CVE_HCLK) \
	COMP_GATE_ONLY(TS_CLK_TX5215_AMR_HCLK) \
	COMP_GATE_ONLY(TS_CLK_TX5215_DISP_CFG_HCLK) \
	COMP_GATE_ONLY(TS_CLK_TX5215_NEMADC_HCLK) \
	COMP_GATE_ONLY(TS_CLK_TX5215_DVP_ACLK) \
	COMP_GATE_ONLY(TS_CLK_TX5215_CVE_ACLK) \
	COMP_GATE_ONLY(TS_CLK_TX5215_AMR_ACLK) \
	COMP_GATE_ONLY(TS_CLK_TX5215_RNE_HCLK) \
	COMP_GATE_ONLY(TS_CLK_TX5215_RNE_ACLK) \
	COMP_GATE_ONLY(TS_CLK_TX5215_VPU_HCLK) \
	COMP_GATE_ONLY(TS_CLK_TX5215_H265_CCLK) \
	COMP_GATE_ONLY(TS_CLK_TX5215_H265_PCLK) \
	COMP_GATE_ONLY(TS_CLK_TX5215_H265_ACLK) \
	COMP_GATE_ONLY(TS_CLK_TX5215_H264_PCLK) \
	COMP_GATE_ONLY(TS_CLK_TX5215_H264_ACLK)

#define _COMPIFY(id) TS_CLK_COMP_##id
#define COMPIFY(id) _COMPIFY(id)

enum ts_comp_ids {
#define COMP_FULL(id, ...) COMPIFY(id),
	COMP_LIST
#undef COMP_FULL
};

static const struct ts_comp_params ts_tx5215_comps[] = {
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

static int clk_tx5215_is_frac_divider(const char *name)
{
	u32 i;
	char *frac_div_arr[] = {"uart0_sclk", "uart1_sclk", "uart2_sclk"};

	for (i = 0; i < ARRAY_SIZE(frac_div_arr); i++) {
		if (!strncmp(frac_div_arr[i], name, strlen(frac_div_arr[i])))
			return true;
	}
	return false;
}

static struct clk_div_table oclk_div_table[] = {
	{ 0, 1 }, { 1, 3 }, { 2, 5 }, { 3, 9 }
};
struct clk_div_table *clk_tx5215_get_div_table(const char *name)
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
struct clk *clk_tx5215_register_comp(const struct ts_comp_params *params,
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
			printf("MUX_NONE with parent not NULL\n");
			return ERR_PTR(-EINVAL);
		}

		mux_ops = NULL;
		mux = NULL;
		parent_names = &parent;
		num_parents = 1;
	} else {
		mux_ops = &clk_mux_ops;
		mux = ts_create_mux(&ts_tx5215_muxes[params->mux], base);
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
		if (clk_tx5215_is_frac_divider(name)) {
			div_ops = &clk_fractional_divider_ops;
			div = (struct clk_fractional_divider *)ts_create_fractional_div(&ts_tx5215_divs[div_id], base);
			clk = &((struct clk_fractional_divider *)div)->clk;
		} else {
			div_ops = &clk_divider_ops;
			table = clk_tx5215_get_div_table(name);
			div = (struct clk_divider *)ts_create_div(&ts_tx5215_divs[div_id], base, table);
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
		gate = ts_create_gate(&ts_tx5215_gates[params->gate], base);
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

static int clk_tx5215_register_plls(struct ts_clk_params *paras)
{
	void *top_base = paras->top_base;
	void __iomem *reg;
	void __iomem *lock;
	int i;

	for (i = 0; i < ARRAY_SIZE(ts_tx5215_plls); i++) {
		struct ts_pll_params *params = &ts_tx5215_plls[i];

		reg = top_base + params->off;
		lock = top_base + params->lock_off;

		clk_dm(params->id,
			ts_register_pll(params->name, &osc_in[0], reg, lock,
				params->shift, params->width, PLL_REG_MODE_COMPACT));
	}

	/* fix-frequency clocks for plls */
	clk_dm(TS_CLK_TX5215_PLL0_D2,
		ts_clk_fixed_divider("pll0_d2", "pll0", 2));
	clk_dm(TS_CLK_TX5215_PLL0_D4,
		ts_clk_fixed_divider("pll0_d4", "pll0", 4));
	clk_dm(TS_CLK_TX5215_PLL0_D5,
		ts_clk_fixed_divider("pll0_d5", "pll0", 5));
	clk_dm(TS_CLK_TX5215_PLL0_D8,
		ts_clk_fixed_divider("pll0_d8", "pll0", 8));
	clk_dm(TS_CLK_TX5215_PLL0_D10,
		ts_clk_fixed_divider("pll0_d10", "pll0", 10));
	clk_dm(TS_CLK_TX5215_PLL1_D3,
		ts_clk_fixed_divider("pll1_d3", "pll1", 3));
	clk_dm(TS_CLK_TX5215_PLL1_D4,
		ts_clk_fixed_divider("pll1_d4", "pll1", 4));
	clk_dm(TS_CLK_TX5215_PLL1_D6,
		ts_clk_fixed_divider("pll1_d6", "pll1", 6));
	clk_dm(TS_CLK_TX5215_PLL0_D40,
		ts_clk_fixed_divider("pll0_d40", "pll0", 40));


	/* divider "div0_codec_dac_cclk" is parent of comp "aud_dac_cclk"), register here */
	const struct ts_comp_params *params;

	params = &ts_tx5215_comps[DIVIFY(TS_CLK_TX5215_DIV0_CODEC_DAC_CCLK)];
	clk_dm(TS_CLK_TX5215_DIV0_CODEC_DAC_CCLK,
		clk_tx5215_register_comp(params, top_base, "div0_codec_dac_cclk", "pll1"));

	/* divider "div0_codec_adc_cclk" is parent of comp "aud_adc_cclk"), register here */
	params = &ts_tx5215_comps[DIVIFY(TS_CLK_TX5215_DIV0_CODEC_ADC_CCLK)];
	clk_dm(TS_CLK_TX5215_DIV0_CODEC_ADC_CCLK,
		clk_tx5215_register_comp(params, top_base, "div0_codec_adc_cclk", "pll1"));
	return 0;
}

static int clk_tx5215_register_comps(struct ts_clk_params *paras)
{
	void *top_base = paras->top_base;

#define REGISTER_COMP(id, name) \
		clk_dm(id, \
			clk_tx5215_register_comp(&ts_tx5215_comps[COMPIFY(id)], \
				top_base, name, NULL))

	//printf("--TS_CLK_MUX_NONE  %d--\n", TS_CLK_MUX_NONE);
	//printf("--TS_CLK_DIV_NONE  %d--\n", TS_CLK_DIV_NONE);
	//printf("--TS_CLK_GATE_NONE %d--\n", TS_CLK_GATE_NONE);

	REGISTER_COMP(TS_CLK_TX5215_CPU_CCLK, "cpu_cclk");
	REGISTER_COMP(TS_CLK_TX5215_CPU_JTAG_CLK, "cpu_jtag_clk");
	REGISTER_COMP(TS_CLK_TX5215_TOP_AHB_CLK, "top_ahb_clk");
	REGISTER_COMP(TS_CLK_TX5215_TOP_AXI_CLK, "top_axi_clk");
	REGISTER_COMP(TS_CLK_TX5215_SD0_CCLK_DIV1, "sd0_cclk_div1");
	clk_dm(TS_CLK_TX5215_SD0_CCLK_ALIEND,
		ts_clk_fixed_divider("sd0_cclk_aligned", "sd0_cclk_div1", 1));
	clk_dm(TS_CLK_TX5215_SD0_CCLK_SHIFT_1_10,
		ts_clk_fixed_divider("sd0_cclk_shift_1_10", "sd0_cclk_div1", 1));
	clk_dm(TS_CLK_TX5215_SD0_CCLK_SHIFT_2_10,
		ts_clk_fixed_divider("sd0_cclk_shift_2_10", "sd0_cclk_div1", 1));
	clk_dm(TS_CLK_TX5215_SD0_CCLK_SHIFT_3_10,
		ts_clk_fixed_divider("sd0_cclk_shift_3_10", "sd0_cclk_div1", 1));
	clk_dm(TS_CLK_TX5215_SD0_CCLK_SHIFT_4_10,
		ts_clk_fixed_divider("sd0_cclk_shift_4_10", "sd0_cclk_div1", 1));
	clk_dm(TS_CLK_TX5215_SD0_CCLK_SHIFT_5_10,
		ts_clk_fixed_divider("sd0_cclk_shift_5_10", "sd0_cclk_div1", 1));
	clk_dm(TS_CLK_TX5215_SD0_CCLK_SHIFT_6_10,
		ts_clk_fixed_divider("sd0_cclk_shift_6_10", "sd0_cclk_div1", 1));
	clk_dm(TS_CLK_TX5215_SD0_CCLK_SHIFT_7_10,
		ts_clk_fixed_divider("sd0_cclk_shift_7_10", "sd0_cclk_div1", 1));
	clk_dm(TS_CLK_TX5215_SD0_CCLK_SHIFT_8_10,
		ts_clk_fixed_divider("sd0_cclk_shift_8_10", "sd0_cclk_div1", 1));
	clk_dm(TS_CLK_TX5215_SD0_CCLK_SHIFT_9_10,
		ts_clk_fixed_divider("sd0_cclk_shift_9_10", "sd0_cclk_div1", 1));
	REGISTER_COMP(TS_CLK_TX5215_SD0_CCLK_SMPL, "sd0_cclk_smpl");
	REGISTER_COMP(TS_CLK_TX5215_SD0_CCLK_DRV, "sd0_cclk_drv");
	REGISTER_COMP(TS_CLK_TX5215_SD1_CCLK_DIV1, "sd1_cclk_div1");
	clk_dm(TS_CLK_TX5215_SD1_CCLK_ALIEND,
		ts_clk_fixed_divider("sd1_cclk_aligned", "sd1_cclk_div1", 1));
	clk_dm(TS_CLK_TX5215_SD1_CCLK_SHIFT_1_10,
		ts_clk_fixed_divider("sd1_cclk_shift_1_10", "sd1_cclk_div1", 1));
	clk_dm(TS_CLK_TX5215_SD1_CCLK_SHIFT_2_10,
		ts_clk_fixed_divider("sd1_cclk_shift_2_10", "sd1_cclk_div1", 1));
	clk_dm(TS_CLK_TX5215_SD1_CCLK_SHIFT_3_10,
		ts_clk_fixed_divider("sd1_cclk_shift_3_10", "sd1_cclk_div1", 1));
	clk_dm(TS_CLK_TX5215_SD1_CCLK_SHIFT_4_10,
		ts_clk_fixed_divider("sd1_cclk_shift_4_10", "sd1_cclk_div1", 1));
	clk_dm(TS_CLK_TX5215_SD1_CCLK_SHIFT_5_10,
		ts_clk_fixed_divider("sd1_cclk_shift_5_10", "sd1_cclk_div1", 1));
	clk_dm(TS_CLK_TX5215_SD1_CCLK_SHIFT_6_10,
		ts_clk_fixed_divider("sd1_cclk_shift_6_10", "sd1_cclk_div1", 1));
	clk_dm(TS_CLK_TX5215_SD1_CCLK_SHIFT_7_10,
		ts_clk_fixed_divider("sd1_cclk_shift_7_10", "sd1_cclk_div1", 1));
	clk_dm(TS_CLK_TX5215_SD1_CCLK_SHIFT_8_10,
		ts_clk_fixed_divider("sd1_cclk_shift_8_10", "sd1_cclk_div1", 1));
	clk_dm(TS_CLK_TX5215_SD1_CCLK_SHIFT_9_10,
		ts_clk_fixed_divider("sd1_cclk_shift_9_10", "sd1_cclk_div1", 1));
	REGISTER_COMP(TS_CLK_TX5215_SD1_CCLK_SMPL, "sd1_cclk_smpl");
	REGISTER_COMP(TS_CLK_TX5215_SD1_CCLK_DRV, "sd1_cclk_drv");
	REGISTER_COMP(TS_CLK_TX5215_OSPI_REF_CLK, "ospi_ref_clk");
	REGISTER_COMP(TS_CLK_TX5215_USB_PHY_CLK, "usb_phy_clk");
	REGISTER_COMP(TS_CLK_TX5215_I2C1_ICCLK, "i2c1_icclk");
	REGISTER_COMP(TS_CLK_TX5215_I2C0_ICCLK, "i2c0_icclk");
	REGISTER_COMP(TS_CLK_TX5215_I2C3_ICCLK, "i2c3_icclk");
	REGISTER_COMP(TS_CLK_TX5215_I2C2_ICCLK, "i2c2_icclk");
	REGISTER_COMP(TS_CLK_TX5215_UART0_SCLK, "uart0_sclk");
	REGISTER_COMP(TS_CLK_TX5215_UART1_SCLK, "uart1_sclk");
	REGISTER_COMP(TS_CLK_TX5215_UART2_SCLK, "uart2_sclk");
	REGISTER_COMP(TS_CLK_TX5215_SPI_SSICLK, "spi_ssiclk");
	REGISTER_COMP(TS_CLK_TX5215_CHIP_OCLK_I0, "chip_oclk_i0");
	REGISTER_COMP(TS_CLK_TX5215_CHIP_OCLK_I1, "chip_oclk_i1");
	REGISTER_COMP(TS_CLK_TX5215_CHIP_OCLK_I2, "chip_oclk_i2");
	REGISTER_COMP(TS_CLK_TX5215_CHIP_OCLK_I3, "chip_oclk_i3");

#undef REGISTER_COMP

	return 0;
}

static int clk_tx5215_register_comps_without_mux(struct ts_clk_params *paras)
{
	void *top_base = paras->top_base;

	/* Dividing clocks, no mux */
#define REGISTER_COMP_NOMUX(id, name, parent) \
		clk_dm(id, \
			clk_tx5215_register_comp(&ts_tx5215_comps[COMPIFY(id)], \
				top_base, name, parent))

	REGISTER_COMP_NOMUX(TS_CLK_TX5215_SD0_CCLK_DIV2, "sd0_cclk_div2", "sd0_cclk_div1");
	REGISTER_COMP_NOMUX(TS_CLK_TX5215_SD1_CCLK_DIV2, "sd1_cclk_div2", "sd1_cclk_div1");
	REGISTER_COMP_NOMUX(TS_CLK_TX5215_TMR_T8CLK, "tmr_t8clk", "pll0_d10");
	REGISTER_COMP_NOMUX(TS_CLK_TX5215_TMR_T7CLK, "tmr_t7clk", "pll0_d10");
	REGISTER_COMP_NOMUX(TS_CLK_TX5215_TMR_T6CLK, "tmr_t6clk", "pll0_d10");
	REGISTER_COMP_NOMUX(TS_CLK_TX5215_TMR_T5CLK, "tmr_t5clk", "pll0_d10");
	REGISTER_COMP_NOMUX(TS_CLK_TX5215_TMR_T4CLK, "tmr_t4clk", &osc_in[0]);
	REGISTER_COMP_NOMUX(TS_CLK_TX5215_TMR_T3CLK, "tmr_t3clk", &osc_in[0]);
	REGISTER_COMP_NOMUX(TS_CLK_TX5215_I2S0_OCLK, "i2s0_oclk", "pll1");
	REGISTER_COMP_NOMUX(TS_CLK_TX5215_I2S0_MCLK, "i2s0_mclk", "pll1");
	REGISTER_COMP_NOMUX(TS_CLK_TX5215_PDM_MCLK, "pdm_mclk", "pll1");
	REGISTER_COMP_NOMUX(TS_CLK_TX5215_DIV1_CODEC_DAC_CCLK, "aud_dac_cclk", "div0_codec_dac_cclk");
	REGISTER_COMP_NOMUX(TS_CLK_TX5215_AUD_DAC_PBCLK, "aud_dac_pbclk", "aud_dac_cclk");
	REGISTER_COMP_NOMUX(TS_CLK_TX5215_DIV1_CODEC_ADC_CCLK, "aud_adc_cclk", "div0_codec_adc_cclk");
	REGISTER_COMP_NOMUX(TS_CLK_TX5215_MCTL_P5_ACLK, "mctl_p5_aclk", "pll0");
	REGISTER_COMP_NOMUX(TS_CLK_TX5215_MCTL_P4_ACLK, "mctl_p4_aclk", "pll0");
	REGISTER_COMP_NOMUX(TS_CLK_TX5215_MCTL_P3_ACLK, "mctl_p3_aclk", "pll0");
	REGISTER_COMP_NOMUX(TS_CLK_TX5215_MCTL_P2_ACLK, "mctl_p2_aclk", "pll0");
	REGISTER_COMP_NOMUX(TS_CLK_TX5215_MCTL_P1_ACLK, "mctl_p1_aclk", "pll0");
	REGISTER_COMP_NOMUX(TS_CLK_TX5215_MCTL_P0_ACLK, "mctl_p0_aclk", "pll0");
	REGISTER_COMP_NOMUX(TS_CLK_TX5215_VI_DDR_ACLK, "vi_ddr_aclk", "pll0");
	REGISTER_COMP_NOMUX(TS_CLK_TX5215_ISP_SCLK, "isp_sclk", "pll0");
	REGISTER_COMP_NOMUX(TS_CLK_TX5215_VPE_CCLK, "vpe_cclk", "pll0");
	REGISTER_COMP_NOMUX(TS_CLK_TX5215_MIPI_TXCLKESC, "mipi_txclkesc", "pll0_d10");
	REGISTER_COMP_NOMUX(TS_CLK_TX5215_CV_DDR_ACLK, "cv_ddr_aclk", "pll0");
	REGISTER_COMP_NOMUX(TS_CLK_TX5215_DISP_DDR_ACLK, "disp_ddr_aclk", "pll0");
	REGISTER_COMP_NOMUX(TS_CLK_TX5215_NEMADC_CCLK, "nemadc_cclk", "pll0");
	REGISTER_COMP_NOMUX(TS_CLK_TX5215_CVE_CCLK, "cve_cclk", "pll0");
	REGISTER_COMP_NOMUX(TS_CLK_TX5215_AMR_CCLK, "amr_cclk", "pll0");

#undef REGISTER_COMP_NOMUX
	return 0;
}


static int clk_tx5215_register_dividers(struct ts_clk_params *paras)
{
	void *top_base = paras->top_base;

#define REGISTER_DIV(id, name, parent) \
		clk_dm(id, \
			clk_tx5215_register_comp(&ts_tx5215_comps[COMPIFY(id)], \
				top_base, name, parent))

	REGISTER_DIV(TS_CLK_TX5215_CPU_ACLK, "cpu_aclk", "cpu_cclk");
	REGISTER_DIV(TS_CLK_TX5215_CPU_DBG_PCLK, "cpu_dbg_pclk", "cpu_cclk");
	REGISTER_DIV(TS_CLK_TX5215_PERI_APB_CLK, "peri_apb_clk", "top_ahb_clk");
	//REGISTER_DIV(TS_CLK_TX5215_DIV0_CODEC_DAC_CCLK, "div0_codec_dac_cclk", "pll1");
	//REGISTER_DIV(TS_CLK_TX5215_DIV0_CODEC_ADC_CCLK, "div0_codec_adc_cclk", "pll1");
	REGISTER_DIV(TS_CLK_TX5215_VPU_ACLK, "vpu_aclk", "pll0");


#undef REGISTER_DIV
	return 0;
}

static int clk_tx5215_register_gates(struct ts_clk_params *paras)
{
	void *top_base = paras->top_base;

#define REGISTER_GATE(id, name, parent) \
		clk_dm(id, \
			clk_tx5215_register_comp(&ts_tx5215_comps[COMPIFY(id)], \
				top_base, name, parent))

	REGISTER_GATE(TS_CLK_TX5215_TRNG_CLK, "trng_clk", "top_axi_clk");
	REGISTER_GATE(TS_CLK_TX5215_PKE_CLK, "pke_clk", "top_axi_clk");
	REGISTER_GATE(TS_CLK_TX5215_HASH_CLK, "hash_clk", "top_axi_clk");
	REGISTER_GATE(TS_CLK_TX5215_SPACC_CLK, "spacc_clk", "top_axi_clk");
	REGISTER_GATE(TS_CLK_TX5215_SKE_CLK, "ske_clk", "top_axi_clk");
	REGISTER_GATE(TS_CLK_TX5215_OCRAM_ACLK, "ocram_aclk", "top_axi_clk");
	REGISTER_GATE(TS_CLK_TX5215_MEM_DMA_ACLK, "mem_dma_aclk", "top_axi_clk");

	//REGISTER_GATE(TS_CLK_TX5215_AUD_CODEC_HCLK, "aud_codec_hclk", "top_ahb_clk");
	REGISTER_GATE(TS_CLK_TX5215_OSPI_HCLK, "ospi_hclk", "top_ahb_clk");
	REGISTER_GATE(TS_CLK_TX5215_USB2C_HCLK, "usb2c_hclk", "top_ahb_clk");
	REGISTER_GATE(TS_CLK_TX5215_SDHC1_HCLK, "sdhc1_hclk", "top_ahb_clk");
	REGISTER_GATE(TS_CLK_TX5215_SDHC0_HCLK, "sdhc0_hclk", "top_ahb_clk");
	REGISTER_GATE(TS_CLK_TX5215_PERI_DMA1_HCLK, "peri_dma1_hclk", "top_ahb_clk");
	REGISTER_GATE(TS_CLK_TX5215_PERI_DMA0_HCLK, "peri_dma0_hclk", "top_ahb_clk");
	REGISTER_GATE(TS_CLK_TX5215_BOOTROM_HCLK, "bootrom_hclk", "top_ahb_clk");

	REGISTER_GATE(TS_CLK_TX5215_I2S0_PCLK, "i2s0_pclk", "peri_apb_clk");
	REGISTER_GATE(TS_CLK_TX5215_USI1_PCLK, "usi1_pclk", "peri_apb_clk");
	REGISTER_GATE(TS_CLK_TX5215_USI0_PCLK, "usi0_pclk", "peri_apb_clk");
	REGISTER_GATE(TS_CLK_TX5215_UART2_PCLK, "uart2_pclk", "peri_apb_clk");
	REGISTER_GATE(TS_CLK_TX5215_UART1_PCLK, "uart1_pclk", "peri_apb_clk");
	REGISTER_GATE(TS_CLK_TX5215_UART0_PCLK, "uart0_pclk", "peri_apb_clk");
	REGISTER_GATE(TS_CLK_TX5215_I2C3_PCLK, "i2c3_pclk", "peri_apb_clk");
	REGISTER_GATE(TS_CLK_TX5215_I2C2_PCLK, "i2c2_pclk", "peri_apb_clk");
	REGISTER_GATE(TS_CLK_TX5215_I2C1_PCLK, "i2c1_pclk", "peri_apb_clk");
	REGISTER_GATE(TS_CLK_TX5215_I2C0_PCLK, "i2c0_pclk", "peri_apb_clk");

	REGISTER_GATE(TS_CLK_TX5215_GPIO_B_PCLK, "gpio_b_pclk", "peri_apb_clk");
	REGISTER_GATE(TS_CLK_TX5215_GPIO_A_PCLK, "gpio_a_pclk", "peri_apb_clk");
	REGISTER_GATE(TS_CLK_TX5215_PDM_PCLK, "pdm_pclk", "peri_apb_clk");
	REGISTER_GATE(TS_CLK_TX5215_SPI_PCLK, "spi_pclk", "peri_apb_clk");
	REGISTER_GATE(TS_CLK_TX5215_SYS_REG_PCLK, "sys_reg_pclk", "peri_apb_clk");
	REGISTER_GATE(TS_CLK_TX5215_OTPC_PCLK, "otpc_pclk", "peri_apb_clk");
	REGISTER_GATE(TS_CLK_TX5215_OSPI_PCLK, "ospi_pclk", "peri_apb_clk");
	REGISTER_GATE(TS_CLK_TX5215_PWM_PCLK, "pwm_pclk", "peri_apb_clk");
	REGISTER_GATE(TS_CLK_TX5215_TMR_PCLK, "tmr_pclk", "peri_apb_clk");
	REGISTER_GATE(TS_CLK_TX5215_WDT_PCLK, "wdt_pclk", "peri_apb_clk");
	REGISTER_GATE(TS_CLK_TX5215_GPIO_PCLK, "gpio_pclk", "peri_apb_clk");
	REGISTER_GATE(TS_CLK_TX5215_ADC_PCLK, "adc_pclk", "peri_apb_clk");
	REGISTER_GATE(TS_CLK_TX5215_DDRC_CORE_CLK, "ddrc_core_clk", "pll0");//parent uncertain
	REGISTER_GATE(TS_CLK_TX5215_DDR_PHY_PCLK, "ddr_phy_pclk", "pll0");//parent uncertain
	REGISTER_GATE(TS_CLK_TX5215_UMCTL_PCLK, "umctl_pclk", "peri_apb_clk");//parent uncertain
	REGISTER_GATE(TS_CLK_TX5215_VI_CFG_HCLK, "vi_cfg_hclk", "top_ahb_clk");
	REGISTER_GATE(TS_CLK_TX5215_MIPI_PHY_PCLK, "mipi_phy_pclk", "vi_cfg_hclk");
	REGISTER_GATE(TS_CLK_TX5215_MIPI_RX1_PCLK, "mipi_rx1_pclk", "vi_cfg_hclk");
	REGISTER_GATE(TS_CLK_TX5215_MIPI_RX0_PCLK, "mipi_rx0_pclk", "vi_cfg_hclk");
	REGISTER_GATE(TS_CLK_TX5215_HDR_HCLK, "hdr_hclk", "vi_cfg_hclk");
	REGISTER_GATE(TS_CLK_TX5215_VPE_HCLK, "vpe_hclk", "vi_cfg_hclk");
	REGISTER_GATE(TS_CLK_TX5215_HDR_ACLK, "hdr_aclk", "vi_ddr_aclk");
	REGISTER_GATE(TS_CLK_TX5215_VPE_ACLK, "vpe_aclk", "vi_ddr_aclk");
	REGISTER_GATE(TS_CLK_TX5215_ISP_ACLK, "isp_aclk", "vi_ddr_aclk");
	REGISTER_GATE(TS_CLK_TX5215_MIPI_RX1_PIXCLK, "mipi_rx1_pixclk", "isp_sclk");
	REGISTER_GATE(TS_CLK_TX5215_MIPI_RX0_PIXCLK1, "mipi_rx0_pixclk1", "isp_sclk");
	REGISTER_GATE(TS_CLK_TX5215_MIPI_RX0_PIXCLK0, "mipi_rx0_pixclk0", "isp_sclk");
	REGISTER_GATE(TS_CLK_TX5215_HDR_SCLK, "hdr_sclk", "isp_sclk");
	REGISTER_GATE(TS_CLK_TX5215_VPE_ISP_CLK, "vpe_isp_clk", "isp_sclk");
	REGISTER_GATE(TS_CLK_TX5215_DVP_HCLK, "dvp_hclk", "top_ahb_clk");
	REGISTER_GATE(TS_CLK_TX5215_CVE_HCLK, "cve_hclk", "top_ahb_clk");
	REGISTER_GATE(TS_CLK_TX5215_AMR_HCLK, "amr_hclk", "top_ahb_clk");
	REGISTER_GATE(TS_CLK_TX5215_DISP_CFG_HCLK, "disp_cfg_hclk", "top_ahb_clk");
	REGISTER_GATE(TS_CLK_TX5215_NEMADC_HCLK, "nemadc_hclk", "disp_ddr_aclk");
	REGISTER_GATE(TS_CLK_TX5215_DVP_ACLK, "dvp_aclk", "disp_ddr_aclk");
	REGISTER_GATE(TS_CLK_TX5215_CVE_ACLK, "cve_aclk", "cv_ddr_aclk");
	REGISTER_GATE(TS_CLK_TX5215_AMR_ACLK, "amr_aclk", "cv_ddr_aclk");
	REGISTER_GATE(TS_CLK_TX5215_RNE_HCLK, "rne_hclk", "top_ahb_clk");
	REGISTER_GATE(TS_CLK_TX5215_RNE_ACLK, "rne_aclk", "mctl_p2_aclk");
	REGISTER_GATE(TS_CLK_TX5215_VPU_HCLK, "vpu_hclk", "top_ahb_clk");
	REGISTER_GATE(TS_CLK_TX5215_H265_CCLK, "h265_cclk", "pll0"); //parent uncertain
	REGISTER_GATE(TS_CLK_TX5215_H265_PCLK, "h265_pclk", "vpu_aclk");
	REGISTER_GATE(TS_CLK_TX5215_H265_ACLK, "h265_aclk", "vpu_aclk");
	REGISTER_GATE(TS_CLK_TX5215_H264_PCLK, "h264_pclk", "vpu_aclk");
	REGISTER_GATE(TS_CLK_TX5215_H264_ACLK, "h264_aclk", "vpu_aclk");

#undef REGISTER_GATE
	return 0;
}

int ts_clk_init_funcs(struct ts_clk_reg_funcs *pfuncs)
{
	if (!pfuncs)
		return -EINVAL;

	pfuncs->register_plls = clk_tx5215_register_plls;
	pfuncs->register_comps = clk_tx5215_register_comps;
	pfuncs->register_comps_without_mux = clk_tx5215_register_comps_without_mux;
	pfuncs->register_dividers = clk_tx5215_register_dividers;
	pfuncs->register_gates = clk_tx5215_register_gates;
	return 0;
}

// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2022-2022 Tsingmicro, Inc.
 */
#include <linux/io.h>
#include <linux/uaccess.h>
#include <linux/clk.h>
#include <linux/clkdev.h>
#include <linux/clk-provider.h>
#include <dt-bindings/clock/ts-tx5336-clk.h>

#include "ts_pll.h"
#include "ts_clk.h"

/* TOP_CGU */
#define PLL0_CFG0			0x000
#define PLL0_CFG1			0x004
#define PLL1_CFG0			0x010
#define PLL1_CFG1			0x014
#define PLL2_CFG0			0x020
#define PLL2_CFG1			0x024
#define PLL_STATUS			0x02c
#define CPU_CFG0			0x030
#define CPU_CFG1			0x034
#define TOP_RST_CFG			0x040
#define TOP_CLK_CFG			0x044
#define PERIPH0_CFG0		0x048
#define PERIPH0_CFG1		0x04c
#define PERIPH1_CFG0		0x050
#define PERIPH1_CFG1		0x054
#define PERIPH2_CFG0		0x058
#define PERIPH2_CFG1		0x05c
#define QSPI_CFG			0x060
#define AUD_AHB_CFG			0x064
#define AUD_DAC_CFG			0x068
#define AUD_ED_TIMER_CFG	0x06c
#define AUD_ADC_CFG			0x070
#define SPI_CFG0			0x074
#define SPI_CFG1			0x078
#define TIMER_CFG0			0x07C
#define TIMER_CFG1			0x080
#define TIMER_CFG2			0x084
#define TIMER_CFG3			0x088
#define I2C_CFG0			0x090
#define I2C_CFG1			0x094
#define I2C_CFG2			0x098
#define I2C_CFG3			0x09C
#define UART_CFG0			0x0A0
#define UART_CFG1			0x0A4
#define UART_CFG2			0x0A8
#define UART_CFG3			0x0AC
#define UART_CFG4			0x0B0
#define I2S_CFG0			0x0B4
#define SDHC_CFG0			0x0B8
#define	SDHC_CFG1			0x0BC
#define USB_CFG				0x0C0
#define GMAC_CFG0			0x0C4
#define GMAC_CFG1			0x0C8
#define GMAC_CFG2			0x0CC
#define UMCTL_CFG0			0x0D0
#define UMCTL_CFG1			0x0D4
#define UMCTL_CFG2			0x0D8
#define UMCTL_CFG3			0x0DC
#define RNE_CFG0			0x0E0
#define RNE_CFG1			0x0E4
#define VI_CFG				0x0EC
#define ISP_CFG				0x0F0
#define VPE_CFG				0x0F4
#define DOF_CFG				0x0F8
#define EBD_CFG				0x0FC
#define MIPI_RX_CFG			0x0100
#define CV_CFG				0x0104
#define CRYPTO_CFG			0x0108
#define AMR_CFG				0x010C
#define CVE_CFG				0x0110
#define BIS_CFG				0x0114
#define GDC_CFG				0x0118
#define DPU_CFG				0x011C
#define MIPI_TX_CFG			0x0120
#define ADMA_CFG			0x0124
#define CHIP_OCLK0_CFG		0x0128
#define CHIP_OCLK1_CFG		0x012C
#define CHIP_OCLK2_CFG		0x0130
#define CHIP_OCLK3_CFG		0x0134
#define PDM_CFG				0x0138
#define PDM_CFG1			0x013C
#define VPU_CFG0			0x0140
#define VPU_CFG1			0x0144
#define PLL3_CFG0			0x0150
#define PLL3_CFG1			0x0154
/* All methods are delegated to CCF clocks */
/* Parents for muxed clocks */

static const char * const pll_mux_sels[] = { "cpu_cclk_pll1", "cpu_cclk_pll0"};
static const char * const cpu_cclk_sels[] = { &osc_in[0], "pll_mux"};
static const char * const gic_clk_sels[] = {  &osc_in[0], "pll0_d2" };

static const char * const pll_mux_topnoc_clk_sels[] = {  "topnoc_pll1_d2", "topnoc_pll0"};
static const char * const topnoc_clk_sels[] = {  &osc_in[0], "pll_mux_topnoc"};
static const char * const peri_apb_clk_sels[] = {  &osc_in[0], "pll0_d2"};

static const char * const qspi_ref_clk_sels[] = {  &osc_in[0], "pll0_d2"};
static const char * const qspi_ahb_clk_sels[] = {  &osc_in[0], "pll0_d2"};

static const char * const spi_core_clk_sels[] = {  &osc_in[0], "pll0_d2"};
static const char * const i2c_core_clk_sels[] = {  &osc_in[0], "pll0_d4"};
static const char * const uart_core_clk_sels[] = {  &osc_in[0], "pll0_d8"};
static const char * const sdhc_cclk_sels[] = {  &osc_in[0], "pll0"};
static const char * const usb_ref_clk_sels[] = {  &osc_in[0], "usb_ref_pll0_d4"};

static const char * const gmac_ref_clk_sels[] = {  "pll0_d4", };
static const char * const umctl_p1_clk_sels[] = { "umctl_p1_pll1_d2","umctl_p1_pll0" };
static const char * const umctl_p2_clk_sels[] = { "umctl_p2_pll1_d2" ,"umctl_p2_pll0" };
static const char * const umctl_p4_clk_sels[] = {  "umctl_p4_pll1_d2","umctl_p4_pll0" };
static const char * const pll01_mux_rne_core_clk_sels[] = {  "rne_core_pll1_d2_clk","rne_core_pll0_clk"};
static const char * const rne_core_clk_sels[] = {   &osc_in[0],"rne_core_pll01"};
static const char * const membuf_axi_clk_sels[] = {   &osc_in[0],"pll0"};
static const char * const vi_noc_clk_sels[] = {  "vi_noc_pll1_d2","vi_noc_pll0"};
static const char * const isp_drv_clk_sels[] = {  "isp_drv_pll1_d2","isp_drv_pll0"};
static const char * const isp_axi_clk_sels[] = {  "isp_axi_pll1_d2","isp_axi_pll0"};
static const char * const vpe_core_clk_sels[] = {  "vpe_core_pll1_d2","vpe_core_pll0"};
static const char * const vpe_axi_clk_sels[] = {  "vpe_axi_pll1_d2","vpe_axi_pll0"};
static const char * const dof_axi_clk_sels[] = {  "dof_axi_pll1_d2","dof_axi_pll0"};
static const char * const cv_clk_sels[] = {  &osc_in[0],"pll0"};
static const char * const mux_cv_ahb_clk_sels[] = {  &osc_in[0],"pll0"};
static const char * const bis_core_clk_sels[] = {   "bis_core_pll1_d2", "bis_core_pll0"};
static const char * const gdc_core_clk_sels[] = {  "gdc_core_pll1_d2","gdc_core_pll0"};
static const char * const mipitx_refclk_sels[] = {  &osc_in[0],"mipi_ref_pll0_d4"};
static const char * const chip_oclk_sels[] = {&osc_in[0], "chip_oclk0_osc","chip_oclk0_pll0_d4","chip_oclk0_pll1_d8"};
static const char * const chip_oclk1_sels[] = {&osc_in[0], "chip_oclk1_osc","chip_oclk1_pll0_d4","chip_oclk1_pll1_d8"};
static const char * const chip_oclk2_sels[] = {&osc_in[0], "chip_oclk2_osc","chip_oclk2_pll0_d4","chip_oclk2_pll1_d8"};
static const char * const chip_oclk3_sels[] = {&osc_in[0], "chip_oclk3_osc","chip_oclk3_pll0_d4","chip_oclk3_pll1_d8"};
static const char * const vpu_noc_clk_sels[] = {  "vpu_noc_pll1_d2","vpu_noc_pll0"};

struct ts_mmc_clock {
	struct clk_hw	hw;
	void __iomem	*reg;
	int		id;
	int		shift;
    int     we;
    uint8_t width;
};

#define to_mmc_clock(_hw) container_of(_hw, struct ts_mmc_clock, hw)
#define PLL_CCLK_RATE     (2000000000UL)

#define __MTYMASK(length)                    \
    ({       \
        int i;                           \
        typeof(length) mask = 0;        \
        for (i = 0; i < (length); ++i) { \
            mask |= (1 << i);           \
        }                               \
        mask;                           \
    })

static int ts_mmc_set_phase(struct clk_hw *hw, int degrees)
{
	struct ts_mmc_clock *mmc_clock = to_mmc_clock(hw);
    unsigned long mmc_cclk_rate = clk_get_rate(hw->clk);

    uint32_t shift_reg_val;
    uint8_t bitoff = mmc_clock->shift;
    void __iomem *reg = mmc_clock->reg;
    uint8_t write_enable_bit = mmc_clock->we;
    uint8_t width = mmc_clock->width;

    uint32_t tmp;
    uint8_t DEGREES_PER_SHFITV;

    DEGREES_PER_SHFITV = (360) / (PLL_CCLK_RATE / mmc_cclk_rate);

    /* degrees = DEGREE_PER_SHIFTV(10) * shift_reg_val */
    shift_reg_val = DIV_ROUND_UP(degrees, DEGREES_PER_SHFITV);

    tmp = readl(reg);
    /* spurious action that smpl feild should be cleard */
    tmp &= (~(__MTYMASK(width) << bitoff));
    tmp |= (1) << (write_enable_bit);
    tmp |= ((shift_reg_val & __MTYMASK(width)) << (bitoff));
    writel(tmp, reg);

    pr_debug("write:val:%u PER_SHFITV:%d we:%d tmp:%u\n",
        shift_reg_val, DEGREES_PER_SHFITV, write_enable_bit, tmp);

	return 0;
}

static const struct clk_ops ts_mmc_clk_ops = {
	// .recalc_rate	= ts_mmc_recalc,
	// .get_phase	= ts_mmc_get_phase,
	.set_phase	= ts_mmc_set_phase,
};

/* phase tuning clocks */
#define SHIFT(id, off, shift, width, we, sync, flags) SHIFT_FLAGS(id, off, shift, width, we, sync, flags)
#define SHIFT_LIST \
    SHIFT(TS_CLK_TX5336_SDHC0_SMPL_CLK, SDHC_CFG0, 24, 6, 0, SYNC_INVALID, 0)

#define _SHIFTIFY(id) TS_CLK_SHIFT_##id
#define SHIFTIFY(id) _SHIFTIFY(id)

enum ts_shift_ids {
#define SHIFT_FLAGS(id, ...) SHIFTIFY(id),
	SHIFT_LIST
#undef SHIFT_FLAGS
	TS_CLK_SHIFT_NONE,
};

static const struct ts_shift_params ts_tx5336_shifts[] = {
#define SHIFT_FLAGS(id, _off, _shift, _width, _we, _sync, _flags) \
	[SHIFTIFY(id)] = { \
		.off = (_off), \
		.shift = (_shift), \
		.width = (_width), \
		.we = (_we), \
		.sync = (_sync), \
		.flags = (_flags), \
	},
	SHIFT_LIST
#undef SHIFT_FLAGS
};

#undef SHIFT
#undef SHIFT_LIST

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
//-----------------------------------------------1 div----------------------------------------------------------------
#define DIV(id, off, shift, width, we, sync, flags) DIV_FLAGS(id, off, shift, width, we, sync, flags)
#define DIV_LIST \
	DIV(TS_CLK_TX5336_CPU_PLL0_CCLK, CPU_CFG1, 12, 3, 0, SYNC_INVALID, 0) \
	DIV(TS_CLK_TX5336_CPU_PLL1_CCLK, CPU_CFG1, 8, 3, 0, SYNC_INVALID, 0) \
	DIV(TS_CLK_TX5336_GIC_CLK, CPU_CFG0, 20, 4, 16, SYNC_INVALID, 0) \
	DIV(TS_CLK_TX5336_CPU_ACLK, CPU_CFG1, 24, 4, 0, SYNC_INVALID, 0) \
	DIV(TS_CLK_TX5336_PLL0_TOPNOC_CCLK, TOP_CLK_CFG, 12, 4, 0, SYNC_INVALID, 0) \
	DIV(TS_CLK_TX5336_PLL1_D2_TOPNOC_CLK, TOP_CLK_CFG, 8, 4, 0, SYNC_INVALID, 0) \
	DIV(TS_CLK_TX5336_PERI0_APB_CLK, PERIPH0_CFG0, 8, 4, 0, SYNC_INVALID, 0) \
	DIV(TS_CLK_TX5336_PERI1_APB_CLK, PERIPH1_CFG0, 8, 4, 0, SYNC_INVALID, 0) \
	DIV(TS_CLK_TX5336_PERI2_AHB_CLK, PERIPH2_CFG0, 8, 4, 0, SYNC_INVALID, 0) \
	DIV(TS_CLK_TX5336_QSPI_AHB_CLK, QSPI_CFG, 12, 4, 0, SYNC_INVALID, 0) \
	DIV(TS_CLK_TX5336_QSPI_REF_CLK, QSPI_CFG, 16, 4, 0, SYNC_INVALID, 0) \
	DIV(TS_CLK_TX5336_AUD_CODEC_AHB_CLK, AUD_AHB_CFG, 8, 4, 0, SYNC_INVALID, 0) \
	DIV(TS_CLK_TX5336_AUD_DAC_PBCLK , AUD_DAC_CFG, 20, 6, 0, SYNC_INVALID, 0) \
	DIV(TS_CLK_TX5336_AUD_DAC_CCLK, AUD_DAC_CFG, 8, 12, 0, SYNC_INVALID, 0) \
	DIV(TS_CLK_TX5336_AUD_ED_TMR_STRB_CLK, AUD_ED_TIMER_CFG, 16, 10, 0, SYNC_INVALID, 0) \
	DIV(TS_CLK_TX5336_AUD_ADC_CCLK, AUD_ADC_CFG, 8, 12, 0, SYNC_INVALID, 0) \
	DIV(TS_CLK_TX5336_SPI0_CORE_CLK, SPI_CFG0, 8, 4, 0, SYNC_INVALID, 0) \
	DIV(TS_CLK_TX5336_SPI1_CORE_CLK, SPI_CFG0, 24, 4, 16, SYNC_INVALID, 0) \
	DIV(TS_CLK_TX5336_SPI2_CORE_CLK, SPI_CFG1, 8, 4, 0, SYNC_INVALID, 0) \
	DIV(TS_CLK_TX5336_TIME0_7_CLK, TIMER_CFG1, 18, 6, 0, SYNC_INVALID, 0) \
	DIV(TS_CLK_TX5336_TIME0_8_CLK, TIMER_CFG1, 24, 6, 0, SYNC_INVALID, 0) \
	DIV(TS_CLK_TX5336_TIME0_6_CLK, TIMER_CFG1, 12, 6, 0, SYNC_INVALID, 0) \
	DIV(TS_CLK_TX5336_TIME0_5_CLK, TIMER_CFG0, 16, 6, 0, SYNC_INVALID, 0) \
	DIV(TS_CLK_TX5336_TIME1_7_CLK, TIMER_CFG3, 18, 6, 0, SYNC_INVALID, 0) \
	DIV(TS_CLK_TX5336_TIME1_8_CLK, TIMER_CFG3, 24, 6, 0, SYNC_INVALID, 0) \
	DIV(TS_CLK_TX5336_TIME1_6_CLK, TIMER_CFG3, 12, 6, 0, SYNC_INVALID, 0) \
	DIV(TS_CLK_TX5336_TIME1_5_CLK, TIMER_CFG2, 16, 6, 0, SYNC_INVALID, 0) \
	DIV(TS_CLK_TX5336_I2S_O_CLK, I2S_CFG0, 8, 12, 0, SYNC_INVALID, 0) \
	DIV(TS_CLK_TX5336_I2S_M_CLK, I2S_CFG0, 20, 12, 0, SYNC_INVALID, 0) \
	DIV(TS_CLK_TX5336_SDHC0_CCLK, SDHC_CFG0, 8, 6, 0, SYNC_INVALID, 0) \
	DIV(TS_CLK_TX5336_SDHC1_CCLK, SDHC_CFG1, 8, 6, 0, SYNC_INVALID, 0) \
	DIV(TS_CLK_TX5336_USB_REF_PLL0_D4_CLK, USB_CFG, 8, 4, 0, SYNC_INVALID, 0) \
	DIV(TS_CLK_TX5336_DDR_APB_CLK, UMCTL_CFG0, 4, 4, 0, SYNC_INVALID, 0) \
	DIV(TS_CLK_TX5336_UMCTL_P0_CLK, UMCTL_CFG1, 12, 4, 0, SYNC_INVALID, 0) \
	DIV(TS_CLK_TX5336_UMCTL_P1_PLL0_CLK, UMCTL_CFG1, 28, 4, 16, SYNC_INVALID, 0) \
	DIV(TS_CLK_TX5336_UMCTL_P1_PLL1_D2_CLK, UMCTL_CFG1, 24, 4, 16, SYNC_INVALID, 0) \
	DIV(TS_CLK_TX5336_UMCTL_P2_PLL0_CLK, UMCTL_CFG2, 12, 4, 0, SYNC_INVALID, 0) \
	DIV(TS_CLK_TX5336_UMCTL_P2_PLL1_D2_CLK, UMCTL_CFG2, 8, 4, 0, SYNC_INVALID, 0) \
	DIV(TS_CLK_TX5336_UMCTL_P3_CLK, UMCTL_CFG2, 28, 4, 16, SYNC_INVALID, 0) \
	DIV(TS_CLK_TX5336_UMCTL_P4_PLL0_CLK, UMCTL_CFG3, 12, 4, 0, SYNC_INVALID, 0) \
	DIV(TS_CLK_TX5336_UMCTL_P4_PLL1_D2_CLK, UMCTL_CFG3, 8, 4, 0, SYNC_INVALID, 0) \
	DIV(TS_CLK_TX5336_UMCTL_P5_CLK, UMCTL_CFG3, 28, 4, 16, SYNC_INVALID, 0) \
	DIV(TS_CLK_TX5336_RNE_AHB_CLK, RNE_CFG0, 28, 4, 0, SYNC_INVALID, 0) \
	DIV(TS_CLK_TX5336_RNE_AXI_CLK, RNE_CFG0, 24, 4, 0, SYNC_INVALID, 0) \
	DIV(TS_CLK_TX5336_RNE_CORE_PLL0_CLK, RNE_CFG0, 20, 4, 0, SYNC_INVALID, 0) \
	DIV(TS_CLK_TX5336_RNE_CORE_PLL1_D2_CLK, RNE_CFG0, 16, 4, 0, SYNC_INVALID, 0) \
	DIV(TS_CLK_TX5336_MEMBUF_AXI_CLK, RNE_CFG1, 16, 4, 0, SYNC_INVALID, 0) \
	DIV(TS_CLK_TX5336_VI_AHB_CLK, VI_CFG, 16, 4, 0, SYNC_INVALID, 0) \
	DIV(TS_CLK_TX5336_VI_APB_CLK, VI_CFG, 20, 4, 0, SYNC_INVALID, 0) \
	DIV(TS_CLK_TX5336_VI_NOC_PLL0_CLK, VI_CFG, 12, 4, 0, SYNC_INVALID, 0) \
	DIV(TS_CLK_TX5336_VI_NOC_PLL1_D2_CLK, VI_CFG, 8, 4, 0, SYNC_INVALID, 0) \
	DIV(TS_CLK_TX5336_ISP_AXI_PLL0_CLK, ISP_CFG, 12, 4, 0, SYNC_INVALID, 0) \
	DIV(TS_CLK_TX5336_ISP_AXI_PLL1_D2_CLK, ISP_CFG, 8, 4, 0, SYNC_INVALID, 0) \
	DIV(TS_CLK_TX5336_ISP_DRV_PLL0_CLK, ISP_CFG, 24, 4, 16, SYNC_INVALID, 0) \
	DIV(TS_CLK_TX5336_ISP_DRV_PLL1_D2_CLK, ISP_CFG, 20, 4, 16, SYNC_INVALID, 0) \
	DIV(TS_CLK_TX5336_VPE_AXI_PLL0_CLK, VPE_CFG, 16, 4, 0, SYNC_INVALID, 0) \
	DIV(TS_CLK_TX5336_VPE_AXI_PLL1_D2_CLK, VPE_CFG, 12, 4, 0, SYNC_INVALID, 0) \
	DIV(TS_CLK_TX5336_VPE_CORE_PLL0_CLK, VPE_CFG, 24, 4, 0, SYNC_INVALID, 0) \
	DIV(TS_CLK_TX5336_VPE_CORE_PLL1_D2_CLK, VPE_CFG, 20, 4, 0, SYNC_INVALID, 0) \
	DIV(TS_CLK_TX5336_DOF_AXI_PLL0_CLK, DOF_CFG, 16, 4, 0, SYNC_INVALID, 0) \
	DIV(TS_CLK_TX5336_DOF_AXI_PLL1_D2_CLK, DOF_CFG, 12, 4, 0, SYNC_INVALID, 0) \
	DIV(TS_CLK_TX5336_EBD_AXI_CLK, EBD_CFG, 12, 4, 0, SYNC_INVALID, 0) \
	DIV(TS_CLK_TX5336_MIPI_TXCLKESC_CLK, MIPI_RX_CFG, 24, 5, 0, SYNC_INVALID, 0) \
	DIV(TS_CLK_TX5336_CV_AHB_CLK, CV_CFG, 16, 4, 0, SYNC_INVALID, 0) \
	DIV(TS_CLK_TX5336_CV_APB_CLK, CV_CFG, 20, 4, 0, SYNC_INVALID, 0) \
	DIV(TS_CLK_TX5336_CV_NOC_CLK, CV_CFG, 24, 4, 0, SYNC_INVALID, 0) \
	DIV(TS_CLK_TX5336_AMR_AXI_CLK, AMR_CFG, 8, 4, 0, SYNC_INVALID, 0) \
	DIV(TS_CLK_TX5336_AMR_CORE_CLK, AMR_CFG, 12, 4, 0, SYNC_INVALID, 0) \
	DIV(TS_CLK_TX5336_CVE_AXI_CLK, CVE_CFG, 8, 4, 0, SYNC_INVALID, 0) \
	DIV(TS_CLK_TX5336_CVE_CORE_CLK, CVE_CFG, 12, 4, 0, SYNC_INVALID, 0) \
	DIV(TS_CLK_TX5336_BIS_AXI_CLK, BIS_CFG, 20, 4, 0, SYNC_INVALID, 0) \
	DIV(TS_CLK_TX5336_BIS_CORE_PLL0_CLK, BIS_CFG, 28, 4, 0, SYNC_INVALID, 0) \
	DIV(TS_CLK_TX5336_BIS_CORE_PLL1_D2_CLK, BIS_CFG, 24, 4, 0, SYNC_INVALID, 0) \
	DIV(TS_CLK_TX5336_GDC_AXI_CLK, GDC_CFG, 20, 4, 0, SYNC_INVALID, 0) \
	DIV(TS_CLK_TX5336_GDC_CORE_PLL0_CLK, GDC_CFG, 28, 4, 0, SYNC_INVALID, 0) \
	DIV(TS_CLK_TX5336_GDC_CORE_PLL1_D2_CLK, GDC_CFG, 24, 4, 0, SYNC_INVALID, 0) \
	DIV(TS_CLK_TX5336_DPU_AXI_CLK, DPU_CFG, 8, 4, 0, SYNC_INVALID, 0) \
	DIV(TS_CLK_TX5336_DPU_CORE_CLK, DPU_CFG, 12, 6, 0, SYNC_INVALID, 0) \
	DIV(TS_CLK_TX5336_MIPI_REF_PLL0_D4_CLK, MIPI_TX_CFG, 8, 5, 0, SYNC_INVALID, 0) \
	DIV(TS_CLK_TX5336_ADMA_AXI_CLK, ADMA_CFG, 8, 4, 0, SYNC_INVALID, 0) \
	DIV(TS_CLK_TX5336_CHIP_OCLK0_OSC_CLK, CHIP_OCLK0_CFG, 8, 4, 0, SYNC_INVALID, 0) \
	DIV(TS_CLK_TX5336_CHIP_OCLK0_pll0_d4_CLK, CHIP_OCLK0_CFG, 12, 5, 0, SYNC_INVALID, 0) \
	DIV(TS_CLK_TX5336_CHIP_OCLK0_pll1_d8_CLK, CHIP_OCLK0_CFG, 20, 5, 0, SYNC_INVALID, 0) \
	DIV(TS_CLK_TX5336_CHIP_OCLK1_OSC_CLK, CHIP_OCLK1_CFG, 8, 4, 0, SYNC_INVALID, 0) \
	DIV(TS_CLK_TX5336_CHIP_OCLK1_pll0_d4_CLK, CHIP_OCLK1_CFG, 12, 5, 0, SYNC_INVALID, 0) \
	DIV(TS_CLK_TX5336_CHIP_OCLK1_pll1_d8_CLK, CHIP_OCLK1_CFG, 20, 5, 0, SYNC_INVALID, 0) \
	DIV(TS_CLK_TX5336_CHIP_OCLK2_OSC_CLK, CHIP_OCLK2_CFG, 8, 4, 0, SYNC_INVALID, 0) \
	DIV(TS_CLK_TX5336_CHIP_OCLK2_pll0_d4_CLK, CHIP_OCLK2_CFG, 12, 5, 0, SYNC_INVALID, 0) \
	DIV(TS_CLK_TX5336_CHIP_OCLK2_pll1_d8_CLK, CHIP_OCLK2_CFG, 20, 5, 0, SYNC_INVALID, 0) \
	DIV(TS_CLK_TX5336_CHIP_OCLK3_OSC_CLK, CHIP_OCLK3_CFG, 8, 4, 0, SYNC_INVALID, 0) \
	DIV(TS_CLK_TX5336_CHIP_OCLK3_pll0_d4_CLK, CHIP_OCLK3_CFG, 12, 5, 0, SYNC_INVALID, 0) \
	DIV(TS_CLK_TX5336_CHIP_OCLK3_pll1_d8_CLK, CHIP_OCLK3_CFG, 20, 5, 0, SYNC_INVALID, 0) \
	DIV(TS_CLK_TX5336_PDM0_M_CLK, PDM_CFG, 6, 9, 0, SYNC_INVALID, 0) \
	DIV(TS_CLK_TX5336_PDM1_M_CLK, PDM_CFG, 22, 9, 16, SYNC_INVALID, 0) \
	DIV(TS_CLK_TX5336_PDM2_M_CLK, PDM_CFG1, 6, 9, 0, SYNC_INVALID, 0) \
	DIV(TS_CLK_TX5336_PDM3_M_CLK, PDM_CFG1, 22, 9, 16, SYNC_INVALID, 0) \
	DIV(TS_CLK_TX5336_VPU_NOC_PLL0_CLK, VPU_CFG0, 12, 4, 0, SYNC_INVALID, 0) \
	DIV(TS_CLK_TX5336_VPU_NOC_PLL1_D2_CLK, VPU_CFG0, 8, 4, 0, SYNC_INVALID, 0) \
	DIV(TS_CLK_TX5336_VPU_CORE_CLK, VPU_CFG1, 8, 5, 0, SYNC_INVALID, 0) \
    DIV(TS_CLK_TX5336_I2C0_CCLK, I2C_CFG0, 8, 5, 0, SYNC_INVALID, 0) \
    DIV(TS_CLK_TX5336_I2C1_CCLK, I2C_CFG0, 24, 5, 16, SYNC_INVALID, 0) \
    DIV(TS_CLK_TX5336_I2C2_CCLK, I2C_CFG1, 8, 5, 0, SYNC_INVALID, 0) \
    DIV(TS_CLK_TX5336_I2C3_CCLK, I2C_CFG1, 24, 5, 16, SYNC_INVALID, 0) \
    DIV(TS_CLK_TX5336_I2C4_CCLK, I2C_CFG2, 8, 5, 0, SYNC_INVALID, 0) \
    DIV(TS_CLK_TX5336_I2C5_CCLK, I2C_CFG2, 24, 5, 16, SYNC_INVALID, 0) \
    DIV(TS_CLK_TX5336_I2C6_CCLK, I2C_CFG3, 8, 5, 0, SYNC_INVALID, 0) \

#define _DIVIFY(id) TS_CLK_DIV_##id
#define DIVIFY(id) _DIVIFY(id)

enum ts_div_ids {
#define DIV_FLAGS(id, ...) DIVIFY(id),
	DIV_LIST
#undef DIV_FLAGS
	TS_CLK_DIV_NONE,
};

static const struct ts_div_params ts_tx5336_divs[] = {
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

#define FRAC_DIV(id, off, shift, width, we, sync, flags, _mshift, _mwidth, _nshift, _nwidth) FRAC_DIV_FLAGS(id, off, shift, width, we, sync, flags, _mshift, _mwidth, _nshift, _nwidth)
#define FRAC_DIV_LIST \
	FRAC_DIV(TS_CLK_TX5336_UART0_CORE_CLK, UART_CFG0, 4, 31, 0, 0, 0, 4, 12, 16, 16) \
	FRAC_DIV(TS_CLK_TX5336_UART1_CORE_CLK, UART_CFG1, 4, 31, 0, 0, 0, 4, 12, 16, 16) \
	FRAC_DIV(TS_CLK_TX5336_UART2_CORE_CLK, UART_CFG2, 4, 31, 0, 0, 0, 4, 12, 16, 16) \
	FRAC_DIV(TS_CLK_TX5336_UART3_CORE_CLK, UART_CFG3, 4, 31, 0, 0, 0, 4, 12, 16, 16) \

#define _FRAC_DIVIFY(id) TS_CLK_FRAC_DIV_##id
#define FRAC_DIVIFY(id) _FRAC_DIVIFY(id)

enum ts_frac_div_ids {
#define FRAC_DIV_FLAGS(id, ...) FRAC_DIVIFY(id),
	FRAC_DIV_LIST
#undef FRAC_DIV_FLAGS
	TS_CLK_FRAC_DIV_NONE,
};

static const struct ts_frac_div_params ts_tx5336_frac_divs[] = {
#define FRAC_DIV_FLAGS(id, _off, _shift, _width, _we, _sync, _flags, _mshift, _mwidth, _nshift, _nwidth) \
	[FRAC_DIVIFY(id)] = { \
		.off = (_off), \
		.shift = (_shift), \
		.width = (_width), \
		.we = (_we), \
		.sync = (_sync), \
		.flags = (_flags), \
		.mshift = (_mshift), \
		.mwidth = (_mwidth), \
		.nshift = (_nshift), \
		.nwidth = (_nwidth), \
	},
	FRAC_DIV_LIST
#undef FRAC_DIV_FLAGS
};

#undef FRAC_DIV
#undef FRAC_DIV_LIST

//-----------------------------------------------2 gate----------------------------------------------------------------
#define GATE_LIST \
	GATE(TS_CLK_TX5336_PERI0_APB_CLK, PERIPH0_CFG0, 12, 0) \
	GATE(TS_CLK_TX5336_OTPC_APB_CLK, PERIPH0_CFG1, 16, 2) \
	GATE(TS_CLK_TX5336_GPADC_APB_CLK, PERIPH0_CFG1, 17, 3) \
	GATE(TS_CLK_TX5336_IOMUX_APB_CLK, PERIPH0_CFG1, 18, 4) \
	GATE(TS_CLK_TX5336_SYSREG_APB_CLK, PERIPH0_CFG1, 19, 5) \
	GATE(TS_CLK_TX5336_DAPLITE_APB_CLK, PERIPH0_CFG1, 20, 6) \
	GATE(TS_CLK_TX5336_GPIO_APB_CLK, PERIPH0_CFG1, 21, 7) \
	GATE(TS_CLK_TX5336_WDT_APB_CLK, PERIPH0_CFG1, 22, 8) \
	GATE(TS_CLK_TX5336_PERI1_APB_CLK, PERIPH1_CFG0, 12, 0) \
	GATE(TS_CLK_TX5336_PWM_APB_CLK, PERIPH1_CFG1, 8, 2) \
	GATE(TS_CLK_TX5336_USI0_APB_CLK, PERIPH1_CFG1, 9, 3) \
	GATE(TS_CLK_TX5336_USI1_APB_CLK, PERIPH1_CFG1, 10, 4) \
	GATE(TS_CLK_TX5336_PERI2_AHB_CLK, PERIPH2_CFG0, 12, 0) \
	GATE(TS_CLK_TX5336_HDMA0_AHB_CLK, PERIPH2_CFG1, 4, 2) \
	GATE(TS_CLK_TX5336_HDMA1_AHB_CLK, PERIPH2_CFG1, 5, 3) \
	GATE(TS_CLK_TX5336_QSPI_APB_CLK, QSPI_CFG, 2, 0) \
	GATE(TS_CLK_TX5336_QSPI_AHB_CLK, QSPI_CFG, 3, 0) \
	GATE(TS_CLK_TX5336_QSPI_REF_CLK, QSPI_CFG, 5, 0) \
	GATE(TS_CLK_TX5336_AUD_CODEC_AHB_CLK, AUD_AHB_CFG, 2, 0) \
	GATE(TS_CLK_TX5336_AUD_DAC_CCLK, AUD_DAC_CFG, 3, 0) \
	GATE(TS_CLK_TX5336_AUD_DAC_PBCLK, AUD_DAC_CFG, 2, 0) \
	GATE(TS_CLK_TX5336_AUD_ED_TMR_STRB_CLK, AUD_ED_TIMER_CFG, 2, 0) \
	GATE(TS_CLK_TX5336_AUD_ADC_CCLK, AUD_ADC_CFG, 2, 0) \
	GATE(TS_CLK_TX5336_SPI0_APB_CLK, SPI_CFG0, 4, 0) \
	GATE(TS_CLK_TX5336_SPI0_CORE_CLK, SPI_CFG0, 5, 0) \
	GATE(TS_CLK_TX5336_SPI1_APB_CLK, SPI_CFG0, 20, 16) \
	GATE(TS_CLK_TX5336_SPI1_CORE_CLK, SPI_CFG0, 21, 16) \
	GATE(TS_CLK_TX5336_SPI2_APB_CLK, SPI_CFG1, 4, 0) \
	GATE(TS_CLK_TX5336_SPI2_CORE_CLK, SPI_CFG0, 5, 0) \
	GATE(TS_CLK_TX5336_TIME0_APB_CLK, TIMER_CFG0, 2, 0) \
	GATE(TS_CLK_TX5336_TIME0_3_CLK, TIMER_CFG0, 3, 0) \
	GATE(TS_CLK_TX5336_TIME0_4_CLK, TIMER_CFG0, 4, 0) \
	GATE(TS_CLK_TX5336_TIME0_5_CLK, TIMER_CFG0, 5, 0) \
    GATE(TS_CLK_TX5336_TIME0_6_CLK, TIMER_CFG1, 3, 0) \
    GATE(TS_CLK_TX5336_TIME0_7_CLK, TIMER_CFG1, 4, 0) \
    GATE(TS_CLK_TX5336_TIME0_8_CLK, TIMER_CFG1, 5, 0) \
    GATE(TS_CLK_TX5336_TIME1_APB_CLK, TIMER_CFG2, 2, 0) \
	GATE(TS_CLK_TX5336_TIME1_3_CLK, TIMER_CFG2, 3, 0) \
	GATE(TS_CLK_TX5336_TIME1_4_CLK, TIMER_CFG2, 4, 0) \
	GATE(TS_CLK_TX5336_TIME1_5_CLK, TIMER_CFG2, 5, 0) \
    GATE(TS_CLK_TX5336_TIME1_6_CLK, TIMER_CFG3, 3, 0) \
    GATE(TS_CLK_TX5336_TIME1_7_CLK, TIMER_CFG3, 4, 0) \
    GATE(TS_CLK_TX5336_TIME1_8_CLK, TIMER_CFG3, 5, 0) \
	GATE(TS_CLK_TX5336_UART0_APB_CLK, UART_CFG0, 1, 0) \
	GATE(TS_CLK_TX5336_UART0_CORE_CLK, UART_CFG0, 2, 0) \
	GATE(TS_CLK_TX5336_UART1_APB_CLK, UART_CFG1, 1, 0) \
	GATE(TS_CLK_TX5336_UART1_CORE_CLK, UART_CFG1, 2, 0) \
	GATE(TS_CLK_TX5336_UART2_APB_CLK, UART_CFG2, 1, 0) \
	GATE(TS_CLK_TX5336_UART2_CORE_CLK, UART_CFG2, 2, 0) \
	GATE(TS_CLK_TX5336_UART3_APB_CLK, UART_CFG3, 1, 0) \
	GATE(TS_CLK_TX5336_UART3_CORE_CLK, UART_CFG3, 2, 0) \
	GATE(TS_CLK_TX5336_I2S_APB_CLK, I2S_CFG0, 2, 0) \
	GATE(TS_CLK_TX5336_I2S_O_CLK, I2S_CFG0, 3, 0) \
	GATE(TS_CLK_TX5336_I2S_M_CLK, I2S_CFG0, 4, 0) \
	GATE(TS_CLK_TX5336_I2S_O_CLK_O, I2S_CFG0, 7, 0) \
	GATE(TS_CLK_TX5336_SDHC0_AHB_CLK, SDHC_CFG0, 2, 0) \
	GATE(TS_CLK_TX5336_SDHC0_CCLK, SDHC_CFG0, 3, 0) \
	GATE(TS_CLK_TX5336_SDHC1_AHB_CLK, SDHC_CFG1, 2, 0) \
	GATE(TS_CLK_TX5336_SDHC1_CCLK, SDHC_CFG1, 3, 0) \
	GATE(TS_CLK_TX5336_USB_AHB_CLK, USB_CFG, 2, 0) \
	GATE(TS_CLK_TX5336_USB_REF_CLK, USB_CFG, 3, 0) \
	GATE(TS_CLK_TX5336_DDR_APB_CLK, UMCTL_CFG0, 2, 0) \
	GATE(TS_CLK_TX5336_UMCTL_P0_CLK, UMCTL_CFG1, 2, 0) \
	GATE(TS_CLK_TX5336_UMCTL_P1_CLK, UMCTL_CFG1, 18, 16) \
	GATE(TS_CLK_TX5336_UMCTL_P2_CLK, UMCTL_CFG2, 2, 0) \
	GATE(TS_CLK_TX5336_UMCTL_P3_CLK, UMCTL_CFG2, 18, 16) \
	GATE(TS_CLK_TX5336_UMCTL_P4_CLK, UMCTL_CFG3, 2, 0) \
	GATE(TS_CLK_TX5336_UMCTL_P5_CLK, UMCTL_CFG3, 18, 16) \
	GATE(TS_CLK_TX5336_RNE_AHB_CLK, RNE_CFG0, 2, 0) \
	GATE(TS_CLK_TX5336_RNE_AXI_CLK, RNE_CFG0, 3, 0) \
	GATE(TS_CLK_TX5336_RNE_CORE_CLK, RNE_CFG0, 5, 0) \
	GATE(TS_CLK_TX5336_RNE_CORE_PLL_CLK, RNE_CFG0, 4, 0) /* bit4 default enabled */ \
	GATE(TS_CLK_TX5336_MEMBUF_AXI_CLK, RNE_CFG1, 2, 0) \
	GATE(TS_CLK_TX5336_VI_AHB_CLK, VI_CFG, 2, 0) \
	GATE(TS_CLK_TX5336_VI_APB_CLK, VI_CFG, 3, 0) \
	GATE(TS_CLK_TX5336_VI_NOC_CLK, VI_CFG, 5, 0) \
	GATE(TS_CLK_TX5336_ISP_CORE_CLK, ISP_CFG, 2, 0) \
	GATE(TS_CLK_TX5336_ISP_AXI_CLK, ISP_CFG, 3, 0) \
	GATE(TS_CLK_TX5336_ISP_DRV_CLK, ISP_CFG, 19, 16) \
	GATE(TS_CLK_TX5336_VPE_AHB_CLK, VPE_CFG, 2, 0) \
	GATE(TS_CLK_TX5336_VPE_AXI_CLK, VPE_CFG, 3, 0) \
	GATE(TS_CLK_TX5336_VPE_ISP_CLK, VPE_CFG, 4, 0) \
	GATE(TS_CLK_TX5336_VPE_CORE_CLK, VPE_CFG, 5, 0) \
	GATE(TS_CLK_TX5336_DOF_APB_CLK, DOF_CFG, 2, 0) \
	GATE(TS_CLK_TX5336_DOF_CORE_CLK, DOF_CFG, 3, 0) \
	GATE(TS_CLK_TX5336_DOF_AXI_CLK, DOF_CFG, 4, 0) \
	GATE(TS_CLK_TX5336_EBD_AHB_CLK, EBD_CFG, 2, 0) \
	GATE(TS_CLK_TX5336_EBD_CORE_CLK, EBD_CFG, 3, 0) \
	GATE(TS_CLK_TX5336_EBD_AXI_CLK, EBD_CFG, 4, 0) \
	GATE(TS_CLK_TX5336_MIPI_CSI0_APB_CLK, MIPI_RX_CFG, 2, 0) \
	GATE(TS_CLK_TX5336_MIPI_CSI1_APB_CLK, MIPI_RX_CFG, 3, 0) \
	GATE(TS_CLK_TX5336_MIPI_CSI2_APB_CLK, MIPI_RX_CFG, 4, 0) \
	GATE(TS_CLK_TX5336_MIPI_CSI3_APB_CLK, MIPI_RX_CFG, 5, 0) \
	GATE(TS_CLK_TX5336_MIPI_RXPHY0_APB_CLK, MIPI_RX_CFG, 6, 0) \
	GATE(TS_CLK_TX5336_MIPI_RXPHY1_APB_CLK, MIPI_RX_CFG, 7, 0) \
	GATE(TS_CLK_TX5336_MIPI_CSI0_IPI0_CLK, MIPI_RX_CFG, 8, 0) \
	GATE(TS_CLK_TX5336_MIPI_CSI0_IPI1_CLK, MIPI_RX_CFG, 9, 0) \
	GATE(TS_CLK_TX5336_MIPI_CSI1_IPI_CLK, MIPI_RX_CFG, 10, 0) \
	GATE(TS_CLK_TX5336_MIPI_CSI2_IPI0_CLK, MIPI_RX_CFG, 11, 0) \
	GATE(TS_CLK_TX5336_MIPI_CSI2_IPI1_CLK, MIPI_RX_CFG, 12, 0) \
	GATE(TS_CLK_TX5336_MIPI_CSI3_IPI_CLK, MIPI_RX_CFG, 13, 0) \
	GATE(TS_CLK_TX5336_MIPI_TXCLKESC_CLK, MIPI_RX_CFG, 14, 0) \
	GATE(TS_CLK_TX5336_CV_AHB_CLK, CV_CFG, 2, 0) \
	GATE(TS_CLK_TX5336_CV_APB_CLK, CV_CFG, 4, 0) \
	GATE(TS_CLK_TX5336_CV_NOC_CLK, CV_CFG, 13, 0) \
	GATE(TS_CLK_TX5336_HASH_CLK, CRYPTO_CFG, 2, 0) \
	GATE(TS_CLK_TX5336_SKE_CLK, CRYPTO_CFG, 3, 0) \
	GATE(TS_CLK_TX5336_SPACC_CLK, CRYPTO_CFG, 4, 0) \
	GATE(TS_CLK_TX5336_PKE_CLK, CRYPTO_CFG, 5, 0) \
	GATE(TS_CLK_TX5336_TRNG_CLK, CRYPTO_CFG, 6, 0) \
	GATE(TS_CLK_TX5336_AMR_AHB_CLK, AMR_CFG, 2, 0) \
	GATE(TS_CLK_TX5336_AMR_AXI_CLK, AMR_CFG, 3, 0) \
	GATE(TS_CLK_TX5336_AMR_CORE_CLK, AMR_CFG, 4, 0) \
	GATE(TS_CLK_TX5336_CVE_AHB_CLK, CVE_CFG, 2, 0) \
	GATE(TS_CLK_TX5336_CVE_AXI_CLK, CVE_CFG, 3, 0) \
	GATE(TS_CLK_TX5336_CVE_CORE_CLK, CVE_CFG, 4, 0) \
	GATE(TS_CLK_TX5336_BIS_APB_CLK, BIS_CFG, 2, 0) \
	GATE(TS_CLK_TX5336_BIS_AXI_CLK, BIS_CFG, 3, 0) \
	GATE(TS_CLK_TX5336_BIS_CORE_CLK, BIS_CFG, 4, 0) \
	GATE(TS_CLK_TX5336_GDC_APB_CLK, GDC_CFG, 2, 0) \
	GATE(TS_CLK_TX5336_GDC_AXI_CLK, GDC_CFG, 3, 0) \
	GATE(TS_CLK_TX5336_GDC_CORE_CLK, GDC_CFG, 4, 0) \
	GATE(TS_CLK_TX5336_DPU_AHB_CLK, DPU_CFG, 2, 0) \
	GATE(TS_CLK_TX5336_DPU_AXI_CLK, DPU_CFG, 3, 0) \
	GATE(TS_CLK_TX5336_DPU_CORE_CLK, DPU_CFG, 4, 0) \
	GATE(TS_CLK_TX5336_MIPI_DSI_APB_CLK, MIPI_TX_CFG, 2, 0) \
	GATE(TS_CLK_TX5336_MIPI_TXPHY_APB_CLK, MIPI_TX_CFG, 3, 0) \
	GATE(TS_CLK_TX5336_MIPI_REF_CLK, MIPI_TX_CFG, 4, 0) \
	GATE(TS_CLK_TX5336_ADMA_AHB_CLK, ADMA_CFG, 2, 0) \
	GATE(TS_CLK_TX5336_ADMA_AXI_CLK, ADMA_CFG, 3, 0) \
	GATE(TS_CLK_TX5336_CHIP_OCLK0_CLK, CHIP_OCLK0_CFG, 2, 0) \
	GATE(TS_CLK_TX5336_CHIP_OCLK1_CLK, CHIP_OCLK1_CFG, 2, 0) \
	GATE(TS_CLK_TX5336_CHIP_OCLK2_CLK, CHIP_OCLK2_CFG, 2, 0) \
	GATE(TS_CLK_TX5336_CHIP_OCLK3_CLK, CHIP_OCLK3_CFG, 2, 0) \
	GATE(TS_CLK_TX5336_PDM0_APB_CLK, PDM_CFG, 2, 0) \
	GATE(TS_CLK_TX5336_PDM0_M_CLK, PDM_CFG, 3, 0) \
	GATE(TS_CLK_TX5336_PDM1_APB_CLK, PDM_CFG, 18, 16) \
	GATE(TS_CLK_TX5336_PDM1_M_CLK, PDM_CFG, 19, 16) \
	GATE(TS_CLK_TX5336_PDM2_APB_CLK, PDM_CFG1, 2, 0) \
	GATE(TS_CLK_TX5336_PDM2_M_CLK, PDM_CFG1, 3, 0) \
	GATE(TS_CLK_TX5336_PDM3_APB_CLK, PDM_CFG1, 18, 16) \
	GATE(TS_CLK_TX5336_PDM3_M_CLK, PDM_CFG1, 19, 16) \
	GATE(TS_CLK_TX5336_VPU_NOC_CLK, VPU_CFG0, 5, 0) \
	GATE(TS_CLK_TX5336_VPU_CORE_CLK, VPU_CFG1, 2, 0) \
    GATE(TS_CLK_TX5336_I2C0_APB_CLK, I2C_CFG0, 2, 0) \
    GATE(TS_CLK_TX5336_I2C0_CCLK, I2C_CFG0, 3, 0) \
    GATE(TS_CLK_TX5336_I2C1_APB_CLK, I2C_CFG0, 18, 16) \
    GATE(TS_CLK_TX5336_I2C1_CCLK, I2C_CFG0, 19, 16) \
    GATE(TS_CLK_TX5336_I2C2_APB_CLK, I2C_CFG1, 2, 0) \
    GATE(TS_CLK_TX5336_I2C2_CCLK, I2C_CFG1, 3, 0) \
    GATE(TS_CLK_TX5336_I2C3_APB_CLK, I2C_CFG1, 18, 16) \
    GATE(TS_CLK_TX5336_I2C3_CCLK, I2C_CFG1, 19, 16) \
    GATE(TS_CLK_TX5336_I2C4_APB_CLK, I2C_CFG2, 2, 0) \
    GATE(TS_CLK_TX5336_I2C4_CCLK, I2C_CFG2, 3, 0) \
    GATE(TS_CLK_TX5336_I2C5_APB_CLK, I2C_CFG2, 18, 16) \
    GATE(TS_CLK_TX5336_I2C5_CCLK, I2C_CFG2, 19, 16) \
    GATE(TS_CLK_TX5336_I2C6_APB_CLK, I2C_CFG3, 2, 0) \
    GATE(TS_CLK_TX5336_I2C6_CCLK, I2C_CFG3, 3, 0) \


#define _GATEIFY(id) TS_CLK_GATE_##id
#define GATEIFY(id) _GATEIFY(id)

enum ts_gate_ids {
#define GATE(id, ...) GATEIFY(id),
	GATE_LIST
#undef GATE
	TS_CLK_GATE_NONE,
};

static const struct ts_gate_params ts_tx5336_gates[] = {
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

//-----------------------------------------------3 mux----------------------------------------------------------------
#define MUX(id, reg, shift, width, we, sync) \
	MUX_PARENTS(id, generic_sels, reg, shift, width, we, sync)
#define MUX_LIST \
	MUX_PARENTS(TS_CLK_TX5336_PLL_MUX, pll_mux_sels, CPU_CFG1, 4, 1, 0, SYNC_INVALID) \
	MUX_PARENTS(TS_CLK_TX5336_CPU_CCLK, cpu_cclk_sels, CPU_CFG1, 5, 1, 0, SYNC_INVALID) \
	MUX_PARENTS(TS_CLK_TX5336_GIC_CLK, gic_clk_sels, CPU_CFG0, 19, 1, 16, SYNC_INVALID) \
	MUX_PARENTS(TS_CLK_TX5336_PLL_TOPNOC_CLK, pll_mux_topnoc_clk_sels, TOP_CLK_CFG, 4, 1, 0, SYNC_INVALID) \
	MUX_PARENTS(TS_CLK_TX5336_TOPNOC_CLK, topnoc_clk_sels, TOP_CLK_CFG, 5, 1, 0, SYNC_INVALID) \
	MUX_PARENTS(TS_CLK_TX5336_PERI0_APB_CLK, peri_apb_clk_sels, PERIPH0_CFG0, 4, 1, 0, SYNC_INVALID) \
	MUX_PARENTS(TS_CLK_TX5336_PERI1_APB_CLK, peri_apb_clk_sels, PERIPH1_CFG0, 4, 1, 0, SYNC_INVALID) \
	MUX_PARENTS(TS_CLK_TX5336_PERI2_AHB_CLK, peri_apb_clk_sels, PERIPH2_CFG0, 4, 1, 0, SYNC_INVALID) \
	MUX_PARENTS(TS_CLK_TX5336_QSPI_AHB_CLK, qspi_ahb_clk_sels, QSPI_CFG, 4, 1, 0, SYNC_INVALID) \
	MUX_PARENTS(TS_CLK_TX5336_QSPI_REF_CLK, qspi_ref_clk_sels, QSPI_CFG, 6, 1, 0, SYNC_INVALID) \
	MUX_PARENTS(TS_CLK_TX5336_SPI0_CORE_CLK, spi_core_clk_sels, SPI_CFG0, 6, 1, 0, SYNC_INVALID) \
	MUX_PARENTS(TS_CLK_TX5336_SPI1_CORE_CLK, spi_core_clk_sels, SPI_CFG0, 22, 1, 16, SYNC_INVALID) \
	MUX_PARENTS(TS_CLK_TX5336_SPI2_CORE_CLK, spi_core_clk_sels, SPI_CFG1, 6, 1, 0, SYNC_INVALID) \
	MUX_PARENTS(TS_CLK_TX5336_UART0_CORE_CLK, uart_core_clk_sels, UART_CFG0, 3, 1, 0, SYNC_INVALID) \
	MUX_PARENTS(TS_CLK_TX5336_UART1_CORE_CLK, uart_core_clk_sels, UART_CFG1, 3, 1, 0, SYNC_INVALID) \
	MUX_PARENTS(TS_CLK_TX5336_UART2_CORE_CLK, uart_core_clk_sels, UART_CFG2, 3, 1, 0, SYNC_INVALID) \
	MUX_PARENTS(TS_CLK_TX5336_UART3_CORE_CLK, uart_core_clk_sels, UART_CFG3, 3, 1, 0, SYNC_INVALID) \
	MUX_PARENTS(TS_CLK_TX5336_SDHC0_CCLK, sdhc_cclk_sels, SDHC_CFG0, 5, 1, 0, SYNC_INVALID) \
	MUX_PARENTS(TS_CLK_TX5336_SDHC1_CCLK, sdhc_cclk_sels, SDHC_CFG1, 5, 1, 0, SYNC_INVALID) \
	MUX_PARENTS(TS_CLK_TX5336_USB_REF_CLK, usb_ref_clk_sels, USB_CFG, 4, 1, 0, SYNC_INVALID) \
	MUX_PARENTS(TS_CLK_TX5336_UMCTL_P1_CLK, umctl_p1_clk_sels, UMCTL_CFG1, 19, 1, 16, SYNC_INVALID) \
	MUX_PARENTS(TS_CLK_TX5336_UMCTL_P2_CLK, umctl_p2_clk_sels, UMCTL_CFG2, 3, 1, 0, SYNC_INVALID) \
	MUX_PARENTS(TS_CLK_TX5336_UMCTL_P4_CLK, umctl_p4_clk_sels, UMCTL_CFG3, 3, 1, 0, SYNC_INVALID) \
	MUX_PARENTS(TS_CLK_TX5336_RNE_CORE_PLL_CLK, pll01_mux_rne_core_clk_sels, RNE_CFG0, 6, 1, 0, SYNC_INVALID) \
	MUX_PARENTS(TS_CLK_TX5336_RNE_CORE_CLK, rne_core_clk_sels, RNE_CFG0, 7, 1, 0, SYNC_INVALID) \
	MUX_PARENTS(TS_CLK_TX5336_MEMBUF_AXI_CLK, membuf_axi_clk_sels, RNE_CFG1, 3, 1, 0, SYNC_INVALID) \
	MUX_PARENTS(TS_CLK_TX5336_VI_NOC_CLK, vi_noc_clk_sels, VI_CFG, 5, 1, 0, SYNC_INVALID) \
	MUX_PARENTS(TS_CLK_TX5336_ISP_AXI_CLK, isp_axi_clk_sels, ISP_CFG, 4, 1, 0, SYNC_INVALID) \
	MUX_PARENTS(TS_CLK_TX5336_ISP_DRV_CLK, isp_drv_clk_sels, ISP_CFG, 18, 1, 16, SYNC_INVALID) \
	MUX_PARENTS(TS_CLK_TX5336_VPE_AXI_CLK, vpe_axi_clk_sels, VPE_CFG, 6, 1, 0, SYNC_INVALID) \
	MUX_PARENTS(TS_CLK_TX5336_VPE_CORE_CLK, vpe_core_clk_sels, VPE_CFG, 7, 1, 0, SYNC_INVALID) \
	MUX_PARENTS(TS_CLK_TX5336_DOF_AXI_CLK, dof_axi_clk_sels, DOF_CFG, 5, 1, 0, SYNC_INVALID) \
	MUX_PARENTS(TS_CLK_TX5336_CV_AHB_CLK, cv_clk_sels, CV_CFG, 3, 1, 0, SYNC_INVALID) \
	MUX_PARENTS(TS_CLK_TX5336_CV_NOC_CLK, cv_clk_sels, CV_CFG, 7, 1, 0, SYNC_INVALID) \
	MUX_PARENTS(TS_CLK_TX5336_BIS_CORE_CLK, bis_core_clk_sels, BIS_CFG, 6, 1, 0, SYNC_INVALID) \
	MUX_PARENTS(TS_CLK_TX5336_GDC_CORE_CLK, gdc_core_clk_sels, GDC_CFG, 6, 1, 0, SYNC_INVALID) \
	MUX_PARENTS(TS_CLK_TX5336_MIPI_REF_CLK, mipitx_refclk_sels, MIPI_TX_CFG, 5, 1, 0, SYNC_INVALID) \
	MUX_PARENTS(TS_CLK_TX5336_CHIP_OCLK0_CLK, chip_oclk_sels, CHIP_OCLK0_CFG, 3, 2, 0, SYNC_INVALID) \
	MUX_PARENTS(TS_CLK_TX5336_CHIP_OCLK1_CLK, chip_oclk1_sels, CHIP_OCLK1_CFG, 3, 2, 0, SYNC_INVALID) \
	MUX_PARENTS(TS_CLK_TX5336_CHIP_OCLK2_CLK, chip_oclk2_sels, CHIP_OCLK2_CFG, 3, 2, 0, SYNC_INVALID) \
	MUX_PARENTS(TS_CLK_TX5336_CHIP_OCLK3_CLK, chip_oclk3_sels, CHIP_OCLK3_CFG, 3, 2, 0, SYNC_INVALID) \
	MUX_PARENTS(TS_CLK_TX5336_VPU_NOC_CLK, vpu_noc_clk_sels, VPU_CFG0, 4, 1, 0, SYNC_INVALID) \
	MUX_PARENTS(TS_CLK_TX5336_I2C0_CCLK, i2c_core_clk_sels, I2C_CFG0, 4, 1, 0, SYNC_INVALID) \
	MUX_PARENTS(TS_CLK_TX5336_I2C1_CCLK, i2c_core_clk_sels, I2C_CFG0, 20, 1, 16, SYNC_INVALID) \
	MUX_PARENTS(TS_CLK_TX5336_I2C2_CCLK, i2c_core_clk_sels, I2C_CFG1, 4, 1, 0, SYNC_INVALID) \
	MUX_PARENTS(TS_CLK_TX5336_I2C3_CCLK, i2c_core_clk_sels, I2C_CFG1, 20, 1, 16, SYNC_INVALID) \
	MUX_PARENTS(TS_CLK_TX5336_I2C4_CCLK, i2c_core_clk_sels, I2C_CFG2, 4, 1, 0, SYNC_INVALID) \
	MUX_PARENTS(TS_CLK_TX5336_I2C5_CCLK, i2c_core_clk_sels, I2C_CFG2, 20, 1, 16, SYNC_INVALID) \
	MUX_PARENTS(TS_CLK_TX5336_I2C6_CCLK, i2c_core_clk_sels, I2C_CFG3, 4, 1, 0, SYNC_INVALID) \

#define _MUXIFY(id) TS_CLK_MUX_##id
#define MUXIFY(id) _MUXIFY(id)

enum ts_mux_ids {
#define MUX_PARENTS(id, ...) MUXIFY(id),
	MUX_LIST
#undef MUX_PARENTS
	TS_CLK_MUX_NONE,
};

static const struct ts_mux_params ts_tx5336_muxes[] = {
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

//-----------------------------------------------4 pll----------------------------------------------------------------
static struct ts_pll_params ts_tx5336_plls[] = {
#define PLL(_name, _id, _off, _shift, _width) { \
	.name = _name, \
	.id = _id, \
	.off = (_off), \
	.lock_off = PLL_STATUS, \
	.shift = (_shift), \
	.width = (_width), \
}
	[0] = PLL("pll0", TS_CLK_TX5336_PLL0, PLL0_CFG0, 0, 1),
	[1] = PLL("pll1", TS_CLK_TX5336_PLL1, PLL1_CFG0, 1, 1),
	[2] = PLL("pll2", TS_CLK_TX5336_PLL2, PLL2_CFG0, 2, 1),
	[3] = PLL("pll3", TS_CLK_TX5336_PLL3, PLL3_CFG0, 3, 1),
#undef PLL
};

//-----------------------------------------------5 comp----------------------------------------------------------------
#define COMP(id) \
	COMP_FULL(id, MUXIFY(id), DIVIFY(id), GATEIFY(id))
#define COMP_FRAC_DIV(id) \
	COMP_FULL(id, MUXIFY(id), FRAC_DIVIFY(id), GATEIFY(id))
#define COMP_NOMUX(id) \
	COMP_FULL(id, TS_CLK_MUX_NONE, DIVIFY(id), GATEIFY(id))
#define COMP_NOGATE(id) \
	COMP_FULL(id, MUXIFY(id), DIVIFY(id), TS_CLK_GATE_NONE)
#define COMP_NODIV(id) \
	COMP_FULL(id, MUXIFY(id), TS_CLK_DIV_NONE, GATEIFY(id))
#define COMP_NODIV_NOGATE(id) \
	COMP_FULL(id, MUXIFY(id), TS_CLK_DIV_NONE, TS_CLK_GATE_NONE)
#define COMP_GATE_ONLY(id) \
	COMP_FULL(id, TS_CLK_MUX_NONE, TS_CLK_DIV_NONE, GATEIFY(id))

#define COMP_LIST \
    COMP_NODIV_NOGATE(TS_CLK_TX5336_PLL_MUX) \
    COMP_NODIV_NOGATE(TS_CLK_TX5336_CPU_CCLK) \
    COMP_NODIV_NOGATE(TS_CLK_TX5336_PLL_TOPNOC_CLK) \
    COMP_NODIV_NOGATE(TS_CLK_TX5336_TOPNOC_CLK) \
    COMP_NOGATE(TS_CLK_TX5336_GIC_CLK) \
    COMP(TS_CLK_TX5336_PERI0_APB_CLK) \
    COMP(TS_CLK_TX5336_PERI1_APB_CLK) \
    COMP(TS_CLK_TX5336_PERI2_AHB_CLK) \
    COMP(TS_CLK_TX5336_QSPI_AHB_CLK) \
    COMP(TS_CLK_TX5336_QSPI_REF_CLK) \
    COMP_NOMUX(TS_CLK_TX5336_AUD_CODEC_AHB_CLK) \
    COMP_NOMUX(TS_CLK_TX5336_AUD_DAC_CCLK) \
    COMP_NOMUX(TS_CLK_TX5336_AUD_DAC_PBCLK) \
    COMP_NOMUX(TS_CLK_TX5336_AUD_ED_TMR_STRB_CLK) \
    COMP_NOMUX(TS_CLK_TX5336_AUD_ADC_CCLK) \
    COMP(TS_CLK_TX5336_SPI0_CORE_CLK) \
    COMP(TS_CLK_TX5336_SPI1_CORE_CLK) \
    COMP(TS_CLK_TX5336_SPI2_CORE_CLK) \
    COMP_FRAC_DIV(TS_CLK_TX5336_UART0_CORE_CLK) \
    COMP_FRAC_DIV(TS_CLK_TX5336_UART1_CORE_CLK) \
    COMP_FRAC_DIV(TS_CLK_TX5336_UART2_CORE_CLK) \
    COMP_FRAC_DIV(TS_CLK_TX5336_UART3_CORE_CLK) \
    COMP_NOMUX(TS_CLK_TX5336_I2S_O_CLK) \
    COMP_NOMUX(TS_CLK_TX5336_I2S_M_CLK) \
    COMP(TS_CLK_TX5336_SDHC0_CCLK) \
    COMP(TS_CLK_TX5336_SDHC1_CCLK) \
    COMP_NOMUX(TS_CLK_TX5336_DDR_APB_CLK) \
    COMP_NODIV(TS_CLK_TX5336_USB_REF_CLK) \
    COMP_NOMUX(TS_CLK_TX5336_UMCTL_P0_CLK) \
    COMP_NODIV(TS_CLK_TX5336_UMCTL_P1_CLK) \
    COMP_NODIV(TS_CLK_TX5336_UMCTL_P2_CLK) \
    COMP_NOMUX(TS_CLK_TX5336_UMCTL_P3_CLK) \
    COMP_NODIV(TS_CLK_TX5336_UMCTL_P4_CLK) \
    COMP_NOMUX(TS_CLK_TX5336_UMCTL_P5_CLK) \
    COMP_NOMUX(TS_CLK_TX5336_RNE_AHB_CLK) \
    COMP_NOMUX(TS_CLK_TX5336_RNE_AXI_CLK) \
    COMP_NODIV(TS_CLK_TX5336_RNE_CORE_PLL_CLK) \
    COMP_NODIV(TS_CLK_TX5336_RNE_CORE_CLK) \
    COMP(TS_CLK_TX5336_MEMBUF_AXI_CLK) \
    COMP_NOMUX(TS_CLK_TX5336_VI_AHB_CLK) \
    COMP_NOMUX(TS_CLK_TX5336_VI_APB_CLK) \
    COMP_NODIV(TS_CLK_TX5336_VI_NOC_CLK) \
    COMP_NODIV(TS_CLK_TX5336_ISP_AXI_CLK) \
    COMP_NODIV(TS_CLK_TX5336_ISP_DRV_CLK) \
    COMP_NODIV(TS_CLK_TX5336_VPE_AXI_CLK) \
    COMP_NODIV(TS_CLK_TX5336_VPE_CORE_CLK) \
    COMP_NODIV(TS_CLK_TX5336_DOF_AXI_CLK) \
    COMP_NOMUX(TS_CLK_TX5336_EBD_AXI_CLK) \
    COMP_NOMUX(TS_CLK_TX5336_MIPI_TXCLKESC_CLK) \
    COMP(TS_CLK_TX5336_CV_AHB_CLK) \
    COMP_NOMUX(TS_CLK_TX5336_CV_APB_CLK) \
    COMP(TS_CLK_TX5336_CV_NOC_CLK) \
    COMP_NOMUX(TS_CLK_TX5336_AMR_AXI_CLK) \
    COMP_NOMUX(TS_CLK_TX5336_AMR_CORE_CLK) \
    COMP_NOMUX(TS_CLK_TX5336_CVE_AXI_CLK) \
    COMP_NOMUX(TS_CLK_TX5336_CVE_CORE_CLK) \
    COMP_NOMUX(TS_CLK_TX5336_BIS_AXI_CLK) \
    COMP_NODIV(TS_CLK_TX5336_BIS_CORE_CLK) \
    COMP_NOMUX(TS_CLK_TX5336_GDC_AXI_CLK) \
    COMP_NODIV(TS_CLK_TX5336_GDC_CORE_CLK) \
    COMP_NOMUX(TS_CLK_TX5336_DPU_AXI_CLK) \
    COMP_NOMUX(TS_CLK_TX5336_DPU_CORE_CLK) \
    COMP_NODIV(TS_CLK_TX5336_MIPI_REF_CLK) \
    COMP_NOMUX(TS_CLK_TX5336_ADMA_AXI_CLK) \
    COMP_NODIV(TS_CLK_TX5336_CHIP_OCLK0_CLK)\
    COMP_NODIV(TS_CLK_TX5336_CHIP_OCLK1_CLK)\
    COMP_NODIV(TS_CLK_TX5336_CHIP_OCLK2_CLK)\
    COMP_NODIV(TS_CLK_TX5336_CHIP_OCLK3_CLK)\
    COMP_NOMUX(TS_CLK_TX5336_PDM0_M_CLK)\
    COMP_NOMUX(TS_CLK_TX5336_PDM1_M_CLK)\
    COMP_NOMUX(TS_CLK_TX5336_PDM2_M_CLK)\
    COMP_NOMUX(TS_CLK_TX5336_PDM3_M_CLK)\
    COMP_NODIV(TS_CLK_TX5336_VPU_NOC_CLK)\
    COMP_NOMUX(TS_CLK_TX5336_VPU_CORE_CLK)\
    COMP_NOMUX(TS_CLK_TX5336_TIME0_8_CLK) \
    COMP_NOMUX(TS_CLK_TX5336_TIME0_7_CLK) \
    COMP_NOMUX(TS_CLK_TX5336_TIME0_6_CLK) \
    COMP_NOMUX(TS_CLK_TX5336_TIME0_5_CLK) \
    COMP_NOMUX(TS_CLK_TX5336_TIME1_8_CLK) \
    COMP_NOMUX(TS_CLK_TX5336_TIME1_7_CLK) \
    COMP_NOMUX(TS_CLK_TX5336_TIME1_6_CLK) \
    COMP_NOMUX(TS_CLK_TX5336_TIME1_5_CLK) \
    COMP_GATE_ONLY(TS_CLK_TX5336_I2C0_APB_CLK) \
    COMP(TS_CLK_TX5336_I2C0_CCLK) \
    COMP_GATE_ONLY(TS_CLK_TX5336_I2C1_APB_CLK) \
    COMP(TS_CLK_TX5336_I2C1_CCLK) \
    COMP_GATE_ONLY(TS_CLK_TX5336_I2C2_APB_CLK) \
    COMP(TS_CLK_TX5336_I2C2_CCLK) \
    COMP_GATE_ONLY(TS_CLK_TX5336_I2C3_APB_CLK) \
    COMP(TS_CLK_TX5336_I2C3_CCLK) \
    COMP_GATE_ONLY(TS_CLK_TX5336_I2C4_APB_CLK) \
    COMP(TS_CLK_TX5336_I2C4_CCLK) \
    COMP_GATE_ONLY(TS_CLK_TX5336_I2C5_APB_CLK) \
    COMP(TS_CLK_TX5336_I2C5_CCLK) \
    COMP_GATE_ONLY(TS_CLK_TX5336_I2C6_APB_CLK) \
    COMP(TS_CLK_TX5336_I2C6_CCLK) \

#define _COMPIFY(id) TS_CLK_COMP_##id
#define COMPIFY(id) _COMPIFY(id)

enum ts_comp_ids {
#define COMP_FULL(id, ...) COMPIFY(id),
	COMP_LIST
#undef COMP_FULL
};

static const struct ts_comp_params ts_tx5336_comps[] = {
#define COMP_FULL(id, _mux, _div, _gate) [COMPIFY(id)] = { .mux = (_mux), .div = (_div), .gate = (_gate), },
	COMP_LIST
#undef COMP_FULL
};

#undef COMP
#undef COMP_ID
#undef COMP_NOMUX
#undef COMP_NOMUX_ID
#undef COMP_LIST

static char *frac_div_arr[] = {"uart0_core_clk", "uart1_core_clk", "uart2_core_clk", "uart3_core_clk"};
static int clk_tx5336_is_frac_divider(const char *name)
{
	u32 i;

	for (i = 0; i < ARRAY_SIZE(frac_div_arr); i++) {
		if (!strncmp(frac_div_arr[i], name, strlen(frac_div_arr[i])))
			return true;
	}
	return false;
}

/* Create all sub-clocks, and then register the composite clock */
struct clk *clk_tx5336_register_comp(const struct ts_comp_params *params,
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
	struct clk_hw *hw = NULL;
	u32 div_id;
	unsigned long flags = 0;

	// 1 mux
	if (params->mux == TS_CLK_MUX_NONE) {
		if (!parent) {
			pr_err("MUX_NONE with parent not NULL\n");
			return ERR_PTR(-EINVAL);
		}

		mux_ops = NULL;
		mux = NULL;
		parent_names = &parent;
		num_parents = 1;
	} else {
		mux_ops = &clk_mux_ops;
		mux = ts_create_mux(&ts_tx5336_muxes[params->mux], base);
		if (!mux) {
			pr_err("create clk %s mux %d failed\n",
				name, params->mux);
			return ERR_PTR(-ENOMEM);
		}

		parent_names = ts_tx5336_muxes[params->mux].parent_names;
		num_parents = ts_tx5336_muxes[params->mux].num_parents;
	}
	// 2 div
	if (params->div == TS_CLK_DIV_NONE) {
		div_ops = NULL;
		div = NULL;
		if ((params->mux == MUXIFY(TS_CLK_TX5336_CHIP_OCLK0_CLK))
			|| (params->mux == MUXIFY(TS_CLK_TX5336_CHIP_OCLK1_CLK))
			|| (params->mux == MUXIFY(TS_CLK_TX5336_CHIP_OCLK2_CLK))
			|| (params->mux == MUXIFY(TS_CLK_TX5336_CHIP_OCLK3_CLK))
			|| (params->mux == MUXIFY(TS_CLK_TX5336_ISP_AXI_CLK))
			|| (params->mux == MUXIFY(TS_CLK_TX5336_ISP_DRV_CLK))
			|| (params->mux == MUXIFY(TS_CLK_TX5336_VI_NOC_CLK))
			|| (params->mux == MUXIFY(TS_CLK_TX5336_DOF_AXI_CLK))
            || (params->mux == MUXIFY(TS_CLK_TX5336_VPU_NOC_CLK))
            || (params->mux == MUXIFY(TS_CLK_TX5336_CPU_CCLK))
            || (params->mux == MUXIFY(TS_CLK_TX5336_PLL_MUX))
            ) {
			flags |= CLK_SET_RATE_PARENT;
		}
	} else {
		div_id = params->div;
		if (clk_tx5336_is_frac_divider(name)) {
			pr_debug("----div_id %d frac div\n----", div_id);
			div_ops = &clk_fractional_divider_ops;
			div = (struct clk_fractional_divider *)ts_create_fractional_div(&ts_tx5336_frac_divs[div_id], base);
			hw = &((struct clk_fractional_divider *)div)->hw;
		} else {
			div_ops = &clk_divider_ops;
			div = (struct clk_divider *)ts_create_div(&ts_tx5336_divs[div_id], base);
			hw = &((struct clk_divider *)div)->hw;
		}

		if (!div) {
			pr_err("create clk %s div %d failed\n",	name, div_id);
			comp = ERR_PTR(-ENOMEM);
			goto cleanup_mux;
		}
	}
	// 3 gate
	if (params->gate == TS_CLK_GATE_NONE) {
		gate_ops = NULL;
		gate = NULL;
	} else {
		gate_ops = &clk_gate_ops;
		gate = ts_create_gate(&ts_tx5336_gates[params->gate], base);
		if (!gate) {
			pr_err("create clk %s gate %d failed\n", name, params->gate);
			comp = ERR_PTR(-ENOMEM);
			goto cleanup_div;
		}
	}

	comp = clk_register_composite(NULL, name, parent_names, num_parents,
					(mux ? &mux->hw : NULL), mux_ops,
					(div ? hw : NULL), div_ops,
					(gate ? &gate->hw : NULL), gate_ops, flags);
	if (IS_ERR_OR_NULL(comp)) {
		pr_err("register composite %s failed\n", name);
		goto cleanup_gate;
	}
	return comp;

cleanup_gate:
	kfree(gate);
cleanup_div:
	kfree(div);
cleanup_mux:
	kfree(mux);
	return comp;
}


static struct clk *tx_tx5336_clks[TS_CLK_TX5336_MAX];

static inline void clk_dm(ulong id, const char *name, struct clk *clk)
{
	int ret;

	if (id >= TS_CLK_TX5336_MAX) {
		pr_err("clk_register_clkdev %s failed\n", name);
		return;
	}

	if (IS_ERR_OR_NULL(clk)) {
		//pr_err("%s %lu failed\n", __func__, id);
		return;
	}

	ret = clk_register_clkdev(clk, name, NULL);
	if (ret) {
		pr_err("clk_register_clkdev %s failed\n", name);
		return;
	}

	tx_tx5336_clks[id] = clk;
}
// 3 register comps without mux
static int clk_tx5336_register_phases(struct ts_clk_params *paras)
{
    void *top_base = paras->top_base;

#define REGISTER_SHIFT(clk_id, clk_name, clk_parent_name) do {\
        struct clk_init_data init;  \
        struct ts_mmc_clock *mmc_clock; \
		const struct ts_shift_params *params = &ts_tx5336_shifts[SHIFTIFY(clk_id)]; \
        mmc_clock = kmalloc(sizeof(*mmc_clock), GFP_KERNEL);    \
		if (!mmc_clock) \
            return -ENOMEM; \
        init.name = (const char *)clk_name;   \
        init.flags = params->sync; \
        init.num_parents = 1;   \
        init.parent_names = clk_parent_name; \
        init.ops = &ts_mmc_clk_ops; \
        mmc_clock->id = clk_id; \
		mmc_clock->hw.init = &init; \
		mmc_clock->reg = top_base + (params->off);   \
        mmc_clock->shift = params->shift;   \
        mmc_clock->we = params->we;   \
        mmc_clock->width = params->width; \
		clk_dm(clk_id, clk_name, \
			clk_register(NULL, &mmc_clock->hw)); \
	} while (false)

	REGISTER_SHIFT(TS_CLK_TX5336_SDHC0_SMPL_CLK, "sdhc0_smpl_clk", (const char* const []){"sdhc0_cclk"});

#undef REGISTER_SHIFT
    return 0;
}

// 1 register pll ok
static int clk_tx5336_register_plls(struct ts_clk_params *paras)
{
	void *top_base = paras->top_base;
	void __iomem *reg;
	void __iomem *lock;
	int i;
	struct ts_pll_params *params;
	struct clk *pclk;

	for (i = 0; i < ARRAY_SIZE(ts_tx5336_plls); i++) {
		params = &ts_tx5336_plls[i];
		reg = top_base + params->off;
		lock = top_base + params->lock_off;

		clk_dm(params->id, params->name,
			ts_register_pll(params->name, &osc_in[0], reg, lock,
				params->shift, params->width, PLL_REG_MODE_COMPACT));
	}

	/* fix-frequency clocks for plls */
	clk_dm(TS_CLK_TX5336_PLL0_D2, "pll0_d2",
		ts_clk_fixed_divider("pll0_d2", "pll0", 2));
	clk_dm(TS_CLK_TX5336_PLL0_D4, "pll0_d4",
		ts_clk_fixed_divider("pll0_d4", "pll0", 4));
	clk_dm(TS_CLK_TX5336_PLL0_D8, "pll0_d8",
		ts_clk_fixed_divider("pll0_d8", "pll0", 8));
	clk_dm(TS_CLK_TX5336_PLL0_D20, "pll0_d20",
		ts_clk_fixed_divider("pll0_d20", "pll0", 20));
	clk_dm(TS_CLK_TX5336_PLL0_D40, "pll0_d40",
		ts_clk_fixed_divider("pll0_d40", "pll0", 40));


	clk_dm(TS_CLK_TX5336_PLL1_D2, "pll1_d2",
		ts_clk_fixed_divider("pll1_d2", "pll1", 2));
	clk_dm(TS_CLK_TX5336_PLL1_D4, "pll1_d4",
		ts_clk_fixed_divider("pll1_d4", "pll1", 4));
	clk_dm(TS_CLK_TX5336_PLL1_D8, "pll1_d8",
		ts_clk_fixed_divider("pll1_d8", "pll1", 8));

	pclk = clk_get(NULL, &osc_in[0]);
	if (IS_ERR_OR_NULL(pclk)) {
		pr_err("register get %s failed\n", &osc_in[0]);
		return -ENODEV;
	}

	tx_tx5336_clks[TS_CLK_TX5336_OSC_24M] = pclk;

	return 0;
}

// 2 register comps go on
static int clk_tx5336_register_comps(struct ts_clk_params *paras)
{
	void *top_base = paras->top_base;
#define REGISTER_COMP(id, name) \
		clk_dm(id, name, \
			   clk_tx5336_register_comp(&ts_tx5336_comps[COMPIFY(id)], \
						top_base, \
						name, NULL))

	REGISTER_COMP(TS_CLK_TX5336_GIC_CLK, "gic_clk");
	REGISTER_COMP(TS_CLK_TX5336_PLL_MUX, "pll_mux");
	REGISTER_COMP(TS_CLK_TX5336_CPU_CCLK, "cpu_cclk");
	REGISTER_COMP(TS_CLK_TX5336_PLL_TOPNOC_CLK, "pll_mux_topnoc");
	REGISTER_COMP(TS_CLK_TX5336_TOPNOC_CLK, "topnoc_clk");
	REGISTER_COMP(TS_CLK_TX5336_PERI0_APB_CLK, "peri0_apb_clk");
	REGISTER_COMP(TS_CLK_TX5336_PERI1_APB_CLK, "peri1_apb_clk");
	REGISTER_COMP(TS_CLK_TX5336_PERI2_AHB_CLK, "peri2_ahb_clk");
	REGISTER_COMP(TS_CLK_TX5336_QSPI_AHB_CLK, "qspi_ahb_clk");
	REGISTER_COMP(TS_CLK_TX5336_QSPI_REF_CLK, "qspi_ref_clk");
	REGISTER_COMP(TS_CLK_TX5336_SPI0_CORE_CLK, "spi0_core_clk");
	REGISTER_COMP(TS_CLK_TX5336_SPI1_CORE_CLK, "spi1_core_clk");
	REGISTER_COMP(TS_CLK_TX5336_SPI2_CORE_CLK, "spi2_core_clk");
	REGISTER_COMP(TS_CLK_TX5336_UART0_CORE_CLK, "uart0_core_clk");
	REGISTER_COMP(TS_CLK_TX5336_UART1_CORE_CLK, "uart1_core_clk");
	REGISTER_COMP(TS_CLK_TX5336_UART2_CORE_CLK, "uart2_core_clk");
	REGISTER_COMP(TS_CLK_TX5336_UART3_CORE_CLK, "uart3_core_clk");
	REGISTER_COMP(TS_CLK_TX5336_SDHC0_CCLK, "sdhc0_cclk");
	REGISTER_COMP(TS_CLK_TX5336_SDHC1_CCLK, "sdhc1_cclk");
	REGISTER_COMP(TS_CLK_TX5336_USB_REF_CLK, "usb_ref_clk");
	REGISTER_COMP(TS_CLK_TX5336_UMCTL_P1_CLK, "umctl_p1_clk");
	REGISTER_COMP(TS_CLK_TX5336_UMCTL_P2_CLK, "umctl_p2_clk");
	REGISTER_COMP(TS_CLK_TX5336_UMCTL_P4_CLK, "umctl_p4_clk");
	REGISTER_COMP(TS_CLK_TX5336_RNE_CORE_PLL_CLK, "rne_core_pll01");
	REGISTER_COMP(TS_CLK_TX5336_RNE_CORE_CLK, "rne_core_clk");
	REGISTER_COMP(TS_CLK_TX5336_MEMBUF_AXI_CLK, "membfuf_axi_clk");
	REGISTER_COMP(TS_CLK_TX5336_VI_NOC_CLK, "vi_noc_clk");
	REGISTER_COMP(TS_CLK_TX5336_ISP_AXI_CLK, "isp_axi_clk");
	REGISTER_COMP(TS_CLK_TX5336_ISP_DRV_CLK, "isp_drv_clk");
	REGISTER_COMP(TS_CLK_TX5336_VPE_AXI_CLK, "vpe_axi_clk");
	REGISTER_COMP(TS_CLK_TX5336_VPE_CORE_CLK, "vpe_core_clk");
	REGISTER_COMP(TS_CLK_TX5336_DOF_AXI_CLK, "dof_axi_clk");
	REGISTER_COMP(TS_CLK_TX5336_CV_AHB_CLK, "cv_ahb_clk");
	REGISTER_COMP(TS_CLK_TX5336_CV_NOC_CLK, "cv_noc_clk");
	REGISTER_COMP(TS_CLK_TX5336_BIS_CORE_CLK, "bis_core_clk");
	REGISTER_COMP(TS_CLK_TX5336_GDC_CORE_CLK, "gdc_core_clk");
	REGISTER_COMP(TS_CLK_TX5336_MIPI_REF_CLK, "mipi_ref_clk");
	REGISTER_COMP(TS_CLK_TX5336_CHIP_OCLK0_CLK, "chip_oclk0_clk");
	REGISTER_COMP(TS_CLK_TX5336_CHIP_OCLK1_CLK, "chip_oclk1_clk");
	REGISTER_COMP(TS_CLK_TX5336_CHIP_OCLK2_CLK, "chip_oclk2_clk");
	REGISTER_COMP(TS_CLK_TX5336_CHIP_OCLK3_CLK, "chip_oclk3_clk");
	REGISTER_COMP(TS_CLK_TX5336_VPU_NOC_CLK, "vpu_noc_clk");
    REGISTER_COMP(TS_CLK_TX5336_I2C0_CCLK, "i2c0_cclk");
    REGISTER_COMP(TS_CLK_TX5336_I2C1_CCLK, "i2c1_cclk");
    REGISTER_COMP(TS_CLK_TX5336_I2C2_CCLK, "i2c2_cclk");
    REGISTER_COMP(TS_CLK_TX5336_I2C3_CCLK, "i2c3_cclk");
    REGISTER_COMP(TS_CLK_TX5336_I2C4_CCLK, "i2c4_cclk");
    REGISTER_COMP(TS_CLK_TX5336_I2C5_CCLK, "i2c5_cclk");
    REGISTER_COMP(TS_CLK_TX5336_I2C6_CCLK, "i2c6_cclk");

#undef REGISTER_COMP
	return 0;
}


// 3 register comps without mux
static int clk_tx5336_register_comps_without_mux(struct ts_clk_params *paras)
{
	void *top_base = paras->top_base;
		/* Dividing clocks, no mux */
#define REGISTER_COMP_NOMUX(id, name, parent) \
		clk_dm(id, name, \
			   clk_tx5336_register_comp(&ts_tx5336_comps[COMPIFY(id)], \
						top_base, name, parent))
	REGISTER_COMP_NOMUX(TS_CLK_TX5336_AUD_CODEC_AHB_CLK, "aud_codec_ahb_clk", "pll0_d2");
	REGISTER_COMP_NOMUX(TS_CLK_TX5336_AUD_DAC_CCLK, "aud_dac_cclk", "pll1_d4");
	REGISTER_COMP_NOMUX(TS_CLK_TX5336_AUD_DAC_PBCLK, "aud_dac_pbclk", "aud_dac_cclk");
	REGISTER_COMP_NOMUX(TS_CLK_TX5336_AUD_ED_TMR_STRB_CLK, "aud_ed_tmr_strb_clk", "aud_dac_cclk");
	REGISTER_COMP_NOMUX(TS_CLK_TX5336_AUD_ADC_CCLK, "aud_adc_cclk", "pll1_d4");
	REGISTER_COMP_NOMUX(TS_CLK_TX5336_DDR_APB_CLK, "ddr_apb_clk", "pll0_d2");
	REGISTER_COMP_NOMUX(TS_CLK_TX5336_UMCTL_P0_CLK, "umctl_p0_clk", "pll0");
	REGISTER_COMP_NOMUX(TS_CLK_TX5336_UMCTL_P3_CLK, "umctl_p3_clk", "pll0");
	REGISTER_COMP_NOMUX(TS_CLK_TX5336_UMCTL_P5_CLK, "umctl_p5_clk", "pll0");
	REGISTER_COMP_NOMUX(TS_CLK_TX5336_RNE_AHB_CLK, "rne_ahb_clk", "pll0_d2");
	REGISTER_COMP_NOMUX(TS_CLK_TX5336_RNE_AXI_CLK, "rne_axi_clk", "pll0");
	REGISTER_COMP_NOMUX(TS_CLK_TX5336_VI_AHB_CLK, "vi_ahb_clk", "pll0_d2");
	REGISTER_COMP_NOMUX(TS_CLK_TX5336_VI_APB_CLK, "vi_apb_clk", "pll0_d2");
	REGISTER_COMP_NOMUX(TS_CLK_TX5336_EBD_AXI_CLK, "ebd_axi_clk", "pll0");
	REGISTER_COMP_NOMUX(TS_CLK_TX5336_MIPI_TXCLKESC_CLK, "mipi_txclkesc_clk", "pll0_d4");
	REGISTER_COMP_NOMUX(TS_CLK_TX5336_CV_APB_CLK, "cv_apb_clk", "pll0_d2");
	REGISTER_COMP_NOMUX(TS_CLK_TX5336_AMR_AXI_CLK, "amr_axi_clk", "pll0");
	REGISTER_COMP_NOMUX(TS_CLK_TX5336_AMR_CORE_CLK, "amr_core_clk", "pll0");
	REGISTER_COMP_NOMUX(TS_CLK_TX5336_CVE_AXI_CLK, "cve_axi_clk", "pll0");
	REGISTER_COMP_NOMUX(TS_CLK_TX5336_CVE_CORE_CLK, "cve_core_clk", "pll0");
	REGISTER_COMP_NOMUX(TS_CLK_TX5336_BIS_AXI_CLK, "bis_axi_clk", "pll0");
	REGISTER_COMP_NOMUX(TS_CLK_TX5336_GDC_AXI_CLK, "gdc_axi_clk", "pll0");
	REGISTER_COMP_NOMUX(TS_CLK_TX5336_DPU_AXI_CLK, "dpu_axi_clk", "pll0");
	REGISTER_COMP_NOMUX(TS_CLK_TX5336_DPU_CORE_CLK, "dpu_core_clk", "pll0");
	REGISTER_COMP_NOMUX(TS_CLK_TX5336_ADMA_AXI_CLK, "adma_axi_clk", "pll0");
	REGISTER_COMP_NOMUX(TS_CLK_TX5336_PDM0_M_CLK, "pdm0_m_clk", "pll1_d4");
	REGISTER_COMP_NOMUX(TS_CLK_TX5336_PDM1_M_CLK, "pdm1_m_clk", "pll1_d4");
	REGISTER_COMP_NOMUX(TS_CLK_TX5336_PDM2_M_CLK, "pdm2_m_clk", "pll1_d4");
	REGISTER_COMP_NOMUX(TS_CLK_TX5336_PDM3_M_CLK, "pdm3_m_clk", "pll1_d4");
	REGISTER_COMP_NOMUX(TS_CLK_TX5336_VPU_CORE_CLK, "vpu_core_clk", "pll3");
	REGISTER_COMP_NOMUX(TS_CLK_TX5336_I2S_M_CLK, "i2s_m_clk", "pll1_d4");
	REGISTER_COMP_NOMUX(TS_CLK_TX5336_I2S_O_CLK, "i2s_o_clk", "pll1_d4");
    REGISTER_COMP_NOMUX(TS_CLK_TX5336_TIME0_8_CLK, "timer0_8", "pll0_d4");
    REGISTER_COMP_NOMUX(TS_CLK_TX5336_TIME0_7_CLK, "timer0_7", "pll0_d4");
    REGISTER_COMP_NOMUX(TS_CLK_TX5336_TIME0_6_CLK, "timer0_6", &osc_in[0]);
    REGISTER_COMP_NOMUX(TS_CLK_TX5336_TIME0_5_CLK, "timer0_5", &osc_in[0]);
    REGISTER_COMP_NOMUX(TS_CLK_TX5336_TIME1_8_CLK, "timer1_8", "pll0_d4");
    REGISTER_COMP_NOMUX(TS_CLK_TX5336_TIME1_7_CLK, "timer1_7", "pll0_d4");
    REGISTER_COMP_NOMUX(TS_CLK_TX5336_TIME1_6_CLK, "timer1_6", &osc_in[0]);
    REGISTER_COMP_NOMUX(TS_CLK_TX5336_TIME1_5_CLK, "timer1_5", &osc_in[0]);
#undef REGISTER_COMP_NOMUX
	return 0;
}

static DEFINE_SPINLOCK(ts_clk_lock);
// 4 register divider
static int clk_tx5336_register_dividers(struct ts_clk_params *paras)
{
	void *top_base = paras->top_base;
#define REGISTER_DIV(id, name, parent) do {\
		const struct ts_div_params *params = &ts_tx5336_divs[DIVIFY(id)]; \
		clk_dm(id, name, \
			clk_register_divider_with_sync(NULL, name, parent, 0, \
					top_base + params->off, \
					params->shift, params->width, \
					params->we, params->sync, 0, &ts_clk_lock)); \
	} while (false)

	 REGISTER_DIV(TS_CLK_TX5336_CPU_PLL0_CCLK, "cpu_cclk_pll0", "pll0");
	 REGISTER_DIV(TS_CLK_TX5336_CPU_PLL1_CCLK, "cpu_cclk_pll1", "pll1");
    REGISTER_DIV(TS_CLK_TX5336_CPU_ACLK, "cpu_aclk", "cpu_cclk");
	 REGISTER_DIV(TS_CLK_TX5336_PLL0_TOPNOC_CCLK, "topnoc_pll0", "pll0");
	 REGISTER_DIV(TS_CLK_TX5336_PLL1_D2_TOPNOC_CLK, "topnoc_pll1_d2", "pll1");

	 REGISTER_DIV(TS_CLK_TX5336_UMCTL_P1_PLL0_CLK, "umctl_p1_pll0", "pll0");
	 REGISTER_DIV(TS_CLK_TX5336_UMCTL_P1_PLL1_D2_CLK, "umctl_p1_pll1_d2", "pll1_d2");
	 REGISTER_DIV(TS_CLK_TX5336_UMCTL_P2_PLL0_CLK, "umctl_p2_pll0", "pll0");
	 REGISTER_DIV(TS_CLK_TX5336_UMCTL_P2_PLL1_D2_CLK, "umctl_p2_pll1_d2", "pll1_d2");
	 REGISTER_DIV(TS_CLK_TX5336_UMCTL_P4_PLL0_CLK, "umctl_p4_pll0", "pll0");
	 REGISTER_DIV(TS_CLK_TX5336_UMCTL_P4_PLL1_D2_CLK, "umctl_p4_pll1_d2", "pll1_d2");
	 REGISTER_DIV(TS_CLK_TX5336_RNE_CORE_PLL0_CLK, "rne_core_pll0_clk", "pll0");
	 REGISTER_DIV(TS_CLK_TX5336_RNE_CORE_PLL1_D2_CLK, "rne_core_pll1_d2_clk", "pll1_d2");
	 REGISTER_DIV(TS_CLK_TX5336_VI_NOC_PLL0_CLK, "vi_noc_pll0", "pll0");
	 REGISTER_DIV(TS_CLK_TX5336_VI_NOC_PLL1_D2_CLK, "vi_noc_pll1_d2", "pll1_d2");
	 REGISTER_DIV(TS_CLK_TX5336_ISP_AXI_PLL0_CLK, "isp_axi_pll0", "pll0");
	 REGISTER_DIV(TS_CLK_TX5336_ISP_AXI_PLL1_D2_CLK, "isp_axi_pll1_d2", "pll1_d2");
	 REGISTER_DIV(TS_CLK_TX5336_ISP_DRV_PLL0_CLK, "isp_drv_pll0", "pll0");
	 REGISTER_DIV(TS_CLK_TX5336_ISP_DRV_PLL1_D2_CLK, "isp_drv_pll1_d2", "pll1_d2");
	 REGISTER_DIV(TS_CLK_TX5336_VPE_AXI_PLL0_CLK, "vpe_axi_pll0", "pll0");
	 REGISTER_DIV(TS_CLK_TX5336_VPE_AXI_PLL1_D2_CLK, "vpe_axi_pll1_d2", "pll1_d2");
	 REGISTER_DIV(TS_CLK_TX5336_VPE_CORE_PLL0_CLK, "vpe_core_pll0", "pll0");
	 REGISTER_DIV(TS_CLK_TX5336_VPE_CORE_PLL1_D2_CLK, "vpe_core_pll1_d2", "pll1_d2");
	 REGISTER_DIV(TS_CLK_TX5336_DOF_AXI_PLL0_CLK, "dof_axi_pll0", "pll0");
	 REGISTER_DIV(TS_CLK_TX5336_DOF_AXI_PLL1_D2_CLK, "dof_axi_pll1_d2", "pll1_d2");
	 REGISTER_DIV(TS_CLK_TX5336_BIS_CORE_PLL0_CLK, "bis_core_pll0", "pll0");
	 REGISTER_DIV(TS_CLK_TX5336_BIS_CORE_PLL1_D2_CLK, "bis_core_pll1_d2", "pll1_d2");
	 REGISTER_DIV(TS_CLK_TX5336_GDC_CORE_PLL0_CLK, "gdc_core_pll0", "pll0");
	 REGISTER_DIV(TS_CLK_TX5336_GDC_CORE_PLL1_D2_CLK, "gdc_core_pll1_d2", "pll1_d2");

	 REGISTER_DIV(TS_CLK_TX5336_USB_REF_PLL0_D4_CLK, "usb_ref_pll0_d4", "pll0_d4");
	 REGISTER_DIV(TS_CLK_TX5336_CHIP_OCLK0_OSC_CLK, "chip_oclk0_osc", &osc_in[0]);
	 REGISTER_DIV(TS_CLK_TX5336_CHIP_OCLK0_pll0_d4_CLK, "chip_oclk0_pll0_d4", "pll0_d4");
	 REGISTER_DIV(TS_CLK_TX5336_CHIP_OCLK0_pll1_d8_CLK, "chip_oclk0_pll1_d8", "pll1_d8");
	 REGISTER_DIV(TS_CLK_TX5336_CHIP_OCLK1_OSC_CLK, "chip_oclk1_osc", &osc_in[0]);
 	 REGISTER_DIV(TS_CLK_TX5336_CHIP_OCLK1_pll0_d4_CLK, "chip_oclk1_pll0_d4", "pll0_d4");

	REGISTER_DIV(TS_CLK_TX5336_MIPI_REF_PLL0_D4_CLK, "mipi_ref_pll0_d4", "pll0_d4");
 	 REGISTER_DIV(TS_CLK_TX5336_CHIP_OCLK1_pll1_d8_CLK, "chip_oclk1_pll1_d8", "pll1_d8");
	 REGISTER_DIV(TS_CLK_TX5336_CHIP_OCLK2_OSC_CLK, "chip_oclk2_osc", &osc_in[0]);
	 REGISTER_DIV(TS_CLK_TX5336_CHIP_OCLK2_pll0_d4_CLK, "chip_oclk2_pll0_d4", "pll0_d4");
	 REGISTER_DIV(TS_CLK_TX5336_CHIP_OCLK2_pll1_d8_CLK, "chip_oclk2_pll1_d8", "pll1_d8");
	 REGISTER_DIV(TS_CLK_TX5336_CHIP_OCLK3_OSC_CLK, "chip_oclk3_osc", &osc_in[0]);
	 REGISTER_DIV(TS_CLK_TX5336_CHIP_OCLK3_pll0_d4_CLK, "chip_oclk3_pll0_d4", "pll0_d4");
	 REGISTER_DIV(TS_CLK_TX5336_CHIP_OCLK3_pll1_d8_CLK, "chip_oclk3_pll1_d8", "pll1_d8");
	 REGISTER_DIV(TS_CLK_TX5336_VPU_NOC_PLL0_CLK, "vpu_noc_pll0", "pll0");
	 REGISTER_DIV(TS_CLK_TX5336_VPU_NOC_PLL1_D2_CLK, "vpu_noc_pll1_d2", "pll1_d2");

#undef REGISTER_DIV
	return 0;
}

// 5 register gate
static int clk_tx5336_register_gates(struct ts_clk_params *paras)
{
	void *top_base = paras->top_base;

#define REGISTER_GATE(id, name, parent) do { \
		const struct ts_gate_params *params = &ts_tx5336_gates[GATEIFY(id)]; \
		clk_dm(id, name, \
			clk_register_gate(NULL, name, parent, 0, \
				top_base + params->off, \
				params->bit_idx, params->we, NULL)); \
	} while (false)
REGISTER_GATE(TS_CLK_TX5336_OTPC_APB_CLK,"otpc_apb_clk","peri0_apb_clk");
REGISTER_GATE(TS_CLK_TX5336_GPADC_APB_CLK,"gpadc_apb_clk","peri0_apb_clk");
REGISTER_GATE(TS_CLK_TX5336_IOMUX_APB_CLK,"iomux_apb_clk","peri0_apb_clk");
REGISTER_GATE(TS_CLK_TX5336_SYSREG_APB_CLK,"sysreg_apb_clk","peri0_apb_clk");
REGISTER_GATE(TS_CLK_TX5336_DAPLITE_APB_CLK,"daplite_apb_clk","peri0_apb_clk");
REGISTER_GATE(TS_CLK_TX5336_GPIO_APB_CLK,"gpio_apb_clk","peri0_apb_clk");
REGISTER_GATE(TS_CLK_TX5336_WDT_APB_CLK,"wdt_apb_clk","peri0_apb_clk");
REGISTER_GATE(TS_CLK_TX5336_PWM_APB_CLK,"pwm_apb_clk","peri1_apb_clk");
REGISTER_GATE(TS_CLK_TX5336_USI0_APB_CLK,"usi0_apb_clk","peri1_apb_clk");
REGISTER_GATE(TS_CLK_TX5336_USI1_APB_CLK,"usi1_apb_clk","peri1_apb_clk");
REGISTER_GATE(TS_CLK_TX5336_HDMA0_AHB_CLK,"hdma0_ahb_clk","peri2_ahb_clk");
REGISTER_GATE(TS_CLK_TX5336_HDMA1_AHB_CLK,"hdma1_ahb_clk","peri2_ahb_clk");
REGISTER_GATE(TS_CLK_TX5336_QSPI_APB_CLK,"qspi_apb_clk","peri0_apb_clk");
REGISTER_GATE(TS_CLK_TX5336_SPI0_APB_CLK,"spi0_apb_clk","peri0_apb_clk");
REGISTER_GATE(TS_CLK_TX5336_SPI1_APB_CLK,"spi1_apb_clk","peri0_apb_clk");
REGISTER_GATE(TS_CLK_TX5336_SPI2_APB_CLK,"spi2_apb_clk","peri0_apb_clk");
REGISTER_GATE(TS_CLK_TX5336_TIME0_APB_CLK,"time0_apb_clk","peri0_apb_clk");
REGISTER_GATE(TS_CLK_TX5336_TIME1_APB_CLK,"time1_apb_clk","peri0_apb_clk");
REGISTER_GATE(TS_CLK_TX5336_TIME0_3_CLK,"time0_3_clk",&osc_in[0]);
REGISTER_GATE(TS_CLK_TX5336_TIME0_4_CLK,"time0_4_clk",&osc_in[0]);
REGISTER_GATE(TS_CLK_TX5336_TIME1_3_CLK,"time1_3_clk",&osc_in[0]);
REGISTER_GATE(TS_CLK_TX5336_TIME1_4_CLK,"time1_4_clk",&osc_in[0]);

REGISTER_GATE(TS_CLK_TX5336_UART0_APB_CLK,"uart0_apb_clk","peri1_apb_clk");
REGISTER_GATE(TS_CLK_TX5336_UART1_APB_CLK,"uart1_apb_clk","peri1_apb_clk");
REGISTER_GATE(TS_CLK_TX5336_UART2_APB_CLK,"uart2_apb_clk","peri1_apb_clk");
REGISTER_GATE(TS_CLK_TX5336_UART3_APB_CLK,"uart3_apb_clk","peri1_apb_clk");
REGISTER_GATE(TS_CLK_TX5336_I2S_APB_CLK,"i2s_apb_clk","peri1_apb_clk");
REGISTER_GATE(TS_CLK_TX5336_I2S_O_CLK_O,"i2s_o_clk_o","pll1_d4");
REGISTER_GATE(TS_CLK_TX5336_SDHC0_AHB_CLK,"sdhc0_ahb_clk","peri2_ahb_clk");
REGISTER_GATE(TS_CLK_TX5336_SDHC1_AHB_CLK,"sdhc1_ahb_clk","peri2_ahb_clk");
REGISTER_GATE(TS_CLK_TX5336_USB_AHB_CLK,"usb_ahb_clk","peri2_ahb_clk");
REGISTER_GATE(TS_CLK_TX5336_ISP_CORE_CLK,"isp_core_clk","isp_drv_clk");
REGISTER_GATE(TS_CLK_TX5336_VPE_AHB_CLK,"vpe_ahb_clk","vi_ahb_clk");
REGISTER_GATE(TS_CLK_TX5336_VPE_ISP_CLK,"vpe_isp_clk","isp_drv_clk");
REGISTER_GATE(TS_CLK_TX5336_DOF_APB_CLK,"dof_apb_clk","vi_apb_clk");
REGISTER_GATE(TS_CLK_TX5336_DOF_CORE_CLK,"dof_core_clk","isp_drv_clk");
REGISTER_GATE(TS_CLK_TX5336_EBD_AHB_CLK,"ebd_ahb_clk","vi_apb_clk");
REGISTER_GATE(TS_CLK_TX5336_EBD_CORE_CLK,"ebd_core_clk","isp_drv_clk");
REGISTER_GATE(TS_CLK_TX5336_MIPI_CSI0_APB_CLK,"mipi_csi0_apb_clk","vi_apb_clk");
REGISTER_GATE(TS_CLK_TX5336_MIPI_CSI1_APB_CLK,"mipi_csi1_apb_clk","vi_apb_clk");
REGISTER_GATE(TS_CLK_TX5336_MIPI_CSI2_APB_CLK,"mipi_csi2_apb_clk","vi_apb_clk");
REGISTER_GATE(TS_CLK_TX5336_MIPI_CSI3_APB_CLK,"mipi_csi3_apb_clk","vi_apb_clk");
REGISTER_GATE(TS_CLK_TX5336_MIPI_RXPHY0_APB_CLK,"mipi_rxphy0_apb_clk","vi_apb_clk");
REGISTER_GATE(TS_CLK_TX5336_MIPI_RXPHY1_APB_CLK,"mipi_rxphy1_apb_clk","vi_apb_clk");
REGISTER_GATE(TS_CLK_TX5336_MIPI_CSI0_IPI0_CLK,"mipi_csi0_ipi0_clk","isp_drv_clk");
REGISTER_GATE(TS_CLK_TX5336_MIPI_CSI0_IPI1_CLK,"mipi_csi0_ipi1_clk","isp_drv_clk");
REGISTER_GATE(TS_CLK_TX5336_MIPI_CSI1_IPI_CLK,"mipi_csi1_ipi_clk","isp_drv_clk");
REGISTER_GATE(TS_CLK_TX5336_MIPI_CSI2_IPI0_CLK,"mipi_csi2_ipi0_clk","isp_drv_clk");
REGISTER_GATE(TS_CLK_TX5336_MIPI_CSI2_IPI1_CLK,"mipi_csi2_ipi1_clk","isp_drv_clk");
REGISTER_GATE(TS_CLK_TX5336_MIPI_CSI3_IPI_CLK,"mipi_csi3_ipi_clk","isp_drv_clk");
REGISTER_GATE(TS_CLK_TX5336_HASH_CLK,"hash_clk","cv_ahb_clk");
REGISTER_GATE(TS_CLK_TX5336_SKE_CLK,"ske_clk","cv_ahb_clk");
REGISTER_GATE(TS_CLK_TX5336_SPACC_CLK,"spacc_clk","cv_ahb_clk");
REGISTER_GATE(TS_CLK_TX5336_PKE_CLK,"pke_clk","cv_ahb_clk");
REGISTER_GATE(TS_CLK_TX5336_TRNG_CLK,"trng_clk","cv_ahb_clk");
REGISTER_GATE(TS_CLK_TX5336_AMR_AHB_CLK,"amr_ahb_clk","cv_ahb_clk");
REGISTER_GATE(TS_CLK_TX5336_CVE_AHB_CLK,"cve_ahb_clk","cv_ahb_clk");
REGISTER_GATE(TS_CLK_TX5336_BIS_APB_CLK,"bis_apb_clk","cv_apb_clk");
REGISTER_GATE(TS_CLK_TX5336_GDC_APB_CLK,"gdc_apb_clk","cv_apb_clk");
REGISTER_GATE(TS_CLK_TX5336_DPU_AHB_CLK,"dpu_ahb_clk","cv_ahb_clk");
REGISTER_GATE(TS_CLK_TX5336_MIPI_DSI_APB_CLK,"mipi_dsi_apb_clk","cv_apb_clk");
REGISTER_GATE(TS_CLK_TX5336_MIPI_TXPHY_APB_CLK,"mipi_txphy_apb_clk","cv_apb_clk");
REGISTER_GATE(TS_CLK_TX5336_ADMA_AHB_CLK,"adma_ahb_clk","cv_ahb_clk");
REGISTER_GATE(TS_CLK_TX5336_PDM0_APB_CLK,"pdm0_apb_clk","peri0_apb_clk");
REGISTER_GATE(TS_CLK_TX5336_PDM1_APB_CLK,"pdm1_apb_clk","peri0_apb_clk");
REGISTER_GATE(TS_CLK_TX5336_PDM2_APB_CLK,"pdm2_apb_clk","peri0_apb_clk");
REGISTER_GATE(TS_CLK_TX5336_PDM3_APB_CLK,"pdm3_apb_clk","peri0_apb_clk");
REGISTER_GATE(TS_CLK_TX5336_I2C0_APB_CLK, "i2c0_apb_clk", "peri1_apb_clk");
REGISTER_GATE(TS_CLK_TX5336_I2C1_APB_CLK, "i2c1_apb_clk", "peri1_apb_clk");
REGISTER_GATE(TS_CLK_TX5336_I2C2_APB_CLK, "i2c2_apb_clk", "peri1_apb_clk");
REGISTER_GATE(TS_CLK_TX5336_I2C3_APB_CLK, "i2c3_apb_clk", "peri1_apb_clk");
REGISTER_GATE(TS_CLK_TX5336_I2C4_APB_CLK, "i2c4_apb_clk", "peri1_apb_clk");
REGISTER_GATE(TS_CLK_TX5336_I2C5_APB_CLK, "i2c5_apb_clk", "peri1_apb_clk");
REGISTER_GATE(TS_CLK_TX5336_I2C6_APB_CLK, "i2c6_apb_clk", "peri1_apb_clk");

#undef REGISTER_GATE

	return 0;
}

// 6 init clk data
static void clk_tx5336_init_clk_data(struct ts_clk_params *paras)
{
	struct clk_onecell_data *clk_data = &paras->clk_data;
	pr_debug("++++++++++++++++++++++++++++%s \r\n",__func__);

	clk_data->clks = &tx_tx5336_clks[0];
	clk_data->clk_num = ARRAY_SIZE(tx_tx5336_clks);

}

int ts_clk_init_funcs(struct ts_clk_reg_funcs *pfuncs)
{
	if (!pfuncs)
		return -EINVAL;

	pfuncs->register_plls = clk_tx5336_register_plls;
	pfuncs->register_comps = clk_tx5336_register_comps;
	pfuncs->register_comps_without_mux = clk_tx5336_register_comps_without_mux;
	pfuncs->register_dividers = clk_tx5336_register_dividers;
	pfuncs->register_gates = clk_tx5336_register_gates;
    pfuncs->register_phases = clk_tx5336_register_phases;
	pfuncs->init_clk_data = clk_tx5336_init_clk_data;
	return 0;
}



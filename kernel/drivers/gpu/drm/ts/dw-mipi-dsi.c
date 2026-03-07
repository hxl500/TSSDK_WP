/*******************************************************************************
 * COPYRIGHT NOTICE
 * Copyright 2022 Tsingmicro, Inc.
 * All rights reserved.
 *
 * This program is free software and is provided to you under the terms of the
 * GNU General Public License version 2 as published by the Free Software
 * Foundation, and any use by you of this program is subject to the terms
 * of such GNU licence
 ******************************************************************************/
#include "linux/ioport.h"
#include <linux/clk.h>
#include <linux/component.h>
#include <linux/iopoll.h>
#include <linux/math64.h>
#include <linux/module.h>
#include <linux/of_device.h>
#include <linux/pm_runtime.h>
#include <linux/regmap.h>
#include <linux/reset.h>
#include <linux/mfd/syscon.h>
#include <linux/debugfs.h>
#include <drm/drm_atomic_helper.h>
#include <drm/drm_crtc.h>
#include <drm/drm_crtc_helper.h>
#include <drm/drm_mipi_dsi.h>
#include <drm/drm_of.h>
#include <drm/drm_panel.h>
#include <drm/drmP.h>
#include <video/mipi_display.h>

//#define DW_DPY_DEBUG

#define DRIVER_NAME    "dw-mipi-dsi"


#define SYS_MIPITX_CFG0			0x78
#define GRF_MIPITX_CFG0			0x54
#define GRF_MIPITX_CFGCLK_MASK		0xff
#define GRF_MIPITX_CFGCLK_SHIFT		0
#define GRF_SET_CFGCLK(val, cfgclk) { \
	val &= ~(GRF_MIPITX_CFGCLK_MASK << GRF_MIPITX_CFGCLK_SHIFT); \
	val |= (cfgclk << GRF_MIPITX_CFGCLK_SHIFT); \
}
#define GRF_MIPITX_HSFREQ_MASK		0x7f
#define GRF_MIPITX_HSFREQ_SHIFT		8
#define GRF_SET_HSFREQ(val, hsfreq) { \
	val &= ~(GRF_MIPITX_HSFREQ_MASK << GRF_MIPITX_HSFREQ_SHIFT); \
	val |= (hsfreq << GRF_MIPITX_HSFREQ_SHIFT); \
}
#define GRF_MIPITX_COLMAP_MASK		0xf
#define GRF_MIPITX_COLMAP_SHIFT		28
#define GRF_SET_COLMAP(val, color) { \
	 val &= ~(GRF_MIPITX_COLMAP_MASK << GRF_MIPITX_COLMAP_SHIFT); \
	 val |= (color << GRF_MIPITX_COLMAP_SHIFT); \
}

#define DSI_VERSION			0x00
#define VERSION				GENMASK(31, 8)
#define HWVER_131			0x31333100	/* IP version 1.31 */

#define DSI_PWR_UP			0x04
#define RESET				0
#define POWERUP				BIT(0)

#define DSI_CLKMGR_CFG			0x08
#define TO_CLK_DIVIDSION(div)		(((div) & 0xff) << 8)
#define TX_ESC_CLK_DIVIDSION(div)	(((div) & 0xff) << 0)

#define DSI_DPI_VCID			0x0c
#define DPI_VID(vid)			(((vid) & 0x3) << 0)

#define DSI_DPI_COLOR_CODING		0x10
#define EN18_LOOSELY			BIT(8)
#define DPI_COLOR_CODING_16BIT_1	0x0
#define DPI_COLOR_CODING_16BIT_2	0x1
#define DPI_COLOR_CODING_16BIT_3	0x2
#define DPI_COLOR_CODING_18BIT_1	0x3
#define DPI_COLOR_CODING_18BIT_2	0x4
#define DPI_COLOR_CODING_24BIT		0x5

#define DSI_DPI_CFG_POL			0x14
#define COLORM_ACTIVE_LOW		BIT(4)
#define SHUTD_ACTIVE_LOW		BIT(3)
#define HSYNC_ACTIVE_LOW		BIT(2)
#define VSYNC_ACTIVE_LOW		BIT(1)
#define DATAEN_ACTIVE_LOW		BIT(0)

#define DSI_DPI_LP_CMD_TIM		0x18
#define OUTVACT_LPCMD_TIME(p)		(((p) & 0xff) << 16)
#define INVACT_LPCMD_TIME(p)		((p) & 0xff)

#define DSI_DBI_CFG			0x20
#define DSI_DBI_CMDSIZE			0x28

#define DSI_PCKHDL_CFG			0x2c
#define EN_CRC_RX			BIT(4)
#define EN_ECC_RX			BIT(3)
#define EN_BTA				BIT(2)
#define EN_EOTP_RX			BIT(1)
#define EN_EOTP_TX			BIT(0)

#define DSI_MODE_CFG			0x34
#define ENABLE_VIDEO_MODE		0
#define ENABLE_CMD_MODE			BIT(0)

#define DSI_VID_MODE_CFG		0x38
#define FRAME_BTA_ACK			BIT(14)
#define ENABLE_LOW_POWER		(0x3f << 8)//(0x3f << 8)
#define ENABLE_LOW_POWER_MASK		(0x3f << 8)
#define VID_MODE_TYPE_NON_BURST_SYNC_PULSES	0x0
#define VID_MODE_TYPE_NON_BURST_SYNC_EVENTS	0x1
#define VID_MODE_TYPE_BURST			0x2
#define VID_MODE_TYPE_MASK			0x3
#define ENABLE_LOW_POWER_CMD		BIT(15)
#define VID_MODE_VPG_ENABLE		BIT(16)
#define VID_MODE_VPG_MODE		BIT(20)
#define VID_MODE_VPG_HORIZONTAL		BIT(24)

#define DSI_VID_PKT_SIZE		0x3c
#define VID_PKT_SIZE(p)			(((p) & 0x3fff) << 0)
#define VID_PKT_MAX_SIZE		0x3fff

#define DSI_VID_HSA_TIME		0x48
#define DSI_VID_HBP_TIME		0x4c
#define DSI_VID_HLINE_TIME		0x50
#define DSI_VID_VSA_LINES		0x54
#define DSI_VID_VBP_LINES		0x58
#define DSI_VID_VFP_LINES		0x5c
#define DSI_VID_VACTIVE_LINES		0x60
#define DSI_CMD_MODE_CFG		0x68
#define MAX_RD_PKT_SIZE_LP		BIT(24)
#define DCS_LW_TX_LP			BIT(19)
#define DCS_SR_0P_TX_LP			BIT(18)
#define DCS_SW_1P_TX_LP			BIT(17)
#define DCS_SW_0P_TX_LP			BIT(16)
#define GEN_LW_TX_LP			BIT(14)
#define GEN_SR_2P_TX_LP			BIT(13)
#define GEN_SR_1P_TX_LP			BIT(12)
#define GEN_SR_0P_TX_LP			BIT(11)
#define GEN_SW_2P_TX_LP			BIT(10)
#define GEN_SW_1P_TX_LP			BIT(9)
#define GEN_SW_0P_TX_LP			BIT(8)
#define EN_ACK_RQST			BIT(1)
#define EN_TEAR_FX			BIT(0)

#define CMD_MODE_ALL_LP			(MAX_RD_PKT_SIZE_LP | \
					 DCS_LW_TX_LP | \
					 DCS_SR_0P_TX_LP | \
					 DCS_SW_1P_TX_LP | \
					 DCS_SW_0P_TX_LP | \
					 GEN_LW_TX_LP | \
					 GEN_SR_2P_TX_LP | \
					 GEN_SR_1P_TX_LP | \
					 GEN_SR_0P_TX_LP | \
					 GEN_SW_2P_TX_LP | \
					 GEN_SW_1P_TX_LP | \
					 GEN_SW_0P_TX_LP)

#define DSI_GEN_HDR			0x6c
#define GEN_HDATA(data)			(((data) & 0xffff) << 8)
#define GEN_HDATA_MASK			(0xffff << 8)
#define GEN_HTYPE(type)			(((type) & 0xff) << 0)
#define GEN_HTYPE_MASK			0xff

#define DSI_GEN_PLD_DATA		0x70

#define DSI_CMD_PKT_STATUS		0x74
#define GEN_CMD_EMPTY			BIT(0)
#define GEN_CMD_FULL			BIT(1)
#define GEN_PLD_W_EMPTY			BIT(2)
#define GEN_PLD_W_FULL			BIT(3)
#define GEN_PLD_R_EMPTY			BIT(4)
#define GEN_PLD_R_FULL			BIT(5)
#define GEN_RD_CMD_BUSY			BIT(6)

#define DSI_TO_CNT_CFG			0x78
#define HSTX_TO_CNT(p)			(((p) & 0xffff) << 16)
#define LPRX_TO_CNT(p)			((p) & 0xffff)

#define DSI_BTA_TO_CNT			0x8c
#define DSI_LPCLK_CTRL			0x94
#define AUTO_CLKLANE_CTRL		BIT(1)
#define PHY_TXREQUESTCLKHS		BIT(1) | BIT(0)

#define DSI_PHY_TMR_LPCLK_CFG		0x98
#define PHY_CLKHS2LP_TIME(lbcc)		(((lbcc) & 0x3ff) << 16)
#define PHY_CLKLP2HS_TIME(lbcc)		((lbcc) & 0x3ff)

#define DSI_PHY_TMR_CFG			0x9c
#define PHY_HS2LP_TIME(lbcc)		(((lbcc) & 0xff) << 24)
#define PHY_LP2HS_TIME(lbcc)		(((lbcc) & 0xff) << 16)
#define MAX_RD_TIME(lbcc)		((lbcc) & 0x7fff)
#define PHY_HS2LP_TIME_V131(lbcc)	(((lbcc) & 0x3ff) << 16)
#define PHY_LP2HS_TIME_V131(lbcc)	((lbcc) & 0x3ff)


#define DSI_PHY_RSTZ			0xa0
#define PHY_DISFORCEPLL			0
#define PHY_ENFORCEPLL			BIT(3)
#define PHY_DISABLECLK			0
#define PHY_ENABLECLK			BIT(2)
#define PHY_RSTZ			0
#define PHY_UNRSTZ			BIT(1)
#define PHY_SHUTDOWNZ			0
#define PHY_UNSHUTDOWNZ			BIT(0)

#define DSI_PHY_IF_CFG			0xa4
#define N_LANES(n)			((((n) - 1) & 0x3) << 0)
#define PHY_STOP_WAIT_TIME(cycle)	(((cycle) & 0xff) << 8)

#define DSI_PHY_STATUS			0xb0
#define LOCK				BIT(0)
#define STOP_STATE_CLK_LANE		BIT(2)

#define DSI_PHY_TST_CTRL0		0xb4
#define PHY_TESTCLK			BIT(1)
#define PHY_UNTESTCLK			0
#define PHY_TESTCLR			BIT(0)
#define PHY_UNTESTCLR			0

#define DSI_PHY_TST_CTRL1		0xb8
#define PHY_TESTEN			BIT(16)
#define PHY_UNTESTEN			0
#define PHY_TESTDOUT(n)			(((n) & 0xff) << 8)
#define PHY_TESTDIN(n)			(((n) & 0xff) << 0)

#define DSI_INT_ST0			0xbc
#define DSI_INT_ST1			0xc0
#define DSI_INT_MSK0			0xc4
#define DSI_INT_MSK1			0xc8

#define DSI_PHY_TMR_RD_CFG		0xf4
#define MAX_RD_TIME_V131(lbcc)		((lbcc) & 0x7fff)

/* DPHY register */
#define DPHY_PLL_CTRL_15E		0X15E
#define   PLL_CPBIAS_CNTRL		0x10
#define DPHY_PLL_CTRL_162		0X162
#define   PLL_GMP_CNTRL			0x00
#define   PLL_INT_CNTRL			(0x01 << 2)
#define DPHY_PLL_CTRL_16E		0X16E
#define   PLL_PROP_CNTRL		0x0C
#define DPHY_PLL_CTRL_178		0x178
#define   PLL_N_OVR_EN			BIT(7)
#define   PLL_N(val)			((((val) - 1)  & 0x0f) << 3)
#define DPHY_PLL_CTRL_179		0x179
#define DPHY_PLL_CTRL_17A		0x17A
#define   PLL_M_L(val)			(((val) - 2) & 0xff)
#define   PLL_M_H(val)			((((val) - 2) >> 8) & 0x03)
#define DPHY_PLL_CTRL_17B		0x17B
#define   PLL_VCO_OVR_EN		BIT(7)
#define   PLL_VCO_CNT(val)		(((val) & 0x3f) << 1)
#define   PLL_M_OVR_EN			BIT(0)
#define DPHY_COMM_BLK_CTRL_1AC		0x1ac
#define   CB_CLKDIV_CLK_EN		BIT(4)
#define   CB_CAL_CHOP_EN		BIT(3)	/* default value */
#define   CB_ATB_SEL			0x03	/* default value */
#define DPHY_T_SLEW_CTL_270		0x270
#define DPHY_T_SLEW_CTL_271		0x271
#define   SR_OSC_FREQ_TARGET_L(val)	((val) & 0xff)
#define   SR_OSC_FREQ_TARGET_H(val)	((val >> 8) & 0x0f)
#define DPHY_T_SLEW_CTL_272		0x272
#define   SLEW_ON			(0x1 << 4)
#define DPHY_CLK_SLEW_CTL_310		0x310
#define DPHY_D0_SLEW_CTL_50B		0x50B
#define DPHY_D1_SLEW_CTL_70B		0x70B
#define DPHY_D2_SLEW_CTL_90B		0x90B
#define DPHY_D3_SLEW_CTL_B0B		0xB0B
#define    SR_FINISH_OVER_EN		BIT(3)
#define    SR_FINISH_OVER		BIT(2)
#define    SRCAL_EN_OVER_EN		BIT(1)


#define PHY_STATUS_TIMEOUT_US		10000
#define CMD_PKT_STATUS_TIMEOUT_US	20000

#define DW_MIPI_NEEDS_PHY_CFG_CLK	BIT(0)
#define DW_MIPI_NEEDS_GRF_CLK		BIT(1)

// #define DPHY_PREDIV_REG			0x00c
// #define DPHY_FBDIV_REG			0x010
#define DPHY_GLB_CFG00			0x000
#define DPHY_GLB_CFG01			0x004
#define DPHY_GLB_CFG03			0x00c
#define DPHY_GLB_CFG04			0x010
#define DPHY_GLB_CFG20			0x080

struct dw_mipi_dsi_dphy_timing {
	u16 data_hs2lp;
	u16 data_lp2hs;
	u16 clk_hs2lp;
	u16 clk_lp2hs;
};

struct dw_mipi_dsi_plat_data {
	unsigned int flags;
	unsigned int max_data_lanes;
};

#ifdef CONFIG_DEBUG_FS
#define VPG_DEFS(name, dsi) \
	((void __force *)&((*dsi).vpg_defs.name))

#define REGISTER(name, mask, dsi) \
	{ #name, VPG_DEFS(name, dsi), mask, dsi }

struct debugfs_entries {
	const char				*name;
	bool					*reg;
	u32					mask;
	struct dw_mipi_dsi			*dsi;
};
#endif /* CONFIG_DEBUG_FS */


struct dw_mipi_dsi {
	struct drm_encoder encoder;
	struct drm_connector connector;
	struct mipi_dsi_host dsi_host;
	struct drm_panel *panel;
	struct device *dev;
	struct regmap *grf_regmap;
	struct regmap *sys_regmap;
	void __iomem *base;
	void __iomem *phy_base;

	struct clk *grf_clk;
	struct clk *pllref_clk;
	struct clk *pclk;
	struct clk *phy_cfg_clk;

	struct clk *apb_clk;
	struct clk *txphy_apb_clk;
	struct clk *txclkesc_clk;
	struct clk *ref_clk;
	unsigned long ref_clk_rate;

	int dpms_mode;
	unsigned int lane_mbps; /* per lane */
	u32 channel;
	u32 lanes;
	u32 format;
	u16 input_div;
	u16 feedback_div;
	unsigned long mode_flags;

#ifdef CONFIG_DEBUG_FS
	struct dentry *debugfs;
	struct debugfs_entries *debugfs_vpg;
	struct {
		bool vpg;
		bool vpg_horizontal;
		bool vpg_ber_pattern;
	} vpg_defs;
#endif /* CONFIG_DEBUG_FS */

	const struct dw_mipi_dsi_plat_data *pdata;
};

enum dw_mipi_dsi_mode {
	DW_MIPI_DSI_CMD_MODE,
	DW_MIPI_DSI_VID_MODE,
};

struct dphy_pll_hsfreq_map {
	unsigned int mbps;
	u8 hsfreq;
};

static const struct dphy_pll_hsfreq_map hsfreq_map[] = {
	{  80, 0x00},  //      80 - 97.125  Mbps
	{  90, 0x10},  //      80 - 107.625 Mbps
	{ 100, 0x20},  //  83.125 - 118.125 Mbps
	{ 110, 0x30},  //  92.625 - 128.625 Mbps
	{ 120, 0x01},  // 102.125 - 139.125 Mbps
	{ 130, 0x11},  // 111.625 - 149.625 Mbps
	{ 140, 0x21},  // 121.125 - 160.125 Mbps
	{ 150, 0x31},  // 130.625 - 170625  Mbps
	{ 160, 0x02},  // 140.125 - 181.125 Mbps
	{ 170, 0x12},  // 149.625 - 191.625 Mbps
	{ 180, 0x22},  // 159.125 - 202 125 Mbps
	{ 190, 0x32},  // 168.625 - 212.625 Mbps
	{ 205, 0x03},  // 182.875 - 228 375 Mbps
	{ 220, 0x13},  // 197.125 - 244.125 Mbps
	{ 235, 0x23},  // 211.375 - 259.875 Mbps
	{ 250, 0x33},  // 225.625 - 275.625 Mbps
	{ 275, 0x04},  // 249.375 - 301.875 Mbps
	{ 300, 0x14},  // 273.125 - 328.125 Mbps
	{ 325, 0x25},  // 296.875 - 354.375 Mbps
	{ 350, 0x35},  // 320.625 - 380.625 Mbps
	{ 400, 0x05},  // 368.125 - 433.125 Mbps
	{ 450, 0x16},  // 415.625 - 485.625 Mbps
	{ 500, 0x26},  // 463.125 - 538.125 Mbps
	{ 550, 0x37},  // 510.625 - 590.625 Mbps
	{ 600, 0x07},  // 558.125 - 643.125 Mbps
	{ 650, 0x18},  // 605.625 - 695.625 Mbps
	{ 700, 0x28},  // 653.125 - 748.125 Mbps
	{ 750, 0x39},  // 700.625 - 800.625 Mbps
	{ 800, 0x09},  // 748.125 - 853.125 Mbps
	{ 850, 0x19},  // 795.625 - 905.625 Mbps
	{ 900, 0x29},  // 843.125 - 958 125 Mbps
	{ 950, 0x3a},  // 890.625 - 1010.625 Mbps
	{1000, 0x0a},  // 938.125 - 1063.125 Mbps
	{1050, 0x1a},  // 985.625 - 1115.625 Mbps
	{1100, 0x2a},  //1033.125 - 1168.125 Mbps
	{1150, 0x3b},  //1080.625 - 1220.625 Mbps
	{1200, 0x0b},  //1128.125 - 1273.125 Mbps
	{1250, 0x1b},  //1175.625 - 1325.625 Mbps
	{1300, 0x2b},  //1223.125 - 1378.125 Mbps
	{1350, 0x3c},  //1270.625 - 1430.625 Mbps
	{1400, 0x0c},  //1318.125 - 1483.125 Mbps
	{1450, 0x1c},  //1365.625 - 1535.625 Mbps
	{1500, 0x2c},  //1413.125 - 1588.125 Mbps
};

struct hstt {
	unsigned int maxfreq;
	struct dw_mipi_dsi_dphy_timing timing;
};

#define HSTT(_maxfreq, _c_lp2hs, _c_hs2lp, _d_lp2hs, _d_hs2lp)	\
{					\
	.maxfreq = _maxfreq,		\
	.timing = {			\
		.clk_lp2hs = _c_lp2hs,	\
		.clk_hs2lp = _c_hs2lp,	\
		.data_lp2hs = _d_lp2hs,	\
		.data_hs2lp = _d_hs2lp,	\
	}				\
}

/* Table A-4 High-Speed Transition Times */
static struct hstt hstt_table[] = {
	HSTT(90, 21, 17, 15, 10),
	HSTT(100, 23, 17, 16, 10),
	HSTT(110, 22, 17, 16, 10),
	HSTT(130, 26, 20, 18, 11),
	HSTT(140, 27, 19, 19, 11),
	HSTT(150, 27, 19, 19, 11),
	HSTT(160, 28, 20, 20, 12),
	HSTT(170, 30, 21, 22, 13),
	HSTT(180, 30, 21, 23, 13),
	HSTT(190, 31, 21, 23, 13),
	HSTT(205, 32, 22, 24, 13),
	HSTT(220, 35, 22, 25, 13),
	HSTT(235, 37, 26, 27, 15),
	HSTT(250, 38, 28, 27, 16),
	HSTT(275, 41, 29, 30, 71),
	HSTT(300, 43, 29, 32, 18),
	HSTT(325, 45, 32, 35, 19),
	HSTT(350, 48, 33, 36, 18),
	HSTT(400, 51, 35, 40, 20),
	HSTT(450, 59, 37, 44, 21),
	HSTT(500, 65, 40, 49, 23),
	HSTT(550, 71, 41, 54, 24),
	HSTT(600, 77, 44, 57, 26),
	HSTT(650, 82, 46, 64, 27),
	HSTT(700, 87, 48, 67, 28),
	HSTT(750, 94, 52, 71, 29),
	HSTT(800, 99, 52, 75, 31),
	HSTT(850, 105, 55, 82, 32),
	HSTT(900, 110, 58, 85, 32),
	HSTT(950, 115, 58, 88, 35),
	HSTT(1000, 120, 62, 93, 36),
	HSTT(1050, 128, 63, 99, 38),
	HSTT(1100, 132, 65, 102, 38),
	HSTT(1150, 138, 67, 106, 39),
	HSTT(1200, 146, 69, 112, 42),
	HSTT(1250, 151, 71, 117, 43),
	HSTT(1300, 153, 74, 120, 45),
	HSTT(1350, 160, 73, 124, 46),
	HSTT(1400, 165, 76, 130, 47),
	HSTT(1450, 172, 78, 134, 49),
	HSTT(1500, 177, 80, 138, 49)
};

struct dphy_slew_rate {
	unsigned int mbps;
	u16 sr_osc_freq_target;
	u8 sr_range;
};

static const struct dphy_slew_rate slew_rate_map[] = {
	{  500, 0x384, 1},
	{ 1000, 0x4e2, 1},
	{ 1500, 0x7d0, 0},
};

#define VCO_RANGE_MIN	0
#define VCO_RANGE_MAX	1
#define VCO_RANGE_VAL	2
uint32_t dsi_vco_pll_table[][3] = {
	{1150 * 2, 1250 * 2, 0x01},	// 1150 - 1250 Mbps
	{1100 * 2, 1152 * 2, 0x01},
	{630 * 2, 1149 * 2, 0x03},
	{420 * 2, 660 * 2, 0x09},
	{320 * 2, 440 * 2, 0x0f},
	{210 * 2, 330 * 2, 0x19},
	{160 * 2, 220 * 2, 0x1f},
	{105 * 2, 165 * 2, 0x29},
	{80  * 2, 110 * 2, 0x2f},
	{105,    165,  0x39},	// 52.5 - 82.5 MHz
	{40 * 2, 55 * 2, 0x3f},
};

 int mbps_to_hsfreq(unsigned int lane_mbps)
{
	uint32_t diff_i, diff_i_plus_1;
	uint32_t clk_sel;
	uint32_t hsfreq;
	int i;

	for (i = 0; i < (ARRAY_SIZE(hsfreq_map) - 1); i++) {
		if ((lane_mbps >= hsfreq_map[i].mbps)
			&& (lane_mbps <= hsfreq_map[i + 1].mbps)) {
			diff_i = lane_mbps - hsfreq_map[i].mbps;
			diff_i_plus_1 = hsfreq_map[i + 1].mbps - lane_mbps;
			if (diff_i < diff_i_plus_1) {
				clk_sel = hsfreq_map[i].mbps;
				hsfreq = hsfreq_map[i].hsfreq;
			} else {
				clk_sel = hsfreq_map[i + 1].mbps;
				hsfreq = hsfreq_map[i + 1].hsfreq;
			}
			DRM_INFO("target %d, select %d, hsfreq 0x%x\n",
				lane_mbps, clk_sel, hsfreq);
			return hsfreq;
		}
	}
	return -EINVAL;
}

 int mbps_to_slew(unsigned int lane_mbps, struct dphy_slew_rate *slew)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(slew_rate_map); i++)
		if (lane_mbps <= slew_rate_map[i].mbps)
			break;

	if (i == ARRAY_SIZE(slew_rate_map))
		return -EINVAL;

	memcpy(slew, &slew_rate_map[i], sizeof(struct dphy_slew_rate));
	DRM_DEBUG("%d Mbps slew osc freq 0x%x range %d\n", lane_mbps,
		slew->sr_osc_freq_target, slew->sr_range);
	return 0;
}

 int mbps_to_vco(unsigned int lane_mbps)
{
	int i;

	for (i = 0; i <= ARRAY_SIZE(dsi_vco_pll_table); i++) {
		if ((lane_mbps >= dsi_vco_pll_table[i][VCO_RANGE_MIN])
			&& (lane_mbps <= dsi_vco_pll_table[i][VCO_RANGE_MAX])) {
			DRM_DEBUG("%d Mbps i %d vco cnt 0x%x\n", lane_mbps, i,
				dsi_vco_pll_table[i][VCO_RANGE_VAL]);
			return dsi_vco_pll_table[i][VCO_RANGE_VAL];
		}
	}
	return -EINVAL;
}


/*
 * The controller should generate 2 frames before
 * preparing the peripheral.
 */
 void dw_mipi_dsi_wait_for_two_frames(struct drm_display_mode *mode)
{
	int refresh, two_frames;

	refresh = drm_mode_vrefresh(mode);
	two_frames = DIV_ROUND_UP(MSEC_PER_SEC, refresh) * 2;
	msleep(two_frames);
}


static inline struct dw_mipi_dsi *host_to_dsi(struct mipi_dsi_host *host)
{
	return container_of(host, struct dw_mipi_dsi, dsi_host);
}

static inline struct dw_mipi_dsi *con_to_dsi(struct drm_connector *con)
{
	return container_of(con, struct dw_mipi_dsi, connector);
}

static inline struct dw_mipi_dsi *encoder_to_dsi(struct drm_encoder *encoder)
{
	return container_of(encoder, struct dw_mipi_dsi, encoder);
}

static inline void dsi_write(struct dw_mipi_dsi *dsi, u32 reg, u32 val)
{
	writel(val, dsi->base + reg);
}

static inline u32 dsi_read(struct dw_mipi_dsi *dsi, u32 reg)
{
	return readl(dsi->base + reg);
}

static inline void phy_write(struct dw_mipi_dsi *dsi, u32 reg, u32 val)
{
	writel(val, dsi->phy_base + reg);
}

static inline u32 phy_read(struct dw_mipi_dsi *dsi, u32 reg)
{
	return readl(dsi->phy_base + reg);
}
 
void dw_mipi_dsi_phy_write(struct dw_mipi_dsi *dsi, u16 reg, u8 val)
{
	u32 tmp;

	/* write address[11:8] */
	tmp = PHY_UNTESTCLK | PHY_UNTESTCLR;
	dsi_write(dsi, DSI_PHY_TST_CTRL0, tmp);
	tmp = PHY_UNTESTEN | PHY_TESTDOUT(0) | PHY_TESTDIN(0);
	dsi_write(dsi, DSI_PHY_TST_CTRL1, tmp);
	tmp = PHY_TESTEN | PHY_TESTDOUT(0) | PHY_TESTDIN(0);
	dsi_write(dsi, DSI_PHY_TST_CTRL1, tmp);
	tmp = PHY_TESTCLK | PHY_UNTESTCLR;
	dsi_write(dsi, DSI_PHY_TST_CTRL0, tmp);
	tmp = PHY_TESTEN | PHY_TESTDOUT(0) | PHY_TESTDIN(0);
	dsi_write(dsi, DSI_PHY_TST_CTRL1, tmp);
	tmp = PHY_UNTESTCLK | PHY_UNTESTCLR;
	dsi_write(dsi, DSI_PHY_TST_CTRL0, tmp);
	tmp = PHY_UNTESTEN | PHY_TESTDOUT(0) | PHY_TESTDIN(0);
	dsi_write(dsi, DSI_PHY_TST_CTRL1, tmp);
	tmp = PHY_UNTESTEN | PHY_TESTDOUT(0) | ((reg & 0xf00) >> 8);
	dsi_write(dsi, DSI_PHY_TST_CTRL1, tmp);
	tmp = PHY_TESTCLK | PHY_UNTESTCLR;
	dsi_write(dsi, DSI_PHY_TST_CTRL0, tmp);
	tmp = PHY_UNTESTCLK | PHY_UNTESTCLR;
	dsi_write(dsi, DSI_PHY_TST_CTRL0, tmp);

	/* write address[7:0] */
	tmp = PHY_TESTEN | PHY_TESTDOUT(0);
	dsi_write(dsi, DSI_PHY_TST_CTRL1, tmp);
	tmp = PHY_TESTCLK | PHY_UNTESTCLR;
	dsi_write(dsi, DSI_PHY_TST_CTRL0, tmp);
	tmp = PHY_TESTEN | PHY_TESTDOUT(0) | (reg & 0xff);
	dsi_write(dsi, DSI_PHY_TST_CTRL1, tmp);
	tmp = PHY_UNTESTCLK | PHY_UNTESTCLR;
	dsi_write(dsi, DSI_PHY_TST_CTRL0, tmp);
	tmp = PHY_UNTESTEN | PHY_TESTDOUT(0);
	dsi_write(dsi, DSI_PHY_TST_CTRL1, tmp);
	tmp = PHY_UNTESTEN | PHY_TESTDOUT(0) | val;
	dsi_write(dsi, DSI_PHY_TST_CTRL1, tmp);
	tmp = PHY_TESTCLK | PHY_UNTESTCLR;
	dsi_write(dsi, DSI_PHY_TST_CTRL0, tmp);
	tmp = PHY_UNTESTCLK | PHY_UNTESTCLR;
	dsi_write(dsi, DSI_PHY_TST_CTRL0, tmp);
}

#ifdef DW_DPY_DEBUG
static u32 dw_mipi_dsi_phy_read(struct dw_mipi_dsi *dsi, u16 reg)
{
	u32 read_data;
	u32 data;
	u32 tmp;

	//write address[11:8]
	tmp = PHY_TESTEN | PHY_TESTDOUT(0) | PHY_TESTDIN(0);
	dsi_write(dsi, DSI_PHY_TST_CTRL1, tmp);
	tmp = PHY_TESTCLK + PHY_UNTESTCLR;
	dsi_write(dsi, DSI_PHY_TST_CTRL0, tmp);
	tmp = PHY_UNTESTCLK + PHY_UNTESTCLR;
	dsi_write(dsi, DSI_PHY_TST_CTRL0, tmp);
	tmp = PHY_UNTESTEN + PHY_TESTDOUT(0) + ((reg & 0xf00) >> 8);
	dsi_write(dsi, DSI_PHY_TST_CTRL1, tmp);
	tmp = PHY_TESTCLK + PHY_UNTESTCLR;
	dsi_write(dsi, DSI_PHY_TST_CTRL0, tmp);
	tmp = PHY_UNTESTCLK + PHY_UNTESTCLR;
	dsi_write(dsi, DSI_PHY_TST_CTRL0, tmp);

	//write address[7:0] and data
	tmp = PHY_TESTEN + PHY_TESTDOUT(0) + (reg & 0xff);
	dsi_write(dsi, DSI_PHY_TST_CTRL1, tmp);
	tmp = PHY_TESTCLK + PHY_UNTESTCLR;
	dsi_write(dsi, DSI_PHY_TST_CTRL0, tmp);
	tmp = PHY_UNTESTCLK + PHY_UNTESTCLR;
	dsi_write(dsi, DSI_PHY_TST_CTRL0, tmp);
	tmp = PHY_UNTESTEN + PHY_TESTDOUT(0) + PHY_TESTDIN(0);
	dsi_write(dsi, DSI_PHY_TST_CTRL1, tmp);
	read_data = dsi_read(dsi, DSI_PHY_TST_CTRL1);
	data = (read_data & 0xff00) >> 8;

	return data;
}
#endif

static int dw_mipi_dsi_phy_init(struct dw_mipi_dsi *dsi)
{
	int __maybe_unused ret, hsfreq, vco_cntrl, val, cfgclk, cfgclkrange,prediv,ref_clk_rate;
	struct  dphy_slew_rate __maybe_unused slew;
	u16 __maybe_unused reg_list[] = {
		DPHY_T_SLEW_CTL_270, DPHY_T_SLEW_CTL_271,
		DPHY_T_SLEW_CTL_272, DPHY_D3_SLEW_CTL_B0B,
		DPHY_D2_SLEW_CTL_90B, DPHY_D1_SLEW_CTL_70B,
		DPHY_PLL_CTRL_179, DPHY_PLL_CTRL_17A,
		DPHY_PLL_CTRL_17B, DPHY_PLL_CTRL_178,
		DPHY_PLL_CTRL_15E, DPHY_PLL_CTRL_162,
		DPHY_PLL_CTRL_16E, DPHY_COMM_BLK_CTRL_1AC};
	int __maybe_unused reg, i;

	/* Start by clearing PHY state */
	dsi_write(dsi, DSI_PHY_RSTZ, PHY_DISFORCEPLL | PHY_DISABLECLK
		  | PHY_RSTZ | PHY_SHUTDOWNZ);
	dsi_write(dsi, DSI_PHY_TST_CTRL0, PHY_UNTESTCLR);
	dsi_write(dsi, DSI_PHY_TST_CTRL0, PHY_TESTCLR);
	dsi_write(dsi, DSI_PHY_TST_CTRL0, PHY_UNTESTCLR);

#ifdef CONFIG_ARCH_TS_TX5336

	ret = regmap_read(dsi->sys_regmap, SYS_MIPITX_CFG0, &val);
	if (ret)
		dev_err(dsi->dev, "%s:Failed to read sys regs.\n", __func__);
	val |= 0x01;
	ret = regmap_write(dsi->sys_regmap, SYS_MIPITX_CFG0, val);
	if (ret)
		dev_err(dsi->dev, "%s:Failed to write sys regs.\n", __func__);

	ref_clk_rate = dsi->ref_clk_rate/1000000;
	prediv = 1;
	val = (dsi->lane_mbps * prediv * 2) / (ref_clk_rate * 2);
	
	phy_write(dsi,DPHY_GLB_CFG03,prediv); 	// fbdiv[8] and prediv
	phy_write(dsi,DPHY_GLB_CFG04,val); 		// fbdiv

	phy_write(dsi,DPHY_GLB_CFG01,0xe4);		// reg_syncrst reset
	phy_write(dsi,DPHY_GLB_CFG00,0x7d);		// lane_en,clk_en enable
	phy_write(dsi,DPHY_GLB_CFG01,0xe0);		// reg_syncrst normal
	ndelay(10);
	phy_write(dsi,DPHY_GLB_CFG20,0x1e);
	phy_write(dsi,DPHY_GLB_CFG20,0x1f);

#if 0
	phy_write(dsi,0x114,0x03);
	phy_write(dsi,0x194,0x03);
	phy_write(dsi,0x214,0x03);
	phy_write(dsi,0x294,0x03);
	phy_write(dsi,0x314,0x03);

	phy_write(dsi,0x118,0x7c);
	phy_write(dsi,0x198,0x7c);
	phy_write(dsi,0x218,0x7c);
	phy_write(dsi,0x298,0x7c);
	phy_write(dsi,0x318,0x7c);

	phy_write(dsi,0x11c,0x1B);
	phy_write(dsi,0x19c,0x07);
	phy_write(dsi,0x21c,0x07);
	phy_write(dsi,0x29c,0x07);
	phy_write(dsi,0x31c,0x07);

	phy_write(dsi,0x120,0x08);
	phy_write(dsi,0x1a0,0x08);
	phy_write(dsi,0x220,0x08);
	phy_write(dsi,0x2a0,0x08);
	phy_write(dsi,0x320,0x08);

	// phy_write(dsi,0x128,0x00);
	// phy_write(dsi,0x1a8,0x00);
	// phy_write(dsi,0x228,0x00);
	// phy_write(dsi,0x2a8,0x00);
	// phy_write(dsi,0x328,0x00);

	// phy_write(dsi,0x140,0x84);
	// phy_write(dsi,0x1c0,0x84);
	// phy_write(dsi,0x240,0x84);
	// phy_write(dsi,0x2c0,0x84);
	// phy_write(dsi,0x340,0x84);

#endif
#else
	ret = clk_prepare_enable(dsi->phy_cfg_clk);
	if (ret) {
		dev_err(dsi->dev, "Failed to enable phy_cfg_clk\n");
		return ret;
	}

	/* write hsfreq to grf register */
	hsfreq = mbps_to_hsfreq(dsi->lane_mbps);
	if (hsfreq < 0) {
		dev_err(dsi->dev, "fail to get hsfreq for %dmbps lane clock\n",
			dsi->lane_mbps);
		return hsfreq;
	}
	ret = regmap_read(dsi->grf_regmap, GRF_MIPITX_CFG0, &val);
	if (ret) {
		dev_err(dsi->dev, "Failed to read 1 grf regs.\n");
		return ret;
	}
	GRF_SET_HSFREQ(val, hsfreq);
	ret = regmap_write(dsi->grf_regmap, GRF_MIPITX_CFG0, val);
	if (ret) {
		dev_err(dsi->dev, "Failed to write 1 grf regs.\n");
		return ret;
	}

	/* hs tx slew rate calibration */
	ret = mbps_to_slew(dsi->lane_mbps, &slew);
	if (ret < 0) {
		dev_err(dsi->dev, "fail to get slew for %dmbps lane clock\n",
			dsi->lane_mbps);
		return ret;
	}
	dw_mipi_dsi_phy_write(dsi, DPHY_T_SLEW_CTL_270,
		SR_OSC_FREQ_TARGET_L(slew.sr_osc_freq_target));
	dw_mipi_dsi_phy_write(dsi, DPHY_T_SLEW_CTL_271,
		SR_OSC_FREQ_TARGET_H(slew.sr_osc_freq_target));
	dw_mipi_dsi_phy_write(dsi, DPHY_T_SLEW_CTL_272,
		slew.sr_range | SLEW_ON);
	val = SR_FINISH_OVER_EN | SR_FINISH_OVER | SRCAL_EN_OVER_EN;
	switch (dsi->lanes) {
	case 3:
		dw_mipi_dsi_phy_write(dsi, DPHY_D3_SLEW_CTL_B0B, val);
	case 2:
		dw_mipi_dsi_phy_write(dsi, DPHY_D2_SLEW_CTL_90B, val);
	case 1:
		dw_mipi_dsi_phy_write(dsi, DPHY_D1_SLEW_CTL_70B, val);
		break;
	case 4:
		break;
	default:
		dev_err(dsi->dev, "lane num %d error when slew\n", dsi->lanes);
		break;
	}

	/* write cfgclkfreqrange[7:0] to grf register */
	cfgclk = DIV_ROUND_UP(clk_get_rate(dsi->phy_cfg_clk), USEC_PER_SEC);
	cfgclkrange = (cfgclk - 17) * 4;
	ret = regmap_read(dsi->grf_regmap, GRF_MIPITX_CFG0, &val);
	if (ret) {
		dev_err(dsi->dev, "Failed to read 2 grf regs.\n");
		return ret;
	}
	GRF_SET_CFGCLK(val, cfgclkrange);
	ret = regmap_write(dsi->grf_regmap, GRF_MIPITX_CFG0, val);
	if (ret) {
		dev_err(dsi->dev, "Failed to write 2 grf regs.\n");
		return ret;
	}

	/* PLL: m,n,vco_cntrl,cpbias_cntrl,gmp_cntrl,int_cntrl,prop_cntrl */
	vco_cntrl = mbps_to_vco(dsi->lane_mbps);
	#if 0
	dw_mipi_dsi_phy_write(dsi, DPHY_PLL_CTRL_179,
		PLL_M_L(dsi->feedback_div));
	dw_mipi_dsi_phy_write(dsi, DPHY_PLL_CTRL_17A,
		PLL_M_H(dsi->feedback_div));
	#endif
	dw_mipi_dsi_phy_write(dsi, DPHY_PLL_CTRL_17B,
		/*PLL_M_OVR_EN |*/ PLL_VCO_OVR_EN | PLL_VCO_CNT(vco_cntrl));
	#if 0
	dw_mipi_dsi_phy_write(dsi, DPHY_PLL_CTRL_178,
		PLL_N_OVR_EN | PLL_N(dsi->input_div));
	#endif
	dw_mipi_dsi_phy_write(dsi, DPHY_PLL_CTRL_15E, PLL_CPBIAS_CNTRL);
	dw_mipi_dsi_phy_write(dsi, DPHY_PLL_CTRL_162,
		PLL_GMP_CNTRL | PLL_INT_CNTRL);
	dw_mipi_dsi_phy_write(dsi, DPHY_PLL_CTRL_16E, PLL_PROP_CNTRL);

	if (dsi->lane_mbps < 450) {
	//if (dsi->lane_mbps <= 400) {
		/* TODO: may not need  */
		dw_mipi_dsi_phy_write(dsi, 0x59, 0x10);
		dw_mipi_dsi_phy_write(dsi, 0x60, 0x10);

		dw_mipi_dsi_phy_write(dsi, DPHY_COMM_BLK_CTRL_1AC,
			CB_CLKDIV_CLK_EN | CB_CAL_CHOP_EN | CB_ATB_SEL);
	}
#endif

	dsi_write(dsi, DSI_PHY_RSTZ, PHY_ENFORCEPLL | PHY_ENABLECLK |
		PHY_UNRSTZ | PHY_UNSHUTDOWNZ);

	ret = readl_poll_timeout(dsi->base + DSI_PHY_STATUS,
				val, val & LOCK, 1000, PHY_STATUS_TIMEOUT_US);
	if (ret < 0) {
		dev_err(dsi->dev, "failed to wait for phy lock state\n");
		goto phy_init_end;
	}

	ret = readl_poll_timeout(dsi->base + DSI_PHY_STATUS, val,
		val & STOP_STATE_CLK_LANE, 1000, PHY_STATUS_TIMEOUT_US);
	if (ret < 0)
		dev_err(dsi->dev, "fail to wait for phy clk lane stop state\n");

#ifdef DW_DPY_DEBUG
	for (i = 0; i < ARRAY_SIZE(reg_list); i++) {
		reg = reg_list[i];
		dev_info(dsi->dev, "dphy read reg %x:%x\n", reg,
			dw_mipi_dsi_phy_read(dsi, reg));
	}
#endif

phy_init_end:
	/*
	 * do not disable cfg clk
	 * clk_disable_unprepare(dsi->phy_cfg_clk);
	 */

	return ret;
}

static unsigned long dw_mipi_dsi_get_lane_bps(struct dw_mipi_dsi *dsi,
					struct drm_display_mode *mode)
{
	struct device *dev = dsi->dev;
	unsigned long max_lane_rate = hsfreq_map[ARRAY_SIZE(hsfreq_map) - 1].mbps;
	unsigned long lane_rate;
	unsigned int value;
	int bpp, lanes;
	u64 tmp;

	/* optional override of the desired bandwidth */
	if (!of_property_read_u32(dev->of_node, "ts,lane-rate", &value)) {
		//dev_info(dsi->dev, "override lane rate(dts ts,lane-rate %d)\n",value);
		dsi->lane_mbps = value;		/* TODO: */
		return value * USEC_PER_SEC;
	}

	bpp = mipi_dsi_pixel_format_to_bpp(dsi->format);
	if (bpp < 0)
		bpp = 24;

	lanes = dsi->lanes;
	tmp = (u64)mode->clock * 1000 * bpp;
	do_div(tmp, lanes);

	/* take 1 / 0.9, since mbps must big than bandwidth of RGB */
	tmp *= 10;
	do_div(tmp, 9);

	if (tmp > max_lane_rate)
		lane_rate = max_lane_rate;
	else
		lane_rate = tmp;

	dev_info(dsi->dev, "lane rate %ld\n", lane_rate);
	dsi->lane_mbps = lane_rate / USEC_PER_SEC;	/* TODO: */
	return lane_rate;
}

/*
 * TODO: IP defconfig: Fcfg_clk = 24M, N = 2, select M by hsfreq
 * this function need modify if use M/N
 */
#if 1
static void dw_mipi_dsi_set_pll(struct dw_mipi_dsi *dsi, unsigned long rate)
{
	unsigned long fin, fout;
	unsigned long fvco_min, fvco_max, best_freq = 984000000;
	u8 min_prediv, max_prediv;
	u8 _prediv, best_prediv = 2;
	u16 _fbdiv, best_fbdiv = 82;
	u32 min_delta = UINT_MAX;

	fin = clk_get_rate(dsi->ref_clk);
	fout = rate;

	/* 5Mhz < Fref / N < 40MHz, 80MHz < Fvco < 1500Mhz */
	min_prediv = DIV_ROUND_UP(fin, 40000000);
	max_prediv = fin / 5000000;
	fvco_min = 80000000;
	fvco_max = 1500000000;

	for (_prediv = min_prediv; _prediv <= max_prediv; _prediv++) {
		u64 tmp, _fout;
		u32 delta;

		/* Fvco = Fref * M / N */
		tmp = (u64)fout * _prediv;
		do_div(tmp, fin);
		_fbdiv = tmp;

		/*
		 * Due to the use of a "by 2 pre-scaler," the range of the
		 * feedback multiplication value M is limited to even division
		 * numbers, and m must be greater than 12, less than 1000.
		 */
		if (_fbdiv <= 12 || _fbdiv >= 1000)
			continue;

		if (_fbdiv % 2)
			++_fbdiv;

		_fout = (u64)_fbdiv * fin;
		do_div(_fout, _prediv);

		if (_fout < fvco_min || _fout > fvco_max)
			continue;

		delta = abs(fout - _fout);
		if (!delta) {
			best_prediv = _prediv;
			best_fbdiv = _fbdiv;
			best_freq = _fout;
			break;
		} else if (delta < min_delta) {
			best_prediv = _prediv;
			best_fbdiv = _fbdiv;
			best_freq = _fout;
			min_delta = delta;
		}
	}

	dsi->lane_mbps = best_freq / USEC_PER_SEC;
	dsi->input_div = best_prediv;
	dsi->feedback_div = best_fbdiv;
}
#endif

static int dw_mipi_dsi_host_attach(struct mipi_dsi_host *host,
				   struct mipi_dsi_device *device)
{
	struct dw_mipi_dsi *dsi = host_to_dsi(host);

	if (device->lanes > dsi->pdata->max_data_lanes) {
		dev_err(dsi->dev, "the number of data lanes(%u) is too many\n",
			device->lanes);
		return -EINVAL;
	}

	dsi->lanes = device->lanes;
	dsi->channel = device->channel;
	dsi->format = device->format;
	dsi->mode_flags = device->mode_flags;
	dsi->panel = of_drm_find_panel(device->dev.of_node);
	if (dsi->panel)
		return drm_panel_attach(dsi->panel, &dsi->connector);

	return -EINVAL;
}

static int dw_mipi_dsi_host_detach(struct mipi_dsi_host *host,
				   struct mipi_dsi_device *device)
{
	struct dw_mipi_dsi *dsi = host_to_dsi(host);

	drm_panel_detach(dsi->panel);

	return 0;
}

static void dw_mipi_message_config(struct dw_mipi_dsi *dsi,
				   const struct mipi_dsi_msg *msg)
{
	bool lpm = msg->flags & MIPI_DSI_MSG_USE_LPM;
	u32 val = 0;

	/*
	 * TODO dw drv improvements
	 * largest packet sizes during hfp or during vsa/vpb/vfp
	 * should be computed according to byte lane, lane number and only
	 * if sending lp cmds in high speed is enable (PHY_TXREQUESTCLKHS)
	 */
	dsi_write(dsi, DSI_DPI_LP_CMD_TIM, OUTVACT_LPCMD_TIME(16)
		  | INVACT_LPCMD_TIME(4));

	if (msg->flags & MIPI_DSI_MSG_REQ_ACK)
		val |= EN_ACK_RQST;
	if (lpm)
		val |= CMD_MODE_ALL_LP;

	dsi_write(dsi, DSI_LPCLK_CTRL, lpm ? 0 : PHY_TXREQUESTCLKHS);
	dsi_write(dsi, DSI_CMD_MODE_CFG, val);
}

static int dw_mipi_dsi_gen_pkt_hdr_write(struct dw_mipi_dsi *dsi, u32 hdr_val)
{
	int ret;
	u32 val, mask;

	ret = readl_poll_timeout(dsi->base + DSI_CMD_PKT_STATUS,
				 val, !(val & GEN_CMD_FULL), 1000,
				 CMD_PKT_STATUS_TIMEOUT_US);
	if (ret < 0) {
		dev_err(dsi->dev, "failed to get available command FIFO\n");
		return ret;
	}

	dsi_write(dsi, DSI_GEN_HDR, hdr_val);

	mask = GEN_CMD_EMPTY | GEN_PLD_W_EMPTY;
	ret = readl_poll_timeout(dsi->base + DSI_CMD_PKT_STATUS,
				 val, (val & mask) == mask,
				 1000, CMD_PKT_STATUS_TIMEOUT_US);
	if (ret < 0) {
		dev_err(dsi->dev, "failed to write command FIFO\n");
		return ret;
	}

	return 0;
}

static int dw_mipi_dsi_dcs_short_write(struct dw_mipi_dsi *dsi,
				       const struct mipi_dsi_msg *msg)
{
	const u8 *tx_buf = msg->tx_buf;
	u16 data = 0;
	u32 val;

	if (msg->tx_len > 0)
		data |= tx_buf[0];
	if (msg->tx_len > 1)
		data |= tx_buf[1] << 8;

	if (msg->tx_len > 2) {
		dev_err(dsi->dev, "too long tx buf length %zu for short write\n",
			msg->tx_len);
		return -EINVAL;
	}

	val = GEN_HDATA(data) | GEN_HTYPE(msg->type);
	return dw_mipi_dsi_gen_pkt_hdr_write(dsi, val);
}

static int dw_mipi_dsi_dcs_long_write(struct dw_mipi_dsi *dsi,
				      const struct mipi_dsi_msg *msg)
{
	const u8 *tx_buf = msg->tx_buf;
	int len = msg->tx_len, pld_data_bytes = sizeof(u32), ret;
	u32 hdr_val = GEN_HDATA(msg->tx_len) | GEN_HTYPE(msg->type);
	u32 remainder;
	u32 val;

	if (msg->tx_len < 3) {
		dev_err(dsi->dev, "wrong tx buf length %zu for long write\n",
			msg->tx_len);
		return -EINVAL;
	}

	while (DIV_ROUND_UP(len, pld_data_bytes)) {
		if (len < pld_data_bytes) {
			remainder = 0;
			memcpy(&remainder, tx_buf, len);
			dsi_write(dsi, DSI_GEN_PLD_DATA, remainder);
			len = 0;
		} else {
			memcpy(&remainder, tx_buf, pld_data_bytes);
			dsi_write(dsi, DSI_GEN_PLD_DATA, remainder);
			tx_buf += pld_data_bytes;
			len -= pld_data_bytes;
		}

		ret = readl_poll_timeout(dsi->base + DSI_CMD_PKT_STATUS,
					 val, !(val & GEN_PLD_W_FULL), 1000,
					 CMD_PKT_STATUS_TIMEOUT_US);
		if (ret < 0) {
			dev_err(dsi->dev,
				"failed to get available write payload FIFO\n");
			return ret;
		}
	}

	return dw_mipi_dsi_gen_pkt_hdr_write(dsi, hdr_val);
}

static int dw_mipi_dsi_read(struct dw_mipi_dsi *dsi,
			    const struct mipi_dsi_msg *msg)
{
	int i, j, ret, len = msg->rx_len;
	u8 *buf = msg->rx_buf;
	u32 val;

	ret = dw_mipi_dsi_dcs_short_write(dsi, msg);
	if (ret) {
		dev_err(dsi->dev, "DCS short wr err when reading.\n");
		return ret;
	}

	/* Wait end of the read operation */
	ret = readl_poll_timeout(dsi->base + DSI_CMD_PKT_STATUS,
				 val, !(val & GEN_RD_CMD_BUSY),
				 1000, CMD_PKT_STATUS_TIMEOUT_US);
	if (ret) {
		dev_err(dsi->dev, "Timeout during read operation\n");
		return ret;
	}

	for (i = 0; i < len; i += 4) {
		/* Read fifo must not be empty before all bytes are read */
		ret = readl_poll_timeout(dsi->base + DSI_CMD_PKT_STATUS,
					 val, !(val & GEN_PLD_R_EMPTY),
					 1000, CMD_PKT_STATUS_TIMEOUT_US);
		if (ret) {
			dev_err(dsi->dev, "Read payload FIFO is empty\n");
			return ret;
		}

		val = dsi_read(dsi, DSI_GEN_PLD_DATA);
		for (j = 0; j < 4 && j + i < len; j++)
			buf[i + j] = val >> (8 * j);
	}

	return ret;
}

static ssize_t dw_mipi_dsi_host_transfer(struct mipi_dsi_host *host,
					 const struct mipi_dsi_msg *msg)
{
	struct dw_mipi_dsi *dsi = host_to_dsi(host);
	int ret;

	dw_mipi_message_config(dsi, msg);

	switch (msg->type) {
	case MIPI_DSI_DCS_SHORT_WRITE:
	case MIPI_DSI_DCS_SHORT_WRITE_PARAM:
	case MIPI_DSI_SET_MAXIMUM_RETURN_PACKET_SIZE:
		ret = dw_mipi_dsi_dcs_short_write(dsi, msg);
		break;
	case MIPI_DSI_DCS_LONG_WRITE:
		ret = dw_mipi_dsi_dcs_long_write(dsi, msg);
		break;
	case MIPI_DSI_DCS_READ:
		ret = dw_mipi_dsi_read(dsi, msg);
		break;
	default:
		dev_err(dsi->dev, "unsupported message type 0x%02x\n",
			msg->type);
		ret = -EINVAL;
	}

	return ret;
}

static const struct mipi_dsi_host_ops dw_mipi_dsi_host_ops = {
	.attach = dw_mipi_dsi_host_attach,
	.detach = dw_mipi_dsi_host_detach,
	.transfer = dw_mipi_dsi_host_transfer,
};

static void dw_mipi_dsi_video_mode_config(struct dw_mipi_dsi *dsi)
{
	u32 val;

	val = ENABLE_LOW_POWER;

	if (dsi->mode_flags & MIPI_DSI_MODE_VIDEO_BURST)
		val |= VID_MODE_TYPE_BURST;
	else if (dsi->mode_flags & MIPI_DSI_MODE_VIDEO_SYNC_PULSE)
		val |= VID_MODE_TYPE_NON_BURST_SYNC_PULSES;
	else
		val |= VID_MODE_TYPE_NON_BURST_SYNC_EVENTS;

#ifdef CONFIG_DEBUG_FS
	if (dsi->vpg_defs.vpg) {
		val |= VID_MODE_VPG_ENABLE;
		val |= dsi->vpg_defs.vpg_horizontal ?
			VID_MODE_VPG_HORIZONTAL : 0;
		val |= dsi->vpg_defs.vpg_ber_pattern ? VID_MODE_VPG_MODE : 0;
	}
#endif /* CONFIG_DEBUG_FS */
	// zhc add 
	dsi_write(dsi, DSI_VID_MODE_CFG, val);
}

static void dw_mipi_dsi_set_mode(struct dw_mipi_dsi *dsi,
				 enum dw_mipi_dsi_mode mode)
{
	if (mode == DW_MIPI_DSI_CMD_MODE) {
		dsi_write(dsi, DSI_PWR_UP, RESET);
		dsi_write(dsi, DSI_MODE_CFG, ENABLE_CMD_MODE);
		dsi_write(dsi, DSI_PWR_UP, POWERUP);
	} else {
		dsi_write(dsi, DSI_PWR_UP, RESET);
		dsi_write(dsi, DSI_MODE_CFG, ENABLE_VIDEO_MODE);
		dw_mipi_dsi_video_mode_config(dsi);
		dsi_write(dsi, DSI_LPCLK_CTRL, PHY_TXREQUESTCLKHS);
		dsi_write(dsi, DSI_PWR_UP, POWERUP);
	}
}

static void dw_mipi_dsi_disable(struct dw_mipi_dsi *dsi)
{
	dsi_write(dsi, DSI_PWR_UP, RESET);
	dsi_write(dsi, DSI_PHY_RSTZ, PHY_RSTZ);
}

static void dw_mipi_dsi_init(struct dw_mipi_dsi *dsi)
{
	unsigned int esc_rate; /* in MHz */
	u32 esc_clk_division;

	esc_rate = 20; /* Default to 20MHz */

	/*
	 * We want :
	 *     (lane_mbps >> 3) / esc_clk_division < X
	 * which is:
	 *     (lane_mbps >> 3) / X > esc_clk_division
	 */
	esc_clk_division = (dsi->lane_mbps >> 3) / esc_rate + 1;

	dsi_write(dsi, DSI_PWR_UP, RESET);

	/*
	 * TODO dw drv improvements
	 * timeout clock division should be computed with the
	 * high speed transmission counter timeout and byte lane...
	 */
	dsi_write(dsi, DSI_CLKMGR_CFG, TO_CLK_DIVIDSION(10) |TX_ESC_CLK_DIVIDSION(esc_clk_division));
}

static void dw_mipi_dsi_dpi_config(struct dw_mipi_dsi *dsi,
				   struct drm_display_mode *mode)
{
	u32 val = 0, color = 0;
	int __maybe_unused ret;

	switch (dsi->format) {
	case MIPI_DSI_FMT_RGB888:
		color = DPI_COLOR_CODING_24BIT;
		break;
	case MIPI_DSI_FMT_RGB666:
		color = DPI_COLOR_CODING_18BIT_2 | EN18_LOOSELY;
		break;
	case MIPI_DSI_FMT_RGB666_PACKED:
		color = DPI_COLOR_CODING_18BIT_1;
		break;
	case MIPI_DSI_FMT_RGB565:
		color = DPI_COLOR_CODING_16BIT_1;
		break;
	}

	dsi_write(dsi, DSI_DPI_VCID, DPI_VID(dsi->channel));

	dsi_write(dsi, DSI_DPI_COLOR_CODING, color);

	// val = 4 | (4<<8) | (2<<16);
	// dsi_write(dsi, DSI_DBI_CFG, val);
#ifndef CONFIG_ARCH_TS_TX5336
	ret = regmap_read(dsi->grf_regmap, GRF_MIPITX_CFG0, &val);
	if (ret)
		dev_err(dsi->dev, "%s:Failed to read grf regs.\n", __func__);
	GRF_SET_COLMAP(val, color);
	ret = regmap_write(dsi->grf_regmap, GRF_MIPITX_CFG0, val);
	if (ret)
		dev_err(dsi->dev, "%s:Failed to write grf regs.\n", __func__);
#endif
	val = 0;
	#if 1	// TODO
	if (mode->flags & DRM_MODE_FLAG_NVSYNC)
		val |= VSYNC_ACTIVE_LOW;
	if (mode->flags & DRM_MODE_FLAG_NHSYNC)
		val |= HSYNC_ACTIVE_LOW;
	#endif
	dsi_write(dsi, DSI_DPI_CFG_POL, val);
	dsi_write(dsi, DSI_DPI_LP_CMD_TIM, OUTVACT_LPCMD_TIME(4)
	  | INVACT_LPCMD_TIME(4));
}

static void dw_mipi_dsi_packet_handler_config(struct dw_mipi_dsi *dsi)
{
	dsi_write(dsi, DSI_PCKHDL_CFG, EN_CRC_RX | EN_ECC_RX | EN_BTA);
}

static void dw_mipi_dsi_video_packet_config(struct dw_mipi_dsi *dsi,
					    struct drm_display_mode *mode)
{
	dsi_write(dsi, DSI_VID_PKT_SIZE, VID_PKT_SIZE(mode->hdisplay));
}

static void dw_mipi_dsi_command_mode_config(struct dw_mipi_dsi *dsi)
{
	dsi_write(dsi, DSI_TO_CNT_CFG, HSTX_TO_CNT(1000) | LPRX_TO_CNT(1000));
	dsi_write(dsi, DSI_BTA_TO_CNT, 0xd00);
	dsi_write(dsi, DSI_MODE_CFG, ENABLE_CMD_MODE);
}

/* Get lane byte clock cycles. */
static u32 dw_mipi_dsi_get_hcomponent_lbcc(struct dw_mipi_dsi *dsi,
					   struct drm_display_mode *mode,
					   u32 hcomponent)
{
	u32 frac, lbcc;

	lbcc = hcomponent * dsi->lane_mbps * MSEC_PER_SEC / 8;

	frac = lbcc % mode->clock;
	lbcc = lbcc / mode->clock;
	if (frac)
		lbcc++;

	return lbcc;
}

static void dw_mipi_dsi_line_timer_config(struct dw_mipi_dsi *dsi,
					  struct drm_display_mode *mode)
{
	u32 htotal, hsa, hbp, lbcc;

	htotal = mode->htotal;
	hsa = mode->hsync_end - mode->hsync_start;
	hbp = mode->htotal - mode->hsync_end;

	lbcc = dw_mipi_dsi_get_hcomponent_lbcc(dsi, mode, htotal);
	dsi_write(dsi, DSI_VID_HLINE_TIME, lbcc);

	lbcc = dw_mipi_dsi_get_hcomponent_lbcc(dsi, mode, hsa);
	dsi_write(dsi, DSI_VID_HSA_TIME, lbcc);

	lbcc = dw_mipi_dsi_get_hcomponent_lbcc(dsi, mode, hbp);
	dsi_write(dsi, DSI_VID_HBP_TIME, lbcc);
}

static void dw_mipi_dsi_vertical_timing_config(struct dw_mipi_dsi *dsi,
					       struct drm_display_mode *mode)
{
	u32 vactive, vsa, vfp, vbp;

	vactive = mode->vdisplay;
	vsa = mode->vsync_end - mode->vsync_start;
	vfp = mode->vsync_start - mode->vdisplay;
	vbp = mode->vtotal - mode->vsync_end;

	dsi_write(dsi, DSI_VID_VACTIVE_LINES, vactive);
	dsi_write(dsi, DSI_VID_VSA_LINES, vsa);
	dsi_write(dsi, DSI_VID_VFP_LINES, vfp);
	dsi_write(dsi, DSI_VID_VBP_LINES, vbp);
}

static int dw_mipi_dsi_phy_get_timing(unsigned int lane_mbps,
			struct dw_mipi_dsi_dphy_timing *timing)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(hstt_table); i++)
		if (lane_mbps < hstt_table[i].maxfreq)
			break;

	if (i == ARRAY_SIZE(hstt_table))
		i--;

	*timing = hstt_table[i].timing;

	return 0;
}


static void dw_mipi_dsi_dphy_timing_config(struct dw_mipi_dsi *dsi)
{
	struct dw_mipi_dsi_dphy_timing timing;
	u32 hw_version;
	int ret;

	ret = dw_mipi_dsi_phy_get_timing(dsi->lane_mbps, &timing);
	if (ret)
		DRM_DEV_ERROR(dsi->dev, "Retrieving phy timings failed\n");

	/*
	 * TODO dw drv improvements
	 * data & clock lane timers should be computed according to panel
	 * blankings and to the automatic clock lane control mode...
	 * note: DSI_PHY_TMR_CFG.MAX_RD_TIME should be in line with
	 * DSI_CMD_MODE_CFG.MAX_RD_PKT_SIZE_LP (see CMD_MODE_ALL_LP)
	 */
	hw_version = dsi_read(dsi, DSI_VERSION) & VERSION;

	if (hw_version >= HWVER_131) {
		dsi_write(dsi, DSI_PHY_TMR_CFG,
			PHY_HS2LP_TIME_V131(timing.data_hs2lp) |
			PHY_LP2HS_TIME_V131(timing.data_lp2hs));
		dsi_write(dsi, DSI_PHY_TMR_RD_CFG, MAX_RD_TIME_V131(10000));
	} else {
		dsi_write(dsi, DSI_PHY_TMR_CFG,
			PHY_HS2LP_TIME(timing.data_hs2lp) |
			PHY_LP2HS_TIME(timing.data_lp2hs) |
			MAX_RD_TIME(10000));
	}

	dsi_write(dsi, DSI_PHY_TMR_LPCLK_CFG,
		PHY_CLKHS2LP_TIME(timing.clk_hs2lp) |
		PHY_CLKLP2HS_TIME(timing.clk_lp2hs));
}

static void dw_mipi_dsi_dphy_interface_config(struct dw_mipi_dsi *dsi)
{
	dsi_write(dsi, DSI_PHY_IF_CFG, PHY_STOP_WAIT_TIME(0x20) |
		  N_LANES(dsi->lanes));
}

static void dw_mipi_dsi_clear_err(struct dw_mipi_dsi *dsi)
{
	dsi_read(dsi, DSI_INT_ST0);
	dsi_read(dsi, DSI_INT_ST1);
	dsi_write(dsi, DSI_INT_MSK0, 0);
	dsi_write(dsi, DSI_INT_MSK1, 0);
}

static void dw_mipi_dsi_encoder_disable(struct drm_encoder *encoder)
{
	struct dw_mipi_dsi *dsi = encoder_to_dsi(encoder);

	if (dsi->dpms_mode != DRM_MODE_DPMS_ON)
		return;
#if 0
	if (clk_prepare_enable(dsi->pclk)) {
		dev_err(dsi->dev, "%s: Failed to enable pclk\n", __func__);
		return;
	}
#endif
	drm_panel_disable(dsi->panel);

	dw_mipi_dsi_set_mode(dsi, DW_MIPI_DSI_CMD_MODE);
	drm_panel_unprepare(dsi->panel);

	dw_mipi_dsi_disable(dsi);
	pm_runtime_put(dsi->dev);
#if 0
	clk_disable_unprepare(dsi->pclk);
#endif
	dsi->dpms_mode = DRM_MODE_DPMS_OFF;
}

static void dw_mipi_dsi_encoder_enable(struct drm_encoder *encoder)
{
	struct dw_mipi_dsi *dsi = encoder_to_dsi(encoder);
	struct drm_display_mode *mode = &encoder->crtc->state->adjusted_mode;
	unsigned long rate;

	rate = dw_mipi_dsi_get_lane_bps(dsi, mode);

	/* TODO: not use M/N */
	dw_mipi_dsi_set_pll(dsi, rate);

	if (dsi->dpms_mode == DRM_MODE_DPMS_ON)
		return;

#if 0
	if (clk_prepare_enable(dsi->pclk)) {
		dev_err(dsi->dev, "%s: Failed to enable pclk\n", __func__);
		return;
	}
#endif

	pm_runtime_get_sync(dsi->dev);
	dw_mipi_dsi_init(dsi);
	dw_mipi_dsi_dpi_config(dsi, mode);
	dw_mipi_dsi_packet_handler_config(dsi);
	dw_mipi_dsi_video_mode_config(dsi);
	dw_mipi_dsi_video_packet_config(dsi, mode);
	dw_mipi_dsi_command_mode_config(dsi);
	dw_mipi_dsi_line_timer_config(dsi, mode);
	dw_mipi_dsi_vertical_timing_config(dsi, mode);
	dw_mipi_dsi_dphy_timing_config(dsi);
	dw_mipi_dsi_dphy_interface_config(dsi);
	dw_mipi_dsi_clear_err(dsi);

	dw_mipi_dsi_phy_init(dsi);
	dw_mipi_dsi_wait_for_two_frames(mode);

	dw_mipi_dsi_set_mode(dsi, DW_MIPI_DSI_CMD_MODE);
	if (drm_panel_prepare(dsi->panel))
		dev_err(dsi->dev, "failed to prepare panel\n");
	dw_mipi_dsi_set_mode(dsi, DW_MIPI_DSI_VID_MODE);
	drm_panel_enable(dsi->panel);
#if 0
	clk_disable_unprepare(dsi->pclk);
#endif
	dsi->dpms_mode = DRM_MODE_DPMS_ON;
#ifndef CONFIG_ARCH_TS_TX5336
	clk_disable_unprepare(dsi->grf_clk);
#endif
}

static int
dw_mipi_dsi_encoder_atomic_check(struct drm_encoder *encoder,
				 struct drm_crtc_state *crtc_state,
				 struct drm_connector_state *conn_state)
{
	struct dw_mipi_dsi *dsi = encoder_to_dsi(encoder);

	switch (dsi->format) {
	case MIPI_DSI_FMT_RGB888:
		break;
	case MIPI_DSI_FMT_RGB666:
		break;
	case MIPI_DSI_FMT_RGB565:
		break;
	default:
		WARN_ON(1);
		return -EINVAL;
	}

	return 0;
}

static const struct drm_encoder_helper_funcs
dw_mipi_dsi_encoder_helper_funcs = {
	.enable = dw_mipi_dsi_encoder_enable,
	.disable = dw_mipi_dsi_encoder_disable,
	.atomic_check = dw_mipi_dsi_encoder_atomic_check,
};

static const struct drm_encoder_funcs dw_mipi_dsi_encoder_funcs = {
	.destroy = drm_encoder_cleanup,
};

static int dw_mipi_dsi_connector_get_modes(struct drm_connector *connector)
{
	struct dw_mipi_dsi *dsi = con_to_dsi(connector);

	return drm_panel_get_modes(dsi->panel);
}

static struct drm_connector_helper_funcs dw_mipi_dsi_connector_helper_funcs = {
	.get_modes = dw_mipi_dsi_connector_get_modes,
};

static void dw_mipi_dsi_drm_connector_destroy(struct drm_connector *connector)
{
	drm_connector_unregister(connector);
	drm_connector_cleanup(connector);
}

static const struct drm_connector_funcs dw_mipi_dsi_atomic_connector_funcs = {
	.fill_modes = drm_helper_probe_single_connector_modes,
	.destroy = dw_mipi_dsi_drm_connector_destroy,
	.reset = drm_atomic_helper_connector_reset,
	.atomic_duplicate_state = drm_atomic_helper_connector_duplicate_state,
	.atomic_destroy_state = drm_atomic_helper_connector_destroy_state,
};

static int dw_mipi_dsi_register(struct drm_device *drm,
				struct dw_mipi_dsi *dsi)
{
	struct drm_encoder *encoder = &dsi->encoder;
	struct drm_connector *connector = &dsi->connector;
	struct device *dev = dsi->dev;
	int ret;

	encoder->possible_crtcs = drm_of_find_possible_crtcs(drm,
							     dev->of_node);
	/*
	 * If we failed to find the CRTC(s) which this encoder is
	 * supposed to be connected to, it's because the CRTC has
	 * not been registered yet.  Defer probing, and hope that
	 * the required CRTC is added later.
	 */
	if (encoder->possible_crtcs == 0)
		return -EPROBE_DEFER;

	drm_encoder_helper_add(&dsi->encoder,
			       &dw_mipi_dsi_encoder_helper_funcs);
	ret = drm_encoder_init(drm, &dsi->encoder, &dw_mipi_dsi_encoder_funcs,
			       DRM_MODE_ENCODER_DSI, NULL);
	if (ret) {
		dev_err(dev, "Failed to initialize encoder with drm\n");
		return ret;
	}

	drm_connector_helper_add(connector,
				 &dw_mipi_dsi_connector_helper_funcs);

	drm_connector_init(drm, &dsi->connector,
			   &dw_mipi_dsi_atomic_connector_funcs,
			   DRM_MODE_CONNECTOR_DSI);

	drm_mode_connector_attach_encoder(connector, encoder);

	return 0;
}

int ts_mipi_parse_dt(struct dw_mipi_dsi *dsi)
{
	struct device_node *np = dsi->dev->of_node;

#ifdef CONFIG_ARCH_TS_TX5336
	dsi->sys_regmap = syscon_regmap_lookup_by_phandle(np, "ts,sysctl");
	if (IS_ERR(dsi->sys_regmap)) {
		dev_err(dsi->dev, "Unable to get ts,sysctl\n");
		return PTR_ERR(dsi->sys_regmap);
	}
#else
	dsi->grf_regmap = syscon_regmap_lookup_by_phandle(np, "ts,grf");
	if (IS_ERR(dsi->grf_regmap)) {
		dev_err(dsi->dev, "Unable to get ts,grf\n");
		return PTR_ERR(dsi->grf_regmap);
	}
#endif
	return 0;
}

#ifdef CONFIG_DEBUG_FS

static int dw_mipi_dsi_debugfs_write(void *data, u64 val)
{
	struct debugfs_entries *vpg = data;
	struct dw_mipi_dsi *dsi;
	u32 mode_cfg;

	if (!vpg)
		return -ENODEV;

	dsi = vpg->dsi;

	*vpg->reg = (bool)val;

	mode_cfg = dsi_read(dsi, DSI_VID_MODE_CFG);

	if (*vpg->reg)
		mode_cfg |= vpg->mask;
	else
		mode_cfg &= ~vpg->mask;

	dsi_write(dsi, DSI_VID_MODE_CFG, mode_cfg);

	return 0;
}

static int dw_mipi_dsi_debugfs_show(void *data, u64 *val)
{
	struct debugfs_entries *vpg = data;

	if (!vpg)
		return -ENODEV;

	*val = *vpg->reg;

	return 0;
}

DEFINE_DEBUGFS_ATTRIBUTE(fops_x32, dw_mipi_dsi_debugfs_show,
			dw_mipi_dsi_debugfs_write, "%llu\n");

static void debugfs_create_files(void *data)
{
	struct dw_mipi_dsi *dsi = data;
	struct debugfs_entries debugfs[] = {
		REGISTER(vpg, VID_MODE_VPG_ENABLE, dsi),
		REGISTER(vpg_horizontal, VID_MODE_VPG_HORIZONTAL, dsi),
		REGISTER(vpg_ber_pattern, VID_MODE_VPG_MODE, dsi),
	};
	int i;

	dsi->debugfs_vpg = kmemdup(debugfs, sizeof(debugfs), GFP_KERNEL);
	if (!dsi->debugfs_vpg)
		return;

	for (i = 0; i < ARRAY_SIZE(debugfs); i++)
		debugfs_create_file(dsi->debugfs_vpg[i].name, 0644,
				    dsi->debugfs, &dsi->debugfs_vpg[i],
				    &fops_x32);
}

static void dw_mipi_dsi_debugfs_init(struct dw_mipi_dsi *dsi)
{
	dsi->debugfs = debugfs_create_dir(dev_name(dsi->dev), NULL);
	if (IS_ERR(dsi->debugfs)) {
		dev_err(dsi->dev, "failed to create debugfs root\n");
		return;
	}

	debugfs_create_files(dsi);
}

static void dw_mipi_dsi_debugfs_remove(struct dw_mipi_dsi *dsi)
{
	debugfs_remove_recursive(dsi->debugfs);
	kfree(dsi->debugfs_vpg);
}

#else

static void dw_mipi_dsi_debugfs_init(struct dw_mipi_dsi *dsi) { }
static void dw_mipi_dsi_debugfs_remove(struct dw_mipi_dsi *dsi) { }

#endif /* CONFIG_DEBUG_FS */


static struct dw_mipi_dsi_plat_data ts_mipi_dsi_drv_data = {
	.max_data_lanes = 4,
	.flags = DW_MIPI_NEEDS_PHY_CFG_CLK,
};


static const struct of_device_id dw_mipi_dsi_dt_ids[] = {
	{
	 .compatible = "ts,dw-mipi-dsi",
	 .data = &ts_mipi_dsi_drv_data,
	},
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, dw_mipi_dsi_dt_ids);

static int dw_mipi_dsi_bind(struct device *dev, struct device *master,
			    void *data)
{
	const struct of_device_id *of_id =
			of_match_device(dw_mipi_dsi_dt_ids, dev);
	const struct dw_mipi_dsi_plat_data *pdata = of_id->data;
	struct platform_device *pdev = to_platform_device(dev);
	struct  reset_control __maybe_unused *apb_rst;
	struct drm_device *drm = data;
	struct dw_mipi_dsi *dsi;
	struct resource *res;
#ifdef CONFIG_ARCH_TS_TX5336
    struct resource *res1;
#endif
	int ret;

	dsi = devm_kzalloc(dev, sizeof(*dsi), GFP_KERNEL);
	if (!dsi) {
		dev_err(dev, "fail to alloc dsi\n");
		return -ENOMEM;
	}

	dsi->dev = dev;
	dsi->pdata = pdata;
	dsi->dpms_mode = DRM_MODE_DPMS_OFF;

	ret = ts_mipi_parse_dt(dsi);
	if (ret) {
		dev_err(dev, "ts_mipi_parse_dt %d\n", ret);
		return ret;
	}
	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!res) {
		dev_err(dev, "platform_get_resource %d\n", ret);
		return -ENODEV;
	}
	dsi->base = devm_ioremap_resource(dev, res);
	if (IS_ERR(dsi->base)) {
		dev_err(dev, "devm_ioremap_resource %p\n", dsi->base);
		return PTR_ERR(dsi->base);
	}

#ifdef CONFIG_ARCH_TS_TX5336
	res1 = platform_get_resource(pdev, IORESOURCE_MEM, 1);
	if (!res1) {
		dev_err(dev, "platform_get_resource_1 %d\n", ret);
		return -ENODEV;
	}
	dsi->phy_base = devm_ioremap_resource(dev, res1);
	if (IS_ERR(dsi->phy_base)) {
		dev_err(dev, "devm_ioremap_resource %p\n", dsi->phy_base);
		return PTR_ERR(dsi->phy_base);
	}
	dsi->apb_clk = devm_clk_get(dev, "mipi_dsi_apb");
	if (IS_ERR(dsi->apb_clk)) {
		ret = PTR_ERR(dsi->apb_clk);
		dev_err(dev, "Unable to get apb_clk clock: %d\n", ret);
		return ret;
	}
	dsi->txphy_apb_clk = devm_clk_get(dev, "mipi_txphy_apb");
	if (IS_ERR(dsi->txphy_apb_clk)) {
		ret = PTR_ERR(dsi->txphy_apb_clk);
		dev_err(dev, "Unable to get txphy_apb_clk: %d\n", ret);
		return ret;
	}

	dsi->txclkesc_clk = devm_clk_get(dev, "mipi_txclkesc");
	if (IS_ERR(dsi->txclkesc_clk)) {
		ret = PTR_ERR(dsi->txclkesc_clk);
		dev_err(dev, "Unable to get txclkesc_clk: %d\n", ret);
		return ret;
	}

	dsi->ref_clk = devm_clk_get(dev, "mipi_ref");
	if (IS_ERR(dsi->ref_clk)) {
		ret = PTR_ERR(dsi->ref_clk);
		dev_err(dev, "Unable to get ref_clk: %d\n", ret);
		return ret;
	}
	dsi->ref_clk_rate = clk_get_rate(dsi->ref_clk);

	ret = clk_prepare_enable(dsi->ref_clk);
	if (ret) {
		dev_err(dev, "%s: Failed to enable ref_clk\n", __func__);
		return ret;
	}
	ret = clk_prepare_enable(dsi->txphy_apb_clk);
	if (ret) {
		dev_err(dev, "%s: Failed to enable txphy_apb_clk\n", __func__);
		return ret;
	}
	ret = clk_prepare_enable(dsi->apb_clk);
	if (ret) {
		dev_err(dev, "%s: Failed to enable apb_clk\n", __func__);
		return ret;
	}
	ret = clk_prepare_enable(dsi->txclkesc_clk);
	if (ret) {
		dev_err(dev, "%s: Failed to enable txclkesc_clk\n", __func__);
		return ret;
	}
#else
	dsi->pllref_clk = devm_clk_get(dev, "ref");
	if (IS_ERR(dsi->pllref_clk)) {
		ret = PTR_ERR(dsi->pllref_clk);
		dev_err(dev, "Unable to get pll reference clock: %d\n", ret);
		return ret;
	}

	dsi->pclk = devm_clk_get(dev, "pclk");
	if (IS_ERR(dsi->pclk)) {
		ret = PTR_ERR(dsi->pclk);
		dev_err(dev, "Unable to get pclk: %d\n", ret);
		return ret;
	}
	/*
	 * Note that the reset was not defined in the initial device tree, so
	 * we have to be prepared for it not being found.
	 */
	apb_rst = devm_reset_control_get(dev, "apb");
	if (IS_ERR(apb_rst)) {
		ret = PTR_ERR(apb_rst);
		if (ret == -ENOENT) {
			apb_rst = NULL;
		} else {
			dev_err(dev, "Unable to get reset control: %d\n", ret);
			return ret;
		}
	}

	if (apb_rst) {
		ret = clk_prepare_enable(dsi->pclk);
		if (ret) {
			dev_err(dev, "%s: Failed to enable pclk\n", __func__);
			return ret;
		}

		reset_control_assert(apb_rst);
		usleep_range(10, 20);
		reset_control_deassert(apb_rst);

		clk_disable_unprepare(dsi->pclk);
	}

	if (pdata->flags & DW_MIPI_NEEDS_PHY_CFG_CLK) {
		dsi->phy_cfg_clk = devm_clk_get(dev, "phy_cfg");
		if (IS_ERR(dsi->phy_cfg_clk)) {
			ret = PTR_ERR(dsi->phy_cfg_clk);
			dev_err(dev, "Unable to get phy_cfg_clk: %d\n", ret);
			return ret;
		}
	}

	if (pdata->flags & DW_MIPI_NEEDS_GRF_CLK) {
		dsi->grf_clk = devm_clk_get(dev, "grf");
		if (IS_ERR(dsi->grf_clk)) {
			ret = PTR_ERR(dsi->grf_clk);
			dev_err(dev, "Unable to get grf_clk: %d\n", ret);
			return ret;
		}
	}

	ret = clk_prepare_enable(dsi->pllref_clk);
	if (ret) {
		dev_err(dev, "%s: Failed to enable pllref_clk\n", __func__);
		return ret;
	}
#endif
	dw_mipi_dsi_debugfs_init(dsi);

	ret = dw_mipi_dsi_register(drm, dsi);
	if (ret) {
		dev_err(dev, "Failed to register mipi_dsi: %d\n", ret);
		goto err_pllref;
	}

	dsi->dsi_host.ops = &dw_mipi_dsi_host_ops;
	dsi->dsi_host.dev = dev;
	ret = mipi_dsi_host_register(&dsi->dsi_host);
	if (ret) {
		dev_err(dev, "Failed to register MIPI host: %d\n", ret);
		goto err_cleanup;
	}

	if (!dsi->panel) {
		ret = -EPROBE_DEFER;
		dev_err(dev, "dsi no panel\n");
		goto err_mipi_dsi_host;
	}

	pr_info("%s success.\n", __func__);
	dev_set_drvdata(dev, dsi);
	pm_runtime_enable(dev);
	return 0;

err_mipi_dsi_host:
	mipi_dsi_host_unregister(&dsi->dsi_host);
err_cleanup:
	drm_encoder_cleanup(&dsi->encoder);
	drm_connector_cleanup(&dsi->connector);
err_pllref:
	dw_mipi_dsi_debugfs_remove(dsi);
	clk_disable_unprepare(dsi->pllref_clk);
	return ret;
}

static void dw_mipi_dsi_unbind(struct device *dev, struct device *master,
			       void *data)
{
	struct dw_mipi_dsi *dsi = dev_get_drvdata(dev);

	dw_mipi_dsi_debugfs_remove(dsi);
	mipi_dsi_host_unregister(&dsi->dsi_host);
	pm_runtime_disable(dev);
	clk_disable_unprepare(dsi->pllref_clk);
}

static const struct component_ops dw_mipi_dsi_ops = {
	.bind	= dw_mipi_dsi_bind,
	.unbind	= dw_mipi_dsi_unbind,
};

static int dw_mipi_dsi_probe(struct platform_device *pdev)
{
	return component_add(&pdev->dev, &dw_mipi_dsi_ops);
}

static int dw_mipi_dsi_remove(struct platform_device *pdev)
{
	component_del(&pdev->dev, &dw_mipi_dsi_ops);
	return 0;
}

struct platform_driver dw_mipi_dsi_driver = {
	.probe		= dw_mipi_dsi_probe,
	.remove		= dw_mipi_dsi_remove,
	.driver		= {
		.of_match_table = dw_mipi_dsi_dt_ids,
		.name	= DRIVER_NAME,
	},
};

module_platform_driver(dw_mipi_dsi_driver);

MODULE_DESCRIPTION("ts dw dsi driver");
MODULE_AUTHOR("lwp");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:" DRIVER_NAME);

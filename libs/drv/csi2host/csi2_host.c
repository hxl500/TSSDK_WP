/***************************************************************************
 *                      COPYRIGHT NOTICE
 *               Copyright 2023 Tsingmicro, Inc.
 *                     All rights reserved.
 ***************************************************************************/

#include <linux/platform_device.h>
#include <linux/uaccess.h>
#include <linux/clk.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/iopoll.h>
#include <linux/irq.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/of_graph.h>
#include <linux/mfd/syscon.h>
#include <linux/regmap.h>
#include <uapi/linux/types.h>
#include <linux/cdev.h>
#include "csi2h_uapi.h"
#include "../sys/sys_drv.h"

#define CSI2_DEFAULT_MAX_MBPS 500
#define CSI2_MAX_MIPI_NUM   4
#define CSI2_MAX_IPI_NUM    2

struct mipi_csi2_ipi {
	uint8_t data_type;
	uint8_t vcid;
	/*timing*/
};

struct mipi_csi2_bus {
	int data_lanes;
	int clk_lanes;
	uint32_t mbps;
	uint32_t bpp;
};

struct mipirx_res {
	uint8_t mipi_idx;
	uint8_t ipi_num;
};

enum csi2_state {
	CSI2_STATE_STOP = 0,
	CSI2_STATE_START = 1
};

struct csi2_dev {//4 csi2host
	struct class *class;
	struct device *dev;
	struct device *chardev;
	struct cdev cdev;
	dev_t devno;

	struct clk *dphy_clk;
	struct clk *pclk;
	struct clk *pix_clk[CSI2_MAX_IPI_NUM];
	void __iomem *base;
	void __iomem *grf_base;
	void __iomem *phy_base;

	/* lock to protect all members below */
	struct mutex lock;

	struct mipi_csi2_bus bus;
	struct mipi_csi2_ipi ipi[CSI2_MAX_IPI_NUM];

	struct mipirx_res hw_res;

	enum csi2_state state;
};

#define DEVICE_NAME "mipi-rx"

/* CSI2 Register offsets */
#define CSI2_VERSION            0x000
#define CSI2_N_LANES            0x004
#define CSI2_RESETN             0x008
#define CSI2_ST_MAIN            0x00c
#define CSI2_ST_AP_MAIN         0x02c
#define CSI2_PHY_SHUTDOWNZ      0x040
#define CSI2_DPHY_RSTZ          0x044
#define CSI2_PHY_RX             0x048
#define PHY_RXCLKACTIVEHS       BIT(17)
#define CSI2_PHY_STOPSTATE      0x04c
#define PHY_STOPSTATECLK        BIT(16)
#define PHY_STOPSTATEDATA_BIT   0
#define PHY_STOPSTATEDATA(n)    BIT(PHY_STOPSTATEDATA_BIT + (n))
#define CSI2_PHY_TST_CTRL0      0x050
#define CSI2_PHY_TST_CTRL1      0x054
#define CSI2_IPI_SOFTRSTN      0x0a0

#define PHY_TESTCLK 1
#define PHY_TESTCLR 0
#define PHY_TESTEN  16
#define PHY_TESTOUT 8
#define PHY_TESTIN  0

#define CSI2_IPI0_MODE          0x080
#define IPI_ENABLE              BIT(24)
#define IPI_CUT_THROUGH         BIT(16)
#define IPI_COLOR_48BIT         (~BIT(8))
#define IPI_COLOR_16BIT         BIT(8)
#define IPI_CAMERA_MODE         (~BIT(0))
#define IPI_CONTRL_MODE         BIT(0)
#define CSI2_IPI0_VCID          0x084
#define CSI2_IPI0_DATA_TYPE     0x088
#define CSI2_IPI0_MEM_FLUSH     0x08c

#define CSI2_IPI0_HSA_TIME      0x090
#define CSI2_IPI0_HBP_TIME      0x094
#define CSI2_IPI0_HSD_TIME      0x098
#define CSI2_IPI_HLINE_TIME    0x09c
#define CSI2_IPI0_ADV_FEATURES  0x0ac
#define LINE_EVENT_SEL         BIT(16)
#define EN_VIDEO_PKT           BIT(17)
#define EN_EMBED               BIT(21)
#define IPI_SYNC_EVENT_MODE    BIT(24)
#define CSI2_IPI_VSA_LINES     0x0b0
#define CSI2_IPI_VBP_LINES     0x0b4
#define CSI2_IPI_VFP_LINES     0x0b8
#define CSI2_IPI_VACT_LINES    0x0bc

#define CSI2_VC_EXTERNSION     0x0c8
#define CSI2_PHY_CAL           0x0cc
#define CSI2_ST_PHY_FATAL      0x0e0
#define CSI2_ST_PKT_FATAL      0x0f0
#define CSI2_ST_PHY            0x110
#define CSI2_ST_LINE           0x130
#define CSI2_ST_IPI_FATAL      0x140
#define CSI2_SCRAMBLING        0x300

/*MIPI RX CFG registers*/
#define MIPI_RX_SYSREG_CFG0 0x7c
#define MIPI_RX_SYSREG_CFG1 0x80
#define MIPI_RX_SYSREG_CFG2 0xb4
#define MIPI_DPHY_EN            BIT(16)
#define MIPI_DPHY_HSFREQRANGE       8
#define MIPI_DPHY_CFGCLKFREQRANGE   0
#define MIPI_DPHY_FREQRANGE_MASK    0xff

/*MIPI RX DPHY registers*/
#define MIPI_DPHY_LANE_EN	0
#define MIPI_DPHY_CLK_LANE_MODE	0x80
#define MIPI_DPHY_CLK_LANE_DATARATE	0x160
#define MIPI_DPHY_DATA0_DATARATE	0x1e0
#define MIPI_DPHY_DATA1_DATARATE	0x260
#define MIPI_DPHY_DATA2_DATARATE	0x2e0
#define MIPI_DPHY_DATA3_DATARATE	0x360


#define MIPI_DPHY_CLK_LANE_MODE	0x80

#define MIPI_DPHY_FREQ_TABLE_IND_NUM	63
#define DPHY_COUNTER_FOR_DES_EN_BYPASS	1
#define DPHY_OSC_FREQ_TARGET_OVR_EN		1
#define DPHY_DESKEW_POLARITY_RW			0x20

#define LANE_EN_BIT_CLK 6
#define LANE_EN_BIT_DATA0 2
#define LANE_EN_BIT_DATA1 3
#define LANE_EN_BIT_DATA2 4
#define LANE_EN_BIT_DATA3 5

static void csi2_dphy_enable(struct csi2_dev *csi2, bool enable)
{
	if (enable) {
		writel(0x1, csi2->base + CSI2_PHY_SHUTDOWNZ);
		writel(0x1, csi2->base + CSI2_DPHY_RSTZ);
	} else {
		writel(0x0, csi2->base + CSI2_PHY_SHUTDOWNZ);
		writel(0x0, csi2->base + CSI2_DPHY_RSTZ);
	}
}

#if 0 //DT52 PHY
static inline void mipi_dphy_reg_write_mask(struct csi2_dev *csi2,
		uint32_t addr, uint32_t data, uint32_t shift, uint32_t mask)
{
	uint32_t tmp;

	tmp = readl(csi2->base + addr);
	tmp = tmp & (~(mask<<shift));
	tmp = tmp | ((data & mask)<<shift);
	writel(tmp ,csi2->base + addr);
}
static void mipi_dphy_write(struct csi2_dev *csi2, uint16_t reg, uint8_t data)
{
	mipi_dphy_reg_write_mask(csi2, CSI2_PHY_TST_CTRL1, 0, PHY_TESTEN, 0x1);
	mipi_dphy_reg_write_mask(csi2, CSI2_PHY_TST_CTRL0, 0, PHY_TESTCLK, 0x1);

	//address MSB  (write reg0)
	mipi_dphy_reg_write_mask(csi2, CSI2_PHY_TST_CTRL1, 1, PHY_TESTEN, 0x1);
	mipi_dphy_reg_write_mask(csi2, CSI2_PHY_TST_CTRL0, 1, PHY_TESTCLK, 0x1);
	mipi_dphy_reg_write_mask(csi2, CSI2_PHY_TST_CTRL1, 0, PHY_TESTIN, 0xff);
	mipi_dphy_reg_write_mask(csi2, CSI2_PHY_TST_CTRL0, 0, PHY_TESTCLK, 1);
	//reg0
	mipi_dphy_reg_write_mask(csi2, CSI2_PHY_TST_CTRL1, 0, PHY_TESTEN, 1);
	mipi_dphy_reg_write_mask(csi2, CSI2_PHY_TST_CTRL1, reg>>8, PHY_TESTIN, 0xff);
	mipi_dphy_reg_write_mask(csi2, CSI2_PHY_TST_CTRL0, 1, PHY_TESTCLK, 0x1);
	mipi_dphy_reg_write_mask(csi2, CSI2_PHY_TST_CTRL0, 0, PHY_TESTCLK, 0x1);

	//address LSB
	mipi_dphy_reg_write_mask(csi2, CSI2_PHY_TST_CTRL1, 1, PHY_TESTEN, 0x1);
	mipi_dphy_reg_write_mask(csi2, CSI2_PHY_TST_CTRL0, 1, PHY_TESTCLK, 0x1);
	mipi_dphy_reg_write_mask(csi2, CSI2_PHY_TST_CTRL1, reg, PHY_TESTIN, 0xff);
	mipi_dphy_reg_write_mask(csi2, CSI2_PHY_TST_CTRL0, 0, PHY_TESTCLK, 0x1);

	//data
	mipi_dphy_reg_write_mask(csi2, CSI2_PHY_TST_CTRL1, 0, PHY_TESTEN, 0x1);
	mipi_dphy_reg_write_mask(csi2, CSI2_PHY_TST_CTRL1, data, PHY_TESTIN, 0xff);
	mipi_dphy_reg_write_mask(csi2, CSI2_PHY_TST_CTRL0, 1, PHY_TESTCLK, 0x1);
	mipi_dphy_reg_write_mask(csi2, CSI2_PHY_TST_CTRL0, 0, PHY_TESTCLK, 0x1);
}

static uint8_t mipi_dphy_read(struct csi2_dev *csi2, uint16_t reg)
{
	uint32_t data;
	mipi_dphy_reg_write_mask(csi2, CSI2_PHY_TST_CTRL0, 0, PHY_TESTCLK, 0x1);
	mipi_dphy_reg_write_mask(csi2, CSI2_PHY_TST_CTRL1, 0, PHY_TESTEN, 0x1);

	//address MSB  (write reg0)
	mipi_dphy_reg_write_mask(csi2, CSI2_PHY_TST_CTRL1, 1, PHY_TESTEN, 0x1);
	mipi_dphy_reg_write_mask(csi2, CSI2_PHY_TST_CTRL0, 1, PHY_TESTCLK, 0x1);
	mipi_dphy_reg_write_mask(csi2, CSI2_PHY_TST_CTRL1, 0x00, PHY_TESTIN, 0xff);
	mipi_dphy_reg_write_mask(csi2, CSI2_PHY_TST_CTRL0, 0, PHY_TESTCLK, 0x1);
	//reg0
	mipi_dphy_reg_write_mask(csi2, CSI2_PHY_TST_CTRL1, 0, PHY_TESTEN, 1);
	mipi_dphy_reg_write_mask(csi2, CSI2_PHY_TST_CTRL1, reg>>8, PHY_TESTIN, 0xff);
	mipi_dphy_reg_write_mask(csi2, CSI2_PHY_TST_CTRL0, 1, PHY_TESTCLK, 0x1);
	mipi_dphy_reg_write_mask(csi2, CSI2_PHY_TST_CTRL0, 0, PHY_TESTCLK, 0x1);

	//address LSB
	mipi_dphy_reg_write_mask(csi2, CSI2_PHY_TST_CTRL1, 1, PHY_TESTEN, 0x1);
	mipi_dphy_reg_write_mask(csi2, CSI2_PHY_TST_CTRL0, 1, PHY_TESTCLK, 0x1);
	mipi_dphy_reg_write_mask(csi2, CSI2_PHY_TST_CTRL1, reg, PHY_TESTIN, 0xff);
	mipi_dphy_reg_write_mask(csi2, CSI2_PHY_TST_CTRL0, 0, PHY_TESTCLK, 0x1);

	writel(0x0, csi2->base + CSI2_PHY_TST_CTRL1);
	data = (readl(csi2->base + CSI2_PHY_TST_CTRL1) >> PHY_TESTOUT) & 0xff;
	return (uint8_t)data;
}

/*
*/
struct mipi_freq_table {
	uint16_t mbps;
	uint8_t  hs_freq_range;
	uint16_t osc_freq_target;
}mipi_hsfreq_map[] = {
	{80,	0b0000000, 438},
	{90,	0b0010000, 438},
	{100,	0b0100000, 438},
	{110,	0b0110000, 438},
	{120,	0b0000001, 438},
	{130,	0b0010001, 438},
	{140,	0b0100001, 438},
	{150,	0b0110001, 438},
	{160,	0b0000010, 438},
	{170,	0b0010010, 438},
	{180,	0b0100010, 438},
	{190,	0b0110010, 438},
	{205,	0b0000011, 438},
	{220,	0b0010011, 438},
	{235,	0b0100011, 438},
	{250,	0b0110011, 438},
	{275,	0b0000100, 438},
	{300,	0b0010100, 438},
	{325,	0b0100101, 438},
	{350,	0b0110101, 438},
	{400,	0b0000101, 438},
	{450,	0b0010110, 438},
	{500,	0b0100110, 438},
	{550,	0b0110111, 438},
	{600,	0b0000111, 438},
	{650,	0b0011000, 438},
	{700,	0b0101000, 438},
	{750,	0b0111001, 438},
	{800,	0b0001001, 438},
	{850,	0b0011001, 438},
	{900,	0b0101001, 438},
	{950,	0b0000000, 438},
	{1000,	0b0111010, 438},
	{1050,	0b0011010, 438},
	{1100,	0b0101010, 438},
	{1150,	0b0111011, 438},
	{1200,	0b0001011, 438},
	{1250,	0b0011011, 438},
	{1300,	0b0101011, 438},
	{1350,	0b0111100, 438},
	{1400,	0b0001100, 438},
	{1450,	0b0011100, 438},
	{1500,	0b0101100, 438},
	{1550,	0b0111101, 271},
	{1600,	0b0001101, 280},
	{1650,	0b0011101, 289},
	{1700,	0b0101110, 298},
	{1750,	0b0111110, 306},
	{1800,	0b0001110, 315},
	{1850,	0b0011110, 324},
	{1900,	0b0101111, 333},
	{1950,	0b0111111, 341},
	{2000,	0b0001111, 350},
	{2050,	0b1000000, 359},
	{2100,	0b1000001, 368},
	{2150,	0b1000010, 376},
	{2200,	0b1000011, 385},
	{2250,	0b1000100, 394},
	{2300,	0b1000101, 403},
	{2350,	0b1000110, 411},
	{2400,	0b1000111, 420},
	{2450,	0b1001000, 429},
	{2500,	0b1001001, 438},
};

static int mbps_to_hsfreqrange_sel(int mbps)
{
	int i;
	int closest;

	closest = abs(mbps - mipi_hsfreq_map[0].mbps);
	for (i = 1; i < ARRAY_SIZE(mipi_hsfreq_map); i++) {
		if (abs(mbps - mipi_hsfreq_map[i].mbps) > closest)
			return i-1;

		closest = abs(mbps - mipi_hsfreq_map[i].mbps);
	}

	return -EINVAL;
}

int mipi_get_cfgclkfreq(int fcfg_clk)
{
	if ((fcfg_clk < 17) || (fcfg_clk > 52)) {
		pr_err("input fcfg_clk %d err.", fcfg_clk);
		return -1;
	}

	return (fcfg_clk - 17) * 4;
}

int mipi_get_counter_for_des_en_cfg(int fcfg_clk)
{
	int cfg_clk_freq = 0;
	int counter_for_des_en_cfg = 0;

	cfg_clk_freq = mipi_get_cfgclkfreq(fcfg_clk);
	if (cfg_clk_freq < 0) {
		pr_err("get cfg clk freq err.\n");
		return -1;
	}

	if ((cfg_clk_freq >= 0) && (cfg_clk_freq <= 12)) {
		counter_for_des_en_cfg = 0;
	} else if ((cfg_clk_freq >= 13) && (cfg_clk_freq <= 38)) {
		counter_for_des_en_cfg = 1;
	} else if ((cfg_clk_freq >= 39) && (cfg_clk_freq <= 65)) {
		counter_for_des_en_cfg = 2;
	} else if ((cfg_clk_freq >= 66) && (cfg_clk_freq <= 92)) {
		counter_for_des_en_cfg = 3;
	} else if ((cfg_clk_freq >= 93) && (cfg_clk_freq <= 118)) {
		counter_for_des_en_cfg = 4;
	} else if ((cfg_clk_freq >= 119) && (cfg_clk_freq <= 140)) {
		counter_for_des_en_cfg = 5;
	} else {
		pr_err("cfg_clk_freq %d err.", cfg_clk_freq);
		counter_for_des_en_cfg = -1;
	}

	return counter_for_des_en_cfg;
}
#endif

static inline void csi2_dphy_preset(struct csi2_dev *csi2)
{
	/*set D-PHY to be active, high pulse */
	writel(1, csi2->base + CSI2_PHY_TST_CTRL0);
	udelay(100);
	writel(0, csi2->base + CSI2_PHY_TST_CTRL0);
}

static inline void dphy_set_lane_en(struct csi2_dev *csi2, u8 data_lanes, u8 en)
{
	uint32_t lane_en_regval;

	if (en) {
		if (data_lanes == 1) {
			lane_en_regval =
			BIT(LANE_EN_BIT_CLK)   |
			BIT(LANE_EN_BIT_DATA0);
		} else if (data_lanes == 2) {
			lane_en_regval =
			BIT(LANE_EN_BIT_CLK)   |
			BIT(LANE_EN_BIT_DATA0) |
			BIT(LANE_EN_BIT_DATA1);
		} else {//default 4 lane
			lane_en_regval =
			BIT(LANE_EN_BIT_CLK)   |
			BIT(LANE_EN_BIT_DATA0) |
			BIT(LANE_EN_BIT_DATA1) |
			BIT(LANE_EN_BIT_DATA2) |
			BIT(LANE_EN_BIT_DATA3);
		}
		lane_en_regval |= BIT(0);
	} else {
		lane_en_regval = 0;
	}

	writel(lane_en_regval, csi2->phy_base + MIPI_DPHY_LANE_EN);
}

static inline void dphy_set_clk_lane_mode(struct csi2_dev *csi2, u8 clk_lanes)
{
	uint32_t clk_lane_mode;
	
	if (clk_lanes == 2)//dual clk mode
		clk_lane_mode = BIT(6) | BIT(0);
	else
		clk_lane_mode = BIT(0);

	clk_lane_mode |= BIT(1) | BIT(2) | BIT(3) | BIT(4);

	writel(clk_lane_mode, csi2->phy_base + MIPI_DPHY_CLK_LANE_MODE);
}

static inline void dphy_set_lane_data_rate(struct csi2_dev *csi2)
{
	uint32_t data_rate = csi2->bus.mbps;
	uint32_t data_rate_regval = 0x16;

	if (data_rate > 700 && data_rate <= 800)
		data_rate_regval = 0x12;
	else if (data_rate > 800 && data_rate <= 1000)
		data_rate_regval = 0x16;
	else
		data_rate_regval = 0x16;

	writel(data_rate_regval, csi2->phy_base + MIPI_DPHY_CLK_LANE_DATARATE);
	writel(data_rate_regval, csi2->phy_base + MIPI_DPHY_DATA0_DATARATE);
	writel(data_rate_regval, csi2->phy_base + MIPI_DPHY_DATA1_DATARATE);
	writel(data_rate_regval, csi2->phy_base + MIPI_DPHY_DATA2_DATARATE);
	writel(data_rate_regval, csi2->phy_base + MIPI_DPHY_DATA3_DATARATE);
}

static inline void dphy_set_lane_cfg(struct csi2_dev *csi2)
{
	int data_lanes = csi2->bus.data_lanes;
	int clk_lanes = csi2->bus.clk_lanes;
	pr_info("data lanes:%d, clk lanes:%d\n", data_lanes, clk_lanes);

	dphy_set_lane_en(csi2, data_lanes, true);
	dphy_set_clk_lane_mode(csi2, clk_lanes);
	dphy_set_lane_data_rate(csi2);
}

static inline void dphy_vi_sys1_enable(struct csi2_dev *csi2, bool en)
{
	writel(en, csi2->grf_base);
}

static int csi2_dphy_init(struct csi2_dev *csi2)
{
#if 0 //DT52 PHY
	int sel;
	uint32_t value;
	int counter_for_des_en_cfg;
	int hsfreqrange = 0;
	int osc_freq_target = 0;
	int cfg_clk_freq = 0;

	unsigned long phy_clk_rate;
	uint32_t mipi_rx_cfg_reg;
	//uint32_t mipi_rx_mux_reg;
	//int mipi_idx = csi2->hw_res.mipi_idx;

	if (mipi_idx == 0)
		mipi_rx_cfg_reg = MIPI_RX_SYSREG_CFG0;
	else if (mipi_idx == 1)
		mipi_rx_cfg_reg = MIPI_RX_SYSREG_CFG1;
	else
		mipi_rx_cfg_reg = MIPI_RX_SYSREG_CFG2;

	sel = mbps_to_hsfreqrange_sel(csi2->bus.mbps);
	if (sel < 0)
		return -1;

	//phy_clk_rate = clk_get_rate(csi2->dphy_clk);
	//pr_info("cfg_clk: %luhz\n", phy_clk_rate);

/* start init dphy*/
	hsfreqrange = mipi_hsfreq_map[sel].hs_freq_range;
	osc_freq_target = mipi_hsfreq_map[sel].osc_freq_target;
	cfg_clk_freq = mipi_get_cfgclkfreq(phy_clk_rate/1000000);
	pr_info("hsfreq: %d, oscfreq: %d, cfg_clk_freq: %d\n", hsfreqrange, osc_freq_target, cfg_clk_freq);

	regmap_write(csi2->grf_base, mipi_rx_cfg_reg, MIPI_DPHY_EN |
			(cfg_clk_freq & MIPI_DPHY_CFGCLKFREQRANGE) |
			((hsfreqrange&MIPI_DPHY_FREQRANGE_MASK)<<MIPI_DPHY_HSFREQRANGE));

	/* set counter_for_des_en_cfg */
	mipi_dphy_write(csi2, 0xe5, DPHY_COUNTER_FOR_DES_EN_BYPASS);
	value = mipi_dphy_read(csi2, 0xe5);
	pr_debug("MIPI D-PHY REG 0xe5 = 0x%x", value);

	value = mipi_dphy_read(csi2, 0xe4);
	counter_for_des_en_cfg = mipi_get_counter_for_des_en_cfg(phy_clk_rate/1000000);
	if (counter_for_des_en_cfg < 0) {
		pr_err("get counter_for_des_en_cfg err.");
		return -2;
	}

	value = (value & 0xf) | (counter_for_des_en_cfg << 4);
	mipi_dphy_write(csi2, 0xe4, value);
	value = mipi_dphy_read(csi2, 0xe4);
	pr_debug("counter_for_des_en_cfg: %d\n", counter_for_des_en_cfg);
	pr_debug("MIPI D-PHY REG 0xe4 = 0x%x\n", value);

	/*global control over all lanes*/
    mipi_dphy_write(csi2, 0xe2, osc_freq_target & 0xff);
    value = mipi_dphy_read(csi2, 0xe2);
    pr_debug("MIPI D-PHY REG 0xe2 = 0x%x\n", value);

    mipi_dphy_write(csi2, 0xe3, osc_freq_target >> 8);
    value = mipi_dphy_read(csi2, 0xe3);
    pr_debug("MIPI D-PHY REG 0xe3 = 0x%x\n", value);

	/* enable counter_for_des_en */
    value = mipi_dphy_read(csi2, 0xe4);
    value |= DPHY_OSC_FREQ_TARGET_OVR_EN;
    mipi_dphy_write(csi2, 0xe4, value);

	switch (csi2->bus.data_lanes) {
	case 4:
		/* lane 3 */
		mipi_dphy_write(csi2, 0xc0a, osc_freq_target & 0xff);
		value = mipi_dphy_read(csi2, 0xc0a);
		pr_debug("MIPI D-PHY REG 0xc0a = 0x%x\n", value);
		mipi_dphy_write(csi2, 0xc0b, osc_freq_target >> 8);
		value = mipi_dphy_read(csi2, 0xc0b);
		pr_debug("MIPI D-PHY REG 0xc0b = 0x%x\n", value);
		value = mipi_dphy_read(csi2, 0xc0c);
		value |= DPHY_OSC_FREQ_TARGET_OVR_EN;
		mipi_dphy_write(csi2, 0xc0c, value);
	case 3:
		/*lane 2*/
		mipi_dphy_write(csi2, 0xa0a, osc_freq_target & 0xff);
		value = mipi_dphy_read(csi2, 0xa0a);
		pr_debug("MIPI D-PHY REG 0xa0a = 0x%x\n", value);
		mipi_dphy_write(csi2, 0xa0b, osc_freq_target >> 8);
		value = mipi_dphy_read(csi2, 0xa0b);
		pr_debug("MIPI D-PHY REG 0xa0b = 0x%x\n", value);
		value = mipi_dphy_read(csi2, 0xa0c);
		value |= DPHY_OSC_FREQ_TARGET_OVR_EN;
		mipi_dphy_write(csi2, 0xa0c, value);
	case 2:
		/* lane 1 */
		mipi_dphy_write(csi2, 0x80a, osc_freq_target & 0xff);
		value = mipi_dphy_read(csi2, 0x80a);
		pr_debug("MIPI D-PHY REG 0x80a = 0x%x\n", value);
		mipi_dphy_write(csi2, 0x80b, osc_freq_target >> 8);
		value = mipi_dphy_read(csi2, 0x80b);
		pr_debug("MIPI D-PHY REG 0x80b = 0x%x\n", value);
		value = mipi_dphy_read(csi2, 0x80c);
		value |= DPHY_OSC_FREQ_TARGET_OVR_EN;
		mipi_dphy_write(csi2, 0x80c, value);
	case 1:
		/*lane 0*/
		mipi_dphy_write(csi2, 0x60a, osc_freq_target & 0xff);
		value = mipi_dphy_read(csi2, 0x60a);
		pr_debug("MIPI D-PHY REG 0x60a = 0x%x\n", value);
		mipi_dphy_write(csi2, 0x60b, osc_freq_target >> 8);
		value = mipi_dphy_read(csi2, 0x60b);
		pr_debug("MIPI D-PHY REG 0x60b = 0x%x\n", value);
		value = mipi_dphy_read(csi2, 0x60c);
		value |= DPHY_OSC_FREQ_TARGET_OVR_EN;
		mipi_dphy_write(csi2, 0x60c, value);
	default:
		break;
	}
	value = mipi_dphy_read(csi2, 0x8);
	value |= DPHY_DESKEW_POLARITY_RW;
	mipi_dphy_write(csi2, 0x8, value);

	value = mipi_dphy_read(csi2, 0x1f);
	pr_debug("hsfreqrange: 0x%x\n", value);
#else
	uint32_t mipi_rx_cfg_reg;
	mipi_rx_cfg_reg = MIPI_RX_SYSREG_CFG1;

	dphy_set_lane_cfg(csi2);
	dphy_vi_sys1_enable(csi2, true);
#endif

	return 0;
}

#if 0
/* Waits for low-power LP-11 state on data and clock lanes. */
static void csi2_dphy_wait_stopstate(struct csi2_dev *csi2)
{
	uint32_t mask, reg;
	int ret;

	mask = PHY_STOPSTATECLK | ((BIT(csi2->bus.data_lanes)-1) << PHY_STOPSTATEDATA_BIT);

	ret = readl_poll_timeout(csi2->base + CSI2_PHY_STOPSTATE, reg,
			(reg & mask) == mask, 0, 20000);  //20ms
	if (ret) {
		dev_warn(csi2->dev, "LP-11 wait timeout, likely a sensor driver bug, expect capture failures.\n");
		dev_warn(csi2->dev, "phy_state = 0x%08x\n", reg);
	}
}

/* Wait for active clock on the clock lane. */
static int csi2_dphy_wait_clock_lane(struct csi2_dev *csi2)
{
	uint32_t reg;
	int ret;

	ret = readl_poll_timeout(csi2->base + CSI2_PHY_RX, reg,
			(reg & PHY_RXCLKACTIVEHS), 0, 20000);
	if (ret) {
		dev_warn(csi2->dev, "clock lane timeout, phy_state = 0x%08x\n", reg);
		return ret;
	}

	return 0;
}
#endif

static void csi2_ctrl_enable(struct csi2_dev *csi2, bool enable)
{
	if (enable) {
		writel(0x1, csi2->base + CSI2_RESETN);
	} else {
		writel(0x0, csi2->base + CSI2_RESETN);
	}
}

static inline void csi2_set_lanes(struct csi2_dev *csi2)
{
	int lanes = csi2->bus.data_lanes;
	writel(lanes - 1, csi2->base + CSI2_N_LANES);
}

static inline void csi2_disable_scrambling(struct csi2_dev *csi2)
{
	writel(0x0, csi2->base + CSI2_SCRAMBLING);
}

static void csi2_ipi_en(struct csi2_dev *csi2, bool en)
{
	uint32_t mode_reg = CSI2_IPI0_MODE;

	if (en)
		writel(IPI_CUT_THROUGH | IPI_COLOR_16BIT | IPI_CAMERA_MODE | IPI_ENABLE,
			csi2->base + mode_reg);
	else
		writel(0, csi2->base + mode_reg);
}

static int csi2_ipi_init(struct csi2_dev *csi2, int idx)
{
	uint32_t temp;
	uint32_t data_type_reg;
	uint32_t vcid_reg;
	uint32_t adv_feat_reg;
	uint32_t hsd_time_reg;
	uint32_t hsa_time_reg;
	uint32_t hbp_time_reg;
	uint32_t mode_reg;
	uint32_t mem_flush_reg;

	data_type_reg = CSI2_IPI0_DATA_TYPE;
	vcid_reg = CSI2_IPI0_VCID;
	adv_feat_reg = CSI2_IPI0_ADV_FEATURES;
	hsd_time_reg = CSI2_IPI0_HSD_TIME;
	hsa_time_reg = CSI2_IPI0_HSA_TIME;
	hbp_time_reg = CSI2_IPI0_HBP_TIME;
	mode_reg = CSI2_IPI0_MODE;
	mem_flush_reg = CSI2_IPI0_MEM_FLUSH;

	/*reset ipi*/
	temp = readl(csi2->base + CSI2_IPI_SOFTRSTN);
	writel(temp & (~BIT(idx)), csi2->base + CSI2_IPI_SOFTRSTN);
	writel(temp | BIT(idx), csi2->base + CSI2_IPI_SOFTRSTN);

	csi2->ipi[idx].data_type = 0x2a;
	csi2->ipi[idx].vcid = 0;
	/*data type/vcid/mem_flush_reg*/
	writel(csi2->ipi[idx].data_type, csi2->base + data_type_reg);
	writel(csi2->ipi[idx].vcid, csi2->base + vcid_reg);
	writel(IPI_SYNC_EVENT_MODE | EN_EMBED | EN_VIDEO_PKT | LINE_EVENT_SEL
		, csi2->base + adv_feat_reg);
	writel(0x100, csi2->base + mem_flush_reg);

	/*timing value is from experience*/
	writel(0x100, csi2->base + hsd_time_reg);//val from isp_app dump
	writel(0x2, csi2->base + hsa_time_reg);
	writel(0x10, csi2->base + hbp_time_reg);
	/*ipi mode*/
	writel(IPI_CUT_THROUGH | IPI_COLOR_16BIT | IPI_CAMERA_MODE | IPI_ENABLE,
			csi2->base + mode_reg);

	return 0;
}

static int csi2_start(struct csi2_dev *csi2)
{
	int ret;

	/*csi2 in reset state*/
	csi2_ctrl_enable(csi2, false);
	//udelay(100);
	//csi2_ctrl_enable(csi2, true);

	csi2_set_lanes(csi2);
	csi2_disable_scrambling(csi2);

	/*ipi0*/
	csi2_ipi_init(csi2, 0);
	//csi2_ipi_en(csi2, true);
#if 0
	clk_prepare_enable(csi2->pix_clk[0]);

	/*dphy*/
	ret = clk_prepare_enable(csi2->dphy_clk);
	if (ret)
		goto err_dis_clk;
#endif
	csi2_dphy_enable(csi2, false);
	csi2_dphy_preset(csi2);//phy test ctrl

	ret = csi2_dphy_init(csi2);
	csi2_dphy_enable(csi2, true);
	/*enable*/
	csi2_ctrl_enable(csi2, true);//maybe need to push front

	/*check if lane is in stop state*/
	//csi2_dphy_wait_stopstate(csi2);

	csi2->state = CSI2_STATE_START;
	pr_info("start ok\n");
	return 0;
#if 0
err_dis_phy_clk:
	clk_disable_unprepare(csi2->dphy_clk);
err_dis_clk:
	clk_disable_unprepare(csi2->pix_clk[0]);
#endif
	return ret;
}

static void csi2_stop(struct csi2_dev *csi2)
{
	if (csi2->state == CSI2_STATE_STOP)
		return;

	csi2_dphy_enable(csi2, false);
	csi2_ctrl_enable(csi2, false);
	csi2_ipi_en(csi2, false);
	//clk_disable_unprepare(csi2->pix_clk[0]);
	//clk_disable_unprepare(csi2->dphy_clk);

	csi2->state = CSI2_STATE_STOP;
	pr_info("stop\n");
}

typedef struct _mipi_data_type_s {
	const char *name;
	uint8_t data_type;
} mipi_data_type_t;

mipi_data_type_t data_type_table[] = {
	{"rgb565", 0x22},
	{"rgb666", 0x23},
	{"rgb888", 0x24},
	{"raw8",   0x2a},
	{"raw10",  0x2b},
	{"raw12",  0x2c},
	{"raw16",  0x2e},
	{"yuv420", 0x18},
	{"yuv420sp_nv12", 0x18},
	{"yuv420_3p", 0x18},
	{"yuv422", 0x1e},
	{"yuv422sp_nv12", 0x1e},
	{"yuv422_3p", 0x1e},
};

int csi2_get_data_type(const char *input_name, uint8_t *dt)
{
	int i;

	for (i=0; i<sizeof(data_type_table)/sizeof(data_type_table[0]); i++) {
		if (!strcmp(input_name, data_type_table[i].name)) {
			*dt = data_type_table[i].data_type;
			return 0;
		}
	}
	return -1;
}

static int csi2_parse_dt(struct csi2_dev *csi2)
{
	return 0;

#if 0 //do not use port0

	int ret;
	struct device_node *node = csi2->dev->of_node;
	struct device_node *ep;
	struct device_node *remote_ep;
	int data_lanes;
	uint32_t data_type;
	uint32_t lane_mbps;
	uint32_t vcid;
	const char *str;
	/*port0(sensor<->mipi), params: node, port-reg 0, endpoint-reg ignore*/
	ep = of_graph_get_endpoint_by_regs(node, 0, -1);
	if (!ep) {
		dev_err(csi2->dev, "failed to get src endpoint node\n");
		return -EINVAL;
	}
	remote_ep = of_graph_get_remote_endpoint(ep);//get remote mipi-tx
	if (!remote_ep) {
		dev_err(csi2->dev, "failed to get remote endpoint node\n");
		return -EINVAL;
	}

	ret = of_property_read_u32_index(remote_ep, "data-lanes", 0, &data_lanes);
	if (ret) {
		dev_err(csi2->dev, "failed to get attr data-lanes\n");
		return -EINVAL;
	}

	ret = of_property_read_u32_index(remote_ep, "lane-mbps", 0, &lane_mbps);
	if (ret) {
		dev_err(csi2->dev, "failed to get attr lane-rate\n");
		return -EINVAL;
	}
	/*port1(mipi<->ebd), ipi0*/
	ep = of_graph_get_endpoint_by_regs(node, 1, -1);//csi host i
	if (!ep) {
		dev_err(csi2->dev, "failed to get src endpoint node\n");
		return -EINVAL;
	}

	ret = of_property_read_string_index(ep, "data-type", 0, &str);
	if (ret) {
		dev_err(csi2->dev, "failed to get attr data-type\n");
		return -EINVAL;
	}
	ret = csi2_get_data_type(str, (uint8_t *)&data_type);
	if (ret) {
		dev_err(csi2->dev, "failed to find data type\n");
		return -EINVAL;
	}

	ret = of_property_read_u32_index(ep, "vcid", 0, &vcid);
	if (ret) {
		dev_err(csi2->dev, "failed to get attr vcid\n");
		return -EINVAL;
	}

	ret = of_property_read_u32_index(ep, "data-lanes", 0, &data_lanes);
	if (ret) {
		dev_err(csi2->dev, "failed to get attr data-lanes\n");
		return -EINVAL;
	}

	ret = of_property_read_u32_index(ep, "lane-mbps", 0, &lane_mbps);
	if (ret) {
		dev_err(csi2->dev, "failed to get attr lane-rate\n");
		return -EINVAL;
	}

#if 1
	csi2->bus.data_lanes = 4;
	csi2->bus.mbps = 500;
	csi2->ipi[0].data_type = 0;
	csi2->ipi[0].vcid = 0;
#else
	csi2->bus.data_lanes = data_lanes;
	csi2->bus.mbps = lane_mbps;
	csi2->ipi[0].data_type = data_type;
	csi2->ipi[0].vcid = vcid;
#endif
	dev_info(csi2->dev, "lane rate: %dmbps\n", csi2->bus.mbps);
	dev_info(csi2->dev, "data lanes: %u\n", csi2->bus.data_lanes);
	dev_info(csi2->dev, "bpp: %u\n", csi2->bus.bpp);
	dev_info(csi2->dev, "ipi%d data type: 0x%x\n", 0, csi2->ipi[0].data_type);
	dev_info(csi2->dev, "ipi%d vcid: %d\n", 0, csi2->ipi[0].vcid);

	csi2->grf_base = syscon_regmap_lookup_by_phandle(node, "ts,grf");
	if (IS_ERR(csi2->grf_base)) {
		dev_err(csi2->dev, "Unable to get ts,grf\n");
		return -EINVAL;
	}
#endif

	return 0;
}

static int csi2_get_mipi_clk(struct csi2_dev *csi2)
{
	struct device_node *mipi_np = csi2->dev->of_node;
	struct clk *tt_clk;
	char clk_name[32];
	int mipi_idx;
	int ipi_idx;

	return 0;
	mipi_idx = csi2->hw_res.mipi_idx;
	/*mipi rx0 clk*/
	sprintf(clk_name, "rx%d_pclk", mipi_idx);
	tt_clk = of_clk_get_by_name(mipi_np, clk_name);
	if (IS_ERR(tt_clk)) {
		pr_err("clock %s not found.\n", clk_name);
		return PTR_ERR(tt_clk);
	}
	csi2->pclk = tt_clk;

	for (ipi_idx=0; ipi_idx < csi2->hw_res.ipi_num; ipi_idx++) {
		sprintf(clk_name, "rx%d_pixclk%d", mipi_idx, ipi_idx);
		tt_clk = of_clk_get_by_name(mipi_np, clk_name);
		if (IS_ERR(tt_clk)) {
			pr_err("clock %s not found.\n", clk_name);
			return PTR_ERR(tt_clk);
		}
		csi2->pix_clk[ipi_idx] = tt_clk;
	}

	sprintf(clk_name, "rxphy%d_cfgclk", mipi_idx);
	tt_clk = of_clk_get_by_name(mipi_np, clk_name);
	if (IS_ERR(tt_clk)) {
		pr_err("clock %s not found.\n", clk_name);
		return PTR_ERR(tt_clk);
	}
	csi2->dphy_clk = tt_clk;
	return 0;
}

static int csi2_mipi_clk_enable(struct csi2_dev *csi2)
{
	//clk_prepare_enable(csi2->pclk);
#if 0
	int ipi_idx;
	clk_prepare_enable(csi2->dphy_clk);

	for(ipi_idx = 0; ipi_idx < csi2->hw_res.ipi_num; ipi_idx++) {
		if (csi2->pix_clk[ipi_idx])
			clk_prepare_enable(csi2->pix_clk[ipi_idx]);
	}
#endif
	return 0;
}

static int csi2_mipi_clk_disable(struct csi2_dev *csi2)
{
#if 0
	int ipi_idx;
		for(ipi_idx = 0; ipi_idx < csi2->hw_res.ipi_num; ipi_idx++) {
			if (csi2->pix_clk[ipi_idx])
				clk_disable_unprepare(csi2->pix_clk[ipi_idx]);
		}
		clk_disable_unprepare(csi2->dphy_clk);
#endif
	//clk_disable_unprepare(csi2->pclk);
	return 0;
}

static int csi2_bit_rate_cfg(struct csi2_dev *csi2, unsigned long arg)
{
	uint32_t bit_rate_mbps;

	if (get_user(bit_rate_mbps, (int __user *) arg))
		return -1;

	csi2->bus.mbps = bit_rate_mbps;
	return 0;
}
static int csi2_init_cfg(struct csi2_dev *csi2, unsigned long arg)
{
	lane_cfg cfg;
	int ret;

	ret = copy_from_user((lane_cfg *)&cfg, (int __user *) arg,
				sizeof(cfg));
	if (ret) {
		pr_err("csi2_init_cfg fail,ret:%d\n", ret);
		ret = -1;
		return ret;
	}

	csi2->bus.mbps = cfg.data_rate;
	csi2->bus.data_lanes = cfg.data_lane_num;
	csi2->bus.clk_lanes = cfg.clk_lane_num;
	csi2->bus.bpp = cfg.bpp;

	return 0;
}

static int csi2_stream(struct csi2_dev *csi2, unsigned long arg)
{
	int ret;
	int enable = 0;

	if (get_user(enable, (int __user *) arg)) {
		ret = -1; goto err;
	}

	mutex_lock(&csi2->lock);
	if (enable) {
		ret = csi2_start(csi2);
		if (ret) {
			ret = -2; goto err;
		}
	} else
		csi2_stop(csi2);

	mutex_unlock(&csi2->lock);
	return 0;
err:
	mutex_unlock(&csi2->lock);
	pr_err("%s err %d\n", __func__, ret);
	return -ret;
}

static int csi2_open(struct inode *inode, struct file *file)
{
	struct csi2_dev *csi2;

	csi2 = container_of(inode->i_cdev, struct csi2_dev, cdev);
	file->private_data = csi2;

	pr_info("csi2host open.\n");
	return 0;
}

static int csi2_close(struct inode *inode, struct file *file)
{
	struct csi2_dev *csi2;

	csi2 = container_of(inode->i_cdev, struct csi2_dev, cdev);
	file->private_data = csi2;
	if (csi2->state != CSI2_STATE_STOP)
		csi2_stop(csi2);

	pr_info("csi2host close.\n");
	return 0;
}

static ssize_t csi2_write(struct file *file, const char __user *buf,
		size_t count, loff_t *ppos)
{
	return 0;
}

static ssize_t csi2_read(struct file *file, char __user *buf, size_t size,
		loff_t *ppos)
{
	return 0;
}

static long csi2_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	int ret = 0;
	struct csi2_dev *csi2;

	csi2 = file->private_data;
	pr_info("ioctl data_lanes: %d\n", csi2->bus.data_lanes);

	switch (cmd) {
	case MIPI_RX_IOC_INIT:
		ret = csi2_init_cfg(csi2, arg);
		if (ret)
			return ret;
		break;
	case MIPI_RX_IOC_STREAM:
		ret = csi2_stream(csi2, arg);
		if (ret)
			return ret;
		break;
	case MIPI_RX_IOC_BIT_RATE:
		ret = csi2_bit_rate_cfg(csi2, arg);
		if (ret)
			return ret;
		break;
	default:
		break;
	}

	return 0;
}

static const struct file_operations csi2_fops = {
	.owner = THIS_MODULE,
	.open = csi2_open,
	.release = csi2_close,
	.write = csi2_write,
	.read = csi2_read,
	.unlocked_ioctl = csi2_ioctl,
	.compat_ioctl = csi2_ioctl,
};

static int csi2_device_node_init(struct csi2_dev *csi2)
{
	int ret = 0;
	char dev_name[32];

	sprintf(dev_name, "mipi-rx%d", csi2->hw_res.mipi_idx);
	ret = alloc_chrdev_region(&csi2->devno, 0, MAX_CSI2H_DEVICE_NUM, dev_name);
	if (ret < 0) {
		pr_err("Error %d while alloc chrdev csi2\n", ret);
		goto err_req_cdev;
	}

	cdev_init(&csi2->cdev, &csi2_fops);
	csi2->cdev.owner = THIS_MODULE;
	ret = cdev_add(&csi2->cdev, csi2->devno, MAX_CSI2H_DEVICE_NUM);
	if (ret) {
		pr_err("Error %d while adding csi2 cdev\n", ret);
		goto err_req_cdev;
	}

	if (!sys_class) {
		pr_err("csi2 check vps class null\n");
		goto err_cr_cls;
	}
	csi2->class = sys_class;

	csi2->chardev = device_create(csi2->class, NULL,
			MKDEV(MAJOR(csi2->devno), 0), NULL, dev_name);
	if (IS_ERR(csi2->dev)) {
		ret = -EINVAL;
		pr_err("csi2 device create fail\n");
		goto err_dev;
	}
	dev_set_drvdata(csi2->chardev, csi2);

	return ret;

err_dev:
	device_destroy(csi2->class, MKDEV(MAJOR(csi2->devno), 0));
err_cr_cls:
	cdev_del(&csi2->cdev);
err_req_cdev:
	unregister_chrdev_region(csi2->devno, MAX_CSI2H_DEVICE_NUM);
	return ret;
}

static int csi2_probe(struct platform_device *pdev)
{
	struct csi2_dev *csi2;
	struct resource *res;
	int ret;

	csi2 = devm_kzalloc(&pdev->dev, sizeof(*csi2), GFP_KERNEL);
	if (!csi2)
		return -ENOMEM;

	csi2->dev = &pdev->dev;
	memcpy(&csi2->hw_res, (void *)of_device_get_match_data(csi2->dev),
			sizeof(struct mipirx_res));

	dev_err(csi2->dev, "parse device tree ok, csi [%d], ipi_num:%d\n"
		, csi2->hw_res.mipi_idx, csi2->hw_res.ipi_num);

	ret = csi2_parse_dt(csi2);
	if (ret) {
		dev_err(csi2->dev, "parse device tree fail\n");
		goto _err_free;
	}

	ret = csi2_get_mipi_clk(csi2);
	if (ret) {
		dev_err(csi2->dev, "get clk fail\n");
		goto _err_unmap1;
	}

	res = platform_get_resource_byname(pdev, IORESOURCE_MEM, "csi");
	if (!res) {
		dev_err(csi2->dev, "failed to get platform csi resources\n");
		goto _err_free;
	}
	csi2->base = devm_ioremap_nocache(csi2->dev, res->start, resource_size(res));
	if (!csi2->base) {
		dev_err(csi2->dev, "failed to map CSI-2 registers\n");
		goto _err_free;
	}

	res = platform_get_resource_byname(pdev, IORESOURCE_MEM, "sysreg_vi_cfg1");
	if (!res) {
		dev_err(csi2->dev, "failed to get platform sysreg1 resources\n");
		goto _err_unmap0;
	}
	csi2->grf_base = devm_ioremap_nocache(csi2->dev, res->start, resource_size(res));
	if (!csi2->grf_base) {
		dev_err(csi2->dev, "failed to map sysreg1 registers\n");
		goto _err_unmap0;
	}

	res = platform_get_resource_byname(pdev, IORESOURCE_MEM, "dphy");
	if (!res) {
		dev_err(csi2->dev, "failed to get platform dphy resources\n");
		goto _err_unmap1;
	}
	csi2->phy_base = devm_ioremap_nocache(csi2->dev, res->start, resource_size(res));
	if (!csi2->phy_base) {
		dev_err(csi2->dev, "failed to map dphy registers\n");
		goto _err_unmap1;
	}

	ret = csi2_device_node_init(csi2);
	if (ret) {
		dev_err(csi2->dev, "device node init fail\n");
		goto _err_unmap2;
	}

	csi2_mipi_clk_enable(csi2);

	mutex_init(&csi2->lock);
	platform_set_drvdata(pdev, csi2);

	dev_info(csi2->dev, "init ok\n");
	return 0;

_err_unmap2:
	devm_iounmap(csi2->dev, csi2->phy_base);
_err_unmap1:
	devm_iounmap(csi2->dev, csi2->grf_base);
_err_unmap0:
	devm_iounmap(csi2->dev, csi2->base);

_err_free:
	devm_kfree(&pdev->dev, csi2);
	return ret;
}

static int csi2_remove(struct platform_device *pdev)
{
	struct csi2_dev *csi2 = platform_get_drvdata(pdev);

	device_destroy(csi2->class, MKDEV(MAJOR(csi2->devno), 0));
	cdev_del(&csi2->cdev);
	unregister_chrdev_region(csi2->devno, MAX_CSI2H_DEVICE_NUM);

	if (csi2->state != CSI2_STATE_STOP)
		csi2_stop(csi2);

	csi2_mipi_clk_disable(csi2);
	mutex_destroy(&csi2->lock);

	devm_iounmap(&pdev->dev, csi2->grf_base);
	devm_iounmap(&pdev->dev, csi2->base);
	devm_kfree(&pdev->dev, csi2);
	pr_info("remove\n");
	return 0;
}

struct mipirx_res mipirx0_res = {
	.mipi_idx = 0,
	.ipi_num = 2,
};
struct mipirx_res mipirx1_res = {
	.mipi_idx = 1,
	.ipi_num = 1,
};
struct mipirx_res mipirx2_res = {
	.mipi_idx = 2,
	.ipi_num = 2,
};
struct mipirx_res mipirx3_res = {
	.mipi_idx = 3,
	.ipi_num = 1,
};

static const struct of_device_id csi2_dt_ids[] = {
	{
		.compatible = "ts,mipi-csi2-rx0",
		.data = &mipirx0_res,
	},
	{
		.compatible = "ts,mipi-csi2-rx1",
		.data = &mipirx1_res,
	},
	{
		.compatible = "ts,mipi-csi2-rx2",
		.data = &mipirx2_res,
	},
	{
		.compatible = "ts,mipi-csi2-rx3",
		.data = &mipirx3_res,
	},
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, csi2_dt_ids);

static struct platform_driver csi2_driver = {
	.driver = {
		.name = DEVICE_NAME,
		.of_match_table = csi2_dt_ids,
	},
	.probe = csi2_probe,
	.remove = csi2_remove,
};

module_platform_driver(csi2_driver);

MODULE_DESCRIPTION("TS MIPI CSI-2 Receiver driver");
MODULE_LICENSE("GPL v2");

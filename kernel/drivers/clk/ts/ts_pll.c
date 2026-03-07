// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2022-2022 Tsingmicro, Inc.
 */
#include <linux/io.h>
#include <linux/slab.h>
#include <linux/bitfield.h>
#include <linux/clk-provider.h>
#include <linux/delay.h>
#include <linux/err.h>
#ifndef CONFIG_ARM64
#include <asm/div64.h>
#endif

#include "ts_pll.h"

#define CLK_TS_PLL "ts_clk_pll"

#define TS_PLL_CFG0			0x0
#define TS_PLL_CFG1			0x4
#define TS_PLL_CFG2			0x8
#define TS_PLL_EN			0xC

union ts_pll_cfg0_un {
	u32 data;
	struct {
		u32 refdiv:6;      /* [0:5] */
		u32 reserved0:2;   /* [6:7] */
		u32 fbdiv:12;      /* [8:19] */
		u32 posdiv1:3;     /* [20:22] */
		u32 reserved1:1;   /* [23] */
		u32 posdiv2:3;     /* [24:26] */
		u32 reserved2:5;   /* [27:31] */
	};
};

union ts_pll_cfg1_un {
	u32 data;
	struct {
		u32 frac:24;       /* [0:23] */
		u32 dsmpd:1;       /* [24] */
		u32 dacpd:1;       /* [25] */
		u32 vcopd:1;       /* [26] */
		u32 reserved0:4;   /* [27:30] */
		u32 bypass:1;      /* [31] */
	};
};

union ts_pll_cfg2_un {
	u32 data;
	struct {
		u32 fourphasepd:1; /* [0] */
		u32 postdivpd:1;   /* [1] */
		u32 reserved0:30;  /* [2:31] */
	} loosely;
	struct {
		u32 powerdown:1;   /* [0] */
		u32 fourphasepd:1; /* [1] */
		u32 postdivpd:1;   /* [2] */
		u32 reserved0:29;  /* [3:31] */
	} compact;
};

union ts_pll_en_un {
	u32 data;
	struct {
		u32 powerdown:1;   /* [0] */
		u32 reserved0:31;  /* [1:31] */
	} loosely;
};

struct ts_pll_configs {
	union ts_pll_cfg0_un cfg0;
	union ts_pll_cfg1_un cfg1;
	union ts_pll_cfg2_un cfg2;
	union ts_pll_en_un en;
};

static int ts_pll_enable(struct clk_hw *hw);
static void ts_pll_disable(struct clk_hw *hw);

int ts_pll_calc_config(struct ts_pll *pll, unsigned long rate,
	unsigned long rate_in, struct ts_pll_configs *best)
{
	union ts_pll_cfg0_un pll_cfg0;
	union ts_pll_cfg1_un pll_cfg1;
	union ts_pll_cfg2_un pll_cfg2;
	u64 tmp;
	u32 fbdiv;
	u32 frac;
#ifndef CONFIG_ARM64
	u32 mod;
#endif

	/* to do */
	pr_info("Calculating parameters with rate=%lu and rate_in=%lu\n",
			rate, rate_in);

	pll_cfg0.data = readl(pll->reg + TS_PLL_CFG0);
	pll_cfg1.data = readl(pll->reg + TS_PLL_CFG1);
	pll_cfg2.data = readl(pll->reg + TS_PLL_CFG2);

	pll_cfg0.refdiv = 1;
	pll_cfg0.posdiv1 = 1;
	pll_cfg0.posdiv2 = 1;
	fbdiv = rate / rate_in;
	tmp = rate % rate_in;
	tmp <<= 24;

#ifndef CONFIG_ARM64
	mod = do_div(tmp, rate_in);
#else
	tmp /= rate_in;
#endif

	frac = (u32)tmp;

	pll_cfg0.fbdiv = fbdiv;
	pll_cfg1.data = readl(pll->reg + TS_PLL_CFG1);
	pll_cfg1.frac = frac;
	pll_cfg2.data = readl(pll->reg + TS_PLL_CFG2);

	best->cfg0.data = pll_cfg0.data;
	best->cfg1.data = pll_cfg1.data;
	best->cfg2.data = pll_cfg2.data;

	pr_info("cfg to set: 0x%x, 0x%x, 0x%x\n",
		best->cfg0.data, best->cfg1.data, best->cfg2.data);

	return 0;
}

static int ts_pll_set_rate(struct clk_hw *hw, unsigned long rate,
				    unsigned long parent_rate)
{
	int err;
	long long rate_in = parent_rate;
	struct ts_pll_configs config = {};
	struct ts_pll *pll = to_ts_pll(hw);

	if (rate_in < 0)
		return rate_in;

	err = ts_pll_calc_config(pll, rate, rate_in, &config);
	if (err)
		return err;

	/*
	 * Don't use clk_disable as it might not actually disable the pll due to
	 * refcounting
	 */
	ts_pll_disable(hw);

	writel(config.cfg0.data, pll->reg + TS_PLL_CFG0);
	writel(config.cfg1.data, pll->reg + TS_PLL_CFG1);
	writel(config.cfg2.data, pll->reg + TS_PLL_CFG2);

	err = ts_pll_enable(hw);
	if (err)
		return err;

	return clk_hw_get_rate(hw);
}

/* pll freq = osc_clk * (fbdiv + (frac>>24)) / (refdiv * postdiv1 * postdiv2) */
static ulong ts_pll_get_rate(struct clk_hw *hw, unsigned long parent_rate)
{
	long long rate_in = parent_rate;
	struct ts_pll *pll = to_ts_pll(hw);
	ulong freq;
	u32 fbdiv, refdiv, pos_div1, pos_div2, frac, div;
	u64 tmp;
#ifndef CONFIG_ARM64
	u32 mod;
#endif
	union ts_pll_cfg0_un pll_cfg0;
	union ts_pll_cfg1_un pll_cfg1;

	pll_cfg1.data = readl(pll->reg + TS_PLL_CFG1);
	if ((rate_in < 0) || (pll_cfg1.bypass == 1)) {
		freq = rate_in;
	} else {
		pll_cfg0.data = readl(pll->reg + TS_PLL_CFG0);
		fbdiv = pll_cfg0.fbdiv;
		refdiv = pll_cfg0.refdiv;
		pos_div1 = pll_cfg0.posdiv1;
		pos_div2 = pll_cfg0.posdiv2;

		frac = pll_cfg1.frac;
		tmp = rate_in * fbdiv + ((rate_in * frac) >> 24);
		div = refdiv * pos_div1 * pos_div2;

#ifndef CONFIG_ARM64
		mod = do_div(tmp, div);
		freq = tmp;
#else
		freq = tmp / div;
#endif
		if ((freq % 10) == 9)
			freq += 1;
		//pr_info("--pll freq %lu--\n", freq);
	}
	return freq;
}

static long ts_pll_round_rate(struct clk_hw *hw, unsigned long rate,
					unsigned long *parent_rate)
{
	struct ts_pll *pll = to_ts_pll(hw);

	pr_info("--%s enter stub, pll id %d--\n", __func__, pll->id);
	return rate;
}

/*
 * Wait for the PLL to be locked. If the PLL is not locked, try clearing the
 * slip before retrying
 */
void ts_pll_waitfor_lock(struct ts_pll *pll)
{
	u32 mask = GENMASK(pll->width - 1, 0) << pll->shift;

	while (true) {
		u32 reg = readl(pll->lock);

		if ((reg & mask) == mask)
			break;
	}
}

/* Adapted from sysctl_pll_enable */
static int ts_pll_enable(struct clk_hw *hw)
{
	struct ts_pll *pll = to_ts_pll(hw);
	union ts_pll_cfg1_un regcfg1;
	union ts_pll_cfg2_un regcfg2;
	union ts_pll_en_un regen;

	pr_info("--%s pll%d, reg_mode %d--\n", __func__,
		pll->id, pll->reg_mode);
	pr_info("--%s return\n", __func__);
	return 0;

	if (pll->reg_mode == PLL_REG_MODE_LOOSELY) {
		regen.data = readl(pll->reg + TS_PLL_EN);
		if (regen.loosely.powerdown) {
			regen.loosely.powerdown = 0;
			writel(regen.data, pll->reg + TS_PLL_EN);
		}
	} else {
		regcfg2.data = readl(pll->reg + TS_PLL_CFG2);
		if (regcfg2.compact.powerdown) {
			regcfg2.compact.powerdown = 0;
			writel(regcfg2.data, pll->reg + TS_PLL_CFG2);
		}
	}

	regcfg1.data = readl(pll->reg + TS_PLL_CFG1);
	if (regcfg1.bypass) {
		regcfg1.bypass = 0;
		writel(regcfg1.data, pll->reg + TS_PLL_CFG1);
	}

#ifndef CONFIG_ON_FPGA
	ts_pll_waitfor_lock(pll);
#endif	
	return 0;
}

static void ts_pll_disable(struct clk_hw *hw)
{
	struct ts_pll *pll = to_ts_pll(hw);
	union ts_pll_cfg1_un regcfg1;
	union ts_pll_cfg2_un regcfg2;
	union ts_pll_en_un regen;

	pr_info("--%s pll%d, reg_mode %d--\n", __func__,
		pll->id, pll->reg_mode);
	pr_info("--%s return\n", __func__);
	return;

	/*
	 * Bypassing before powering off is important so child clocks don't stop
	 * working. This is especially important for pll0, the indirect parent
	 * of the cpu clock.
	 */
	regcfg1.data = readl(pll->reg + TS_PLL_CFG1);
	if (!regcfg1.bypass) {
		regcfg1.bypass = 1;
		writel(regcfg1.data, pll->reg + TS_PLL_CFG1);
	}

	if (pll->reg_mode == PLL_REG_MODE_LOOSELY) {
		regen.data = readl(pll->reg + TS_PLL_EN);
		if (!regen.loosely.powerdown) {
			regen.loosely.powerdown = 1;
			writel(regcfg1.data, pll->reg + TS_PLL_EN);
		}
	} else {
		regcfg2.data = readl(pll->reg + TS_PLL_CFG2);
		if (!regcfg2.compact.powerdown) {
			regcfg2.compact.powerdown = 1;
			writel(regcfg2.data, pll->reg + TS_PLL_CFG2);
		}
	}
}

const struct clk_ops ts_pll_ops = {
	.recalc_rate = ts_pll_get_rate,
	.round_rate = ts_pll_round_rate,
	.set_rate = ts_pll_set_rate,
	.enable = ts_pll_enable,
	.disable = ts_pll_disable,
};

struct clk *ts_register_pll_struct(const char *name, const char *parent_name,
				     struct ts_pll *pll)
{
	int ret;
	struct clk_init_data init = {};

	init.name = name;
	init.parent_names = &parent_name;
	init.num_parents = 1;
	init.ops = &ts_pll_ops;

	pll->hw.init = &init;

	ret = clk_hw_register(NULL, &pll->hw);
	if (ret)
		return ERR_PTR(ret);

	return pll->hw.clk;
}

struct clk *ts_register_pll(const char *name, const char *parent_name,
			      void __iomem *reg, void __iomem *lock, u8 shift,
			      u8 width, u8 reg_mode)
{
	struct clk *clk;
	struct ts_pll *pll;

	pll = kzalloc(sizeof(*pll), GFP_KERNEL);
	if (!pll)
		return ERR_PTR(-ENOMEM);
	pll->reg = reg;
	pll->lock = lock;
	pll->shift = shift;
	pll->width = width;
	pll->reg_mode = reg_mode;

	clk = ts_register_pll_struct(name, parent_name, pll);
	if (IS_ERR(clk))
		kfree(pll);
	return clk;
}

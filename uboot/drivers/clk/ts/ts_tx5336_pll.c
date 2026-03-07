// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2022-2022 Tsingmicro, Inc.
 */
#define LOG_CATEGORY UCLASS_CLK

#include <common.h>
#include <dm.h>
/* For DIV_ROUND_DOWN_ULL, defined in linux/kernel.h */
#include <div64.h>
#include <log.h>
#include <serial.h>
#include <asm/io.h>
#include <linux/bitfield.h>
#include <linux/clk-provider.h>
#include <linux/delay.h>
#include <linux/err.h>
#include "ts_pll.h"

#define CLK_TS_PLL "ts_clk_pll"

#define TS_PLL_CFG0			0x0     
#define TS_PLL_CFG1			0x4    
#define TS_PLL_STAT         0X2c

union ts_pll_cfg0_un {
	u32 data;
	struct {
		u32 refdiv:6;      /* [0:5] */      // 32
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
        u32 frac:24;            /* [0:23] */  
		u32 dsmpd:1;            /* [24] */
		u32 dacpd:1;            /* [25] */
		u32 vcopd:1;            /* [26] */
		u32 four_phasepd:1;     /* [27] */
		u32 posdivpd:1;         /* [28] */
		u32 pd:1;               /* [29] */
		u32 reserved3:1;        /* [30] */
        u32 clk_gate_en:1;      /* [31] */
	};
};

union ts_pll_stat_un {
	u32 data;
	struct {
		u32 pll0_lock:1;   /* [0] */
		u32 pll1_lock:1;   /* [1] */
        u32 pll2_lock:1;   /* [2] */
        u32 pll3_lock:1;   /* [2] */
        u32 reserved:29;   /* [3:31] */
	};
};

struct ts_pll_configs {
	union ts_pll_cfg0_un cfg0;
	union ts_pll_cfg1_un cfg1;
};

static int ts_pll_enable(struct clk *clk);
static int ts_pll_disable(struct clk *clk);

int ts_pll_calc_config(struct ts_pll *pll, ulong rate, ulong rate_in,
				     struct ts_pll_configs *best)
{
	union ts_pll_cfg0_un pll_cfg0;
	union ts_pll_cfg1_un pll_cfg1;

	u64 tmp;
	u32 fbdiv;
	u32 frac;

	/* to do */
	log_debug("Calculating parameters with rate=%lu and rate_in=%lu\n",
		  rate, rate_in);

	pll_cfg0.data = readl(pll->reg + TS_PLL_CFG0);

	pll_cfg0.refdiv = 1;
	pll_cfg0.posdiv1 = 1;
	pll_cfg0.posdiv2 = 1;
	fbdiv = rate / rate_in;
	tmp = rate % rate_in;
	tmp <<= 24;

	tmp /= rate_in;

	frac = (u32)tmp;

	pll_cfg0.fbdiv = fbdiv;
	pll_cfg1.data = readl(pll->reg + TS_PLL_CFG1);
	pll_cfg1.frac = frac;

	best->cfg0.data = pll_cfg0.data;
	best->cfg1.data = pll_cfg1.data;

	// printf("cfg to set: 0x%x, 0x%x\n",
		// best->cfg0.data, best->cfg1.data);
	return 0;
}

static ulong ts_pll_set_rate(struct clk *clk, ulong rate)
{
	int err;
	long long rate_in = clk_get_parent_rate(clk);
	struct ts_pll_configs config = {};
	struct ts_pll *pll = to_ts_pll(clk);

	if (rate_in < 0)
		return rate_in;

	err = ts_pll_calc_config(pll, rate, rate_in, &config);
	if (err)
		return err;

	/*
	 * Don't use clk_disable as it might not actually disable the pll due to
	 * refcounting
	 */
	ts_pll_disable(clk);

	writel(config.cfg0.data, pll->reg + TS_PLL_CFG0);
	writel(config.cfg1.data, pll->reg + TS_PLL_CFG1);

	err = ts_pll_enable(clk);
	if (err)
		return err;

	serial_setbrg();
	return clk_get_rate(clk);
}

/* pll freq = osc_clk * (fbdiv + (frac>>24)) / (refdiv * postdiv1 * postdiv2) */
static ulong ts_pll_get_rate(struct clk *clk)
{
	long long rate_in = clk_get_parent_rate(clk);
	struct ts_pll *pll = to_ts_pll(clk);
	ulong freq;
	u32 fbdiv, refdiv, pos_div1, pos_div2, frac, div;
	u64 tmp;
	union ts_pll_cfg0_un pll_cfg0;
	union ts_pll_cfg1_un pll_cfg1;
	
	//printf("--clk %s rate_in %lld, reg %p--\n", clk->dev->name, rate_in, pll->reg);

	pll_cfg1.data = readl(pll->reg + TS_PLL_CFG1);
	if ((rate_in < 0)) {
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

		freq = tmp / div;
		if ((freq % 10) == 9)
			freq += 1;
		//printf("pll_cfg1 0x%x, pll_cfg0 0x%x\n", pll_cfg1, pll_cfg0);
		// printf("fbdiv 0x%ulx, refdiv 0x%ulx, pos_div1 0x%ulx, pos_div2 0x%ulx\n", fbdiv, refdiv, pos_div1, pos_div2);
		// printf("freq 0x%lx\n", freq);
	}
	return freq;
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
        debug("ts_pll_waitfor_lock spinning\n");
		if ((reg & mask) == mask)
			break;
	}
}

/* Adapted from sysctl_pll_enable */
static int ts_pll_enable(struct clk *clk)
{
	struct ts_pll *pll = to_ts_pll(clk);
	union ts_pll_cfg1_un regcfg1;

	debug("--%s %s, reg_mode %d--\n", __func__, clk->dev->name, pll->reg_mode);

    regcfg1.data = readl(pll->reg + TS_PLL_CFG1);
    /* enable pll */
    if(regcfg1.pd) {
        regcfg1.pd = 0;        
    }

    /* open pll output */
    if (!regcfg1.clk_gate_en) {
        regcfg1.clk_gate_en = 1;
    }

    writel(regcfg1.data, pll->reg + TS_PLL_CFG1);

#ifndef CONFIG_ON_FPGA
	ts_pll_waitfor_lock(pll);
#endif
	return 0;
}

static int ts_pll_disable(struct clk *clk)
{
	struct ts_pll *pll = to_ts_pll(clk);
	union ts_pll_cfg1_un regcfg1;

	printf("--%s %s, reg_mode %d--\n", __func__, clk->dev->name, pll->reg_mode);

	/*
	 * Bypassing before powering off is important so child clocks don't stop
	 * working. This is especially important for pll0, the indirect parent
	 * of the cpu clock.
     * note: dt57 and dt73 do not have bypass register(by luyuan)
	 */
#if 0
	regcfg1.data = readl(pll->reg + TS_PLL_CFG1);
	if (!regcfg1.bypass) {
		regcfg1.bypass = 1;
		writel(regcfg1.data, pll->reg + TS_PLL_CFG1);
	}
#endif

    /* close pll output */
    regcfg1.data = readl(pll->reg + TS_PLL_CFG1);
    if (regcfg1.clk_gate_en) {
        regcfg1.clk_gate_en = 0;
    }

    /* disable pll */
    if(!regcfg1.pd) {
        regcfg1.pd = 1;     
    }

    writel(regcfg1.data, pll->reg + TS_PLL_CFG1);
	return 0;
}

const struct clk_ops ts_pll_ops = {
	.get_rate = ts_pll_get_rate,
	.set_rate = ts_pll_set_rate,
	.enable = ts_pll_enable,
	.disable = ts_pll_disable,
};

static struct clk *ts_register_pll_struct(const char *name, const char *parent_name,
				     struct ts_pll *pll)
{
	int ret;
	struct clk *clk = &pll->clk;

	clk->flags = CLK_GET_RATE_NOCACHE;
	ret = clk_register(clk, CLK_TS_PLL, name, parent_name);
	if (ret)
		return ERR_PTR(ret);
	return clk;
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


U_BOOT_DRIVER(ts_pll) = {
	.name	= CLK_TS_PLL,
	.id	= UCLASS_CLK,
	.ops	= &ts_pll_ops,
};
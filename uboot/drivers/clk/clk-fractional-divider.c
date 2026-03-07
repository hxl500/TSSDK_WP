// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2014 Intel Corporation
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Adjustable fractional divider clock implementation.
 * Output rate = (m / n) * parent_rate.
 * Uses rational best approximation algorithm.
 */
#include <common.h>
#include <asm/io.h>
#include <malloc.h>
#include <clk-uclass.h>
#include <dm/device.h>
#include <dm/devres.h>
#include <dm/uclass.h>
#include <dm/lists.h>
#include <dm/device-internal.h>
#include <linux/bug.h>
#include <linux/clk-provider.h>
#include <linux/err.h>
#include <linux/log2.h>
#include <div64.h>
#include <clk.h>
#include "clk.h"

#define UBOOT_DM_CLK_CCF_FRACTIONAL_DIVIDER "ccf_clk_fractional_divider"

static ulong clk_fd_recalc_rate(struct clk *clk)
{
	struct clk_fractional_divider *fd = to_clk_fd(clk);
	unsigned long parent_rate = clk_get_parent_rate(clk);
	unsigned long m, n;
	unsigned int val;
	u64 ret;

	val = readl(fd->reg);
	m = (val & fd->mmask) >> fd->mshift;
	n = (val & fd->nmask) >> fd->nshift;

	if (!n || !m)
		return parent_rate;

	ret = (u64)parent_rate * m;
	do_div(ret, n);

	//debug("\n--parent_rate %ld, m 0x%lx, n 0x%lx, ret %lld--\n", parent_rate, m, n, ret);
	return ret;
}

/*
 * calculate best rational approximation for a given fraction
 * taking into account restricted register size, e.g. to find
 * appropriate values for a pll with 5 bit denominator and
 * 8 bit numerator register fields, trying to set up with a
 * frequency ratio of 3.1415, one would say:
 *
 * rational_best_approximation(31415, 10000,
 *		(1 << 8) - 1, (1 << 5) - 1, &n, &d);
 *
 * you may look at given_numerator as a fixed point number,
 * with the fractional part size described in given_denominator.
 *
 * for theoretical background, see:
 * http://en.wikipedia.org/wiki/Continued_fraction
 */
static void rational_best_approximation(unsigned long given_numerator,
					unsigned long given_denominator,
					unsigned long max_numerator,
					unsigned long max_denominator,
					unsigned long *best_numerator,
					unsigned long *best_denominator)
{
	unsigned long n, d, n0, d0, n1, d1;

	n = given_numerator;
	d = given_denominator;
	n0 = 0;
	d1 = 0;
	n1 = 1;
	d0 = 1;
	for (;;) {
		unsigned long t, a;

		if (n1 > max_numerator || d1 > max_denominator) {
			n1 = n0;
			d1 = d0;
			break;
		}
		if (d == 0)
			break;
		t = d;
		a = n / d;
		d = n % d;
		n = t;
		t = n0 + a * n1;
		n0 = n1;
		n1 = t;
		t = d0 + a * d1;
		d0 = d1;
		d1 = t;
	}
	*best_numerator = n1;
	*best_denominator = d1;
}

static ulong clk_fd_set_rate(struct clk *clk, unsigned long rate)
{
	struct clk_fractional_divider *fd = to_clk_fd(clk);
	unsigned long parent_rate = clk_get_parent_rate(clk);
	unsigned long m, n;
	unsigned int val;

	rational_best_approximation(rate, parent_rate,
			GENMASK(fd->mwidth - 1, 0), GENMASK(fd->nwidth - 1, 0),
			&m, &n);
	//debug("\n--[%s] clk %ld m 0x%lx n 0x%lx--\n", __func__, clk->id, m, n);

	val = readl(fd->reg);

#if CONFIG_IS_ENABLED(CLK_TS)
	/* write 0 to sync bit */
	if (fd->sync < SYNC_INVALID) {
		val &= ~(BIT(fd->sync));
		writel(val, fd->reg);
	}
#endif

	/* update SUM and STEP */
	val &= ~(fd->mmask | fd->nmask);
	val |= (m << fd->mshift) | (n << fd->nshift);
	writel(val, fd->reg);

#if CONFIG_IS_ENABLED(CLK_TS)
	/* write 1 to sync bit */
	if (fd->sync < SYNC_INVALID) {
		val |= BIT(fd->sync);
		writel(val, fd->reg);
	}
#endif

	return clk_get_rate(clk);
}

const struct clk_ops clk_fractional_divider_ops = {
	.get_rate = clk_fd_recalc_rate,
	.set_rate = clk_fd_set_rate,
};

U_BOOT_DRIVER(ccf_clk_fractional_divider) = {
	.name	= UBOOT_DM_CLK_CCF_FRACTIONAL_DIVIDER,
	.id	= UCLASS_CLK,
	.ops	= &clk_fractional_divider_ops,
	.flags = DM_FLAG_PRE_RELOC,
};

static struct clk * _register_fractional_divider(struct device *dev,
		const char *name, const char *parent_name, unsigned long flags,
		void __iomem *reg, u8 mshift, u8 mwidth, u8 nshift, u8 nwidth,
		u8 clk_divider_flags)
{
	struct clk_fractional_divider *fd;
	struct clk *clk;
	int ret;

	fd = kzalloc(sizeof(*fd), GFP_KERNEL);
	if (!fd)
		return ERR_PTR(-ENOMEM);

	fd->reg = reg;
	fd->mshift = mshift;
	fd->mwidth = mwidth;
	fd->mmask = GENMASK(mwidth - 1, 0) << mshift;
	fd->nshift = nshift;
	fd->nwidth = nwidth;
	fd->nmask = GENMASK(nwidth - 1, 0) << nshift;
	fd->flags = clk_divider_flags;

	/* register the clock */
	clk = &fd->clk;
	clk->flags = flags;

	ret = clk_register(clk, UBOOT_DM_CLK_CCF_FRACTIONAL_DIVIDER,
		name, parent_name);
	if (ret) {
		kfree(fd);
		return ERR_PTR(ret);
	}

	return clk;
}

struct clk *clk_register_fractional_divider(struct device *dev,
		const char *name, const char *parent_name, unsigned long flags,
		void __iomem *reg, u8 mshift, u8 mwidth, u8 nshift, u8 nwidth,
		u8 clk_divider_flags)
{
	struct clk *clk;

	clk = _register_fractional_divider(dev, name, parent_name, flags,
			reg, mshift, mwidth, nshift, nwidth, clk_divider_flags);
	if (IS_ERR(clk))
		return ERR_CAST(clk);
	return clk;
}

#if CONFIG_IS_ENABLED(CLK_TS)
static struct clk * _register_fractional_divider_with_sync(
		struct device *dev,
		const char *name, const char *parent_name, unsigned long flags,
		void __iomem *reg, u8 mshift, u8 mwidth, u8 nshift, u8 nwidth,
		u8 sync, u8 clk_divider_flags)
{
	struct clk_fractional_divider *fd;
	struct clk *clk;
	int ret;

	fd = kzalloc(sizeof(*fd), GFP_KERNEL);
	if (!fd)
		return ERR_PTR(-ENOMEM);

	fd->reg = reg;
	fd->mshift = mshift;
	fd->mwidth = mwidth;
	fd->mmask = GENMASK(mwidth - 1, 0) << mshift;
	fd->nshift = nshift;
	fd->nwidth = nwidth;
	fd->nmask = GENMASK(nwidth - 1, 0) << nshift;
	fd->sync = sync;
	fd->flags = clk_divider_flags;

	/* register the clock */
	clk = &fd->clk;
	clk->flags = flags;

	ret = clk_register(clk, UBOOT_DM_CLK_CCF_FRACTIONAL_DIVIDER,
		name, parent_name);
	if (ret) {
		kfree(fd);
		return ERR_PTR(ret);
	}

	return clk;
}

struct clk *clk_register_fractional_divider_with_sync(
		struct device *dev,
		const char *name, const char *parent_name, unsigned long flags,
		void __iomem *reg, u8 mshift, u8 mwidth, u8 nshift, u8 nwidth,
		u8 sync, u8 clk_divider_flags)
{
	struct clk *clk;

	clk = _register_fractional_divider_with_sync(dev, name, parent_name, flags,
			reg, mshift, mwidth, nshift, nwidth, sync, clk_divider_flags);
	if (IS_ERR(clk))
		return ERR_CAST(clk);
	return clk;
}

#endif

// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2022-2022 Tsingmicro, Inc.
 */
#include <linux/io.h>
#include <linux/of.h>
#include <linux/of_platform.h>
#include <linux/of_address.h>
#include <linux/debugfs.h>
#include <linux/uaccess.h>
#include <linux/clk.h>
#include <linux/clkdev.h>
#include <linux/clk-provider.h>
#include "ts_pll.h"
#include "ts_clk.h"
#include <linux/clk/clk-conf.h>
#include <linux/bits.h>

char osc_in[32] = {0};

struct ts_clk_params ts_clk_paras = {0};
void *ts_clk_get_para(void)
{
	return &ts_clk_paras;
}

/* Helper functions to create sub-clocks */
struct clk_mux *ts_create_mux(const struct ts_mux_params *params,
				       void *base)
{
	struct clk_mux *mux = kzalloc(sizeof(*mux), GFP_KERNEL);

	if (!mux)
		return mux;

	mux->reg = base + params->off;
	mux->mask = BIT(params->width) - 1;
	mux->shift = params->shift;
	mux->we    = params->we;
	mux->sync  = params->sync;

	return mux;
}

struct clk_divider *ts_create_div(const struct ts_div_params *params,
					   void *base)
{
	struct clk_divider *div = kzalloc(sizeof(*div), GFP_KERNEL);

	if (!div)
		return div;

	div->reg = base + params->off;
	div->shift = params->shift;
	div->width = params->width;
	div->we    = params->we;
	div->sync  = params->sync;
	div->flags = params->flags;

    if(params->table) {
        div->table = params->table;
    }

	return div;
}

struct clk_fractional_divider *ts_create_fractional_div(
	const struct ts_frac_div_params *params, void *base)
{
	struct clk_fractional_divider *frac_div;

	frac_div = kzalloc(sizeof(*frac_div), GFP_KERNEL);
	if (!frac_div)
		return frac_div;

	frac_div->reg =  base + params->off;
	frac_div->mshift = params->mshift;
	frac_div->mwidth = params->mwidth;
	frac_div->mmask = GENMASK(params->mwidth + params->mshift -1, params->mshift);
	frac_div->nshift = params->nshift;
	frac_div->nwidth = params->nwidth;
	frac_div->nmask = GENMASK(params->nwidth + params->nshift -1, params->nshift);
	frac_div->sync = params->sync;
	frac_div->flags = params->flags;
	return frac_div;
}

struct clk_fractional_divider *ts_create_fractional_div_legacy(
	const struct ts_div_params *params, void *base)
{
	struct clk_fractional_divider *frac_div;

	frac_div = kzalloc(sizeof(*frac_div), GFP_KERNEL);
	if (!frac_div)
		return frac_div;

	frac_div->reg =  base + params->off;
	frac_div->mshift = 16;
	frac_div->mwidth = 8;
	frac_div->mmask = GENMASK(27, 16);
	frac_div->nshift = 0;
	frac_div->nwidth = 16;
	frac_div->nmask = GENMASK(15, 0);
	frac_div->sync = 28;
	frac_div->flags = 0;

	return frac_div;
}

struct clk_gate *ts_create_gate(const struct ts_gate_params *params,
					 void *base)
{
	struct clk_gate *gate = kzalloc(sizeof(*gate), GFP_KERNEL);

	if (!gate)
		return gate;

	gate->reg = base + params->off;
	gate->bit_idx = params->bit_idx;

#if defined(CONFIG_ARCH_TS_TX5336)
		gate->flags = params->we;
#endif


	return gate;
}

static int ts_clk_register_plls(struct ts_clk_params *paras)
{
	struct ts_clk_reg_funcs *pfuncs = &paras->funcs;

	if (pfuncs->register_plls)
		return pfuncs->register_plls(paras);

	return 0;
}

static int ts_clk_register_comps(struct ts_clk_params *paras)
{
	struct ts_clk_reg_funcs *pfuncs = &paras->funcs;

	if (pfuncs->register_comps)
		return pfuncs->register_comps(paras);

	return 0;
}

static int ts_clk_register_comps_without_mux(struct ts_clk_params *paras)
{
	struct ts_clk_reg_funcs *pfuncs = &paras->funcs;

	if (pfuncs->register_comps_without_mux)
		return pfuncs->register_comps_without_mux(paras);

	return 0;
}

static int ts_clk_register_dividers(struct ts_clk_params *paras)
{
	struct ts_clk_reg_funcs *pfuncs = &paras->funcs;

	if (pfuncs->register_dividers)
		return pfuncs->register_dividers(paras);

	return 0;
}

static int ts_clk_register_gates(struct ts_clk_params *paras)
{
	struct ts_clk_reg_funcs *pfuncs = &paras->funcs;

	if (pfuncs->register_gates)
		return pfuncs->register_gates(paras);

	return 0;
}

static int ts_clk_register_phases(struct ts_clk_params *paras)
{
	struct ts_clk_reg_funcs *pfuncs = &paras->funcs;

	if (pfuncs->register_phases)
		return pfuncs->register_phases(paras);

	return 0;
}

static int ts_clk_add_provider(struct device_node *np,
	struct ts_clk_params *paras)
{
	int ret;
	struct ts_clk_reg_funcs *pfuncs = &paras->funcs;

	if (pfuncs->init_clk_data)
		pfuncs->init_clk_data(paras);

	ret = of_clk_add_provider(np, of_clk_src_onecell_get, &paras->clk_data);

    of_clk_set_defaults(np, false);
	return ret;
}

int ts_clk_of_parse(struct device_node *np, struct ts_clk_params *paras)
{
	void __iomem *top_base;
	void __iomem *mcu_base;
	const char *in0_name;
	struct clk *clk;

	top_base = of_iomap(np, 0);
	if (top_base == NULL) {
		pr_err("%s of_iomap 0 not found\n", __func__);
		return -ENOENT;
	}
	paras->top_base = top_base;

	mcu_base = of_iomap(np, 1);
	if (mcu_base == NULL)
		pr_info("%s of_iomap 1 not found\n", __func__);
	else
		paras->mcu_base = mcu_base;

	//pr_info("--top_base %p--\n", top_base);
	//pr_info("--mcu_base %p--\n", mcu_base);

	clk = of_clk_get(np, 0);
	if (IS_ERR_OR_NULL(clk)) {
		pr_err("%s of_clk_get failed\n", __func__);
		return -ENODEV;
	}


	in0_name = __clk_get_name(clk);
	if (!in0_name) {
		pr_err("%s __clk_get_name failed\n", __func__);
		return -ENOENT;
	}

	/*register osc to dev*/
	clk_register_clkdev(clk, in0_name, NULL);

	//pr_info("--in0 %s--\n", in0_name);
	strncpy(osc_in, in0_name, strlen(in0_name));
	return 0;
}

__weak int ts_clk_init_funcs(struct ts_clk_reg_funcs *pfuncs)
{
	pr_info("WARNING: %s is a stub\n", __func__);
	return 0;
}

static bool probed;

static void __init ts_clk_init(struct device_node *np)
{
	int ret;
	struct ts_clk_params *paras = NULL;

	pr_debug("--%s enter.\n", __func__);

	if (probed)
		return;

	paras = (struct ts_clk_params *)ts_clk_get_para();
	ret = ts_clk_of_parse(np, paras);
	if (ret) {
		pr_err("%s of parse failed, ret %d\n", __func__, ret);
		return;
	}
	ts_clk_init_funcs(&paras->funcs);

	/* PLL clocks */
	ret = ts_clk_register_plls(paras);
	if (ret) {
		pr_err("%s register plls failed, ret %d\n", __func__, ret);
		return;
	}

	/* Composite clocks with mux, with or without divider or gate */
	ret = ts_clk_register_comps(paras);
	if (ret) {
		pr_err("%s register comps, ret %d\n", __func__, ret);
		return;
	}

	/* Composite clocks without mux */
	ret = ts_clk_register_comps_without_mux(paras);
	if (ret) {
		pr_err("%s register comps without mux failed, ret %d\n",
			__func__, ret);
		return;
	}

	/* Dividing clocks */
	ret = ts_clk_register_dividers(paras);
	if (ret) {
		pr_err("%s register dividers failed, ret %d\n", __func__, ret);
		return;
	}

	/* Gated clocks */
	ret = ts_clk_register_gates(paras);
	if (ret) {
		pr_err("%s register gates, ret %d\n", __func__, ret);
		return;
	}

    ret = ts_clk_register_phases(paras);
	if (ret) {
		pr_err("%s register phases, ret %d\n", __func__, ret);
		return;
	}

	ret = ts_clk_add_provider(np, paras);
	if (ret) {
		pr_err("%s add provider, ret %d\n", __func__, ret);
		return;
	}

	probed = true;
	//pr_info("%s exit.\n", __func__);
}

CLK_OF_DECLARE(ts_clk, "ts,ts-common-clk", ts_clk_init);


#ifdef CONFIG_DEBUG_FS
struct dentry *ts_clk_debugfs;
#define TS_CLK_BUFSIZE		1024
static int ts_clk_test_show(char *name)
{
	struct clk *curr_clk, *pclk, *temp_clk;
	unsigned long rate;
	int max_cnt = 10;

	pr_info("\n--------%s--------\n", __func__);
	curr_clk = clk_get(NULL, name);
	if (IS_ERR_OR_NULL(curr_clk)) {
		pr_err("get clk %s failed\n", name);
		return PTR_ERR(curr_clk);
	}
	temp_clk = curr_clk;

	rate = clk_get_rate(curr_clk);
	pr_info("clock %s rate: %lu\n", __clk_get_name(curr_clk), rate);

	do {
		pclk = clk_get_parent(curr_clk);
		if (IS_ERR_OR_NULL(pclk))
			break;

		rate = clk_get_rate(pclk);
		pr_info("<--: %s, rate %lu\n", __clk_get_name(pclk), rate);

		curr_clk = pclk;
	} while (max_cnt--);

	return 0;
}

static int ts_clk_test_enable(char *name, int enable)
{
	struct clk *curr_clk;
	int ret;

	pr_info("\n--------%s--------\n", __func__);
	curr_clk = clk_get(NULL, name);
	if (IS_ERR_OR_NULL(curr_clk)) {
		pr_err("get clk %s failed\n", name);
		return PTR_ERR(curr_clk);
	}

	if (enable) {
		pr_info("prepare clock\n");
		ret = clk_prepare(curr_clk);
		pr_info("ret %d\n", ret);

		pr_info("enable clock\n");
		ret = clk_enable(curr_clk);
		pr_info("ret %d\n", ret);
	} else {
		pr_info("disable clock\n");
		clk_disable(curr_clk);

		pr_info("unprepare clock\n");
		clk_unprepare(curr_clk);
	}

	return 0;
}

static int ts_clk_test_set_parent(char *name, char *parent)
{
	struct clk *curr_clk, *pclk;
	int ret;

	pr_info("\n--------%s--------\n", __func__);
	curr_clk = clk_get(NULL, name);
	if (IS_ERR_OR_NULL(curr_clk)) {
		pr_err("get clk %s failed\n", name);
		return PTR_ERR(curr_clk);
	}
	pr_info("%s found\n", __clk_get_name(curr_clk));

	pclk = clk_get_parent(curr_clk);
	if (IS_ERR_OR_NULL(pclk)) {
		pr_err("get parent %s failed, pclk 0x%p\n", parent, pclk);
		return -ENODEV;
	}
	pr_info("ori parent: %s, pclk 0x%p\n", __clk_get_name(pclk), pclk);

	pclk = clk_get(NULL, parent);
	if (IS_ERR_OR_NULL(pclk)) {
		pr_err("get %s failed, pclk 0x%p\n", parent, pclk);
		return -ENODEV;
	}
	pr_info("%s found\n", __clk_get_name(pclk));

	ret = clk_set_parent(curr_clk, pclk);
	if (ret) {
		pr_err("set clk parent to %s failed, ret %d\n",
			__clk_get_name(pclk), ret);
		return ret;
	}

	pclk = clk_get_parent(curr_clk);
	if (IS_ERR_OR_NULL(pclk)) {
		pr_err("get parent %s failed, pclk 0x%p\n", parent, pclk);
		return -ENODEV;
	}
	pr_info("new parent: %s\n", __clk_get_name(pclk));
	return 0;
}

static int ts_clk_test_set_half_rate(char *name)
{
	unsigned long rate;
	struct clk *curr_clk;
	int ret;

	pr_info("\n--------%s--------\n", __func__);
	curr_clk = clk_get(NULL, name);
	if (IS_ERR_OR_NULL(curr_clk)) {
		pr_info("get clk %s failed\n", name);
		return PTR_ERR(curr_clk);
	}
	pr_info("%s found\n", __clk_get_name(curr_clk));

	rate = clk_get_rate(curr_clk);
	pr_info("ori rate %lu\n", rate);

	ret = clk_set_rate(curr_clk, rate / 2);
	pr_info("ret %d\n", ret);

	rate = clk_get_rate(curr_clk);
	pr_info("new rate %lu\n", rate);
	return 0;
}

static int ts_clk_test_set_rate(char *name, int new_rate)
{
	unsigned long rate;
	struct clk *curr_clk;
	int ret;

	pr_info("\n--------%s--------\n", __func__);
	if (new_rate == 0)
		return -EINVAL;

	curr_clk = clk_get(NULL, name);
	if (IS_ERR_OR_NULL(curr_clk)) {
		pr_info("get clk %s failed\n", name);
		return PTR_ERR(curr_clk);
	}
	pr_info("%s found\n", __clk_get_name(curr_clk));

	rate = clk_get_rate(curr_clk);
	pr_info("ori rate %lu\n", rate);

	ret = clk_set_rate(curr_clk, new_rate);
	pr_info("ret %d\n", ret);

	rate = clk_get_rate(curr_clk);
	pr_info("new rate %lu\n", rate);
	return 0;
}


static ssize_t ts_clk_show_usage(struct file *file, char __user *user_buf,
		size_t count, loff_t *ppos)
{
	char *buf;
	u32 len = 0;
	ssize_t ret;

	buf = kzalloc(TS_CLK_BUFSIZE, GFP_KERNEL);
	if (!buf)
		return 0;

	len += snprintf(buf + len, TS_CLK_BUFSIZE - len,
			"echo cmd para1 [para2] [para3] > dfx:\n");
	len += snprintf(buf + len, TS_CLK_BUFSIZE - len,
			"cmd: get <clk_name>, show clock and parents\n");
	len += snprintf(buf + len, TS_CLK_BUFSIZE - len,
			"cmd: enable <clk_name>, enable target clock\n");
	len += snprintf(buf + len, TS_CLK_BUFSIZE - len,
			"cmd: disable <clk_name>, disable target clock\n");
	len += snprintf(buf + len, TS_CLK_BUFSIZE - len,
			"cmd: set_parent <clk_name> <pclk_name>, set mux clock parent\n");
	len += snprintf(buf + len, TS_CLK_BUFSIZE - len,
			"cmd: half_rate <clk_name>, set clock rate to half\n");
	len += snprintf(buf + len, TS_CLK_BUFSIZE - len,
			"cmd: new_rate <clk_name> at <rate>, set clock to new rate\n");
	ret = simple_read_from_buffer(user_buf, count, ppos, buf, len);
	kfree(buf);
	return ret;
}

static ssize_t ts_clk_write_test(struct file *file,
		const char __user *user_buf, size_t count, loff_t *ppos)
{
	char buf[64] = {0};
	char cmd[64] = {0};
	char para1[64] = {0};
	char para2[64] = {0};
	int new_rate = 0;
	int ret;

	if (copy_from_user(buf, user_buf,
			min_t(size_t, sizeof(buf) - 1, count)))
		return -EFAULT;

	ret = sscanf(buf, "%s %s %s %d", cmd, para1, para2, &new_rate);
	if (ret < 0) {
		pr_err("read cmd failed\n");
		return count;
	}
	//pr_info("cmd: %s, para1: %s, para2: %s\n", cmd, para1, para2);

	if (!strcmp(cmd, "get"))
		ts_clk_test_show(para1);
	else if (!strcmp(cmd, "enable"))
		ts_clk_test_enable(para1, 1);
	else if (!strcmp(cmd, "disable"))
		ts_clk_test_enable(para1, 0);
	else if (!strcmp(cmd, "set_parent"))
		ts_clk_test_set_parent(para1, para2);
	else if (!strcmp(cmd, "half_rate"))
		ts_clk_test_set_half_rate(para1);
	else if (!strcmp(cmd, "new_rate"))
		ts_clk_test_set_rate(para1, new_rate);
	else
		pr_info("invalid cmd\n");

	return count;
}

static const struct file_operations ts_clk_debugfs_ops = {
	.owner		= THIS_MODULE,
	.open		= simple_open,
	.read		= ts_clk_show_usage,
	.write		= ts_clk_write_test,
	.llseek		= default_llseek,
};

static int ts_clk_debugfs_init(void)
{
	pr_debug("%s enter\n", __func__);
	ts_clk_debugfs = debugfs_create_dir("ts_clks", NULL);
	if (!ts_clk_debugfs) {
		pr_err("%s create dir failed\n", __func__);
		return -ENOMEM;
	}

	debugfs_create_file("dfx", S_IFREG | 0660,
		ts_clk_debugfs, NULL, &ts_clk_debugfs_ops);
	pr_debug("%s exit\n", __func__);
	return 0;
}
late_initcall(ts_clk_debugfs_init);

#else
static inline int ts_clk_debugfs_init(void)
{
	return 0;
}
#endif /* CONFIG_DEBUG_FS */

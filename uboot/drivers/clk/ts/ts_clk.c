// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2022-2022 Tsingmicro, Inc.
 */
#include <asm/io.h>
#include <dm.h>
#include <log.h>
#include <mapmem.h>
#include "ts_pll.h"
#include "ts_clk.h"

char osc_in[32] = {0};

struct ts_clk_params ts_clk_paras = {0};
void *ts_clk_get_para(void)
{
	return &ts_clk_paras;
}

/* All methods are delegated to CCF clocks */
static ulong ts_clk_get_rate(struct clk *clk)
{
	struct clk *c;
	int err = clk_get_by_id(clk->id, &c);

	if (err)
		return err;
	return clk_get_rate(c);
}

static ulong ts_clk_set_rate(struct clk *clk, unsigned long rate)
{
	struct clk *c;
	int err = clk_get_by_id(clk->id, &c);

	if (err)
		return err;
	return clk_set_rate(c, rate);
}

static int ts_clk_set_parent(struct clk *clk, struct clk *parent)
{
	struct clk *c, *p;
	int err = clk_get_by_id(clk->id, &c);

	if (err)
		return err;

	err = clk_get_by_id(parent->id, &p);
	if (err)
		return err;

	return clk_set_parent(c, p);
}

static int ts_clk_endisable(struct clk *clk, bool enable)
{
	struct clk *c;
	int err = clk_get_by_id(clk->id, &c);

	if (err)
		return err;
	return enable ? clk_enable(c) : clk_disable(c);
}

static int ts_clk_enable(struct clk *clk)
{
	return ts_clk_endisable(clk, true);
}

static int ts_clk_disable(struct clk *clk)
{
	return ts_clk_endisable(clk, false);
}

/* translate clk->id to real clock */
static int ts_clk_request(struct clk *clk)
{
	struct clk *c;
	int err;

	//debug("--%s id %ld enter--\n", __func__, clk->id);
	err = clk_get_by_id(clk->id, &c);
	if (err)
		return err;

	//debug("--clk1 %s--\n", clk->dev->name);
	//debug("--clk2 %s--\n", c->dev->name);
	memcpy(clk, c, sizeof(*c));
	return 0;
}

static const struct clk_ops ts_clk_ops = {
	.request = ts_clk_request,
	.set_rate = ts_clk_set_rate,
	.get_rate = ts_clk_get_rate,
	.set_parent = ts_clk_set_parent,
	.enable = ts_clk_enable,
	.disable = ts_clk_disable,
};

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
	mux->parent_names = params->parent_names;
	mux->num_parents = params->num_parents;
	mux->we    = params->we;
	mux->sync  = params->sync;

	return mux;
}

struct clk_divider *ts_create_div(const struct ts_div_params *params,
					   void *base, struct clk_div_table *table)
{
	struct clk_divider *div = kzalloc(sizeof(*div), GFP_KERNEL);
	struct clk *clk;

	if (!div)
		return div;

	div->reg = base + params->off;
	div->shift = params->shift;
	div->width = params->width;
	div->we    = params->we;
	div->sync  = params->sync;
	div->flags = params->flags;
	div->table = table;

	clk = &div->clk;
	clk->flags = CLK_GET_RATE_NOCACHE;

	return div;
}

struct clk_fractional_divider *ts_create_fractional_div(
	const struct ts_div_params *params, void *base)
{
	struct clk_fractional_divider *frac_div;
	struct clk *clk;

	frac_div = kzalloc(sizeof(*frac_div), GFP_KERNEL);
	if (!frac_div)
		return frac_div;

	frac_div->reg =  base + params->off;
	frac_div->mshift = 16;
	frac_div->mwidth = 12;
	frac_div->mmask = GENMASK(27, 16);
	frac_div->nshift = 0;
	frac_div->nwidth = 16;
	frac_div->nmask = GENMASK(15, 0);
	frac_div->sync = 28;
	frac_div->flags = 0;

	clk = &frac_div->clk;
	clk->flags = CLK_GET_RATE_NOCACHE;
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

#if defined(CONFIG_ARCH_TS_TX5112)  ||  defined(CONFIG_ARCH_TS_TX5336) 
	gate->we = params->we;
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


static int ts_clk_register_all(struct ts_clk_params *paras)
{
	struct ts_clk_reg_funcs *pfuncs = &paras->funcs;
	if (pfuncs->register_all)
		return pfuncs->register_all(paras);

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

int ts_clk_cfg_by_of(struct udevice *dev)
{
	struct ts_clk_of_cfg *cfg;
	struct clk *clk;
	ulong rate;
	ulong rate2;
	u32 cfgnum = 0;
	u32 temp[4] = {0};
	int ret;
	int i;

	ret = dev_read_u32(dev, "clkcfg_num", &cfgnum);
	if (ret < 0)
		return -EINVAL;

	printf("--clkcfg_num %u--\n", cfgnum);
	if ((cfgnum == 0) || (cfgnum > TS_CLK_OF_CFG_MAX_NUM))
		return -EINVAL;

	cfg = kzalloc(cfgnum * sizeof(*cfg), GFP_KERNEL);
	if (!cfg) {
		printf("--kzalloc for cfg failed--\n");
		return -ENOMEM;
	}

	for (i = 0; i < cfgnum; i++) {
		ret = dev_read_u32_index(dev, "clkcfg_arr", i*3, &temp[0]);
		ret |= dev_read_u32_index(dev, "clkcfg_arr", i*3+1, &temp[1]);
		ret |= dev_read_u32_index(dev, "clkcfg_arr", i*3+2, &temp[2]);
		if (ret < 0) {
			printf("read prop clkcfg_arr %d failed, ret %d\n", i, ret);
			goto EXIT_OF_CFG;
		}
		cfg[i].id = temp[0];
		cfg[i].rate = ((u64)temp[1] << 32) | ((u64)temp[2]);
		printf("[%d] id %u, rate %llu\n", i, cfg[i].id, cfg[i].rate);
	}

	for (i = 0; i < cfgnum; i++) {
		ret = clk_get_by_id(cfg[i].id, &clk);
		if (ret) {
			printf("get clk id %d failed, ret %d\n", cfg[i].id, ret);
			continue;
		}

		rate = clk_get_rate(clk);
		printf("%s, ori rate %lu\n", clk->dev->name, rate);

		rate2 = clk_set_rate(clk, cfg[i].rate);
		printf("rate2 %ld\n", rate2);

		rate = clk_get_rate(clk);
		printf("%s, new rate %lu\n", clk->dev->name, rate);
	}

EXIT_OF_CFG:
	free(cfg);
	return ret;
}

int ts_clk_of_parse(struct udevice *dev, struct ts_clk_params *paras)
{
	int ret;
	const char *in0;
	struct clk *in0_clk;
	fdt_addr_t addr;

	addr = dev_read_addr_index(dev, 0);
	if (addr == FDT_ADDR_T_NONE) {
		printf("%s get addr0 failed", __func__);
		return -EINVAL;
	}
	paras->top_base = (void *)addr;

	addr = dev_read_addr_index(dev, 1);
	if (addr == FDT_ADDR_T_NONE) {
		pr_debug("only have addr0, no addr1\n");
	} else {
		paras->mcu_base = (void *)addr;
	}

	// printf("--top_base %p--\n", paras->top_base);

	in0_clk = kzalloc(sizeof(*in0_clk), GFP_KERNEL);
	if (!in0_clk) {
		printf("%s kzalloc in0_clk failed\n", __func__);
		return -ENOMEM;
	}

	ret = clk_get_by_index(dev, 0, in0_clk);
	if (ret) {
		printf("clk_get_by_index 0 failed, ret %d\n", ret);
		return ret;
	}

	in0 = in0_clk->dev->name;
	// printf("--in0 %s--\n", in0);
	strncpy(osc_in, in0_clk->dev->name, strlen(in0_clk->dev->name));
	return 0;
}

__weak int ts_clk_init_funcs(struct ts_clk_reg_funcs *pfuncs)
{
	puts("WARNING: ts_clk_init_funcs is a stub\n");
	return 0;
}

static bool probed = false;
static int ts_clk_probe(struct udevice *dev)
{
	int ret;

	/*
	 * Only one instance of this driver allowed. This prevents weird bugs
	 * when the driver fails part-way through probing. Some clocks will
	 * already have been registered, and re-probing will register them
	 * again, creating a bunch of duplicates. Better error-handling/cleanup
	 * could fix this, but it's Probably Not Worth It (TM).
	 */
    
	if (probed)
		return -ENOTSUPP;

	struct ts_clk_params *paras = ts_clk_get_para();
	ret = ts_clk_of_parse(dev, paras);
	if (ret) {
		printf("%s of parse failed, ret %d\n", __func__, ret);
		return ret;
	}
	ts_clk_init_funcs(&paras->funcs);
	probed = true;

	/* PLL clocks */
	ret = ts_clk_register_plls(paras);
	if (ret) {
		printf("%s register plls failed, ret %d\n", __func__, ret);
		return ret;
	}

	/* Composite clocks with mux, with or without divider or gate */
	ret = ts_clk_register_comps(paras);
	if (ret) {
		printf("%s register comps, ret %d\n", __func__, ret);
		return ret;
	}

	/* Composite clocks without mux */
	ret = ts_clk_register_comps_without_mux(paras);
	if (ret) {
		printf("%s register comps without mux failed, ret %d\n", __func__, ret);
		return ret;
	}
	/* Dividing clocks */
	ret = ts_clk_register_dividers(paras);
	if (ret) {
		printf("%s register dividers failed, ret %d\n", __func__, ret);
		return ret;
	}
	/* Gated clocks */
	ret = ts_clk_register_gates(paras);
	if (ret) {
		printf("%s register gates, ret %d\n", __func__, ret);
		return ret;
	}
	//ts_clk_cfg_by_of(dev);
    
    /* all clk register */
    ret = ts_clk_register_all(paras);
	if (ret) {
		printf("%s register all, ret %d\n", __func__, ret);
		return ret;
	}
	// printf("--%s exit--\n", __func__);
    
	return 0;
}

static int ts_clk_bind(struct udevice *dev)
{
	// printf("--%s clk framework enter--\n", __func__);
	return 0;
}

static const struct udevice_id ts_clk_ids[] = {
	{ .compatible = "ts,ts-common-clk" },
	{ },
};

U_BOOT_DRIVER(ts_clk) = {
	.name = "ts-clk",
	.id = UCLASS_CLK,
	.of_match = ts_clk_ids,
	.ops = &ts_clk_ops,
	.bind = ts_clk_bind,
	.probe = ts_clk_probe,
};

/* Tests for Common Clock Framework driver */
int ts_clk_test_show(int clk_id)
{
	struct clk *clk, *pclk;
	struct clk* curr_clk;
	long long rate;
	int ret;

	printf("\n--------%s--------\n", __func__);

	/* Test for clk_get_by_id() */
	ret = clk_get_by_id(clk_id, &clk);
	if (ret) {
		printf("get clk id %d failed, ret %d\n", clk_id, ret);
		return ret;
	}

	printf("clock name: %s\n", clk->dev->name);
	rate = clk_get_rate(clk);
	printf("clock rate: %lld\n", rate);

	curr_clk = clk;
	do {
		pclk = clk_get_parent(curr_clk);
		if (IS_ERR(pclk)) {
			break;
		}

		rate = clk_get_parent_rate(curr_clk);
		printf("<--: %s, id %ld, rate %lld\n", pclk->dev->name, pclk->id, rate);

		curr_clk = pclk;
	} while (1);

	return 0;
}

int ts_clk_test_enable(int clk_id, int enable)
{
	struct clk *clk;
	int ret;

	printf("\n--------%s--------\n", __func__);

	/* Test for clk_get_by_id() */
	ret = clk_get_by_id(clk_id, &clk);
	if (ret) {
		printf("get clk id %d failed, ret %d\n", clk_id, ret);
		return ret;
	}

	printf("clock name: %s\n", clk->dev->name);

	if (enable) {
		printf("enable clock\n");
		clk_enable(clk);
	} else {
		printf("disable clock\n");
		clk_disable(clk);
	}
	return 0;
}

int ts_clk_test_set_parent(int clk_id, int parent_id)
{
	struct clk *clk, *pclk;
	long long rate;
	int ret;

	printf("\n--------%s--------\n", __func__);
	/* Test the mux of CCF */
	ret = clk_get_by_id(clk_id, &clk);
	if (ret) {
		printf("get clk id %d failed, ret %d\n", clk_id, ret);
		return ret;
	}
	pclk = clk_get_parent(clk);
	if (IS_ERR(pclk)) {
		printf("get %s parent failed\n", clk->dev->name);
		return -ENODEV;
	}
	rate = clk_get_parent_rate(clk);
	printf("ori parent:id %ld, %s, rate %lld\n", pclk->id, pclk->dev->name, rate);

	ret = clk_get_by_id(parent_id, &pclk);
	if (ret) {
		printf("get clk id %d failed, ret %d\n", parent_id, ret);
		return ret;
	}

	printf("try to set parent: id %ld, %s\n", pclk->id, pclk->dev->name);
	ret = clk_set_parent(clk, pclk);
	if (ret) {
		printf("set clk parent to %s failed, ret %d\n", pclk->dev->name, ret);
		return ret;
	}

	pclk = clk_get_parent(clk);
	if (IS_ERR(pclk)) {
		printf("get %s parent failed\n", clk->dev->name);
		return -ENODEV;
	}
	rate = clk_get_parent_rate(clk);
	printf("new parent: id %ld, %s, rate %lld\n", pclk->id, pclk->dev->name, rate);
	return 0;
}

int ts_clk_test_set_half_rate(int clk_id)
{
	struct clk *clk;
	long long rate;
	ulong rate2;
	int ret;

	printf("\n--------%s--------\n", __func__);
	ret = clk_get_by_id(clk_id, &clk);
	if (ret) {
		printf("get clk id %d failed, ret %d\n", clk_id, ret);
		return ret;
	}
	rate = clk_get_rate(clk);
	printf("%s, ori rate %lld\n", clk->dev->name, rate);

	rate2 = clk_set_rate(clk, rate/2);
	printf("rate2 %ld\n", rate2);

	rate = clk_get_rate(clk);
	printf("%s, new rate %lld\n", clk->dev->name, rate);
	return 0;
}

int ts_clk_test_set_rate(int clk_id, long long rate)
{
	struct clk *clk;
	ulong rate2;
	int ret;

	printf("\n--------%s--------\n", __func__);
	ret = clk_get_by_id(clk_id, &clk);
	if (ret) {
		printf("get clk id %d failed, ret %d\n", clk_id, ret);
		return ret;
	}
	rate2 = clk_get_rate(clk);
	printf("%s, ori rate %ld\n", clk->dev->name, rate2);

	rate2 = clk_set_rate(clk, rate);
	printf("rate2 %ld\n", rate2);

	rate2 = clk_get_rate(clk);
	printf("%s, new rate %ld\n", clk->dev->name, rate2);
	return 0;
}

extern struct clk *find_clk_by_name(char *name);

int clk_test_entry(int argc, char *const argv[])
{
	int ret = 0;
	u32 rate;
	char *endp;
	struct clk *clk_tmp;
	struct clk *clk_parent;

	if (argc < 3) {
		printf("argc %d err\n", argc);
		goto TEST_EXIT;
	}

	if (!strncmp(argv[1], "get", 3)) {
		clk_tmp = find_clk_by_name(argv[2]);
		if (!clk_tmp) {
			printf("find_clk_by_name %s failed, ret %d\n", argv[2], ret);
			goto TEST_EXIT;
		}
		ret = ts_clk_test_show(clk_tmp->id);
	} else if (!strncmp(argv[1], "enable", 6)) {
		clk_tmp = find_clk_by_name(argv[2]);
		if (!clk_tmp) {
			printf("find_clk_by_name %s failed, ret %d\n", argv[2], ret);
			goto TEST_EXIT;
		}
		ret = ts_clk_test_enable(clk_tmp->id, true);
	} else if (!strncmp(argv[1], "disable", 7)) {
		clk_tmp = find_clk_by_name(argv[2]);
		if (!clk_tmp) {
			printf("find_clk_by_name %s failed, ret %d\n", argv[2], ret);
			goto TEST_EXIT;
		}
		ret = ts_clk_test_enable(clk_tmp->id, false);
	} else if (!strncmp(argv[1], "set_parent", 10)) {
		if (argc < 4) {
			printf("argc %d err\n", argc);
			goto TEST_EXIT;
		}
		clk_tmp = find_clk_by_name(argv[2]);
		if (!clk_tmp) {
			printf("find_clk_by_name %s failed, ret %d\n", argv[2], ret);
			goto TEST_EXIT;
		}

		clk_parent = find_clk_by_name(argv[3]);
		if (!clk_parent) {
			printf("find_clk_by_name %s failed, ret %d\n", argv[3], ret);
			goto TEST_EXIT;
		}

		ret = ts_clk_test_set_parent(clk_tmp->id, clk_parent->id);
	} else if (!strncmp(argv[1], "new_rate", 8)) {
		if (argc < 4) {
			printf("argc %d err\n", argc);
			goto TEST_EXIT;
		}
		clk_tmp = find_clk_by_name(argv[2]);
		if (!clk_tmp) {
			printf("find_clk_by_name %s failed, ret %d\n", argv[2], ret);
			goto TEST_EXIT;
		}

		rate = simple_strtoul(argv[3], &endp, 0);
		printf("rate %d\n", rate);
		ret = ts_clk_test_set_rate(clk_tmp->id, rate);
	}
	goto TEST_DONE;

TEST_EXIT:
	printf("invalid cmd\n");
	printf("usage:\n\t clktest get <clk_name> \n"
		"\t clktest enable <clk_name>\n"
		"\t clktest disable <clk_name>\n"
		"\t clktest set_parent <clk_name> <parent_name>\n"
		"\t clktest new_rate <clk_name> <rate>\n");
TEST_DONE:
	return ret;
}

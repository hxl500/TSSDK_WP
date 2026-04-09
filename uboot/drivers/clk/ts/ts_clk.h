/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright (C) 2022-2022 Tsingmicro, Inc.
 */

#ifndef TS_CLK_H
#define TS_CLK_H

#include <linux/types.h>
#include <linux/clk-provider.h>

struct ts_div_params {
	u32 off;
	u8 shift;
	u8 width;
	u8 we;   //write_enable shift
	u8 sync; //generate update sync event shift
	u8 flags;
};

struct ts_gate_params {
	u32 off;
	u8 bit_idx;
	u8 we;
};

struct ts_mux_params {
	const char *const *parent_names;
	u8 num_parents;
	u32 off;
	u8 shift;
	u8 width;
	u8 we;   //write_enable shift
	u8 sync; //generate update sync event shift
};


struct ts_comp_params {
	u8 mux;
	u8 div;
	u8 gate;
};

struct ts_pll_params {
	char *name;
	u32 id;
	u32 off;
	u32 lock_off;
	u8 shift;
	u8 width;
};

struct ts_clk_params;

struct ts_clk_reg_funcs {
	int (*register_plls)(struct ts_clk_params *paras);
	int (*register_comps)(struct ts_clk_params *paras);
	int (*register_comps_without_mux)(struct ts_clk_params *paras);
	int (*register_dividers)(struct ts_clk_params *paras);
	int (*register_gates)(struct ts_clk_params *paras);
    int (*register_all)(struct ts_clk_params *paras);
};

struct ts_clk_params {
	void *top_base;
	void *mcu_base;
	struct ts_clk_reg_funcs funcs;
};

#define TS_CLK_OF_CFG_MAX_NUM		10
struct ts_clk_of_cfg {
	u32 id;
	u64 rate;
};

extern char osc_in[32];

static inline struct clk *ts_clk_gate(const char *name,
					const char *parent_name,
					void __iomem *reg, u8 bit_idx)
{
	return clk_register_gate(NULL, name, parent_name, 0, reg, bit_idx, 0,
				 NULL);
}

static inline struct clk *ts_clk_half(const char *name,
					const char *parent_name)
{
	return clk_register_fixed_factor(NULL, name, parent_name, CLK_GET_RATE_NOCACHE, 1, 2);
}

static inline struct clk *ts_clk_fixed_divider(const char *name,
					const char *parent_name, unsigned int div)
{
	return clk_register_fixed_factor(NULL, name, parent_name, CLK_GET_RATE_NOCACHE, 1, div);
}

static inline struct clk *ts_clk_div(const char *name,
				       const char *parent_name,
				       void __iomem *reg, u8 shift, u8 width)
{
	return clk_register_divider(NULL, name, parent_name, CLK_GET_RATE_NOCACHE, reg, shift,
				    width, 0);
}

struct clk_mux *ts_create_mux(const struct ts_mux_params *params,
			void *base);
struct clk_divider *ts_create_div(const struct ts_div_params *params,
			void *base, struct clk_div_table *table);
struct clk_fractional_divider *ts_create_fractional_div(
	const struct ts_div_params *params, void *base);
struct clk_gate *ts_create_gate(const struct ts_gate_params *params,
			void *base);
int ts_clk_init_funcs(struct ts_clk_reg_funcs *pfuncs);
int ts_clk_test_show(int clk_id);
int ts_clk_test_set_parent(int clk_id, int parent_id);
int ts_clk_test_set_half_rate(int clk_id);
int ts_clk_test_set_rate(int clk_id, long long rate);

#endif /* TS_CLK_H */

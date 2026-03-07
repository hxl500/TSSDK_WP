// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2022-2022 Tsingmicro, Inc.
 */

#define DEBUG

#include <asm/io.h>
#include <dt-bindings/clock/ts-tx5336-clk.h>
#include <dm.h>
#include <log.h>
#include <mapmem.h>
#include "linux/clk-provider.h"
#include "ts_pll.h"
#include "ts_clk.h"

/* TOP_CGU */
#define PLL0_CFG0			0x000
#define PLL0_CFG1			0x004
#define PLL1_CFG0			0x010
#define PLL1_CFG1			0x014
#define PLL2_CFG0			0x020
#define PLL2_CFG1			0x024
#define PLL3_CFG0			0x0150
#define PLL3_CFG1			0x0154
#define PLL_STATUS			0x02C

#define CPU_CLK_CFG0		0x030
#define CPU_CLK_CFG1		0x034

#define PERIPH0_CFG0        0X048
#define PERIPH0_CFG1        0X04C

#define PERIPH1_CFG0        0X050
#define PERIPH1_CFG1        0X054

#define QSPI_CFG            0X060
#define SDHC_CFG0           0X0B8
#define SDHC_CFG1           0X0BC

/* All methods are delegated to CCF clocks */

/* Parents for muxed clocks */
static const char * const pll_clk_sels[] = { "pll0_cpu_clk_div", "pll1_cpu_clk_div"};
static const char * const cpu_clk_sels[] = { "pll_cpu_clk_mux", &osc_in[0]};
static const char * const gic_clk_sels[] = {  &osc_in[0], "pll0_d2_gic_clk_div"};
static const char * const peri0_apb_clk_sels[] = {"peri0_apb_clk_gate", "pll0_d2_peri0_apb_clk_div"};
static const char * const peri1_apb_clk_sels[] = {"peri1_apb_clk_gate", "pll0_d2_peri1_apb_clk_div"};
static const char * const qspi_ahb_clk_sels[] = {"qspi_ahb_clk_icg", "qspi_ahb_clk_div"};
static const char * const qspi_ref_clk_sels[] = {"qspi_ref_clk_icg", "qspi_ref_clk_div"};
static const char * const sdhc0_core_clk_sels[] = {"osc_sdhc0_core_clk_icg", "pll0_sdhc0_core_clk_div"};
static const char * const sdhc1_core_clk_sels[] = {"osc_sdhc1_core_clk_icg", "pll0_sdhc1_core_clk_div"};

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

#define DIV(id, off, shift, width, we, sync) DIV_FLAGS(id, off, shift, width, we, sync, 0)
#define DIV_LIST \
    DIV(TS_CLK_TX5336_PLL0_CPU_CLK_DIV, CPU_CLK_CFG1, 12, 3, 0, SYNC_INVALID) \
    DIV(TS_CLK_TX5336_PLL1_CPU_CLK_DIV, CPU_CLK_CFG1, 8, 3, 0, SYNC_INVALID) \
    DIV(TS_CLK_TX5336_PLL0_D2_GIC_CLK_DIV, CPU_CLK_CFG0, 20, 4, 16, SYNC_INVALID) \
    DIV(TS_CLK_CPU_AXI_CLK_DIV, CPU_CLK_CFG1, 24, 4, 0, SYNC_INVALID) \
    DIV(TS_CLK_DAPLITE_DBG_PCLK_DIV, CPU_CLK_CFG1, 16, 5, 0, SYNC_INVALID) \
    DIV(TS_CLK_PLL0_D2_PERI0_APB_CLK_DIV, PERIPH0_CFG0, 8, 4, 0, SYNC_INVALID) \
    DIV(TS_CLK_PLL0_D2_PERI1_APB_CLK_DIV, PERIPH1_CFG0, 8, 4, 0, SYNC_INVALID) \
    DIV(TS_CLK_TOP_OSC_QSPI_AHB_CLK_DIV, QSPI_CFG, 12, 4, 0, SYNC_INVALID) \
    DIV(TS_CLK_TOP_OSC_QSPI_REF_CLK_DIV, QSPI_CFG, 16, 4, 0, SYNC_INVALID) \
    DIV(TS_CLK_PERIPH2_PLL0_SDHC0_CORE_CLK_DIV, SDHC_CFG0, 8, 6, 0, SYNC_INVALID)   \
    DIV(TS_CLK_PERIPH2_PLL0_SDHC1_CORE_CLK_DIV, SDHC_CFG1, 8, 6, 0, SYNC_INVALID)   \


#define _DIVIFY(id) TS_CLK_TX5336_DIV_##id
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

#define GATE_LIST \
    GATE(TS_CLK_PERI0_APB_CLK_GATE, PERIPH0_CFG0, 12, 0) \
    GATE(TS_CLK_PERI1_APB_CLK_GATE, PERIPH1_CFG0, 12, 0) \
    GATE(TS_CLK_TX5336_OTPC_APB_CLK_GATE, PERIPH0_CFG1, 16, 2)  \
    GATE(TS_CLK_PERIPH1_PWM_APB_CLK_GATE, PERIPH1_CFG1, 8, 2)  \
    GATE(TS_CLK_PERIPH1_QSPI_APB_CLK_GATE, QSPI_CFG, 2, 0)  \
    GATE(TS_CLK_PERIPH0_IOMUX_APB_CLK_GATE, PERIPH0_CFG1, 18, 4) \
    GATE(TS_CLK_PERIPH0_GPADC_APB_CLK_GATE, PERIPH0_CFG1, 17, 3) \
    GATE(TS_CLK_PERIPH0_GPIO_APB_CLK_GATE, PERIPH0_CFG1, 21, 7) \
    GATE(TS_CLK_TOP_OSC_QSPI_AHB_CLK_GATE, QSPI_CFG, 3, 0)  \
    GATE(TS_CLK_TOP_OSC_QSPI_REF_CLK_GATE, QSPI_CFG, 5, 0)  \
    GATE(TS_CLK_PERIPH2_SDHC0_AHB_CLK_GATE, SDHC_CFG0, 2, 0) \
    GATE(TS_CLK_PERIPH2_SDHC1_AHB_CLK_GATE, SDHC_CFG1, 2, 0) \
    GATE(TS_CLK_PERIPH2_OSC_SDHC0_CORE_CLK_GATE, SDHC_CFG0, 3, 0) \
    GATE(TS_CLK_PERIPH2_OSC_SDHC1_CORE_CLK_GATE, SDHC_CFG1, 3, 0) \
	GATE(TS_CLK_PERIPH2_SDHC0_CORE_CLK_MUX, SDHC_CFG0, 3, 0) \
    GATE(TS_CLK_PERIPH2_SDHC1_CORE_CLK_MUX, SDHC_CFG1, 3, 0) \

#define _GATEIFY(id) TS_CLK_TX5336_GATE_##id
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

#define MUX(id, reg, shift, width, we, sync) \
	MUX_PARENTS(id, generic_sels, reg, shift, width, we, sync)

/* TODO:*/
#define MUX_LIST \
    MUX_PARENTS(TS_CLK_TX5336_PLL_CPU_CLK_MUX, pll_clk_sels, CPU_CLK_CFG1, 4, 1, 0, SYNC_INVALID) \
    MUX_PARENTS(TS_CLK_TX5336_CPU_CLK_MUX, cpu_clk_sels, CPU_CLK_CFG1, 5, 1, 0, SYNC_INVALID) \
    MUX_PARENTS(TS_CLK_TX5336_GIC_CLK_MUX, gic_clk_sels, CPU_CLK_CFG0, 19, 1, 16, SYNC_INVALID) \
    MUX_PARENTS(TS_CLK_PERI0_APB_CLK_MUX, peri0_apb_clk_sels, PERIPH0_CFG0, 4, 1, 0, SYNC_INVALID) \
    MUX_PARENTS(TS_CLK_PERI1_APB_CLK_MUX, peri1_apb_clk_sels, PERIPH1_CFG0, 4, 1, 0, SYNC_INVALID) \
    MUX_PARENTS(TS_CLK_TOP_OSC_QSPI_AHB_CLK_MUX, qspi_ahb_clk_sels, QSPI_CFG, 4, 1, 0, SYNC_INVALID)    \
    MUX_PARENTS(TS_CLK_TOP_OSC_QSPI_REF_CLK_MUX, qspi_ref_clk_sels, QSPI_CFG, 6, 1, 0, SYNC_INVALID)    \
    MUX_PARENTS(TS_CLK_PERIPH2_SDHC0_CORE_CLK_MUX, sdhc0_core_clk_sels, SDHC_CFG0, 5, 1, 0, SYNC_INVALID) \
    MUX_PARENTS(TS_CLK_PERIPH2_SDHC1_CORE_CLK_MUX, sdhc1_core_clk_sels, SDHC_CFG1, 5, 1, 0, SYNC_INVALID) \

#define _MUXIFY(id) TS_CLK_TX5336_MUX_##id
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

#define COMP(id) \
	COMP_FULL(id, MUXIFY(id), DIVIFY(id), GATEIFY(id))
#define COMP_NOMUX(id) \
	COMP_FULL(id, TS_CLK_MUX_NONE, DIVIFY(id), GATEIFY(id))
#define COMP_NOGATE(id) \
	COMP_FULL(id, MUXIFY(id), DIVIFY(id), TS_CLK_GATE_NONE)
#define COMP_NODIV(id) \
	COMP_FULL(id, MUXIFY(id), TS_CLK_DIV_NONE, GATEIFY(id))
#define COMP_MUX_ONLY(id) \
	COMP_FULL(id, MUXIFY(id), TS_CLK_DIV_NONE, TS_CLK_GATE_NONE)
#define COMP_DIV_ONLY(id) \
	COMP_FULL(id, TS_CLK_MUX_NONE, DIVIFY(id), TS_CLK_GATE_NONE)
#define COMP_GATE_ONLY(id) \
	COMP_FULL(id, TS_CLK_MUX_NONE, TS_CLK_DIV_NONE, GATEIFY(id))
#define COMP_MUX_ONLY(id) \
	COMP_FULL(id, MUXIFY(id), TS_CLK_DIV_NONE, TS_CLK_GATE_NONE)
#define COMP_LIST \
    COMP_DIV_ONLY(TS_CLK_TX5336_PLL0_CPU_CLK_DIV)   \
    COMP_DIV_ONLY(TS_CLK_TX5336_PLL1_CPU_CLK_DIV)   \
    COMP_DIV_ONLY(TS_CLK_TX5336_PLL0_D2_GIC_CLK_DIV) \
    COMP_DIV_ONLY(TS_CLK_CPU_AXI_CLK_DIV)   \
    COMP_DIV_ONLY(TS_CLK_DAPLITE_DBG_PCLK_DIV)   \
    COMP_DIV_ONLY(TS_CLK_PLL0_D2_PERI0_APB_CLK_DIV)   \
    COMP_DIV_ONLY(TS_CLK_PLL0_D2_PERI1_APB_CLK_DIV)   \
    COMP_DIV_ONLY(TS_CLK_TOP_OSC_QSPI_AHB_CLK_DIV) \
    COMP_DIV_ONLY(TS_CLK_TOP_OSC_QSPI_REF_CLK_DIV) \
    COMP_DIV_ONLY(TS_CLK_PERIPH2_PLL0_SDHC0_CORE_CLK_DIV)   \
    COMP_DIV_ONLY(TS_CLK_PERIPH2_PLL0_SDHC1_CORE_CLK_DIV)   \
    COMP_MUX_ONLY(TS_CLK_TX5336_PLL_CPU_CLK_MUX)   \
    COMP_MUX_ONLY(TS_CLK_TX5336_CPU_CLK_MUX)   \
    COMP_MUX_ONLY(TS_CLK_TX5336_GIC_CLK_MUX)   \
    COMP_MUX_ONLY(TS_CLK_PERI0_APB_CLK_MUX)     \
    COMP_MUX_ONLY(TS_CLK_PERI1_APB_CLK_MUX)     \
    COMP_MUX_ONLY(TS_CLK_TOP_OSC_QSPI_AHB_CLK_MUX)  \
    COMP_NODIV(TS_CLK_PERIPH2_SDHC0_CORE_CLK_MUX)  \
    COMP_NODIV(TS_CLK_PERIPH2_SDHC1_CORE_CLK_MUX)  \
    COMP_MUX_ONLY(TS_CLK_TOP_OSC_QSPI_REF_CLK_MUX)  \
    COMP_GATE_ONLY(TS_CLK_PERI0_APB_CLK_GATE)   \
    COMP_GATE_ONLY(TS_CLK_PERI1_APB_CLK_GATE)   \
    COMP_GATE_ONLY(TS_CLK_TX5336_OTPC_APB_CLK_GATE) \
    COMP_GATE_ONLY(TS_CLK_PERIPH1_QSPI_APB_CLK_GATE) \
    COMP_GATE_ONLY(TS_CLK_PERIPH0_IOMUX_APB_CLK_GATE) \
    COMP_GATE_ONLY(TS_CLK_PERIPH0_GPADC_APB_CLK_GATE) \
    COMP_GATE_ONLY(TS_CLK_PERIPH0_GPIO_APB_CLK_GATE) \
    COMP_GATE_ONLY(TS_CLK_TOP_OSC_QSPI_AHB_CLK_GATE) \
    COMP_GATE_ONLY(TS_CLK_TOP_OSC_QSPI_REF_CLK_GATE) \
    COMP_GATE_ONLY(TS_CLK_PERIPH2_SDHC0_AHB_CLK_GATE) \
    COMP_GATE_ONLY(TS_CLK_PERIPH2_SDHC1_AHB_CLK_GATE) \
    COMP_GATE_ONLY(TS_CLK_PERIPH2_OSC_SDHC0_CORE_CLK_GATE)  \
    COMP_GATE_ONLY(TS_CLK_PERIPH2_OSC_SDHC1_CORE_CLK_GATE)  \
    COMP_GATE_ONLY(TS_CLK_PERIPH1_PWM_APB_CLK_GATE) \

#define _COMPIFY(id) TS_CLK_COMP_##id
#define COMPIFY(id) _COMPIFY(id)

enum ts_comp_ids {
#define COMP_FULL(id, ...) COMPIFY(id),
	COMP_LIST
#undef COMP_FULL
};

static const struct ts_comp_params ts_tx5336_comps[] = {
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

static int clk_tx5336_is_frac_divider(const char *name)
{
	u32 i;
	char *frac_div_arr[] = {"uart0_sclk", "uart1_sclk"};

	for (i = 0; i < ARRAY_SIZE(frac_div_arr); i++) {
		if (!strncmp(frac_div_arr[i], name, strlen(frac_div_arr[i])))
			return true;
	}
	return false;
}

static struct clk_div_table oclk_div_table[] = {
	{ 0, 1 }, { 1, 3 }, { 2, 5 }, { 3, 9 }
};
struct clk_div_table *clk_tx5336_get_div_table(const char *name)
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
	struct clk *clk;
	u32 div_id = 0;
	struct clk_div_table *table;

	if (params->mux == TS_CLK_MUX_NONE) {
		if (!parent) {
			//printf("params->mux:%d div:%d gate:%d \n",params->mux, params->div, params->gate);
			//printf("MUX_NONE with parent not NULL\n");
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
		if (clk_tx5336_is_frac_divider(name)) {
			div_ops = &clk_fractional_divider_ops;
			div = (struct clk_fractional_divider *)ts_create_fractional_div(&ts_tx5336_divs[div_id], base);
			clk = &((struct clk_fractional_divider *)div)->clk;
		} else {
			div_ops = &clk_divider_ops;
			table = clk_tx5336_get_div_table(name);
			div = (struct clk_divider *)ts_create_div(&ts_tx5336_divs[div_id], base, table);
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
		gate = ts_create_gate(&ts_tx5336_gates[params->gate], base);

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

static int clk_tx5336_register_plls(struct ts_clk_params *paras)
{
	void *top_base = paras->top_base;
	void __iomem *reg;
	void __iomem *lock;
	int i;

    // printf(" ======== %s pll register at %d \n", __func__, __LINE__);

	for (i = 0; i < ARRAY_SIZE(ts_tx5336_plls); i++) {
		struct ts_pll_params *params = &ts_tx5336_plls[i];
		reg = top_base + params->off;
		lock = top_base + params->lock_off;

		clk_dm(params->id,
			ts_register_pll(params->name, &osc_in[0], reg, lock,
				params->shift, params->width, PLL_REG_MODE_COMPACT));
	}

	/* fix-frequency clocks for plls */
	clk_dm(TS_CLK_TX5336_PLL0_D2,
		ts_clk_fixed_divider("pll0_d2", "pll0", 2));
	clk_dm(TS_CLK_TX5336_PLL0_D4,
		ts_clk_fixed_divider("pll0_d4", "pll0", 4));
	clk_dm(TS_CLK_TX5336_PLL0_D5,
		ts_clk_fixed_divider("pll0_d5", "pll0", 5));
	clk_dm(TS_CLK_TX5336_PLL0_D8,
		ts_clk_fixed_divider("pll0_d8", "pll0", 8));
	clk_dm(TS_CLK_TX5336_PLL0_D10,
		ts_clk_fixed_divider("pll0_d10", "pll0", 10));
	clk_dm(TS_CLK_TX5336_PLL0_D27,
		ts_clk_fixed_divider("pll0_d27", "pll0", 27));
	clk_dm(TS_CLK_TX5336_PLL1_D3,
		ts_clk_fixed_divider("pll1_d3", "pll1", 3));
	clk_dm(TS_CLK_TX5336_PLL1_D4,
		ts_clk_fixed_divider("pll1_d4", "pll1", 4));
	clk_dm(TS_CLK_TX5336_PLL1_D6,
		ts_clk_fixed_divider("pll1_d6", "pll1", 6));
	clk_dm(TS_CLK_TX5336_PLL0_D2_D5_SDHC0_AHB_CLK,
		ts_clk_fixed_divider("pll0_d2_d5_sdhc0_ahb_clk", "pll0_d2", 5));
	clk_dm(TS_CLK_TX5336_PLL0_D2_D5_SDHC1_AHB_CLK,
		ts_clk_fixed_divider("pll0_d2_d5_sdhc1_ahb_clk", "pll0_d2", 5));
#if 0
	/* divider "div0_codec_dac_cclk" is parent of comp "aud_dac_cclk"), register here */
	const struct ts_comp_params *params;
	params = &ts_tx5336_comps[DIVIFY(TS_CLK_TX5336_DIV0_CODEC_DAC_CCLK)];
	clk_dm(TS_CLK_TX5336_DIV0_CODEC_DAC_CCLK,
		clk_tx5336_register_comp(params, top_base, "div0_codec_dac_cclk", "pll1"));

	/* divider "div0_codec_adc_cclk" is parent of comp "aud_adc_cclk"), register here */
	params = &ts_tx5336_comps[DIVIFY(TS_CLK_TX5336_DIV0_CODEC_ADC_CCLK)];
	clk_dm(TS_CLK_TX5336_DIV0_CODEC_ADC_CCLK,
		clk_tx5336_register_comp(params, top_base, "div0_codec_adc_cclk", "pll1"));
#endif
	return 0;
}

static int clk_tx5336_register_comps(struct ts_clk_params *paras)
{
	return 0;
}

static int clk_tx5336_register_comps_without_mux(struct ts_clk_params *paras)
{

	return 0;
}


static int clk_tx5336_register_dividers(struct ts_clk_params *paras)
{

	return 0;
}

static int clk_tx5336_register_gates(struct ts_clk_params *paras)
{
	return 0;
}
#if 1
static int clk_tx5336_register_all(struct ts_clk_params *paras)
{
	void *top_base = paras->top_base;

#define REGISTER_GATE(id, name, parent) \
        clk_dm(id, \
            clk_tx5336_register_comp(&ts_tx5336_comps[COMPIFY(id)], \
                top_base, name, parent))

#define REGISTER_DIV(id, name, parent) \
		clk_dm(id, \
			clk_tx5336_register_comp(&ts_tx5336_comps[COMPIFY(id)], \
				top_base, name, parent))

#define REGISTER_COMP(id, name) \
		clk_dm(id, \
			clk_tx5336_register_comp(&ts_tx5336_comps[COMPIFY(id)], \
							top_base, name, NULL))

    REGISTER_COMP(TS_CLK_TX5336_CPU_CLK_MUX, "cpu_clk_mux");
    REGISTER_GATE(TS_CLK_PERI0_APB_CLK_GATE, "peri0_apb_clk_gate", &osc_in[0]);
    REGISTER_GATE(TS_CLK_PERI1_APB_CLK_GATE, "peri1_apb_clk_gate", &osc_in[0]);

    REGISTER_DIV(TS_CLK_TX5336_PLL0_CPU_CLK_DIV, "pll0_cpu_clk_div", "pll0");
    REGISTER_DIV(TS_CLK_TX5336_PLL1_CPU_CLK_DIV, "pll1_cpu_clk_div", "pll1");
    REGISTER_DIV(TS_CLK_TX5336_PLL0_D2_GIC_CLK_DIV, "pll0_d2_gic_clk_div", "pll0_d2");
    REGISTER_DIV(TS_CLK_CPU_AXI_CLK_DIV, "cpu_axi_clk_div", "cpu_clk_mux");
    REGISTER_DIV(TS_CLK_DAPLITE_DBG_PCLK_DIV, "daplite_dbg_pclk_div", "cpu_clk_mux");
    REGISTER_DIV(TS_CLK_PLL0_D2_PERI0_APB_CLK_DIV, "pll0_d2_peri0_apb_clk_div", "pll0_d2");
    REGISTER_DIV(TS_CLK_PLL0_D2_PERI1_APB_CLK_DIV, "pll0_d2_peri1_apb_clk_div", "pll0_d2");

    REGISTER_DIV(TS_CLK_TOP_OSC_QSPI_AHB_CLK_DIV, "qspi_ahb_clk_div", "pll0_d2");
    REGISTER_DIV(TS_CLK_TOP_OSC_QSPI_REF_CLK_DIV, "qspi_ref_clk_div", "pll0_d2");
    REGISTER_DIV(TS_CLK_PERIPH2_PLL0_SDHC0_CORE_CLK_DIV, "pll0_sdhc0_core_clk_div" ,"pll0");
    REGISTER_DIV(TS_CLK_PERIPH2_PLL0_SDHC1_CORE_CLK_DIV, "pll0_sdhc1_core_clk_div" ,"pll0");

    REGISTER_COMP(TS_CLK_PERI0_APB_CLK_MUX, "peri0_apb_clk_mux");
    REGISTER_COMP(TS_CLK_PERI1_APB_CLK_MUX, "peri1_apb_clk_mux");

    REGISTER_GATE(TS_CLK_PERIPH1_QSPI_APB_CLK_GATE, "qspi_apb_clk_icg", "peri0_apb_clk_mux");
    REGISTER_GATE(TS_CLK_PERIPH0_GPIO_APB_CLK_GATE, "gpio_apb_clk_icg", "peri0_apb_clk_mux");
    REGISTER_GATE(TS_CLK_PERIPH0_IOMUX_APB_CLK_GATE, "iomux_apb_clk_icg", "peri0_apb_clk_mux");
    REGISTER_GATE(TS_CLK_PERIPH0_GPADC_APB_CLK_GATE, "gpadc_apb_clk_icg", "peri0_apb_clk_mux");
    REGISTER_GATE(TS_CLK_TX5336_OTPC_APB_CLK_GATE, "otpc_pclk", "peri0_apb_clk_mux");
    REGISTER_GATE(TS_CLK_PERIPH1_PWM_APB_CLK_GATE, "pwm_pclk", "peri1_apb_clk_mux");
    REGISTER_GATE(TS_CLK_TOP_OSC_QSPI_AHB_CLK_GATE, "qspi_ahb_clk_icg", &osc_in[0]);
    REGISTER_GATE(TS_CLK_TOP_OSC_QSPI_REF_CLK_GATE, "qspi_ref_clk_icg", &osc_in[0]);
    REGISTER_GATE(TS_CLK_PERIPH2_SDHC0_AHB_CLK_GATE, "sdhc0_ahb_clk", "pll0_d2_d5_sdhc0_ahb_clk");
    REGISTER_GATE(TS_CLK_PERIPH2_SDHC1_AHB_CLK_GATE, "sdhc1_ahb_clk", "pll0_d2_d5_sdhc1_ahb_clk");
    REGISTER_GATE(TS_CLK_PERIPH2_OSC_SDHC0_CORE_CLK_GATE, "osc_sdhc0_core_clk_icg", &osc_in[0]);
    REGISTER_GATE(TS_CLK_PERIPH2_OSC_SDHC1_CORE_CLK_GATE, "osc_sdhc1_core_clk_icg", &osc_in[0]);

	REGISTER_COMP(TS_CLK_TX5336_PLL_CPU_CLK_MUX, "pll_cpu_clk_mux");
    REGISTER_COMP(TS_CLK_TX5336_GIC_CLK_MUX, "gic_clk_mux");
    REGISTER_COMP(TS_CLK_TOP_OSC_QSPI_AHB_CLK_MUX, "qspi_ahb_clk_mux");
    REGISTER_COMP(TS_CLK_TOP_OSC_QSPI_REF_CLK_MUX, "qspi_ref_clk_mux");
    REGISTER_COMP(TS_CLK_PERIPH2_SDHC0_CORE_CLK_MUX, "sdhc0_core_clk_mux");
    REGISTER_COMP(TS_CLK_PERIPH2_SDHC1_CORE_CLK_MUX, "sdhc1_core_clk_mux");

    // REGISTER_COMP(TS_CLK_TX5336_PLL0_CPU_CLK_DIV, "pll0_cpu_clk_div");
    // REGISTER_COMP(TS_CLK_TX5336_PLL1_CPU_CLK_DIV, "pll1_cpu_clk_div");


#undef REGISTER_DIV
#undef REGISTER_GATE
#undef REGISTER_COMP

    return 0;
}
#endif

int ts_clk_init_funcs(struct ts_clk_reg_funcs *pfuncs)
{
	if (!pfuncs)
		return -EINVAL;

	pfuncs->register_plls = clk_tx5336_register_plls;
	pfuncs->register_comps = clk_tx5336_register_comps;
	pfuncs->register_comps_without_mux = clk_tx5336_register_comps_without_mux;
	pfuncs->register_dividers = clk_tx5336_register_dividers;
	pfuncs->register_gates = clk_tx5336_register_gates;
    pfuncs->register_all = clk_tx5336_register_all;
	return 0;
}

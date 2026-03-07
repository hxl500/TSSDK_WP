/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright (C) 2022-2022 Tsingmicro, Inc.
 */
#ifndef TS_PLL_H
#define TS_PLL_H

#include <clk.h>
#include <asm/io.h>

#define PLL_REG_MODE_LOOSELY		0
#define PLL_REG_MODE_COMPACT		1

struct ts_pll {
	struct clk clk;
	void __iomem *reg; /* Base PLL register */
	void __iomem *lock; /* Common PLL lock register */
	u8 shift; /* Offset of bits in lock register */
	u8 width; /* Width of lock bits to test against */
	u8 reg_mode;  /*compact: CFG0-CFG2, loosely: += PLL_EN*/
};

#define to_ts_pll(_clk) container_of(_clk, struct ts_pll, clk)

// struct clk *ts_register_pll_struct(const char *name, const char *parent_name,
// 				     struct ts_pll *pll);
struct clk *ts_register_pll(const char *name, const char *parent_name,
			      void __iomem *reg, void __iomem *lock, u8 shift,
			      u8 width, u8 reg_mode);

#endif /* TS_PLL_H */

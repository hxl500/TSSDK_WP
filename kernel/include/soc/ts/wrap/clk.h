/***************************************************************************
 *                      COPYRIGHT NOTICE
 *               Copyright 2022 Tsingmicro, Inc.
 *                     All rights reserved.
 ***************************************************************************/

#ifndef __TS_WRAP_CLK_H__
#define __TS_WRAP_CLK_H__

#include <linux/clk.h>

/*
 * do not use clk_prepare_enable() directly, use this function
 */
int ts_clk_prepare_enable(struct clk *clk);


void ts_clk_disable_unprepare(struct clk *clk);


#endif


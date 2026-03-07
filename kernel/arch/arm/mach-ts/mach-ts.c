/*
 * Copyright 2013-2014 Freescale Semiconductor, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <asm/mach/arch.h>

//#include "common.h"
extern const struct smp_operations ts_smp_ops;

static const char * const ts_dt_compat_5215[] __initconst = {
	"tsingmicro,ts-tx5215",
	NULL,
};

DT_MACHINE_START(TSTX5215, "Tsingmicro TX5215")
	.smp		= smp_ops(ts_smp_ops),
	.dt_compat	= ts_dt_compat_5215,

MACHINE_END

static const char * const ts_dt_compat_5239[] __initconst = {
	"tsingmicro,ts-tx5239",
	NULL,
};

DT_MACHINE_START(TSTX5239, "Tsingmicro TX5239")
	.smp		= smp_ops(ts_smp_ops),
	.dt_compat	= ts_dt_compat_5239,

MACHINE_END

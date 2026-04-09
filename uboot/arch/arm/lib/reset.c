// SPDX-License-Identifier: GPL-2.0+
/*
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
 *
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Alex Zuepke <azu@sysgo.de>
 *
 * (C) Copyright 2002
 * Gary Jennejohn, DENX Software Engineering, <garyj@denx.de>
 *
 * (C) Copyright 2004
 * DAVE Srl
 * http://www.dave-tech.it
 * http://www.wawnet.biz
 * mailto:info@wawnet.biz
 *
 * (C) Copyright 2004 Texas Insturments
 */

#include <common.h>
#include <command.h>
#include <cpu_func.h>
#include <irq_func.h>
#include <linux/delay.h>

__weak void reset_misc(void)
{
#if CONFIG_ARCH_TS_TX536
	*(uint32_t *)(0xF12A2000 + 0x38) = 0x01;
#endif
#if CONFIG_ARCH_TS_TX5336
	*(uint32_t *)(0xf0517000 + 0x10) = 0x01;
#endif
#if CONFIG_ARCH_TS_TX5215
	*(uint32_t *)(0xF24e0000 + 0x10) = 0x01;
#endif
#if CONFIG_ARCH_TS_TX5112
	*(uint32_t *)(0xF24e0000 + 0x10) = 0x01;
#endif
}

int do_reset(struct cmd_tbl *cmdtp, int flag, int argc, char *const argv[])
{
	puts("resetting ...\n");

	mdelay(50);				/* wait 50 ms */

	disable_interrupts();

	reset_misc();
	reset_cpu(0);

	/*NOTREACHED*/
	return 0;
}

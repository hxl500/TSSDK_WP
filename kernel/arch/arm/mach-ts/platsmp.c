/*
 * Copyright 2011 Freescale Semiconductor, Inc.
 * Copyright 2011 Linaro Ltd.
 *
 * The code contained herein is licensed under the GNU General Public
 * License. You may obtain a copy of the GNU General Public License
 * Version 2 or later at the following locations:
 *
 * http://www.opensource.org/licenses/gpl-license.html
 * http://www.gnu.org/copyleft/gpl.html
 */

#include <linux/init.h>
#include <linux/of_address.h>
#include <linux/of.h>
#include <linux/smp.h>

#include <asm/cacheflush.h>
#include <asm/page.h>
#include <asm/smp_scu.h>
#include <asm/mach/map.h>

#define TSM_SRAM_SIZE 0x10000  //64K
#define TSM_MAILBOX_SIZE 256
#define TSM_MAILBOX_HOLD_BASE (TSM_SRAM_SIZE - TSM_MAILBOX_SIZE)
#define TSM_CORE_FLAG TSM_MAILBOX_HOLD_BASE
#define TSM_CORE_ENTRY (TSM_MAILBOX_HOLD_BASE + 4)
#define TSM_CORE_STATE_WAIT UL(0)
#define TSM_CORE_STATE_GO UL(1)
#define TSM_CORE_STATE_KICK UL(2)

static int ts_boot_secondary(unsigned int cpu, struct task_struct *idle)
{
	struct device_node *np;
	void __iomem *sram_base;
	unsigned long paddr;
	const char *name;
	int err;
	
	np = of_find_compatible_node(NULL, NULL, "ts,armv7-cpucfg");
	if (!np) {
		pr_info("Missing CPU sram config node in the device tree\n");
		return 0;
	}    

	err = of_property_read_string(np, "status", &name);
	if((err < 0) || strcasecmp(name, "okay"))
		return 0;
        
	sram_base = of_iomap(np, 0);
	BUG_ON(!sram_base);


	paddr = __pa_symbol(secondary_startup);
	writel_relaxed(paddr, sram_base + TSM_CORE_ENTRY);
	writel_relaxed(TSM_CORE_STATE_KICK, sram_base + TSM_CORE_FLAG);

	arch_send_wakeup_ipi_mask(cpumask_of(cpu));
	//printk("^^^^^^^^^^^^^paddr %lu sram_base %p sram_base %p\r\n",paddr,sram_base, sram_base+TSM_CORE_ENTRY);
	return 0;
}

static void __init ts_smp_prepare_cpus(unsigned int max_cpus)
{
	return;
}

const struct smp_operations ts_smp_ops __initconst = {
	.smp_prepare_cpus	= ts_smp_prepare_cpus,
	.smp_boot_secondary	= ts_boot_secondary,
};

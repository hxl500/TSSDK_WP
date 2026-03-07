// SPDX-License-Identifier: GPL-2.0+
/*
 * (C) Copyright 2014 - 2015 Xilinx, Inc.
 * Michal Simek <michal.simek@xilinx.com>
 */

#include <common.h>
#include <command.h>
#include <cpu_func.h>
#include <debug_uart.h>
#include <env.h>
#include <init.h>
#include <log.h>
#include <net.h>
#include <ahci.h>
#include <scsi.h>
#include <malloc.h>
#include <wdt.h>
#include <asm/arch/clk.h>
#include <asm/arch/hardware.h>
#include <asm/arch/sys_proto.h>
#include <asm/arch/psu_init_gpl.h>
#include <asm/cache.h>
#include <asm/io.h>
#include <asm/ptrace.h>
#include <dm/device.h>
#include <dm/uclass.h>
#include <usb.h>
#include <dwc3-uboot.h>
#include <tx536.h>
#include <g_dnl.h>
#include <linux/bitops.h>
#include <linux/delay.h>
#include <linux/sizes.h>
#include <fdt_support.h>
#include <env_internal.h>
#include "board.h"

DECLARE_GLOBAL_DATA_PTR;

enum boot_sel ts_get_boot_sel(void)
{
	uint32_t chip_status;
	uint32_t boot_sel;

	chip_status = readl(TS_SMU_BASE + TS_SMU_CHIP_STATUS_OFF);
	boot_sel = (chip_status & CHIP_STA_BOOT_SEL_MASK) >> CHIP_STA_BOOT_SEL_SHIFT;

	return boot_sel;
}

int board_early_init_f(void)
{
#ifdef CONFIG_DEBUG_UART
	/* Uart debug for sure */
	debug_uart_init();
	puts("Debug uart enabled\n"); /* or printch() */
#endif

	return 0;
}

static int multi_boot(void)
{
	u32 multiboot;

	multiboot = readl(&csu_base->multi_boot);

	printf("Multiboot:\t%d\n", multiboot);

	return 0;
}

int board_init(void)
{
	printf("EL Level:\tEL%d\n", current_el());
	/* Bug in ROM sets wrong value in this register */
	if (current_el() == 3)
		multi_boot();

	return 0;
}

int board_early_init_r(void)
{
#if (CONFIG_TS_HASH | CONFIG_TS_SKE | CONFIG_TS_PKE)
	int ret = 0;
	struct udevice *dev;
#endif

#if CONFIG_TS_HASH
	ret = uclass_get_device_by_driver(UCLASS_MISC, DM_GET_DRIVER(ts_hash),
						  &dev);
	if (ret)
		pr_err("%s get ts hash err %d\n", __func__, ret);
#endif

#if CONFIG_TS_SKE
	ret = uclass_get_device_by_driver(UCLASS_MISC,
					  DM_GET_DRIVER(ts_ske),
					  &dev);
	if (ret)
		pr_err("%s get ts ske err %d\n", __func__, ret);
#endif

#if CONFIG_TS_PKE
	ret = uclass_get_device_by_driver(UCLASS_MISC,
					  DM_GET_DRIVER(ts_pke),
					  &dev);
	if (ret)
		pr_err("%s get ts pke err %d\n", __func__, ret);
#endif

	if (current_el() != 3)
		return 0;

	return 0;
}

unsigned long do_go_exec(ulong (*entry)(int, char * const []), int argc,
			 char *const argv[])
{
	int ret = 0;

	if (current_el() > 1) {
		smp_kick_all_cpus();
		dcache_disable();
		armv8_switch_to_el1(0x0, 0, 0, 0, (unsigned long)entry,
				    ES_TO_AARCH64);
	} else {
		printf("FAIL: current EL is not above EL1\n");
		ret = EINVAL;
	}
	return ret;
}

#if !defined(CONFIG_SYS_SDRAM_BASE) && !defined(CONFIG_SYS_SDRAM_SIZE)
int dram_init_banksize(void)
{
	int ret;

	ret = fdtdec_setup_memory_banksize();
	if (ret)
		return ret;

	mem_map_fill();

	return 0;
}

int dram_init(void)
{
	if (fdtdec_setup_mem_size_base() != 0)
		return -EINVAL;

	return 0;
}
#else
int dram_init_banksize(void)
{
#if defined(CONFIG_NR_DRAM_BANKS)
	gd->bd->bi_dram[0].start = CONFIG_SYS_SDRAM_BASE;
	gd->bd->bi_dram[0].size = get_effective_memsize();
#endif

	mem_map_fill();

	return 0;
}

int dram_init(void)
{
	gd->ram_size = get_ram_size((void *)CONFIG_SYS_SDRAM_BASE,
				    CONFIG_SYS_SDRAM_SIZE);

	return 0;
}
#endif

void reset_cpu(ulong addr)
{

}

int boot_select_is_rawnand(void)
{
	uint32_t boot_sel = ts_get_boot_sel();

	switch (boot_sel) {
	case BOOT_SEL_OSPI_NOR_SPI:		/* OSPI port, SPI(1) */
	case BOOT_SEL_OSPI_3P3_NOR_SPI:		/* OSPI port, 3.3V, SPI(1) */
	case BOOT_SEL_OSPI_NOR_QSPI:	/* OSPI port, QSPI(4) */
	case BOOT_SEL_OSPI_3P3_NOR_QSPI:		/* OSPI port, 3.3V, QSPI(4) */
	case BOOT_SEL_OSPI_NOR_OSPI:		/* OSPI port, OSPI(8) */
	break;

	case BOOT_SEL_OSPI_NAND_SPI:		/* OSPI port, SPI(1) */
	case BOOT_SEL_OSPI_NAND_QSPI:		/* OSPI port, QSPI(4) */
	case BOOT_SEL_OSPI_3P3_NAND_SPI:	/* OSPI port, 3.3V, SPI(1) */
	case BOOT_SEL_OSPI_3P3_NAND_QSPI:	/* OSPI port, 3.3V, QSPI(4) */
	break;

	case BOOT_SEL_PARELLEL_NAND:
		return 1;
	break;

	case BOOT_SEL_SDIO0_EMMC_1LINE:
	case BOOT_SEL_SDIO0_EMMC_8LINE:
	case BOOT_SEL_SDIO1_SDCARD_1LINE:
	case BOOT_SEL_SDIO1_SDCARD_4LINE:

	break;

	case BOOT_SEL_UART_SLAVE:
	case BOOT_SEL_CK804_XIP:		/* Boot from ck804 XIP mode */

	default:
		return 0;
	break;
	}

	return 0;
}


int boot_select_is_emmc(void)
{
	uint32_t boot_sel = ts_get_boot_sel();

	switch (boot_sel) {
	case BOOT_SEL_OSPI_NOR_SPI:		/* OSPI port, SPI(1) */
	case BOOT_SEL_OSPI_3P3_NOR_SPI:		/* OSPI port, 3.3V, SPI(1) */
	case BOOT_SEL_OSPI_NOR_QSPI:	/* OSPI port, QSPI(4) */
	case BOOT_SEL_OSPI_3P3_NOR_QSPI:		/* OSPI port, 3.3V, QSPI(4) */
	case BOOT_SEL_OSPI_NOR_OSPI:		/* OSPI port, OSPI(8) */
	break;

	case BOOT_SEL_OSPI_NAND_SPI:		/* OSPI port, SPI(1) */
	case BOOT_SEL_OSPI_NAND_QSPI:		/* OSPI port, QSPI(4) */
	case BOOT_SEL_OSPI_3P3_NAND_SPI:	/* OSPI port, 3.3V, SPI(1) */
	case BOOT_SEL_OSPI_3P3_NAND_QSPI:	/* OSPI port, 3.3V, QSPI(4) */
	break;

	case BOOT_SEL_PARELLEL_NAND:
	break;

	case BOOT_SEL_SDIO0_EMMC_1LINE:
	case BOOT_SEL_SDIO0_EMMC_8LINE:
	case BOOT_SEL_SDIO1_SDCARD_1LINE:
	case BOOT_SEL_SDIO1_SDCARD_4LINE:
		return 1;
	break;

	case BOOT_SEL_UART_SLAVE:
	case BOOT_SEL_CK804_XIP:		/* Boot from ck804 XIP mode */

	default:
		return 2; //Always initialize EMMC
	break;
	}

	return 3; //Always initialize EMMC
}


int boot_select_is_ospi_nand(void)
{
	uint32_t boot_sel = ts_get_boot_sel();

	switch (boot_sel) {
	case BOOT_SEL_OSPI_NOR_SPI:		/* OSPI port, SPI(1) */
	case BOOT_SEL_OSPI_3P3_NOR_SPI:		/* OSPI port, 3.3V, SPI(1) */
	case BOOT_SEL_OSPI_NOR_QSPI:	/* OSPI port, QSPI(4) */
	case BOOT_SEL_OSPI_3P3_NOR_QSPI:		/* OSPI port, 3.3V, QSPI(4) */
	case BOOT_SEL_OSPI_NOR_OSPI:		/* OSPI port, OSPI(8) */
	break;

	case BOOT_SEL_OSPI_NAND_SPI:		/* OSPI port, SPI(1) */
	case BOOT_SEL_OSPI_NAND_QSPI:		/* OSPI port, QSPI(4) */
	case BOOT_SEL_OSPI_3P3_NAND_SPI:	/* OSPI port, 3.3V, SPI(1) */
	case BOOT_SEL_OSPI_3P3_NAND_QSPI:	/* OSPI port, 3.3V, QSPI(4) */
		return 1;
	break;

	case BOOT_SEL_PARELLEL_NAND:
	break;

	case BOOT_SEL_SDIO0_EMMC_1LINE:
	case BOOT_SEL_SDIO0_EMMC_8LINE:
	case BOOT_SEL_SDIO1_SDCARD_1LINE:
	case BOOT_SEL_SDIO1_SDCARD_4LINE:

	break;

	case BOOT_SEL_UART_SLAVE:
	case BOOT_SEL_CK804_XIP:		/* Boot from ck804 XIP mode */

	default:
		return 0;
	break;
	}

	return 0;
}

#if 1
enum env_location env_get_location(enum env_operation op, int prio)
{
	uint32_t boot_sel = ts_get_boot_sel();

	if (prio)
		return ENVL_UNKNOWN;

	switch (boot_sel) {
	case BOOT_SEL_OSPI_NOR_SPI:		/* OSPI port, SPI(1) */
	case BOOT_SEL_OSPI_3P3_NOR_SPI:		/* OSPI port, 3.3V, SPI(1) */
	case BOOT_SEL_OSPI_NOR_QSPI:	/* OSPI port, QSPI(4) */
	case BOOT_SEL_OSPI_3P3_NOR_QSPI:		/* OSPI port, 3.3V, QSPI(4) */
	case BOOT_SEL_OSPI_NOR_OSPI:		/* OSPI port, OSPI(8) */
	if (CONFIG_IS_ENABLED(ENV_IS_IN_SPI_FLASH))
		return ENVL_SPI_FLASH;
	else
		return ENVL_NOWHERE;
	break;

	case BOOT_SEL_OSPI_NAND_SPI:		/* OSPI port, SPI(1) */
	case BOOT_SEL_OSPI_NAND_QSPI:		/* OSPI port, QSPI(4) */
	case BOOT_SEL_OSPI_3P3_NAND_SPI:	/* OSPI port, 3.3V, SPI(1) */
	case BOOT_SEL_OSPI_3P3_NAND_QSPI:	/* OSPI port, 3.3V, QSPI(4) */
	if (CONFIG_IS_ENABLED(ENV_IS_IN_UBI))
		return ENVL_UBI;
	else
		return ENVL_NAND;
		//return ENVL_NOWHERE; //todo...
	break;

	case BOOT_SEL_PARELLEL_NAND:
	if (CONFIG_IS_ENABLED(ENV_IS_IN_UBI))
		return ENVL_UBI;
	else
		return ENVL_NAND;
		//return ENVL_NOWHERE;
	break;

	case BOOT_SEL_SDIO0_EMMC_1LINE:
	case BOOT_SEL_SDIO0_EMMC_8LINE:
	case BOOT_SEL_SDIO1_SDCARD_1LINE:
	case BOOT_SEL_SDIO1_SDCARD_4LINE:
	if (CONFIG_IS_ENABLED(ENV_IS_IN_MMC))
		return ENVL_MMC;
	else if (CONFIG_IS_ENABLED(ENV_IS_IN_EXT4))
		return ENVL_EXT4;
	else
		return ENVL_NOWHERE;

	break;

	case BOOT_SEL_UART_SLAVE:
	case BOOT_SEL_CK804_XIP:		/* Boot from ck804 XIP mode */

	default:
		return ENVL_NOWHERE;
	break;
	}

	return ENVL_NOWHERE;
}
#endif

int mmc_get_env_dev(void)
{
	uint32_t boot_sel = ts_get_boot_sel();

	switch (boot_sel) {
	case BOOT_SEL_SDIO0_EMMC_1LINE:
	case BOOT_SEL_SDIO0_EMMC_8LINE:
	return 0;

	case BOOT_SEL_SDIO1_SDCARD_1LINE:
	case BOOT_SEL_SDIO1_SDCARD_4LINE:
	return 1;

	case BOOT_SEL_OSPI_NOR_SPI:		/* OSPI port, SPI(1) */
	case BOOT_SEL_OSPI_3P3_NOR_SPI:		/* OSPI port, 3.3V, SPI(1) */
	case BOOT_SEL_OSPI_NOR_QSPI:	/* OSPI port, QSPI(4) */
	case BOOT_SEL_OSPI_3P3_NOR_QSPI:		/* OSPI port, 3.3V, QSPI(4) */
	case BOOT_SEL_OSPI_NOR_OSPI:		/* OSPI port, OSPI(8) */
	case BOOT_SEL_OSPI_NAND_SPI:		/* OSPI port, SPI(1) */
	case BOOT_SEL_OSPI_NAND_QSPI:		/* OSPI port, QSPI(4) */
	case BOOT_SEL_OSPI_3P3_NAND_SPI:	/* OSPI port, 3.3V, SPI(1) */
	case BOOT_SEL_OSPI_3P3_NAND_QSPI:	/* OSPI port, 3.3V, QSPI(4) */
	case BOOT_SEL_PARELLEL_NAND:
	case BOOT_SEL_UART_SLAVE:
	case BOOT_SEL_CK804_XIP:		/* Boot from ck804 XIP mode */

	default:
	#if defined(CONFIG_MMC_SDHCI) && defined(CONFIG_ENV_IS_IN_MMC)
		return CONFIG_SYS_MMC_ENV_DEV;
	#endif
	break;
	}

	#if defined(CONFIG_MMC_SDHCI) && defined(CONFIG_ENV_IS_IN_MMC)
	return CONFIG_SYS_MMC_ENV_DEV;
	#else
	return 0;
	#endif
}

int board_info_get_fdt(void)
{
	char boardname[32] = {"ts-tx536-xx"};
	char boardserial[150] = {"xxx-a53-xxx-2211"}, boardrev[150] = {"V1.0"};
	bool save_env = false;
	const char *str;

	str = fdt_get_board_model();
	if (str) {
		snprintf(boardname, sizeof(boardname), "%s", str);
		if (env_get("boardname") && strcmp(boardname, env_get("boardname")))
			save_env = true;

		env_set("boardname", boardname);
	}

#if !defined(CONFIG_OF_SEPARATE)
	uint32_t fdt_base_addr = 0;
	set_working_fdt_addr(env_get_hex("fdtcontroladdr", fdt_base_addr));

	str = fdt_get_board_revision();
	if (str) {
		snprintf(boardrev, sizeof(boardrev), "%s", str);
		if (env_get("boardrev") &&
		    strcmp(boardrev, env_get("boardrev")))
			save_env = true;
		env_set("boardrev", boardrev);
	}

	str = fdt_get_board_serial();
	if (str) {
		snprintf(boardserial, sizeof(boardserial), "%s", str);
		if (env_get("boardserial") &&
		    strcmp(boardserial, env_get("boardserial")))
			save_env = true;
		env_set("boardserial", boardserial);
	}

	/* Some boot devices have not been initiallized */
	//if (save_env)
		//env_save();
#endif

	printf("Board: %s-%s-%s\n", boardname, boardrev, boardserial);
	return 0;
}

int checkboard(void)
{
	board_info_get_fdt();
	return 0;
}

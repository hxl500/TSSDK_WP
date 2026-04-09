/* SPDX-License-Identifier: GPL-2.0 */
/*
 * (C) Copyright 2022 TsingMicro, Inc,
 */

#ifndef _TS_TX5336_H_
#define _TS_TX5336_H_

#include <linux/bitops.h>

#define TS_SMU_BASE					0xF0517000
#define TS_SMU_CHIP_STATUS_OFF		0x70
#define CHIP_STA_BOOT_SEL_SHIFT		0
#define CHIP_STA_BOOT_SEL_MASK		0x07

/*******************************************************************************
 * Function and variable prototypes
 ******************************************************************************/
enum boot_sel {
	BOOT_SEL_OSPI_NOR_SPI = 0,		/* OSPI port, SPI(1) */
	BOOT_SEL_OSPI_3P3_NOR_SPI = 1,		/* OSPI port, 3.3V, SPI(1) */
	BOOT_SEL_OSPI_NAND_SPI = 2,		/* OSPI port, SPI(1) */
	BOOT_SEL_OSPI_3P3_NAND_SPI = 3,	/* OSPI port, 3.3V, SPI(1) */
	BOOT_SEL_SDIO0_EMMC_1LINE = 4,
	BOOT_SEL_UART_SLAVE  = 5,
	BOOT_SEL_SDIO1_SDCARD_1LINE = 6,
	BOOT_SEL_CK804_XIP = 7,		/* Boot from ck804 XIP mode */

	BOOT_SEL_OSPI_NOR_QSPI = 8,		/* OSPI port, QSPI(4) */
	BOOT_SEL_OSPI_3P3_NOR_QSPI = 9,		/* OSPI port, 3.3V, QSPI(4) */
	BOOT_SEL_OSPI_NAND_QSPI = 10,		/* OSPI port, QSPI(4) */
	BOOT_SEL_OSPI_3P3_NAND_QSPI = 11,	/* OSPI port, 3.3V, QSPI(4) */
	BOOT_SEL_SDIO0_EMMC_8LINE = 12,
	BOOT_SEL_SDIO1_SDCARD_4LINE = 14,
	BOOT_SEL_OSPI_NOR_OSPI = 13,		/* OSPI port, OSPI(8) */
	BOOT_SEL_PARELLEL_NAND = 15,
	BOOT_SEL_COUNT = 16,
};

enum boot_sel ts_get_boot_sel(void);


struct tx5336_model {
	const char *boardname;
	const char *fdtfile;
	bool has_onboard_eth;
};

enum tx5336_mode_id {
	TX5336_EVB = 0,
	TX5336_BOX,
	TX5336_EVB_BOX,
};

static const struct tx5336_model tx5336_models_scheme[] = {
	[TX5336_EVB] = {
		"TS-TX5336-EVB",
		"ts-tx5336-evb.dtb",
		true,
	},
	[TX5336_BOX] = {
		"TS-TX5336-BOX",
		"ts-tx5336-box.dtb",
		true,
	},
	[TX5336_EVB_BOX] = {
		"TS-TX5336-EVB-BOX",
		"ts-tx5336-evb-box.dtb",
		true,
	},
};


#endif /* _TX5336_H_ */

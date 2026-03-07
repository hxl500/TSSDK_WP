/* SPDX-License-Identifier: GPL-2.0 */
/*
 * (C) Copyright 2022 TsingMicro, Inc,
 */

#ifndef _TX536_H_
#define _TX536_H_

#include <linux/bitops.h>

#define TS_SMU_BASE					0xF12A2000
#define TS_SMU_CHIP_STATUS_OFF		0xEC
#define CHIP_STA_BOOT_SEL_SHIFT		4
#define CHIP_STA_BOOT_SEL_MASK		0xF0

/*******************************************************************************
 * Function and variable prototypes
 ******************************************************************************/
enum boot_sel {
	BOOT_SEL_OSPI_NOR_SPI = 0,		/* OSPI port, SPI(1) */
	BOOT_SEL_OSPI_NOR_QSPI = 1,		/* OSPI port, QSPI(4) */
	BOOT_SEL_OSPI_NOR_OSPI = 2,		/* OSPI port, OSPI(8) */
	BOOT_SEL_OSPI_NAND_SPI = 3,		/* OSPI port, SPI(1) */
	BOOT_SEL_OSPI_NAND_QSPI = 4,		/* OSPI port, QSPI(4) */
	BOOT_SEL_OSPI_3P3_NOR_SPI = 5,		/* OSPI port, 3.3V, SPI(1) */
	BOOT_SEL_OSPI_3P3_NOR_QSPI = 6,		/* OSPI port, 3.3V, QSPI(4) */
	BOOT_SEL_PARELLEL_NAND = 7,
	BOOT_SEL_SDIO0_EMMC_1LINE = 8,
	BOOT_SEL_SDIO0_EMMC_8LINE = 9,
	BOOT_SEL_SDIO1_SDCARD_1LINE = 10,
	BOOT_SEL_SDIO1_SDCARD_4LINE = 11,
	BOOT_SEL_UART_SLAVE = 12,
	BOOT_SEL_OSPI_3P3_NAND_SPI = 13,	/* OSPI port, 3.3V, SPI(1) */
	BOOT_SEL_OSPI_3P3_NAND_QSPI = 14,	/* OSPI port, 3.3V, QSPI(4) */
	BOOT_SEL_CK804_XIP = 15,		/* Boot from ck804 XIP mode */
	BOOT_SEL_COUNT
};

enum boot_sel ts_get_boot_sel(void);


struct tx536_model {
	const char *boardname;
	const char *fdtfile;
	bool has_onboard_eth;
};

enum tx536_mode_id {
	TX536_EVB = 0,
	TX536_BOX,
	TX536_EVB_BOX,
};

static const struct tx536_model tx536_models_scheme[] = {
	[TX536_EVB] = {
		"TS-TX536-EVB",
		"ts-tx536-evb.dtb",
		true,
	},
	[TX536_BOX] = {
		"TS-TX536-BOX",
		"ts-tx536-box.dtb",
		true,
	},
	[TX536_EVB_BOX] = {
		"TS-TX536-EVB-BOX",
		"ts-tx536-evb-box.dtb",
		true,
	},
};


#endif /* _TX536_H_ */

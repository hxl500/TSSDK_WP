/* SPDX-License-Identifier: GPL-2.0 */
/*
 * (C) Copyright 2022 TsingMicro, Inc,
 */

#ifndef _TX52xx_H_
#define _TX52xx_H_

#include <linux/bitops.h>

#define TS_SMU_BASE            0xF24E0000UL
#define SMU_CHIP_STATUS (TS_SMU_BASE + 0x70)
#define SMU_BOOT_FLAG   (TS_SMU_BASE + 0x100)
#define SMU_SYS_CNT_RST (TS_SMU_BASE + 0x110)
#define BOOT_MODE_MASK  0x7

/*******************************************************************************
 * Function and variable prototypes
 ******************************************************************************/

enum boot_sel {
	BOOT_SEL_QSPI_FLASH_18 = 0,
	BOOT_SEL_QSPI_FLASH_33 = 1,
	BOOT_SEL_QSPI_NAND_18  = 2,
	BOOT_SEL_QSPI_NAND_33  = 3,
	BOOT_SEL_EMMC          = 4,
	BOOT_SEL_UART           = 5,
	BOOT_SEL_QSPI_FLASH_1L_XIP = 6,
	BOOT_SEL_QSPI_FLASH_4L_XIP = 7,
};

enum boot_sel ts_get_boot_sel(void);


struct tx52xx_model {
	const char *boardname;
	const char *fdtfile;
	bool has_onboard_eth;
};

enum tx52xx_mode_id {
	TX52xx_EVB = 0,
	TX52xx_IPC,
	TX52xx_PTZ,
};

static const struct tx52xx_model tx52xx_models_scheme[] = {
	[TX52xx_EVB] = {
		"TS-TX52xx-EVB",
		"ts-tx52xx-evb.dtb",
		true,
	},
	[TX52xx_IPC] = {
		"TS-TX52xx-IPC",
		"ts-tx52xx-ipc.dtb",
		true,
	},
	[TX52xx_PTZ] = {
		"TS-TX52xx-PTZ",
		"ts-tx52xx-ptz.dtb",
		true,
	},
};

#endif /* _TX52xx_H_ */

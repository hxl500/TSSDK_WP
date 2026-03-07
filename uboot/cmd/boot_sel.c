// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (c) 2022 TsingMicro.Inc
 */

#include <common.h>
#include <command.h>
#include <asm/io.h>
#include <env.h>
#include <malloc.h>
#ifdef CONFIG_ARCH_TS_TX536
#include <tx536.h>
#endif

#ifdef CONFIG_ARCH_TS_TX5336
#include <tx5336.h>
#endif

#ifdef CONFIG_ARCH_TS_TX5336
#include <tx5336.h>
#endif

#ifdef CONFIG_ARCH_TS_TX5215
#include <tx52xx.h>
#endif

#ifdef CONFIG_ARCH_TS_TX5112
#include <tx51xx.h>
#endif

#if defined(CONFIG_ARCH_TS_TX5336)
static int do_boot_sel(struct cmd_tbl *cmdtp, int flag, int argc,
		    char * const argv[])
{
	bool save_env = false;
	uint32_t boot_sel = ts_get_boot_sel();
	uint32_t device_id = 0;

	device_id = env_get_ulong("device_id", 10, 0);

	switch (boot_sel) {
	case BOOT_SEL_OSPI_NOR_SPI:		/* OSPI port, SPI(1) */
	case BOOT_SEL_OSPI_3P3_NOR_SPI:		/* OSPI port, 3.3V, SPI(1) */
	case BOOT_SEL_OSPI_NOR_QSPI:	/* OSPI port, QSPI(4) */
	case BOOT_SEL_OSPI_3P3_NOR_QSPI:		/* OSPI port, 3.3V, QSPI(4) */
	case BOOT_SEL_OSPI_NOR_OSPI:		/* OSPI port, OSPI(8) */
	env_set("bootmode", "boot_spinor");
	//printf("Booting from SPI-NOR\n");
	break;

	case BOOT_SEL_OSPI_NAND_SPI:		/* OSPI port, SPI(1) */
	case BOOT_SEL_OSPI_NAND_QSPI:		/* OSPI port, QSPI(4) */
	case BOOT_SEL_OSPI_3P3_NAND_SPI:	/* OSPI port, 3.3V, SPI(1) */
	case BOOT_SEL_OSPI_3P3_NAND_QSPI:	/* OSPI port, 3.3V, QSPI(4) */
	env_set("bootmode", "boot_spinand");
	//printf("Booting from SPI-NAND\n");
	break;

	case BOOT_SEL_SDIO0_EMMC_1LINE:
	case BOOT_SEL_SDIO0_EMMC_8LINE:
	env_set("bootmode", "boot_emmc");
	//printf("Booting from EMMC\n");
	break;

	case BOOT_SEL_SDIO1_SDCARD_1LINE:
	case BOOT_SEL_SDIO1_SDCARD_4LINE:
	env_set("bootmode", "boot_sd");
	//printf("Booting from SD\n");
	break;

	case BOOT_SEL_UART_SLAVE:
	env_set("bootmode", "boot_uart");
	//printf("Booting from UART\n");
	break;

	default:
	env_set("bootmode", "boot_uart");
	break;
	}

	if (save_env)
		env_save();
	return boot_sel;
}
#endif

#if defined(CONFIG_ARCH_TS_TX536)
static int do_boot_sel(struct cmd_tbl *cmdtp, int flag, int argc,
		    char * const argv[])
{
	bool save_env = false;
	uint32_t boot_sel = ts_get_boot_sel();
	uint32_t device_id = 0;

	device_id = env_get_ulong("device_id", 10, 0);

	switch (boot_sel) {
	case BOOT_SEL_OSPI_NOR_SPI:		/* OSPI port, SPI(1) */
	case BOOT_SEL_OSPI_3P3_NOR_SPI:		/* OSPI port, 3.3V, SPI(1) */
	case BOOT_SEL_OSPI_NOR_QSPI:	/* OSPI port, QSPI(4) */
	case BOOT_SEL_OSPI_3P3_NOR_QSPI:		/* OSPI port, 3.3V, QSPI(4) */
	case BOOT_SEL_OSPI_NOR_OSPI:		/* OSPI port, OSPI(8) */
	env_set("bootmode", "boot_spinor");
	//printf("Booting from SPI-NOR\n");
	break;

	case BOOT_SEL_OSPI_NAND_SPI:		/* OSPI port, SPI(1) */
	case BOOT_SEL_OSPI_NAND_QSPI:		/* OSPI port, QSPI(4) */
	case BOOT_SEL_OSPI_3P3_NAND_SPI:	/* OSPI port, 3.3V, SPI(1) */
	case BOOT_SEL_OSPI_3P3_NAND_QSPI:	/* OSPI port, 3.3V, QSPI(4) */
	env_set("bootmode", "boot_spinand");
	//printf("Booting from SPI-NAND\n");
	break;

	case BOOT_SEL_PARELLEL_NAND:
	env_set("bootmode", "boot_nand");

	//printf("Booting from NAND\n");
	break;

	case BOOT_SEL_SDIO0_EMMC_1LINE:
	case BOOT_SEL_SDIO0_EMMC_8LINE:
	env_set("bootmode", "boot_emmc");

	//printf("Booting from EMMC\n");
	break;

	case BOOT_SEL_SDIO1_SDCARD_1LINE:
	case BOOT_SEL_SDIO1_SDCARD_4LINE:
	env_set("bootmode", "boot_sd");

	//printf("Booting from SD\n");
	break;

	case BOOT_SEL_UART_SLAVE:
	env_set("bootmode", "boot_uart");
	//printf("Booting from UART\n");
	break;

	case BOOT_SEL_CK804_XIP:		/* Boot from ck804 XIP mode */
	printf("Booting from CK804\n");
	break;

	default:
	env_set("bootmode", "boot_emmc");
	break;
	}

	/* update bootmode and ipaddr */
	switch (device_id) {
	case 1:
	if (strcmp(env_get("ipaddr"), env_get("ipaddr1"))) {
		env_set("serverip", env_get("ipaddr0"));
		env_set("ipaddr", env_get("ipaddr1"));
		save_env = true;
	}

	if (strcmp(env_get("bootmode"), "boot_spinor") == 0)
		env_set("bootmode", "tftp_kernel");
	break;
	case 2:
	if (strcmp(env_get("ipaddr"), env_get("ipaddr2"))) {
		env_set("serverip", env_get("ipaddr0"));
		env_set("ipaddr", env_get("ipaddr2"));
		save_env = true;
	}
	if (strcmp(env_get("bootmode"), "boot_spinor") == 0)
		env_set("bootmode", "tftp_kernel");
	break;
	case 3:
	if (strcmp(env_get("ipaddr"), env_get("ipaddr3"))) {
		env_set("serverip", env_get("ipaddr0"));
		env_set("ipaddr", env_get("ipaddr3"));
		save_env = true;
	}
	if (strcmp(env_get("bootmode"), "boot_spinor") == 0)
		env_set("bootmode", "tftp_kernel");
	break;
	case 0:
	default:
		break;
	}

	if (save_env)
		env_save();

	return boot_sel;
}
#endif


#if defined(CONFIG_ARCH_TS_TX5215) || defined(CONFIG_ARCH_TS_TX5112)

#ifdef CONFIG_MTD_SPI_NAND
unsigned char boot_from_part = 1;
int get_boot_from_partition_group(void)
{
	char cmd_buf[128] ={0};
	unsigned char *data_buf = NULL;

	data_buf = malloc(128 *1024);
	if(data_buf != NULL) {
		//config partition
		sprintf(cmd_buf,"mtd read nand0 0x%p 0x080000 0x20000", data_buf);
		printf("cmd_buf: %s\n", cmd_buf);
		run_command(cmd_buf, 0);
		printf("########try_boot=%d, ok_boot=%d\n", data_buf[0], data_buf[1]);
		if(data_buf[0]%10 > 0 && data_buf[0] != 0XFF){
			if(1 == data_buf[0]%10){
				data_buf[0] = 0;
				boot_from_part = data_buf[1];

			} else {
				boot_from_part = data_buf[0]/10;
				data_buf[0] -= 1;
			}


			printf("decrease try boot flag to %d\n", data_buf[0]);

			run_command("mtd erase nand0 0x080000 0x20000", 0);
			sprintf(cmd_buf,"mtd write nand0 0x%p 0x080000 0x20000", data_buf);
			run_command(cmd_buf, 0);

		} else {
			//normal bootup
			boot_from_part = data_buf[1];
		}

		//invoid data_buf[1] value misc
		if(boot_from_part != 2)
			boot_from_part = 1;

		free(data_buf);
		data_buf = NULL;
	}

	printf("#boot_from_part: %d\n", boot_from_part);

	return boot_from_part;
}
#endif


static int do_boot_sel(struct cmd_tbl *cmdtp, int flag, int argc,
		    char * const argv[])
{
	bool save_env = false;
	uint32_t boot_sel = ts_get_boot_sel();
#ifdef CONFIG_MTD_SPI_NAND
	int boot_spinand_num = 0;
#endif
	switch (boot_sel) {
	case BOOT_SEL_QSPI_FLASH_18:
	case BOOT_SEL_QSPI_FLASH_33:
	case BOOT_SEL_QSPI_FLASH_1L_XIP:
	case BOOT_SEL_QSPI_FLASH_4L_XIP:
	env_set("bootmode", "boot_spinor");
	//printf("Booting from SPI-NOR\n");
	break;
#ifdef CONFIG_MTD_SPI_NAND
	case BOOT_SEL_QSPI_NAND_18:
	case BOOT_SEL_QSPI_NAND_33:
	env_set("bootmode", "boot_spinand");

	boot_spinand_num = get_boot_from_partition_group();
	if (boot_spinand_num == 1)
		env_set("boot_slot", "1");
	else if (boot_spinand_num == 2)
		env_set("boot_slot", "2");
	printf("Booting from SPI-NAND_%ld\n", env_get_ulong("boot_slot", 10, 0));
	break;
#endif
	case BOOT_SEL_EMMC:
	env_set("bootmode", "boot_emmc");

	//printf("Booting from EMMC\n");
	break;


	case BOOT_SEL_UART:
	env_set("bootmode", "boot_uart");
	//printf("Booting from UART\n");
	break;

	default:
	env_set("bootmode", "boot_spinor");
	break;
	}

	//save_env = true;

	if (save_env)
		env_save();

	return boot_sel;
}
#endif

U_BOOT_CMD(
	boot_sel,	1,	1,	do_boot_sel,
	"exec boot_sel command",
	"get boot pin value."
);

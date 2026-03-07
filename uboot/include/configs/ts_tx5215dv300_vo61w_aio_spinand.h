/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Configuration for Tsingmicro TX5215
 * (C) Copyright 2021 Tsingmicro, Inc.
 *
 */

#ifndef __TSINGMICRO_TX5215DV300_VO61W_AIO_SPINAND_H
#define __TSINGMICRO_TX5215DV300_VO61W_AIO_SPINAND_H
#include <linux/sizes.h>

//#define DEBUG

/*
 * TAGS
 */

#define CONFIG_CMDLINE_TAG
//#define CONFIG_SETUP_MEMORY_TAGS
//#define CONFIG_INITRD_TAG
//#define CONFIG_REVISION_TAG
//#define CONFIG_SERIAL_TAG

#define CONFIG_SYS_NAND_BASE	0xF0DA8038 //temp add by lyy

#define CONFIG_REMAKE_ELF

#define CONFIG_SYS_INIT_SP_ADDR		CONFIG_SYS_TEXT_BASE

/* Generic Timer Definitions - setup in EL3. Setup by ATF for other cases */
#if !defined(COUNTER_FREQUENCY)
#define COUNTER_FREQUENCY		100000000
#endif
//#define CONFIG_SYS_ARCH_TIMER
#define CONFIG_SYS_HZ_CLOCK		24000000
/* Size of malloc() pool */
#define CONFIG_SYS_MALLOC_LEN		(CONFIG_ENV_SIZE + 0x2000000)

#define CONFIG_SYS_BAUDRATE_TABLE \
	{ 4800, 9600, 19200, 38400, 57600, 115200 }

/* BOOTP options */
#define CONFIG_BOOTP_BOOTFILESIZE
#define CONFIG_BOOTP_MAY_FAIL

/* Miscellaneous configurable options */
#define CONFIG_SYS_LOAD_ADDR		0x8000000

#if !defined(DFU_ALT_INFO)
# define DFU_ALT_INFO
#endif

#if !defined(PARTS_DEFAULT)
# define PARTS_DEFAULT
#endif

/* Monitor Command Prompt */
/* Console I/O Buffer Size */
#define CONFIG_SYS_CBSIZE		2048
#define CONFIG_SYS_BARGSIZE		CONFIG_SYS_CBSIZE
#define CONFIG_SYS_MAXARGS		64

/* Ethernet driver */
#define CONFIG_SYS_FAULT_ECHO_LINK_DOWN
#define PHY_ANEG_TIMEOUT       20000

#define CONFIG_SYS_BOOTM_LEN	(128 * 1024 * 1024)

#define CONFIG_CLOCKS

#define ENV_MEM_LAYOUT_SETTINGS \
	"fdt_addr_r=0x40000000\0" \
	"fdt_size_r=0x400000\0" \
	"pxefile_addr_r=0x10000000\0" \
	"kernel_addr_r=0x18000000\0" \
	"kernel_size_r=0x10000000\0" \
	"scriptaddr=0x20000000\0" \
	"ramdisk_addr_r=0x02100000\0" \
	"script_size_f=0x80000\0" \

#define BOOT_TARGET_DEVICES_MMC(func)
#define BOOT_TARGET_DEVICES_SCSI(func)
#define BOOT_TARGET_DEVICES_USB(func)
#define BOOT_TARGET_DEVICES_PXE(func)
#define BOOT_TARGET_DEVICES_DHCP(func)
#define BOOT_TARGET_DEVICES_QSPI(func)
#define BOOT_TARGET_DEVICES_NAND(func)

#define BOOTENV_DEV_QSPI(devtypeu, devtypel, instance) \
	"bootcmd_" #devtypel #instance "=sf probe " #instance " 0 0 && " \
		       "sf read $scriptaddr $script_offset_f $script_size_f && " \
		       "echo QSPI: Trying to boot script at ${scriptaddr} && " \
		       "source ${scriptaddr}; echo QSPI: SCRIPT FAILED: continuing...;\0"

#define BOOTENV_DEV_NAME_QSPI(devtypeu, devtypel, instance) \
	#devtypel #instance " "

#define BOOTENV_DEV_NAND(devtypeu, devtypel, instance) \
	"bootcmd_" #devtypel #instance "= nand info && " \
		       "nand read $scriptaddr $script_offset_f $script_size_f && " \
		       "echo NAND: Trying to boot script at ${scriptaddr} && " \
		       "source ${scriptaddr}; echo NAND: SCRIPT FAILED: continuing...;\0"

#define BOOTENV_DEV_NAME_NAND(devtypeu, devtypel, instance) \
	#devtypel #instance " "

#define BOOT_TARGET_DEVICES_JTAG(func)	func(JTAG, jtag, na)

#define BOOTENV_DEV_JTAG(devtypeu, devtypel, instance) \
	"bootcmd_jtag=echo JTAG: Trying to boot script at ${scriptaddr} && " \
		"source ${scriptaddr}; echo JTAG: SCRIPT FAILED: continuing...;\0"

#define BOOTENV_DEV_NAME_JTAG(devtypeu, devtypel, instance) \
	"jtag "

#define BOOT_TARGET_DEVICES(func) \
	BOOT_TARGET_DEVICES_JTAG(func) \
	BOOT_TARGET_DEVICES_MMC(func) \
	BOOT_TARGET_DEVICES_QSPI(func) \
	BOOT_TARGET_DEVICES_NAND(func) \
	BOOT_TARGET_DEVICES_USB(func) \
	BOOT_TARGET_DEVICES_SCSI(func) \
	BOOT_TARGET_DEVICES_PXE(func) \
	BOOT_TARGET_DEVICES_DHCP(func)

#include <config_distro_bootcmd.h>

/* Initial environment variables */
#ifndef CONFIG_EXTRA_ENV_SETTINGS
#define CONFIG_EXTRA_ENV_SETTINGS \
	ENV_MEM_LAYOUT_SETTINGS \
	BOOTENV \
	DFU_ALT_INFO
#endif

#define CONFIG_SPL_STACK		0xfffffffc
#define CONFIG_SPL_MAX_SIZE		0x40000

/* Just random location in OCM */
#define CONFIG_SPL_BSS_START_ADDR	0x0
#define CONFIG_SPL_BSS_MAX_SIZE		0x80000

/* u-boot is like dtb */
#define CONFIG_SPL_FS_LOAD_ARGS_NAME	"u-boot.bin"
#define CONFIG_SYS_SPL_ARGS_ADDR	0x8000000

/* ATF is my kernel image */
#define CONFIG_SPL_FS_LOAD_KERNEL_NAME	"atf-uboot.ub"
#define CONFIG_SYS_SPL_MALLOC_START	0x20000000
#define CONFIG_SYS_SPL_MALLOC_SIZE	0x100000

/* Undef unneeded configs */
#undef CONFIG_BOOTCOMMAND
#undef CONFIG_EXTRA_ENV_SETTINGS
#undef CONFIG_SYS_MALLOC_LEN

/* BOOTP options */
#undef CONFIG_BOOTP_BOOTFILESIZE
#undef CONFIG_BOOTP_MAY_FAIL
#undef CONFIG_SYS_CBSIZE
#define CONFIG_SYS_CBSIZE		1024


#define CONFIG_SYS_MALLOC_LEN	0x800000
#define CONFIG_TFTP_TSIZE

#define CONFIG_IPADDR		192.168.1.10
#define CONFIG_NETMASK		255.255.255.0
#define CONFIG_SERVERIP		192.168.1.20
#define CONFIG_GATEWAYIP	192.168.1.1

#define CONFIG_SYS_MAX_FLASH_BANKS_DETECT	1 /* nor flash detect */
/* MTD device partition table */
#if 0
#define TS_FIP_1		0x000000	//0
#define TS_HW_INFO		0x07c000	//496K
#define TS_FIP_MTD_SIZE	TS_HW_INFO //
#define TS_KERNEL_1		0x080000	//512K 4MB
#define TS_ROOTFS_1		0x480000	//4.5M 2.5MB
#define TS_RAMDISK_1	TS_ROOTFS_1
#define TS_USER_1		0x700000	//7M 3.5MB
#define TS_APP			0xA80000	//10.5M 2.375MB
#define TS_BACKUP		0xCE0000	//12.875M 2.375MB
#define TS_USER_2		0xF40000	//15.25M
#else
/* MTD device partition table for spinand*/
#define TS_FIP_1			0x000000	//0
#define TS_FIP_MTD_SIZE		0x080000
//env addr 0x1c0000 size 0x40000 (2block,256KB, envsize 64KB)
#define TS_ENV_1			0x080000
#define TS_KERNEL		0x100000		//start addr :1  M
#define TS_KERNEL_MTD_SIZE	0x500000		//kernel size:5  M
#define TS_ROOTFS		0x600000		//start addr :6  M
#define TS_ROOTFS_MTD_SIZE	0x1400000		//rootfs size:20 M
#define TS_SYSTEM_START		0x1A00000		//start addr :26 M
#define TS_SYSTEM_SIZE		0x1400000		//system size:20 M
#define TS_WWW_START		0x2E00000		//start addr :46 M
#define TS_WWW_SIZE		0x600000		//www size   :6  M
#define TS_MODEL_START		0x3400000		//start addr :52 M
#define TS_MODEL_SIZE		0x4A00000		//model size :74 M
#define TS_PARAM_START		0x7E00000		//start addr :124M
#define TS_PARAM_SIZE		0x100000		//param size :1  M
#define TS_RAMDISK_1		TS_ROOTFS_1
#define TS_ROOT_MTD_SIZE    0x4b00000
#define TS_ENV_MTD_SIZE     0x20000
#define TS_ALL_WITHOUT_CONFIG_SIZE		0x7EA0000	//127.0M
#endif

/* BLOCK device partition table */
#define TS_MMC_START	38
#define TS_MMC_ROOTFS	512
#define TS_MMC_UPDATE	1024

#define TS_WHOLE_IMAGE		0x00000000
#define TS_HW_INFO_MMC		0x800	//1M
#define TS_FIP_MMC_1		0x0000	//boot1 0M
#define TS_FIP_MMC_2		0x0000	//boot2 0M
#define TS_KERNEL_MMC_1		0x3000	//6M
#define TS_KERNEL_MMC_2		0xB000	//22M
#define TS_ROOTFS_MMC_1		0x13000	//38M
#define TS_ROOTFS_MMC_2		0x113000	//550M
#define TS_USER_MMC_1		0x213000	//1062M
#define TS_USER_MMC_2		0x413000	//2086M
//#define TS_LPJ				5000000		// CPU_CLK / 2
#define TS_LPJ				1000000		// CPU_CLK / 500HZ / 2



#define CONFIG_EXTRA_ENV_SETTINGS					\
		"boardname=TS-TX5215DV300-VO61W-AIO-SPINAND\0" \
		"fip_name=uboot.bin\0" \
		"kernel_name=kernel\0"  \
		"kernel_ramfs_name=ts-tx5215dv300-vo61w-aio-ramdisk.itb\0" \
		"fdt_high=0xffffffff\0" \
		"ini_name=update.ini\0" \
		"swupdate_flag=y\0" \
		"swupdate_reset_env=y\0" \
		"swupdate_fip_flag=n\0" \
		"swupdate_kernel_flag=n\0" \
		"swupdate_rootfs_flag=n\0" \
		"swupdate_usrfs_flag=n\0" \
		"swupdate_interface=mmc\0" \
		"swupdate_dev=0\0" \
		"swupdate_part=0\0" \
		"swupdate_dir=/\0" \
		"boot_dev=mmc\0" \
		"boot_slot=1\0" \
		"bootm_kernel=bootz ${loadaddr}\0" \
		"rootfs_ubi_name=rootfs.ubi\0"  \
		"system_ubi_name=system.ubi\0"  \
		"loadaddr=0x02000000\0" \
		"loadaddr_gz=0x00a00000\0" \
		"netdev=eth0\0"			\
		"mmcdev=0\0"			\
		"arg_debug_console=console=ttyS0,115200n8 ignore_loglevel earlycon=uart8250,mmio32,0xf2449000 \
		lpj=" __stringify(TS_LPJ) "\0"\
		"arg_console=console=ttyS0,115200n8  lpj=" __stringify(TS_LPJ) "\0"\
		"set_dev_args=setenv hw_args \0" \
		"spinandparts=mtdparts=spi0.0:1M(boot),5M(kernel),20M(rootfs),20M(system),6M(www),74M(model),1M(param)\0"\
		"mtdroot1=ubi.mtd=2,2048 root=ubi0:rootfs rootfstype=ubifs ro \0"\
		"switch_sd=mmc rescan; mmc dev 1; setenv mmcdev 1; setenv mmcroot ${sdroot}; run set_dev_args;\0" \
		"switch_spinand=mtd list; run set_dev_args;\0" \
		\
		"fip_mtd=" __stringify(TS_FIP_1) "\0" \
		"fip_mtd_size=" __stringify(TS_FIP_MTD_SIZE) "\0" \
		"spinand_name=nand0\0" \
		"write_fip_spinand=nand erase ${fip_mtd} ${fip_mtd_size}; nand write ${loadaddr} ${fip_mtd} ${fip_mtd_size}\0" \
		"update_fip_sd2spinand=run get_fip_spinand; \0" \
		"write_fip_spinand=nand erase ${fip_mtd} ${fip_mtd_size}; nand write ${loadaddr} ${fip_mtd} ${fip_mtd_size}\0" \
		\
		"kernel_mtd=" __stringify(TS_KERNEL) "\0"      \
		"kernel_mtd_size=" __stringify(TS_KERNEL_MTD_SIZE) " \0"   \
		"write_kernel_spinand=nand erase ${kernel_mtd} ${kernel_mtd_size}; nand write ${loadaddr} ${kernel_mtd} ${kernel_mtd_size}\0" \
		"update_kernel_sd2spinand=run get_kernel_spinand; \0" \
		"read_kernel1_spinand=nand read ${loadaddr} ${kernel_mtd} ${kernel_mtd_size}\0" \
		\
		"sd_kernel=run ramdisk_bootargs; while true; do if load ${swupdate_interface} ${swupdate_dev} ${loadaddr} ${swupdate_dir}/${kernel_ramfs_name}; then run bootm_kernel; else echo [SD]:FAILED KERNEL!; fi; done;\0"\
		\
		"rootfs_mtd=" __stringify(TS_ROOTFS) "\0"      \
		"rootfs_mtd_size=" __stringify(TS_ROOTFS_MTD_SIZE) "\0"   \
		"system_mtd=" __stringify(TS_SYSTEM_START) "\0"      \
		"system_mtd_size=" __stringify(TS_SYSTEM_SIZE) "\0"   \
		"env_mtd=" __stringify(TS_ENV_1) "\0"      \
		"env_mtd_size=" __stringify(TS_ENV_MTD_SIZE) "\0"   \
		"all_mtd=" __stringify(TS_FIP_1) "\0" \
		"all_mtd_size=0x7f00000\0"   \
		"write_rootfs_spinand=nand erase ${rootfs_mtd} ${rootfs_mtd_size}; nand write ${loadaddr} ${rootfs_mtd} ${rootfs_mtd_size}\0" \
		"write_system_spinand=nand erase ${system_mtd} ${system_mtd_size}; nand write ${loadaddr} ${system_mtd} ${system_mtd_size}\0" \
		"update_rootfs_sd2spinand=run get_rootfs_spinand; \0" \
		"update_system_sd2spinand=run get_system_spinand; \0" \
		"erase_env_spinand=nand erase ${env_mtd} ${env_mtd_size};\0" \
		"erase_root_spinand=nand erase ${root_mtd} ${root_mtd_size};\0" \
		\
		"swupdate_err=n\0"\
		"reset_env=if test ${swupdate_reset_env} = y; then env default -a; setenv swupdate_reset_env n; saveenv;fi\0" \
		"get_ini=setenv swupdate_err n; if load ${swupdate_interface} ${swupdate_dev} ${loadaddr} ${swupdate_dir}/${ini_name}; then ini uboot; ini kernel; ini rootfs; ini usrfs; ini env; else echo [GET]:no INI!; setenv swupdate_err y; fi; \0" \
		"get_fip_spinand=if load ${swupdate_interface} ${swupdate_dev} ${loadaddr} ${swupdate_dir}/${fip_name}; then run switch_spinand; run write_fip_spinand; setenv swupdate_fip_flag n; else echo [GET]:FAILED FIP!; setenv swupdate_err y; fi; \0" \
		"get_kernel_spinand=if load ${swupdate_interface} ${swupdate_dev} ${loadaddr} ${swupdate_dir}/${kernel_name}; then run switch_spinand; run write_kernel_spinand; setenv swupdate_kernel_flag n; else echo [GET]:FAILED KERNEL!; setenv swupdate_err y; fi; \0" \
		"get_rootfs_spinand=if load ${swupdate_interface} ${swupdate_dev} ${loadaddr} ${swupdate_dir}/${rootfs_ubi_name}; then run switch_spinand; run write_rootfs_spinand; else echo [GET]:FAILED ROOTFS!; setenv swupdate_err y; fi; \0" \
		"get_system_spinand=if load ${swupdate_interface} ${swupdate_dev} ${loadaddr} ${swupdate_dir}/${system_ubi_name}; then run switch_spinand; run write_system_spinand; else echo [GET]:FAILED ROOTFS!; setenv swupdate_err y; fi; \0" \
		"mtd_bootargs1=setenv bootargs ${arg_console} ${mtdroot1} ${spinandparts} ${hw_args}\0" \
		"boot_spinand=run switch_spinand; run mtd_bootargs${boot_slot}; run read_kernel${boot_slot}_spinand; run bootm_kernel\0" \
		"bootmode=boot_spinand\0"


#define CONFIG_BOOTCOMMAND \
"run reset_env; run swupdate_all; boot_sel; run ${bootmode}"
/*note default env not big*/


/* Size of malloc() pool */
#define CONFIG_SYS_NONCACHED_MEMORY	0x00100000

#endif /* __TSINGMICRO_TX5215_H */

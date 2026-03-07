/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Configuration for Tsingmicro TX5215
 * (C) Copyright 2021 Tsingmicro, Inc.
 *
 */

#ifndef __TSINGMICRO_TX5215DV200_IPC_H
#define __TSINGMICRO_TX5215DV200_IPC_H
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
#define PHY_MOTORCOMM_RMII2

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

/* Allow environment variable to be overwritten */
//#define CONFIG_ENV_OVERWRITE

#define CONFIG_ETHADDR		00:06:3B:FF:FF:FF
#define CONFIG_IPADDR		192.168.1.10
#define CONFIG_NETMASK		255.255.255.0
#define CONFIG_SERVERIP		192.168.1.20
#define CONFIG_GATEWAYIP	192.168.1.1

#define CONFIG_SYS_MAX_FLASH_BANKS_DETECT	1 /* nor flash detect */
/* MTD device partition table */
#define TS_FIP_1		0x000000	//0
#define TS_HW_INFO		0x07c000	//496K
//#define TS_FIP_MTD_SIZE	TS_HW_INFO //
#define TS_KERNEL_1		0x08000c	//512K + 12byte
#define TS_ROOTFS_1		0x480000	//4.5M
#define TS_FIP_MTD_SIZE	TS_ROOTFS_1 //
#define TS_RAMDISK_1	TS_ROOTFS_1
#define TS_USER_1		0x880000	//8.5M
#define TS_USER_2		0xe80000	//14.5M

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
#define TS_LPJ				5000000		// CPU_CLK / 2

#define CONFIG_EXTRA_ENV_SETTINGS					\
		"boardname=TS-TX5215DV200-IPC\0" \
		"fip_name=fip_tx5215dv200_ipc-fast.bin\0" \
		"kernel_name=ts-tx5215dv200-ipc.itb\0"  \
		"kernel_ramfs_name=ts-tx5215dv200-ipc-ramdisk.itb\0" \
		"fdt_high=0xffffffff\0" \
		"ini_name=update.ini\0" \
		"emmc_image_name=emmc.img.gz\0" \
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
		"bootm_kernel=bootz ${loadaddr}\0" \
		"rootfs_name=rootfs.ext4.gz\0"  \
		"rootfs_name_mtd=rootfs.initrd\0"  \
		"usrfs_name_mtd=system.squashfs\0"  \
		"loadaddr=0x02000000\0" \
		"loadaddr_gz=0x00a00000\0" \
		"netdev=eth0\0"			\
		"ethaddr=" __stringify(CONFIG_ETHADDR) "\0"				\
		"mmcdev=0\0"			\
		"ipaddr=" __stringify(CONFIG_IPADDR) "\0"\
		"arg_console=console=ttyS0,115200n8 ignore_loglevel earlycon=uart8250,mmio32,0xf2449000 lpj=" __stringify(TS_LPJ) "\0"\
		"set_dev_args=setenv hw_args \0" \
		"set_ip=setenv arg_ip ip=${ipaddr}:${serverip}:${gatewayip}:${netmask}:${boardname}:${netdev}:off\0" \
		"set_nfsroot=setenv arg_nfsroot nfsroot=${serverip}:${rootpath},nolock,v3\0" \
		"set_nfs=run set_ip; run set_nfsroot; setenv arg_nfs ${nfs_root} ${arg_nfsroot} ${arg_ip}\0"			\
		"nfs_root=root=/dev/nfs rw  rootwait hlt\0"\
		"mtdparts=mtdparts=f1000000.spi.0:512K(boot),4M(kernel),4M(romfs)ro,6M(user),-(root) \0"\
		"ramroot=root=/dev/ram0 rootfstype=ext4 rootwait rw \0"\
		"norroot=root=/dev/mtdblock2 rootfstype=squashfs init=/sbin/init\0"\
		"emmcroot=root=/dev/mmcblk0p1 rw init=/sbin/init rootwait\0"\
		"sdroot=root=/dev/mmcblk1p1 rw init=/sbin/init rootwait\0"\
		"nfsargs=setenv ipaddr ${ipaddr}; setenv serverip ${serverip}; run set_nfs; run set_dev_args;\0" \
		"switch_emmc=mmc rescan; mmc dev 0; setenv mmcdev 0; setenv mmcroot ${emmcroot}; run set_dev_args;\0" \
		"switch_sd=mmc rescan; mmc dev 1; setenv mmcdev 1; setenv mmcroot ${sdroot}; run set_dev_args;\0" \
		"switch_emmc_boot0=run switch_emmc; mmc partconf 0 0 1 1;\0" \
		"switch_spinand=mtd list; run set_dev_args;\0" \
		"switch_spinor=sf probe; run set_dev_args;\0" \
		\
		"fip_mtd=" __stringify(TS_FIP_1) "\0" \
		"fip_mtd_2=" __stringify(TS_FIP_2) "\0" \
		"fip_mtd_size=" __stringify(TS_FIP_MTD_SIZE) "\0" \
		"fip_emmc=" __stringify(TS_FIP_MMC_1) "\0" \
		"fip_emmc_2=" __stringify(TS_FIP_MMC_2) "\0" \
		"fip_mmc_size=0x1000\0" \
		"spinand_name=spi-nand0\0" \
		"write_fip_mmc=mmc write ${loadaddr} ${fip_emmc} ${fip_mmc_size}\0" \
		"write_fip_mmc_2=mmc write ${loadaddr} ${fip_emmc_2} ${fip_mmc_size}\0" \
		"write_fip_spinand=mtd erase ${spinand_name} ${fip_mtd} ${fip_mtd_size}; mtd write ${spinand_name} ${loadaddr} ${fip_mtd} ${fip_mtd_size}\0" \
		"write_fip_spinor=sf update ${loadaddr} ${fip_mtd} ${fip_mtd_size}\0" \
		"update_fip_emmc=run nfsargs; while true; do  if tftp ${loadaddr} ${fip_name}; then run switch_emmc_boot0; run write_fip_mmc; exit; else echo [TFTP]:FAILED FIP!; fi; done; \0" \
		"update_fip_sd=run nfsargs; while true; do  if tftp ${loadaddr} ${fip_name}; then run switch_sd; run write_fip_mmc; exit; else echo [TFTP]:FAILED FIP!; fi; done; \0" \
		"update_fip_spinand=run nfsargs; while true; do  if tftp ${loadaddr} ${fip_name}; then run switch_spinand; run write_fip_spinand; exit; else echo [TFTP]:FAILED FIP!; fi; done; \0" \
		"update_fip_spinor=run nfsargs; while true; do  if tftp ${loadaddr} ${fip_name}; then run switch_spinor; run write_fip_spinor; exit; else echo [TFTP]:FAILED FIP!; fi; done; \0" \
		"update_fip_sd2spinor=run get_fip; \0" \
		\
		"kernel_mtd=" __stringify(TS_KERNEL_1) "\0"      \
		"kernel_mtd_2=" __stringify(TS_KERNEL_2) "\0"      \
		"kernel_mtd_size=" __stringify(0x280000) "\0"   \
		"kernel_emmc=" __stringify(TS_KERNEL_MMC_1) "\0"      \
		"kernel_emmc_2=" __stringify(TS_KERNEL_MMC_2) "\0"      \
		"kernel_mmc_size=0x8000\0"   \
		"read_kernel_mmc=mmc read ${loadaddr} ${kernel_emmc} ${kernel_mmc_size}\0" \
		"read_kernel_spinor=sf read ${loadaddr} ${kernel_mtd} ${kernel_mtd_size}\0" \
		"write_kernel_mmc=mmc write ${loadaddr} ${kernel_emmc} ${kernel_mmc_size}\0" \
		"write_kernel_spinand=mtd erase ${spinand_name} ${kernel_mtd} ${kernel_mtd_size}; mtd write ${spinand_name} ${loadaddr} ${kernel_mtd} ${kernel_mtd_size}\0" \
		"write_kernel_spinor=sf update ${loadaddr} ${kernel_mtd} ${kernel_mtd_size}\0" \
		"write_kernel_sd=run switch_sd; run write_kernel_mmc\0" \
		"write_kernel_emmc=run switch_emmc; run write_kernel_mmc\0" \
		"update_kernel_sd=run nfsargs; while true; do  if tftp ${loadaddr} ${kernel_name}; then run write_kernel_sd; exit; else echo [TFTP]:FAILED KERNEL!; fi; done; \0" \
		"update_kernel_emmc=run nfsargs;while true; do   if tftp ${loadaddr} ${kernel_name}; then run write_kernel_emmc; exit; else echo [TFTP]:FAILED KERNEL!; fi; done; \0" \
		"update_kernel_spinand=run nfsargs;while true; do  if tftp ${loadaddr} ${kernel_name}; then run switch_spinand; run write_kernel_spinand; exit; else echo [TFTP]:FAILED KERNEL!; fi; done;  \0" \
		"update_kernel_spinor=run nfsargs;while true; do   if tftp ${loadaddr} ${kernel_name}; then run switch_spinor; run write_kernel_spinor; exit; else echo [TFTP]:FAILED KERNEL!; fi; done; \0" \
		"update_kernel_sd2spinor=run get_kernel; \0" \
		\
		"ramfs_emmc=" __stringify(TS_RAMDISK_MMC_1) "\0"      \
		"ramfs_emmc_2=" __stringify(TS_RAMDISK_MMC_2) "\0"      \
		"ramfs_mmc_size=0x36800\0"   \
		"ramfs_mtd=" __stringify(TS_RAMDISK_1) "\0"      \
		"ramfs_mtd_2=" __stringify(TS_RAMDISK_2) "\0"      \
		"ramfs_mtd_size=0x6d00000\0"   \
		"read_ramfs_mmc=mmc read ${loadaddr} ${ramfs_emmc} ${ramfs_mmc_size}\0" \
		"read_ramfs_spinand=mtd read ${spinand_name} ${loadaddr} ${ramfs_mtd} ${ramfs_mtd_size}\0" \
		"read_ramfs_spinor=sf read ${loadaddr} ${ramfs_mtd} ${ramfs_mtd_size}\0" \
		"write_ramfs_mmc=mmc write ${loadaddr} ${ramfs_emmc} ${ramfs_mmc_size}\0" \
		"write_ramfs_spinand=mtd erase ${spinand_name} ${ramfs_mtd} ${ramfs_mtd_size}; mtd write ${spinand_name} ${loadaddr} ${ramfs_mtd} ${ramfs_mtd_size}\0" \
		"write_ramfs_spinor=sf update ${loadaddr} ${ramfs_mtd} ${ramfs_mtd_size}\0" \
		"update_ramfs_sd=run nfsargs; while true; do  if tftp ${loadaddr} ${kernel_ramfs_name}; then run switch_sd; run write_ramfs_mmc; exit; else echo [TFTP]:FAILED KERNEL!; fi; done; \0" \
		"update_ramfs_emmc=run nfsargs; while true; do  if tftp ${loadaddr} ${kernel_ramfs_name}; then run switch_emmc; run write_ramfs_mmc; exit; else echo [TFTP]:FAILED KERNEL!; fi; done; \0" \
		"update_ramfs_spinand=run nfsargs; while true; do  if tftp ${loadaddr} ${kernel_ramfs_name}; then run switch_spinand; run write_ramfs_spinand; exit; else echo [TFTP]:FAILED KERNEL!; fi; done; \0" \
		"update_ramfs_spinor=run nfsargs; while true; do  if tftp ${loadaddr} ${kernel_ramfs_name}; then run switch_spinor; run write_ramfs_spinor; exit; else echo [TFTP]:FAILED KERNEL!; fi; done; \0" \
		"tftp_kernel=run nfsargs; run ramdisk_bootargs; while true; do if tftp ${loadaddr} ${kernel_ramfs_name}; then run bootm_kernel; else echo [TFTP]:FAILED KERNEL!; fi; done;\0"\
		"sd_kernel=run ramdisk_bootargs; while true; do if load ${swupdate_interface} ${swupdate_dev} ${loadaddr} ${swupdate_dir}/${kernel_ramfs_name}; then run bootm_kernel; else echo [SD]:FAILED KERNEL!; fi; done;\0"\
		\
		"rootfs_emmc=" __stringify(TS_ROOTFS_MMC_1) "\0"      \
		"rootfs_emmc_2=" __stringify(TS_ROOTFS_MMC_2) "\0"      \
		"rootfs_mmc_size=0xc0000\0"   \
		"rootfs_mtd_size=0x400000\0"   \
		"usrfs_mtd=" __stringify(TS_USER_1) "\0"      \
		"usrfs_mtd_size=0x00600000\0"   \
		"check_mmc_part=fdisk -c ${mmcdev} " __stringify(TS_MMC_START) " " __stringify(TS_MMC_ROOTFS) " \
" __stringify(TS_MMC_ROOTFS)" " __stringify(TS_MMC_UPDATE) " \0"      \
		"write_rootfs=run check_mmc_part; mmc write ${loadaddr} ${rootfs_emmc} ${rootfs_mmc_size}\0" \
		"write_rootfs_spinor=sf update ${loadaddr} ${ramfs_mtd} ${rootfs_mtd_size}\0" \
		"write_usrfs_spinor=sf update ${loadaddr} ${usrfs_mtd} ${usrfs_mtd_size}\0" \
		"update_rootfs_sd=run nfsargs; run switch_sd; while true; do if tftp ${loadaddr} ${rootfs_name}; then run write_rootfs; exit; else echo [TFTP]:FAILED ROOTFS!; fi; done;\0" \
		"update_rootfs_emmc=run nfsargs; run switch_emmc; while true; do if tftp ${loadaddr_gz} ${rootfs_name}; then unzip ${loadaddr_gz} ${loadaddr}; run write_rootfs; exit; else echo [TFTP]:FAILED ROOTFS!; fi;  done;\0" \
		"update_rootfs_spinor=run nfsargs; while true; do  if tftp ${loadaddr} ${rootfs_name_mtd}; then run switch_spinor; run write_rootfs_spinor; exit; else echo [TFTP]:FAILED ROOTFS!; fi; done; \0" \
		"update_usrfs_spinor=run nfsargs; while true; do  if tftp ${loadaddr} ${usrfs_name_mtd}; then run switch_spinor; run write_usrfs_spinor; exit; else echo [TFTP]:FAILED ROOTFS!; fi; done; \0" \
		"update_rootfs_sd2spinor=run get_rootfs; \0" \
		"update_usrfs_sd2spinor=run get_usrfs; \0" \
		\
		"image_emmc=" __stringify(TS_WHOLE_IMAGE) "\0"      \
		"image_mmc_size=0xd3000\0"   \
		"write_image_emmc=mmc write ${loadaddr} ${image_emmc} ${image_mmc_size}\0" \
		"update_image_emmc=run nfsargs; run switch_emmc; while true; do if tftp ${loadaddr_gz} ${emmc_image_name}; then unzip ${loadaddr_gz} ${loadaddr}; run write_image_emmc; exit; else echo [TFTP]:FAILED ROOTFS!; fi; done; \0" \
		\
		"swupdate_err=n\0"\
		"reset_env=if test ${swupdate_reset_env} = y; then env default -a; setenv swupdate_reset_env n; saveenv;fi\0" \
		"get_ini=setenv swupdate_err n; if load ${swupdate_interface} ${swupdate_dev} ${loadaddr} ${swupdate_dir}/${ini_name}; then ini uboot; ini kernel; ini rootfs; ini usrfs; ini env; else echo [GET]:no INI!; setenv swupdate_err y; fi; \0" \
		"get_fip=if load ${swupdate_interface} ${swupdate_dev} ${loadaddr} ${swupdate_dir}/${fip_name}; then run switch_spinor; run write_fip_spinor; setenv swupdate_fip_flag n; else echo [GET]:FAILED FIP!; setenv swupdate_err y; fi; \0" \
		"get_kernel=if load ${swupdate_interface} ${swupdate_dev} ${loadaddr} ${swupdate_dir}/${kernel_name}; then run switch_spinor; run write_kernel_spinor; setenv swupdate_kernel_flag n; else echo [GET]:FAILED KERNEL!; setenv swupdate_err y; fi; \0" \
		"get_ramfs=if load ${swupdate_interface} ${swupdate_dev}:${swupdate_part} ${loadaddr} ${swupdate_dir}/${kernel_ramfs_name}; then run switch_emmc; run write_ramfs_mmc; else echo [GET]:FAILED RAMFS!; fi; \0" \
		"get_rootfs=if load ${swupdate_interface} ${swupdate_dev} ${loadaddr} ${swupdate_dir}/${rootfs_name_mtd}; then run switch_spinor; run write_rootfs_spinor; else echo [GET]:FAILED ROOTFS!; setenv swupdate_err y; fi; \0" \
		"get_usrfs=if load ${swupdate_interface} ${swupdate_dev} ${loadaddr} ${swupdate_dir}/${usrfs_name_mtd}; then run switch_spinor; run write_usrfs_spinor; else echo [GET]:FAILED USRFS!; setenv swupdate_err y; fi;\0" \
		"swupdate_all= if test ${swupdate_flag} = y; then run get_ini; if test ${swupdate_fip_flag} = y; then echo [UPDATE] FIP...; run get_fip; fi; if test ${swupdate_kernel_flag} = y; then echo [UPDATE] KERNEL...; run get_kernel; fi; if test ${swupdate_rootfs_flag} = y; then echo [UPDATE] ROOTFS...; run get_rootfs; fi; if test ${swupdate_usrfs_flag} = y; then echo [UPDATE] USRFS...; run get_usrfs; fi; if test ${swupdate_err} = n; then saveenv; while true; do echo [UPDATE_ALL] DONE, Please remove sdcard and reset; sleep 5; done; else echo [UPDATE_ALL] NotDone.; fi; fi;\0"\
		"block_bootargs=setenv bootargs ${arg_console} ${mmcroot} ${hw_args}\0" \
		"nor_bootargs=setenv bootargs ${arg_console} ${norroot} ${mtdparts} ${hw_args}\0" \
		"ramdisk_bootargs=setenv bootargs ${arg_console} ${ramroot} ${mtdparts} ${hw_args}\0" \
		"nfs_bootargs=run set_nfs;setenv bootargs ${arg_console} ${arg_nfs} ${hw_args}\0" \
		"boot_ramfs_sd=run ramdisk_bootargs; run switch_sd; run read_ramfs_mmc; run  bootm_kernel\0"\
		"boot_ramfs_emmc=run ramdisk_bootargs; run switch_emmc; run read_ramfs_mmc; run  bootm_kernel\0"\
		"boot_ramfs_spinand=run ramdisk_bootargs; run switch_spinand; run read_ramfs_spinand; run bootm_kernel\0"\
		"boot_ramfs_spinor=run ramdisk_bootargs; run switch_spinor; run read_ramfs_spinor; run  bootm_kernel\0"\
		"boot_emmc=run switch_emmc; run block_bootargs; run read_kernel_mmc; run bootm_kernel\0" \
		"boot_spinand=run boot_ramfs_spinand\0" \
		"boot_spinor=run switch_spinor; run nor_bootargs; run read_kernel_spinor; run bootm_kernel\0" \
		"boot_nfs=run nfs_bootargs; run switch_emmc; run read_kernel_mmc; run bootm_kernel\0" \
		"bootmode=boot_spinor\0"


#define CONFIG_BOOTCOMMAND \
"run reset_env; run swupdate_all; run ${bootmode}"
/*note default env not big*/


/* Size of malloc() pool */
#define CONFIG_SYS_NONCACHED_MEMORY	0x00100000

#endif /* __TSINGMICRO_TX5215_H */

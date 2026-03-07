#!/bin/bash

export TARGET_UBOOT_DEFCONFIG="ts_tx5215dv300_vo61w_aio_spinand_v2p0_defconfig"
export TARGET_KERNEL_DEFCONFIG="ts_tx52xx_vo61w_aio_spinand_v2p0_defconfig"
export TARGET_KERNEL_RAMDISK_DEFCONFIG="ts_tx52xx_vo61w_aio_spinand_ramdisk_defconfig"
export TARGET_DTB="ts-tx5215dv300-v2p0-vo61w-aio-spinand.dtb"
export TARGET_FIT_ITS="ts-tx5215dv300-vo61w-aio-spinand.its"
export TARGET_FIT_ITB="ts-tx5215dv300-vo61w-aio-spinand.itb"
export TARGET_FIT_ITB_RAMDISK="ts-tx5215dv300-vo61w-aio-spinand-ramdisk.itb"
export TARGET_IMAGE_TYPE="nand"
export TARGET_ROOTFS_CUSTOM="spinand"
################################################################################
# 						common env import from fastboot
################################################################################
export TARGET_KERNEL_TYPE="compressed"
export TARGET_LIBS_TYPE="static"
export TARGET_KERNEL_SYSFS="enable"
export TARGET_KERNEL_DRV_EXPORT="disable"
export TARGET_MPP_PRELOAD="disable"
export TARGET_SDK_ISP_DRV_PACK="TRUE"
export TARGET_INSTALL_MOD_PATH=""
################################################################################
# 						fastboot private config below
################################################################################
export TARGET_ROOTFS_TYPE=""
export TARGET_SYSTEM_PARTITION=""
export TARGET_KERNEL_RELEASE=""
export TARGET_NET_TYPE=""
export TARGET_TEST_MODE=""
export TARGET_SDK_BOOT_MODE=""
export TARGET_FAST_APP=""
export TARGET_FAST_APP_PARA=""
export TARGET_DYNAMIC_DEV_FILE=""
export TARGET_SYSTEM_PART_PATH=""
################################################################################

#TARGET_LOADER_2="${TARGET_PREBUILTS_DIR}/tsbin/tx5215dv300/ddr_init.bin"
#TARGET_LOADER_2="${TARGET_PREBUILTS_DIR}/tsbin/tx5215dv300/ddr2_1333.bin"
TARGET_LOADER_2="${TARGET_PREBUILTS_DIR}/tsbin/tx5215dv300/ddr3_2133.bin"
TARGET_LOADER_2_NAND="${TARGET_PREBUILTS_DIR}/tsbin/tx5215dv300/ddr3_2133_nand_flash.bin"


################################################################################
compiler_uclibc
#compiler_glibc

#!/bin/bash

export TARGET_UBOOT_DEFCONFIG="ts_tx5215dv200_ipcv1p1_fast_defconfig"
export TARGET_KERNEL_DEFCONFIG="ts_tx52xx_ipc_v1p1_fast_defconfig"
export TARGET_KERNEL_RAMDISK_DEFCONFIG="ts_tx52xx_ipc_ramdisk_defconfig"
export TARGET_DTB="ts-tx5215dv200-ipcv1p1-fast.dtb"
export TARGET_FIT_ITS="ts-tx5215dv200-ipcv1p1.its"
export TARGET_FIT_ITB="ts-tx5215dv200-ipc.itb"
export TARGET_FIT_ITB_RAMDISK="ts-tx5215dv200-ipc-ramdisk.itb"
export TARGET_IMAGE_TYPE="initrd"
export TARGET_ROOTFS_CUSTOM=""
################################################################################
# 						common env import from fastboot
################################################################################
export TARGET_KERNEL_TYPE="uncompressed"
# export TARGET_LIBS_TYPE="shared"
export TARGET_LIBS_TYPE="static"
export TARGET_KERNEL_SYSFS="enable"
# export TARGET_KERNEL_SYSFS="enable"
export TARGET_KERNEL_DRV_EXPORT="enable"
export TARGET_MPP_PRELOAD="enable"
# normal|delay|kernel
# normal: user filesystems mount by rcS
# delay:  user filesystems not mount by rcS, user delay mount by self
# kernel: user filesystems mount by kernel
export TARGET_FS_MOUNT_MODE="normal"
export TARGET_SDK_ISP_DRV_PACK="TRUE"
export TARGET_INSTALL_MOD_PATH=${TARGET_KERNEL_DIR}/_modules
################################################################################
# 						fastboot private config below
################################################################################
export TARGET_ROOTFS_TYPE="tiny"
export TARGET_SYSTEM_PARTITION="enable"
export TARGET_KERNEL_RELEASE=4.14.249
# export TARGET_NET_TYPE="wifi"
export TARGET_NET_TYPE="ethernet"
export TARGET_TEST_MODE="disable"
export TARGET_SDK_BOOT_MODE="fastboot"
export TARGET_FAST_APP="sample_fastboot"
export TARGET_FAST_APP_PARA="0"
export TARGET_DYNAMIC_DEV_FILE="enable"
export TARGET_SYSTEM_PART_PATH=${TARGET_OUT_DIR}/system
################################################################################


TARGET_LOADER_2="${TARGET_PREBUILTS_DIR}/tsbin/tx5215dv200/ddr3_2133_fast.bin"

################################################################################
compiler_uclibc
#compiler_glibc

#!/bin/bash
###
 # @copyright (c) 2023 Tsingmicro Intelligent Technology Co., Limited
 # @FilePath: reconfig.sh
 # @brief:
 # @Author: ymc (yangmengchao@tsingmicro.com)
 # @Version: 1.0
 # @Date: 2023-06-27
###

PATH_CONFIG=/system/bin:/bin:/sbin:/usr/bin:/usr/sbin
LD_LIBRARY_PATH_CONFIG=/system/lib:/usr/lib
KCONFIG=${TARGET_KERNEL_DIR}/arch/arm/configs/${TARGET_KERNEL_DEFCONFIG}
RCS=etc/init.d/rcS
FSTAB=etc/fstab

function config_rcS()
{
    echo "======================================================================================="
    echo -e "[-RECONFIG rootfs \033[32m${RCS}\033[0m...]"

    if [ "x"${TARGET_FAST_APP} == "x""aov_loop" ] ; then
         sed -i '/\/bin\/mount.*/a\aov_alive' "${RCS}"
         #
         #	just for linux test stream, 
         # 	can move to load in kernel 
         # 	for shorten time.
         #
         sed -i "/S11load_ko/s/^# //" ${RCS}
    fi

    # env
    if [ "x"${TARGET_SYSTEM_PARTITION} == "x""enable" ] ; then
        sed -i "s|export PATH=.*|export PATH=|g" ${RCS}
        sed -i "s|export LD_LIBRARY_PATH=.*|export LD_LIBRARY_PATH=|g" ${RCS}
        sed -i "s|export PATH=|export PATH=${PATH_CONFIG}|g" ${RCS}
        sed -i "s|export LD_LIBRARY_PATH=|export LD_LIBRARY_PATH=${LD_LIBRARY_PATH_CONFIG}|g" ${RCS}
    fi
    # ajust mount fs
    if [ "x"${TARGET_IMAGE_TYPE} == "x""initrd" ] ; then
        # ajust mount system fs
        echo -e "[-TARGET_FS_MOUNT_MODE \033[32m${TARGET_FS_MOUNT_MODE}\033[0m...]"
        sed -i "s|mount -t squashfs.*||" ${RCS}
        if [ "x"${TARGET_FS_MOUNT_MODE} == "x""normal" ] ; then
            sed -i "/^mount -t jffs2/i\insmod_sfc" ${RCS}
            if [ "x"${TARGET_SYSTEM_PARTITION} == "x""enable" ] ; then
                sed -i "/# system-mount/a\mount -t squashfs -o loop /dev/mtdblock2 /system\n" ${RCS}
                sed -i "s|/usr/bin/sample_venc.*||g" ${RCS}
                if [ "x"${TARGET_CHIPSERIES} == "x""TX5112DX" ] ; then
                    sed -i "/insmod_sfc/i\/usr/bin/${TARGET_FAST_APP} ${TARGET_FAST_APP_PARA} &" ${RCS}
                else
                    sed -i "/insmod_sfc/i\/usr/bin/${TARGET_FAST_APP} &" ${RCS}
                fi
            fi
        elif [ "x"${TARGET_FS_MOUNT_MODE} == "x""delay" ] ; then
            if [ "x"${TARGET_SYSTEM_PARTITION} == "x""enable" ] ; then
                sed -i "s|^mount -t jffs2.*|# &|g" ${RCS}
                # now net ko in system ,so del net insmod cmd,
                # if need insmod fast,
                # can move net ko to initrd, then can keep net cmd
                sed -i "s|^insmod_ethernet.*|# &|g" ${RCS}
                sed -i "s|^ifconfig eth0.*|# &|g" ${RCS}
                sed -i "s|^telnetd.*|# &|g" ${RCS}
                sed -i "s|/usr/bin/sample_venc.*||g" ${RCS}
                if [ "x"${TARGET_CHIPSERIES} == "x""TX5112DX" ] ; then
                    sed -i "/# system-mount/i\/usr/bin/${TARGET_FAST_APP} ${TARGET_FAST_APP_PARA} &" ${RCS}
                else
                    sed -i "/# system-mount/i\/usr/bin/${TARGET_FAST_APP} &" ${RCS}
                fi
            fi
        elif [ "x"${TARGET_FS_MOUNT_MODE} == "x""kernel" ] ; then
            if [ "x"${TARGET_SYSTEM_PARTITION} == "x""enable" ] ; then
                sed -i "s|^mount -t jffs2.*|# &|g" ${RCS}
                # now same to ${TARGET_FS_MOUNT_MODE} == "delay",
                # in order to calc first encode time since powerup
                # except insmod net ko, user can insmod by shell later
                sed -i "s|^insmod_ethernet.*|# &|g" ${RCS}
                sed -i "s|^ifconfig eth0.*|# &|g" ${RCS}
                sed -i "s|^telnetd.*|# &|g" ${RCS}
                sed -i "s|/usr/bin/sample_venc.*||g" ${RCS}
                if [ "x"${TARGET_CHIPSERIES} == "x""TX5112DX" ] ; then
                    sed -i "/# system-mount/i\/usr/bin/${TARGET_FAST_APP} ${TARGET_FAST_APP_PARA} &" ${RCS}
                else
                    sed -i "/# system-mount/i\/usr/bin/${TARGET_FAST_APP} &" ${RCS}
                fi
            fi
        fi
    fi

    # scripts
    if [ "x"${TARGET_TEST_MODE} == "x""enable" ] ; then
        sed -i "/# net/i\/root/start.sh  &" ${RCS}
        sed -i "s|/usr/bin/${TARGET_FAST_APP}.*||g" ${RCS}
    fi
    # pre-load mpp ko
    if [ "x"${TARGET_MPP_PRELOAD} == "x""enable" ] ; then
        if [ "x"${TARGET_ROOTFS_TYPE} == "x""tiny" ] ; then
            sed -i "s|^insmod $LIBPAHT.*|# &|g" ${RCS}
        else
            sed -i "s|/etc/init.d/S11load_ko.*||g" ${RCS}
        fi
    fi    # mdev
    if [ "x"${TARGET_FS_MOUNT_MODE} != "x""kernel" ] ; then
        if [ "x"${TARGET_DYNAMIC_DEV_FILE} == "x""enable" -a "x"${TARGET_KERNEL_SYSFS} == "x""enable" ] ; then
            sed -i "/mdev -s/s/^# //" ${RCS}
        fi
    fi
    # modprobe
    sed -i '$a\echo "/sbin/modprobe" > /proc/sys/kernel/modprobe'  ${RCS}
    echo "[-DONE]"
    echo "======================================================================================="
}

function config_fstab()
{
    echo "======================================================================================="
    echo -e "[-RECONFIG rootfs \033[32m${FSTAB}\033[0m...]"


    if [ "x"${TARGET_KERNEL_SYSFS} == "x""disable" ] ; then
        sed -i "s|^sysfs.*|#&|" ${FSTAB}
    else
        sed -i "/sysfs/s/^#//" ${FSTAB}
    fi
    echo "[-DONE]"
    echo "======================================================================================="
}

function config_rootfs()
{
    local dir=$1

    echo "======================================================================================="
    echo "[ENTER $dir...]"
    echo "======================================================================================="
    pushd $dir
    config_rcS
    config_fstab
    popd
    echo "[EXIT $dir...]"
    echo "======================================================================================="
    echo "[DONE]"
    echo "======================================================================================="
}

function config_initrd()
{
    local initrd_size=$1

    echo "======================================================================================="
    echo "[-CONFIG INITRD:]"
    echo -e "[-INITRD_SIZE \033[32m$initrd_size Byte\033[0m]"

    # initrd size
    sed -i "s/initrd=0x1200000,[0-9]*/initrd=0x1200000,/g" ${KCONFIG}
    sed -i "s/initrd=0x1200000,[0-9]*/initrd=0x1200000,/g" ${KCONFIG}
    sed -i "s/initrd=0x1200000,/initrd=0x1200000,${initrd_size}/g" ${KCONFIG}

    echo "[-DONE]"
    echo "======================================================================================="
}

# only compile kernel
function config_defconfig()
{
    echo "======================================================================================="
    echo "[-CONFIG kernel defconfig:]"
    echo -e "[-TARGET_MPP_PRELOAD:\033[32m${TARGET_MPP_PRELOAD}\033[0m]"
    echo -e "[-TARGET_KERNEL_SYSFS:\033[32m${TARGET_KERNEL_SYSFS}\033[0m]"

    # mpp pre-load
    if [ "x"${TARGET_MPP_PRELOAD} == "x""disable" ] ; then
        sed -i "s|^CONFIG_MPP_DRV_LOAD.*|# &|g" ${KCONFIG}
        sed -i "s|^CONFIG_MPP_DRV_PATH.*|# &|g" ${KCONFIG}
    else
        sed -i "/CONFIG_MPP_DRV_LOAD/s/^# //g" ${KCONFIG}
        sed -i "/CONFIG_MPP_DRV_PATH/s/^# //g" ${KCONFIG}
    fi

    # usr fs mount by kernel
    if [ "x"${TARGET_FS_MOUNT_MODE} != "x""kernel" ] ; then
        sed -i "s|^CONFIG_USER_FS_MOUNT.*|# &|g" ${KCONFIG}
        sed -i "s|^CONFIG_USER_FS1_DEVPATH.*|# &|g" ${KCONFIG}
        sed -i "s|^CONFIG_USER_FS1_MOUNTPOINT.*|# &|g" ${KCONFIG}
        sed -i "s|^CONFIG_USER_FS1_TYPE.*|# &|g" ${KCONFIG}
        sed -i "s|^CONFIG_USER_FS2_DEVPATH.*|# &|g" ${KCONFIG}
        sed -i "s|^CONFIG_USER_FS2_MOUNTPOINT.*|# &|g" ${KCONFIG}
        sed -i "s|^CONFIG_USER_FS2_TYPE.*|# &|g" ${KCONFIG}
        sed -i 's/^CONFIG_MTD_SPI_NOR=y/CONFIG_MTD_SPI_NOR=m/' ${KCONFIG}
        sed -i 's/^CONFIG_SPI_CADENCE_QUADSPI_NOR=y/CONFIG_SPI_CADENCE_QUADSPI_NOR=m/' ${KCONFIG}
        sed -i 's/^CONFIG_JFFS2_FS=y/CONFIG_JFFS2_FS=m/' ${KCONFIG}
        sed -i 's/^CONFIG_SQUASHFS=y/CONFIG_SQUASHFS=m/' ${KCONFIG}
    else
        sed -i "/CONFIG_USER_FS_MOUNT/s/^# //g" ${KCONFIG}
        sed -i "/CONFIG_USER_FS1_DEVPATH/s/^# //g" ${KCONFIG}
        sed -i "/CONFIG_USER_FS1_MOUNTPOINT/s/^# //g" ${KCONFIG}
        sed -i "/CONFIG_USER_FS1_TYPE/s/^# //g" ${KCONFIG}
        sed -i "/CONFIG_USER_FS2_DEVPATH/s/^# //g" ${KCONFIG}
        sed -i "/CONFIG_USER_FS2_MOUNTPOINT/s/^# //g" ${KCONFIG}
        sed -i "/CONFIG_USER_FS2_TYPE/s/^# //g" ${KCONFIG}
        sed -i 's/^CONFIG_MTD_SPI_NOR=m/CONFIG_MTD_SPI_NOR=y/' ${KCONFIG}
        sed -i 's/^CONFIG_SPI_CADENCE_QUADSPI_NOR=m/CONFIG_SPI_CADENCE_QUADSPI_NOR=y/' ${KCONFIG}
        sed -i 's/^CONFIG_JFFS2_FS=m/CONFIG_JFFS2_FS=y/' ${KCONFIG}
        sed -i 's/^CONFIG_SQUASHFS=m/CONFIG_SQUASHFS=y/' ${KCONFIG}
    fi

    # sysfs relative
    if [ "x"${TARGET_KERNEL_SYSFS} == "x""disable" ] ; then
        # sysfs
        sed -i '/# CONFIG_SYSFS is not set/d' ${KCONFIG}
        sed -i "/CONFIG_TMPFS/i\# CONFIG_SYSFS is not set" ${KCONFIG}
        # iio
        sed -i "s|^CONFIG_IIO.*|# &|g" ${KCONFIG}
        sed -i "s|^CONFIG_TS_GPADC.*|# &|g" ${KCONFIG}
    else
        # sysfs
        sed -i '/# CONFIG_SYSFS is not set/d' ${KCONFIG}
        # iio
        sed -i "/CONFIG_IIO/s/^# //g" ${KCONFIG}
        sed -i "/CONFIG_TS_GPADC/s/^# //g" ${KCONFIG}
    fi

    echo "[-DONE]"
    echo "======================================================================================="
}

# note:
# compile sdk then compile kernel
# need modify initrd size
#
function config_kernel()
{
    local initrd_size=$1

    echo "======================================================================================="
    echo "[CONIFG:]"
    echo -e "[RECONFIG \033[32m$KCONFIG\033[0m]"

    config_initrd $initrd_size
    config_defconfig

    echo "[DONE]"
    echo "======================================================================================="
}

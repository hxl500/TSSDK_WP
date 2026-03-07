#!/bin/bash

###
 # @copyright (c) 2023 Tsingmicro Intelligent Technology Co., Limited
 # @FilePath: export_driver.sh
 # @brief:
 # @Author: ymc (yangmengchao@tsingmicro.com)
 # @Version: 1.0
 # @Date: 2023-07-27
####

SRC_DIR=${TARGET_INSTALL_MOD_PATH}/lib/modules/${TARGET_KERNEL_RELEASE}/kernel
DST_DIR=${TARGET_OUT_DIR}/rootfs/usr/lib/modules
LINK_DIR=${TARGET_OUT_DIR}/rootfs/lib/modules
LINK_ASSIST_DRV_DIR=usr
if [ -d ${TARGET_LIBS_DIR}/hichannel ]; \
	then \
	WIFI_DIR=${TARGET_LIBS_DIR}/hichannel
else	\
	WIFI_DIR=${TARGET_LIBS_DIR}/drv/hichannel
fi

# assistant drv dir
DST_ASSIST_DRV_DIR=${DST_DIR}
if [ ${TARGET_SYSTEM_PARTITION} == "enable" ] ; then
    DST_ASSIST_DRV_DIR=${TARGET_SYSTEM_PART_PATH}/lib/modules
    mkdir -p ${DST_ASSIST_DRV_DIR}
    LINK_ASSIST_DRV_DIR=system
fi

function copy_file()
{
    local file=$1
    local dstdir=$2

    if [ -f $file ]
    then
        echo "[-COPY $file TO ====> DSTDIR $dstdir]"
        cp -vaprf $file $dstdir
    else
        echo "[-NONE]"
    fi
}

function export_files()
{
    echo "======================================================================================="
	echo "[EXPORT DRV:]"
    echo "======================================================================================="

    #enter the kernel directory
    cd ${TARGET_KERNEL_DIR}
    KERNELRELEASE=`cat include/config/kernel.release 2> /dev/null`
    if [ "${KERNELRELEASE}" != "${TARGET_KERNEL_RELEASE}" ]; then
        echo "Please rebuild kernel"
        exit 1
    fi
    #return to the previous directory
    cd -
    # copy in kernel source drivers
    # nfs
    echo "======================================================================================="
	echo "[-NFS DRVs:]"
    echo "======================================================================================="
    copy_file ${SRC_DIR}/fs/lockd/lockd.ko         ${DST_ASSIST_DRV_DIR}
    copy_file ${SRC_DIR}/fs/nfs/nfs.ko             ${DST_ASSIST_DRV_DIR}
    copy_file ${SRC_DIR}/fs/nfs/nfsv2.ko           ${DST_ASSIST_DRV_DIR}
    copy_file ${SRC_DIR}/fs/nfs/nfsv3.ko           ${DST_ASSIST_DRV_DIR}
    copy_file ${SRC_DIR}/fs/nfs_common/grace.ko    ${DST_ASSIST_DRV_DIR}
    copy_file ${SRC_DIR}/net/sunrpc/sunrpc.ko      ${DST_ASSIST_DRV_DIR}

    # sd
    echo "======================================================================================="
	echo "[-MMC DRV:]"
    echo "======================================================================================="
    copy_file ${SRC_DIR}/drivers/mmc/core/mmc_core.ko       ${DST_ASSIST_DRV_DIR}
    copy_file ${SRC_DIR}/drivers/mmc/core/mmc_block.ko      ${DST_ASSIST_DRV_DIR}
    copy_file ${SRC_DIR}/drivers/mmc/host/dw_mmc.ko         ${DST_ASSIST_DRV_DIR}
    copy_file ${SRC_DIR}/drivers/mmc/host/dw_mmc-pltfm.ko   ${DST_ASSIST_DRV_DIR}
    copy_file ${SRC_DIR}/fs/nls/nls_cp936.ko                ${DST_ASSIST_DRV_DIR}
    copy_file ${SRC_DIR}/fs/fat/fat.ko                      ${DST_ASSIST_DRV_DIR}
    copy_file ${SRC_DIR}/fs/fat/vfat.ko                     ${DST_ASSIST_DRV_DIR}

    # spi
    echo "======================================================================================="
	echo "[-SPI DRV:]"
    echo "======================================================================================="
    if [ "x"${TARGET_SYSTEM_PARTITION} == "x""enable" ] ; then
        copy_file ${SRC_DIR}/drivers/mtd/spi-nor/spi-nor.ko                ${DST_DIR}
        copy_file ${SRC_DIR}/drivers/mtd/spi-nor/cadence-quadspi.ko        ${DST_DIR}
        copy_file ${SRC_DIR}/lib/lzo/lzo_compress.ko                       ${DST_DIR}
        copy_file ${SRC_DIR}/fs/jffs2/jffs2.ko                             ${DST_DIR}
        copy_file ${SRC_DIR}/fs/squashfs/squashfs.ko                       ${DST_DIR}
    fi

    if [ "x""${TARGET_NET_TYPE}" == "x""ethernet" ]; then
        # ethernet
        echo "======================================================================================="
        echo "[-ETH DRV:]"
        echo "======================================================================================="
        copy_file ${SRC_DIR}/drivers/net/phy/fixed_phy.ko    ${DST_ASSIST_DRV_DIR}
        copy_file ${SRC_DIR}/drivers/net/phy/libphy.ko       ${DST_ASSIST_DRV_DIR}
        copy_file ${SRC_DIR}/drivers/net/phy/motorcomm.ko    ${DST_ASSIST_DRV_DIR}
        copy_file ${SRC_DIR}/drivers/net/mii.ko              ${DST_ASSIST_DRV_DIR}
        copy_file ${SRC_DIR}/drivers/of/of_mdio.ko           ${DST_ASSIST_DRV_DIR}
        copy_file ${SRC_DIR}/drivers/net/ethernet/stmicro/stmmac/stmmac.ko              ${DST_ASSIST_DRV_DIR}
        copy_file ${SRC_DIR}/drivers/net/ethernet/stmicro/stmmac/stmmac-platform.ko     ${DST_ASSIST_DRV_DIR}
        copy_file ${SRC_DIR}/drivers/net/ethernet/stmicro/stmmac/dwmac-generic.ko       ${DST_ASSIST_DRV_DIR}
        copy_file ${SRC_DIR}/drivers/net/ethernet/stmicro/stmmac/dwmac-dwc-qos-eth.ko   ${DST_ASSIST_DRV_DIR}
    fi

    if [ "x""${TARGET_NET_TYPE}" == "x""wifi" ]; then
        #wifi
        echo "======================================================================================="
        echo "[-WIFI DRV:]"
        echo "======================================================================================="
        copy_file ${SRC_DIR}/net/wireless/cfg80211.ko         ${DST_ASSIST_DRV_DIR}
        copy_file ${SRC_DIR}/net/mac80211/mac80211.ko         ${DST_ASSIST_DRV_DIR}
        copy_file ${WIFI_DIR}/${LIBC_TYPE}/sysfs_${TARGET_KERNEL_SYSFS}/hichannel.ko       ${DST_ASSIST_DRV_DIR}
    fi

    mkdir -p ${LINK_DIR}
    cd ${LINK_DIR}
    # nfs
    ln -sf /${LINK_ASSIST_DRV_DIR}/lib/modules/nfs.ko nfs.ko
    ln -sf /${LINK_ASSIST_DRV_DIR}/lib/modules/nfsv2.ko nfsv2.ko
    ln -sf /${LINK_ASSIST_DRV_DIR}/lib/modules/nfsv3.ko nfsv3.ko
    ln -sf /${LINK_ASSIST_DRV_DIR}/lib/modules/grace.ko grace.ko
    ln -sf /${LINK_ASSIST_DRV_DIR}/lib/modules/lockd.ko lockd.ko
    ln -sf /${LINK_ASSIST_DRV_DIR}/lib/modules/sunrpc.ko sunrpc.ko
    # mmc
    ln -sf /${LINK_ASSIST_DRV_DIR}/lib/modules/mmc_core.ko mmc_core.ko
    ln -sf /${LINK_ASSIST_DRV_DIR}/lib/modules/mmc_block.ko mmc_block.ko
    ln -sf /${LINK_ASSIST_DRV_DIR}/lib/modules/dw_mmc.ko dw_mmc.ko
    ln -sf /${LINK_ASSIST_DRV_DIR}/lib/modules/dw_mmc-pltfm.ko dw_mmc-pltfm.ko
    ln -sf /${LINK_ASSIST_DRV_DIR}/lib/modules/nls_cp936.ko nls_cp936.ko
    ln -sf /${LINK_ASSIST_DRV_DIR}/lib/modules/fat.ko fat.ko
    ln -sf /${LINK_ASSIST_DRV_DIR}/lib/modules/vfat.ko vfat.ko

    if [ "x""${TARGET_NET_TYPE}" == "x""ethernet" ]; then
        # eth
        ln -sf /${LINK_ASSIST_DRV_DIR}/lib/modules/fixed_phy.ko fixed_phy.ko
        ln -sf /${LINK_ASSIST_DRV_DIR}/lib/modules/libphy.ko libphy.ko
        ln -sf /${LINK_ASSIST_DRV_DIR}/lib/modules/motorcomm.ko motorcomm.ko
        ln -sf /${LINK_ASSIST_DRV_DIR}/lib/modules/mii.ko mii.ko
        ln -sf /${LINK_ASSIST_DRV_DIR}/lib/modules/of_mdio.ko of_mdio.ko
        ln -sf /${LINK_ASSIST_DRV_DIR}/lib/modules/stmmac.ko stmmac.ko
        ln -sf /${LINK_ASSIST_DRV_DIR}/lib/modules/stmmac-platform.ko stmmac-platform.ko
        ln -sf /${LINK_ASSIST_DRV_DIR}/lib/modules/dwmac-generic.ko dwmac-generic.ko
        ln -sf /${LINK_ASSIST_DRV_DIR}/lib/modules/dwmac-dwc-qos-eth.ko dwmac-dwc-qos-eth.ko
    fi

    if [ "x""${TARGET_NET_TYPE}" == "x""wifi" ]; then
        # wifi
        ln -sf /${LINK_ASSIST_DRV_DIR}/lib/modules/hichannel.ko hichannel.ko
        ln -sf /${LINK_ASSIST_DRV_DIR}/lib/modules/cfg80211.ko cfg80211.ko
        ln -sf /${LINK_ASSIST_DRV_DIR}/lib/modules/mac80211.ko mac80211.ko
    fi

    if [ "x"${TARGET_SYSTEM_PARTITION} == "x""enable" ] ; then
        ln -sf /usr/lib/modules/spi-nor.ko spi-nor.ko
        ln -sf /usr/lib/modules/cadence-quadspi.ko cadence-quadspi.ko
        ln -sf /usr/lib/modules/lzo_compress.ko lzo_compress.ko
        ln -sf /usr/lib/modules/jffs2.ko jffs2.ko
        ln -sf /usr/lib/modules/squashfs.ko squashfs.ko
    fi
    cd -
    echo "======================================================================================="
}

set -e

export_files

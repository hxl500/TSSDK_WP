#!/bin/bash

SRC_DIR=${TARGET_INSTALL_MOD_PATH}/lib/modules/4.14.249/kernel
DST_DIR=${TARGET_OUT_DIR}/rootfs/usr/lib/modules
LINK_DIR=${TARGET_OUT_DIR}/rootfs/lib/modules

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
    copy_file ${SRC_DIR}/fs/lockd/lockd.ko         ${DST_DIR}
    copy_file ${SRC_DIR}/fs/nfs/nfs.ko             ${DST_DIR}
    copy_file ${SRC_DIR}/fs/nfs/nfsv2.ko           ${DST_DIR}
    copy_file ${SRC_DIR}/fs/nfs/nfsv3.ko           ${DST_DIR}
    copy_file ${SRC_DIR}/fs/nfs_common/grace.ko    ${DST_DIR}
    copy_file ${SRC_DIR}/net/sunrpc/sunrpc.ko      ${DST_DIR}

    # sd
    echo "======================================================================================="
	echo "[-MMC DRV:]"
    echo "======================================================================================="
    copy_file ${SRC_DIR}/drivers/mmc/core/mmc_core.ko       ${DST_DIR}
    copy_file ${SRC_DIR}/drivers/mmc/core/mmc_block.ko      ${DST_DIR}
    copy_file ${SRC_DIR}/drivers/mmc/host/dw_mmc.ko         ${DST_DIR}
    copy_file ${SRC_DIR}/drivers/mmc/host/dw_mmc-pltfm.ko   ${DST_DIR}
    #copy_file ${SRC_DIR}/fs/nls/nls_cp936.ko                ${DST_DIR}
    copy_file ${SRC_DIR}/fs/fat/fat.ko                      ${DST_DIR}
    copy_file ${SRC_DIR}/fs/fat/vfat.ko                     ${DST_DIR}
	copy_file ${SRC_DIR}/fs/nls/nls_cp437.ko                ${DST_DIR}
	copy_file ${SRC_DIR}/fs/nls/nls_iso8859-1.ko            ${DST_DIR}


    if [ "${TARGET_NET_TYPE}" == "ethernet" ]; then
        # ethernet
        echo "======================================================================================="
        echo "[-ETH DRV:]"
        echo "======================================================================================="
        copy_file ${SRC_DIR}/drivers/net/phy/fixed_phy.ko    ${DST_DIR}
        copy_file ${SRC_DIR}/drivers/net/phy/libphy.ko       ${DST_DIR}
        copy_file ${SRC_DIR}/drivers/net/phy/motorcomm.ko    ${DST_DIR}
        copy_file ${SRC_DIR}/drivers/net/mii.ko              ${DST_DIR}
        copy_file ${SRC_DIR}/drivers/of/of_mdio.ko           ${DST_DIR}
        copy_file ${SRC_DIR}/drivers/net/ethernet/stmicro/stmmac/stmmac.ko              ${DST_DIR}
        copy_file ${SRC_DIR}/drivers/net/ethernet/stmicro/stmmac/stmmac-platform.ko     ${DST_DIR}
        copy_file ${SRC_DIR}/drivers/net/ethernet/stmicro/stmmac/dwmac-generic.ko       ${DST_DIR}
        copy_file ${SRC_DIR}/drivers/net/ethernet/stmicro/stmmac/dwmac-dwc-qos-eth.ko   ${DST_DIR}
    fi

    mkdir -p ${LINK_DIR}
    cd ${LINK_DIR}
    # nfs
    ln -sf /usr/lib/modules/nfs.ko nfs.ko
    ln -sf /usr/lib/modules/nfsv2.ko nfsv2.ko
    ln -sf /usr/lib/modules/nfsv3.ko nfsv3.ko
    ln -sf /usr/lib/modules/grace.ko grace.ko
    ln -sf /usr/lib/modules/lockd.ko lockd.ko
    ln -sf /usr/lib/modules/sunrpc.ko sunrpc.ko
    # mmc
    ln -sf /usr/lib/modules/mmc_core.ko mmc_core.ko
    ln -sf /usr/lib/modules/mmc_block.ko mmc_block.ko
    ln -sf /usr/lib/modules/dw_mmc.ko dw_mmc.ko
    ln -sf /usr/lib/modules/dw_mmc-pltfm.ko dw_mmc-pltfm.ko
    #ln -sf /usr/lib/modules/nls_cp936.ko nls_cp936.ko
	ln -sf /usr/lib/modules/nls_cp437.ko  nls_cp437.ko
	ln -sf /usr/lib/modules/nls_iso8859-1.ko  nls_iso8859-1.ko
    ln -sf /usr/lib/modules/fat.ko fat.ko
    ln -sf /usr/lib/modules/vfat.ko vfat.ko

    if [ "${TARGET_NET_TYPE}" == "ethernet" ]; then
        # eth
        ln -sf /usr/lib/modules/fixed_phy.ko fixed_phy.ko
        ln -sf /usr/lib/modules/libphy.ko libphy.ko
        ln -sf /usr/lib/modules/motorcomm.ko motorcomm.ko
        ln -sf /usr/lib/modules/mii.ko mii.ko
        ln -sf /usr/lib/modules/of_mdio.ko of_mdio.ko
        ln -sf /usr/lib/modules/stmmac.ko stmmac.ko
        ln -sf /usr/lib/modules/stmmac-platform.ko stmmac-platform.ko
        ln -sf /usr/lib/modules/dwmac-generic.ko dwmac-generic.ko
        ln -sf /usr/lib/modules/dwmac-dwc-qos-eth.ko dwmac-dwc-qos-eth.ko
    fi
    cd -
    echo "======================================================================================="
}

set -e

export_files

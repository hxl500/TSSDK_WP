#!/bin/bash

# call by build/build.sh
# if need, cp tools from develop_32/64
#    rootfs/etc/init.d: usr can put lunch special scrits to this folder, name S**
#    rootfs/usr, rootfs/lib: not suggest
#    scripts: some scripts call by construct

source ${TARGET_BUILD_DIR}/scripts/common_set.sh

echo -e "=============================================================="
echo -e "\e[7m=====construct rootfs post build=====\e[0m"
echo -e "=============================================================="

#if [ -e ${TARGET_STAGING_INC}/pcppdetect_cfg.cfg ]; then
#	cp ${TARGET_STAGING_INC}/pcppdetect_cfg.cfg    ${TARGET_OUT_DIR}/rootfs/usr/bin
#fi
#if [ -e ${TARGET_STAGING_INC}/pcppdetect_weight.weight ]; then
#	cp ${TARGET_STAGING_INC}/pcppdetect_weight.weight    ${TARGET_OUT_DIR}/rootfs/usr/bin
#fi


#!/bin/bash

# call by build/build.sh

# if need, cp tools from develop_32/64
#    rootfs/etc/init.d: usr can put lunch special scrits to this folder, name S**
#    rootfs/usr, rootfs/lib: not suggest
#    scripts: some scripts call by construct

source ${TARGET_BUILD_DIR}/scripts/common_set.sh

echo -e "=============================================================="
echo -e "\e[7m=====construct rootfs pre build=====\e[0m"
echo -e "=============================================================="

rootfs_path=${TARGET_PREBUILTS_DIR}/custom_lunch/${LUNCH_SEL_NUM}/rootfs
cp -arf ${rootfs_path}/* ${TARGET_OUT_DIR}/rootfs/

#delete no need
rm ${TARGET_OUT_DIR}/rootfs/etc/init.d/S01syslogd
rm ${TARGET_OUT_DIR}/rootfs/etc/init.d/S02klogd
rm ${TARGET_OUT_DIR}/rootfs/etc/init.d/S02sysctl
rm ${TARGET_OUT_DIR}/rootfs/etc/init.d/S40network
rm ${TARGET_OUT_DIR}/rootfs/etc/init.d/S50telnet
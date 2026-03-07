#!/bin/bash

source ${TARGET_BUILD_DIR}/scripts/common_set.sh

# $1=all|clean

# do two things:
#   1:  build
#   2:  cp so to ${TARGET_OUT_DIR}/rootfs/lib
#       cp ko to ${TARGET_OUT_DIR}/rootfs/lib/modules
function build_all()
{
    cd ${TARGET_LIBS_DIR}/drv/lcd/gc9108/; ./build.sh all
    cd ${TARGET_LIBS_DIR}/drv
}

function clean()
{
    cd ${TARGET_LIBS_DIR}/drv/lcd/gc9108/; ./build.sh clean
    cd ${TARGET_LIBS_DIR}/drv
}

if [ $1 == "all" ] ; then
    build_all $1
elif [ $1 == "clean" ] ; then
    clean $1
else
    echo "$(pwd)/build.sh pere($1) err, should be all/clean"
fi

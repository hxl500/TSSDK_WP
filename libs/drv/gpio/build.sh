#!/bin/bash

source ${TARGET_BUILD_DIR}/scripts/common_set.sh


# $1=all|clean

# do two things:
# 	1: build
# 	2: cp so to ${TARGET_OUT_DIR}/rootfs/lib
# 	   cp ko to ${TARGET_OUT_DIR}/rootfs/lib/module
function build_all()
{
	make
	cp -rf vp_gpio_drv.ko ${TARGET_STAGING_MODULE}/
}

function clean()
{
	make clean
}

if [ $1 == "all" ] ; then
	build_all $1
elif [ $1 == "clean" ] ; then
	clean $1
else
	echo "$(pwd)/build.sh pere($1) err, should be all/clean"
fi

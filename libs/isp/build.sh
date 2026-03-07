#!/bin/bash

source ${TARGET_BUILD_DIR}/scripts/common_set.sh

# $1=all|clean

# do two things:
# 	1: build
# 	2: cp so to ${TARGET_OUT_DIR}/staging
# 	   cp ko to ${TARGET_OUT_DIR}/staging/module

function build_all()
{
	if [ "${BUILD_ISP_TUNNING}" = "y" ] ; then
		cd ${TARGET_LIBS_DIR}/isp/lib/${TARGET_LIBC}/tunning && ./build.sh all
	else
		cd ${TARGET_LIBS_DIR}/isp/lib/${TARGET_LIBC}/no_tunning && ./build.sh all
	fi
	cp ${TARGET_LIBS_DIR}/isp/module/${TARGET_LIBC}/*ko ${TARGET_STAGING_MODULE} -aprf
}

function clean_all()
{
	if [ "${BUILD_ISP_TUNNING}" = "y" ] ; then
		cd ${TARGET_LIBS_DIR}/isp/lib/${TARGET_LIBC}/tunning && ./build.sh clean
	else
		cd ${TARGET_LIBS_DIR}/isp/lib/${TARGET_LIBC}/no_tunning && ./build.sh clean
	fi
}


if [ $1 == "all" ] ; then
	build_all ${TARGET_CHIP}
elif [ $1 == "clean" ] ; then
	clean_all ${TARGET_CHIP}
else
	echo "$(pwd)/build.sh pere($1) err, should be all/clean"
fi


#!/bin/bash

source ${TARGET_BUILD_DIR}/scripts/common_set.sh

# $1=all|clean

# do two things:
# 	1: build
# 	2: cp so to ${TARGET_OUT_DIR}/staging
# 	   cp ko to ${TARGET_OUT_DIR}/staging/module

function build_all()
{
	cd ./gpio; ./build.sh all
	cd ../
	cp ./gpio/*.ko ./module/${TARGET_LIBC}  -aprf
	cd ./lcd/gc9108/; ./build.sh all
	cd ../..
	cp ./lcd/gc9108/driver/*ko ./module/${TARGET_LIBC}  -aprf

	if [ -n "$(ls -A ./lib/${TARGET_LIBC})" ]; then
	    cp ./lib/${TARGET_LIBC}/* ${TARGET_STAGING_LIB}  -aprf
	fi
	if [ -n "$(ls -A ./lib/bin/${TARGET_LIBC})" ]; then
	    cp ./lib/bin/${TARGET_LIBC}/* ${TARGET_STAGING_BIN}  -aprf
	fi
	cp ./module/${TARGET_LIBC}/*ko ${TARGET_STAGING_MODULE}  -aprf
	if [ "$(ls -A ./config/)" ]; then
		cp ./config/* ${TARGET_OUT_DIR}/rootfs/root
	fi
}

function clean()
{
	cd ./lcd/gc9108/; ./build.sh clean
	cd ../..
	echo "clean"
}

if [ $1 == "all" ] ; then
	build_all $1
elif [ $1 == "clean" ] ; then
	clean $1
else
	echo "$(pwd)/build.sh pere($1) err, should be all/clean"
fi

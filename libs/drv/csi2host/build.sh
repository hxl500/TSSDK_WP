#!/bin/bash
# $1=all|clean

# do two things:
# 	1: build
# 	2: cp so to ${TARGET_OUT_DIR}/rootfs/lib
# 	   cp ko to ${TARGET_OUT_DIR}/rootfs/lib/module
function build_all()
{
	make
	cp -rf mipi-rx.ko ${TARGET_STAGING_MODULE}/
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

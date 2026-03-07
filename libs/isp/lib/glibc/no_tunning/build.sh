#!/bin/bash
# $1=all|clean
source ${TARGET_BUILD_DIR}/scripts/common_set.sh
# do two things:
# 	1: build
# 	2: cp so to ${TARGET_OUT_DIR}/rootfs/lib
# 	   cp ko to ${TARGET_OUT_DIR}/rootfs/lib/modules

function build_all()
{
	cd ${cur_dir}/build
	./launch_build
	cd ${cur_dir}
}

function clean()
{
	cd ${cur_dir}/build
	./clean.sh
	cd ${cur_dir}
}

cur_dir=$(cd $(dirname $0); pwd)
if [ $1 == "all" ] ; then
	build_all $1
elif [ $1 == "clean" ] ; then
	clean $1
else
	echo "$(pwd)/build.sh pere($1) err, should be all/clean"
fi

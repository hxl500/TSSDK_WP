#!/bin/bash

source ${TARGET_BUILD_DIR}/scripts/common_set.sh

# $1=all|clean

# do two things:
# 	1: build
# 	2: cp so to ${TARGET_OUT_DIR}/staging
# 	   cp ko to ${TARGET_OUT_DIR}/staging/module

function build_all()
{
	cp ./module/${TARGET_LIBC}/*ko ${TARGET_OUT_DIR}/staging/module  -aprf
}

function clean()
{
	echo "clean"
}

if [ $1 == "all" ] ; then
	build_all $1
elif [ $1 == "clean" ] ; then
	clean $1
else
	echo "/home/test/jenkins/workspace/5X/CD/PRE_CD/code/TX5215BX_T8a0_V010R081B050/libs/vpu/build.sh pere($1) err, should be all/clean"
fi

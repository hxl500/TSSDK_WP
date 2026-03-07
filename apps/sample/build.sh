#!/bin/bash

source ${TARGET_BUILD_DIR}/scripts/common_set.sh

# $1=all|clean

# do two things:
# 	1: build
# 	2: cp the so and other need files to ${TARGET_OUT_DIR}/rootfs
function build_all()
{
	./cross-build.sh
}

function clean()
{
	./clean.sh
}

if [ $1 == "all" ] ; then
	build_all $1
else
	clean $1
fi

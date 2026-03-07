#!/bin/bash
# $1=all/clean
source ${TARGET_BUILD_DIR}/scripts/common_set.sh

function build_all()
{
    cp ./lib/${TARGET_LIBC}/librne_pal_* ${TARGET_STAGING_LIB}/librne_pal_linux.a -aprf
    cp ./lib/${TARGET_LIBC}/librne_rt_g3.a ${TARGET_STAGING_LIB} -aprf
    cp ./lib/${TARGET_LIBC}/librne_common.a ${TARGET_STAGING_LIB} -aprf
    cp ./include/* ${TARGET_STAGING_INC} -aprf
    #cp ./module/${TARGET_LIBC}/* ${TARGET_STAGING_MODULE} -aprf
    echo "build done"
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
	echo "Not support, should be all/clean"
	false
fi

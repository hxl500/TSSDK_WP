#!/bin/bash
# $1=all/clean
source ${TARGET_BUILD_DIR}/scripts/common_set.sh

function build_all()
{
	if [ "${BUILD_ISP_TUNNING}" = "y" ] ; then
		cp lib/${TARGET_LIBC}/tunning/* ${TARGET_STAGING_LIB} -aprf
	else
		cp lib/${TARGET_LIBC}/no_tunning/* ${TARGET_STAGING_LIB} -aprf
	fi
	cp include/* ${TARGET_STAGING_INC} -aprf
	cp module/${TARGET_LIBC}/* ${TARGET_STAGING_MODULE} -aprf

	if [ ${TARGET_CHIPNAME} == "tx5215cv200" ] || [ ${TARGET_CHIPNAME} == "tx5215dv200" ] ; then
    cp include/ts_defines_p1v3c3.h  include/ts_defines.h
	elif [ ${TARGET_CHIPNAME} == "tx5215dv300"  ] ; then
    cp include/ts_defines_p2v3c3.h include/ts_defines.h
	fi
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

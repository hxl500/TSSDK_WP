#!/bin/bash

source ${TARGET_BUILD_DIR}/scripts/common_set.sh

if [ "${MPP_USE_PCSIMULATOR}" = "0" ]; then
    echo "MPP_USE_PCSIMULATOR is 0"
    export MPP_USE_PCSIMULATOR=0;
    export CC=${CROSS_COMPILE}gcc
    export CXX=${CROSS_COMPILE}g++
else 
    echo "MPP_USE_PCSIMULATOR is 1"
    export MPP_USE_PCSIMULATOR=1;
    export CC=gcc
    export CXX=g++
fi

export APP_CUR_DIR=`pwd`
export APP_SRC_DIR="${APP_CUR_DIR}"
export APP_OUT_DIR="${APP_SRC_DIR}/out"
export APP_TEMP_DIR="${APP_SRC_DIR}/temp"
export APP_STATIC_COMPILE=0

if [ "x"${TARGET_LIBS_TYPE} == "x""static" ] ; then
    export APP_STATIC_COMPILE=1
fi


#rm -rf ${APP_OUT_DIR}
#rm -rf ${APP_TEMP_DIR}
mkdir -p ${APP_OUT_DIR}
mkdir -p ${APP_TEMP_DIR}

echo "set (APP_SRC_DIR            ${APP_SRC_DIR})"              >   ${APP_TEMP_DIR}/env.cmake
echo "set (TARGET_APPS_DIR        ${TARGET_APPS_DIR})"          >>  ${APP_TEMP_DIR}/env.cmake
echo "set (MPP_USE_PCSIMULATOR    ${MPP_USE_PCSIMULATOR})"      >>  ${APP_TEMP_DIR}/env.cmake
echo "set (APP_STATIC_COMPILE     ${APP_STATIC_COMPILE})"       >>  ${APP_TEMP_DIR}/env.cmake

cp -f CMakeLists.txt ${APP_TEMP_DIR}
cp -f common.cmake ${APP_TEMP_DIR}

#if [ "${TARGET_CHIP}" = "tx536" ]; then
#cp ${APP_SRC_DIR}/../../libs/isp/lin_i5/system/build/temp/lib/lib*   ${APP_SRC_DIR}/../../out/rootfs/usr/lib
#cp ${APP_SRC_DIR}/../../libs/isp/lin_i5/system/build/out/usr/lib/lib*   ${APP_SRC_DIR}/../../out/rootfs/usr/lib
#else
#cp ${APP_SRC_DIR}/../../libs/isp/lin_i5_ts/system/build/temp/lib/lib*   ${APP_SRC_DIR}/../../out/rootfs/usr/lib
#cp ${APP_SRC_DIR}/../../libs/isp/lin_i5_ts/system/build/out/usr/lib/lib*   ${APP_SRC_DIR}/../../out/rootfs/usr/lib
#fi


function assert()
{
    if [ $1 -ne 0 ];then
        echo "Failed  : "$?
        cd ${ARMCB_CUR_DIR}
        exit 1
    fi
}

echo "########################## FLAGS ########################"
echo "### APP_STATIC_COMPILE ${APP_STATIC_COMPILE}          ###"
echo "#########################################################"

cd ${APP_TEMP_DIR}
cmake .
assert $?

make -j16
assert $?

cd ${APP_CUR_DIR}

exit 0

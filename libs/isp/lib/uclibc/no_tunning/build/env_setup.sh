#!/bin/bash

###################################### Cross compile tool #####################################
export ARMCB_CUR_DIR=`pwd`
cd ${ARMCB_CUR_DIR}/../
export ARMCB_CUST_DIR=`pwd`
export ARMCB_SRC_DIR="${ARMCB_CUST_DIR}/src/"
export ARMCB_INC_DIR="${ARMCB_CUST_DIR}/inc/"
export ARMCB_LIB_DIR="${ARMCB_CUST_DIR}/lib/"
export ARMCB_OUTPUT_DIR="${ARMCB_CUR_DIR}/out/"
export ARMCB_TEMP_DIR="${ARMCB_CUR_DIR}/temp/"
export ARMCB_TEMP_LIB_DIR="${ARMCB_TEMP_DIR}/lib/"
export ARMCB_OPENSOURCE_DIR="${ARMCB_CUST_DIR}/openSource/"
export ARMCB_OPENSOURCE_INC="${ARMCB_OPENSOURCE_DIR}/inc/"
export ARMCB_OPENSOURCE_SRC="${ARMCB_OPENSOURCE_DIR}/src/"
export ARMCB_OPENSOURCE_LIB="${ARMCB_OPENSOURCE_DIR}/lib/"

if [ "${BUILD_ISP_TUNNING}" == "" ] || [ "${BUILD_ISP_TUNNING}" == "y" ] ; then
export ALL_LIBS=" -lisp_hal -l3a -laecd -lawbd -lafcd -laec1d -lawb1d -lafc1d \
        -lsensor -lisp -lisp_submod_common -lisp_stats -lisp_aestat -lisp_afstat -lisp_awbstat \
        -lisp_blc -lisp_cac -lisp_cmb -lisp_dgn -lisp_dpc -lisp_ggc -lisp_lsc \
        -lisp_ltm -lisp_ltm_sw -lisp_lsh -lisp_mvd -lisp_top -lisp_vin -lisp_l2q \
        -lisp_psc -lisp_dcp -lisp_lnc -lisp_ccm -lisp_cnr -lisp_csc -lisp_csc2 \
        -lisp_dms -lisp_dpf -lisp_np -lisp_rir -lisp_rnr -lisp_scale -lisp_tnr \
        -lisp_tpg -lisp_wbg -lisp_sde -lisp_vout -lisp_ttnr -lpproc -lbuf_mgr \
        -lfrm_mgr -lcmdr -lsen_modules \
        -lisp_osutils -lniss -lisp_osif -lui_uart \
        -ljson_convert -lcjson -lisp_utils -lc -lgcc -lm\
        "
else
export ALL_LIBS=" -lisp_hal -l3a -laecd -lawbd -lafcd -laec1d -lawb1d -lafc1d \
        -lsensor -lisp -lisp_submod_common -lisp_stats -lisp_aestat -lisp_afstat -lisp_awbstat \
        -lisp_blc -lisp_cac -lisp_cmb -lisp_dgn -lisp_dpc -lisp_ggc -lisp_lsc \
        -lisp_ltm -lisp_ltm_sw -lisp_lsh -lisp_mvd -lisp_top -lisp_vin -lisp_l2q \
        -lisp_psc -lisp_dcp -lisp_lnc -lisp_ccm -lisp_cnr -lisp_csc -lisp_csc2 \
        -lisp_dms -lisp_dpf -lisp_np -lisp_rir -lisp_rnr -lisp_scale -lisp_tnr \
        -lisp_tpg -lisp_wbg -lisp_sde -lisp_vout -lisp_ttnr -lpproc -lbuf_mgr \
        -lfrm_mgr -lcmdr -lsen_modules \
        -lisp_osutils -lisp_osif -lui_uart \
        -ljson_convert -lcjson -lisp_utils -lc -lgcc -lm \
        "
fi

#	-lsensor_gc4023 -lcsi2d_gc4023 -ltuning_gc4023_normal -lcalibration_gc4023\
# 	-lsensor_sc200ai -lcsi2d_sc200ai -ltuning_sc200ai_normal -lcalibration_sc200ai\
#	-ltuning_os04a10_normal -ltuning_os04a10_dol2 -lcalibration_os04a10 -lsensor_os04a10 -lcsi2d_os04a10 \
#	-lsensor_imx327 -lcsi_imx327 -lcsi2d_imx327 -ltuning_imx327_normal -lcalibration_imx327 \
#	-lcsi_ov5647 -lcsi_ar0239 -lactuator_ms41929 -lactuator_ms41929_aux \
#   -lsensor_k17 -lcsi2d_k17 -ltuning_k17_normal -lcalibration_k17 -ltuning_k17_dol2\

#if [[ "${TARGET_BOARD}" == "evb"* ]]; then
#	export ALL_LIBS=$ALL_LIBS"-lsensor_imx415 -lcsi2d_imx415 -ltuning_imx415_normal -ltuning_imx415_dol2 -lcalibration_imx415"
	# echo $ALL_LIBS
#fi

if [ ${CROSS_BUILD} -eq 1 ]; then
	_CROSS_COMPILE_GLOBAL=${CROSS_COMPILE}
	export CROSS_COMPILER_PATH=${_CROSS_COMPILE_GLOBAL%bin/*}
	export TOOLCHAIN=${_CROSS_COMPILE_GLOBAL##*bin/}
	export TOOLCHAIN_PRE=${TOOLCHAIN%-*}
elif [ ${CROSS_BUILD} -eq 2 ]; then
	export CROSS_COMPILER_PATH=/project/isp/Titian/InternalShare/Louis/gcc-arm-none-eabi-7-2018-q2-update/
	export BOOTGEN_PATH=/project/isp/Titian/InternalShare/Louis/bootgen-master/
fi

######################################## Copy makefile ########################################

rm -rf ${ARMCB_OUTPUT_DIR}
mkdir -p ${ARMCB_OUTPUT_DIR}/usr/lib
mkdir -p ${ARMCB_OUTPUT_DIR}/usr/bin
mkdir -p ${ARMCB_TEMP_DIR}
mkdir -p ${ARMCB_TEMP_LIB_DIR}

if [ -d "${ARMCB_LIB_DIR}/" ]; then
	LIB_FILE=`find ${ARMCB_LIB_DIR}/ -name "*.so"`
	if [ -n "$LIB_FILE" ]; then
		cp -rf $LIB_FILE ${ARMCB_OUTPUT_DIR}/usr/lib/
	fi
    LIB_FILE=`find ${ARMCB_LIB_DIR}/ -name "*.a"`
    if [ -n "$LIB_FILE" ]; then
        cp -rf $LIB_FILE ${ARMCB_OUTPUT_DIR}/usr/lib/
    fi
fi
#cp -af `find ${ARMCB_OPENSOURCE_LIB} -name "*.so*"`     ${ARMCB_OUTPUT_DIR}/usr/lib/
#cp -rf ${ARMCB_CUST_DIR}/example/usr/bin/params_parser  ${ARMCB_OUTPUT_DIR}/usr/bin/
cp -rf ${ARMCB_CUR_DIR}/CMakeLists.txt                  ${ARMCB_TEMP_DIR}
cp -rf ${ARMCB_CUR_DIR}/common.cmake                    ${ARMCB_TEMP_DIR}

###################################### Generate env.cmake #####################################

echo "set (ARMCB_DEBUG_BUILD        ${ARMCB_DEBUG_BUILD})"      >  ${ARMCB_TEMP_DIR}/env.cmake
echo "set (ARMCB_STATIC_COMPILE     ${ARMCB_STATIC_COMPILE})"   >> ${ARMCB_TEMP_DIR}/env.cmake
echo "set (ARMCB_ISP_PROJECT        ${ARMCB_ISP_PROJECT})"      >> ${ARMCB_TEMP_DIR}/env.cmake
echo "set (ARMCB_CROSS_BUILD        ${CROSS_BUILD})"            >> ${ARMCB_TEMP_DIR}/env.cmake
echo "set (ARMCB_AARCH64_BUILD      ${ARMCB_AARCH64_BUILD})"    >> ${ARMCB_TEMP_DIR}/env.cmake
echo "set (ARMCB_CUR_DIR            ${ARMCB_CUR_DIR})"          >> ${ARMCB_TEMP_DIR}/env.cmake
echo "set (ARMCB_SRC_DIR            ${ARMCB_SRC_DIR})"          >> ${ARMCB_TEMP_DIR}/env.cmake
echo "set (ARMCB_INC_DIR            ${ARMCB_INC_DIR})"          >> ${ARMCB_TEMP_DIR}/env.cmake
echo "set (ARMCB_LIB_DIR            ${ARMCB_LIB_DIR})"          >> ${ARMCB_TEMP_DIR}/env.cmake
echo "set (ARMCB_TEMP_DIR           ${ARMCB_TEMP_DIR})"         >> ${ARMCB_TEMP_DIR}/env.cmake
echo "set (ARMCB_TEMP_LIB_DIR       ${ARMCB_TEMP_LIB_DIR})"     >> ${ARMCB_TEMP_DIR}/env.cmake
echo "set (ARMCB_OUTPUT_DIR         ${ARMCB_OUTPUT_DIR})"       >> ${ARMCB_TEMP_DIR}/env.cmake
echo "set (ALL_LIBS                 ${ALL_LIBS})"               >> ${ARMCB_TEMP_DIR}/env.cmake
echo "set (ARMCB_CUST_BUILD         ${ARMCB_CUST_BUILD})"       >> ${ARMCB_TEMP_DIR}/env.cmake
echo "set (ARMCB_OPENSOURCE_DIR     ${ARMCB_OPENSOURCE_DIR})"   >> ${ARMCB_TEMP_DIR}/env.cmake
echo "set (ARMCB_OPENSOURCE_INC     ${ARMCB_OPENSOURCE_INC})"   >> ${ARMCB_TEMP_DIR}/env.cmake
echo "set (ARMCB_OPENSOURCE_SRC     ${ARMCB_OPENSOURCE_SRC})"   >> ${ARMCB_TEMP_DIR}/env.cmake
echo "set (ARMCB_OPENSOURCE_LIB     ${ARMCB_OPENSOURCE_LIB})"   >> ${ARMCB_TEMP_DIR}/env.cmake
echo "set (TARGET_CHIP_BUILD        ${TARGET_CHIP})"            >> ${ARMCB_TEMP_DIR}/env.cmake
echo "set (TARGET_BOARD_BUILD       ${TARGET_BOARD})"           >> ${ARMCB_TEMP_DIR}/env.cmake
echo "set (BUILD_OPTION_UCLIBC      ${BUILD_OPTION_UCLIBC})"    >> ${ARMCB_TEMP_DIR}/env.cmake

include (${CMAKE_CURRENT_SOURCE_DIR}/env.cmake)

#################################### Global Variables ##########################################

set (ARMCB_GENERATED_OUT            ${ARMCB_OUTPUT_DIR})
set (ARMCB_GENERATED_OUT_DATA       ${ARMCB_OUTPUT_DIR}/data)
set (ARMCB_GENERATED_OUT_JSON       ${ARMCB_OUTPUT_DIR}/json)
set (ARMCB_GENERATED_OUT_JSON_SRC   ${ARMCB_OUTPUT_DIR}/json/src)
set (ARMCB_GENERATED_OUT_JSON_INC   ${ARMCB_OUTPUT_DIR}/json/inc)
set (ARMCB_GENERATED_TEMP           ${ARMCB_TEMP_DIR})

set (ARMCB_ISP_TOOL                 ${ARMCB_SRC_DIR}/tools)
set (ARMCB_ISP_UTILS                ${ARMCB_SRC_DIR}/utils)

set (ARMCB_ISP_APP                  ${ARMCB_SRC_DIR}/app_layer)
set (ARMCB_ISP_HAL                  ${ARMCB_SRC_DIR}/hal_layer)
set (ARMCB_ISP_FW                   ${ARMCB_SRC_DIR}/fw_layer)
set (ARMCB_ISP_OS                   ${ARMCB_SRC_DIR}/os_abstract)
set (ARMCB_ISP_CUSTOM               ${ARMCB_SRC_DIR}/custom)

if ( ARMCB_CUST_BUILD EQUAL 1 )
    set (ARMCB_ISP_EXTERNAL             ${ARMCB_OPENSOURCE_SRC})
else ()
    set (ARMCB_ISP_EXTERNAL             ${ARMCB_SRC_DIR}/external)
endif()


if (ARMCB_AARCH64_BUILD EQUAL 1)
    set (ARMCB_EXTERNAL_LIB         ${ARMCB_GENERATED_OUT}/lib64)
else ()
    set (ARMCB_EXTERNAL_LIB         ${ARMCB_GENERATED_OUT}/lib)
endif ()

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${ARMCB_GENERATED_OUT}/usr/bin)
if (ARMCB_STATIC_COMPILE EQUAL 1)
    set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${ARMCB_GENERATED_OUT}/usr/lib)
else ()
    set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${ARMCB_GENERATED_OUT}/usr/lib)
endif()

if (ARMCB_CROSS_BUILD EQUAL 2)
    set (ARMCB_BSPINC_DIR           ${ARMCB_BSP_DIR}/ps7_cortexa9_0/include)
    set (ARMCB_BSPLIB_DIR           ${ARMCB_BSP_DIR}/ps7_cortexa9_0/lib)
endif()

set(BUILD_ISP_TUNNING $ENV{BUILD_ISP_TUNNING})
if((NOT DEFINED ENV{BUILD_ISP_TUNNING}) OR (BUILD_ISP_TUNNING STREQUAL "y"))
    add_compile_definitions(_BUILD_ISP_TUNNING)
endif()

#################################### Include Directories #######################################

include_directories(${ARMCB_INC_DIR})
include_directories(${ARMCB_OPENSOURCE_INC})
include_directories(${ARMCB_SRC_DIR})
include_directories(${ARMCB_SRC_DIR}/common)
include_directories(${ARMCB_ISP_UTILS})
include_directories(${ARMCB_OPENSOURCE_SRC}/cJSON)
include_directories(${ARMCB_ISP_OS}/osutils)
include_directories(${ARMCB_ISP_TOOL}/tuning_convert)
include_directories(${ARMCB_ISP_TOOL}/awb_simulation)
include_directories(${ARMCB_ISP_APP})
include_directories(${ARMCB_ISP_APP}/config)
include_directories(${ARMCB_ISP_APP}/device)
include_directories(${ARMCB_ISP_APP}/usecase)
include_directories(${ARMCB_ISP_HAL})
include_directories(${ARMCB_ISP_HAL}/bus)
include_directories(${ARMCB_ISP_HAL}/context)
include_directories(${ARMCB_ISP_HAL}/metadata)
include_directories(${ARMCB_ISP_HAL}/pipeline)
include_directories(${ARMCB_ISP_FW}/common)
include_directories(${ARMCB_ISP_FW}/cmdr)
include_directories(${ARMCB_ISP_FW}/frame_mgr)
include_directories(${ARMCB_ISP_FW}/hw_env)
include_directories(${ARMCB_ISP_FW}/buf_mgr)
include_directories(${ARMCB_ISP_FW}/api)

include_directories(${ARMCB_ISP_FW}/sw3a_node)
include_directories(${ARMCB_ISP_FW}/sw3a_node/aec)
include_directories(${ARMCB_ISP_FW}/sw3a_node/af)
include_directories(${ARMCB_ISP_FW}/sw3a_node/awb)
include_directories(${ARMCB_ISP_FW}/sw3a_node/aec/extern)
include_directories(${ARMCB_ISP_FW}/sw3a_node/af/extern)
include_directories(${ARMCB_ISP_FW}/sw3a_node/awb/extern)

include_directories(${ARMCB_ISP_FW}/sensor_node)
include_directories(${ARMCB_ISP_FW}/sensor_node/elements/sensor)
include_directories(${ARMCB_ISP_FW}/sensor_node/elements/actuator)
include_directories(${ARMCB_ISP_FW}/sensor_node/elements/csi)
include_directories(${ARMCB_ISP_FW}/sensor_node/elements/csi2d)

include_directories(${ARMCB_ISP_FW}/sensor_node/elements/eeprom)
include_directories(${ARMCB_ISP_FW}/sensor_node/elements/tuning/tune)
include_directories(${ARMCB_ISP_FW}/sensor_node/elements/tuning/tune/3A)
include_directories(${ARMCB_ISP_FW}/sensor_node/elements/tuning/tune/isp)
include_directories(${ARMCB_ISP_FW}/sensor_node/elements/tuning/tune/isp/blc)
include_directories(${ARMCB_ISP_FW}/sensor_node/elements/tuning/tune/isp/ccm)
include_directories(${ARMCB_ISP_FW}/sensor_node/elements/tuning/tune/isp/cmb)
include_directories(${ARMCB_ISP_FW}/sensor_node/elements/tuning/tune/isp/cnr)
include_directories(${ARMCB_ISP_FW}/sensor_node/elements/tuning/tune/isp/csp)
include_directories(${ARMCB_ISP_FW}/sensor_node/elements/tuning/tune/isp/dgn)
include_directories(${ARMCB_ISP_FW}/sensor_node/elements/tuning/tune/isp/dms)
include_directories(${ARMCB_ISP_FW}/sensor_node/elements/tuning/tune/isp/dpc)
include_directories(${ARMCB_ISP_FW}/sensor_node/elements/tuning/tune/isp/dpf)
include_directories(${ARMCB_ISP_FW}/sensor_node/elements/tuning/tune/isp/cac)
include_directories(${ARMCB_ISP_FW}/sensor_node/elements/tuning/tune/isp/gain_range)
include_directories(${ARMCB_ISP_FW}/sensor_node/elements/tuning/tune/isp/ggc)
include_directories(${ARMCB_ISP_FW}/sensor_node/elements/tuning/tune/isp/lsc)
include_directories(${ARMCB_ISP_FW}/sensor_node/elements/tuning/tune/isp/lsh)
include_directories(${ARMCB_ISP_FW}/sensor_node/elements/tuning/tune/isp/ltm)
include_directories(${ARMCB_ISP_FW}/sensor_node/elements/tuning/tune/isp/mvd)
include_directories(${ARMCB_ISP_FW}/sensor_node/elements/tuning/tune/isp/np)
include_directories(${ARMCB_ISP_FW}/sensor_node/elements/tuning/tune/isp/rir)
include_directories(${ARMCB_ISP_FW}/sensor_node/elements/tuning/tune/isp/rnr)
include_directories(${ARMCB_ISP_FW}/sensor_node/elements/tuning/tune/isp/sde)
include_directories(${ARMCB_ISP_FW}/sensor_node/elements/tuning/tune/isp/tnr)
include_directories(${ARMCB_ISP_FW}/sensor_node/elements/tuning/tune/isp/ynr)
include_directories(${ARMCB_ISP_FW}/sensor_node/elements/tuning/calibration)
include_directories(${ARMCB_ISP_FW}/isp_node)
include_directories(${ARMCB_ISP_FW}/isp_node/modules)
include_directories(${ARMCB_ISP_FW}/isp_node/stats)
include_directories(${ARMCB_ISP_FW}/isp_node/aestat)
include_directories(${ARMCB_ISP_FW}/isp_node/aestat/aestat10)
include_directories(${ARMCB_ISP_FW}/isp_node/aestat/aestat20)
include_directories(${ARMCB_ISP_FW}/isp_node/afstat)
include_directories(${ARMCB_ISP_FW}/isp_node/afstat/afstat10)
include_directories(${ARMCB_ISP_FW}/isp_node/afstat/afstat20)
include_directories(${ARMCB_ISP_FW}/isp_node/awbstat)
include_directories(${ARMCB_ISP_FW}/isp_node/blc)
include_directories(${ARMCB_ISP_FW}/isp_node/blc/blc10)
include_directories(${ARMCB_ISP_FW}/isp_node/blc/blc20)
include_directories(${ARMCB_ISP_FW}/isp_node/cac)
include_directories(${ARMCB_ISP_FW}/isp_node/cac/cac10)
include_directories(${ARMCB_ISP_FW}/isp_node/cac/cac20)
include_directories(${ARMCB_ISP_FW}/isp_node/cbc/cbc20)
include_directories(${ARMCB_ISP_FW}/isp_node/ccm)
include_directories(${ARMCB_ISP_FW}/isp_node/ccm/ccm10)
include_directories(${ARMCB_ISP_FW}/isp_node/cmb)
include_directories(${ARMCB_ISP_FW}/isp_node/cmb/cmb10)
include_directories(${ARMCB_ISP_FW}/isp_node/cmb/cmb20)
include_directories(${ARMCB_ISP_FW}/isp_node/cnr)
include_directories(${ARMCB_ISP_FW}/isp_node/cnr/cnr10)
include_directories(${ARMCB_ISP_FW}/isp_node/csc)
include_directories(${ARMCB_ISP_FW}/isp_node/csc/csc_10)
include_directories(${ARMCB_ISP_FW}/isp_node/csc2)
include_directories(${ARMCB_ISP_FW}/isp_node/csc2/csc2_10)
include_directories(${ARMCB_ISP_FW}/isp_node/dgn)
include_directories(${ARMCB_ISP_FW}/isp_node/dgn/dgn10)
include_directories(${ARMCB_ISP_FW}/isp_node/dgn/dgn20)
include_directories(${ARMCB_ISP_FW}/isp_node/dcp)
include_directories(${ARMCB_ISP_FW}/isp_node/dcp/dcp10)
include_directories(${ARMCB_ISP_FW}/isp_node/dms)
include_directories(${ARMCB_ISP_FW}/isp_node/dms/dms10)
include_directories(${ARMCB_ISP_FW}/isp_node/dpc)
include_directories(${ARMCB_ISP_FW}/isp_node/dpc/dpc10)
include_directories(${ARMCB_ISP_FW}/isp_node/dpc/dpc20)
include_directories(${ARMCB_ISP_FW}/isp_node/dpf)
include_directories(${ARMCB_ISP_FW}/isp_node/dpf/dpf10)
include_directories(${ARMCB_ISP_FW}/isp_node/ggc)
include_directories(${ARMCB_ISP_FW}/isp_node/ggc/ggc10)
include_directories(${ARMCB_ISP_FW}/isp_node/ggc/ggc20)
include_directories(${ARMCB_ISP_FW}/isp_node/hardware)
include_directories(${ARMCB_ISP_FW}/isp_node/hardware/pipeline/octob)
include_directories(${ARMCB_ISP_FW}/isp_node/hardware/pipeline/titian)
include_directories(${ARMCB_ISP_FW}/isp_node/hardware/pipeline/octob_cus0)
include_directories(${ARMCB_ISP_FW}/isp_node/afbc)
include_directories(${ARMCB_ISP_FW}/isp_node/afbc/afbc20)
include_directories(${ARMCB_ISP_FW}/isp_node/ifbc/ifbc20)
include_directories(${ARMCB_ISP_FW}/isp_node/ifbd/ifbd20)
include_directories(${ARMCB_ISP_FW}/isp_node/l2q)
include_directories(${ARMCB_ISP_FW}/isp_node/l2q/l2q10)
include_directories(${ARMCB_ISP_FW}/isp_node/l2q/l2q20)
include_directories(${ARMCB_ISP_FW}/isp_node/lnc)
include_directories(${ARMCB_ISP_FW}/isp_node/lnc/lnc10)
include_directories(${ARMCB_ISP_FW}/isp_node/lnc/lnc20)
include_directories(${ARMCB_ISP_FW}/isp_node/lsc)
include_directories(${ARMCB_ISP_FW}/isp_node/lsc/lsc10)
include_directories(${ARMCB_ISP_FW}/isp_node/lsc/lsc20)
include_directories(${ARMCB_ISP_FW}/isp_node/lsh)
include_directories(${ARMCB_ISP_FW}/isp_node/lsh/lsh10)
include_directories(${ARMCB_ISP_FW}/isp_node/lsh/lsh20)
include_directories(${ARMCB_ISP_FW}/isp_node/ltm)
include_directories(${ARMCB_ISP_FW}/isp_node/ltm/ltm10)
include_directories(${ARMCB_ISP_FW}/isp_node/ltm/ltm20)
include_directories(${ARMCB_ISP_FW}/isp_node/ltm_sw)
include_directories(${ARMCB_ISP_FW}/isp_node/ltm_sw/ltm10_sw)
include_directories(${ARMCB_ISP_FW}/isp_node/ltm_sw/ltm20_sw)
include_directories(${ARMCB_ISP_FW}/isp_node/mvd)
include_directories(${ARMCB_ISP_FW}/isp_node/mvd/mvd10)
include_directories(${ARMCB_ISP_FW}/isp_node/mvd/mvd20)
include_directories(${ARMCB_ISP_FW}/isp_node/np)
include_directories(${ARMCB_ISP_FW}/isp_node/np/np10)
include_directories(${ARMCB_ISP_FW}/isp_node/psc)
include_directories(${ARMCB_ISP_FW}/isp_node/psc/psc10)
include_directories(${ARMCB_ISP_FW}/isp_node/psc/psc20)
include_directories(${ARMCB_ISP_FW}/isp_node/ptm)
include_directories(${ARMCB_ISP_FW}/isp_node/ptm/ptm20)
include_directories(${ARMCB_ISP_FW}/isp_node/q2l)
include_directories(${ARMCB_ISP_FW}/isp_node/q2l/q2l20)
include_directories(${ARMCB_ISP_FW}/isp_node/rir)
include_directories(${ARMCB_ISP_FW}/isp_node/rir/rir10)
include_directories(${ARMCB_ISP_FW}/isp_node/rnr)
include_directories(${ARMCB_ISP_FW}/isp_node/rnr/rnr10)
include_directories(${ARMCB_ISP_FW}/isp_node/rtnr)
include_directories(${ARMCB_ISP_FW}/isp_node/rtnr/rtnr20)
include_directories(${ARMCB_ISP_FW}/isp_node/scale/sca)
include_directories(${ARMCB_ISP_FW}/isp_node/scale/sca/sca10)
include_directories(${ARMCB_ISP_FW}/isp_node/scale/scb)
include_directories(${ARMCB_ISP_FW}/isp_node/scale/scb/scb10)
include_directories(${ARMCB_ISP_FW}/isp_node/scale/scd)
include_directories(${ARMCB_ISP_FW}/isp_node/scale/scd/scd10)
include_directories(${ARMCB_ISP_FW}/isp_node/scale/sce)
include_directories(${ARMCB_ISP_FW}/isp_node/scale/sce/sce10)
include_directories(${ARMCB_ISP_FW}/isp_node/sde)
include_directories(${ARMCB_ISP_FW}/isp_node/sde/sde10)
include_directories(${ARMCB_ISP_FW}/isp_node/tnr)
include_directories(${ARMCB_ISP_FW}/isp_node/tnr/tnr10)
include_directories(${ARMCB_ISP_FW}/isp_node/top)
include_directories(${ARMCB_ISP_FW}/isp_node/top/top10)
include_directories(${ARMCB_ISP_FW}/isp_node/top/top20)
include_directories(${ARMCB_ISP_FW}/isp_node/tpg)
include_directories(${ARMCB_ISP_FW}/isp_node/tpg/tpg10)
include_directories(${ARMCB_ISP_FW}/isp_node/tpg/tpg20)
include_directories(${ARMCB_ISP_FW}/isp_node/ttnr)
include_directories(${ARMCB_ISP_FW}/isp_node/ttnr/ttnr_536)

include_directories(${ARMCB_ISP_FW}/isp_node/vin)
include_directories(${ARMCB_ISP_FW}/isp_node/vin/vin10)
include_directories(${ARMCB_ISP_FW}/isp_node/vin/vin20)
include_directories(${ARMCB_ISP_FW}/isp_node/vin/vinx)
include_directories(${ARMCB_ISP_FW}/isp_node/vout0)
include_directories(${ARMCB_ISP_FW}/isp_node/vout1)
include_directories(${ARMCB_ISP_FW}/isp_node/vout/vout10)
include_directories(${ARMCB_ISP_FW}/isp_node/vout/vout10/vout0)
include_directories(${ARMCB_ISP_FW}/isp_node/vout/vout10/vout1)
include_directories(${ARMCB_ISP_FW}/isp_node/vout/vout10/vout2)
include_directories(${ARMCB_ISP_FW}/isp_node/vout/vout10/vout3)
include_directories(${ARMCB_ISP_FW}/isp_node/wbg)
include_directories(${ARMCB_ISP_FW}/isp_node/wbg/wbg10)
include_directories(${ARMCB_ISP_FW}/isp_node/ynr/ynr20)
include_directories(${ARMCB_ISP_FW}/isp_node/ytnr/ytnr20)
include_directories(${ARMCB_ISP_FW}/pproc_node)
include_directories(${ARMCB_ISP_OS}/niss)
include_directories(${ARMCB_ISP_OS}/niss/niss_platform)
include_directories(${ARMCB_ISP_OS}/niss/niss_ethernet)
include_directories(${ARMCB_ISP_OS}/niss/niss_usb)
include_directories(${ARMCB_ISP_OS}/niss/niss_uart)
include_directories(${ARMCB_ISP_OS}/niss/niss_hdmi)
include_directories(${ARMCB_ISP_OS}/niss/niss_tcpip)
#### Include camera modules  #####
include_directories(${ARMCB_ISP_CUSTOM}/actuator/ms41929)
include_directories(${ARMCB_ISP_CUSTOM}/actuator/ms41929_aux)

include_directories(${ARMCB_ISP_CUSTOM}/csi/imx327)
include_directories(${ARMCB_ISP_CUSTOM}/csi/ov5647)
include_directories(${ARMCB_ISP_CUSTOM}/csi/ar0239)
include_directories(${ARMCB_ISP_CUSTOM}/csi2d/imx327)
include_directories(${ARMCB_ISP_CUSTOM}/csi2d/imx415)
include_directories(${ARMCB_ISP_CUSTOM}/csi2d/gc4023)
include_directories(${ARMCB_ISP_CUSTOM}/csi2d/os04a10)
include_directories(${ARMCB_ISP_CUSTOM}/csi2d/sc200ai)
include_directories(${ARMCB_ISP_CUSTOM}/csi2d/sc2336p)
include_directories(${ARMCB_ISP_CUSTOM}/csi2d/c3390)
include_directories(${ARMCB_ISP_CUSTOM}/csi2d/gc4653)
include_directories(${ARMCB_ISP_CUSTOM}/csi2d/cv4002)
include_directories(${ARMCB_ISP_CUSTOM}/csi2d/c3390)
include_directories(${ARMCB_ISP_CUSTOM}/csi2d/gc4653)
include_directories(${ARMCB_ISP_CUSTOM}/csi2d/gc2083)
include_directories(${ARMCB_ISP_CUSTOM}/csi2d/k17)
include_directories(${ARMCB_ISP_CUSTOM}/csi2d/sc430ai)
include_directories(${ARMCB_ISP_CUSTOM}/csi2d/sc501ai)
include_directories(${ARMCB_ISP_CUSTOM}/csi2d/hi556)
include_directories(${ARMCB_ISP_CUSTOM}/csi2d/cv2003)

include_directories(${ARMCB_ISP_CUSTOM}/sensor/imx307)
include_directories(${ARMCB_ISP_CUSTOM}/sensor/imx327)
include_directories(${ARMCB_ISP_CUSTOM}/sensor/ov5647)
include_directories(${ARMCB_ISP_CUSTOM}/sensor/ar0239)
include_directories(${ARMCB_ISP_CUSTOM}/sensor/imx415)
include_directories(${ARMCB_ISP_CUSTOM}/sensor/gc4023)
include_directories(${ARMCB_ISP_CUSTOM}/sensor/os04a10)
include_directories(${ARMCB_ISP_CUSTOM}/sensor/sc200ai)
include_directories(${ARMCB_ISP_CUSTOM}/sensor/sc2336p)
include_directories(${ARMCB_ISP_CUSTOM}/sensor/c3390)
include_directories(${ARMCB_ISP_CUSTOM}/sensor/gc4653)
include_directories(${ARMCB_ISP_CUSTOM}/sensor/cv4002)
include_directories(${ARMCB_ISP_CUSTOM}/sensor/c3390)
include_directories(${ARMCB_ISP_CUSTOM}/sensor/gc4653)
include_directories(${ARMCB_ISP_CUSTOM}/sensor/gc2083)
include_directories(${ARMCB_ISP_CUSTOM}/sensor/k17)
include_directories(${ARMCB_ISP_CUSTOM}/sensor/sc430ai)
include_directories(${ARMCB_ISP_CUSTOM}/sensor/sc501ai)
include_directories(${ARMCB_ISP_CUSTOM}/sensor/hi556)
include_directories(${ARMCB_ISP_CUSTOM}/sensor/cv2003)

include_directories(${ARMCB_ISP_CUSTOM}/tuning/imx307)
include_directories(${ARMCB_ISP_CUSTOM}/tuning/imx327)
include_directories(${ARMCB_ISP_CUSTOM}/tuning/imx307/)
include_directories(${ARMCB_ISP_CUSTOM}/tuning/imx327/)
include_directories(${ARMCB_ISP_CUSTOM}/tuning/ov5647/)
include_directories(${ARMCB_ISP_CUSTOM}/tuning/ar0239/)
include_directories(${ARMCB_ISP_CUSTOM}/tuning/imx415)
include_directories(${ARMCB_ISP_CUSTOM}/tuning/gc4023)
include_directories(${ARMCB_ISP_CUSTOM}/tuning/os04a10)
include_directories(${ARMCB_ISP_CUSTOM}/tuning/sc200ai)
include_directories(${ARMCB_ISP_CUSTOM}/tuning/sc2336p)
include_directories(${ARMCB_ISP_CUSTOM}/tuning/c3390)
include_directories(${ARMCB_ISP_CUSTOM}/tuning/gc4653)
include_directories(${ARMCB_ISP_CUSTOM}/tuning/cv4002)
include_directories(${ARMCB_ISP_CUSTOM}/tuning/c3390)
include_directories(${ARMCB_ISP_CUSTOM}/tuning/gc4653)
include_directories(${ARMCB_ISP_CUSTOM}/tuning/gc2083)
include_directories(${ARMCB_ISP_CUSTOM}/tuning/k17)
include_directories(${ARMCB_ISP_CUSTOM}/tuning/sc430ai)
include_directories(${ARMCB_ISP_CUSTOM}/tuning/sc501ai)
include_directories(${ARMCB_ISP_CUSTOM}/tuning/hi556)
include_directories(${ARMCB_ISP_CUSTOM}/tuning/cv2003)


include_directories(${ARMCB_ISP_CUSTOM}/module)

if (ARMCB_CROSS_BUILD EQUAL 2)
    include_directories(${ARMCB_BSPINC_DIR})
    include_directories(${ARMCB_ISP_OS}/isp_os_if)
    include_directories(${ARMCB_ISP_OS}/isp_os_if/xilinx)
    include_directories(${ARMCB_ISP_OS}/isp_os_if/xilinx/isp)
    include_directories(${ARMCB_ISP_OS}/isp_os_if/xilinx/isp/debug)
    include_directories(${ARMCB_ISP_OS}/isp_os_if/xilinx/sensor)
    include_directories(${ARMCB_ISP_OS}/isp_os_if/xilinx/mipi)
    include_directories(${ARMCB_ISP_OS}/isp_os_if/linux)
    include_directories(${ARMCB_ISP_OS}/isp_os_if/mem)
    include_directories(${ARMCB_ISP_OS}/platform_if/common)
    include_directories(${ARMCB_ISP_OS}/platform_if/protocol/cmd_if)
    include_directories(${ARMCB_ISP_OS}/platform_if/protocol/ui_uart)
    include_directories(${ARMCB_ISP_OS}/platform_if/xilinx/io/i2c)
    include_directories(${ARMCB_ISP_OS}/platform_if/xilinx/io/spi)
    include_directories(${ARMCB_ISP_OS}/platform_if/xilinx/io/reg)
    include_directories(${ARMCB_ISP_OS}/platform_if/xilinx/sdcard)
    include_directories(${ARMCB_ISP_OS}/platform_if/xilinx/timer)
    include_directories(${ARMCB_ISP_OS}/platform_if/xilinx/ethernet)
    include_directories(${ARMCB_ISP_OS}/platform_if/xilinx/ethernet/lwip)
    include_directories(${ARMCB_ISP_OS}/platform_if/xilinx/hdmi)
    include_directories(${ARMCB_ISP_OS}/platform_if/xilinx/usb)
    include_directories(${ARMCB_ISP_OS}/platform_if/xilinx/interrupt)
    include_directories(${ARMCB_ISP_OS}/platform_if/xilinx/dma)
    include_directories(${ARMCB_ISP_OS}/platform_if/xilinx/vtc_v7_2)
    include_directories(${ARMCB_ISP_OS}/platform_if/xilinx/peri)
else()
    include_directories(${ARMCB_GENERATED_TEMP}/linux_headers/include)
    include_directories(${ARMCB_OPENSOURCE_SRC}/libpcap/libpcap-1.9.1/include)
    include_directories(${ARMCB_ISP_OS}/isp_os_if)
    include_directories(${ARMCB_ISP_OS}/isp_os_if/mem)
    include_directories(${ARMCB_ISP_OS}/isp_os_if/linux)
    include_directories(${ARMCB_ISP_OS}/isp_os_if/xilinx)
    include_directories(${ARMCB_ISP_OS}/platform_if/common)
    include_directories(${ARMCB_ISP_OS}/platform_if/protocol/cmd_if)
    include_directories(${ARMCB_ISP_OS}/platform_if/protocol/ui_uart)
endif()

if (ARMCB_TEST_MODE EQUAL 1)
    include_directories(${ARMCB_ISP_APP}/sanity_test)
    add_definitions(-DARMCB_STEST_ENABLE)
    message ("ARMCB_STEST_ENABLE = ${ARMCB_TEST_MODE}")
endif()

message ("TARGET_CHIP_BUILD = ${TARGET_CHIP_BUILD}")
if ( ${TARGET_CHIP_BUILD} STREQUAL "tx536")
	add_definitions (-DTARGET_CHIP_TX536)
else ()
    if ( ${TARGET_CHIP_BUILD} MATCHES "tx5215")
        add_definitions (-DTARGET_CHIP_TX5215)
    elseif ( ${TARGET_CHIP_BUILD} MATCHES "tx5239dv200")
        add_definitions (-DTARGET_CHIP_TX5239)
    elseif ( ${TARGET_CHIP_BUILD} MATCHES "tx5119")
        add_definitions (-DTARGET_CHIP_TX5239)
    elseif ( ${TARGET_CHIP_BUILD} MATCHES "tx5239dv201")
        add_definitions (-DTARGET_CHIP_TX5112)
    elseif ( ${TARGET_CHIP_BUILD} MATCHES "tx5239cv201")
        add_definitions (-DTARGET_CHIP_TX5112)
    elseif ( ${TARGET_CHIP_BUILD} MATCHES "tx5112")
        add_definitions (-DTARGET_CHIP_TX5112)
    elseif ( ${TARGET_CHIP_BUILD} MATCHES "tx5239dv300")
        add_definitions (-DTARGET_CHIP_TX5215)
    endif ()
endif ()

message ("TARGET_BOARD_BUILD = ${TARGET_BOARD_BUILD}")
if ( ${TARGET_BOARD_BUILD} MATCHES "evb88")
    add_definitions (-DTARGET_BOARD_EVB88)
endif ()
if ( ${TARGET_BOARD_BUILD} MATCHES "evb132")
    add_definitions (-DTARGET_BOARD_EVB132)
endif ()
if ( ${TARGET_BOARD_BUILD} MATCHES "aov")
    add_definitions (-DTARGET_BOARD_AOV)
endif ()
if ( ${TARGET_BOARD_BUILD} MATCHES "ptz")
    add_definitions (-DTARGET_BOARD_PTZ)
endif ()
if ( ${TARGET_BOARD_BUILD} MATCHES "ipc")
    add_definitions (-DTARGET_BOARD_IPC)
endif ()
if ( ${TARGET_BOARD_BUILD} MATCHES "fast")
    add_definitions (-DTARGET_BOARD_IPC)
endif ()
if ( ${TARGET_BOARD_BUILD} MATCHES "aio")
    add_definitions (-DTARGET_BOARD_AIO)
endif ()
if ( ${TARGET_BOARD_BUILD} MATCHES "fpga")
    add_definitions (-DTARGET_BOARD_FPGA)
endif ()
if ( ${TARGET_BOARD_BUILD} MATCHES "evbt")
    add_definitions (-DTARGET_BOARD_EVBT)
endif ()
if ( ${TARGET_BOARD_BUILD} MATCHES "evb")
    add_definitions (-DTARGET_BOARD_EVB)
endif ()
#################################### MACRO ######################################################

add_definitions(-D${ARMCB_ISP_PROJECT})
add_definitions(-DSOURCE_ROOT_DIR="${ARMCB_SRC_DIR}")
#need extra \\ if defined project
#add_definitions(-DSOURCE_ROOT_DIR=\\"${ARMCB_SRC_DIR}\\")

# message ("ARMCB_ISP_PROJECT= ${ARMCB_ISP_PROJECT}")

if (ARMCB_AARCH64_BUILD EQUAL 1)
    add_definitions(-DARMCB_AARCH64_BUILD)
    message ("ARMCB_AARCH64_BUILD = ${ARMCB_AARCH64_BUILD}")
endif()

if (ARMCB_CROSS_BUILD LESS 2)
    option(ENABLE_PARAMS_PARSER "Enable building param parser test APP" ON)

    add_definitions(-DENABLE_ISP_APP)
    add_definitions(-DENABLE_PARAM_PARSER_APP)
    add_definitions(-DPRINTALL)

    find_path (ION_HEADER ion.h)
    if (ION_HEADER)
        add_definitions(-DLINUX_ION_ENABLE)
    endif(ION_HEADER)
endif()

#################################### Compile FLAGS ##############################################

if (ARMCB_CROSS_BUILD EQUAL 2)
    set (ARMCB_COMPILE_CFLAGS "-std=gnu99")
    set (ARMCB_COMPILE_CPPFLAGS "-std=c++11")
    set (ARMCB_COMPILE_FLAGS
        -Wall
        -O2
        -g3
        -c
        -fmessage-length=0
        -MT"$@"
        -mcpu=cortex-a9
        -mfpu=vfpv3
        -mfloat-abi=hard
        -MMD
        -MP
        -MF"${@:%.o=%.d}"
        -MT"$@"
    )
    add_compile_options (${ARMCB_COMPILE_FLAGS})
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${ARMCB_COMPILE_CFLAGS} -mcpu=cortex-a9 -mfpu=vfpv3 -mfloat-abi=hard")
    # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ARMCB_COMPILE_CPPFLAGS}")
else()
    set (ARMCB_COMPILE_CFLAGS "-std=gnu99")
    set (ARMCB_COMPILE_CPPFLAGS "-std=c++11")
    set (ARMCB_COMPILE_FLAGS
        -fPIC
        -O2
        -Wall
        -Werror
        #-Wextra
		#-Wno-error=unused-parameter
		-Wno-error=unused-function
		-Wno-error=unused-variable
		-Wno-error=unused-but-set-variable
		#-Wno-error=strict-overflow
		-Wno-unused-variable
		-Wno-unused-function
		-Wno-unused-but-set-variable
		-fno-aggressive-loop-optimizations
        -std=gnu99
    )
    add_compile_options (${ARMCB_COMPILE_FLAGS})
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${ARMCB_COMPILE_CFLAGS}")
    # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ARMCB_COMPILE_CPPFLAGS}")
endif()

if(DEFINED ENV{ASAN_ENABLE})
    set(CMAKE_C_FLAGS
        "${CMAKE_C_FLAGS} -fsanitize=address -fno-omit-frame-pointer -g")
endif()

# message (STATUS "CMAKE_C_FLAGS: ${CMAKE_C_FLAGS}")

# add_definitions (${ARMCB_COMPILE_FLAGS})

#################################### Link FLAGS ###############################################
link_directories(${ARMCB_EXTERNAL_LIB})
link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

# Awalys shared link gcc libs
if (ARMCB_CROSS_BUILD EQUAL 2)
    link_directories(${ARMCB_BSPLIB_DIR})
    link_directories(${ARMCB_TEMP_LIB_DIR})
    #link_directories(${ARMCB_OPENSOURCE_LIB})
    # set (ARMCB_LINKER_FLAGS "-mcpu=cortex-a9 -mfpu=vfpv3 -mfloat-abi=hard")
    set (ARMCB_LINKER_FLAGS "${ARMCB_LINKER_FLAGS} -Wl,-build-id=none")
    set (ARMCB_LINKER_FLAGS "${ARMCB_LINKER_FLAGS} -specs=${ARMCB_MKBARE_DIR}/Xilinx.spec")
    set (ARMCB_LINKER_FLAGS "${ARMCB_LINKER_FLAGS} -Wl,-T")
    set (ARMCB_LINKER_FLAGS "${ARMCB_LINKER_FLAGS} -Wl,${ARMCB_MKBARE_DIR}/lscript.ld")
    set (ARMCB_LINKER_FLAGS "${ARMCB_LINKER_FLAGS} -L${ARMCB_BSPLIB_DIR}")
    set (CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${ARMCB_LINKER_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${ARMCB_LINKER_FLAGS}")
else()
    # For delivery, do not delete this link path
    link_directories(${ARMCB_LIB_DIR})
    link_directories(${ARMCB_TEMP_LIB_DIR})
    #link_directories(${ARMCB_OPENSOURCE_LIB})
    set (ARMCB_LINKER_FLAGS "${ARMCB_LINKER_FLAGS} -L${ARMCB_BSPLIB_DIR}")
    set (ARMCB_LINKER_FLAGS "-Wl,-Bdynamic,-lgcc_s -lpthread -lm -lc")
    set (CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${ARMCB_LINKER_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${ARMCB_LINKER_FLAGS}")
endif()

#################################### Debug FLAGS ###############################################
# message ("ARMCB_DEBUG_BUILD = ${ARMCB_DEBUG_BUILD}")

if (ARMCB_DEBUG_BUILD)

    set (ARMCB_DEBUG_CFLAGS
        -O0
        -g
        -Wall
        -funwind-tables
        -fexceptions
    )

    add_compile_options (${ARMCB_DEBUG_CFLAGS})

    if (ARMCB_CROSS_BUILD LESS 2)
        set (ARMCB_DEBUG_LINKER_FLAGS
            "-rdynamic -export-dynamic"
        )
    endif()

    set (CMAKE_SHARED_LINKER_FLAGS
         "${CMAKE_SHARED_LINKER_FLAGS} ${ARMCB_DEBUG_LINKER_FLAGS}")

    set (CMAKE_EXE_LINKER_FLAGS
         "${CMAKE_EXE_LINKER_FLAGS} ${ARMCB_DEBUG_LINKER_FLAGS}")

endif()
#################################### Static Compile ##########################################
set (ARMCB_LIB_FLAG "SHARED")
message ("ARMCB_STATIC_COMPILE = ${ARMCB_STATIC_COMPILE}")

if (ARMCB_STATIC_COMPILE)
    set (ARMCB_LIB_FLAG "STATIC")
endif()

#################################### Sanitizer FLAGS ##########################################
# message ("ARMCB_ENABLE_SANITIZE = ${ARMCB_ENABLE_SANITIZE}")

if (ARMCB_ENABLE_SANITIZE)

    set (ARMCB_ISP_SANITIZE_CFLAGS
       -fsanitize=undefined
        #-fsanitize=address -fno-omit-frame-pointer
        #-fno-sanitize-recover
        #-fsanitize=float-divide-by-zero
    )

    include(CheckCCompilerFlag)

    check_c_compiler_flag(${ARMCB_ISP_SANITIZE_CFLAGS} IS_SANITIZER_SUPPORTED)

    if (IS_SANITIZER_SUPPORTED)
        add_compile_options(${ARMCB_ISP_SANITIZE_CFLAGS})
        set (CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -lubsan")
        set (CMAKE_EXE_LINKER_FLAGS    "${CMAKE_EXE_LINKER_FLAGS} -lubsan")
        message("Sanitizer Supported !!")
    else ()
        message("Sanitizer not Supported !!")
    endif()

endif()

#################################### Clean ###################################################
add_custom_target ( clean-all
    COMMAND make clean
    COMMAND rm -rf ${ARMCB_GENERATED_OUT}/*
    COMMAND echo "${ARMCB_GENERATED_OUT} --- cleaned"
    COMMAND rm -rf ${ARMCB_GENERATED_TEMP}/*
    COMMAND echo "${ARMCB_GENERATED_TEMP} --- cleaned"
)


set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${APP_SRC_DIR}/out/usr/bin)
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${APP_SRC_DIR}/out/usr/lib)

set(TX5215BX "TX5215BX")
set(TX5339AX "TX5339AX")
set(TX5368AX "TX5368AX")
set(TX5336EX "TX5336EX")
set(TX5112DX "TX5112DX")
set(TX5239CX "TX5239CX")
set(TX5239DX "TX5239DX")

add_definitions(-DAPP_VERSION_STR="V010R060B010_Firmware")

set(TARGET_CHIPSERIES $ENV{TARGET_CHIPSERIES})
if (TARGET_CHIPSERIES STREQUAL TX5339AX OR TARGET_CHIPSERIES STREQUAL TX5368AX OR TARGET_CHIPSERIES STREQUAL TX5335AX)
    add_compile_definitions(DX5368)
elseif(TARGET_CHIPSERIES STREQUAL TX5336EX OR TARGET_CHIPSERIES STREQUAL TX5256EX)
    add_compile_definitions(DX5336)
elseif(TARGET_CHIPSERIES STREQUAL TX5215BX  OR TARGET_CHIPSERIES STREQUAL TX5239BX)
    add_compile_definitions(DX5215)
elseif(TARGET_CHIPSERIES STREQUAL TX5112DX OR TARGET_CHIPSERIES STREQUAL TX5239DX)
    add_compile_definitions(DX5112)
endif()

if (TARGET_CHIPSERIES STREQUAL TX5339AX OR TARGET_CHIPSERIES STREQUAL TX5368AX)
    message("OSD_RESTRUCT not open")
else()
    add_compile_definitions(OSD_RESTRUCT)
endif()

set (TARGET_BOARD $ENV{TARGET_BOARD})
if(TARGET_BOARD STREQUAL "aov")
    add_compile_definitions(GC4023)
endif()



#################################### Compile FLAGS ##############################################
set (ARMCB_COMPILE_FLAGS
    -fPIC
    -Wall
    -fno-aggressive-loop-optimizations
)

set (ARMCB_COMPILE_CFLAGS
    "-std=gnu99"
)

set (ARMCB_COMPILE_CPPFLAGS
    "-std=c++11"
)

set(BUILD_TYPE $ENV{BUILD_TYPE})
if(BUILD_TYPE STREQUAL "release")
        set(ARMCB_COMPILE_FLAGS ${ARMCB_COMPILE_FLAGS}
                -O2
                -funwind-tables
                -fexceptions
        )
else()
        set(ARMCB_COMPILE_FLAGS ${ARMCB_COMPILE_FLAGS}
                -O2
                -g
                -funwind-tables
                -fexceptions
        )
endif()

add_compile_options (${ARMCB_COMPILE_FLAGS})

set(CMAKE_C_FLAGS
    "${CMAKE_C_FLAGS} ${ARMCB_COMPILE_CFLAGS}")

set(CMAKE_C_LINK_FLAGS
	"${CMAKE_C_LINK_FLAGS} -Wl,--gc-sections,--no-export-dynamic")

if(DEFINED ENV{ASAN_ENABLE})
    set(CMAKE_C_FLAGS
        "${CMAKE_C_FLAGS} -fsanitize=address -fno-omit-frame-pointer -g")
    set(CMAKE_CXX_FLAGS
	"${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer -g")
endif()

if(DEFINED ENV{ENABLE_COVERAGE})
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fprofile-arcs -ftest-coverage")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
endif()

if (TARGET_CHIPSERIES STREQUAL TX5339AX OR TARGET_CHIPSERIES STREQUAL TX5368AX OR TARGET_CHIPSERIES STREQUAL TX5335AX)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=armv8-a -mcpu=cortex-a53")
elseif(TARGET_CHIPSERIES STREQUAL TX5336EX OR TARGET_CHIPSERIES STREQUAL TX5256EX)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=armv8-a -mcpu=cortex-a53")
elseif(TARGET_CHIPSERIES STREQUAL TX5215BX  OR TARGET_CHIPSERIES STREQUAL TX5239BX)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=armv7-a -mfpu=neon-vfpv4")
elseif(TARGET_CHIPSERIES STREQUAL TX5112DX OR TARGET_CHIPSERIES STREQUAL TX5239DX)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=armv7-a -mfpu=neon-vfpv4")
endif()

if("${MPP_USE_PCSIMULATOR}" EQUAL 1)
link_directories(${APP_SRC_DIR}/libs/x86)
else()
include_directories($ENV{APP_SRC_DIR}/common)
include_directories($ENV{TARGET_STAGING_INC})
link_directories($ENV{TARGET_STAGING_LIB})
include_directories($ENV{TARGET_STAGING_INC}/algstore)
link_directories($ENV{TARGET_STAGING_LIB}/algstore)

#[[
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../libs/algstore/libalgopus_v1_4/lib/tx5215/glibc)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../libs/algstore/libalgaacenc/lib/tx5215/glibc)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../libs/algstore/libalgaacdec/lib/tx5215/glibc)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../libs/algstore/libalgg726/lib/tx5215/glibc)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../libs/algstore/libalgadpcm/lib/tx5215/glibc)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../libs/algstore/libalgg711/lib/tx5215/glibc)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../libs/algstore/libalgcommon/include)
]]
endif()
#################################### Link FLAGS ###############################################

set(TARGET_SDK_BOOT_MODE $ENV{TARGET_SDK_BOOT_MODE})
set(FASTBOOT_STR "fastboot")
message("TARGET_SDK_BOOT_MODE = ${TARGET_SDK_BOOT_MODE}")
message("FASTBOOT_STR = ${FASTBOOT_STR}")
if(TARGET_SDK_BOOT_MODE STREQUAL FASTBOOT_STR)
    message("--------------- fastboot")
    add_definitions(-DTARGET_SDK_MODE_FASTBOOT)
else()
    message("--------------- NOT fastboot")
endif()

set(LUNCH_SEL_NUM $ENV{LUNCH_SEL_NUM})
set(718_STR "718")
set(725_STR "725")
set(750_STR "750")
set(751_STR "751")
set(713_STR "713")
set(325_STR "325")
set(329_STR "329")
set(300_STR "300")
set(343_STR "343")
set(344_STR "344")
message("LUNCH_SEL_NUM = ${LUNCH_SEL_NUM}")
message("718_STR = ${718_STR}")


# 定义函数来查找静态库文件并提取其文件名
function(find_and_append_library_names input_libs output_libs)
    set(CMAKE_FIND_LIBRARY_SUFFIXES .a)
    set(SUGGG "")
    foreach(temp ${input_libs})
        find_library(LIB_${temp} NAMES ${temp} HINTS "$ENV{TARGET_STAGING_LIB}" ${ARGN})
        if(LIB_${temp})
            # message(STATUS "Found: ${LIB_${temp}}")
            # 提取文件名
            get_filename_component(LIB_NAME ${LIB_${temp}} NAME)
            list(APPEND SUGGG ${LIB_NAME})
        else()
            message(WARNING "Library ${temp} not found")
        endif()
    endforeach()
    set(${output_libs} ${SUGGG} PARENT_SCOPE)
endfunction()

function(find_and_append_algstore_library_names input_libs output_libs)
    set(CMAKE_FIND_LIBRARY_SUFFIXES .a)
    set(SUGGG "")
    foreach(temp ${input_libs})
        find_library(LIB_${temp} NAMES ${temp} HINTS "$ENV{TARGET_STAGING_LIB}/algstore" ${ARGN})
        if(LIB_${temp})
            # message(STATUS "Found: ${LIB_${temp}}")
            # 提取文件名
            get_filename_component(LIB_NAME ${LIB_${temp}} NAME)
            list(APPEND SUGGG ${LIB_NAME})
        else()
            message(WARNING "Library ${temp} not found")
        endif()
    endforeach()
    set(${output_libs} ${SUGGG} PARENT_SCOPE)
endfunction()

set(MPI_BASE_LIB
    rtsp_server_gb
    3a
    mpi
    omx
    vpu_comm
    vpu_sw
	rt
)

set(MPI_STATIC_LIB
    mpp_sys
    omxc
    omxc_adec
    omxc_aenc
    omxc_ai
    omxc_ao
    omxc_cpm
    omxc_soft_vgs
    omxc_split
    omxc_videosplit
    omxc_vipipe
    omxc_vo
    omxc_vpe
    omxc_vpu
    omxc_vonode
    drm
    isp_comm_hal
    tinyalsa
    up_vqe
    down_vqe
    vdi
    pthread
    dl
)

set(ISP_COMM_LIB
    isp
    isp_hal
    isp_osif
    buf_mgr
    cjson
    json_convert
    cmdr
    frm_mgr
    isp_dpc
    isp_lsh
    isp_mvd
    isp_submod_common
    isp_top
    isp_tpg
    isp_vin
    sen_modules
    sensor
    ui_uart
)

set(ISP_COMM_LIB_EXCEPT_53
    aecd
    awbd
    afcd
    aec1d
    awb1d
    afc1d
    isp_stats
)

set(ISP_STATIC_LIB
    ### 只引用一次
    isp_utils
    isp_awbstat
    isp_rnr
    isp_vout
    isp_wbg
    ### 重复引用
    isp_aestat
    isp_afstat
    isp_blc
    isp_cac
    isp_ccm
    isp_cmb
    isp_cnr
    isp_csc
    isp_csc2
    isp_dcp
    isp_dgn
    isp_dms
    isp_dpf
    isp_ggc
    isp_l2q
    isp_lnc
    isp_lsc
    isp_ltm
    isp_np
    isp_psc
    isp_rir
    isp_scale
    isp_sde
    isp_tnr
    isp_ltm_sw
)

set(ISP_DYNAMIC_LIB
    aec
    af
    aiisp
    api
    awb
    dof
    hw_env
    isp_adb
    isp_aestat
    isp_afbc
    isp_afstat
    isp_blc
    isp_cac
    isp_cbc
    isp_ccm
    isp_clut
    isp_cmb
    isp_cnr
    isp_csc
    isp_csc2
    isp_dcp
    isp_dgn
    isp_dms
    isp_dpf
    isp_driver
    isp_dwp
    isp_gdc
    isp_ggc
    isp_ggc
    isp_hsv
    isp_idpc
    isp_ifbc
    isp_ifbd
    isp_l2q
    isp_lnc
    isp_lsc
    isp_ltm
    isp_ltm_sw
    isp_mcfb
    isp_np
    isp_pce
    isp_psc
    isp_ptm
    isp_q2l
    isp_rir
    isp_rtnr
    isp_scale
    isp_sde
    isp_tnr
    isp_ynr
    isp_ytnr
    isp_gmd
)

set(ISP_SENSOR_IMX415_LIB
    calibration_imx415
    csi2d_imx415
    sensor_imx415
    tuning_imx415_dol2
    tuning_imx415_normal
)

set(ISP_SENSOR_GC4023_LIB
    sensor_gc4023
    csi2d_gc4023
    tuning_gc4023_normal
    calibration_gc4023
)

set(ISP_SENSOR_CV2003_LIB
    calibration_cv2003
    csi2d_cv2003
    sensor_cv2003
    tuning_cv2003_dol2
    tuning_cv2003_normal
)

set(ISP_SENSOR_SC2337P_LIB
    calibration_sc2337p
    csi2d_sc2337p
    sensor_sc2337p
    tuning_sc2337p_normal
)

set(ISP_STATIC_LIB_FOR_53
    fw_utils
    pproc
    calibration_os04a10
    calibration_sc200ai
    calibration_sc450ai
    calibration_sc850sl
    csi2d_imx415_4lane
    csi2d_os04a10
    csi2d_sc200ai
    csi2d_sc450ai
    csi2d_sc850sl
    sensor_imx415_4lane
    sensor_os04a10
    sensor_sc200ai
    sensor_sc450ai
    sensor_sc850sl
    tuning_os04a10_dol2
    tuning_os04a10_normal
    tuning_sc200ai_normal
    tuning_sc450ai_normal
    tuning_sc850sl_normal
    aec
    af
    api
    awb
    hw_env
    isp_adb
    isp_afbc
    isp_cbc
    isp_clut
    isp_driver
    isp_dwp
    isp_gdc
    # isp_gmd
    isp_hsv
    isp_idpc
    isp_ifbc
    isp_ifbd
    isp_mcfb
    isp_pce
    isp_ptm
    isp_q2l
    isp_rtnr
    isp_ynr
    isp_ytnr
    dof
    ${ISP_SENSOR_IMX415_LIB}
)

set(ISP_AIISP_LIB
    # aiisp
    algcommon
	algutil_cv
    algaiisp
    algaiisp_rfr
)

set(ISP_RNE_LIB
    # aiisp
    trp
    rne_rt_g3
	rne_pal_linux
	rne_common
)

set(BUILD_ISP_TUNNING_LIB
    niss
)



### 判断 BUILD_ISP_TUNNING 是否定义环境变量和环境变量的值是否是 Y
set(BUILD_ISP_TUNNING $ENV{BUILD_ISP_TUNNING})
if((NOT DEFINED ENV{BUILD_ISP_TUNNING}) OR (BUILD_ISP_TUNNING STREQUAL "y"))
    list(APPEND MPI_BASE_LIB ${BUILD_ISP_TUNNING_LIB})
endif()

if("${MPP_USE_PCSIMULATOR}" EQUAL 0)
    message("MPP_USE_PCSIMULATOR is 0 in common.cmake")
    if (TARGET_CHIPSERIES STREQUAL TX5339AX OR TARGET_CHIPSERIES STREQUAL TX5368AX)
        # link_libraries(omxc_gpu)
        # link_libraries(GAL)
        # link_libraries(drm)
        list(APPEND MPI_BASE_LIB omxc_gpu GAL drm)
        message("link file for TX5368AX")
    elseif(TARGET_CHIPSERIES STREQUAL TX5336EX )
        add_definitions(-DTX5336)
		link_libraries(libdrm.so)
        message("link file for TX5336EX ")
    elseif(TARGET_CHIPSERIES STREQUAL TX5215BX)
        add_definitions(-DTX5215)
		link_libraries(libdrm.so)
        message("link file for TX5215BX")
    else()
        message("link file for non-TX5368AX")
    endif()

    if("${APP_STATIC_COMPILE}" EQUAL 0)
        list(APPEND MPI_BASE_LIB ${ISP_COMM_LIB})
        if(TARGET_CHIPSERIES STREQUAL TX5336EX)
            list(APPEND MPI_BASE_LIB ${ISP_DYNAMIC_LIB})
        else()
            list(APPEND MPI_BASE_LIB ${ISP_COMM_LIB_EXCEPT_53})
        endif()

    else()

        # 设置查找动态库的优先顺序
        set(CMAKE_FIND_LIBRARY_SUFFIXES .a .so)
        find_and_append_library_names("${MPI_BASE_LIB}" TEMP_LIST)
        message(STATUS "tranlated------- ${TEMP_LIST}")
        set(MPI_BASE_LIB ${TEMP_LIST})

        find_and_append_library_names("${MPI_STATIC_LIB}" TEMP_LIST)
        message(STATUS "tranlated------- ${TEMP_LIST}")
        set(MPI_STATIC_LIB ${TEMP_LIST})

        find_and_append_library_names("${ISP_COMM_LIB}" TEMP_LIST)
        message(STATUS "tranlated------- ${TEMP_LIST}")
        set(ISP_COMM_LIB ${TEMP_LIST})

        find_and_append_library_names("${ISP_STATIC_LIB}" TEMP_LIST)
        message(STATUS "tranlated------- ${TEMP_LIST}")
        set(ISP_STATIC_LIB ${TEMP_LIST})

        list(APPEND MPI_BASE_LIB ${MPI_STATIC_LIB})
        list(APPEND MPI_BASE_LIB ${ISP_COMM_LIB})
        list(APPEND MPI_BASE_LIB ${ISP_STATIC_LIB})

			message(STATUS "tranlated------- ${TEMP_LIST}")
        ### 静态编译
        if (TARGET_CHIPSERIES STREQUAL TX5336EX)
            find_and_append_library_names("${ISP_STATIC_LIB_FOR_53}" TEMP_LIST)
            message(STATUS "tranlated------- ${TEMP_LIST}")
            set(ISP_STATIC_LIB_FOR_53 ${TEMP_LIST})

            # 算法库和别的库不在同一目录下，所以需要单独查找
            find_and_append_algstore_library_names("${ISP_AIISP_LIB}" TEMP_LIST)
            message(STATUS "tranlated------- ${TEMP_LIST}")
            set(ISP_AIISP_LIB ${TEMP_LIST})

            find_and_append_library_names("${ISP_RNE_LIB}" TEMP_LIST)
            message(STATUS "tranlated------- ${TEMP_LIST}")
            set(ISP_RNE_LIB ${TEMP_LIST})

            list(APPEND MPI_BASE_LIB ${ISP_STATIC_LIB_FOR_53})
            list(APPEND MPI_BASE_LIB ${ISP_AIISP_LIB})
            list(APPEND MPI_BASE_LIB ${ISP_RNE_LIB})
            list(APPEND MPI_BASE_LIB aiisp)
            list(APPEND MPI_BASE_LIB omxc_vonode)
			list(APPEND MPI_BASE_LIB drm)
			list(APPEND MPI_BASE_LIB stdc++)
			list(APPEND MPI_BASE_LIB rt)
			set(MPI_GDC_LIB gdc omxc_gdc)
			find_and_append_library_names("${MPI_GDC_LIB}" TEMP_LIST)
			message(STATUS "tranlated------- ${TEMP_LIST}")
			set(MPI_GDC_LIB ${TEMP_LIST})
			list(APPEND MPI_BASE_LIB ${MPI_GDC_LIB})
			message(${MPI_GDC_LIB})
			message(STATUS "tranlated------- ${TEMP_LIST}")
        else()

            find_and_append_library_names("${ISP_COMM_LIB_EXCEPT_53}" TEMP_LIST)
            message(STATUS "tranlated------- ${TEMP_LIST}")
            set(ISP_COMM_LIB_EXCEPT_53 ${TEMP_LIST})
            list(APPEND MPI_BASE_LIB ${ISP_COMM_LIB_EXCEPT_53})
            list(APPEND MPI_BASE_LIB isp_ttnr isp_osutils)
            # link_libraries (isp_ttnr)
            # link_libraries (isp_osutils)
            if(LUNCH_SEL_NUM STREQUAL 718_STR OR LUNCH_SEL_NUM STREQUAL 713_STR
            OR LUNCH_SEL_NUM STREQUAL 725_STR OR LUNCH_SEL_NUM STREQUAL 325_STR
			OR LUNCH_SEL_NUM STREQUAL 329_STR)
                message("static link sensor: imx415")
                find_and_append_library_names("${ISP_SENSOR_IMX415_LIB}" TEMP_LIST)
                message(STATUS "tranlated------- ${TEMP_LIST}")
                set(ISP_SENSOR_IMX415_LIB ${TEMP_LIST})
                list(APPEND MPI_BASE_LIB ${ISP_SENSOR_IMX415_LIB})
            elseif(LUNCH_SEL_NUM STREQUAL 751_STR)
                message("static link sensor: cv2003")
                find_and_append_library_names("${ISP_SENSOR_CV2003_LIB}" TEMP_LIST)
                message(STATUS "tranlated------- ${TEMP_LIST}")
                set(ISP_SENSOR_CV2003_LIB ${TEMP_LIST})
                list(APPEND MPI_BASE_LIB ${ISP_SENSOR_CV2003_LIB})
			elseif(LUNCH_SEL_NUM STREQUAL 300_STR)
                find_and_append_library_names("${ISP_SENSOR_GC4023_LIB}" TEMP_LIST)
                message(STATUS "tranlated------- ${TEMP_LIST}")
                set(ISP_SENSOR_GC4023_LIB ${TEMP_LIST})
                message("static link sensor: gc4023")
                list(APPEND MPI_BASE_LIB ${ISP_SENSOR_GC4023_LIB})
	    elseif(LUNCH_SEL_NUM STREQUAL 343_STR OR LUNCH_SEL_NUM STREQUAL 344_STR)
                find_and_append_library_names("${ISP_SENSOR_SC2337P_LIB}" TEMP_LIST)
                message(STATUS "tranlated------- ${TEMP_LIST}")
                set(ISP_SENSOR_SC2337P_LIB ${TEMP_LIST})
                message("static link sensor: sc2337p")
                list(APPEND MPI_BASE_LIB ${ISP_SENSOR_SC2337P_LIB})
            else()
                find_and_append_library_names("${ISP_SENSOR_GC4023_LIB}" TEMP_LIST)
                message(STATUS "tranlated------- ${TEMP_LIST}")
                set(ISP_SENSOR_GC4023_LIB ${TEMP_LIST})
                message("static link sensor: gc4023")
                list(APPEND MPI_BASE_LIB ${ISP_SENSOR_GC4023_LIB})
            endif()
			if(LUNCH_SEL_NUM STREQUAL 300_STR OR LUNCH_SEL_NUM STREQUAL 325_STR
			OR LUNCH_SEL_NUM STREQUAL 329_STR)
				list(APPEND MPI_BASE_LIB omxc_vonode)
				list(APPEND MPI_BASE_LIB drm)
			endif()
        endif()
    endif()
else()
	add_definitions(-DMPP_USE_PCSIMULATOR)
    message("MPP_USE_PCSIMULATOR is 1 in common.cmake")
    message("TARGET_APPS_DIR is ${TARGET_APPS_DIR}")
#		link_libraries("${TARGET_APPS_DIR}/libs/x86/libmpi.so")
#		link_libraries("${TARGET_APPS_DIR}/libs/x86/libomx.so")
		link_libraries("${TARGET_APPS_DIR}/libs/x86/libdrm.so")
endif()


set (ARMCB_LINKER_FLAGS
     "-Wl,-Bdynamic,-lgcc_s -lpthread -lm -lc")

set (CMAKE_SHARED_LINKER_FLAGS
     "${CMAKE_SHARED_LINKER_FLAGS} ${ARMCB_LINKER_FLAGS}")


set (CMAKE_EXE_LINKER_FLAGS
     "${CMAKE_EXE_LINKER_FLAGS} ${ARMCB_LINKER_FLAGS}")

set (ARMCB_DEBUG_LINKER_FLAGS
     "-rdynamic -export-dynamic"
)

set (CMAKE_SHARED_LINKER_FLAGS
     "${CMAKE_SHARED_LINKER_FLAGS} ${ARMCB_DEBUG_LINKER_FLAGS}")

set (CMAKE_EXE_LINKER_FLAGS
     "${CMAKE_EXE_LINKER_FLAGS} ${ARMCB_DEBUG_LINKER_FLAGS}")

#################################### Clean ###################################################
add_custom_target ( clean-all
    COMMAND make clean
    COMMAND rm -rf ${APP_SRC_DIR}/out/*
    COMMAND echo "${APP_SRC_DIR}/out --- cleaned"
    COMMAND rm -rf ${APP_SRC_DIR}/temp/*
    COMMAND echo "${APP_SRC_DIR}/temp --- cleaned"
)

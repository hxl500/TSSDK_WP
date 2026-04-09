set (SAMPLE_SMART_ENCODE_DIR            ${APP_SRC_DIR}/smart_encode)
set (SAMPLE_COMMON_DIR                ${APP_SRC_DIR}/common)
#################################### Include Directories #######################################
include_directories(${MPP_ROOT_DIR})
#include_directories(${APP_SRC_DIR}/../include)
include_directories(${APP_SRC_DIR}/../../libs/mpp/include)
include_directories(${APP_SRC_DIR}/../../libs/live555/include/live555)

if ($ENV{TARGET_UCLIBC} MATCHES uclibc)
    set(LIBC uclibc)
else ()
    set(LIBC glibc)
endif ()

################################################ clock_app_test ###################################################
if("${MPP_USE_PCSIMULATOR}" STREQUAL "0")
	if(TARGET_CHIPSERIES STREQUAL "TX5368AX")
		add_definitions(-DSENSOR0_TYPE=EVB_SENSOR_4K)
	else()
		add_definitions(-DSENSOR0_TYPE=EVB_SENSOR_4MP)
	endif()
	add_definitions(-DSENSOR1_TYPE=SONY_IMX327_MIPI_2M_30FPS_12BIT)
	add_definitions(-DVENC_IN_ZERO_COPY_V2)
	add_definitions(-DHIDE_UNVERIFIED_CASE)
	add_definitions(-DHIDE_PRINT_0919)
	add_definitions(-DIMPR_PRINT)
	set(SAMPLE_SMART_ENCODE sample_smart_encode)

else()
	add_definitions(-DSENSOR0_TYPE=X86_LOGITECH_HD1080)
	add_definitions(-DSENSOR1_TYPE=SONY_IMX327_MIPI_2M_30FPS_12BIT)
	set(SAMPLE_SMART_ENCODE sample_smart_encode_x86)
endif()


if(TARGET_CHIPSERIES STREQUAL TX5215BX OR  TARGET_CHIPSERIES STREQUAL TX5239CX)
	add_executable(${SAMPLE_SMART_ENCODE}
		${SAMPLE_COMMON_DIR}/sample_comm_isp.c
		${SAMPLE_COMMON_DIR}/sample_comm_sys.c
		${SAMPLE_COMMON_DIR}/sample_comm_vi.c
		${SAMPLE_COMMON_DIR}/sample_comm_vpss.c
		${SAMPLE_COMMON_DIR}/sample_comm_venc.c
		${SAMPLE_COMMON_DIR}/sample_comm_cpm.c
		${SAMPLE_COMMON_DIR}/loadbmp.c
		${SAMPLE_COMMON_DIR}/soft_line.c
		${SAMPLE_SMART_ENCODE_DIR}/soft_alphabet_overlay.c
		${SAMPLE_SMART_ENCODE_DIR}/sample_smart_encode.c
		${SAMPLE_SMART_ENCODE_DIR}/sample_alg_cpm.c
		${SAMPLE_SMART_ENCODE_DIR}/video_common_utils.c
		${SAMPLE_SMART_ENCODE_DIR}/video_alg_motiondetect.c
	)
else()
	message(STATUS "[ERROR] The current chip series does not support this feature.")

endif()


TARGET_LINK_LIBRARIES(${SAMPLE_SMART_ENCODE} -Wl,--start-group dl pthread stdc++ 
	# librtsp_server_gb.a 
	${MPI_BASE_LIB}
	algcommon
	algutil_cv
	rne_rt_g3
	rne_pal_linux
	rne_common
	trp
	algmotiondetect
	-Wl,--end-group
)

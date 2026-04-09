set (SAMPLE_ALG_VIDEO_DIR            ${APP_SRC_DIR}/tsalg_video)
set (SAMPLE_COMMON_DIR                ${APP_SRC_DIR}/common)
#################################### Include Directories #######################################
include_directories(${MPP_ROOT_DIR})
include_directories(${SAMPLE_ALG_VIDEO_DIR})
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
	set(SAMPLE_APP_ALG_VIDEO sample_alg_video)

else()
	add_definitions(-DSENSOR0_TYPE=X86_LOGITECH_HD1080)
	add_definitions(-DSENSOR1_TYPE=SONY_IMX327_MIPI_2M_30FPS_12BIT)
	set(SAMPLE_APP_ALG_VIDEO sample_alg_video_x86)
endif()


link_directories(
	        ${SAMPLE_ALG_VIDEO_DIR}
		)

set(ALGO_LIB 
	trp
	algmotiondetect
	algpcppdetect
	cat_detect_lib
)

if(TARGET_CHIPSERIES STREQUAL "TX5368AX" OR TARGET_CHIPSERIES STREQUAL "TX5339AX")
	add_executable(${SAMPLE_APP_ALG_VIDEO}
		${SAMPLE_COMMON_DIR}/sample_comm_isp.c
		${SAMPLE_COMMON_DIR}/sample_comm_sys.c
		${SAMPLE_COMMON_DIR}/sample_comm_vi.c
		${SAMPLE_COMMON_DIR}/sample_comm_vpss.c
		${SAMPLE_COMMON_DIR}/sample_comm_venc.c
		${SAMPLE_COMMON_DIR}/sample_comm_cpm.c
		${SAMPLE_COMMON_DIR}/loadbmp.c
		${SAMPLE_COMMON_DIR}/soft_line.c
		${SAMPLE_ALG_VIDEO_DIR}/soft_alphabet_overlay.c
		${SAMPLE_ALG_VIDEO_DIR}/sample_alg_video.c
		${SAMPLE_ALG_VIDEO_DIR}/sample_alg_cpm.c
		${SAMPLE_ALG_VIDEO_DIR}/video_common_utils.c
		${SAMPLE_ALG_VIDEO_DIR}/video_alg_motiondetect.c
		${SAMPLE_ALG_VIDEO_DIR}/video_alg_pcppdetect.c 
	)
		
elseif(TARGET_CHIPSERIES STREQUAL TX5215BX OR  TARGET_CHIPSERIES STREQUAL TX5239CX)
	add_executable(${SAMPLE_APP_ALG_VIDEO}
	${SAMPLE_COMMON_DIR}/sample_comm_isp.c
	${SAMPLE_COMMON_DIR}/sample_comm_sys.c
	${SAMPLE_COMMON_DIR}/sample_comm_vi.c
	${SAMPLE_COMMON_DIR}/sample_comm_vpss.c
	${SAMPLE_COMMON_DIR}/sample_comm_venc.c
	${SAMPLE_COMMON_DIR}/sample_comm_cpm.c
	${SAMPLE_COMMON_DIR}/loadbmp.c
	${SAMPLE_COMMON_DIR}/soft_line.c
	${SAMPLE_ALG_VIDEO_DIR}/soft_alphabet_overlay.c
	${SAMPLE_ALG_VIDEO_DIR}/sample_alg_video.c
	${SAMPLE_ALG_VIDEO_DIR}/sample_alg_cpm.c
	${SAMPLE_ALG_VIDEO_DIR}/video_common_utils.c
	${SAMPLE_ALG_VIDEO_DIR}/video_alg_motiondetect.c
	${SAMPLE_ALG_VIDEO_DIR}/video_alg_pcppdetect.c 
	${SAMPLE_ALG_VIDEO_DIR}/video_alg_aiperimeter.c 
	${SAMPLE_ALG_VIDEO_DIR}/video_alg_bodytrack.c
	${SAMPLE_ALG_VIDEO_DIR}/video_alg_catdetect.c
	${SAMPLE_ALG_VIDEO_DIR}/sample_motor.c
	)

	list(APPEND ALGO_LIB algpbcedetect algpts algaiperimeter algbodydetect)
else()
	add_executable(${SAMPLE_APP_ALG_VIDEO}
	${SAMPLE_COMMON_DIR}/sample_comm_isp.c
	${SAMPLE_COMMON_DIR}/sample_comm_sys.c
	${SAMPLE_COMMON_DIR}/sample_comm_vi.c
	${SAMPLE_COMMON_DIR}/sample_comm_vpss.c
	${SAMPLE_COMMON_DIR}/sample_comm_venc.c
	${SAMPLE_COMMON_DIR}/sample_comm_cpm.c
	${SAMPLE_COMMON_DIR}/loadbmp.c
	${SAMPLE_COMMON_DIR}/soft_line.c
	${SAMPLE_ALG_VIDEO_DIR}/video_common_utils.c
	${SAMPLE_ALG_VIDEO_DIR}/soft_alphabet_overlay.c
	${SAMPLE_ALG_VIDEO_DIR}/sample_alg_video.c
	${SAMPLE_ALG_VIDEO_DIR}/sample_alg_cpm.c
	${SAMPLE_ALG_VIDEO_DIR}/video_alg_motiondetect.c
	${SAMPLE_ALG_VIDEO_DIR}/video_alg_pcppdetect.c
	)

endif()


TARGET_LINK_LIBRARIES(${SAMPLE_APP_ALG_VIDEO} -Wl,--start-group dl pthread stdc++ 
	# librtsp_server_gb.a 
	${MPI_BASE_LIB}
	algcommon
	algutil_cv
	#libplat_rne_linux_a53.a libchip_sdk.a 
	rne_rt_g3
	rne_pal_linux
	rne_common
	${ALGO_LIB}
	-Wl,--end-group
)

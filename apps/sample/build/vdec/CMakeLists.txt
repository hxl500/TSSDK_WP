

set (SAMPLE_COMMON_DIR         ${APP_SRC_DIR}/common)
set (SAMPLE_VDEC_DIR            ${APP_SRC_DIR}/vdec)
#set (MPP_ROOT_DIR		   	   ${TARGET_LIBS_DIR}/mpp)
set (SAMPLE_OSD_DIR            ${APP_SRC_DIR}/common/osd)
#################################### Include Directories #######################################
include_directories(${MPP_ROOT_DIR})
include_directories(${SAMPLE_COMMON_DIR})
include_directories(${APP_SRC_DIR}/../include)
include_directories(${APP_SRC_DIR}/../../libs/mpp/include)
include_directories($ENV{TARGET_STAGING_INC}/3rd)
include_directories(${SAMPLE_OSD_DIR})

################################################ clock_app_test ###################################################
if("${MPP_USE_PCSIMULATOR}" STREQUAL "0")
	add_definitions(-DSENSOR0_TYPE=EVB_SENSOR)
	add_definitions(-DSENSOR1_TYPE=SONY_IMX327_MIPI_2M_30FPS_12BIT)
	add_definitions(-DVENC_IN_ZERO_COPY_V2)
	add_definitions(-DHIDE_UNVERIFIED_CASE)
	set(SAMPLE_APP_VDEC sample_vdec)
else()
	add_definitions(-DSENSOR0_TYPE=X86_LOGITECH_HD720)
	add_definitions(-DSENSOR1_TYPE=SONY_IMX327_MIPI_2M_30FPS_12BIT)
	set(SAMPLE_APP_VDEC sample_vdec_x86)
endif()

if(TARGET_CHIPSERIES STREQUAL "TX5368AX" OR TARGET_CHIPSERIES STREQUAL "TX5339AX")
add_executable(

	${SAMPLE_APP_VDEC}

	${SAMPLE_OSD_DIR}/bitmap.c
	${SAMPLE_OSD_DIR}/osd_truetype.c
	${SAMPLE_OSD_DIR}/osd_timestamp.c
	${SAMPLE_COMMON_DIR}/sample_comm_isp.c
	${SAMPLE_COMMON_DIR}/sample_comm_sys.c
	${SAMPLE_COMMON_DIR}/sample_comm_vi.c
	${SAMPLE_COMMON_DIR}/sample_comm_vo.c
	${SAMPLE_COMMON_DIR}/sample_comm_vdec.c
	${SAMPLE_COMMON_DIR}/sample_comm_venc.c
	#${SAMPLE_COMMON_DIR}/sample_comm_vdec_rtsp.c
	${SAMPLE_COMMON_DIR}/sample_comm_cpm.c
	${SAMPLE_COMMON_DIR}/sample_comm_vpss.c
	${SAMPLE_COMMON_DIR}/loadbmp.c
	${SAMPLE_COMMON_DIR}/soft_line.c
	${SAMPLE_COMMON_DIR}/sample_comm_region.c
	#${SAMPLE_COMMON_DIR}/sample_comm_algo.c
	${SAMPLE_VDEC_DIR}/sample_vdec.c)
else()
add_executable(
	${SAMPLE_APP_VDEC}
	${SAMPLE_OSD_DIR}/bitmap.c
	${SAMPLE_OSD_DIR}/osd_truetype.c
	${SAMPLE_OSD_DIR}/osd_timestamp.c
	${SAMPLE_COMMON_DIR}/sample_comm_isp.c
	${SAMPLE_COMMON_DIR}/sample_comm_sys.c
	${SAMPLE_COMMON_DIR}/sample_comm_vi.c
	${SAMPLE_COMMON_DIR}/sample_comm_vdec.c
	${SAMPLE_COMMON_DIR}/sample_comm_venc.c
	${SAMPLE_COMMON_DIR}/sample_comm_cpm.c
	${SAMPLE_COMMON_DIR}/sample_comm_vpss.c
	${SAMPLE_COMMON_DIR}/loadbmp.c
	${SAMPLE_COMMON_DIR}/soft_line.c
	${SAMPLE_COMMON_DIR}/sample_comm_region.c
	# ${SAMPLE_COMMON_DIR}/sample_comm_algo.c
	${SAMPLE_VDEC_DIR}/sample_vdec.c
)
endif()

if(TARGET_CHIPSERIES STREQUAL "TX5368AX" OR TARGET_CHIPSERIES STREQUAL "TX5339AX" OR TARGET_CHIPSERIES STREQUAL "TX5336EX")
	TARGET_LINK_LIBRARIES(${SAMPLE_APP_VDEC} libalgbodydetect2.a)
else()
	TARGET_LINK_LIBRARIES(${SAMPLE_APP_VDEC} libalgbodydetect.a)
endif()

TARGET_LINK_LIBRARIES(${SAMPLE_APP_VDEC} libalgfacedetect2.a)
TARGET_LINK_LIBRARIES(${SAMPLE_APP_VDEC} libalgmotiondetect.a)
TARGET_LINK_LIBRARIES(${SAMPLE_APP_VDEC} libalgpcppdetect.a)

if(TARGET_CHIPSERIES STREQUAL "TX5368AX" OR TARGET_CHIPSERIES STREQUAL "TX5339AX" OR TARGET_CHIPSERIES STREQUAL "TX5336EX")
	TARGET_LINK_LIBRARIES(${SAMPLE_APP_VDEC} -Wl,--start-group dl pthread stdc++
	librne_rt_g3.a
	librne_pal_linux.a
	librne_common.a
	libalgcommon.a
	libalgutil_cv.a
	libtrp.a
	avformat avutil avcodec avutil swscale
	# avformat avutil avcodec avfilter avutil swresample swscale
	${MPI_BASE_LIB}
	-Wl,--end-group)
else()
	TARGET_LINK_LIBRARIES(${SAMPLE_APP_VDEC} -Wl,--start-group dl pthread stdc++
	${MPI_BASE_LIB}
	librne_rt_g3.a
	librne_pal_linux.a
	librne_common.a
	libalgcommon.a
	libalgutil_cv.a
	libtrp.a
	-Wl,--end-group)
endif()

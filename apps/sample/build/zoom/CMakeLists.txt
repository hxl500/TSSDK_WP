

set (SAMPLE_COMMON_DIR         ${APP_SRC_DIR}/common)
set (SAMPLE_ZOOM_DIR            ${APP_SRC_DIR}/zoom)
set (SAMPLE_OSD_DIR            ${APP_SRC_DIR}/common/osd)
#set (MPP_ROOT_DIR		   	   ${TARGET_LIBS_DIR}/mpp)

#################################### Include Directories #######################################
#include_directories(${MPP_ROOT_DIR})
#include_directories(${SAMPLE_COMMON_DIR})
#include_directories(${APP_SRC_DIR}/../include)
#include_directories(${APP_SRC_DIR}/../../libs/mpp/include)
#include_directories(${APP_SRC_DIR}/../../libs/live555/include/live555)
include_directories(${SAMPLE_OSD_DIR})
set(TARGET_CHIPSERIES $ENV{TARGET_CHIPSERIES})
message("TARGET_CHIPSERIES = ${TARGET_CHIPSERIES}")

################################################ clock_app_test ###################################################
if("${MPP_USE_PCSIMULATOR}" STREQUAL "0")
	if(TARGET_CHIPSERIES STREQUAL "TX5368AX")
		add_definitions(-DSENSOR0_TYPE=EVB_SENSOR_4K)
	else()
		add_definitions(-DSENSOR0_TYPE=EVB_SENSOR_4MP)
	endif()
	add_definitions(-DSENSOR1_TYPE=EVB_SENSOR)

	add_definitions(-DIMPR_PRINT)

	set(SAMPLE_APP_ZOOM sample_zoom)
	#link_directories(${APP_SRC_DIR}/../../libs/live555/libs/arm/live/)

#	HAPS:
#	add_definitions(-DSENSOR0_TYPE=HAPS_ARMISP)
#	add_definitions(-DSENSOR1_TYPE=SONY_IMX327_MIPI_2M_30FPS_12BIT)
#	set(SAMPLE_APP_ZOOM sample_zoom)
#	link_directories(
#		${APP_SRC_DIR}/../../libs/live555/libs/arm/live/
#		)
else()
	add_definitions(-DSENSOR0_TYPE=X86_LOGITECH_HD720)
	add_definitions(-DSENSOR1_TYPE=SONY_IMX327_MIPI_2M_30FPS_12BIT)
	set(SAMPLE_APP_ZOOM sample_zoom_x86)
endif()

set(TARGET_BOARD $ENV{TARGET_BOARD})
message("TARGET_BOARD = ${TARGET_BOARD}")
if(TARGET_BOARD STREQUAL "fpga")
message("fpga!")
add_compile_definitions(FPGA_DEBUG)
else()
message("not fpga!")
endif()

add_executable(
    ${SAMPLE_APP_ZOOM}
${SAMPLE_COMMON_DIR}/sample_comm_isp.c
${SAMPLE_COMMON_DIR}/sample_comm_sys.c
${SAMPLE_COMMON_DIR}/sample_comm_vi.c
#${SAMPLE_COMMON_DIR}/sample_comm_vo.c
${SAMPLE_COMMON_DIR}/sample_comm_venc.c
${SAMPLE_COMMON_DIR}/sample_comm_vpss.c
${SAMPLE_COMMON_DIR}/sample_comm_util.c
#${SAMPLE_COMMON_DIR}/loadbmp.c
#${SAMPLE_COMMON_DIR}/sample_comm_region.c
#${SAMPLE_COMMON_DIR}/sample_comm_algo.c
#${SAMPLE_OSD_DIR}/bitmap.c
#${SAMPLE_OSD_DIR}/osd_truetype.c
#${SAMPLE_OSD_DIR}/osd_timestamp.c
${SAMPLE_ZOOM_DIR}/sample_zoom.c
)

if(TARGET_CHIPSERIES STREQUAL "TX5368AX" OR TARGET_CHIPSERIES STREQUAL "TX5339AX")
	TARGET_LINK_LIBRARIES(${SAMPLE_APP_ZOOM} libalgbodydetect2.a)
elseif(TARGET_CHIPSERIES STREQUAL "TX5336EX")
	TARGET_LINK_LIBRARIES(${SAMPLE_APP_ZOOM} libalgbodydetect2.a)
else()
	TARGET_LINK_LIBRARIES(${SAMPLE_APP_ZOOM} libalgbodydetect.a)
endif()

if(TARGET_CHIPSERIES STREQUAL "TX5336EX")
	TARGET_LINK_LIBRARIES(${SAMPLE_APP_ZOOM} libalgfacedetect2.a)
	TARGET_LINK_LIBRARIES(${SAMPLE_APP_ZOOM} libalgmotiondetect.a)
	TARGET_LINK_LIBRARIES(${SAMPLE_APP_ZOOM} libalgpcppdetect.a)
	TARGET_LINK_LIBRARIES(${SAMPLE_APP_ZOOM} -Wl,--start-group dl pthread stdc++
	libalgcommon.a
	libalgutil_cv.a
	#libplat_rne_linux_a53.a libchip_sdk.a
	librne_rt_g3.a
	librne_pal_linux.a
	librne_common.a
	libtrp.a
	${MPI_BASE_LIB}
	-Wl,--end-group)
else()
	TARGET_LINK_LIBRARIES(${SAMPLE_APP_ZOOM} libalgfacedetect2.a)
	TARGET_LINK_LIBRARIES(${SAMPLE_APP_ZOOM} libalgmotiondetect.a)
	TARGET_LINK_LIBRARIES(${SAMPLE_APP_ZOOM} libalgpcppdetect.a)
	TARGET_LINK_LIBRARIES(${SAMPLE_APP_ZOOM} -Wl,--start-group dl pthread stdc++
	libalgcommon.a
	libalgutil_cv.a
	#libplat_rne_linux_a53.a libchip_sdk.a
	librne_rt_g3.a
	librne_pal_linux.a
	librne_common.a
	libtrp.a
		${MPI_BASE_LIB}
	-Wl,--end-group)
endif()


#if(TARGET_CHIP STREQUAL "tx536")
#TARGET_LINK_LIBRARIES(${SAMPLE_APP_VENC} -Wl,--start-group dl pthread stdc++  librtsp_server.a
#libBasicUsageEnvironment.a libgroupsock.a libliveMedia.a
#libUsageEnvironment.a  librtsp_server_gb.a  -Wl,--end-group)
#endif()
#if(TARGET_CHIP STREQUAL "dt56")
#TARGET_LINK_LIBRARIES(${SAMPLE_APP_VENC} -Wl,--start-group dl pthread stdc++ librtsp_server_gb_32bit.a  -Wl,--end-group)
#endif()


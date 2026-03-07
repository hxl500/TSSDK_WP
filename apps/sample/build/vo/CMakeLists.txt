set (SAMPLE_COMMON_DIR         ${APP_SRC_DIR}/common)
set (SAMPLE_VO_DIR            ${APP_SRC_DIR}/vo)
#set (MPP_ROOT_DIR		   	   ${TARGET_LIBS_DIR}/mpp)
set (SAMPLE_OSD_DIR            ${APP_SRC_DIR}/common/osd)
#################################### Include Directories #######################################
include_directories(${MPP_ROOT_DIR})
include_directories(${SAMPLE_COMMON_DIR})
include_directories(${APP_SRC_DIR}/../include)
include_directories(${APP_SRC_DIR}/../../libs/mpp/include)
include_directories(${SAMPLE_OSD_DIR})



################################################ clock_app_test ###################################################
if("${MPP_USE_PCSIMULATOR}" STREQUAL "0")
	add_definitions(-DSENSOR0_TYPE=EVB_SENSOR)
	add_definitions(-DHIDE_PRINT_0919)
	add_definitions(-DSENSOR1_TYPE=SONY_IMX327_MIPI_2M_30FPS_12BIT)
	add_definitions(-DHIDE_PRINT_0919)
	set(SAMPLE_APP_VO sample_vo)
else()
	add_definitions(-DSENSOR0_TYPE=X86_LOGITECH_HD720)
	add_definitions(-DSENSOR1_TYPE=SONY_IMX327_MIPI_2M_30FPS_12BIT)
	set(SAMPLE_APP_VIO sample_vio_x86)
endif()

add_executable(
    ${SAMPLE_APP_VO}
${SAMPLE_OSD_DIR}/bitmap.c
${SAMPLE_OSD_DIR}/osd_truetype.c
${SAMPLE_OSD_DIR}/osd_timestamp.c
${SAMPLE_COMMON_DIR}/sample_comm_isp.c
${SAMPLE_COMMON_DIR}/sample_comm_sys.c
${SAMPLE_COMMON_DIR}/sample_comm_vi.c
${SAMPLE_COMMON_DIR}/sample_comm_vo.c
${SAMPLE_COMMON_DIR}/sample_comm_venc.c
${SAMPLE_COMMON_DIR}/sample_comm_vdec.c
${SAMPLE_COMMON_DIR}/sample_comm_vpss.c
${SAMPLE_COMMON_DIR}/loadbmp.c
${SAMPLE_COMMON_DIR}/sample_comm_region.c
${SAMPLE_VO_DIR}/sample_vo.c
)
TARGET_LINK_LIBRARIES(${SAMPLE_APP_VO} dl pthread)

TARGET_LINK_LIBRARIES(${SAMPLE_APP_VO} -Wl,--start-group dl pthread stdc++ ${MPI_BASE_LIB} -Wl,--end-group)
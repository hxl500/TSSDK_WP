set (SAMPLE_COMMON_DIR         ${APP_SRC_DIR}/common)
set (SAMPLE_VO_GUI_DIR         ${APP_SRC_DIR}/vo_gui)
#set (MPP_ROOT_DIR		   	   ${TARGET_LIBS_DIR}/mpp)
set (SAMPLE_OSD_DIR            ${APP_SRC_DIR}/common/osd)

#################################### Include Directories #######################################
include_directories(${MPP_ROOT_DIR})
include_directories(${SAMPLE_COMMON_DIR})
include_directories(${APP_SRC_DIR}/../include)
include_directories(${APP_SRC_DIR}/../../libs/mpp/include)
include_directories(${SAMPLE_OSD_DIR})
include_directories(${SAMPLE_VO_GUI_DIR}/include)

link_directories($ENV{TARGET_STAGING_LIB})
link_directories(${SAMPLE_VO_GUI_DIR}/libs)

################################################ clock_app_test ###################################################
if("${MPP_USE_PCSIMULATOR}" STREQUAL "0")
	add_definitions(-DSENSOR0_TYPE=EVB_SENSOR)
	add_definitions(-DHIDE_PRINT_0919)
	add_definitions(-DSENSOR1_TYPE=SONY_IMX327_MIPI_2M_30FPS_12BIT)
	add_definitions(-DHIDE_PRINT_0919)
	set(SAMPLE_APP_VO_GUI sample_vo_gui)
else()
	add_definitions(-DSENSOR0_TYPE=X86_LOGITECH_HD720)
	add_definitions(-DSENSOR1_TYPE=SONY_IMX327_MIPI_2M_30FPS_12BIT)
	set(SAMPLE_APP_VO_GUI sample_vo_gui_x86)
endif()

add_executable(
    ${SAMPLE_APP_VO_GUI}
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
	${SAMPLE_VO_GUI_DIR}/sample_vo_gui.c
)

if (TARGET_CHIPSERIES STREQUAL TX5336EX)
	TARGET_LINK_LIBRARIES(${SAMPLE_APP_VO_GUI} -Wl,--start-group dl pthread stdc++ ${MPI_BASE_LIB} lvgl_tx5336 -Wl,--end-group)
elseif (TARGET_CHIPSERIES STREQUAL TX5215BX)
	TARGET_LINK_LIBRARIES(${SAMPLE_APP_VO_GUI} -Wl,--start-group dl pthread stdc++ ${MPI_BASE_LIB} lvgl_tx5215 -Wl,--end-group)
endif()
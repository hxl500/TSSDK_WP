set (SAMPLE_COMMON_DIR         ${APP_SRC_DIR}/common)
set (SAMPLE_SOFT_VDEC_DIR      ${APP_SRC_DIR}/soft_vdec_lcd)
#set (MPP_ROOT_DIR		   	   ${TARGET_LIBS_DIR}/mpp)
#################################### Include Directories #######################################
include_directories(${MPP_ROOT_DIR})
include_directories(${SAMPLE_COMMON_DIR})
include_directories(${APP_SRC_DIR}/../include)
include_directories($ENV{TARGET_LIBS_DIR}/mpp/include)
include_directories($ENV{TARGET_LIBS_DIR}/ffmpeg/include)
include_directories($ENV{TARGET_LIBS_DIR}/drv/lcd/gc9108/driver)

################################################ clock_app_test ###################################################
set(SAMPLE_APP_SOFT_VDEC_LCD sample_soft_vdec_lcd)
link_directories(
	$ENV{TARGET_LIBS_DIR}/ffmpeg/lib/
	)

if(TARGET_CHIPSERIES STREQUAL "TX5215BX")
    set (SAMPLE_COMMON_LCD_C      ${SAMPLE_COMMON_DIR}/sample_comm_lcd.c)
    set (SAMPLE_FFMPEG_LIB        swscale avcodec avutil)
else()
    set (SAMPLE_COMMON_LCD_C    "")
    set (SAMPLE_FFMPEG_LIB      "")
endif()

add_executable(
    ${SAMPLE_APP_SOFT_VDEC_LCD}
    ${SAMPLE_COMMON_DIR}/sample_comm_sys.c
    ${SAMPLE_COMMON_LCD_C}
    ${SAMPLE_SOFT_VDEC_DIR}/sample_soft_vdec_lcd.c
)

TARGET_LINK_LIBRARIES(${SAMPLE_APP_SOFT_VDEC_LCD} -Wl,--start-group dl pthread stdc++ 
${SAMPLE_FFMPEG_LIB} ${MPI_BASE_LIB}
-Wl,--end-group)

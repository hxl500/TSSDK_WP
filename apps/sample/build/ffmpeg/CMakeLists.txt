project(ffmpeg)

# 设置 FFmpeg 和其他库的根目录
set(DIR_FFMPEG $ENV{TARGET_LIBS_DIR}/ffmpeg)

# 添加头文件路径
include_directories(
	${DIR_FFMPEG}/include
	$ENV{TARGET_LIBS_DIR}/mpp/build/out/inc
)

# 添加库文件路径
link_directories($ENV{TARGET_STAGING_LIB})

# 算法相关
include_directories($ENV{TARGET_STAGING_INC}/opencv4)
link_directories($ENV{TARGET_STAGING_LIB}/opencv4/3rdparty)

include_directories(${APP_SRC_DIR}/tsalg/alg_bodydetect/include)
include_directories(${APP_SRC_DIR}/tsalg/alg_bodydetect/model)




set(FF_LIBS avformat avutil avcodec avfilter swresample swscale)
set(FF_LINK_LIBRARIES)

foreach(LIB ${FF_LIBS})
    message(STATUS "########try to Find ${LIB}")
    # 使用唯一变量名（FF_avformat_LIBRARY等）
    find_library(FF_${LIB}_LIBRARY
        NAMES ${LIB}
        PATHS $ENV{TARGET_STAGING_LIB}
        NO_DEFAULT_PATH  # 只在指定路径查找
    )
    if(FF_${LIB}_LIBRARY)
        message(STATUS "Found ${FF_${LIB}_LIBRARY}")
        # 添加实际库路径到链接列表
        list(APPEND FF_LINK_LIBRARIES ${LIB})
    endif()
endforeach()

# 输出链接库列表供调试
message(STATUS "FF_LINK_LIBRARIES: ${FF_LINK_LIBRARIES}")


######################### ffmpeg_sample_vdec #########################

# if(TARGET_CHIPSERIES STREQUAL "TX5368AX" OR TARGET_CHIPSERIES STREQUAL "TX5339AX")

	set(SAMPLE_FFMPEG_TSMPP_VDEC  sample_ffmpeg_tsmpp_vdec)
	set (SAMPLE_TSALG_DIR ${APP_SRC_DIR}/tsalg)

	add_executable(${SAMPLE_FFMPEG_TSMPP_VDEC}
		# ${APP_SRC_DIR}/ffmpeg/sample_ffmpeg_tsmpp_vdec.c
		${SAMPLE_TSALG_DIR}/alg_bodydetect/src/process.c
		${SAMPLE_TSALG_DIR}/alg_bodydetect/src/ts_alg_body_detect.c
		${APP_SRC_DIR}/ffmpeg/sample_ffmpeg_vdec.cpp
	)

	TARGET_LINK_LIBRARIES( ${SAMPLE_FFMPEG_TSMPP_VDEC}
		-Wl,--start-group
		dl
		pthread
		#算法相关库 - 开始
		tscv
		cve
		opencv_world
		zlib
		libopenjp2
		libjpeg-turbo
		libpng
		libwebp
		quirc
		tegra_hal
		ade
		algcommon
		rne_pal_linux
		rne_rt_g3
		rne_common
		trp
		#算法相关库 - 结束
		${FF_LINK_LIBRARIES}
		${MPI_BASE_LIB}
		-Wl,--end-group
	)

# endif()

######################### ffmpeg_sample_venc #########################

# set(SAMPLE_FFMPEG_TSMPP_VENC  sample_ffmpeg_tsmpp_venc)

# add_executable(${SAMPLE_FFMPEG_TSMPP_VENC}
# ${DIR_FFMPEG}/sample_ffmpeg_tsmpp_venc.c
# )

# TARGET_LINK_LIBRARIES( ${SAMPLE_FFMPEG_TSMPP_VENC}
# -Wl,--start-group
# dl
# pthread
# avformat avutil avcodec avfilter avutil swresample swscale
# -Wl,--end-group
# )

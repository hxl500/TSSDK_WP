# set(SAMPLE_LIB_CATDETECT cat_detect_lib)
# set (SAMPLE_CATDETECT_DIR ${APP_SRC_DIR}/cat_detect_lib)

# set(START_GRP "-Wl,--start-group")
# set(END_GRP "-Wl,--end-group")

# message("SOLIBPATH = $ENV{TARGET_STAGING_LIB}")
# link_directories($ENV{TARGET_STAGING_LIB})

# include_directories($ENV{TARGET_STAGING_INC}/opencv4)
# link_directories($ENV{TARGET_STAGING_LIB}/opencv4/3rdparty)

# #include_directories($ENV{TARGET_STAGING_INC}/opencv4)
# #link_directories($ENV{TARGET_STAGING_LIB}/opencv4/3rdparty)
# #include_directories(${APP_SRC_DIR}/tsalg/alg_bodydetect/include)
# #include_directories(${APP_SRC_DIR}/tsalg/alg_bodydetect/model)
# #include_directories(${APP_SRC_DIR}/../../libs/algstore/libalgbodydetect/include)
# #include_directories(${APP_SRC_DIR}/../../libs/algstore/libalgcommon/include)
# #include_directories(${APP_SRC_DIR}/tsalg_lib)
# #link_directories(${APP_SRC_DIR}/temp/tsalg_lib)

# add_library(${SAMPLE_LIB_CATDETECT} STATIC
# 	${SAMPLE_CATDETECT_DIR}/lib_ts_alg_body_detect.cpp
# 	${SAMPLE_CATDETECT_DIR}/lib_video_alg_catdetect.c 
# 	${SAMPLE_CATDETECT_DIR}/lib_tsalg_alg_lib.c
# 	${SAMPLE_CATDETECT_DIR}/lib_tsalg_alg_rsn.c
# 	${SAMPLE_CATDETECT_DIR}/lib_food_rsn.c
# 	${SAMPLE_CATDETECT_DIR}/lib_array_diff.c
# 	${SAMPLE_CATDETECT_DIR}/lib_act.c
# 	${SAMPLE_CATDETECT_DIR}/lib_dirsync.c
# 	${SAMPLE_CATDETECT_DIR}/lib_file_sync.c
# 	${SAMPLE_CATDETECT_DIR}/lib_iot_net.c
# )

# TARGET_LINK_LIBRARIES(${SAMPLE_LIB_CATDETECT} libalgcommon.a)
# TARGET_LINK_LIBRARIES(${SAMPLE_LIB_CATDETECT} libalgfiresmokedetect.a)
# TARGET_LINK_LIBRARIES(${SAMPLE_LIB_CATDETECT} -Wl,--start-group dl pthread stdc++  libalgcommon.a
# 		mpi
#  -Wl,--end-group)

#  set_target_properties(cat_detect_lib PROPERTIES
#     ARCHIVE_OUTPUT_DIRECTORY "${APP_SRC_DIR}/../../libs/algstore/libalgmotiondetect/lib/uclibc"
# )

set(SAMPLE_LIB_CATDETECT cat_detect_lib)
set (SAMPLE_CATDETECT_DIR ${APP_SRC_DIR}/cat_detect_lib)

set(START_GRP "-Wl,--start-group")
set(END_GRP "-Wl,--end-group")
if(TARGET_CHIPSERIES STREQUAL TX5336EX)
set(OPENCV_LIB
	opencv_world
	zlib
	libopenjp2
	libjpeg-turbo
	libpng
	libwebp
	quirc
	tegra_hal
	ade
    )
elseif(TARGET_CHIPSERIES STREQUAL TX5215BX)
set(OPENCV_LIB
	opencv_world
	zlib
	libopenjp2
	libjpeg-turbo
	libpng
	libwebp
	quirc
	ade
	)
else()
	message("$ENV{TARGET_CHIPSERIES} not support tscv")	
endif()
message("SOLIBPATH = $ENV{TARGET_STAGING_LIB}")
link_directories($ENV{TARGET_STAGING_LIB})

include_directories($ENV{TARGET_STAGING_INC}/opencv4)
link_directories($ENV{TARGET_STAGING_LIB}/opencv4/3rdparty)
 
# include_directories(${APP_SRC_DIR}/../../libs/algstore/libalgutil_cv/include)
# include_directories(${APP_SRC_DIR}/../../libs/algstore/libalgutil_cv/lib/uclibc)

# include_directories(${APP_SRC_DIR}/../../libs/yuv/include)
# include_directories(${APP_SRC_DIR}/../../libs/yuv/lib)

 
# include_directories(${APP_SRC_DIR}/../../libs/trp/rne/include)
# include_directories(${APP_SRC_DIR}/../../trp/rne/lib/uclibc)

# include_directories(${SAMPLE_CATDETECT_DIR})
# include_directories(${SAMPLE_CATDETECT_DIR}/record_file)
include_directories(${SAMPLE_CATDETECT_DIR}/include)
include_directories(${SAMPLE_CATDETECT_DIR}/include/api)
include_directories(${SAMPLE_CATDETECT_DIR}/include/core)
include_directories(${SAMPLE_CATDETECT_DIR}/include/utils)
include_directories(${SAMPLE_CATDETECT_DIR}/src/record_file)
include_directories(${APP_SRC_DIR}/../../libs/mpp/include)
# include_directories(${APP_SRC_DIR}/../../libs/trp/cve/include)
# include_directories($ENV{TARGET_STAGING_INC})
# include_directories($ENV{TARGET_STAGING_INC}/opencv4)
# link_directories($ENV{TARGET_STAGING_LIB}/opencv4/3rdparty)
# link_directories($ENV{TARGET_STAGING_LIB})
include_directories(${APP_SRC_DIR}/../../libs/RNE-RT-Lib/include)
include_directories(${APP_SRC_DIR}/../../libs/RNE-RT-Lib/libs/32bit)
link_directories(${APP_SRC_DIR}/../../libs/trp/rne/lib/uclibc)

include_directories(${APP_SRC_DIR}/../../libs/algstore/opencv4.5.5/include/opencv4)
include_directories(${APP_SRC_DIR}/../../libs/algstore/opencv4.5.5/include/opencv4/opencv2)
link_directories(${APP_SRC_DIR}/../../libs/algstore/opencv4.5.5/lib/uclibc/opencv4)
link_directories(${APP_SRC_DIR}/../../libs/algstore/opencv4.5.5/lib/uclibc/opencv4/3rdparty)

include_directories(${APP_SRC_DIR}/../../libs/yuv/include)
include_directories(${APP_SRC_DIR}/../../libs/yuv/lib)
link_directories(${APP_SRC_DIR}/../../libs/yuv/lib)
# include_directories(${APP_SRC_DIR}/../../libs/tscv/include)
# include_directories(${APP_SRC_DIR}/../../libs/tscv/include/opencv4)
# include_directories(${APP_SRC_DIR}/../../libs/tscv/include/opencv4/opencv2)
# link_directories(${APP_SRC_DIR}/../../libs/tscv/lib/uclibc)
#link_directories(${APP_SRC_DIR}/../../libs/tscv/lib/uclibc/opencv4)
#link_directories(${APP_SRC_DIR}/../../libs/tscv/lib/uclibc/opencv4/3rdparty)

# include_directories(${APP_SRC_DIR}/../../libs/trp/cve/lib/uclibc)
# link_directories(${APP_SRC_DIR}/../../libs/trp/cve/lib/uclibc)
#include_directories($ENV{TARGET_STAGING_INC}/opencv4)
#link_directories($ENV{TARGET_STAGING_LIB}/opencv4/3rdparty)
#include_directories(${APP_SRC_DIR}/tsalg/alg_bodydetect/include)
#include_directories(${APP_SRC_DIR}/tsalg/alg_bodydetect/model)
#include_directories(${APP_SRC_DIR}/../../libs/algstore/libalgbodydetect/include)
#include_directories(${APP_SRC_DIR}/../../libs/algstore/libalgcommon/include)
#include_directories(${APP_SRC_DIR}/tsalg_lib)
#link_directories(${APP_SRC_DIR}/temp/tsalg_lib)
link_directories(${APP_SRC_DIR}/../../libs/yuv/lib)

# ===== 调试必加：支持coredump、交叉gdb定位源码行号 =====
add_library(${SAMPLE_LIB_CATDETECT} STATIC
	${SAMPLE_CATDETECT_DIR}/src/core/lib_ts_alg_body_detect.cpp
	${SAMPLE_CATDETECT_DIR}/src/core/lib_video_alg_catdetect.c 
	${SAMPLE_CATDETECT_DIR}/src/core/lib_tsalg_alg_lib.c
	${SAMPLE_CATDETECT_DIR}/src/core/lib_tsalg_alg_rsn.c
	${SAMPLE_CATDETECT_DIR}/src/core/lib_food_rsn.c
	${SAMPLE_CATDETECT_DIR}/src/utils/lib_array_diff.c
	${SAMPLE_CATDETECT_DIR}/src/utils/lib_act.c
	${SAMPLE_CATDETECT_DIR}/src/utils/lib_dirsync.c
	${SAMPLE_CATDETECT_DIR}/src/utils/lib_file_sync.c
	${SAMPLE_CATDETECT_DIR}/src/utils/lib_iot_net.c
	${SAMPLE_CATDETECT_DIR}/src/utils/qi_nv12_yuv.cpp
	${SAMPLE_CATDETECT_DIR}/src/record_file/ts_rne_record_file.c
)
target_compile_options(${SAMPLE_LIB_CATDETECT} PRIVATE
    -g
    -O0
    -Wall
    -funwind-tables
)
target_link_options(${SAMPLE_LIB_CATDETECT} PRIVATE
    -rdynamic
)
# ======================================================

	
	
	
	
# set(SAMPLE_TSCV_DEPS
# 		tscv
# 		cve
# 		${OPENCV_LIB}
# 		#mpi
# 		${MPI_BASE_LIB}
# 		dl
# 		pthread
# 	)	

# target_link_libraries(${SAMPLE_LIB_CATDETECT} ${START_GRP} ${SAMPLE_TSCV_DEPS} ${END_GRP})
target_link_libraries(${SAMPLE_LIB_CATDETECT} libalgcommon.a)
#TARGET_LINK_LIBRARIES(${SAMPLE_LIB_CATDETECT} libalgfiresmokedetect.a)
target_link_libraries(${SAMPLE_LIB_CATDETECT} -Wl,--start-group dl pthread stdc++  libalgcommon.a
		
		# -Wl,--whole-archive ${APP_SRC_DIR}/../../libs/tscv/lib/uclibc/libopencv_world.a -Wl,--no-whole-archive
		# -Wl,--whole-archive ${APP_SRC_DIR}/../../libs/tscv/lib/uclibc/opencv4/3rdparty/libzlib.a -Wl,--no-whole-archive
		# -Wl,--whole-archive ${APP_SRC_DIR}/../../libs/tscv/lib/uclibc/opencv4/3rdparty/liblibopenjp2.a -Wl,--no-whole-archive
		# -Wl,--whole-archive ${APP_SRC_DIR}/../../libs/tscv/lib/uclibc/opencv4/3rdparty/liblibjpeg-turbo.a -Wl,--no-whole-archive
		# -Wl,--whole-archive ${APP_SRC_DIR}/../../libs/tscv/lib/uclibc/opencv4/3rdparty/liblibpng.a -Wl,--no-whole-archive
		# -Wl,--whole-archive ${APP_SRC_DIR}/../../libs/tscv/lib/uclibc/opencv4/3rdparty/liblibwebp.a -Wl,--no-whole-archive
		# -Wl,--whole-archive ${APP_SRC_DIR}/../../libs/tscv/lib/uclibc/opencv4/3rdparty/libquirc.a -Wl,--no-whole-archive
		# -Wl,--whole-archive ${APP_SRC_DIR}/../../libs/tscv/lib/uclibc/opencv4/3rdparty/libade.a -Wl,--no-whole-archive
		# -Wl,--whole-archive ${APP_SRC_DIR}/../../libs/tscv/lib/uclibc/libopencv_img_hash.a -Wl,--no-whole-archive
		# -Wl,--whole-archive ${APP_SRC_DIR}/../../libs/tscv/lib/uclibc/libtscv.a -Wl,--no-whole-archive
		mpi
		#${APP_SRC_DIR}/../../libs/yuv/lib/libyuv.a
		libalgutil_cv.a
		librne_rt_g3.a
	librne_pal_linux.a
	librne_common.a
	librne_pal_linux_a7.a
	${ALGO_LIB}
		libtrp.a
		# dl
		# pthread
 -Wl,--end-group)

#set_target_properties(cat_detect_lib PROPERTIES
#    ARCHIVE_OUTPUT_DIRECTORY "${APP_SRC_DIR}/tsalg_video"
#)
set_target_properties(cat_detect_lib PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${APP_SRC_DIR}/../../libs/algstore/libalgmotiondetect/lib/uclibc"
)

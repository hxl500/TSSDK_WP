set(SAMPLE_APP_TSALG sample_tsalg)
set (SAMPLE_TSALG_DIR ${APP_SRC_DIR}/tsalg)

set(START_GRP "-Wl,--start-group")
set(END_GRP "-Wl,--end-group")
link_directories($ENV{TARGET_STAGING_LIB})

include_directories($ENV{TARGET_STAGING_INC}/opencv4)
link_directories($ENV{TARGET_STAGING_LIB}/opencv4/3rdparty)

include_directories(${APP_SRC_DIR}/tsalg/alg_bodydetect/include)
include_directories(${APP_SRC_DIR}/tsalg/alg_bodydetect/model)

add_executable(
	${SAMPLE_APP_TSALG}_alg
	${SAMPLE_TSALG_DIR}/alg_bodydetect/src/process.c
	${SAMPLE_TSALG_DIR}/alg_bodydetect/src/ts_alg_body_detect.c
	${SAMPLE_TSALG_DIR}/alg_colorconvert/ts_alg_color_convert.c
	${SAMPLE_TSALG_DIR}/sample_tsalg_alg.cpp
)

if(TARGET_CHIPSERIES STREQUAL TX5339AX OR TARGET_CHIPSERIES STREQUAL TX5368AX)

	set(SAMPLE_TSALG_DEPS
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
		dl
		pthread
	)

	target_link_libraries(${SAMPLE_APP_TSALG}_alg ${START_GRP}
		${SAMPLE_TSALG_DEPS}
		tscv
		linux_a53rce_pal
		linux_a53rce_rt
		${MPI_BASE_LIB}
		${END_GRP}
	)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=armv8-a -mcpu=cortex-a53")

elseif(TARGET_CHIPSERIES STREQUAL TX5336EX)
	set(SAMPLE_TSALG_DEPS
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
	dl
	pthread
	)

target_link_libraries(${SAMPLE_APP_TSALG}_alg ${START_GRP}
			${SAMPLE_TSALG_DEPS}
			${MPI_BASE_LIB}
			tscv
			${END_GRP})

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=armv8-a -mcpu=cortex-a53")

elseif(TARGET_CHIPSERIES STREQUAL TX5215BX)	
	set(SAMPLE_TSALG_DEPS
		cve
		opencv_world
		zlib
		libopenjp2
		libjpeg-turbo
		libpng
		libwebp
		quirc
		ade
		algcommon
		rne_pal_linux
		rne_rt_g3
		rne_common
		dl
		pthread
	)

	target_link_libraries(${SAMPLE_APP_TSALG}_alg ${START_GRP}
				${SAMPLE_TSALG_DEPS}
				${MPI_BASE_LIB}
				${END_GRP}
	)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=armv7-a -mfpu=neon")
elseif(TARGET_CHIPSERIES STREQUAL TX5112DX OR TARGET_CHIPSERIES STREQUAL TX5239DX)

	set(SAMPLE_TSALG_DEPS
		cve
		opencv_world
		zlib
		libopenjp2
		libjpeg-turbo
		libpng
		libwebp
		quirc
		#		libtegra_hal.a
		ade
		algcommon
		rne_pal_linux
		rne_rt_g3
		rne_common
		${MPI_BASE_LIB}
		dl
		pthread
	)

	target_link_libraries(${SAMPLE_APP_TSALG}_alg ${START_GRP} 
		cve
		algcommon
		rne_common
		rne_pal_linux
		rne_rt_g3
		${MPI_BASE_LIB}
		rt
		${END_GRP}
	)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=armv7-a -mfpu=neon")
else()
	message("$ENV{TARGET_CHIPSERIES} not support tsalg")
endif()
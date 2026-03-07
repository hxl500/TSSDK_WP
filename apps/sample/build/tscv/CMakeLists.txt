
set(SAMPLE_APP_TSCV sample_tscv)
set (SAMPLE_TSCV_DIR ${APP_SRC_DIR}/tscv)

set(START_GRP "-Wl,--start-group")
set(END_GRP "-Wl,--end-group")

# set(SAMPLE_TSCV_DEPS
# 	libtscv.a
# 	libcve.a
# 	libopencv_world.a
# 	libzlib.a
# 	liblibopenjp2.a
# 	liblibjpeg-turbo.a
# 	liblibpng.a
# 	liblibwebp.a
#     libquirc.a
# 	libtegra_hal.a
#     libade.a
# 	mpi
# 	dl
# 	pthread
# )
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

include_directories($ENV{TARGET_STAGING_INC})
include_directories($ENV{TARGET_STAGING_INC}/opencv4)
link_directories($ENV{TARGET_STAGING_LIB}/opencv4/3rdparty)
link_directories($ENV{TARGET_STAGING_LIB})

if(TARGET_CHIPSERIES STREQUAL TX5335AX OR TARGET_CHIPSERIES STREQUAL TX5368AX OR TARGET_CHIPSERIES STREQUAL TX5339AX)
	set(SAMPLE_TSCV_DEPS
		tscv
		cve
		opencv_world
		${OPENCV_LIB}
		mpi
		dl
		pthread
	)
	#build sample tscv rce###
	add_executable(
	${SAMPLE_APP_TSCV}_rce
	${SAMPLE_TSCV_DIR}/sample_tscv_rce.cpp
	)
	target_link_libraries(${SAMPLE_APP_TSCV}_rce ${START_GRP} ${SAMPLE_TSCV_DEPS} liblinux_a53rce_pal.a liblinux_a53rce_rt.a ${END_GRP})

	##build sample tscv gpu###
	add_executable(
	${SAMPLE_APP_TSCV}_vgs
	${SAMPLE_TSCV_DIR}/sample_tscv_vgs.cpp
	)
	target_link_libraries(${SAMPLE_APP_TSCV}_vgs ${START_GRP}  ${SAMPLE_TSCV_DEPS} ${END_GRP})

	##build sample tscv cve###
	add_executable(
		${SAMPLE_APP_TSCV}_cve
		${SAMPLE_TSCV_DIR}/sample_tscv_cve.cpp
	)
	target_link_libraries(${SAMPLE_APP_TSCV}_cve ${START_GRP} ${SAMPLE_TSCV_DEPS} ${END_GRP})

	##build sample tscv vpss###
	add_executable(
		${SAMPLE_APP_TSCV}_vpss
		${SAMPLE_TSCV_DIR}/sample_tscv_vpss.cpp
	)
	target_link_libraries(${SAMPLE_APP_TSCV}_vpss ${START_GRP} ${SAMPLE_TSCV_DEPS} ${END_GRP})

elseif(TARGET_CHIPSERIES STREQUAL TX5215BX OR TARGET_CHIPSERIES STREQUAL TX5239CX)
	set(SAMPLE_TSCV_DEPS
		tscv
		cve
		${OPENCV_LIB}
		#mpi
		${MPI_BASE_LIB}
		dl
		pthread
	)
	##build sample tscv cve###
	add_executable(
		${SAMPLE_APP_TSCV}_cve
		${SAMPLE_TSCV_DIR}/sample_tscv_cve.cpp
	)
	target_link_libraries(${SAMPLE_APP_TSCV}_cve ${START_GRP} ${SAMPLE_TSCV_DEPS} ${END_GRP})

	##build sample tscv vpss###
	add_executable(
		${SAMPLE_APP_TSCV}_vpss
		${SAMPLE_TSCV_DIR}/sample_tscv_vpss.cpp
	)
	target_link_libraries(${SAMPLE_APP_TSCV}_vpss ${START_GRP} ${SAMPLE_TSCV_DEPS} ${END_GRP})
elseif(TARGET_CHIPSERIES STREQUAL TX5336EX)
	set(SAMPLE_TSCV_DEPS
		tscv
		cve
		stich
		${OPENCV_LIB}
		tegra_hal
		# mpi
		${MPI_BASE_LIB}
		dl
		# pthread
		rt
	)
	##build sample tscv cve###
	add_executable(
		${SAMPLE_APP_TSCV}_cve
		${SAMPLE_TSCV_DIR}/sample_tscv_cve.cpp
	)
	target_link_libraries(${SAMPLE_APP_TSCV}_cve ${START_GRP} ${SAMPLE_TSCV_DEPS} ${END_GRP})

	##build sample tscv vpss###
	add_executable(
		${SAMPLE_APP_TSCV}_vpss
		${SAMPLE_TSCV_DIR}/sample_tscv_vpss.cpp
	)
	target_link_libraries(${SAMPLE_APP_TSCV}_vpss ${START_GRP} ${SAMPLE_TSCV_DEPS} ${END_GRP})
else()
	message("$ENV{TARGET_CHIPSERIES} not support tscv")
endif()
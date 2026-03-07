
set(SAMPLE_APP_CVE sample_cve)
set (SAMPLE_CVE_DIR ${APP_SRC_DIR}/trp/cve)
if(TARGET_CHIPSERIES STREQUAL TX5336EX)
	add_compile_definitions(DX5336)
	# add_compile_definitions(DX5112)
	add_compile_definitions(DX5368)
endif()

set(START_GRP "-Wl,--start-group")
set(END_GRP "-Wl,--end-group")

link_directories($ENV{TARGET_STAGING_LIB}/opencv4/3rdparty)
link_directories($ENV{TARGET_STAGING_LIB})

add_executable(
	${SAMPLE_APP_CVE}
	${SAMPLE_CVE_DIR}/sample_cve.c
)

if(TARGET_CHIPSERIES STREQUAL TX5336EX)
    target_link_libraries(${SAMPLE_APP_CVE} ${START_GRP}
		libcve.a
		stich
		opencv_world
		opencv_img_hash
		ade
		zlib
		libopenjp2
		libjpeg-turbo
		libpng
		libwebp
		quirc
		tegra_hal
		${MPI_BASE_LIB}
		dl
		rt
	${END_GRP})

else()
	target_link_libraries(${SAMPLE_APP_CVE} ${START_GRP} libcve.a  rt ${MPI_BASE_LIB} ${END_GRP})
endif()
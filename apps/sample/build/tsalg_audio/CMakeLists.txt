MESSAGE(STATUS ">>>>>>>> Build tsalg_audio ")

set (SAMPLE_COMMON_DIR         ${APP_SRC_DIR}/common)
set (SAMPLE_AUDIO_DIR          ${APP_SRC_DIR}/tsalg_audio)

#################################### Include Directories #######################################
include_directories(${SAMPLE_COMMON_DIR})
include_directories(${SAMPLE_AUDIO_DIR})
include_directories(${TARGET_STAGING_INC})
link_directories(${TARGET_STAGING_LIB})
################################################ 
if("${MPP_USE_PCSIMULATOR}" STREQUAL "0")
	add_definitions(-DSENSOR0_TYPE=EVB_SENSOR_4K)
	add_definitions(-DSENSOR1_TYPE=SONY_IMX327_MIPI_2M_30FPS_12BIT)
	add_definitions(-DVENC_IN_ZERO_COPY_V2)
	add_definitions(-DHIDE_UNVERIFIED_CASE)
	add_definitions(-DHIDE_PRINT_0919)
	set(SAMPLE_APP_AUDIO sample_alg_audio)
else()
	add_definitions(-DSENSOR0_TYPE=X86_LOGITECH_HD1080)
	add_definitions(-DSENSOR1_TYPE=SONY_IMX327_MIPI_2M_30FPS_12BIT)
	set(SAMPLE_APP_AUDIO sample_alg_audio_x86)
endif()

add_definitions(-DALG_ICD=1)

file(GLOB SAMPLE_AUDIO_ALG_SRC ${SAMPLE_AUDIO_DIR}/*.c) 
add_executable(${SAMPLE_APP_AUDIO}
	${SAMPLE_COMMON_DIR}/sample_comm_sys.c
	${SAMPLE_COMMON_DIR}/sample_comm_audio.c
	${SAMPLE_AUDIO_ALG_SRC}
)

set(AUDIO_DEPS_LIBS algopus algaacenc algaacdec algg726 algadpcm algg711 algicd ) #crydetect

TARGET_LINK_LIBRARIES(${SAMPLE_APP_AUDIO}
-Wl,--start-group dl pthread stdc++
algcommon
algutil_cv
trp
${AUDIO_DEPS_LIBS}
${MPI_BASE_LIB}
rne_rt_g3
rne_pal_linux
rne_common
avformat
avcodec
avutil
-Wl,--end-group)

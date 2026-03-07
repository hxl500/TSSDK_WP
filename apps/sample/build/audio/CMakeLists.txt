set (SAMPLE_COMMON_DIR         ${APP_SRC_DIR}/common)
set (SAMPLE_AUDIO_DIR            ${APP_SRC_DIR}/audio)
#set (MPP_ROOT_DIR		   	   ${TARGET_LIBS_DIR}/mpp)
#################################### Include Directories #######################################
include_directories(${SAMPLE_COMMON_DIR})
include_directories(${SAMPLE_AUDIO_DIR})
include_directories(${TARGET_STAGING_INC})
link_directories(${TARGET_STAGING_LIB})
################################################ clock_app_test ###################################################
if("${MPP_USE_PCSIMULATOR}" STREQUAL "0")
	add_definitions(-DSENSOR0_TYPE=EVB_SENSOR_4K)
	add_definitions(-DSENSOR1_TYPE=SONY_IMX327_MIPI_2M_30FPS_12BIT)
	add_definitions(-DVENC_IN_ZERO_COPY_V2)
	add_definitions(-DHIDE_UNVERIFIED_CASE)
	add_definitions(-DHIDE_PRINT_0919)
	set(SAMPLE_APP_AUDIO sample_audio)
else()
	add_definitions(-DSENSOR0_TYPE=X86_LOGITECH_HD1080)
	add_definitions(-DSENSOR1_TYPE=SONY_IMX327_MIPI_2M_30FPS_12BIT)
	set(SAMPLE_APP_AUDIO sample_audio_x86)
endif()

add_executable(
	${SAMPLE_APP_AUDIO}
${SAMPLE_COMMON_DIR}/sample_comm_sys.c
${SAMPLE_COMMON_DIR}/sample_comm_audio.c
${SAMPLE_AUDIO_DIR}/sample_audio.c
)


if("${TARGET_CHIPSERIES}" STREQUAL "TX5339AX" OR "${TARGET_CHIPSERIES}" STREQUAL "TX5368AX" OR "${TARGET_CHIPSERIES}" STREQUAL "TX5336EX")
	option(USE_VQE_WEBRTC "use vqe webrtc type" ON)
	option(USE_VQE_WEBRTC_ARRCH64 "use vqe webrtc type" ON)
	option(USE_VQE_NN_MONO_8k "use vqe nn type" OFF)
	option(USE_VQE_NN_STEREO_8k "use vqe nn type" OFF)
	option(USE_VQE_NN_MONO_16k "use vqe nn type" OFF)
	option(USE_VQE_NN_STEREO_16k "use vqe nn type" OFF)
	option(USE_VQE_STEREO_MODEL "use vqe mvdr type" OFF) # OFF: dual mic use mvdr
else()
	option(USE_VQE_WEBRTC "use vqe webrtc type" ON)
	option(USE_VQE_STEREO_MODEL "use vqe stereo model type" OFF)
	option(USE_VQE_NN_MONO_16k "use vqe nn type" OFF)
	option(USE_VQE_NN_MONO_8k "use vqe nn type" OFF)
	option(USE_VQE_NN_STEREO_16k "use vqe nn type" OFF)
	if("${TARGET_CHIPSERIES}" STREQUAL "TX5112DX" OR "${TARGET_CHIPSERIES}" STREQUAL "TX5239DX")
		option(USE_VQE_WEBRTC_NN "use vqe webrtc type" OFF)
	endif()
endif()

if (USE_VQE_NN_MONO_8k OR USE_VQE_NN_STEREO_8k OR USE_VQE_NN_MONO_16k OR USE_VQE_NN_STEREO_16k)
	add_definitions(-DVQE_NN)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-error=unused-variable")

	if (USE_VQE_NN_MONO_8k)
		add_definitions(-DVQE_NN_MONO_8K)
	elseif (USE_VQE_NN_STEREO_8k)
		add_definitions(-DVQE_NN_STEREO_8K)
	elseif (USE_VQE_NN_MONO_16k)
		add_definitions(-DVQE_NN_MONO_16K)
	elseif (USE_VQE_NN_STEREO_16k)
		add_definitions(-DVQE_NN_STEREO_16K)
	endif ()

	if(USE_VQE_STEREO_MODEL)
		add_definitions(-DVQE_STEREO_MODEL)
	endif ()

	# include_directories(${APP_SRC_DIR}/../../libs/algstore/libsolvqe/include)
	# include_directories(${APP_SRC_DIR}/../../libs/algstore/libalganraec/include)
	# include_directories(${APP_SRC_DIR}/../../libs/algstore/libalganraec/model/tx5215)
	# include_directories(${APP_SRC_DIR}/../../libs/algstore/libalgwebrtc3a/include)
	# include_directories(${APP_SRC_DIR}/../../libs/trp/rne/TX5215x_TX5119x_TX5112x200_TX5239x200_TX5239x220-Lib/RNE-RT-Lib/include)
	# include_directories(${APP_SRC_DIR}/../../libs/trp/common/src)
	set (VQE_LIB
		libalganraec.a
		libalgwebrtc3a.a
		libalgsolvqenn.a
		librne_common.a
		librne_rt_g3.a
		librne_pal_linux.a
		libtrp.a
		libalgcommon.a
	)
elseif (USE_VQE_WEBRTC)
	add_definitions(-DVQE_WEBRTC)
	# include_directories(${APP_SRC_DIR}/../../libs/algstore/libalgwebrtc3a/include)
	# include_directories(${APP_SRC_DIR}/../../libs/algstore/libalgvqesolwebrtc3a/include)
	# include_directories(${APP_SRC_DIR}/../../libs/algstore/libalgNe10/include)
	
	if(USE_VQE_WEBRTC_ARRCH64)
		set (VQE_LIB
			libalgwebrtc3a.a
			libalgmmsens.a
			libalgomlsa.a
			libalgnoisegate.a
			libalgvqesolwebrtc3a.a
			libalgcommon.a
		)
	else()
		set (VQE_LIB
			libalgwebrtc3a.a
			libalgmmsens.a
			libalgomlsa.a
			libalgnoisegate.a
			libalgvqesolwebrtc3a.a
			libalgcommon.a
			libNE10_intrinsic.a
		)
		if("${TARGET_CHIPSERIES}" STREQUAL "TX5112DX" OR "${TARGET_CHIPSERIES}" STREQUAL "TX5239DX")
			set (VQE_LIB ${VQE_LIB}
				libalganrnn.a
				libalgutil_sp.a
				librne_common.a
				librne_pal_linux.a
				librne_rt_g3.a
				libtrp.a
			)
		endif()
	endif()
	if(USE_VQE_WEBRTC_NN)
		add_definitions(-DVQE_WEBRTC_NN)
	endif()
endif()

if(1)
TARGET_LINK_LIBRARIES(${SAMPLE_APP_AUDIO} -Wl,--start-group dl pthread stdc++
	${VQE_LIB}
	libalgopus.a
	libalgaacenc.a
	libalgaacdec.a
	libalgg726.a
	libalgadpcm.a
	libalgg711.a
	libavformat.a
	libavcodec.a
	libavutil.a
	${MPI_BASE_LIB}
	-Wl,--end-group)
else()
	option(OPUS_CODEC "use opus encoder and decoder" OFF)

	if (OPUS_CODEC)
		#include_directories(${APP_SRC_DIR}/../../libs/algstore/libalgopus_v1_4/include)
		set(OPUS_CODEC_LIB libalgopus.a)
		add_definitions(-DOPUS_CODEC)
	endif ()

	option(AAC_CODEC "use aac encoder and decoder" OFF)

	if (AAC_CODEC)
		#include_directories(${APP_SRC_DIR}/../../libs/algstore/libalgaacenc/include)
		#include_directories(${APP_SRC_DIR}/../../libs/algstore/libalgaacdec/include)
		set(AAC_CODEC_LIB
			libalgaacenc.a
			libalgaacdec.a
		)
		add_definitions(-DAAC_CODEC)
	endif ()

	option(G726_CODEC "use g726 encoder and decoder" OFF)

	if (G726_CODEC)
		#include_directories(${APP_SRC_DIR}/../../libs/algstore/libalgg726/include)
		set(G726_CODEC_LIB libalgg726.a)
		add_definitions(-DG726_CODEC)
	endif ()

	option(ADPCM_CODEC "use adpcm encoder and decoder" OFF)

	if (ADPCM_CODEC)
		#include_directories(${APP_SRC_DIR}/../../libs/algstore/libalgadpcm/include)
		set(ADPCM_CODEC_LIB libalgadpcm.a)
		add_definitions(-DADPCM_CODEC)
	endif ()

	option(G711_CODEC "use g711 encoder and decoder" ON)

	if (G711_CODEC)
		#include_directories(${APP_SRC_DIR}/../../libs/algstore/libalgg711/include)
		set(G711_CODEC_LIB libalgg711.a)
		add_definitions(-DG711_CODEC)
	endif ()

	TARGET_LINK_LIBRARIES(${SAMPLE_APP_AUDIO} -Wl,--start-group dl pthread stdc++
	${VQE_LIB} ${OPUS_CODEC_LIB} ${AAC_CODEC_LIB} ${G726_CODEC_LIB} ${ADPCM_CODEC_LIB} ${G711_CODEC_LIB} -Wl,--end-group)
endif()

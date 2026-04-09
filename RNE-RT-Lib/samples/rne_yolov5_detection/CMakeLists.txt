cmake_minimum_required(VERSION 3.13)

project(sample)

set(ARCH "linux_a7_uclibc" CACHE STRING "Target architecture")
set(ROOT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../.." CACHE PATH "Root path")

if(NOT DEFINED USE_DYNAMIC_LIB)
set(USE_DYNAMIC_LIB 0)
endif()

if(ARCH STREQUAL "linux_a7")
    set(CC_PREFIX "arm-ts-linux-uclibcgnueabihf-")
	set(CPU_FLAGS "-mcpu=cortex-a7 -mfpu=vfp -mfloat-abi=hard")
	set(X_LIB_L_FLAGS rne_pal_linux_a7 gcc)
	set(LIBS_BIT "32bit")
	set(CV_LIBS_SUB_DIR "lib/glibc")
	if(USE_DYNAMIC_LIB)
        set(LIBS_SUB_DIR "bin")
	else()
		set(LIBS_SUB_DIR "libs")
	endif()
elseif(ARCH STREQUAL "linux_a7_uclibc")
    set(CC_PREFIX "arm-ts-linux-uclibcgnueabihf-")
	set(CPU_FLAGS "-mcpu=cortex-a7 -mfpu=vfp -mfloat-abi=hard")
    set(X_LIB_L_FLAGS rne_pal_linux_a7 gcc)
	set(LIBS_BIT "32bit")
	set(CV_LIBS_SUB_DIR "lib/uclibc")
	if(USE_DYNAMIC_LIB)
        set(LIBS_SUB_DIR "arm_ts_linux_uclibcgnueabihf_bin")
	else()
		set(LIBS_SUB_DIR "arm_ts_linux_uclibcgnueabihf_libs")
	endif()
else()
	set(CC_PREFIX "")
    set(CPU_FLAGS "")
    set(X_LIB_L_FLAGS rne_pal_sim rne_vm gcc stdc++)
	set(LIBS_BIT "64bit")
	set(CV_LIBS_SUB_DIR "lib/glibc")
	if(USE_DYNAMIC_LIB)
        set(LIBS_SUB_DIR "bin")
	else()
		set(LIBS_SUB_DIR "libs")
	endif()
endif()

set(CMAKE_C_COMPILER "${CC_PREFIX}gcc")
set(CMAKE_CXX_COMPILER "${CC_PREFIX}g++")

set(LIBS_DIR "${ROOT_PATH}/${LIBS_SUB_DIR}/${LIBS_BIT}")

if(CMAKE_BUILD_TYPE STREQUAL "")
	set(CMAKE_BUILD_TYPE "Release")
endif()

set(CPU_FLAGS_COMMON "-Wall -Werror -Wno-format-truncation -fPIC -ffunction-sections -fdata-sections")
set(CMAKE_C_FLAGS_RELEASE "${CPU_FLAGS} -std=c99 -O2 ${CPU_FLAGS_COMMON}")
set(CMAKE_CXX_FLAGS_RELEASE "${CPU_FLAGS} -std=c++11 -O2 ${CPU_FLAGS_COMMON}")
set(CMAKE_C_FLAGS_DEBUG "${CPU_FLAGS} -std=c99 -O0 -g ${CPU_FLAGS_COMMON}")
set(CMAKE_CXX_FLAGS_DEBUG "${CPU_FLAGS} -std=c++11 -O0 -g ${CPU_FLAGS_COMMON}")

set(BUILD_PATH ${CMAKE_CURRENT_SOURCE_DIR}/build_${ARCH}/${CMAKE_BUILD_TYPE})
set(EXECUTABLE_OUTPUT_PATH ${BUILD_PATH})

include_directories(
    ${ROOT_PATH}/include/
    ${ROOT_PATH}/open_source/crc
    ${ROOT_PATH}/open_source/gp_layers
    ${ROOT_PATH}/open_source/record_file
	${ROOT_PATH}/opencv4.5.5/include/opencv4
)

set(SOURCE_DIR
    "src"
    "../../open_source/crc"
	"../../open_source/gp_layers"
    "../../open_source/record_file"
)

set(ALL_C_FILES "")
foreach(DIR ${SOURCE_DIR})
	file(GLOB_RECURSE C_FILES "${DIR}/*.c")
    file(GLOB_RECURSE CXX_FILES "${DIR}/*.cpp")
	list(APPEND ALL_C_FILES ${C_FILES} ${CXX_FILES})
endforeach()

link_directories(${LIBS_DIR} ${ROOT_PATH}/opencv4.5.5/${CV_LIBS_SUB_DIR}  ${ROOT_PATH}/opencv4.5.5/${CV_LIBS_SUB_DIR}/opencv4/3rdparty)

add_compile_definitions(DISABLE_PRINT_DEBUG_LAYER RESULT_ALL)

get_filename_component(CURRENT_DIR_NAME ${CMAKE_CURRENT_LIST_DIR} NAME)
set(PROJECT_NAME ${CURRENT_DIR_NAME}.elf)
add_executable(${PROJECT_NAME} ${ALL_C_FILES})

set(LINKER_GROUP_START -Wl,--start-group)
set(LINKER_GROUP_END -Wl,--end-group)
set(LL_LINK_LIBS ${X_LIB_L_FLAGS} rne_common rne_pal_linux_a7 rne_rt_g3 c m pthread opencv_world zlib libjpeg-turbo ade libopenjp2 libprotobuf libwebp quirc libpng dl)

target_link_libraries(${PROJECT_NAME} ${LINKER_GROUP_START} ${LL_LINK_LIBS} ${LINKER_GROUP_END})

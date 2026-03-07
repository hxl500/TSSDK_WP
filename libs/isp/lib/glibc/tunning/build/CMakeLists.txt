# To skip cmake check for compiler
set(CMAKE_C_COMPILER_WORKS 1)
set(CMAKE_CXX_COMPILER_WORKS 1)

cmake_minimum_required(VERSION 2.8.5)

project("isp")

include (${CMAKE_CURRENT_SOURCE_DIR}/common.cmake)

add_subdirectory (${ARMCB_OPENSOURCE_SRC}/build                ${ARMCB_GENERATED_TEMP}/external)
add_subdirectory (${ARMCB_ISP_CUSTOM}/build                  ${ARMCB_GENERATED_TEMP}/vendor)

set (SAMPLE_DRV_DIR            ${APP_SRC_DIR}/drv)

set(SAMPLE_APP_DRV_WDT sample_drv_wdt)


set(TARGET_BOARD $ENV{TARGET_BOARD})
message("TARGET_BOARD = ${TARGET_BOARD}")
if(TARGET_BOARD STREQUAL "fpga")
message("fpga!")
add_compile_definitions(FPGA_DEBUG)
else()
message("not fpga!")
endif()

add_executable(
    ${SAMPLE_APP_DRV_WDT}
${SAMPLE_DRV_DIR}/wdt/sample_wdt.c
)

TARGET_LINK_LIBRARIES(${SAMPLE_APP_DRV_WDT} -Wl,--start-group dl pthread stdc++ ${MPI_BASE_LIB} -Wl,--end-group)

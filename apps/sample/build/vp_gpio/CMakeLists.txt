set (SAMPLE_VP_GPIO_DIR            ${APP_SRC_DIR}/vp_gpio)

set(SAMPLE_APP_VENC sample_vp_gpio)

add_executable(
    ${SAMPLE_APP_VENC}
${SAMPLE_VP_GPIO_DIR}/vp_gpio_test.c
)

TARGET_LINK_LIBRARIES(${SAMPLE_APP_VENC} -Wl,--start-group dl pthread stdc++ 
-Wl,--end-group)
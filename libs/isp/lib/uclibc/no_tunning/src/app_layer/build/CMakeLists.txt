################################################libapp_device######################################################
if (ARMCB_TEST_MODE EQUAL 1)
    set(LIB_APP_DEVICE app_device)
    add_library ( ${LIB_APP_DEVICE} ${ARMCB_LIB_FLAG}
        ${ARMCB_ISP_APP}/device/device.c
        ${ARMCB_ISP_APP}/device/device_buf_mgr.c
        ${ARMCB_ISP_APP}/usecase/app_usecase.c
        ${ARMCB_ISP_APP}/config/app_config.c
        ${ARMCB_ISP_APP}/sanity_test/sanity_test.c
    )

    target_link_libraries (${LIB_APP_DEVICE} isp_hal)
    target_link_libraries (${LIB_APP_DEVICE} isp_utils)

else ()
    set(LIB_APP_DEVICE app_device)
    add_library ( ${LIB_APP_DEVICE} ${ARMCB_LIB_FLAG}
        ${ARMCB_ISP_APP}/device/device.c
        ${ARMCB_ISP_APP}/device/device_buf_mgr.c
        ${ARMCB_ISP_APP}/usecase/app_usecase.c
        ${ARMCB_ISP_APP}/config/app_config.c
    )

    target_link_libraries (${LIB_APP_DEVICE} isp_hal)
    target_link_libraries (${LIB_APP_DEVICE} isp_utils)
endif ()

################################################ isp_app_test ###################################################

if (ARMCB_CROSS_BUILD EQUAL 2)
    set(ISP_APP isp_app)
    add_library(
        ${ISP_APP}
        ${ARMCB_ISP_APP}/app_layer.c
    )
    target_link_libraries (${ISP_APP} app_device)
    target_link_libraries (${ISP_APP} niss)
    if ( (ARMCB_PKG_APP EQUAL 1) OR (ARMCB_CUST_BUILD EQUAL 1) )
        install (TARGETS ${ISP_APP}
                 DESTINATION ${ARMCB_TEMP_LIB_DIR})
    endif ()
else()
    set(LIB_APP_UP app_isp)
	add_library ( ${LIB_APP_UP} ${ARMCB_LIB_FLAG}
        ${ARMCB_ISP_APP}/app_layer.c
        ${ARMCB_ISP_APP}/app_upper.c
	)

    target_link_libraries (${LIB_APP_UP} app_device)
    if ( (ARMCB_PKG_APP EQUAL 1) OR (ARMCB_CUST_BUILD EQUAL 1) )
		install (TARGETS ${LIB_APP_UP}
                 DESTINATION ${ARMCB_TEMP_LIB_DIR})
    endif ()

if (ARMCB_STATIC_COMPILE EQUAL 1)
    link_directories(${ARMCB_GENERATED_OUT}/usr/lib)
endif()

    set(ISP_APP isp_app)
    add_executable(
        ${ISP_APP}
        ${ARMCB_ISP_APP}/isp_app.c
    )
    if (ARMCB_CUST_BUILD EQUAL 1)
        target_link_libraries (${ISP_APP} app_isp app_device ${ALL_LIBS} ${ALL_LIBS})
        if (ARMCB_STATIC_COMPILE EQUAL 1)
            target_link_libraries (${ISP_APP} sensor_imx415)
            target_link_libraries (${ISP_APP} csi2d_imx415)
            target_link_libraries (${ISP_APP} tuning_imx415_normal)
            target_link_libraries (${ISP_APP} tuning_imx415_dol2)
            target_link_libraries (${ISP_APP} calibration_imx415)

            target_link_libraries (${ISP_APP} sensor_gc4023)
            target_link_libraries (${ISP_APP} csi2d_gc4023)
            target_link_libraries (${ISP_APP} tuning_gc4023_normal)
            target_link_libraries (${ISP_APP} calibration_gc4023)
            
            target_link_libraries (${ISP_APP} sensor_gc2083)
            target_link_libraries (${ISP_APP} csi2d_gc2083)
            target_link_libraries (${ISP_APP} tuning_gc2083_normal)
            target_link_libraries (${ISP_APP} calibration_gc2083)

            target_link_libraries (${ISP_APP} sensor_sc200ai)
            target_link_libraries (${ISP_APP} csi2d_sc200ai)
            target_link_libraries (${ISP_APP} tuning_sc200ai_normal)
            target_link_libraries (${ISP_APP} calibration_sc200ai)
            target_link_libraries (${ISP_APP} json_convert)

            target_link_libraries (${ISP_APP} sensor_cv2003)
            target_link_libraries (${ISP_APP} csi2d_cv2003)
            target_link_libraries (${ISP_APP} tuning_cv2003_normal)
            target_link_libraries (${ISP_APP} calibration_cv2003)
        endif()
    else ()
        target_link_libraries (${ISP_APP} app_device)
        target_link_libraries (${ISP_APP} niss)
    endif ()

endif()

################################################ config.txt  ###################################################
set (ISP_APP_CONFIG
    ${ARMCB_ISP_APP}/config.txt
    ${ARMCB_ISP_APP}/config_dualcam_preview.txt
)

install(FILES ${ISP_APP_CONFIG} DESTINATION ${ARMCB_GENERATED_OUT_DATA})

############################################### Install target ###############################################
if ( (ARMCB_PKG_APP EQUAL 1) OR (ARMCB_CUST_BUILD EQUAL 1) )
    if (ARMCB_CROSS_BUILD EQUAL 2)
        install (TARGETS ${LIB_APP_DEVICE}
                DESTINATION ${ARMCB_TEMP_LIB_DIR})
    endif ()
endif ()

if (ARMCB_CROSS_BUILD EQUAL 2)
	set (ENV{ARMCB_ALL_LIBS} "$ENV{ARMCB_ALL_LIBS} -l${ISP_APP} -l${LIB_APP_DEVICE}")
endif ()

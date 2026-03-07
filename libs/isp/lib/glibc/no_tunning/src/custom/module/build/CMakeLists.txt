################################################libsen_modules######################################################
set(LIB_SEN_MODULES sen_modules)

add_library ( ${LIB_SEN_MODULES}  ${ARMCB_LIB_FLAG}
    ${ARMCB_ISP_CUSTOM}/module/supported_module.c
    ${ARMCB_ISP_CUSTOM}/module/camera_module_tx52xx.c
    ${ARMCB_ISP_CUSTOM}/actuator/actuator_normal/actuator_normal.c
    #${ARMCB_ISP_CUSTOM}/actuator/actuator_normal/gpio-utils.c
)

target_link_libraries (${LIB_SEN_MODULES} sensor_imx415)
target_link_libraries (${LIB_SEN_MODULES} csi2d_imx415)
target_link_libraries (${LIB_SEN_MODULES} tuning_imx415_normal)
target_link_libraries (${LIB_SEN_MODULES} tuning_imx415_dol2)
target_link_libraries (${LIB_SEN_MODULES} calibration_imx415)

target_link_libraries (${LIB_SEN_MODULES} sensor_gc4023)
target_link_libraries (${LIB_SEN_MODULES} csi2d_gc4023)
target_link_libraries (${LIB_SEN_MODULES} tuning_gc4023_normal)
target_link_libraries (${LIB_SEN_MODULES} calibration_gc4023)

target_link_libraries (${LIB_SEN_MODULES} sensor_gc4653)
target_link_libraries (${LIB_SEN_MODULES} csi2d_gc4653)
target_link_libraries (${LIB_SEN_MODULES} tuning_gc4653_normal)
target_link_libraries (${LIB_SEN_MODULES} calibration_gc4653)

target_link_libraries (${LIB_SEN_MODULES} sensor_cv4002)
target_link_libraries (${LIB_SEN_MODULES} csi2d_cv4002)
target_link_libraries (${LIB_SEN_MODULES} tuning_cv4002_normal)
target_link_libraries (${LIB_SEN_MODULES} calibration_cv4002)

target_link_libraries (${LIB_SEN_MODULES} sensor_c3390)
target_link_libraries (${LIB_SEN_MODULES} csi2d_c3390)
target_link_libraries (${LIB_SEN_MODULES} tuning_c3390_normal)
target_link_libraries (${LIB_SEN_MODULES} calibration_c3390)

target_link_libraries (${LIB_SEN_MODULES} sensor_sc200ai)
target_link_libraries (${LIB_SEN_MODULES} csi2d_sc200ai)
target_link_libraries (${LIB_SEN_MODULES} tuning_sc200ai_normal)
target_link_libraries (${LIB_SEN_MODULES} calibration_sc200ai)

target_link_libraries (${LIB_SEN_MODULES} sensor_sc2336p)
target_link_libraries (${LIB_SEN_MODULES} csi2d_sc2336p)
target_link_libraries (${LIB_SEN_MODULES} tuning_sc2336p_normal)
target_link_libraries (${LIB_SEN_MODULES} calibration_sc2336p)

target_link_libraries (${LIB_SEN_MODULES} sensor_gc2083)
target_link_libraries (${LIB_SEN_MODULES} csi2d_gc2083)
target_link_libraries (${LIB_SEN_MODULES} tuning_gc2083_normal)
target_link_libraries (${LIB_SEN_MODULES} calibration_gc2083)


target_link_libraries (${LIB_SEN_MODULES} sensor_k17)
target_link_libraries (${LIB_SEN_MODULES} csi2d_k17)
target_link_libraries (${LIB_SEN_MODULES} tuning_k17_normal)
target_link_libraries (${LIB_SEN_MODULES} calibration_k17)

target_link_libraries (${LIB_SEN_MODULES} sensor_sc430ai)
target_link_libraries (${LIB_SEN_MODULES} csi2d_sc430ai)
target_link_libraries (${LIB_SEN_MODULES} tuning_sc430ai_normal)
target_link_libraries (${LIB_SEN_MODULES} calibration_sc430ai)

target_link_libraries (${LIB_SEN_MODULES} sensor_sc501ai)
target_link_libraries (${LIB_SEN_MODULES} csi2d_sc501ai)
target_link_libraries (${LIB_SEN_MODULES} tuning_sc501ai_normal)
target_link_libraries (${LIB_SEN_MODULES} calibration_sc501ai)

target_link_libraries (${LIB_SEN_MODULES} sensor_hi556)
target_link_libraries (${LIB_SEN_MODULES} csi2d_hi556)
target_link_libraries (${LIB_SEN_MODULES} tuning_hi556_normal)
target_link_libraries (${LIB_SEN_MODULES} calibration_hi556)

target_link_libraries (${LIB_SEN_MODULES} sensor_cv2003)
target_link_libraries (${LIB_SEN_MODULES} csi2d_cv2003)
target_link_libraries (${LIB_SEN_MODULES} tuning_cv2003_normal)
target_link_libraries (${LIB_SEN_MODULES} calibration_cv2003)

target_link_libraries (${LIB_SEN_MODULES} sensor_gc5603)
target_link_libraries (${LIB_SEN_MODULES} csi2d_gc5603)
target_link_libraries (${LIB_SEN_MODULES} tuning_gc5603_normal)
target_link_libraries (${LIB_SEN_MODULES} calibration_gc5603)


############################################### Install target ###############################################
if ( (ARMCB_PKG_FW_ISPNODE EQUAL 1) OR (ARMCB_CUST_BUILD EQUAL 1) )
    install (TARGETS ${LIB_SEN_MODULES}
             DESTINATION ${ARMCB_TEMP_LIB_DIR})
endif ()
set (ENV{ARMCB_ALL_LIBS} "$ENV{ARMCB_ALL_LIBS} -l${LIB_SEN_MODULES}")

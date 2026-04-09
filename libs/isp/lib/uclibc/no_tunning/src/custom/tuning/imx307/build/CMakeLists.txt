################################################libtuning_imx307_normal######################################################
set(LIB_TUNING_IMX307_NORMAL tuning_imx307_normal)

add_library ( ${LIB_TUNING_IMX307_NORMAL}  
    ${ARMCB_ISP_CUSTOM}/tuning/imx307/tuning_imx307_normal.c
)

target_link_libraries (${LIB_TUNING_IMX307_NORMAL} isp_utils)
target_link_libraries (${LIB_TUNING_IMX307_NORMAL} json_convert)

################################################libcalibration_imx307######################################################
set(LIB_CALIBRATION_IMX307 calibration_imx307)

add_library ( ${LIB_CALIBRATION_IMX307}  
    ${ARMCB_ISP_CUSTOM}/tuning/imx307/calibration_imx307.c
)

target_link_libraries (${LIB_CALIBRATION_IMX307} isp_utils)
target_link_libraries (${LIB_CALIBRATION_IMX307} json_convert)


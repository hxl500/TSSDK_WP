################################################libtuning_ar0239_normal######################################################
set(LIB_TUNING_AR0239_NORMAL tuning_ar0239_normal)

add_library ( ${LIB_TUNING_AR0239_NORMAL}  
    ${ARMCB_ISP_CUSTOM}/tuning/ar0239/tuning_ar0239_normal.c
)

target_link_libraries (${LIB_TUNING_AR0239_NORMAL} isp_utils)
target_link_libraries (${LIB_TUNING_AR0239_NORMAL} json_convert)

################################################libcalibration_ar0239######################################################
set(LIB_CALIBRATION_AR0239 calibration_ar0239)

add_library ( ${LIB_CALIBRATION_AR0239}  
    ${ARMCB_ISP_CUSTOM}/tuning/ar0239/calibration_ar0239.c
)

target_link_libraries (${LIB_CALIBRATION_AR0239} isp_utils)
target_link_libraries (${LIB_CALIBRATION_AR0239} json_convert)


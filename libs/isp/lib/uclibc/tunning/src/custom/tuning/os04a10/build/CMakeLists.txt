################################################libtuning_os04a10_normal######################################################
set(LIB_TUNING_OS04A10_NORMAL tuning_os04a10_normal)

add_library ( ${LIB_TUNING_OS04A10_NORMAL}  
    ${ARMCB_ISP_CUSTOM}/tuning/os04a10/tuning_os04a10_normal.c
)

target_link_libraries (${LIB_TUNING_OS04A10_NORMAL} isp_utils)
target_link_libraries (${LIB_TUNING_OS04A10_NORMAL} json_convert)
################################################libcalibration_os04a10######################################################
set(LIB_CALIBRATION_OS04A10 calibration_os04a10)

add_library ( ${LIB_CALIBRATION_OS04A10}  
    ${ARMCB_ISP_CUSTOM}/tuning/os04a10/calibration_os04a10.c
)

target_link_libraries (${LIB_CALIBRATION_OS04A10} isp_utils)
target_link_libraries (${LIB_CALIBRATION_OS04A10} json_convert)
 ################################################libtuning_os04a10_dol2######################################################
set(LIB_TUNING_OS04A10_DOL2 tuning_os04a10_dol2)

add_library ( ${LIB_TUNING_OS04A10_DOL2}  
    ${ARMCB_ISP_CUSTOM}/tuning/os04a10/tuning_os04a10_dol2.c
)
target_link_libraries (${LIB_TUNING_OS04A10_DOL2} isp_utils)
target_link_libraries (${LIB_TUNING_OS04A10_DOL2} json_convert)
################################################ json install  ###################################################
# set (JSON_OS04A10_OCT
#     ${ARMCB_ISP_CUSTOM}/tuning/os04a10/json_oct/os04a10.cal
#     ${ARMCB_ISP_CUSTOM}/tuning/os04a10/json_oct/os04a10_TS.cal
#     ${ARMCB_ISP_CUSTOM}/tuning/os04a10/json_oct/os04a10_nor.tun
# )
#
# install(FILES ${JSON_OS04A10_OCT} DESTINATION ${ARMCB_GENERATED_OUT_DATA}/json_oct)
# install(FILES ${JSON_OS04A10_OCT} DESTINATION ${ARMCB_GENERATED_OUT_JSON}/json_oct)

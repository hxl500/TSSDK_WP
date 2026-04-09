################################################libtuning_hi556_normal######################################################
set(LIB_TUNING_HI556_NORMAL tuning_hi556_normal)

add_library ( ${LIB_TUNING_HI556_NORMAL}  
    ${ARMCB_ISP_CUSTOM}/tuning/hi556/tuning_hi556_linear_dual.c
    ${ARMCB_ISP_CUSTOM}/tuning/hi556/tuning_hi556_linear_dual_ir.c
)

target_link_libraries (${LIB_TUNING_HI556_NORMAL} isp_utils)
target_link_libraries (${LIB_TUNING_HI556_NORMAL} json_convert)
################################################libcalibration_hi556######################################################
set(LIB_CALIBRATION_HI556 calibration_hi556)

add_library ( ${LIB_CALIBRATION_HI556}  
    ${ARMCB_ISP_CUSTOM}/tuning/hi556/calibration_hi556_dual.c
    ${ARMCB_ISP_CUSTOM}/tuning/hi556/calibration_hi556_dual_ir.c
)

target_link_libraries (${LIB_CALIBRATION_HI556} isp_utils)
target_link_libraries (${LIB_CALIBRATION_HI556} json_convert)
# ################################################libtuning_hi556_dol2######################################################
#set(LIB_TUNING_HI556_DOL2 tuning_hi556_dol2)
#
#add_library ( ${LIB_TUNING_HI556_DOL2}  
#    ${ARMCB_ISP_CUSTOM}/tuning/hi556/tuning_hi556_dol2.c
#)
#target_link_libraries (${LIB_TUNING_HI556_DOL2} isp_utils)
#target_link_libraries (${LIB_TUNING_HI556_DOL2} json_convert)
################################################ json install  ###################################################
# set (JSON_HI556_OCT
#     ${ARMCB_ISP_CUSTOM}/tuning/hi556/json_oct/hi556.cal
#     ${ARMCB_ISP_CUSTOM}/tuning/hi556/json_oct/hi556_TS.cal
#     ${ARMCB_ISP_CUSTOM}/tuning/hi556/json_oct/hi556_nor.tun
# )
#
# install(FILES ${JSON_HI556_OCT} DESTINATION ${ARMCB_GENERATED_OUT_DATA}/json_oct)
# install(FILES ${JSON_HI556_OCT} DESTINATION ${ARMCB_GENERATED_OUT_JSON}/json_oct)

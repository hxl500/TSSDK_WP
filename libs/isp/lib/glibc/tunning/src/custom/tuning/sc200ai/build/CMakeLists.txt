################################################libtuning_sc200ai_normal######################################################
set(LIB_TUNING_SC200ai_NORMAL tuning_sc200ai_normal)

add_library ( ${LIB_TUNING_SC200ai_NORMAL}  
    ${ARMCB_ISP_CUSTOM}/tuning/sc200ai/tuning_sc200ai_linear_dual.c
    ${ARMCB_ISP_CUSTOM}/tuning/sc200ai/tuning_sc200ai_linear_dual_ir.c
)

target_link_libraries (${LIB_TUNING_SC200ai_NORMAL} isp_utils)
target_link_libraries (${LIB_TUNING_SC200ai_NORMAL} json_convert)
################################################libcalibration_sc200ai######################################################
set(LIB_CALIBRATION_SC200ai calibration_sc200ai)

add_library ( ${LIB_CALIBRATION_SC200ai}  
    ${ARMCB_ISP_CUSTOM}/tuning/sc200ai/calibration_sc200ai_dual.c
    ${ARMCB_ISP_CUSTOM}/tuning/sc200ai/calibration_sc200ai_dual_ir.c
)

target_link_libraries (${LIB_CALIBRATION_SC200ai} isp_utils)
target_link_libraries (${LIB_CALIBRATION_SC200ai} json_convert)
# ################################################libtuning_sc200ai_dol2######################################################
#set(LIB_TUNING_SC200ai_DOL2 tuning_sc200ai_dol2)
#
#add_library ( ${LIB_TUNING_SC200ai_DOL2}  
#    ${ARMCB_ISP_CUSTOM}/tuning/sc200ai/tuning_sc200ai_dol2.c
#)
#target_link_libraries (${LIB_TUNING_SC200ai_DOL2} isp_utils)
#target_link_libraries (${LIB_TUNING_SC200ai_DOL2} json_convert)
################################################ json install  ###################################################
# set (JSON_SC200ai_OCT
#     ${ARMCB_ISP_CUSTOM}/tuning/sc200ai/json_oct/415.cal
#     ${ARMCB_ISP_CUSTOM}/tuning/sc200ai/json_oct/415_TS.cal
#     ${ARMCB_ISP_CUSTOM}/tuning/sc200ai/json_oct/415_nor.tun
# )
#
# install(FILES ${JSON_SC200ai_OCT} DESTINATION ${ARMCB_GENERATED_OUT_DATA}/json_oct)
# install(FILES ${JSON_SC200ai_OCT} DESTINATION ${ARMCB_GENERATED_OUT_JSON}/json_oct)

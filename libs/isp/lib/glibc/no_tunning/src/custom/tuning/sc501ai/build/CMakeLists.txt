################################################libtuning_sc501ai_normal######################################################
set(LIB_TUNING_SC501AI_NORMAL tuning_sc501ai_normal)

add_library ( ${LIB_TUNING_SC501AI_NORMAL} 
    ${ARMCB_ISP_CUSTOM}/tuning/sc501ai/tuning_sc501ai_normal.c
    ${ARMCB_ISP_CUSTOM}/tuning/sc501ai/tuning_sc501ai_normal_ir.c
)

target_link_libraries (${LIB_TUNING_SC501AI_NORMAL} isp_utils)
target_link_libraries (${LIB_TUNING_SC501AI_NORMAL} json_convert)
################################################libcalibration_sc501ai######################################################
set(LIB_CALIBRATION_SC501AI calibration_sc501ai)

add_library ( ${LIB_CALIBRATION_SC501AI}
    ${ARMCB_ISP_CUSTOM}/tuning/sc501ai/calibration_sc501ai.c
    ${ARMCB_ISP_CUSTOM}/tuning/sc501ai/calibration_sc501ai_ir.c
)

target_link_libraries (${LIB_CALIBRATION_SC501AI} isp_utils)
target_link_libraries (${LIB_CALIBRATION_SC501AI} json_convert)
# ################################################libtuning_sc501ai_dol2######################################################
#set(LIB_TUNING_SC501AI_DOL2 tuning_sc501ai_dol2)
#
#add_library ( ${LIB_TUNING_SC501AI_DOL2} ${ARMCB_LIB_FLAG}
#    ${ARMCB_ISP_CUSTOM}/tuning/sc501ai/tuning_sc501ai_dol2.c
#)
#target_link_libraries (${LIB_TUNING_SC501AI_DOL2} isp_utils)
#target_link_libraries (${LIB_TUNING_SC501AI_DOL2} json_convert)
################################################ json install  ###################################################
# set (JSON_SC501AI_OCT
#     ${ARMCB_ISP_CUSTOM}/tuning/sc501ai/json_oct/415.cal
#     ${ARMCB_ISP_CUSTOM}/tuning/sc501ai/json_oct/415_TS.cal
#     ${ARMCB_ISP_CUSTOM}/tuning/sc501ai/json_oct/415_nor.tun
# )
#
# install(FILES ${JSON_SC501AI_OCT} DESTINATION ${ARMCB_GENERATED_OUT_DATA}/json_oct)
# install(FILES ${JSON_SC501AI_OCT} DESTINATION ${ARMCB_GENERATED_OUT_JSON}/json_oct)



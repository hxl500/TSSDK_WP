################################################libtuning_sc430ai_normal######################################################
set(LIB_TUNING_SC430AI_NORMAL tuning_sc430ai_normal)

add_library ( ${LIB_TUNING_SC430AI_NORMAL} 
    ${ARMCB_ISP_CUSTOM}/tuning/sc430ai/tuning_sc430ai_normal.c
    ${ARMCB_ISP_CUSTOM}/tuning/sc430ai/tuning_sc430ai_normal_ir.c
)

target_link_libraries (${LIB_TUNING_SC430AI_NORMAL} isp_utils)
target_link_libraries (${LIB_TUNING_SC430AI_NORMAL} json_convert)
################################################libcalibration_sc430ai######################################################
set(LIB_CALIBRATION_SC430AI calibration_sc430ai)

add_library ( ${LIB_CALIBRATION_SC430AI} 
    ${ARMCB_ISP_CUSTOM}/tuning/sc430ai/calibration_sc430ai.c
    ${ARMCB_ISP_CUSTOM}/tuning/sc430ai/calibration_sc430ai_ir.c
)

target_link_libraries (${LIB_CALIBRATION_SC430AI} isp_utils)
target_link_libraries (${LIB_CALIBRATION_SC430AI} json_convert)
# ################################################libtuning_sc430ai_dol2######################################################
#set(LIB_TUNING_SC430AI_DOL2 tuning_sc430ai_dol2)
#
#add_library ( ${LIB_TUNING_SC430AI_DOL2} 
#    ${ARMCB_ISP_CUSTOM}/tuning/sc430ai/tuning_sc430ai_dol2.c
#)
#target_link_libraries (${LIB_TUNING_SC430AI_DOL2} isp_utils)
#target_link_libraries (${LIB_TUNING_SC430AI_DOL2} json_convert)
################################################ json install  ###################################################
# set (JSON_SC430AI_OCT
#     ${ARMCB_ISP_CUSTOM}/tuning/sc430ai/json_oct/415.cal
#     ${ARMCB_ISP_CUSTOM}/tuning/sc430ai/json_oct/415_TS.cal
#     ${ARMCB_ISP_CUSTOM}/tuning/sc430ai/json_oct/415_nor.tun
# )
#
# install(FILES ${JSON_SC430AI_OCT} DESTINATION ${ARMCB_GENERATED_OUT_DATA}/json_oct)
# install(FILES ${JSON_SC430AI_OCT} DESTINATION ${ARMCB_GENERATED_OUT_JSON}/json_oct)
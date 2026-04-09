################################################libtuning_sc2337p_normal######################################################
set(LIB_TUNING_SC2337P_NORMAL tuning_sc2337p_normal)

add_library ( ${LIB_TUNING_SC2337P_NORMAL}  
    ${ARMCB_ISP_CUSTOM}/tuning/sc2337p/tuning_sc2337p_linear_dual.c
    ${ARMCB_ISP_CUSTOM}/tuning/sc2337p/tuning_sc2337p_linear_dual_aux.c
    ${ARMCB_ISP_CUSTOM}/tuning/sc2337p/tuning_sc2337p_linear_dual_ir.c
	${ARMCB_ISP_CUSTOM}/tuning/sc2337p/tuning_sc2337p_linear_dual_ir_aux.c
)

target_link_libraries (${LIB_TUNING_SC2337P_NORMAL} isp_utils)
target_link_libraries (${LIB_TUNING_SC2337P_NORMAL} json_convert)
################################################libcalibration_sc2337p######################################################
set(LIB_CALIBRATION_SC2337P calibration_sc2337p)

add_library ( ${LIB_CALIBRATION_SC2337P}  
    ${ARMCB_ISP_CUSTOM}/tuning/sc2337p/calibration_sc2337p_dual.c
    ${ARMCB_ISP_CUSTOM}/tuning/sc2337p/calibration_sc2337p_dual_ir.c
)

target_link_libraries (${LIB_CALIBRATION_SC2337P} isp_utils)
target_link_libraries (${LIB_CALIBRATION_SC2337P} json_convert)
# ################################################libtuning_sc2337p_dol2######################################################
#set(LIB_TUNING_SC2337P_DOL2 tuning_sc2337p_dol2)
#
#add_library ( ${LIB_TUNING_SC2337P_DOL2}  
#    ${ARMCB_ISP_CUSTOM}/tuning/sc2337p/tuning_sc2337p_dol2.c
#)
#target_link_libraries (${LIB_TUNING_SC2337P_DOL2} isp_utils)
#target_link_libraries (${LIB_TUNING_SC2337P_DOL2} json_convert)
################################################ json install  ###################################################
# set (JSON_SC2337P_OCT
#     ${ARMCB_ISP_CUSTOM}/tuning/sc2337p/json_oct/415.cal
#     ${ARMCB_ISP_CUSTOM}/tuning/sc2337p/json_oct/415_TS.cal
#     ${ARMCB_ISP_CUSTOM}/tuning/sc2337p/json_oct/415_nor.tun
# )
#
# install(FILES ${JSON_SC2337P_OCT} DESTINATION ${ARMCB_GENERATED_OUT_DATA}/json_oct)
# install(FILES ${JSON_SC2337P_OCT} DESTINATION ${ARMCB_GENERATED_OUT_JSON}/json_oct)

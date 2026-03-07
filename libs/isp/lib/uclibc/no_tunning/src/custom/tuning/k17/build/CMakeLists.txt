################################################libtuning_k17_normal######################################################
set(LIB_TUNING_K17_NORMAL tuning_k17_normal)

add_library ( ${LIB_TUNING_K17_NORMAL}
    ${ARMCB_ISP_CUSTOM}/tuning/k17/tuning_k17_normal.c
    ${ARMCB_ISP_CUSTOM}/tuning/k17/tuning_k17_normal_ir.c
)

target_link_libraries (${LIB_TUNING_K17_NORMAL} isp_utils)
target_link_libraries (${LIB_TUNING_K17_NORMAL} json_convert)
################################################libcalibration_k17######################################################
set(LIB_CALIBRATION_K17 calibration_k17)

add_library ( ${LIB_CALIBRATION_K17}
    ${ARMCB_ISP_CUSTOM}/tuning/k17/calibration_k17.c
    ${ARMCB_ISP_CUSTOM}/tuning/k17/calibration_k17_ir.c
)

target_link_libraries (${LIB_CALIBRATION_K17} isp_utils)
target_link_libraries (${LIB_CALIBRATION_K17} json_convert)
# ################################################libtuning_k17_dol2######################################################
set(LIB_TUNING_K17_DOL2 tuning_k17_dol2)

add_library ( ${LIB_TUNING_K17_DOL2}
   ${ARMCB_ISP_CUSTOM}/tuning/k17/tuning_k17_dol2.c
)
target_link_libraries (${LIB_TUNING_K17_DOL2} isp_utils)
target_link_libraries (${LIB_TUNING_K17_DOL2} json_convert)
################################################ json install  ###################################################
# set (JSON_K17_OCT
#     ${ARMCB_ISP_CUSTOM}/tuning/k17/json_oct/415.cal
#     ${ARMCB_ISP_CUSTOM}/tuning/k17/json_oct/415_TS.cal
#     ${ARMCB_ISP_CUSTOM}/tuning/k17/json_oct/415_nor.tun
# )
#
# install(FILES ${JSON_K17_OCT} DESTINATION ${ARMCB_GENERATED_OUT_DATA}/json_oct)
# install(FILES ${JSON_K17_OCT} DESTINATION ${ARMCB_GENERATED_OUT_JSON}/json_oct)
# ############################################### Install target ###############################################
# if ( (ARMCB_PKG_FW_SSRNODE EQUAL 1) OR (ARMCB_CUST_BUILD EQUAL 1) )
#     install (TARGETS ${LIB_TUNING_K17_NORMAL}
#              DESTINATION ${ARMCB_TEMP_LIB_DIR})
#     install (TARGETS ${LIB_CALIBRATION_K17}
#              DESTINATION ${ARMCB_TEMP_LIB_DIR})
#     install (TARGETS ${LIB_TUNING_K17_DOL2}
#     DESTINATION ${ARMCB_TEMP_LIB_DIR})
# endif ()

# set (ENV{ARMCB_ALL_LIBS} "$ENV{ARMCB_ALL_LIBS} -l${LIB_TUNING_K17_NORMAL} -l${LIB_CALIBRATION_K17} -l${LIB_TUNING_K17_DOL2}")

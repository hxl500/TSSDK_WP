################################################libtuning_c3390_normal######################################################
set(LIB_TUNING_C3390_NORMAL tuning_c3390_normal)

add_library ( ${LIB_TUNING_C3390_NORMAL}
    ${ARMCB_ISP_CUSTOM}/tuning/c3390/tuning_c3390_normal.c
    ${ARMCB_ISP_CUSTOM}/tuning/c3390/tuning_c3390_normal_ir.c
    ${ARMCB_ISP_CUSTOM}/tuning/c3390/tuning_c3390_normal_slave.c
)

target_link_libraries (${LIB_TUNING_C3390_NORMAL} isp_utils)
target_link_libraries (${LIB_TUNING_C3390_NORMAL} json_convert)
################################################libcalibration_c3390######################################################
set(LIB_CALIBRATION_C3390 calibration_c3390)

add_library ( ${LIB_CALIBRATION_C3390}
    ${ARMCB_ISP_CUSTOM}/tuning/c3390/calibration_c3390.c
    ${ARMCB_ISP_CUSTOM}/tuning/c3390/calibration_c3390_ir.c
)

target_link_libraries (${LIB_CALIBRATION_C3390} isp_utils)
target_link_libraries (${LIB_CALIBRATION_C3390} json_convert)
# ################################################libtuning_c3390_dol2######################################################
#set(LIB_TUNING_C3390_DOL2 tuning_c3390_dol2)
#
#add_library ( ${LIB_TUNING_C3390_DOL2} ${ARMCB_LIB_FLAG}
#    ${ARMCB_ISP_CUSTOM}/tuning/c3390/tuning_c3390_dol2.c
#)
#target_link_libraries (${LIB_TUNING_C3390_DOL2} isp_utils)
#target_link_libraries (${LIB_TUNING_C3390_DOL2} json_convert)
################################################ json install  ###################################################
# set (JSON_C3390_OCT
#     ${ARMCB_ISP_CUSTOM}/tuning/c3390/json_oct/415.cal
#     ${ARMCB_ISP_CUSTOM}/tuning/c3390/json_oct/415_TS.cal
#     ${ARMCB_ISP_CUSTOM}/tuning/c3390/json_oct/415_nor.tun
# )
#
# install(FILES ${JSON_C3390_OCT} DESTINATION ${ARMCB_GENERATED_OUT_DATA}/json_oct)
# install(FILES ${JSON_C3390_OCT} DESTINATION ${ARMCB_GENERATED_OUT_JSON}/json_oct)
############################################### Install target ###############################################
if ( (ARMCB_PKG_FW_SSRNODE EQUAL 1) OR (ARMCB_CUST_BUILD EQUAL 1) )
    install (TARGETS ${LIB_TUNING_C3390_NORMAL}
             DESTINATION ${ARMCB_TEMP_LIB_DIR})
    install (TARGETS ${LIB_CALIBRATION_C3390}
             DESTINATION ${ARMCB_TEMP_LIB_DIR})
endif ()

set (ENV{ARMCB_ALL_LIBS} "$ENV{ARMCB_ALL_LIBS} -l${LIB_TUNING_C3390_NORMAL} -l${LIB_CALIBRATION_C3390}")

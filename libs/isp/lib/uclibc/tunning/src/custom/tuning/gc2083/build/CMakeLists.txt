################################################libtuning_gc2083_normal######################################################
set(LIB_TUNING_GC2083_NORMAL tuning_gc2083_normal)

add_library(${LIB_TUNING_GC2083_NORMAL} 
    ${ARMCB_ISP_CUSTOM}/tuning/gc2083/tuning_gc2083_normal.c
    ${ARMCB_ISP_CUSTOM}/tuning/gc2083/tuning_gc2083_normal_ir.c
    ${ARMCB_ISP_CUSTOM}/tuning/gc2083/tuning_gc2083_normal_slave.c
    ${ARMCB_ISP_CUSTOM}/tuning/gc2083/tuning_gc2083_normal_ir_slave.c
)

target_link_libraries (${LIB_TUNING_GC2083_NORMAL} isp_utils)
target_link_libraries (${LIB_TUNING_GC2083_NORMAL} json_convert)
################################################libcalibration_gc2083######################################################
set(LIB_CALIBRATION_GC2083 calibration_gc2083)

add_library ( ${LIB_CALIBRATION_GC2083} 
    ${ARMCB_ISP_CUSTOM}/tuning/gc2083/calibration_gc2083.c
    ${ARMCB_ISP_CUSTOM}/tuning/gc2083/calibration_gc2083_ir.c
)

target_link_libraries (${LIB_CALIBRATION_GC2083} isp_utils)
target_link_libraries (${LIB_CALIBRATION_GC2083} json_convert)
# ################################################libtuning_gc2083_dol2######################################################
#set(LIB_TUNING_GC2083_DOL2 tuning_gc2083_dol2)
#
#add_library ( ${LIB_TUNING_GC2083_DOL2} 
#    ${ARMCB_ISP_CUSTOM}/tuning/gc2083/tuning_gc2083_dol2.c
#)
#target_link_libraries (${LIB_TUNING_GC2083_DOL2} isp_utils)
#target_link_libraries (${LIB_TUNING_GC2083_DOL2} json_convert)
################################################ json install  ###################################################
# set (JSON_GC2083_OCT
#     ${ARMCB_ISP_CUSTOM}/tuning/gc2083/json_oct/415.cal
#     ${ARMCB_ISP_CUSTOM}/tuning/gc2083/json_oct/415_TS.cal
#     ${ARMCB_ISP_CUSTOM}/tuning/gc2083/json_oct/415_nor.tun
# )
#
# install(FILES ${JSON_GC2083_OCT} DESTINATION ${ARMCB_GENERATED_OUT_DATA}/json_oct)
# install(FILES ${JSON_GC2083_OCT} DESTINATION ${ARMCB_GENERATED_OUT_JSON}/json_oct)


################################################libtuning_gc4023_normal######################################################
set(LIB_TUNING_GC4023_NORMAL tuning_gc4023_normal)

add_library ( ${LIB_TUNING_GC4023_NORMAL}
    ${ARMCB_ISP_CUSTOM}/tuning/gc4023/tuning_gc4023_normal.c
    ${ARMCB_ISP_CUSTOM}/tuning/gc4023/tuning_gc4023_normal_ir.c
    ${ARMCB_ISP_CUSTOM}/tuning/gc4023/tuning_gc4023_linear_master.c
    ${ARMCB_ISP_CUSTOM}/tuning/gc4023/tuning_gc4023_linear_slave.c
    ${ARMCB_ISP_CUSTOM}/tuning/gc4023/tuning_gc4023_linear_dual_ir.c
)

target_link_libraries (${LIB_TUNING_GC4023_NORMAL} isp_utils)
target_link_libraries (${LIB_TUNING_GC4023_NORMAL} json_convert)
################################################libcalibration_gc4023######################################################
set(LIB_CALIBRATION_GC4023 calibration_gc4023)

add_library ( ${LIB_CALIBRATION_GC4023}  
    ${ARMCB_ISP_CUSTOM}/tuning/gc4023/calibration_gc4023.c
    ${ARMCB_ISP_CUSTOM}/tuning/gc4023/calibration_gc4023_ir.c
    ${ARMCB_ISP_CUSTOM}/tuning/gc4023/calibration_gc4023_dual.c
    ${ARMCB_ISP_CUSTOM}/tuning/gc4023/calibration_gc4023_dual_ir.c
)

target_link_libraries (${LIB_CALIBRATION_GC4023} isp_utils)
target_link_libraries (${LIB_CALIBRATION_GC4023} json_convert)
# ################################################libtuning_gc4023_dol2######################################################
#set(LIB_TUNING_GC4023_DOL2 tuning_gc4023_dol2)
#
#add_library ( ${LIB_TUNING_GC4023_DOL2}  
#    ${ARMCB_ISP_CUSTOM}/tuning/gc4023/tuning_gc4023_dol2.c
#)
#target_link_libraries (${LIB_TUNING_GC4023_DOL2} isp_utils)
#target_link_libraries (${LIB_TUNING_GC4023_DOL2} json_convert)
################################################ json install  ###################################################
# set (JSON_GC4023_OCT
#     ${ARMCB_ISP_CUSTOM}/tuning/gc4023/json_oct/415.cal
#     ${ARMCB_ISP_CUSTOM}/tuning/gc4023/json_oct/415_TS.cal
#     ${ARMCB_ISP_CUSTOM}/tuning/gc4023/json_oct/415_nor.tun
# )
#
# install(FILES ${JSON_GC4023_OCT} DESTINATION ${ARMCB_GENERATED_OUT_DATA}/json_oct)
# install(FILES ${JSON_GC4023_OCT} DESTINATION ${ARMCB_GENERATED_OUT_JSON}/json_oct)

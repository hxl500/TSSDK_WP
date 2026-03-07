################################################libtuning_gc4653_normal######################################################
set(LIB_TUNING_GC4653_NORMAL tuning_gc4653_normal)

add_library(${LIB_TUNING_GC4653_NORMAL} 
    ${ARMCB_ISP_CUSTOM}/tuning/gc4653/tuning_gc4653_normal.c
    ${ARMCB_ISP_CUSTOM}/tuning/gc4653/tuning_gc4653_normal_ir.c
	${ARMCB_ISP_CUSTOM}/tuning/gc4653/tuning_gc4653_normal_wl.c
)

target_link_libraries (${LIB_TUNING_GC4653_NORMAL} isp_utils)
target_link_libraries (${LIB_TUNING_GC4653_NORMAL} json_convert)
################################################libcalibration_gc4653######################################################
set(LIB_CALIBRATION_GC4653 calibration_gc4653)

add_library ( ${LIB_CALIBRATION_GC4653} 
    ${ARMCB_ISP_CUSTOM}/tuning/gc4653/calibration_gc4653.c
    ${ARMCB_ISP_CUSTOM}/tuning/gc4653/calibration_gc4653_ir.c
)

target_link_libraries (${LIB_CALIBRATION_GC4653} isp_utils)
target_link_libraries (${LIB_CALIBRATION_GC4653} json_convert)
# ################################################libtuning_gc4653_dol2######################################################
#set(LIB_TUNING_GC4653_DOL2 tuning_gc4653_dol2)
#
#add_library ( ${LIB_TUNING_GC4653_DOL2} 
#    ${ARMCB_ISP_CUSTOM}/tuning/gc4653/tuning_gc4653_dol2.c
#)
#target_link_libraries (${LIB_TUNING_GC4653_DOL2} isp_utils)
#target_link_libraries (${LIB_TUNING_GC4653_DOL2} json_convert)
################################################ json install  ###################################################
# set (JSON_GC4653_OCT
#     ${ARMCB_ISP_CUSTOM}/tuning/gc4653/json_oct/415.cal
#     ${ARMCB_ISP_CUSTOM}/tuning/gc4653/json_oct/415_TS.cal
#     ${ARMCB_ISP_CUSTOM}/tuning/gc4653/json_oct/415_nor.tun
# )
#
# install(FILES ${JSON_GC4653_OCT} DESTINATION ${ARMCB_GENERATED_OUT_DATA}/json_oct)
# install(FILES ${JSON_GC4653_OCT} DESTINATION ${ARMCB_GENERATED_OUT_JSON}/json_oct)


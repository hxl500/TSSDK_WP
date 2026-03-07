################################################libtuning_gc5603_normal######################################################
set(LIB_TUNING_GC5603_NORMAL tuning_gc5603_normal)

add_library(${LIB_TUNING_GC5603_NORMAL} 
    ${ARMCB_ISP_CUSTOM}/tuning/gc5603/tuning_gc5603_normal.c
    ${ARMCB_ISP_CUSTOM}/tuning/gc5603/tuning_gc5603_normal_ir.c
	${ARMCB_ISP_CUSTOM}/tuning/gc5603/tuning_gc5603_normal_wl.c
)

target_link_libraries (${LIB_TUNING_GC5603_NORMAL} isp_utils)
target_link_libraries (${LIB_TUNING_GC5603_NORMAL} json_convert)
################################################libcalibration_gc5603######################################################
set(LIB_CALIBRATION_GC5603 calibration_gc5603)

add_library ( ${LIB_CALIBRATION_GC5603} 
    ${ARMCB_ISP_CUSTOM}/tuning/gc5603/calibration_gc5603.c
    ${ARMCB_ISP_CUSTOM}/tuning/gc5603/calibration_gc5603_ir.c
)

target_link_libraries (${LIB_CALIBRATION_GC5603} isp_utils)
target_link_libraries (${LIB_CALIBRATION_GC5603} json_convert)
# ################################################libtuning_gc5603_dol2######################################################
#set(LIB_TUNING_GC5603_DOL2 tuning_gc5603_dol2)
#
#add_library ( ${LIB_TUNING_GC5603_DOL2} 
#    ${ARMCB_ISP_CUSTOM}/tuning/gc5603/tuning_gc5603_dol2.c
#)
#target_link_libraries (${LIB_TUNING_GC5603_DOL2} isp_utils)
#target_link_libraries (${LIB_TUNING_GC5603_DOL2} json_convert)
################################################ json install  ###################################################
# set (JSON_GC5603_OCT
#     ${ARMCB_ISP_CUSTOM}/tuning/gc5603/json_oct/415.cal
#     ${ARMCB_ISP_CUSTOM}/tuning/gc5603/json_oct/415_TS.cal
#     ${ARMCB_ISP_CUSTOM}/tuning/gc5603/json_oct/415_nor.tun
# )
#
# install(FILES ${JSON_GC5603_OCT} DESTINATION ${ARMCB_GENERATED_OUT_DATA}/json_oct)
# install(FILES ${JSON_GC5603_OCT} DESTINATION ${ARMCB_GENERATED_OUT_JSON}/json_oct)


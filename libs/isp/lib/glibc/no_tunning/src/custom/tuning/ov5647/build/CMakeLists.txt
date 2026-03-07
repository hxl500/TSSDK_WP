################################################libtuning_ov5647_normal######################################################
set(LIB_TUNING_OV5647_NORMAL tuning_ov5647_normal)

add_library ( ${LIB_TUNING_OV5647_NORMAL}  
    ${ARMCB_ISP_CUSTOM}/tuning/ov5647/tuning_ov5647_normal.c
)

target_link_libraries (${LIB_TUNING_OV5647_NORMAL} isp_utils)
target_link_libraries (${LIB_TUNING_OV5647_NORMAL} json_convert)
################################################libcalibration_ov5647######################################################
#set(LIB_CALIBRATION_OV5647 calibration_ov5647)

#add_library ( ${LIB_CALIBRATION_OV5647}  
#    ${ARMCB_ISP_CUSTOM}/tuning/ov5647/calibration_ov5647.c
#)

#target_link_libraries (${LIB_CALIBRATION_OV5647} isp_utils)
#target_link_libraries (${LIB_CALIBRATION_OV5647} json_convert)

################################################ json install  ###################################################
set (JSON_OV5647
    #${ARMCB_ISP_CUSTOM}/tuning/ov5647/327.cal
    #${ARMCB_ISP_CUSTOM}/tuning/ov5647/327_TS.cal
    #${ARMCB_ISP_CUSTOM}/tuning/ov5647/327_nor.tun
    #${ARMCB_ISP_CUSTOM}/tuning/ov5647/327_dol2.tun
    #${ARMCB_ISP_CUSTOM}/tuning/ov5647/327_dol3.tun
)

install(FILES ${JSON_OV5647} DESTINATION ${ARMCB_GENERATED_OUT_DATA})
install(FILES ${JSON_OV5647} DESTINATION ${ARMCB_GENERATED_OUT_JSON})


################################################libtuning_imx327_normal######################################################
set(LIB_TUNING_IMX327_NORMAL tuning_imx327_normal)

add_library ( ${LIB_TUNING_IMX327_NORMAL}  
    ${ARMCB_ISP_CUSTOM}/tuning/imx327/tuning_imx327_normal.c
)

target_link_libraries (${LIB_TUNING_IMX327_NORMAL} isp_utils)
target_link_libraries (${LIB_TUNING_IMX327_NORMAL} json_convert)
################################################libtuning_imx327_dol2######################################################
# set(LIB_TUNING_IMX327_DOL2 tuning_imx327_dol2)
#
# add_library ( ${LIB_TUNING_IMX327_DOL2}  
#     ${ARMCB_ISP_CUSTOM}/tuning/imx327/tuning_imx327_dol2.c
# )
#
# target_link_libraries (${LIB_TUNING_IMX327_DOL2} isp_utils)
# target_link_libraries (${LIB_TUNING_IMX327_DOL2} json_convert)
################################################libtuning_imx327_dol3######################################################
# set(LIB_TUNING_IMX327_DOL3 tuning_imx327_dol3)
#
# add_library ( ${LIB_TUNING_IMX327_DOL3}  
#     ${ARMCB_ISP_CUSTOM}/tuning/imx327/tuning_imx327_dol3.c
# )
#
# target_link_libraries (${LIB_TUNING_IMX327_DOL3} isp_utils)
# target_link_libraries (${LIB_TUNING_IMX327_DOL3} json_convert)
################################################libcalibration_imx327######################################################
set(LIB_CALIBRATION_IMX327 calibration_imx327)

add_library ( ${LIB_CALIBRATION_IMX327}  
    ${ARMCB_ISP_CUSTOM}/tuning/imx327/calibration_imx327.c
)

target_link_libraries (${LIB_CALIBRATION_IMX327} isp_utils)
target_link_libraries (${LIB_CALIBRATION_IMX327} json_convert)

################################################ json install  ###################################################
set (JSON_IMX327_OCT
    ${ARMCB_ISP_CUSTOM}/tuning/imx327/json_oct/327.cal
    ${ARMCB_ISP_CUSTOM}/tuning/imx327/json_oct/327_TS.cal
    ${ARMCB_ISP_CUSTOM}/tuning/imx327/json_oct/327_nor.tun
    ${ARMCB_ISP_CUSTOM}/tuning/imx327/json_oct/327_dol2.tun
    ${ARMCB_ISP_CUSTOM}/tuning/imx327/json_oct/327_dol3.tun
)

set (JSON_IMX327_TIT
    ${ARMCB_ISP_CUSTOM}/tuning/imx327/json_tit/327.cal
    ${ARMCB_ISP_CUSTOM}/tuning/imx327/json_tit/327_nor.tun
    ${ARMCB_ISP_CUSTOM}/tuning/imx327/json_tit/327_dol2.tun
)

install(FILES ${JSON_IMX327_OCT} DESTINATION ${ARMCB_GENERATED_OUT_DATA}/json_oct)
install(FILES ${JSON_IMX327_OCT} DESTINATION ${ARMCB_GENERATED_OUT_JSON}/json_oct)

install(FILES ${JSON_IMX327_TIT} DESTINATION ${ARMCB_GENERATED_OUT_DATA}/json_tit)
install(FILES ${JSON_IMX327_TIT} DESTINATION ${ARMCB_GENERATED_OUT_JSON}/json_tit)


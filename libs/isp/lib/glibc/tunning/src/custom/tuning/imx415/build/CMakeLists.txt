################################################libtuning_imx415_normal######################################################
set(LIB_TUNING_IMX415_NORMAL tuning_imx415_normal)

add_library ( ${LIB_TUNING_IMX415_NORMAL}  
    ${ARMCB_ISP_CUSTOM}/tuning/imx415/tuning_imx415_normal.c
    ${ARMCB_ISP_CUSTOM}/tuning/imx415/tuning_imx415_normal_ir.c
    ${ARMCB_ISP_CUSTOM}/tuning/imx415/tuning_imx415_dual_linear.c
    ${ARMCB_ISP_CUSTOM}/tuning/imx415/tuning_imx415_dual_linear_ir.c
)

target_link_libraries (${LIB_TUNING_IMX415_NORMAL} isp_utils)
target_link_libraries (${LIB_TUNING_IMX415_NORMAL} json_convert)
################################################libcalibration_imx415######################################################
set(LIB_CALIBRATION_IMX415 calibration_imx415)

add_library ( ${LIB_CALIBRATION_IMX415}  
    ${ARMCB_ISP_CUSTOM}/tuning/imx415/calibration_imx415.c
    ${ARMCB_ISP_CUSTOM}/tuning/imx415/calibration_imx415_ir.c
    ${ARMCB_ISP_CUSTOM}/tuning/imx415/calibration_imx415_dual.c
    ${ARMCB_ISP_CUSTOM}/tuning/imx415/calibration_imx415_dual_ir.c
)

target_link_libraries (${LIB_CALIBRATION_IMX415} isp_utils)
target_link_libraries (${LIB_CALIBRATION_IMX415} json_convert)
################################################libtuning_imx415_dol2######################################################
set(LIB_TUNING_IMX415_DOL2 tuning_imx415_dol2)

add_library ( ${LIB_TUNING_IMX415_DOL2}  
    ${ARMCB_ISP_CUSTOM}/tuning/imx415/tuning_imx415_dol2.c
    ${ARMCB_ISP_CUSTOM}/tuning/imx415/tuning_imx415_dol2_ir.c
)

target_link_libraries (${LIB_TUNING_IMX415_DOL2} isp_utils)
target_link_libraries (${LIB_TUNING_IMX415_DOL2} json_convert)
################################################libtuning_imx415_dol3######################################################
#set(LIB_TUNING_IMX415_DOL3 tuning_imx415_dol3)
#
#add_library ( ${LIB_TUNING_IMX415_DOL3}  
#    ${ARMCB_ISP_CUSTOM}/tuning/imx415/tuning_imx415_dol3.c
#    ${ARMCB_ISP_CUSTOM}/tuning/imx415/tuning_imx415_dol3_ir.c
#)
#
#target_link_libraries (${LIB_TUNING_IMX415_DOL3} isp_utils)
#target_link_libraries (${LIB_TUNING_IMX415_DOL3} json_convert)
################################################ json install  ###################################################
# set (JSON_IMX415_OCT
#     ${ARMCB_ISP_CUSTOM}/tuning/imx415/json_oct/415.cal
#     ${ARMCB_ISP_CUSTOM}/tuning/imx415/json_oct/415_TS.cal
#     ${ARMCB_ISP_CUSTOM}/tuning/imx415/json_oct/415_nor.tun
# )
#
# install(FILES ${JSON_IMX415_OCT} DESTINATION ${ARMCB_GENERATED_OUT_DATA}/json_oct)
# install(FILES ${JSON_IMX415_OCT} DESTINATION ${ARMCB_GENERATED_OUT_JSON}/json_oct)

################################################libtuning_cv4002_normal######################################################
set(LIB_TUNING_CV4002_NORMAL tuning_cv4002_normal)

add_library ( ${LIB_TUNING_CV4002_NORMAL} 
    ${ARMCB_ISP_CUSTOM}/tuning/cv4002/tuning_cv4002_normal.c
    ${ARMCB_ISP_CUSTOM}/tuning/cv4002/tuning_cv4002_normal_ir.c
	${ARMCB_ISP_CUSTOM}/tuning/cv4002/tuning_cv4002_normal_wl.c
)

target_link_libraries (${LIB_TUNING_CV4002_NORMAL} isp_utils)
target_link_libraries (${LIB_TUNING_CV4002_NORMAL} json_convert)
################################################libcalibration_cv4002######################################################
set(LIB_CALIBRATION_CV4002 calibration_cv4002)

add_library ( ${LIB_CALIBRATION_CV4002} 
    ${ARMCB_ISP_CUSTOM}/tuning/cv4002/calibration_cv4002.c
    ${ARMCB_ISP_CUSTOM}/tuning/cv4002/calibration_cv4002_ir.c
	${ARMCB_ISP_CUSTOM}/tuning/cv4002/calibration_cv4002_wl.c
)

target_link_libraries (${LIB_CALIBRATION_CV4002} isp_utils)
target_link_libraries (${LIB_CALIBRATION_CV4002} json_convert)
# ################################################libtuning_cv4002_dol2######################################################
#set(LIB_TUNING_CV4002_DOL2 tuning_cv4002_dol2)
#
#add_library ( ${LIB_TUNING_CV4002_DOL2} ${ARMCB_LIB_FLAG}
#    ${ARMCB_ISP_CUSTOM}/tuning/cv4002/tuning_cv4002_dol2.c
#)
#target_link_libraries (${LIB_TUNING_CV4002_DOL2} isp_utils)
#target_link_libraries (${LIB_TUNING_CV4002_DOL2} json_convert)
################################################ json install  ###################################################
# set (JSON_CV4002_OCT
#     ${ARMCB_ISP_CUSTOM}/tuning/cv4002/json_oct/415.cal
#     ${ARMCB_ISP_CUSTOM}/tuning/cv4002/json_oct/415_TS.cal
#     ${ARMCB_ISP_CUSTOM}/tuning/cv4002/json_oct/415_nor.tun
# )
#
# install(FILES ${JSON_CV4002_OCT} DESTINATION ${ARMCB_GENERATED_OUT_DATA}/json_oct)
# install(FILES ${JSON_CV4002_OCT} DESTINATION ${ARMCB_GENERATED_OUT_JSON}/json_oct)
############################################### Install target ###############################################

################################################libtuning_cv2003_normal######################################################
set(LIB_TUNING_cv2003_NORMAL tuning_cv2003_normal)

add_library ( ${LIB_TUNING_cv2003_NORMAL}
    ${ARMCB_ISP_CUSTOM}/tuning/cv2003/tuning_cv2003_normal.c
    ${ARMCB_ISP_CUSTOM}/tuning/cv2003/tuning_cv2003_normal_ir.c
	${ARMCB_ISP_CUSTOM}/tuning/cv2003/tuning_cv2003_normal_wl.c
)

target_link_libraries (${LIB_TUNING_cv2003_NORMAL} isp_utils)
target_link_libraries (${LIB_TUNING_cv2003_NORMAL} json_convert)
################################################libcalibration_cv2003######################################################
set(LIB_CALIBRATION_cv2003 calibration_cv2003)

add_library ( ${LIB_CALIBRATION_cv2003}
    ${ARMCB_ISP_CUSTOM}/tuning/cv2003/calibration_cv2003.c
    ${ARMCB_ISP_CUSTOM}/tuning/cv2003/calibration_cv2003_ir.c
	${ARMCB_ISP_CUSTOM}/tuning/cv2003/calibration_cv2003_wl.c
)

target_link_libraries (${LIB_CALIBRATION_cv2003} isp_utils)
target_link_libraries (${LIB_CALIBRATION_cv2003} json_convert)
# ################################################libtuning_cv2003_dol2######################################################
#set(LIB_TUNING_cv2003_DOL2 tuning_cv2003_dol2)
#
#add_library ( ${LIB_TUNING_cv2003_DOL2} ${ARMCB_LIB_FLAG}
#    ${ARMCB_ISP_CUSTOM}/tuning/cv2003/tuning_cv2003_dol2.c
#)
#target_link_libraries (${LIB_TUNING_cv2003_DOL2} isp_utils)
#target_link_libraries (${LIB_TUNING_cv2003_DOL2} json_convert)
################################################ json install  ###################################################
# set (JSON_cv2003_OCT
#     ${ARMCB_ISP_CUSTOM}/tuning/cv2003/json_oct/415.cal
#     ${ARMCB_ISP_CUSTOM}/tuning/cv2003/json_oct/415_TS.cal
#     ${ARMCB_ISP_CUSTOM}/tuning/cv2003/json_oct/415_nor.tun
# )
#
# install(FILES ${JSON_cv2003_OCT} DESTINATION ${ARMCB_GENERATED_OUT_DATA}/json_oct)
# install(FILES ${JSON_cv2003_OCT} DESTINATION ${ARMCB_GENERATED_OUT_JSON}/json_oct)
############################################### Install target ###############################################

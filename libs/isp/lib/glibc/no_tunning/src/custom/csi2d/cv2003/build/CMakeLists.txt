################################################libcsi_cv2003######################################################
set(LIB_CSI2D_CV2003 csi2d_cv2003)

add_library ( ${LIB_CSI2D_CV2003}  
    ${ARMCB_ISP_CUSTOM}/csi2d/cv2003/cv2003_csi2d.c
    ${ARMCB_ISP_CUSTOM}/csi2d/cv2003/cv2003_aux1_csi2d.c
	#    ${ARMCB_ISP_CUSTOM}/csi2d/cv2003/cv2003_aux2_csi2d.c
)


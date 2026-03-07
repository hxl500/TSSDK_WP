################################################libcsi_gc2083######################################################
set(LIB_CSI2D_C3390 csi2d_c3390)

add_library ( ${LIB_CSI2D_C3390}
	${ARMCB_ISP_CUSTOM}/csi2d/c3390/c3390_csi2d.c
 	${ARMCB_ISP_CUSTOM}/csi2d/c3390/c3390_aux1_csi2d.c
)


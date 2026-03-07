################################################libcsi_hi556######################################################
set(LIB_CSI2D_HI556 csi2d_hi556)

add_library ( ${LIB_CSI2D_HI556}  
	${ARMCB_ISP_CUSTOM}/csi2d/hi556/hi556_csi2d.c
	${ARMCB_ISP_CUSTOM}/csi2d/hi556/hi556_aux1_csi2d.c
)


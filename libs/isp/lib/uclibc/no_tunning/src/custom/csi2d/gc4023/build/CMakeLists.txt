################################################libcsi_gc4023######################################################
set(LIB_CSI2D_GC4023 csi2d_gc4023)

add_library ( ${LIB_CSI2D_GC4023}  
	${ARMCB_ISP_CUSTOM}/csi2d/gc4023/gc4023_csi2d.c
	${ARMCB_ISP_CUSTOM}/csi2d/gc4023/gc4023_aux1_csi2d.c
)


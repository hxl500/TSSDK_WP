################################################libcsi_gc2083######################################################
set(LIB_CSI2D_GC2083 csi2d_gc2083)

add_library ( ${LIB_CSI2D_GC2083}
	${ARMCB_ISP_CUSTOM}/csi2d/gc2083/gc2083_csi2d.c
 	${ARMCB_ISP_CUSTOM}/csi2d/gc2083/gc2083_aux1_csi2d.c
)


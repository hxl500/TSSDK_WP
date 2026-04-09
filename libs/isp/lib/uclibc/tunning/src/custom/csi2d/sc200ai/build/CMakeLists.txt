################################################libcsi_sc200ai######################################################
set(LIB_CSI2D_SC200ai csi2d_sc200ai)

add_library ( ${LIB_CSI2D_SC200ai}  
	${ARMCB_ISP_CUSTOM}/csi2d/sc200ai/sc200ai_csi2d.c
	${ARMCB_ISP_CUSTOM}/csi2d/sc200ai/sc200ai_aux1_csi2d.c
)


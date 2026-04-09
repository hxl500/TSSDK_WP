################################################libcsi_sc2337p######################################################
set(LIB_CSI2D_SC2337P csi2d_sc2337p)

add_library ( ${LIB_CSI2D_SC2337P}  
	${ARMCB_ISP_CUSTOM}/csi2d/sc2337p/sc2337p_csi2d.c
	${ARMCB_ISP_CUSTOM}/csi2d/sc2337p/sc2337p_aux1_csi2d.c
)


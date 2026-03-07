################################################libcsi_sc2336p######################################################
set(LIB_CSI2D_SC2336P csi2d_sc2336p)

add_library ( ${LIB_CSI2D_SC2336P}  
	${ARMCB_ISP_CUSTOM}/csi2d/sc2336p/sc2336p_csi2d.c
	${ARMCB_ISP_CUSTOM}/csi2d/sc2336p/sc2336p_aux1_csi2d.c
)


################################################libcsi_imx415######################################################
set(LIB_CSI2D_IMX415 csi2d_imx415)

add_library ( ${LIB_CSI2D_IMX415}  
    ${ARMCB_ISP_CUSTOM}/csi2d/imx415/imx415_csi2d.c
    ${ARMCB_ISP_CUSTOM}/csi2d/imx415/imx415_aux1_csi2d.c
	#    ${ARMCB_ISP_CUSTOM}/csi2d/imx415/imx415_aux2_csi2d.c
)


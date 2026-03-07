################################################libcsi_imx327######################################################
set(LIB_CSI2D_IMX327 csi2d_imx327)

add_library ( ${LIB_CSI2D_IMX327}  
    ${ARMCB_ISP_CUSTOM}/csi2d/imx327/imx327_csi2d_536_434.c
    # ${ARMCB_ISP_CUSTOM}/csi2d/imx327/imx327_csi2d_536_414.c
    # ${ARMCB_ISP_CUSTOM}/csi2d/imx327/imx327_csi2d_536_212.c
)


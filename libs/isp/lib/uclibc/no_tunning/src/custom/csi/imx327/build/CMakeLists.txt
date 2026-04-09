################################################libcsi_imx327######################################################
set(LIB_CSI_IMX327 csi_imx327)

add_library ( ${LIB_CSI_IMX327}  
    ${ARMCB_ISP_CUSTOM}/csi/imx327/imx327_csi0.c
    ${ARMCB_ISP_CUSTOM}/csi/imx327/imx327_csi1.c
)


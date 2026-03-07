################################################libsensor_imx327######################################################
set(LIB_CSI_OV5647 csi_ov5647)

add_library ( ${LIB_CSI_OV5647}  
    ${ARMCB_ISP_CUSTOM}/csi/ov5647/ov5647_csi0.c
    ${ARMCB_ISP_CUSTOM}/csi/ov5647/ov5647_csi1.c
    ${ARMCB_ISP_CUSTOM}/csi/ov5647/ov5647_csi2.c
    ${ARMCB_ISP_CUSTOM}/csi/ov5647/ov5647_csi3.c
)


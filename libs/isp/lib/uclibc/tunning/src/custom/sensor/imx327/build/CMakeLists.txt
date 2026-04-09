################################################libsensor_imx327######################################################
set(LIB_SENSOR_IMX327 sensor_imx327)

add_library ( ${LIB_SENSOR_IMX327}  
    ${ARMCB_ISP_CUSTOM}/sensor/imx327/imx327_sensor.c
    ${ARMCB_ISP_CUSTOM}/sensor/imx327/imx327_aux1_sensor.c
)


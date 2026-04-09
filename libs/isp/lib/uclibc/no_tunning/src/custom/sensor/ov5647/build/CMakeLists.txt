################################################libsensor_imx327######################################################
set(LIB_SENSOR_OV5647 sensor_ov5647)

add_library ( ${LIB_SENSOR_OV5647}  
    ${ARMCB_ISP_CUSTOM}/sensor/ov5647/ov5647_sensor.c
)


################################################libsensor_hi556######################################################
set(LIB_SENSOR_HI556 sensor_hi556)

add_library ( ${LIB_SENSOR_HI556}  
    ${ARMCB_ISP_CUSTOM}/sensor/hi556/hi556_sensor.c
    ${ARMCB_ISP_CUSTOM}/sensor/hi556/hi556_aux1_sensor.c
)

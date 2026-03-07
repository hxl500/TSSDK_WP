################################################libsensor_gc2083######################################################
set(LIB_SENSOR_C3390 sensor_c3390)

add_library ( ${LIB_SENSOR_C3390}
    ${ARMCB_ISP_CUSTOM}/sensor/c3390/c3390_sensor.c
    ${ARMCB_ISP_CUSTOM}/sensor/c3390/c3390_aux1_sensor.c
)


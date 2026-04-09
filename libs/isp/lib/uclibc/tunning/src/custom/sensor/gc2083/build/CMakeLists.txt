################################################libsensor_gc2083######################################################
set(LIB_SENSOR_GC2083 sensor_gc2083)

add_library ( ${LIB_SENSOR_GC2083}
    ${ARMCB_ISP_CUSTOM}/sensor/gc2083/gc2083_sensor.c
    ${ARMCB_ISP_CUSTOM}/sensor/gc2083/gc2083_aux1_sensor.c
)


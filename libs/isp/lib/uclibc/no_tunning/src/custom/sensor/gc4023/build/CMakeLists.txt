################################################libsensor_gc4023######################################################
set(LIB_SENSOR_GC4023 sensor_gc4023)

add_library ( ${LIB_SENSOR_GC4023}  
    ${ARMCB_ISP_CUSTOM}/sensor/gc4023/gc4023_sensor.c
    ${ARMCB_ISP_CUSTOM}/sensor/gc4023/gc4023_aux1_sensor.c
)


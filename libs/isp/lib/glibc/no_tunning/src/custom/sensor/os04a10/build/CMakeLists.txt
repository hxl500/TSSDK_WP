################################################libsensor_os04a10######################################################
set(LIB_SENSOR_OS04A10 sensor_os04a10)

add_library ( ${LIB_SENSOR_OS04A10}
    ${ARMCB_ISP_CUSTOM}/sensor/os04a10/os04a10_sensor.c
    ${ARMCB_ISP_CUSTOM}/sensor/os04a10/os04a10_aux1_sensor.c
    ${ARMCB_ISP_CUSTOM}/sensor/os04a10/os04a10_aux2_sensor.c
)


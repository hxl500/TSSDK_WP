################################################libsensor_sc200ai######################################################
set(LIB_SENSOR_SC200ai sensor_sc200ai)

add_library ( ${LIB_SENSOR_SC200ai}  
    ${ARMCB_ISP_CUSTOM}/sensor/sc200ai/sc200ai_sensor.c
    ${ARMCB_ISP_CUSTOM}/sensor/sc200ai/sc200ai_aux1_sensor.c
)

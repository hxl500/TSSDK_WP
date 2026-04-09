################################################libsensor_sc2337p######################################################
set(LIB_SENSOR_SC2337P sensor_sc2337p)

add_library ( ${LIB_SENSOR_SC2337P}  
    ${ARMCB_ISP_CUSTOM}/sensor/sc2337p/sc2337p_sensor.c
    ${ARMCB_ISP_CUSTOM}/sensor/sc2337p/sc2337p_aux1_sensor.c
)

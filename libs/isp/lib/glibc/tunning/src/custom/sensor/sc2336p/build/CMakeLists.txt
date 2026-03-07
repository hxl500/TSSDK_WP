################################################libsensor_sc2336p######################################################
set(LIB_SENSOR_SC2336P sensor_sc2336p)

add_library ( ${LIB_SENSOR_SC2336P}  
    ${ARMCB_ISP_CUSTOM}/sensor/sc2336p/sc2336p_sensor.c
    ${ARMCB_ISP_CUSTOM}/sensor/sc2336p/sc2336p_aux1_sensor.c
)

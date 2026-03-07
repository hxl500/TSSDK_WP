################################################libsensor_SC430AI######################################################
set(LIB_SENSOR_SC430AI sensor_sc430ai)

add_library ( ${LIB_SENSOR_SC430AI} 
    ${ARMCB_ISP_CUSTOM}/sensor/sc430ai/sc430ai_sensor.c
)

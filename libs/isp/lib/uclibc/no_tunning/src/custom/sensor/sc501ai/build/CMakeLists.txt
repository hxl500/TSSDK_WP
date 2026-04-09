################################################libsensor_sc501ai######################################################
set(LIB_SENSOR_SC501AI sensor_sc501ai)

add_library ( ${LIB_SENSOR_SC501AI} 
    ${ARMCB_ISP_CUSTOM}/sensor/sc501ai/sc501ai_sensor.c
)

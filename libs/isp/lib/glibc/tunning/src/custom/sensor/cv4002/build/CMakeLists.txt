################################################libsensor_cv4002######################################################
set(LIB_SENSOR_CV4002 sensor_cv4002)

add_library ( ${LIB_SENSOR_CV4002}
    ${ARMCB_ISP_CUSTOM}/sensor/cv4002/cv4002_sensor.c
)

################################################libsensor_cv2003######################################################
set(LIB_SENSOR_CV2003 sensor_cv2003)

add_library ( ${LIB_SENSOR_CV2003}
    ${ARMCB_ISP_CUSTOM}/sensor/cv2003/cv2003_sensor.c
    ${ARMCB_ISP_CUSTOM}/sensor/cv2003/cv2003_aux1_sensor.c
    # ${ARMCB_ISP_CUSTOM}/sensor/cv2003/cv2003_aux2_sensor.c
)


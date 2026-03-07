################################################libsensor_imx415######################################################
set(LIB_SENSOR_IMX415 sensor_imx415)

add_library ( ${LIB_SENSOR_IMX415}
	#${ARMCB_ISP_CUSTOM}/sensor/imx415/imx415_536_sensor.c
    ${ARMCB_ISP_CUSTOM}/sensor/imx415/imx415_sensor.c
    ${ARMCB_ISP_CUSTOM}/sensor/imx415/imx415_aux1_sensor.c
)


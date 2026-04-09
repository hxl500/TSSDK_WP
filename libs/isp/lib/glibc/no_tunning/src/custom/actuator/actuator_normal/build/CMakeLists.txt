################################################libactuator_normal######################################################
set(LIB_ACTUATOR_NORMAL actuator_normal)

add_library ( ${LIB_ACTUATOR_NORMAL}  
    ${ARMCB_ISP_CUSTOM}/actuator/actuator_normal/actuator_normal.c
    ${ARMCB_ISP_CUSTOM}/actuator/actuator_normal/gpio-utils.c
)


#include "actuator_normal.h"
#if 0
#include "gpio-utils.h"

#if defined(TARGET_BOARD_PTZ)
#define GPIO_DEV "gpiochip1"
#define AIN_GPIO_NUM 10
#define BIN_GPIO_NUM 11
#elif defined(TARGET_BOARD_IPC)
#define GPIO_DEV "gpiochip1"
#define AIN_GPIO_NUM 10
#define BIN_GPIO_NUM 11
#elif defined(TARGET_BOARD_EVB88)
#define GPIO_DEV "gpiochip1"
#define AIN_GPIO_NUM 8
#define BIN_GPIO_NUM 7
#elif defined(TARGET_BOARD_EVB132)
#define GPIO_DEV "gpiochip1"
#define AIN_GPIO_NUM 25
#define BIN_GPIO_NUM 27
#elif defined(TARGET_BOARD_AOV)     /* use dt57evb132 debug, same to TARGET_BOARD_EVB132,need modify later*/
#define GPIO_DEV "gpiochip1"
#define AIN_GPIO_NUM 25
#define BIN_GPIO_NUM 27
#elif defined(TARGET_BOARD_AIO)
#define GPIO_DEV "gpiochip0"
#define AIN_GPIO_NUM 20
#define BIN_GPIO_NUM 19
#define AIN1_GPIO_NUM 22
#define BIN1_GPIO_NUM 21
#elif defined(TARGET_BOARD_FPGA)
#else
#error "no TARGET_BOARD* define"
#endif
#error "no TARGET_BOARD* define"
static u32 normal_ir_cut_process(actuator_config_params_t *pConfigParams, drv_regs_array_info_t *pRegInfo)
{
    SENSOR_CFG("pConfigParams->irCutEnable: %d", pConfigParams->irCutEnable);
#ifndef TARGET_BOARD_FPGA
    /* turn on ir cut*/
    if (pConfigParams->irCutEnable)
    {
        gpiotools_set(GPIO_DEV, AIN_GPIO_NUM, 0);
        gpiotools_set(GPIO_DEV, BIN_GPIO_NUM, 1);
    }
    else
    {
        gpiotools_set(GPIO_DEV, AIN_GPIO_NUM, 1);
        gpiotools_set(GPIO_DEV, BIN_GPIO_NUM, 0);
    }
#endif
    return 0;
}
#endif
/// Actuator driver parameters
static actuator_drv_params_t normal_actuator_params =
{
    .name      = "normal_actuator",
    .slaveInfo =
    {
        .hwType  = DRV_HW_MAX,
        .channel = 0,
    },
    .actuatorType = DRV_ACTUATOR_IR_CUT,

    .irCfgFunc    = NULL,

};

actuator_drv_params_t * load_normal_actuator_driver(void)
{
    return &normal_actuator_params;
}

#if 0//defined(TARGET_BOARD_AIO)
static u32 normal_aux1_ir_cut_process(actuator_config_params_t *pConfigParams, drv_regs_array_info_t *pRegInfo)
{
    SENSOR_CFG("pConfigParams->irCutEnable: %d", pConfigParams->irCutEnable);
    /* turn on ir cut*/
    if (pConfigParams->irCutEnable)
    {
        gpiotools_set(GPIO_DEV, AIN1_GPIO_NUM, 0);
        gpiotools_set(GPIO_DEV, BIN1_GPIO_NUM, 1);
    }
    else
    {
        gpiotools_set(GPIO_DEV, AIN1_GPIO_NUM, 1);
        gpiotools_set(GPIO_DEV, BIN1_GPIO_NUM, 0);
    }

    return 0;
}
#else
/// Actuator driver parameters
static actuator_drv_params_t normal_actuator_aux1_params =
{
    .name      = "normal_actuator_aux1",
    .slaveInfo =
    {
        .hwType  = DRV_HW_MAX,
        .channel = 1,
    },
    .actuatorType = DRV_ACTUATOR_IR_CUT,

    .irCfgFunc    = NULL,

};

actuator_drv_params_t * load_normal_actuator_aux1_driver(void)
{
    return &normal_actuator_aux1_params;
}
#endif

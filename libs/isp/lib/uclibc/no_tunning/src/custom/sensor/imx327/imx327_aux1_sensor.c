/************************************************************************************************
 * This source code is CONFIDENTIAL and any use by you is subject to the terms of the agreement
 * between you and Arm China or the terms of the agreement between you and the party authorised
 * by Arm China to disclose this document to you.
 *
 * The confidential and proprietary information contained in this file may only be used by a
 * person authorised under and to the extent permitted by a subsisting licensing agreement from
 * Arm China.
 *
 * This entire notice must be reproduced on all copies of this file and copies of this file may
 * only be made by a person if such person is permitted to do so under the terms of a subsisting
 * license agreement from Arm China.
 *
 * Release Information: Linlon-i5-r0p2-00eac0
 *
 * Copyright (C) 2021 Arm Technology (China) Co. Ltd. All rights reserved.
 *
 ***********************************************************************************************/
#include "imx327_aux1_sensor.h"

#include "imx327_sensor.h"

sensor_drv_params_t* Load_Imx327_Aux1_Sensor_Driver(void)
{
    memcpy(&imx327_aux1_sensor_params,
           &imx327_sensor_params,
           sizeof(sensor_drv_params_t));

    imx327_aux1_sensor_params.name              = "imx327_aux1";
    imx327_aux1_sensor_params.slaveInfo.channel = 1;

    imx327_aux1_sensor_params.powerUpSettings.powerSetting =
        imx327_aux1_power_up_setting;
    imx327_aux1_sensor_params.powerUpSettings.size =
        sizeof(imx327_aux1_power_up_setting) / sizeof(imx327_aux1_power_up_setting[0]);

    imx327_aux1_sensor_params.powerDownSettings.powerSetting =
        imx327_aux1_power_down_setting;
    imx327_aux1_sensor_params.powerDownSettings.size =
        sizeof(imx327_aux1_power_down_setting) / sizeof(imx327_aux1_power_down_setting[0]);

    return &imx327_aux1_sensor_params;
}

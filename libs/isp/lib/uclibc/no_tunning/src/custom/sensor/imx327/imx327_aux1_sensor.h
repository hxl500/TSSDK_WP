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

#ifndef __IMX327_AUX1_SENSOR_H__
#define __IMX327_AUX1_SENSOR_H__

#include "sensor_common.h"
#include "log_utils.h"


/// 0x83c40044 for Sensor1
static drv_power_setting_t imx327_aux1_power_up_setting[] = {
    {.type = DRV_POWER_REG, .regAddr = 0x83c40044, .bitMask  = 0x01, .bitVal = 0x00, .delayUs = 100000}, /// 100ms
    {.type = DRV_POWER_REG, .regAddr = 0x83c40044, .bitMask  = 0x01, .bitVal = 0x01, .delayUs = 100000}
};

/// 0x83c40044 for Sensor1
static drv_power_setting_t imx327_aux1_power_down_setting[] = {
    {.type = DRV_POWER_REG, .regAddr = 0x83c40044, .bitMask  = 0x01, .bitVal = 0x00, .delayUs = 100000},
    {.type = DRV_POWER_REG, .regAddr = 0x83c40044, .bitMask  = 0x01, .bitVal = 0x01, .delayUs = 100000}
};

static sensor_drv_params_t imx327_aux1_sensor_params;

sensor_drv_params_t* Load_Imx327_Aux1_Sensor_Driver(void);

#endif

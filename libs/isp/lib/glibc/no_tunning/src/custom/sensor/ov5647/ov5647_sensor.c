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

#include "ov5647_sensor.h"


/// This function is used to convert gain value to register value
static u32 OV5647_Gain_To_Reg(u32 expoAgain)
{
    u32 gainRegVal = 0;
    gainRegVal =  expoAgain >> 2;
    return gainRegVal;
}

static u32 OV5647_Reg_To_Gain(u32 regVal)
{
    u32 gain = 0;
    gain = regVal << 2;
    return gain;
}


static isp_ret_t OV5647_Normal_Fill_Exposure_Reg(sensor_apply_exp_param_t *pExpParam, drv_regs_array_info_t *pRegInfo)
{

    isp_ret_t ret = ISP_RET_SUCCESS;

    u32 gainRegValue = 0;

    pRegInfo->regAddrType = DRV_ADDR_TYPE_WORD;
    pRegInfo->regDataType = DRV_DATA_TYPE_BYTE;

    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x3501;
    pRegInfo->settings[pRegInfo->settingSize].regData = (pExpParam->expLineLong >> 8) & 0xFF;
    pRegInfo->settingSize ++;

    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x3502;
    pRegInfo->settings[pRegInfo->settingSize].regData = (pExpParam->expLineLong >> 0) & 0xFF;
    pRegInfo->settingSize ++;

    /// Step 2
    /// convert gain value to register value
    gainRegValue = OV5647_Gain_To_Reg(pExpParam->analogGain);
    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x300a;
    pRegInfo->settings[pRegInfo->settingSize].regData = gainRegValue >> 8;
    pRegInfo->settingSize ++;
    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x300a;
    pRegInfo->settings[pRegInfo->settingSize].regData = gainRegValue & 0xFF;
    pRegInfo->settingSize ++;

    /// Step 3
    /// Must update the real sensor gain to pExpParam
    pExpParam->analogGain = OV5647_Reg_To_Gain(gainRegValue);
    return ret;
}


isp_ret_t OV5647_Fill_Exposure_Reg(sensor_apply_exp_param_t *pExpParam, drv_regs_array_info_t *pRegInfo)
{
    isp_ret_t ret = ISP_RET_SUCCESS;

    if (pExpParam == NULL || pRegInfo == NULL)
    {
        SENSOR_ERR("Input param is NULL ! ");
        ret = ISP_RET_INVALID;
    }

    if (ret == ISP_RET_SUCCESS)
    {
        if (pExpParam->hdrMode == SENSOR_NORMAL_MODE)
        {
            ret = OV5647_Normal_Fill_Exposure_Reg(pExpParam, pRegInfo);
        }
        else
        {
            SENSOR_ERR("Sensor driver don't support this hdr mode %d now", pExpParam->hdrMode);
        }
    }

    return ret ;
}

sensor_drv_params_t* Load_Ov5647_Sensor0_Driver(void)
{
    ov5647_sensor_params.slaveInfo.channel = 0;
    ov5647_sensor_params.powerUpSettings.powerSetting = ov5647_sensor0_power_up_setting;
    ov5647_sensor_params.powerUpSettings.size = sizeof(ov5647_sensor0_power_up_setting) / sizeof(ov5647_sensor0_power_up_setting[0]);
    ov5647_sensor_params.powerDownSettings.powerSetting = ov5647_sensor0_power_down_setting;
    ov5647_sensor_params.powerDownSettings.size = sizeof(ov5647_sensor0_power_down_setting) / sizeof(ov5647_sensor0_power_down_setting[0]);
    return &ov5647_sensor_params;
}

sensor_drv_params_t* Load_Ov5647_Sensor1_Driver(void)
{
    ov5647_sensor_params.slaveInfo.channel = 1;
    ov5647_sensor_params.powerUpSettings.powerSetting = ov5647_sensor1_power_up_setting;
    ov5647_sensor_params.powerUpSettings.size = sizeof(ov5647_sensor1_power_up_setting) / sizeof(ov5647_sensor1_power_up_setting[0]);
    ov5647_sensor_params.powerDownSettings.powerSetting = ov5647_sensor1_power_down_setting;
    ov5647_sensor_params.powerDownSettings.size = sizeof(ov5647_sensor1_power_down_setting) / sizeof(ov5647_sensor1_power_down_setting[0]);
    return &ov5647_sensor_params;
}

sensor_drv_params_t* Load_Ov5647_Sensor2_Driver(void)
{
    ov5647_sensor_params.slaveInfo.channel = 2;
    ov5647_sensor_params.powerUpSettings.powerSetting = ov5647_sensor2_power_up_setting;
    ov5647_sensor_params.powerUpSettings.size = sizeof(ov5647_sensor2_power_up_setting) / sizeof(ov5647_sensor2_power_up_setting[0]);
    ov5647_sensor_params.powerDownSettings.powerSetting = ov5647_sensor2_power_down_setting;
    ov5647_sensor_params.powerDownSettings.size = sizeof(ov5647_sensor2_power_down_setting) / sizeof(ov5647_sensor2_power_down_setting[0]);
    return &ov5647_sensor_params;
}

sensor_drv_params_t* Load_Ov5647_Sensor3_Driver(void)
{
    ov5647_sensor_params.slaveInfo.channel = 3;
    ov5647_sensor_params.powerUpSettings.powerSetting = ov5647_sensor3_power_up_setting;
    ov5647_sensor_params.powerUpSettings.size = sizeof(ov5647_sensor3_power_up_setting) / sizeof(ov5647_sensor3_power_up_setting[0]);
    ov5647_sensor_params.powerDownSettings.powerSetting = ov5647_sensor3_power_down_setting;
    ov5647_sensor_params.powerDownSettings.size = sizeof(ov5647_sensor3_power_down_setting) / sizeof(ov5647_sensor3_power_down_setting[0]);
    return &ov5647_sensor_params;
}



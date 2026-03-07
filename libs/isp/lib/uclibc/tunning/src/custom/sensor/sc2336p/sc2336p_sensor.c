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

#include "sc2336p_sensor.h"

static int oldframelength = 2400;


static isp_ret_t SC2336P_Normal_Fill_Exposure_Reg(sensor_apply_exp_param_t *pExpParam, drv_regs_array_info_t *pRegInfo)
{
    isp_ret_t ret = ISP_RET_SUCCESS;
    // According to sc2336p datasheet

    if(pExpParam->analogGain<1024)
      pExpParam->analogGain=1024;

    if (oldframelength != pExpParam->frameLengthLines)
    {

        pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x320e; 
        pRegInfo->settings[pRegInfo->settingSize].regData = (pExpParam->frameLengthLines >> 8) & 0xFF;
        pRegInfo->settingSize ++;

        pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x320f;
        pRegInfo->settings[pRegInfo->settingSize].regData = pExpParam->frameLengthLines & 0xFF;
        pRegInfo->settingSize ++;

        oldframelength = pExpParam->frameLengthLines;

    }
    u64 texpLineLong=(pExpParam->expLineLong) << 4;

  	// Hold register
    pRegInfo->regAddrType = DRV_ADDR_TYPE_WORD;
    pRegInfo->regDataType = DRV_DATA_TYPE_BYTE;

    // Step 1
    //Update frame length and exposure lines to register

    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x3e00;
    pRegInfo->settings[pRegInfo->settingSize].regData = (texpLineLong >> 16) & 0x0F;
    pRegInfo->settingSize ++;

    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x3e01;
    pRegInfo->settings[pRegInfo->settingSize].regData = (texpLineLong >> 8) & 0xFF;
    pRegInfo->settingSize ++;

    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x3e02;
    pRegInfo->settings[pRegInfo->settingSize].regData = texpLineLong & 0xf0;
    pRegInfo->settingSize ++;


    /// Step 2
    /// convert gain value to register value
    //printf("frameLengthLines:%d  pExpParam->expLineLong:%d  \n",pExpParam->frameLengthLines,pExpParam->expLineLong);
    u64 gain_reg_0x3e09,gain_reg_0x3e06,gain_reg_0x3e07;
    u64 gain_factor = pExpParam->analogGain * 1000 / 1024;
    u64 final_gain = 1;
    if(gain_factor < 1000)
    {
        gain_factor = 1000;
    }
	if(gain_factor > 128*1000)
	{
	    gain_factor =128*1000;
	}

	if (gain_factor < 2000)
	{
		gain_reg_0x3e09 = 0x00;
		gain_reg_0x3e06 = 0x00;
		gain_reg_0x3e07 = (gain_factor * 128 / 1000 / 4) * 4 ;
		final_gain = gain_reg_0x3e07 * 8;
	}
	else if( gain_factor < 4000)
	{
		gain_reg_0x3e09 = 0x08;
		gain_reg_0x3e06 = 0x00;
		gain_reg_0x3e07 = (gain_factor * 128 / 2000 / 4) * 4 ;
		final_gain = 2 * gain_reg_0x3e07 * 8;
	}
	else if( gain_factor < 8000)
	{
		gain_reg_0x3e09 = 0x09;
		gain_reg_0x3e06 = 0x00;
		gain_reg_0x3e07 = (gain_factor * 128 / 4000 / 4) * 4 ;
		final_gain = 4 * gain_reg_0x3e07 * 8;
	}
	else if( gain_factor < 16000)
	{
		gain_reg_0x3e09 = 0x0b;
		gain_reg_0x3e06 = 0x00;
		gain_reg_0x3e07 = (gain_factor * 128 / 8000 / 4) * 4 ;
		final_gain = 8 * gain_reg_0x3e07 * 8;
	}
	else if( gain_factor < 32000)
	{
		gain_reg_0x3e09 = 0x0f;
		gain_reg_0x3e06 = 0x00;
		gain_reg_0x3e07 = (gain_factor * 128 / 16000 / 4) * 4 ;
		final_gain = 16 * gain_reg_0x3e07 * 8;
	}
	else if( gain_factor < 32000*2)//open dgain begin  max digital gain 4X
	{
		gain_reg_0x3e09 = 0x1f;
		gain_reg_0x3e06 = 0x00;
		gain_reg_0x3e07 = (gain_factor * 128 / 32000 / 4) * 4 ;
		final_gain = 32 * gain_reg_0x3e07 * 8;
	}
	else if( gain_factor < 32000*4)
	{
		gain_reg_0x3e09 = 0x1f;
		gain_reg_0x3e06 = 0x01;
		gain_reg_0x3e07 = (gain_factor * 128 / 32000 / 2 / 4) * 4 ;
		final_gain = 64 * gain_reg_0x3e07 * 8;
	}
	else
	{
		gain_reg_0x3e09 = 0x1f;
		gain_reg_0x3e06 = 0x03;
		gain_reg_0x3e07 = 0x80;
		final_gain = 128 * 1024;
	}

    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x3e06;
    pRegInfo->settings[pRegInfo->settingSize].regData = gain_reg_0x3e06;
    pRegInfo->settingSize ++;
    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x3e07;
    pRegInfo->settings[pRegInfo->settingSize].regData = gain_reg_0x3e07;
    pRegInfo->settingSize ++;
    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x3e09;
    pRegInfo->settings[pRegInfo->settingSize].regData = gain_reg_0x3e09;
    pRegInfo->settingSize ++;

	//Release register modification

    /// Step 3
    /// Must update the real sensor gain to pExpParam
    pExpParam->analogGain = final_gain;
    return ret;
}


isp_ret_t SC2336P_Fill_Exposure_Reg(sensor_apply_exp_param_t *pExpParam, drv_regs_array_info_t *pRegInfo)
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
            ret = SC2336P_Normal_Fill_Exposure_Reg(pExpParam, pRegInfo);
        }
        else
        {
            SENSOR_ERR("Sensor driver don't support this hdr mode %d now", pExpParam->hdrMode);
        }
    }

    return ret ;
}


sensor_drv_params_t* load_sc2336p_sensor_driver(void)
{
    return &sc2336p_sensor_params;
}

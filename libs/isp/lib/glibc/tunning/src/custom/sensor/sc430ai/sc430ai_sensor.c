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

#include "sc430ai_sensor.h"
static isp_ret_t sc430ai_Normal_Fill_Exposure_Reg(sensor_apply_exp_param_t *pExpParam, drv_regs_array_info_t *pRegInfo)
{
   isp_ret_t ret = ISP_RET_SUCCESS;
    pRegInfo->regAddrType = DRV_ADDR_TYPE_WORD;
    pRegInfo->regDataType = DRV_DATA_TYPE_BYTE;

    /// Step 1
    ///    Update frame length and exposure lines to register

    u32 half_lines = 0,vts = 0;
        half_lines = pExpParam->expLineLong *2;
    if(half_lines <= 2) half_lines=2;
    if (half_lines >  2 * pExpParam->frameLengthLines-10) {
       half_lines = 2 * pExpParam->frameLengthLines-10;
    }

    
    half_lines = half_lines<<4;
/*
    params->tExpo_reg[0].data = (half_lines>>16) & 0x0f;
    params->tExpo_reg[1].data =  (half_lines>>8) & 0xff;
    params->tExpo_reg[2].data = (half_lines>>0) & 0xf0;
    params->tVts_reg[0].data = (vts >> 8) & 0x00ff;
    params->tVts_reg[1].data = (vts >> 0) & 0x00ff;
*/
    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x320e;
    pRegInfo->settings[pRegInfo->settingSize].regData = (pExpParam->frameLengthLines >> 8) & 0xFF;
    pRegInfo->settingSize ++;

    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x320f; 
    pRegInfo->settings[pRegInfo->settingSize].regData = pExpParam->frameLengthLines & 0xFF;
    pRegInfo->settingSize ++;

    //pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x3024;
    //pRegInfo->settings[pRegInfo->settingSize].regData = pExpParam->frameLengthLines & 0xFF;
    //pRegInfo->settingSize ++;

    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x3e00;
    pRegInfo->settings[pRegInfo->settingSize].regData = (half_lines >> 16) & 0x0F;
    pRegInfo->settingSize ++;

    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x3e01;
    pRegInfo->settings[pRegInfo->settingSize].regData = (half_lines >> 8) & 0xFF;
    pRegInfo->settingSize ++;

    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x3e02;
    pRegInfo->settings[pRegInfo->settingSize].regData = half_lines & 0xF0;
    pRegInfo->settingSize ++;



//update_gain
    u32 gain = pExpParam->analogGain;
    u8 ANA_gain_reg = 0,DGN_gain_reg = 0, DGN_finegain_reg = 0;
	u32 DGN_gain = 0;
    
    if (gain < 2048) // again  2 * 1024
    {
        ANA_gain_reg = 0x00;
		DGN_gain = gain;
    }
    else if (gain <= 2611) // 2.55 * 1024
    {
        ANA_gain_reg = 0x01;
		DGN_gain = gain * 1024 / 2048;
    }
    else if (gain <= 5222) // 5.1 * 1024
    {
        ANA_gain_reg = 0x40;
		DGN_gain = gain * 1024 / 2611;
    }
    else if (gain <= 10445)// 10.2 * 1024
    {
        ANA_gain_reg = 0x41;
		DGN_gain = gain * 1024 / 5222;
    }
    else if (gain <= 20890)// 20.4 * 1024 // end again
    {
        ANA_gain_reg = 0x43;
		DGN_gain = gain * 1024 / 10445;
    }
    else if (gain <= 41779) // 40.8 * 1024
    {
        ANA_gain_reg = 0x47;
		DGN_gain = gain * 1024 / 20890;
    }
    else if (gain <= 83558) // 81.6 * 1024
    {
        ANA_gain_reg = 0x4F;
		DGN_gain = gain * 1024 / 41779;
    }
    else
	{
		ANA_gain_reg = 0x5F;
		DGN_gain = gain * 1024 / 83558;
	}

	if (DGN_gain < 2048)
	{
		DGN_gain_reg = 0x00;
		DGN_finegain_reg = DGN_gain / 8;
		DGN_finegain_reg = DGN_finegain_reg % 2 == 0 ? DGN_finegain_reg:DGN_finegain_reg+1;
	}
	else
	{
		DGN_gain_reg = 0x01;
		DGN_finegain_reg = DGN_gain / 16;
		DGN_finegain_reg = DGN_finegain_reg % 2 == 0 ? DGN_finegain_reg:DGN_finegain_reg+1;
	}
    

    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x3e09;
    pRegInfo->settings[pRegInfo->settingSize].regData = ANA_gain_reg;
    pRegInfo->settingSize ++;
    //pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x3e08;
    //pRegInfo->settings[pRegInfo->settingSize].regData = Coarse_gain_reg;
    //pRegInfo->settingSize ++;
    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x3e07;
    pRegInfo->settings[pRegInfo->settingSize].regData = DGN_finegain_reg;
    pRegInfo->settingSize ++;
    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x3e06;
    pRegInfo->settings[pRegInfo->settingSize].regData = DGN_gain_reg & 0xF;
    pRegInfo->settingSize ++;   
    
    /*
    params->tGain_reg[3].data = ANA_Fine_gain_reg;   // 0x3e09
    params->tGain_reg[2].data = Coarse_gain_reg;     // 0x3e08
    params->tGain_reg[1].data = DIG_Fine_gain_reg;   // 0x3e07
    params->tGain_reg[0].data = DIG_gain_reg & 0xF;  // 0x3e06
    */
// highTemp dpc 
    /*
     if (gain >= 30 * 1024) {
        params->tTemperature_reg_1[0].data = 0x07;
    } else if (gain <= 20 * 1024) {
        params->tTemperature_reg_1[0].data = 0x00;
    }*/
        
    /// Step 3
    /// Must update the real sensor gain to pExpParam
    pExpParam->analogGain = gain;
    return ret;
}


isp_ret_t sc430ai_Fill_Exposure_Reg(sensor_apply_exp_param_t *pExpParam, drv_regs_array_info_t *pRegInfo)
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
            ret = sc430ai_Normal_Fill_Exposure_Reg(pExpParam, pRegInfo);
        }
        else
        {
            SENSOR_ERR("Sensor driver don't support this hdr mode %d now", pExpParam->hdrMode);
        }
    }

    return ret ;
}


sensor_drv_params_t* load_sc430ai_sensor_driver(void)
{
    return &sc430ai_sensor_params;
}

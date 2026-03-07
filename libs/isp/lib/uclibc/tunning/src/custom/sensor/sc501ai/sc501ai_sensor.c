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

#include "sc501ai_sensor.h"
static isp_ret_t sc501ai_Normal_Fill_Exposure_Reg(sensor_apply_exp_param_t *pExpParam, drv_regs_array_info_t *pRegInfo)
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

    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x3024;
    pRegInfo->settings[pRegInfo->settingSize].regData = pExpParam->frameLengthLines & 0xFF;
    pRegInfo->settingSize ++;

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
    u8 i=0 , Coarse_gain = 1,DIG_gain=1;
    u32 Dcg_gainx100 = 1, ANA_Fine_gainx64 = 1,DIG_Fine_gainx1000 =1;
    u8 Dcg_gain_reg = 0,Coarse_gain_reg = 0,DIG_gain_reg=0, ANA_Fine_gain_reg= 0x20,DIG_Fine_gain_reg=0x80;
    
    if (gain < 1552) // start again  1.516 * 1024
    {
        Dcg_gainx100 = 1000;      Coarse_gain = 1;     DIG_gain=1;       DIG_Fine_gainx1000=1000;
        Dcg_gain_reg = 0;  Coarse_gain_reg = 0x03; DIG_gain_reg=0x0;  DIG_Fine_gain_reg=0x80;
    }
    else if (gain <= 3104) // 3.032 * 1024
    {
        Dcg_gainx100 = 1516;      Coarse_gain = 1;     DIG_gain=1;       DIG_Fine_gainx1000=1000;
        Dcg_gain_reg = 1;  Coarse_gain_reg = 0x23; DIG_gain_reg=0x0;  DIG_Fine_gain_reg=0x80;
    }
    else if (gain <= 6209) // 6.064 * 1024
    {
        Dcg_gainx100 = 1516;      Coarse_gain = 2;     DIG_gain=1;       DIG_Fine_gainx1000=1000;
        Dcg_gain_reg = 1;  Coarse_gain_reg = 0x27; DIG_gain_reg=0x0;  DIG_Fine_gain_reg=0x80;
    }
    else if (gain <= 12419)// 12.128 * 1024
    {
        Dcg_gainx100 = 1516;      Coarse_gain = 4;     DIG_gain=1;       DIG_Fine_gainx1000=1000;
        Dcg_gain_reg = 1;  Coarse_gain_reg = 0x2f; DIG_gain_reg=0x0;  DIG_Fine_gain_reg=0x80;
    }
    else if (gain <= 24644)// 24.067 * 1024 // end again
    {
        Dcg_gainx100 = 1516;      Coarse_gain = 8;     DIG_gain=1;       DIG_Fine_gainx1000=1000;
        Dcg_gain_reg = 1;  Coarse_gain_reg = 0x3f; DIG_gain_reg=0x0;  DIG_Fine_gain_reg=0x80;
    }
#if 1
    else if (gain <= 24644 * 2) // start dgain
    {
        Dcg_gainx100 = 1516;      Coarse_gain = 8;     DIG_gain=1;       ANA_Fine_gainx64=127;
        Dcg_gain_reg = 1;  Coarse_gain_reg = 0x3f; DIG_gain_reg=0x0;  ANA_Fine_gain_reg=0x7f;
    }
    else if (gain <= 24644 * 4)
    {
        Dcg_gainx100 = 1516;      Coarse_gain = 8;     DIG_gain=2;       ANA_Fine_gainx64=127;
        Dcg_gain_reg = 1;  Coarse_gain_reg = 0x3f; DIG_gain_reg=0x1;  ANA_Fine_gain_reg=0x7f;
    }
    else if (gain <= 24644 * 8)
    {
        Dcg_gainx100 = 1516;      Coarse_gain = 8;     DIG_gain=4;       ANA_Fine_gainx64=127;
        Dcg_gain_reg = 1;  Coarse_gain_reg = 0x3f; DIG_gain_reg=0x3;  ANA_Fine_gain_reg=0x7f;
    }
   else if (gain <= 24644 * 16)
    {
       Dcg_gainx100 = 1516;      Coarse_gain = 8;      DIG_gain=8;        ANA_Fine_gainx64=127;
       Dcg_gain_reg = 1;  Coarse_gain_reg = 0x3f; DIG_gain_reg=0x7;  ANA_Fine_gain_reg=0x7f;
    }
    else if (gain <= 782466)
    {
        Dcg_gainx100 = 1516;      Coarse_gain = 8;     DIG_gain=16;       ANA_Fine_gainx64=127;
        Dcg_gain_reg = 1;  Coarse_gain_reg = 0x3f; DIG_gain_reg=0xF;  ANA_Fine_gain_reg=0x7f;
    }
#endif

    if(gain == 1552){
        ANA_Fine_gain_reg = 0x40;
    }else if(gain <= 24644)
    {
        ANA_Fine_gain_reg = abs(1000 * gain / (Dcg_gainx100 * Coarse_gain) / 16);
    }else{
        DIG_Fine_gain_reg = abs(1000 * gain /(Dcg_gainx100 * DIG_gain) / ANA_Fine_gainx64);//*8000/coarse_gain = *1000
    }

    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x3e09;
    pRegInfo->settings[pRegInfo->settingSize].regData = ANA_Fine_gain_reg;
    pRegInfo->settingSize ++;
    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x3e08;
    pRegInfo->settings[pRegInfo->settingSize].regData = Coarse_gain_reg;
    pRegInfo->settingSize ++;
    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x3e07;
    pRegInfo->settings[pRegInfo->settingSize].regData = DIG_Fine_gain_reg;
    pRegInfo->settingSize ++;
    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x3e06;
    pRegInfo->settings[pRegInfo->settingSize].regData = DIG_gain_reg & 0xF;
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
     
     if (gain >= 30 * 1024) {
    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x5799;
    pRegInfo->settings[pRegInfo->settingSize].regData = 0x07;
    pRegInfo->settingSize ++; 
    } else if (gain <= 20 * 1024) {
    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x5799;
    pRegInfo->settings[pRegInfo->settingSize].regData = 0x00;
    pRegInfo->settingSize ++; 
    }
        
    /// Step 3
    /// Must update the real sensor gain to pExpParam
    pExpParam->analogGain = gain;
    return ret;
}


isp_ret_t sc501ai_Fill_Exposure_Reg(sensor_apply_exp_param_t *pExpParam, drv_regs_array_info_t *pRegInfo)
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
            ret = sc501ai_Normal_Fill_Exposure_Reg(pExpParam, pRegInfo);
        }
        else
        {
            SENSOR_ERR("Sensor driver don't support this hdr mode %d now", pExpParam->hdrMode);
        }
    }

    return ret ;
}


sensor_drv_params_t* load_sc501ai_sensor_driver(void)
{
    return &sc501ai_sensor_params;
}

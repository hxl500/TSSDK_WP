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

#include "os04a10_sensor.h"


/// This function is used to convert gain value to register value
static u32 OS04A10_Gain_To_Reg(u32 expoAgain)
{
  u32 gainRegVal = 0;
  u32 realAgain = expoAgain;
  u32 realDgain = 0;
	u32 AgainRegVal = 0;
	u32 DgainRegVal = 0;
	u32 Again_H = 0;
	u32 Again_L = 0;
	u32 Dgain_H = 0;
	u32 Dgain_M = 0;
	u32 Dgain_L = 0;
/* 
  if(realAgain>=16*1024)
    realAgain = 16*1024;
  else
  {
  	Again_H = realAgain >> 12; //Integer:expoAgain/1024
	  Again_L = (realAgain - Again_H * 1024) >> 6;//Fractional:{expoAgain-(Integer*1024)}/64
	  AgainRegVal = (Again_H << 4) + Again_L;   //bit[0:3]:Fractional   bit[4:8]:Interger
	  realAgain = (AgainRegVal>>4 & 0xFF)*1024+(AgainRegVal & 0xF)*64;
  }
	realDgain = (expoAgain/realAgain)*1024;
	Dgain_H = realDgain >> 12; //Integer:expoAgain/1024
	Dgain_M = (realDgain - Dgain_H * 1024)>>2 ; //Integer:expoAgain/1024
	Dgain_L = (realDgain - Dgain_H * 1024-Dgain_M * 4);//Fractional:{expoAgain-(Integer*1024)}/64
	DgainRegVal = (Dgain_H << 10) + (Dgain_M << 2) + Dgain_L;
*/
	if(expoAgain<=16*1024)
	{
		Again_H = expoAgain >> 12; //Integer:expoAgain/1024
		Again_L = (expoAgain - Again_H * 1024) >> 6;//Fractional:{expoAgain-(Integer*1024)}/64
		AgainRegVal = (Again_H << 4) + Again_L;   //bit[0:3]:Fractional   bit[4:8]:Interger
		DgainRegVal = 0x400;
	}
	else if(expoAgain<16*16*1024)
	{
		AgainRegVal = 0x0100;
		expoAgain=expoAgain/16;
		Dgain_H = expoAgain >> 12; //Integer:expoAgain/1024
		Dgain_M = (expoAgain - Dgain_H * 1024)>>2 ;; //Integer:expoAgain/1024
		Dgain_L = (expoAgain - Dgain_H * 1024-Dgain_M * 4);//Fractional:{expoAgain-(Integer*1024)}/64
    	DgainRegVal = (Dgain_H << 10) + (Dgain_M << 2) + Dgain_L;
	}
	gainRegVal = (AgainRegVal << 16) + DgainRegVal;
	//printf("Debug gainRegval:%x\n",gainRegVal);
    return gainRegVal;
}

static u32 OS04A10_Reg_To_Gain(u32 regVal)
{
    u32 gain = 0;
	u32 AgainReg = 0;
	u32 DgainReg = 0;
	u32 Again = 0;
	u32 Dgain = 0;
	AgainReg = regVal>>16 & 0xFFFF;
	DgainReg = regVal & 0xFFFF;
    Again = (AgainReg>>4 & 0xFF)*1024+(AgainReg & 0xF)*64;
	Dgain = (DgainReg>>10 & 0xFF)*1024+(DgainReg & 0x3ff);
	gain = (Again*Dgain)/1024;
  //printf("AgainReg:%x DgainReg:%x Again:%x Dgain:%x Debug gain:%d\n",AgainReg,DgainReg,Again,Dgain,gain);
    return gain;
}


static isp_ret_t OS04A10_Normal_Fill_Exposure_Reg(sensor_apply_exp_param_t *pExpParam, drv_regs_array_info_t *pRegInfo)
{

    isp_ret_t ret = ISP_RET_SUCCESS;

    u32 gainRegValue = 0;
    /* printf("exp:%d    Gain:%d\n",pExpParam->expLineLong,pExpParam->analogGain); */
    if(pExpParam->analogGain<1024)
      pExpParam->analogGain=1024;
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
    gainRegValue = OS04A10_Gain_To_Reg(pExpParam->analogGain);
    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x3508;
    pRegInfo->settings[pRegInfo->settingSize].regData = gainRegValue >> 20;
    //printf("0x3508_regData:%x\n",pRegInfo->settings[pRegInfo->settingSize].regData);
    pRegInfo->settingSize ++;
    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x3509;
    pRegInfo->settings[pRegInfo->settingSize].regData = ((gainRegValue >> 16 & 0xF)<<4);
    //printf("0x3509_regData:%x\n",pRegInfo->settings[pRegInfo->settingSize].regData);
    pRegInfo->settingSize ++;
	  pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x350a;
    pRegInfo->settings[pRegInfo->settingSize].regData = gainRegValue >> 10 & 0x1F ;
    //printf("0x350a_regData:%x\n",pRegInfo->settings[pRegInfo->settingSize].regData);
    pRegInfo->settingSize ++;
    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x350b;
    pRegInfo->settings[pRegInfo->settingSize].regData = gainRegValue >> 2 & 0xFF ;
    //printf("0x350b_regData:%x\n",pRegInfo->settings[pRegInfo->settingSize].regData);
    pRegInfo->settingSize ++;    
	  pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x350c;
    pRegInfo->settings[pRegInfo->settingSize].regData = ((gainRegValue & 0x3)<<6);
    //printf("0x350c_regData:%x\n",pRegInfo->settings[pRegInfo->settingSize].regData);
    pRegInfo->settingSize ++;
    

    /// Step 3
    /// Must update the real sensor gain to pExpParam
    pExpParam->analogGain = OS04A10_Reg_To_Gain(gainRegValue);
    return ret;
}

static isp_ret_t OS04A10_DOL2_Fill_Exposure_Reg(sensor_apply_exp_param_t *pExpParam, drv_regs_array_info_t *pRegInfo)
{

    return 0;
    u32 gainRegValue = 0;
    pRegInfo->regAddrType = DRV_ADDR_TYPE_WORD;
    pRegInfo->regDataType = DRV_DATA_TYPE_BYTE;
    pRegInfo->direction   = DRV_DIRECTION_WRITE;

    /// Hold register
    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x0100;
    pRegInfo->settings[pRegInfo->settingSize].regData = 0;
    pRegInfo->settingSize ++;
    /// Step 1.
    ///    1.1 update frame length
    ///    1.2 update long / mid exposure lines to register
    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x380e;
    pRegInfo->settings[pRegInfo->settingSize].regData = (pExpParam->frameLengthLines >> 8) & 0xFF;
    pRegInfo->settingSize ++;

    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x380f;
    pRegInfo->settings[pRegInfo->settingSize].regData = (pExpParam->frameLengthLines >> 0) & 0xFF;
    pRegInfo->settingSize ++;


    /// According to OS04A10 datasheet
    /// expLineLong = pExpParam->frameLengthLines * 2 -1) - SHS2RegVal

    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x3501;
    pRegInfo->settings[pRegInfo->settingSize].regData = (pExpParam->expLineLong >> 8) & 0xFF;
    pRegInfo->settingSize ++;

    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x3502;
    pRegInfo->settings[pRegInfo->settingSize].regData = (pExpParam->expLineLong >> 0) & 0xFF;
    pRegInfo->settingSize ++;

    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x3541;
    pRegInfo->settings[pRegInfo->settingSize].regData = (pExpParam->expLineMid >> 8) & 0xFF;
    pRegInfo->settingSize ++;

    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x3542;
    pRegInfo->settings[pRegInfo->settingSize].regData = (pExpParam->expLineMid >> 0) & 0xFF;
    pRegInfo->settingSize ++;
    /* Set sensor gain */
    gainRegValue = OS04A10_Gain_To_Reg(pExpParam->analogGain);
    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x3508;
    pRegInfo->settings[pRegInfo->settingSize].regData = gainRegValue >> 20;
    pRegInfo->settingSize ++;
    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x3509;
    pRegInfo->settings[pRegInfo->settingSize].regData = gainRegValue >> 16 & 0xF;
    pRegInfo->settingSize ++;
	pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x350a;
    pRegInfo->settings[pRegInfo->settingSize].regData = gainRegValue >> 10 & 0x1F ;
    pRegInfo->settingSize ++;
    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x350b;
    pRegInfo->settings[pRegInfo->settingSize].regData = gainRegValue >> 2 & 0xFF ;
    pRegInfo->settingSize ++;    
	pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x350c;
    pRegInfo->settings[pRegInfo->settingSize].regData = gainRegValue & 0x3;
    pRegInfo->settingSize ++;

   /* if (pExpParam->gainIndividualEn)
    {
        pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x3010;
        pRegInfo->settings[pRegInfo->settingSize].regData = 0x61;
        pRegInfo->settingSize ++;

        /// frame id 1
        gainRegValue = OS04A10_Gain_To_Reg(pExpParam->analogGain1);
        pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x30F2;
        pRegInfo->settings[pRegInfo->settingSize].regData = gainRegValue & 0xFF;
        pRegInfo->settingSize ++;
        pExpParam->analogGain1 = OS04A10_Reg_To_Gain(gainRegValue);
    }
    else
    {
        pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x3010;
        pRegInfo->settings[pRegInfo->settingSize].regData = 0x21;
        pRegInfo->settingSize ++;
    }
*/
    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x0100;    /// Hold register
    pRegInfo->settings[pRegInfo->settingSize].regData = 1 ;
    pRegInfo->settingSize ++;

    /// Step 3
    /// Must update the real sensor gain to pExpParam


   // printf(" OS04A10_DOL2_Fill_Integration_Time_Reg FrameLength 0x%x  SHS2RegVal 0x%x RHS1RegVal 0x%x SHS1RegVal 0x%x gainVal 0x%x \n", pExpParam->frameLengthLines , SHS2RegVal,/ RHS1RegVal, SHS1RegVal ,gainRegValue);
   // printf("FrameLength 0x%x L_exp:%d   s_exp:%d  RHS1RegVal 0x%x SHS1RegVal 0x%x gainVal 0x%x \n", pExpParam->frameLengthLines , pExpParam->expLineLong,pExpParam->expLineMid, RHS1RegVal, SHS1RegVal ,gainRegValue);

    return 0;
}
isp_ret_t OS04A10_Fill_Exposure_Reg(sensor_apply_exp_param_t *pExpParam, drv_regs_array_info_t *pRegInfo)
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
            ret = OS04A10_Normal_Fill_Exposure_Reg(pExpParam, pRegInfo);
        }
        else if(pExpParam->hdrMode == SENSOR_DOL2_MODE)
        {
            //ret = OS04A10_DOL2_Fill_Exposure_Reg(pExpParam, pRegInfo);
        }
        else
        {
            SENSOR_ERR("Sensor driver don't support this hdr mode %d now", pExpParam->hdrMode);
        }
    }

    return ret ;
}

sensor_drv_params_t* load_os04a10_sensor_driver(void)
{
    OS04A10_sensor_params.slaveInfo.channel = 0;
    OS04A10_sensor_params.powerUpSettings.powerSetting = OS04A10_sensor0_power_up_setting;
    OS04A10_sensor_params.powerUpSettings.size = sizeof(OS04A10_sensor0_power_up_setting) / sizeof(OS04A10_sensor0_power_up_setting[0]);
    OS04A10_sensor_params.powerDownSettings.powerSetting = OS04A10_sensor0_power_down_setting;
    OS04A10_sensor_params.powerDownSettings.size = sizeof(OS04A10_sensor0_power_down_setting) / sizeof(OS04A10_sensor0_power_down_setting[0]);
    
    return &OS04A10_sensor_params;
}

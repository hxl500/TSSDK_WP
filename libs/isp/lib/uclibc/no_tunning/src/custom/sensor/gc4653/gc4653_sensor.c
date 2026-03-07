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

#include "gc4653_sensor.h"

static u8 regValTable[26][7] = {   
	 //2b3    2b4    2b8    2b9    515    519    2d9
    {0x00,  0x00,  0x01,  0x00,  0x30,  0x1e,  0x5C},  //0-1x
	
    {0x20,  0x00,  0x01,  0x0B,  0x30,  0x1e,  0x5C},	
    {0x01,  0x00,  0x01,  0x19,  0x30,  0x1d,  0x5B},	
    {0x21,  0x00,  0x01,  0x2A,  0x30,  0x1e,  0x5C},	
    {0x02,  0x00,  0x02,  0x00,  0x30,  0x1e,  0x5C},  //4-2x
	
    {0x22,  0x00,  0x02,  0x17,  0x30,  0x1d,  0x5B},	
    {0x03,  0x00,  0x02,  0x33,  0x20,  0x16,  0x54},	
    {0x23,  0x00,  0x03,  0x14,  0x20,  0x17,  0x55},	
    {0x04,  0x00,  0x04,  0x00,  0x20,  0x17,  0x55},  //8-4x
	
    {0x24,  0x00,  0x04,  0x2F,  0x20,  0x19,  0x57},	
    {0x05,  0x00,  0x05,  0x26,  0x20,  0x19,  0x57},	
    {0x25,  0x00,  0x06,  0x28,  0x20,  0x1b,  0x59},	
    {0x0c,  0x00,  0x08,  0x00,  0x20,  0x1d,  0x5B},  //12-8x
	
    {0x2C,  0x00,  0x09,  0x1E,  0x20,  0x1f,  0x5D},	
    {0x0D,  0x00,  0x0B,  0x0C,  0x20,  0x21,  0x5F},	
    {0x2D,  0x00,  0x0D,  0x11,  0x20,  0x24,  0x62},	
    {0x1C,  0x00,  0x10,  0x00,  0x20,  0x26,  0x64},  //16-16x
    
    {0x3C,  0x00,  0x12,  0x3D,  0x18,  0x2a,  0x68},	
    {0x5C,  0x00,  0x16,  0x19,  0x18,  0x2c,  0x6A},	
    {0x7C,  0x00,  0x1A,  0x22,  0x18,  0x2e,  0x6C},	
    {0x9C,  0x00,  0x20,  0x00,  0x18,  0x32,  0x70},  //20-32x
	
    {0xBC,  0x00,  0x25,  0x3A,  0x18,  0x35,  0x73},	
    {0xDC,  0x00,  0x2C,  0x33,  0x10,  0x36,  0x74},	
    {0xFC,  0x00,  0x35,  0x05,  0x10,  0x38,  0x76},	
    {0x1C,  0x01,  0x40,  0x00,  0x10,  0x3c,  0x7A},  //24-64x
	
    {0x3C,  0x01,  0x4B,  0x35,  0x10,  0x42,  0x80},
};

static u32 gainLevelTable[27] = {	 
		64, //0-1x
    
	75,
	89,
	106,
	128, //4-2x
    
	151,
	179,
	212,
	256,  //8-4x
    
	303,
	358,
	424,
	512,  //12-8x
    
	606,
	716,
	849,
	1024,  //16-16x
    
	1213,  //17-18.95x
	1433,  //18-22.39x
	1698,
	2048,  //20-32x
    
	2426,
	2867,
	3397,
	4096,  //24-64x
    
	4853,
	 0xffffffff,
};
/// This function is used to convert gain value to register value
static 	int total = sizeof(gainLevelTable) / sizeof(u32);



static isp_ret_t GC4653_Normal_Fill_Exposure_Reg(sensor_apply_exp_param_t *pExpParam, drv_regs_array_info_t *pRegInfo)
{
    isp_ret_t ret = ISP_RET_SUCCESS;
    // According to gc4653 datasheet
    //     1. 0x202/0x203          means exposure time
    //     2. 0x340/0x341(VMAX)   means the number of lines per frame(FrameLength)

    u32 gainRegValue = 0;
 /*   if(pExpParam->expLineLong>1400)
      pExpParam->expLineLong=1400;*/
    if(pExpParam->analogGain<1024)
      pExpParam->analogGain=1024;
     
    //  
    pExpParam->expLineLong=pExpParam->expLineLong-1;
    
  	// Hold register
    pRegInfo->regAddrType = DRV_ADDR_TYPE_WORD;
    pRegInfo->regDataType = DRV_DATA_TYPE_BYTE;

    // Step 1
    //Update frame length and exposure lines to register
    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x340; 
    pRegInfo->settings[pRegInfo->settingSize].regData = (pExpParam->frameLengthLines >> 8) & 0x3F;
    pRegInfo->settingSize ++;

    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x341;
    pRegInfo->settings[pRegInfo->settingSize].regData = pExpParam->frameLengthLines & 0xFF;
    pRegInfo->settingSize ++;

    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x202;
    pRegInfo->settings[pRegInfo->settingSize].regData = (pExpParam->expLineLong >> 8) & 0x3F;
    pRegInfo->settingSize ++;

    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x203;
    pRegInfo->settings[pRegInfo->settingSize].regData = pExpParam->expLineLong & 0xFF;
    pRegInfo->settingSize ++;

    /// Step 2
    /// convert gain value to register value
    //printf("frameLengthLines:%d  pExpParam->expLineLong:%d  \n",pExpParam->frameLengthLines,pExpParam->expLineLong);
   int i;
   u32 gain = pExpParam->analogGain/16;
	 u32 tol_dig_gain = 0;
    
	 for(i = 0; i < total; i++)
	 {
	   if((gainLevelTable[i] <= gain)&&(gain < gainLevelTable[i+1]))
	     break;
	 }
   // printf("i=%d\n",i);
	  tol_dig_gain = gain*64/gainLevelTable[i];
   // printf("===pExpParam->analogGain:%d==gain:%d==tol_dig_gain:%d=====gainLevelTable:%d\n",pExpParam->analogGain,gain,tol_dig_gain,gainLevelTable[i]);
    //printf("REg:[i][0]=%d,[i][1]=%d,[i][2]=%d,[i][3]=%d,[i][4]=%d,[i][5]=%d,[i][6]=%d,tol_dig_gain=%d\n",regValTable[i][0],regValTable[i][1],regValTable[i][2],regValTable[i][3],regValTable[i][4],regValTable[i][5],regValTable[i][6],tol_dig_gain);
    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x2b3;
    pRegInfo->settings[pRegInfo->settingSize].regData = regValTable[i][0];
    pRegInfo->settingSize ++;
    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x2b4;
    pRegInfo->settings[pRegInfo->settingSize].regData = regValTable[i][1];
    pRegInfo->settingSize ++;
    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x2b8;
    pRegInfo->settings[pRegInfo->settingSize].regData = regValTable[i][2];
    pRegInfo->settingSize ++;
    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x2b9;
    pRegInfo->settings[pRegInfo->settingSize].regData = regValTable[i][3];
    pRegInfo->settingSize ++;
    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x515;
    pRegInfo->settings[pRegInfo->settingSize].regData = regValTable[i][4];
    pRegInfo->settingSize ++;
    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x519;
    pRegInfo->settings[pRegInfo->settingSize].regData = regValTable[i][5];
    pRegInfo->settingSize ++;
    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x2d9;
    pRegInfo->settings[pRegInfo->settingSize].regData = regValTable[i][6];
    pRegInfo->settingSize ++;
    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x20e;
    pRegInfo->settings[pRegInfo->settingSize].regData =  (tol_dig_gain>>6);
    pRegInfo->settingSize ++;
    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x20f;
    pRegInfo->settings[pRegInfo->settingSize].regData = ((tol_dig_gain&0x3f)<<2);
    pRegInfo->settingSize ++;
	//Release register modification

    /// Step 3
    /// Must update the real sensor gain to pExpParam
    pExpParam->analogGain = gain*16;
    return ret;
}


isp_ret_t GC4653_Fill_Exposure_Reg(sensor_apply_exp_param_t *pExpParam, drv_regs_array_info_t *pRegInfo)
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
            ret = GC4653_Normal_Fill_Exposure_Reg(pExpParam, pRegInfo);
        }
        else
        {
            SENSOR_ERR("Sensor driver don't support this hdr mode %d now", pExpParam->hdrMode);
        }
    }

    return ret ;
}


sensor_drv_params_t* load_gc4653_sensor_driver(void)
{
    return &gc4653_sensor_params;
}

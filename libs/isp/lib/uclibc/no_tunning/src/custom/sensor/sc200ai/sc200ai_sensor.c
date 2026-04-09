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

#include "sc200ai_sensor.h"

static u8 regValTable[25][8] = {   
	 //0x0614 0x0615 0x0218 0x1467 0x1468 0x00b8 0x00b9 
	{0x00, 0x00, 0x00, 0x0D, 0x15, 0x01, 0x00},
	{0x80, 0x02, 0x00, 0x0D, 0x15, 0x01, 0x0B},
	{0x01, 0x00, 0x00, 0x0D, 0x15, 0x01, 0x19},
	{0x81, 0x02, 0x00, 0x0E, 0x16, 0x01, 0x2A},
	{0x02, 0x00, 0x00, 0x0E, 0x16, 0x02, 0x00},
	{0x82, 0x02, 0x00, 0x0F, 0x17, 0x02, 0x17},
	{0x03, 0x00, 0x00, 0x10, 0x18, 0x02, 0x33},
	{0x83, 0x02, 0x00, 0x11, 0x19, 0x03, 0x14},
	{0x04, 0x00, 0x00, 0x12, 0x1a, 0x04, 0x00},
	{0x80, 0x02, 0x20, 0x13, 0x1b, 0x04, 0x2F},
	{0x01, 0x00, 0x20, 0x14, 0x1c, 0x05, 0x26},
	{0x81, 0x02, 0x20, 0x15, 0x1d, 0x06, 0x28},
	{0x02, 0x00, 0x20, 0x16, 0x1e, 0x08, 0x00},
	{0x82, 0x02, 0x20, 0x16, 0x1e, 0x09, 0x1E},
	{0x03, 0x00, 0x20, 0x18, 0x20, 0x0B, 0x0C},
	{0x83, 0x02, 0x20, 0x18, 0x20, 0x0D, 0x11},
	{0x04, 0x00, 0x20, 0x18, 0x20, 0x10, 0x00},
	{0x84, 0x02, 0x20, 0x19, 0x21, 0x12, 0x3D},
	{0x05, 0x00, 0x20, 0x19, 0x21, 0x16, 0x19},
	{0x85, 0x02, 0x20, 0x1A, 0x22, 0x1A, 0x22},
	{0xb5, 0x04, 0x20, 0x1B, 0x23, 0x20, 0x00},
	{0x85, 0x05, 0x20, 0x1B, 0x23, 0x25, 0x3A},
	{0x05, 0x08, 0x20, 0x1C, 0x24, 0x2C, 0x33},
	{0x45, 0x09, 0x20, 0x1D, 0x25, 0x35, 0x05},
	{0x55, 0x0a, 0x20, 0x1F, 0x27, 0x40, 0x00},
};

static u32 gainLevelTable[26] = {	 
		64,	
		76,	
		90,	
		106,	
		128,	
		152,	
		179,	
		212,	
		256,	
		303,	
		358,	
		425,	
		512,	
		607,	
		717,	
		849,	
		1024,
		1213,
		1434,
		1699,
		2048,
		2427,
		2867,
		3398,
		4096,	
		0xffffffff,
   };
/// This function is used to convert gain value to register value
static 	int total = sizeof(gainLevelTable) / sizeof(u32);



static isp_ret_t SC200ai_Normal_Fill_Exposure_Reg(sensor_apply_exp_param_t *pExpParam, drv_regs_array_info_t *pRegInfo)
{
    isp_ret_t ret = ISP_RET_SUCCESS;
    // According to sc200ai datasheet
    //     1. 0x202/0x203          means exposure time
    //     2. 0x340/0x341(VMAX)   means the number of lines per frame(FrameLength)
	printf("ae\n");

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
   // pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x340; 
   // pRegInfo->settings[pRegInfo->settingSize].regData = (pExpParam->frameLengthLines >> 8) & 0x3F;
   // pRegInfo->settingsize ++;

   // pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x341;
   // pRegInfo->settings[pRegInfo->settingSize].regData = pExpParam->frameLengthLines & 0xFF;
   // pRegInfo->settingsize ++;

   // pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x202;
   // pRegInfo->settings[pRegInfo->settingSize].regData = (pExpParam->expLineLong >> 8) & 0x3F;
   // pRegInfo->settingsize ++;

   // pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x203;
   // pRegInfo->settings[pRegInfo->settingSize].regData = pExpParam->expLineLong & 0xFF;
   // pRegInfo->settingsize ++;

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
   // pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x614;
   // pRegInfo->settings[pRegInfo->settingSize].regData = regValTable[i][0];
   // pRegInfo->settingsize ++;
   // pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x615;
   // pRegInfo->settings[pRegInfo->settingSize].regData = regValTable[i][1];
   // pRegInfo->settingsize ++;
   // pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x218;
   // pRegInfo->settings[pRegInfo->settingSize].regData = regValTable[i][2];
   // pRegInfo->settingsize ++;
   // pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x1467;
   // pRegInfo->settings[pRegInfo->settingSize].regData = regValTable[i][3];
   // pRegInfo->settingsize ++;
   // pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x1468;
   // pRegInfo->settings[pRegInfo->settingSize].regData = regValTable[i][4];
   // pRegInfo->settingsize ++;
   // pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x00b8;
   // pRegInfo->settings[pRegInfo->settingSize].regData = regValTable[i][5];
   // pRegInfo->settingsize ++;
   // pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x00b9;
   // pRegInfo->settings[pRegInfo->settingSize].regData = regValTable[i][6];
   // pRegInfo->settingsize ++;
   // pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x0064;
   // pRegInfo->settings[pRegInfo->settingSize].regData =  (tol_dig_gain>>6);
   // pRegInfo->settingsize ++;
   // pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x0065;
   // pRegInfo->settings[pRegInfo->settingSize].regData = ((tol_dig_gain&0x3f)<<2);
   // pRegInfo->settingsize ++;
	//Release register modification

    /// Step 3
    /// Must update the real sensor gain to pExpParam
    pExpParam->analogGain = gain*16;
    return ret;
}


isp_ret_t SC200ai_Fill_Exposure_Reg(sensor_apply_exp_param_t *pExpParam, drv_regs_array_info_t *pRegInfo)
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
            //ret = SC200ai_Normal_Fill_Exposure_Reg(pExpParam, pRegInfo);
        }
        else
        {
            SENSOR_ERR("Sensor driver don't support this hdr mode %d now", pExpParam->hdrMode);
        }
    }

    return ret ;
}


sensor_drv_params_t* load_sc200ai_sensor_driver(void)
{
    return &sc200ai_sensor_params;
}

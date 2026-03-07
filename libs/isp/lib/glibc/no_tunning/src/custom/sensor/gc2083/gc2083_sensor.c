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

#include "gc2083_sensor.h"

static u8 regValTable[29][13] = {    
 //0x00d0  0x0dc1  0x00b8  0x00b9  0x0155  0x0410  0x0411  0x0412  0x0413  0x0414  0x0415  0x0416  0x0417
	{0x00, 0x00, 0x01, 0x00, 0x03, 0x11, 0x11, 0x11, 0x11, 0x6f, 0x6f, 0x6f, 0x6f},
	{0x10, 0x00, 0x01, 0x0c, 0x03, 0x11, 0x11, 0x11, 0x11, 0x6f, 0x6f, 0x6f, 0x6f},
	{0x01, 0x00, 0x01, 0x1a, 0x03, 0x11, 0x11, 0x11, 0x11, 0x6f, 0x6f, 0x6f, 0x6f},
	{0x11, 0x00, 0x01, 0x2b, 0x03, 0x11, 0x11, 0x11, 0x11, 0x6f, 0x6f, 0x6f, 0x6f},
	{0x02, 0x00, 0x02, 0x00, 0x03, 0x11, 0x11, 0x11, 0x11, 0x6f, 0x6f, 0x6f, 0x6f},
	{0x12, 0x00, 0x02, 0x18, 0x03, 0x11, 0x11, 0x11, 0x11, 0x6f, 0x6f, 0x6f, 0x6f},
	{0x03, 0x00, 0x02, 0x33, 0x03, 0x11, 0x11, 0x11, 0x11, 0x6f, 0x6f, 0x6f, 0x6f},
	{0x13, 0x00, 0x03, 0x15, 0x03, 0x11, 0x11, 0x11, 0x11, 0x6f, 0x6f, 0x6f, 0x6f},
	{0x04, 0x00, 0x04, 0x00, 0x03, 0x11, 0x11, 0x11, 0x11, 0x6f, 0x6f, 0x6f, 0x6f},
	{0x14, 0x00, 0x04, 0xe0, 0x03, 0x11, 0x11, 0x11, 0x11, 0x6f, 0x6f, 0x6f, 0x6f},
	{0x05, 0x00, 0x05, 0x26, 0x03, 0x11, 0x11, 0x11, 0x11, 0x6f, 0x6f, 0x6f, 0x6f},
	{0x15, 0x00, 0x06, 0x2b, 0x03, 0x11, 0x11, 0x11, 0x11, 0x6f, 0x6f, 0x6f, 0x6f},
	{0x44, 0x00, 0x08, 0x00, 0x03, 0x11, 0x11, 0x11, 0x11, 0x6f, 0x6f, 0x6f, 0x6f},
	{0x54, 0x00, 0x09, 0x22, 0x03, 0x11, 0x11, 0x11, 0x11, 0x6f, 0x6f, 0x6f, 0x6f},
	{0x45, 0x00, 0x0b, 0x0d, 0x03, 0x11, 0x11, 0x11, 0x11, 0x6f, 0x6f, 0x6f, 0x6f},	
	{0x55, 0x00, 0x0d, 0x16, 0x03, 0x11, 0x11, 0x11, 0x11, 0x6f, 0x6f, 0x6f, 0x6f},
	{0x04, 0x01, 0x10, 0x00, 0x19, 0x16, 0x16, 0x16, 0x16, 0x6f, 0x6f, 0x6f, 0x6f},
	{0x14, 0x01, 0x13, 0x04, 0x19, 0x16, 0x16, 0x16, 0x16, 0x6f, 0x6f, 0x6f, 0x6f},
	{0x24, 0x01, 0x16, 0x1a, 0x19, 0x16, 0x16, 0x16, 0x16, 0x6f, 0x6f, 0x6f, 0x6f},
	{0x34, 0x01, 0x1a, 0x2b, 0x19, 0x16, 0x16, 0x16, 0x16, 0x6f, 0x6f, 0x6f, 0x6f},
	{0x44, 0x01, 0x20, 0x00, 0x36, 0x18, 0x18, 0x18, 0x18, 0x6f, 0x6f, 0x6f, 0x6f},
	{0x54, 0x01, 0x26, 0x07, 0x36, 0x18, 0x18, 0x18, 0x18, 0x6f, 0x6f, 0x6f, 0x6f},
	{0x64, 0x01, 0x2c, 0x33, 0x36, 0x18, 0x18, 0x18, 0x18, 0x6f, 0x6f, 0x6f, 0x6f},
	{0x74, 0x01, 0x35, 0x17, 0x36, 0x18, 0x18, 0x18, 0x18, 0x6f, 0x6f, 0x6f, 0x6f},
	{0x84, 0x01, 0x35, 0x17, 0x64, 0x16, 0x16, 0x16, 0x16, 0x72, 0x72, 0x72, 0x72},
	{0x94, 0x01, 0x35, 0x17, 0x64, 0x16, 0x16, 0x16, 0x16, 0x72, 0x72, 0x72, 0x72},
	{0x85, 0x01, 0x35, 0x17, 0x64, 0x16, 0x16, 0x16, 0x16, 0x72, 0x72, 0x72, 0x72},
	{0x95, 0x01, 0x35, 0x17, 0x64, 0x16, 0x16, 0x16, 0x16, 0x72, 0x72, 0x72, 0x72},
	{0xa5, 0x01, 0x35, 0x17, 0x64, 0x16, 0x16, 0x16, 0x16, 0x72, 0x72, 0x72, 0x72},
};


static u32 gainLevelTable[30] = {
							  64, 
							  77, 
							  92, 
							  110,
							  128,
							  154,
							  186,
							  223,
							  269,
							  323,
							  381,
							  457,
							  544,
							  653,
							  762,
							  914,
							  1078,
							  1293,
							  1541,
							  1849,
							  2177,
							  2612,
							  3136,
							  3764,
							  4710, 			  
							  5652,
							  6656,
							  7988,
							  9474, 						  
							 0xffffffff
						  };

/// This function is used to convert gain value to register value
static 	int total = sizeof(gainLevelTable) / sizeof(u32);

static isp_ret_t GC2083_Normal_Fill_Exposure_Reg(sensor_apply_exp_param_t *pExpParam, drv_regs_array_info_t *pRegInfo)
{
    isp_ret_t ret = ISP_RET_SUCCESS;
    // According to gc4023 datasheet
    //     1. 0x202/0x203          means exposure time
    //     2. 0x340/0x341(VMAX)   means the number of lines per frame(FrameLength)
    static int i_count=0;
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
    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0xd41; 
    pRegInfo->settings[pRegInfo->settingSize].regData = (pExpParam->frameLengthLines >> 8) & 0x3F;
    pRegInfo->settingSize ++;

    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0xd42;
    pRegInfo->settings[pRegInfo->settingSize].regData = pExpParam->frameLengthLines & 0xFF;
    pRegInfo->settingSize ++;

    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0xd03;
    pRegInfo->settings[pRegInfo->settingSize].regData = (pExpParam->expLineLong >> 8) & 0x3F;
    pRegInfo->settingSize ++;

    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0xd04;
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
    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x00d0;
    pRegInfo->settings[pRegInfo->settingSize].regData = regValTable[i][0];
    pRegInfo->settingSize ++;

	pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x031d;
	pRegInfo->settings[pRegInfo->settingSize].regData = 0x2e;
	pRegInfo->settingSize ++;

    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x0dc1;
    pRegInfo->settings[pRegInfo->settingSize].regData = regValTable[i][1];
    pRegInfo->settingSize ++;

	pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x031d;
	pRegInfo->settings[pRegInfo->settingSize].regData = 0x28;
	pRegInfo->settingSize ++;
	
    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x00b8;
    pRegInfo->settings[pRegInfo->settingSize].regData = regValTable[i][2];
    pRegInfo->settingSize ++;
	
    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x00b9;
    pRegInfo->settings[pRegInfo->settingSize].regData = regValTable[i][3];
    pRegInfo->settingSize ++;
	
    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x0155;
    pRegInfo->settings[pRegInfo->settingSize].regData = regValTable[i][4];
    pRegInfo->settingSize ++;
	
    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x0410;
    pRegInfo->settings[pRegInfo->settingSize].regData = regValTable[i][5];
    pRegInfo->settingSize ++;
	
    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x0411;
    pRegInfo->settings[pRegInfo->settingSize].regData = regValTable[i][6];
    pRegInfo->settingSize ++;

    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x0412;
    pRegInfo->settings[pRegInfo->settingSize].regData = regValTable[i][7];
    pRegInfo->settingSize ++;

    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x0413;
    pRegInfo->settings[pRegInfo->settingSize].regData = regValTable[i][8];
    pRegInfo->settingSize ++;

    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x0414;
    pRegInfo->settings[pRegInfo->settingSize].regData = regValTable[i][9];
    pRegInfo->settingSize ++;

    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x0415;
    pRegInfo->settings[pRegInfo->settingSize].regData = regValTable[i][10];
    pRegInfo->settingSize ++;

    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x0416;
    pRegInfo->settings[pRegInfo->settingSize].regData = regValTable[i][11];
    pRegInfo->settingSize ++;

    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x0417;
    pRegInfo->settings[pRegInfo->settingSize].regData = regValTable[i][12];
    pRegInfo->settingSize ++;

    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x00b1;
    pRegInfo->settings[pRegInfo->settingSize].regData =  (tol_dig_gain>>6);
    pRegInfo->settingSize ++;
	
    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x00b2;
    pRegInfo->settings[pRegInfo->settingSize].regData = ((tol_dig_gain&0x3f)<<2);
    pRegInfo->settingSize ++;
	//Release register modification

    if((i_count%100)==0)
    {
        // printf ("========>0925-81 XXX %s frameLengthLines:0x%x pExpParam->expLineLong:0x%x,tol_dig_gain:%08x\n",__func__,
        // pExpParam->frameLengthLines,pExpParam->expLineLong,tol_dig_gain);
        i_count=0x0;
    }
    i_count++;
    /// Step 3
    /// Must update the real sensor gain to pExpParam
    pExpParam->analogGain = gain*16;
    return ret;
}


isp_ret_t GC2083_Fill_Exposure_Reg(sensor_apply_exp_param_t *pExpParam, drv_regs_array_info_t *pRegInfo)
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
            ret = GC2083_Normal_Fill_Exposure_Reg(pExpParam, pRegInfo);
        }
        else
        {
            SENSOR_ERR("Sensor driver don't support this hdr mode %d now", pExpParam->hdrMode);
        }
    }

    return ret ;
}


sensor_drv_params_t* load_gc2083_sensor_driver(void)
{
    return &gc2083_sensor_params;
}

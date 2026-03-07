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

 #include "gc5603_sensor.h"

 static u8 regValTable[26][7] = {   
     
     //0614, 0615, 0225, 1467  1468, 00b8, 00b9  
     {0x00, 0x00, 0x04, 0x15, 0x15, 0x01, 0x00},
     {0x90, 0x02, 0x04, 0x15, 0x15, 0x01, 0x0A},
     {0x00, 0x00, 0x00, 0x15, 0x15, 0x01, 0x12},
     {0x90, 0x02, 0x00, 0x15, 0x15, 0x01, 0x20},//Set
     {0x01, 0x00, 0x00, 0x15, 0x15, 0x01, 0x30},
     {0x91, 0x02, 0x00, 0x15, 0x15, 0x02, 0x05},
     {0x02, 0x00, 0x00, 0x15, 0x15, 0x02, 0x19},
     {0x92, 0x02, 0x00, 0x16, 0x16, 0x02, 0x3F},
     {0x03, 0x00, 0x00, 0x16, 0x16, 0x03, 0x20},
     {0x93, 0x02, 0x00, 0x17, 0x17, 0x04, 0x0A},//Get
     {0x00, 0x00, 0x01, 0x18, 0x18, 0x05, 0x02},
     {0x90, 0x02, 0x01, 0x19, 0x19, 0x05, 0x39},
     {0x01, 0x00, 0x01, 0x19, 0x19, 0x06, 0x3C},
     {0x91, 0x02, 0x01, 0x19, 0x19, 0x08, 0x0D},
     {0x02, 0x00, 0x01, 0x1a, 0x1a, 0x09, 0x21},
     {0x92, 0x02, 0x01, 0x1a, 0x1a, 0x0B, 0x0F},
     {0x03, 0x00, 0x01, 0x1c, 0x1c, 0x0D, 0x17},
     {0x93, 0x02, 0x01, 0x1c, 0x1c, 0x0F, 0x33},
     {0x04, 0x00, 0x01, 0x1d, 0x1d, 0x12, 0x30},
     {0x94, 0x02, 0x01, 0x1d, 0x1d, 0x16, 0x10},
     {0x05, 0x00, 0x01, 0x1e, 0x1e, 0x1A, 0x19},
     {0x95, 0x02, 0x01, 0x1e, 0x1e, 0x1F, 0x13},
     {0x06, 0x00, 0x01, 0x20, 0x20, 0x25, 0x08},
     {0x96, 0x02, 0x01, 0x20, 0x20, 0x2C, 0x03},
     {0xb6, 0x04, 0x01, 0x20, 0x20, 0x34, 0x0F},
     {0x86, 0x06, 0x01, 0x20, 0x20, 0x3D, 0x3D},
 };
 
 static u32 gainLevelTable[27] = {	 
         64,
         74,
         82,
         96,
         112, 
         133, 
         153, 
         191, 
         224, 
         266, 
         322, 
         377, 
         444, 
         525, 
         609, 
         719, 
         855, 
         1011, 
         1200,
         1424,
         1689,
         2003,		
         2376,
         2819,	
         3343,
         3965,
         0xffffffff,
    };
 /// This function is used to convert gain value to register value
 static 	int total = sizeof(gainLevelTable) / sizeof(u32);
 
 // static isp_ret_t GC5603_Normal_Fill_Exposure_Reg(sensor_apply_exp_param_t *pExpParam, drv_regs_array_info_t *pRegInfo)
 // {
 //     isp_ret_t ret = ISP_RET_SUCCESS;
 //     // According to gc5603 datasheet
 //     //     1. 0x202/0x203          means exposure time
 //     //     2. 0x340/0x341(VMAX)   means the number of lines per frame(FrameLength)
 
 //     u32 gainRegValue = 0;
 
 //     if(pExpParam->analogGain<1024)
 //     {
 //         pExpParam->analogGain=1024;
 //     }
 
 //   	// Hold register
 //     pRegInfo->regAddrType = DRV_ADDR_TYPE_WORD;
 //     pRegInfo->regDataType = DRV_DATA_TYPE_BYTE;
 
 //     // Step 1
 //     //Update frame length and exposure lines to register
 //     pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x340; 
 //     pRegInfo->settings[pRegInfo->settingSize].regData = (pExpParam->frameLengthLines >> 8) & 0x3F;
 //     pRegInfo->settingSize ++;
 
 //     pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x341;
 //     pRegInfo->settings[pRegInfo->settingSize].regData = pExpParam->frameLengthLines & 0xFF;
 //     pRegInfo->settingSize ++;
 
 //     pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x202;
 //     pRegInfo->settings[pRegInfo->settingSize].regData = (pExpParam->expLineLong >> 8) & 0x3F;
 //     pRegInfo->settingSize ++;
 
 //     pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x203;
 //     pRegInfo->settings[pRegInfo->settingSize].regData = pExpParam->expLineLong & 0xFF;
 //     pRegInfo->settingSize ++;
 
 //     /// Step 2
 //     /// convert gain value to register value
 // 	//printf("frameLengthLines:%d  pExpParam->expLineLong:%d  \n",pExpParam->frameLengthLines,pExpParam->expLineLong);
 //     int i;
 //     u32 gain = pExpParam->analogGain/16;
 // 	u32 tol_dig_gain = 0;
     
 // 	 for(i = 0; i < total; i++)
 // 	 {
 // 	   if((gainLevelTable[i] <= gain)&&(gain < gainLevelTable[i+1]))
 // 	     break;
 // 	 }
 
 // 	tol_dig_gain = gain*64/gainLevelTable[i];
 
 //     //don't set by sensor fae
 //     // pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x031d;
 //     // pRegInfo->settings[pRegInfo->settingSize].regData = 0x2d;
 //     // pRegInfo->settingSize ++;
 
 //     pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x614;
 //     pRegInfo->settings[pRegInfo->settingSize].regData = regValTable[i][0];
 //     pRegInfo->settingSize ++;
 //     pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x615;
 //     pRegInfo->settings[pRegInfo->settingSize].regData = regValTable[i][1];
 //     pRegInfo->settingSize ++;
 //     pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x225;
 //     pRegInfo->settings[pRegInfo->settingSize].regData = regValTable[i][2];
 //     pRegInfo->settingSize ++;
     
 //     //don't set by sensor fae
 //     // pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x031d;
 //     // pRegInfo->settings[pRegInfo->settingSize].regData = 0x28;
 //     // pRegInfo->settingSize ++;
 
 //     pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x1467;
 //     pRegInfo->settings[pRegInfo->settingSize].regData = regValTable[i][3];
 //     pRegInfo->settingSize ++;
 
 //     pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x1468;
 //     pRegInfo->settings[pRegInfo->settingSize].regData = regValTable[i][4];
 //     pRegInfo->settingSize ++;
 //     pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x00b8;
 //     pRegInfo->settings[pRegInfo->settingSize].regData = regValTable[i][5];
 //     pRegInfo->settingSize ++;
 //     pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x00b9;
 //     pRegInfo->settings[pRegInfo->settingSize].regData = regValTable[i][6];
 //     pRegInfo->settingSize ++;
 //     pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x0064;
 //     pRegInfo->settings[pRegInfo->settingSize].regData =  (tol_dig_gain>>6);
 //     pRegInfo->settingSize ++;
 //     pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x0065;
 //     pRegInfo->settings[pRegInfo->settingSize].regData = ((tol_dig_gain&0x3f)<<2);
 //     pRegInfo->settingSize ++;
 // 	//Release register modification
 
 //     /// Step 3
 //     /// Must update the real sensor gain to pExpParam
 //     pExpParam->analogGain = tol_dig_gain * gainLevelTable[i] / 4;
 // 	pExpParam->lastAgain = pExpParam->analogGain;
 // 	pExpParam->lastDgain = 1024;
 //     SENSOR_INFO("gc5603_sensor_updategain================gainLevelTable=%u,analogGain=%u,lastagain=%u,lastdgain=%u\n",gainLevelTable[i],pExpParam->analogGain,pExpParam->lastAgain,pExpParam->lastDgain);
 //     SENSOR_INFO("i:%d again:%x %x %x %x %x %x %x, dgain:%x %x\n", i, regValTable[i][0], regValTable[i][1], regValTable[i][2], regValTable[i][3], regValTable[i][4], regValTable[i][5], regValTable[i][6], (tol_dig_gain>>6), ((tol_dig_gain&0x3f)<<2));
 //     SENSOR_INFO("vts:%d exp:%d\n", pExpParam->frameLengthLines, pExpParam->expLineLong);
 //     return ret;
 // }
 
 static u32 g_gain_old11 = 0;
 static u32 g_gain_old12 = 0;
 static u32 g_gain_index_old1 = 0;
 
 static isp_ret_t GC5603_Normal_Fill_Exposure_Reg1(sensor_apply_exp_param_t *pExpParam, drv_regs_array_info_t *pRegInfo)
 {
     isp_ret_t ret = ISP_RET_SUCCESS;
     
     // According to gc5603 datasheet
     //     1. 0x202/0x203          means exposure time
     //     2. 0x340/0x341(VMAX)   means the number of lines per frame(FrameLength)
 
     u32 gainRegValue = 0;
 
     if(pExpParam->analogGain<1024)
     {
         pExpParam->analogGain=1024;
     }
 
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
     u32 tol_dig_gain = 0;
     u32 out_gain = 0;
     u32 gain = pExpParam->analogGain/16;
 
     if (g_gain_old11 !=  gain) {
         for(i = 0; i < total; i++)
         {
           if((gainLevelTable[i] <= gain)&&(gain < gainLevelTable[i+1]))
             break;
         }
         tol_dig_gain = gain*64/gainLevelTable[i];
 
         pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x1467;
         pRegInfo->settings[pRegInfo->settingSize].regData = regValTable[i][3];
         pRegInfo->settingSize ++;
         pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x1468;
         pRegInfo->settings[pRegInfo->settingSize].regData = regValTable[i][4];
         pRegInfo->settingSize ++;
         pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x00b8;
         pRegInfo->settings[pRegInfo->settingSize].regData = regValTable[i][5];
         pRegInfo->settingSize ++;
         pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x00b9;
         pRegInfo->settings[pRegInfo->settingSize].regData = regValTable[i][6];
         pRegInfo->settingSize ++;
         pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x0064;
         pRegInfo->settings[pRegInfo->settingSize].regData =  (tol_dig_gain>>6);
         pRegInfo->settingSize ++;
         pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x0065;
         pRegInfo->settings[pRegInfo->settingSize].regData = ((tol_dig_gain&0x3f)<<2);
         pRegInfo->settingSize ++;
     }
     if (g_gain_old12 != g_gain_old11) {
         pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x614;
         pRegInfo->settings[pRegInfo->settingSize].regData = regValTable[g_gain_index_old1][0];
         pRegInfo->settingSize ++;
         pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x615;
         pRegInfo->settings[pRegInfo->settingSize].regData = regValTable[g_gain_index_old1][1];
         pRegInfo->settingSize ++;
         pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x225;
         pRegInfo->settings[pRegInfo->settingSize].regData = regValTable[g_gain_index_old1][2];
         pRegInfo->settingSize ++;
     }
     g_gain_index_old1 = i;
     g_gain_old12 = g_gain_old11;
     g_gain_old11 = gain;
 
     /// Step 3
     /// Must update the real sensor gain to pExpParam
     for(i = 0; i < total; i++)
     {
       if((gainLevelTable[i] <= gain)&&(gain < gainLevelTable[i+1]))
         break;
 }
     out_gain = gain*64/gainLevelTable[i];
 
     pExpParam->analogGain = out_gain * gainLevelTable[i] / 4;
     pExpParam->lastAgain = pExpParam->analogGain;
     pExpParam->lastDgain = 1024;
 
     SENSOR_INFO("gc5603_sensor_updategain1================gainLevelTable=%u,analogGain=%u,lastagain=%u,lastdgain=%u\n",gainLevelTable[i],pExpParam->analogGain,pExpParam->lastAgain,pExpParam->lastDgain);
     SENSOR_INFO("i:%d again:(%x %x %x) (%x %x %x %x), dgain:%x %x\n", i, regValTable[i][0], regValTable[i][1], regValTable[i][2], regValTable[i][3], regValTable[i][4], regValTable[i][5], regValTable[i][6], (tol_dig_gain>>6), ((tol_dig_gain&0x3f)<<2));
     SENSOR_INFO("vts:%d exp:%d\n", pExpParam->frameLengthLines, pExpParam->expLineLong);
     return ret;
 }
 
 static u32 g_gain_old21 = 0;
 static u32 g_gain_old22 = 0;
 static u32 g_gain_index_old2 = 0;
 
 static isp_ret_t GC5603_Normal_Fill_Exposure_Reg2(sensor_apply_exp_param_t *pExpParam, drv_regs_array_info_t *pRegInfo)
 {
     isp_ret_t ret = ISP_RET_SUCCESS;
     // According to gc5603 datasheet
     //     1. 0x202/0x203          means exposure time
     //     2. 0x340/0x341(VMAX)   means the number of lines per frame(FrameLength)
 
     u32 gainRegValue = 0;
 
     if(pExpParam->analogGain<1024)
     {
         pExpParam->analogGain=1024;
     }
 
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
     u32 tol_dig_gain = 0;
     u32 out_gain = 0;
     u32 gain = pExpParam->analogGain/16;
 
     if (g_gain_old21 !=  gain) {
         for(i = 0; i < total; i++)
         {
           if((gainLevelTable[i] <= gain)&&(gain < gainLevelTable[i+1]))
             break;
         }
         tol_dig_gain = gain*64/gainLevelTable[i];
 
         pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x1467;
         pRegInfo->settings[pRegInfo->settingSize].regData = regValTable[i][3];
         pRegInfo->settingSize ++;
         pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x1468;
         pRegInfo->settings[pRegInfo->settingSize].regData = regValTable[i][4];
         pRegInfo->settingSize ++;
         pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x00b8;
         pRegInfo->settings[pRegInfo->settingSize].regData = regValTable[i][5];
         pRegInfo->settingSize ++;
         pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x00b9;
         pRegInfo->settings[pRegInfo->settingSize].regData = regValTable[i][6];
         pRegInfo->settingSize ++;
         pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x0064;
         pRegInfo->settings[pRegInfo->settingSize].regData =  (tol_dig_gain>>6);
         pRegInfo->settingSize ++;
         pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x0065;
         pRegInfo->settings[pRegInfo->settingSize].regData = ((tol_dig_gain&0x3f)<<2);
         pRegInfo->settingSize ++;
     }
     if (g_gain_old22 != g_gain_old21) {
         pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x614;
         pRegInfo->settings[pRegInfo->settingSize].regData = regValTable[g_gain_index_old2][0];
         pRegInfo->settingSize ++;
         pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x615;
         pRegInfo->settings[pRegInfo->settingSize].regData = regValTable[g_gain_index_old2][1];
         pRegInfo->settingSize ++;
         pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x225;
         pRegInfo->settings[pRegInfo->settingSize].regData = regValTable[g_gain_index_old2][2];
         pRegInfo->settingSize ++;
     }
     g_gain_index_old2 = i;
     g_gain_old22 = g_gain_old21;
     g_gain_old21 = gain;
 
     /// Step 3
     /// Must update the real sensor gain to pExpParam
     for(i = 0; i < total; i++)
     {
       if((gainLevelTable[i] <= gain)&&(gain < gainLevelTable[i+1]))
         break;
     }
     out_gain = gain*64/gainLevelTable[i];
 
     pExpParam->analogGain = out_gain * gainLevelTable[i] / 4;
     pExpParam->lastAgain = pExpParam->analogGain;
     pExpParam->lastDgain = 1024;
 
     SENSOR_INFO("gc5603_sensor_updategain2================gainLevelTable=%u,analogGain=%u,lastagain=%u,lastdgain=%u\n",gainLevelTable[i],pExpParam->analogGain,pExpParam->lastAgain,pExpParam->lastDgain);
     SENSOR_INFO("i:%d again:%x %x %x %x %x %x %x, dgain:%x %x\n", i, regValTable[i][0], regValTable[i][1], regValTable[i][2], regValTable[i][3], regValTable[i][4], regValTable[i][5], regValTable[i][6], (tol_dig_gain>>6), ((tol_dig_gain&0x3f)<<2));
     SENSOR_INFO("vts:%d exp:%d\n", pExpParam->frameLengthLines, pExpParam->expLineLong);
     return ret;
 }
 
 isp_ret_t GC5603_Fill_Exposure_Reg1(sensor_apply_exp_param_t *pExpParam, drv_regs_array_info_t *pRegInfo)
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
             ret = GC5603_Normal_Fill_Exposure_Reg1(pExpParam, pRegInfo);
         }
         else
         {
             SENSOR_ERR("Sensor driver don't support this hdr mode %d now", pExpParam->hdrMode);
         }
     }
 
     return ret ;
 }
 
 isp_ret_t GC5603_Fill_Exposure_Reg2(sensor_apply_exp_param_t *pExpParam, drv_regs_array_info_t *pRegInfo)
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
             ret = GC5603_Normal_Fill_Exposure_Reg2(pExpParam, pRegInfo);
         }
         else
         {
             SENSOR_ERR("Sensor driver don't support this hdr mode %d now", pExpParam->hdrMode);
         }
     }
 
     return ret ;
 }
 
 
 sensor_drv_params_t* load_gc5603_sensor_driver(void)
 {
     return &gc5603_sensor_params;
 }
 
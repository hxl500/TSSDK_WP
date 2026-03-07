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

#include "cv4002_sensor.h"

static u16 Tab_SensorRegToGain[250] =
{
    64, 64, 64, 64, 65,
    65, 65, 65, 66, 66,
    66, 66, 67, 67, 67, 
    67, 68, 68, 68, 69, 
    69, 69, 70, 70, 70, 
    70, 71, 71, 71, 72, 
    72, 72, 73, 73, 73, 
    74, 74, 74, 75, 75, 
    75, 76, 76, 76, 77, 
    77, 78, 78, 78, 79, 
    79, 79, 80, 80, 81, 
    81, 81, 82, 82, 83, 
    83, 84, 84, 84, 85, 
    85, 86, 86, 87, 87, 
    88, 88, 89, 89, 90, 
    90, 91, 91, 92, 92, 
    93, 93, 94, 94, 95, 
    95, 96, 96, 97, 98, 
    98, 99, 99, 100, 101, 
    101, 102, 103, 103, 104, 
    105, 105, 106, 107, 107, 
    108, 109, 109, 110, 111, 
    112, 113, 113, 114, 115, 
    116, 117, 117, 118, 119, 
    120, 121, 122, 123, 124, 
    125, 126, 126, 128, 129, 
    130, 131, 132, 133, 134, 
    135, 136, 137, 138, 140, 
    141, 142, 143, 144, 146, 
    147, 148, 150, 151, 153, 
    154, 156, 157, 159, 160, 
    162, 163, 165, 167, 168, 
    170, 172, 174, 176, 178, 
    180, 182, 184, 186, 188, 
    190, 192, 195, 197, 199, 
    202, 204, 207, 210, 212, 
    215, 218, 221, 224, 227, 
    230, 234, 237, 240, 244, 
    248, 252, 256, 260, 264, 
    268, 273, 277, 282, 287, 
    292, 297, 303, 309, 315, 
    321, 327, 334, 341, 348, 
    356, 364, 372, 380, 390, 
    399, 409, 420, 431, 442, 
    455, 468, 481, 496, 512, 
    528, 546, 564, 585, 606, 
    630, 655, 682, 712, 744, 
    780, 819, 862, 910, 963, 
    1024, 1092, 1170,1260,
1365,1489,1638,1820,2048
,0XFFFF
};
/// This function is used to convert gain value to register value



static isp_ret_t CV4002_Normal_Fill_Exposure_Reg(sensor_apply_exp_param_t *pExpParam, drv_regs_array_info_t *pRegInfo)
{
    isp_ret_t ret = ISP_RET_SUCCESS;

    u32 gainRegValue = 0;
 /*   if(pExpParam->expLineLong>1400)
      pExpParam->expLineLong=1400;*/
    if(pExpParam->analogGain<1024)
      pExpParam->analogGain=1024;
     
    
  	// Hold register
    pRegInfo->regAddrType = DRV_ADDR_TYPE_WORD;
    pRegInfo->regDataType = DRV_DATA_TYPE_BYTE;

    int VTS = pExpParam->frameLengthLines ;
	int shutter = VTS - pExpParam->expLineLong;
  
   if ( pExpParam->expLineLong>4999){
	// pExpParam->frameLengthLines= pExpParam->expLineLong+6; 
	 pExpParam->frameLengthLines=0x186a;//6250
	shutter = pExpParam->frameLengthLines - pExpParam->expLineLong;
   }else{
	   
	   if(pExpParam->expLineLong==4999){
		   pExpParam->frameLengthLines=0x138d;//5005
	   }else{
		   pExpParam->frameLengthLines=0x1388;//5000
	   }
	   shutter = pExpParam->frameLengthLines - pExpParam->expLineLong;
   }
   
   
    // Step 1
    //Update frame length and exposure lines to register
    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x3020; 
    pRegInfo->settings[pRegInfo->settingSize].regData = pExpParam->frameLengthLines & 0xFF;
    pRegInfo->settingSize ++;
	
	pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x3021; 
    pRegInfo->settings[pRegInfo->settingSize].regData = (pExpParam->frameLengthLines >> 8) & 0xFF;
    pRegInfo->settingSize ++;

    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x3022;
    pRegInfo->settings[pRegInfo->settingSize].regData = (pExpParam->frameLengthLines >> 16) & 0x0F;
    pRegInfo->settingSize ++;
	
	//printf(" pExpParam->frameLengthLines :%d  \n",pExpParam->frameLengthLines);
	//printf(" pExpParam->expLineLong:%d  \n",pExpParam->expLineLong);
	
    /*
	int VTS = pExpParam->frameLengthLines ;
	int shutter = VTS - pExpParam->expLineLong;
	*/
	//shutter *= 2;
	//printf(" VTS = pExpParam->frameLengthLines :%d  \n",VTS);
	//printf(" pExpParam->expLineLong = i:%d  \n",pExpParam->expLineLong);
    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x3048;
    pRegInfo->settings[pRegInfo->settingSize].regData = shutter & 0xFF;
    pRegInfo->settingSize ++;

    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x3049;
    pRegInfo->settings[pRegInfo->settingSize].regData = (shutter >> 8) & 0xFF;
    pRegInfo->settingSize ++;
	
	pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x304A;
    pRegInfo->settings[pRegInfo->settingSize].regData = (shutter >> 16) & 0x0F;
    pRegInfo->settingSize ++;

    /// Step 2
    /// convert gain value to register value
    //printf("frameLengthLines:%d  pExpParam->expLineLong:%d  \n",pExpParam->frameLengthLines,pExpParam->expLineLong);
   int i;
   u32 gain = pExpParam->analogGain/16;
  // printf("pExpParam->analogGain:%d  gain = pExpParam->analogGain/16:%d  \n",pExpParam->analogGain,gain);
   int Again;
   int dgain;
    
	for (i = 0; i < 250; i++)
    {
        if ((Tab_SensorRegToGain[i] <= gain)&&(Tab_SensorRegToGain[i+1] >= gain))
			
            break;
    }
	
    //Again = i;
	Again = i+1;
	//超过32倍率，again为248索引值
	if(gain>=2048){
		Again=248;
	}
	
	//printf(" Again = i:%d  \n",Again);
	//当gain是小于等于2048，dgain =1倍率，为64
	/*
	if(gain <= 2048)
	{
		dgain=64;
		
	}else {
		//gain 大于2048，映射增益值64-128之间，1-2倍dgain
		dgain=gain >> 5;
	}
	*/
      
	//gain 大于1024，映射增益值64-128之间，1-2倍dgain
	//dgain=(gain >> 4) >= 64 ? (gain >> 4):64;
	dgain = gain * 64 / Tab_SensorRegToGain[i];
	//printf(" gain:%d   Tab_SensorRegToGain[i]:%d  dgain:%d  \n",gain,Tab_SensorRegToGain[i],dgain);

	dgain = dgain <= 1024 ? dgain : 1024;
    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x3141;
    pRegInfo->settings[pRegInfo->settingSize].regData = 1;
    pRegInfo->settingSize ++;
    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x3154;
    pRegInfo->settings[pRegInfo->settingSize].regData = Again;
    pRegInfo->settingSize ++;
    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x314C;
    pRegInfo->settings[pRegInfo->settingSize].regData = dgain&0xff;
    pRegInfo->settingSize ++;
    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x314D;
    pRegInfo->settings[pRegInfo->settingSize].regData = dgain>>8;
    pRegInfo->settingSize ++;
	//Release register modification

    /// Step 3
    /// Must update the real sensor gain to pExpParam
    pExpParam->analogGain = Tab_SensorRegToGain[Again]*16*dgain/64;
    return ret;
}


isp_ret_t CV4002_Fill_Exposure_Reg(sensor_apply_exp_param_t *pExpParam, drv_regs_array_info_t *pRegInfo)
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
            ret = CV4002_Normal_Fill_Exposure_Reg(pExpParam, pRegInfo);
        }
        else
        {
            SENSOR_ERR("Sensor driver don't support this hdr mode %d now", pExpParam->hdrMode);
        }
    }

    return ret ;
}


sensor_drv_params_t* load_cv4002_sensor_driver(void)
{
    return &cv4002_sensor_params;
}

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

#include "c3390_sensor.h"
u32 again [240] ={

    1024,	1088,	1152,	1216,	1280,	1344,	1408,	1472,	1536,	1600,	1664,	1728,	1792,	1856,	1920,	1984,	
	2048,	2048,	2176,	2176,	2304,	2304,	2432,	2432,	2560,	2560,	2688,	2688,	2816,	2816,	2944,	2944,	
	3072,	3072,	3200,	3200,	3328,	3328,	3456,	3456,	3584,	3584,	3712,	3712,	3840,	3840,	3968,	3968,	
	4096,	4096,	4096,	4096,	4352,	4352,	4352,	4352,	4608,	4608,	4608,	4608,	4864,	4864,	4864,	4864,	
	5120,	5120,	5120,	5120,	5376,	5376,	5376,	5376,	5632,	5632,	5632,	5632,	5888,	5888,	5888,	5888,	
	6144,	6144,	6144,	6144,	6400,	6400,	6400,	6400,	6656,	6656,	6656,	6656,	6912,	6912,	6912,	6912,	
	7168,	7168,	7168,	7168,	7424,	7424,	7424,	7424,	7680,	7680,	7680,	7680,	7936,	7936,	7936,	7936,	
	8192,	8192,	8192,	8192,	8192,	8192,	8192,	8192,	8704,	8704,	8704,	8704,	8704,	8704,	8704,	8704,	
	9216,	9216,	9216,	9216,	9216,	9216,	9216,	9216,	9728,	9728,	9728,	9728,	9728,	9728,	9728,	9728,	
	10240,	10240,	10240,	10240,	10240,	10240,	10240,	10240,	10752,	10752,	10752,	10752,	10752,	10752,	10752,	10752,	
	11264,	11264,	11264,	11264,	11264,	11264,	11264,	11264,	11776,	11776,	11776,	11776,	11776,	11776,	11776,	11776,	
	12288,	12288,	12288,	12288,	12288,	12288,	12288,	12288,	12800,	12800,	12800,	12800,	12800,	12800,	12800,	12800,	
	13312,	13312,	13312,	13312,	13312,	13312,	13312,	13312,	13824,	13824,	13824,	13824,	13824,	13824,	13824,	13824,	
	14336,	14336,	14336,	14336,	14336,	14336,	14336,	14336,	14848,	14848,	14848,	14848,	14848,	14848,	14848,	14848,	
	15360,	15360,	15360,	15360,	15360,	15360,	15360,	15360,	15872,	15872,	15872,	15872,	15872,	15872,	15872,	15872,
};

static isp_ret_t c3390_Normal_Fill_Exposure_Reg(sensor_apply_exp_param_t *pExpParam, drv_regs_array_info_t *pRegInfo)
{
   isp_ret_t ret = ISP_RET_SUCCESS;
    pRegInfo->regAddrType = DRV_ADDR_TYPE_WORD;
    pRegInfo->regDataType = DRV_DATA_TYPE_BYTE;

    /// Step 1
    ///    Update frame length and exposure lines to register


  
    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x0340;
    pRegInfo->settings[pRegInfo->settingSize].regData = (pExpParam->frameLengthLines >> 8) & 0xFF;
    pRegInfo->settingSize ++;

    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x0341; 
    pRegInfo->settings[pRegInfo->settingSize].regData = pExpParam->frameLengthLines & 0xFF;
    pRegInfo->settingSize ++;

if(pExpParam->expLineLong  > pExpParam->frameLengthLines )	
	pExpParam->expLineLong = pExpParam->frameLengthLines;

if(pExpParam->expLineLong < 1)
	pExpParam->expLineLong =1;

    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x0202;
    pRegInfo->settings[pRegInfo->settingSize].regData = (pExpParam->expLineLong >> 8) & 0xFF;
	pRegInfo->settingSize ++;
	
    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x0203;
    pRegInfo->settings[pRegInfo->settingSize].regData = pExpParam->expLineLong & 0xFF;
    pRegInfo->settingSize ++;

	u32 againRegValue,dgainvlaue;
	u32 aginvalue=1024;
	u32 gain;

	if(pExpParam->analogGain > again[239])
	{
			aginvalue = again[239];
			againRegValue = 239;
	};
	if(pExpParam->analogGain < again[0])
	{
			aginvalue = again[0];
			againRegValue = 0;
	};
	for (int i = 1; i < 240; i++) 
	{
		if (pExpParam->analogGain < again[i]) 
		{
			aginvalue = again[i - 1];
			againRegValue = i - 1;
			break;
		};
	};


	dgainvlaue = pExpParam->analogGain *256/aginvalue;
    
 if(dgainvlaue < 0x100)
 	
	dgainvlaue=0x100;


    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x0205;
    pRegInfo->settings[pRegInfo->settingSize].regData = againRegValue & 0xff;
    pRegInfo->settingSize ++;
    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x0216;
    pRegInfo->settings[pRegInfo->settingSize].regData = (dgainvlaue>> 8) & 0x07;
    pRegInfo->settingSize ++;
    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x0217;
    pRegInfo->settings[pRegInfo->settingSize].regData = dgainvlaue & 0xff;
    pRegInfo->settingSize ++;

    
	gain = dgainvlaue * aginvalue/256;


        
    /// Step 3
    /// Must update the real sensor gain to pExpParam
    pExpParam->analogGain = gain;
    return ret;
}


isp_ret_t c3390_Fill_Exposure_Reg(sensor_apply_exp_param_t *pExpParam, drv_regs_array_info_t *pRegInfo)
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
            ret = c3390_Normal_Fill_Exposure_Reg(pExpParam, pRegInfo);
        }
        else
        {
            SENSOR_ERR("Sensor driver don't support this hdr mode %d now", pExpParam->hdrMode);
        }
    }

    return ret ;
}


sensor_drv_params_t* load_c3390_sensor_driver(void)
{
    return &c3390_sensor_params;
}

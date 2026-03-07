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

#if defined(TARGET_CHIP_TX536)
#include "cv2003_sensor_tx536.h"
#elif defined(TARGET_CHIP_TX5215) || defined(TARGET_CHIP_TX5239) || defined(TARGET_CHIP_TX5112)
#include "cv2003_sensor_tx5215.h"
#endif


#define CV2003_DOL2_SHORT_1_RHS (509)

#define CV2003_DOL3_SHORT_1_RHS (805)   /// Mid frame max line count, should satisfy 3*n + 7
#define CV2003_DOL3_SHORT_2_RHS (1112)  /// M +S Max line count, should satisfy 3*n + 14

#define MIN_SHUTTER 6
static u16 Tab_SensorRegToGain[250] =
{
	1024, 1028, 1032, 1036, 1040, 1044, 1049, 1053, 1057, 1061, 
	1066, 1070, 1074, 1078, 1083, 1087, 1093, 1097, 1102, 1106, 
	1111, 1115, 1120, 1125, 1129, 1135, 1140, 1145, 1150, 1155, 
	1160, 1165, 1170, 1176, 1181, 1186, 1192, 1197, 1202, 1208, 
	1213, 1220, 1225, 1231, 1237, 1242, 1248, 1254, 1261, 1267, 
	1273, 1279, 1285, 1291, 1297, 1305, 1311, 1317, 1324, 1330, 
	1337, 1345, 1352, 1358, 1365, 1372, 1379, 1386, 1395, 1402, 
	1409, 1417, 1424, 1433, 1441, 1448, 1456, 1464, 1473, 1481, 
	1490, 1498, 1506, 1516, 1524, 1533, 1542, 1551, 1561, 1570, 
	1579, 1589, 1598, 1609, 1618, 1628, 1638, 1649, 1659, 1670, 
	1680, 1692, 1702, 1713, 1724, 1736, 1748, 1759, 1772, 1783, 
	1795, 1808, 1821, 1833, 1846, 1860, 1873, 1886, 1900, 1914, 
	1927, 1942, 1956, 1971, 1986, 2001, 2016, 2032, 2048, 2064, 
	2081, 2097, 2115, 2131, 2148, 2167, 2184, 2203, 2221, 2241,
	2260, 2279, 2300, 2319, 2341, 2361, 2383, 2405, 2427, 2450, 
	2473, 2497, 2521, 2545, 2570, 2596, 2621, 2648, 2675, 2702, 
	2731, 2760, 2788, 2819, 2850, 2881, 2912, 2945, 2979, 3014, 
	3048, 3084, 3121, 3158, 3197, 3236, 3277, 3319, 3361, 3405, 
	3449, 3495, 3542, 3591, 3641, 3693, 3745, 3799, 3855, 3913, 
	3972, 4033, 4096, 4161, 4228, 4298, 4369, 4443, 4520, 4599, 
	4681, 4767, 4855, 4946, 5041, 5140, 5243, 5349, 5461, 5578, 
	5699, 5826, 5958, 6096, 6241, 6394, 6554, 6722, 6899, 7085, 
	7282, 7490, 7710, 7944, 8192, 8456, 8738, 9040, 9362, 9709, 
	10082, 10486, 10923, 11397, 11915, 12483, 13107, 13797, 14563, 15420, 
	16384, 17477, 18725, 20165, 21845, 23832, 26214, 29127, 32768, 0XFFFF
};

/// This function is used to convert gain value to register value

static isp_ret_t CV2003_Normal_Fill_Exposure_Reg(sensor_apply_exp_param_t *pExpParam, drv_regs_array_info_t *pRegInfo)
{
    isp_ret_t ret = ISP_RET_SUCCESS;
    int i;
	u32 gain_search = 1024;
	u32 again = 1;
	u32 dgain = 64;
	
	u32 shutter = 0;
	u32 VTS = 0;
	
    u32 gainRegValue = 0;
    if(pExpParam->analogGain<1024)
      pExpParam->analogGain=1024;
     
    
  	// Hold register
    pRegInfo->regAddrType = DRV_ADDR_TYPE_WORD;
    pRegInfo->regDataType = DRV_DATA_TYPE_BYTE;



	#if 1 //Lation add
//	printf("cv2003 debugA frameLengthLines: %d, expLineLong: %d, analogGain: %d\n", pExpParam->frameLengthLines, pExpParam->expLineLong, pExpParam->analogGain);

	pExpParam->frameLengthLines = (pExpParam->frameLengthLines >> 1) << 1;
	
	shutter = pExpParam->expLineLong;
	VTS = pExpParam->frameLengthLines;

	if (shutter < 2)
	{
		shutter = 2;
	}

	if (shutter > (VTS - 6))
	{
		shutter = VTS - 6;
	}


	if ((shutter % 2) == 0)
	{
		shutter = VTS - shutter;
	}
	else
	{
		pExpParam->analogGain = (pExpParam->analogGain * shutter) / (shutter - 1);
		shutter = shutter - 1;
		pExpParam->expLineLong = shutter;
		shutter = VTS - shutter;	
	}
	//printf("cv2003 debugB frameLengthLines: %d, expLineLong: %d, analogGain: %d, shutter: %d\n", pExpParam->frameLengthLines, pExpParam->expLineLong, pExpParam->analogGain, shutter);	
	#endif


	#if 0
    u32 VTS = pExpParam->frameLengthLines >> 1 ;
	shutter = VTS - (pExpParam->expLineLong>>1);
    shutter = shutter * 2;
    shutter = shutter < MIN_SHUTTER ? MIN_SHUTTER : (shutter > VTS * 2 - 2 ? (VTS * 2 - 2): shutter);
	#endif
	
    // Step 1
    //Update frame length and exposure lines to register
    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x3020; 
    pRegInfo->settings[pRegInfo->settingSize].regData = pExpParam->frameLengthLines & 0xFE;
    pRegInfo->settingSize ++;
	
	pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x3021; 
    pRegInfo->settings[pRegInfo->settingSize].regData = (pExpParam->frameLengthLines >> 8) & 0xFF;
    pRegInfo->settingSize ++;

    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x3022;
    pRegInfo->settings[pRegInfo->settingSize].regData = (pExpParam->frameLengthLines >> 16) & 0x0F;
    pRegInfo->settingSize ++;
	
	//printf(" pExpParam->frameLengthLines :%d  \n",pExpParam->frameLengthLines);
	//printf(" pExpParam->expLineLong:%d  shutter  %d\n",pExpParam->expLineLong, shutter);
	

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

    //u32 gain = pExpParam->analogGain/16;
    
    gain_search = pExpParam->analogGain;
	
    //printf("pExpParam->analogGain:%d\n",pExpParam->analogGain);
        
	for (i = 0; i < 249; i++)
    {
        if ((Tab_SensorRegToGain[i] <= gain_search) && (Tab_SensorRegToGain[i+1] >= gain_search))		
            break;
    }
	
	again = i;
	
	//超过32倍率，again为248索引值
	if (again > 248) {
		again = 248;
	}
	
	dgain = gain_search * 64 / Tab_SensorRegToGain[again];
	
	dgain = dgain <= 64 ? 64 : dgain;
	//dgain = dgain >= 2000 ? 2000 : dgain;
	dgain = dgain >= 128 ? 128 : dgain;

	
    //printf("again  %d   dgain  %d\n", Tab_SensorRegToGain[again], dgain);
    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x3141;
    pRegInfo->settings[pRegInfo->settingSize].regData = 1;
    pRegInfo->settingSize ++;
    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x3154;
    pRegInfo->settings[pRegInfo->settingSize].regData = again;
    pRegInfo->settingSize ++;
    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x314C;
    pRegInfo->settings[pRegInfo->settingSize].regData = dgain&0xff;
    pRegInfo->settingSize ++;
    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x314D;
    pRegInfo->settings[pRegInfo->settingSize].regData = dgain>>8;
    pRegInfo->settingSize ++;
	
    pExpParam->analogGain = Tab_SensorRegToGain[again]*dgain/64;

	//pExpParam->analogGain = (again * dgain * 16) / 64;

	//printf("cv2003 frmlineleg: %d, expLine: %d, Again: %d, shutter: %d\n", 
	//	pExpParam->frameLengthLines, pExpParam->expLineLong, pExpParam->analogGain, shutter);	
	
	//printf("cv2003 vts_reg 0x3020: 0x%x, 0x3021: 0x%x, 0x3022: 0x%x, exp_time_reg 0x3048: 0x%x, 0x3049: 0x%x, 0x304A: 0x%x\n", 
	//pExpParam->frameLengthLines & 0xFE, (pExpParam->frameLengthLines >> 8) & 0xFF, (pExpParam->frameLengthLines >> 16) & 0x0F,
	//shutter & 0xFF, (shutter >> 8) & 0xFF, (shutter >> 16) & 0x0F);

	//printf("cv2003 again_reg: 0x3154: 0x%x, dgain_reg: 0x314C: 0x%x, 0x314D: 0x%x\n", again, dgain&0xff, dgain>>8);
	
    return ret;
}



isp_ret_t CV2003_Fill_Exposure_Reg(sensor_apply_exp_param_t *pExpParam, drv_regs_array_info_t *pRegInfo)
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
            ret = CV2003_Normal_Fill_Exposure_Reg(pExpParam, pRegInfo);
        }
        else
        {
            SENSOR_ERR("Sensor driver don't support this hdr mode %d now", pExpParam->hdrMode);
        }
    }

    return ret ;
}





sensor_drv_params_t* load_cv2003_sensor_driver(void)
{
    printf(" %s !!!!!!!!!!!!!!!\n", __func__);
    return &cv2003_sensor_params;
}

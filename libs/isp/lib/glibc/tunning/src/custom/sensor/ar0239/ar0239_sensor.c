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
#include "ar0239_sensor.h"

/// AR0239 gain table
static u32 AR0239_gain_table[]=
{ 1024 , 1057 , 1093 , 1129 , 1170 , 1213 , 1261 , 1311 , 1365 , 1424 , 1490 , 1561 , 1638 , 1724 , 1821 , 1927 ,
  2048 , 2184 , 2341 , 2521 , 2731 , 2979 , 3072 , 3171 , 3277 , 3389 , 3511 , 3641 , 3781 , 3932 , 4096 , 4274 ,
  4469 , 4681 , 4915 , 5174 , 5461 , 5783 , 6144 , 6554 , 7022 , 7562 , 8192 , 8936 , 9830 , 10923, 12288, 12684,
  13107, 13559, 14043, 14563, 15123, 15729, 16384, 17097, 17874, 18725, 19661, 20696, 21845, 23130, 24576, 25369,
  26214, 27119, 28087, 29127, 30247, 31457, 32768, 34192, 35747, 37449, 39322, 41391, 43691, 46260, 49152, 52429,
  56174, 60495, 65536, 71494, 78643, 87381,
};

/// AR0239 gain table map (register value)
static u8 AR0239_gain_reg_setting_table[][3]=
{ {0, 0,  0}, {0, 0,  1}, {0, 0,  2}, {0, 0,  3}, {0, 0,  4}, {0, 0,  5}, {0, 0,  6}, {0, 0,  7},
  {0, 0,  8}, {0, 0,  9}, {0, 0, 10}, {0, 0, 11}, {0, 0, 12}, {0, 0, 13}, {0, 0, 14}, {0, 0, 15},
  {0, 1,  0}, {0, 1,  2}, {0, 1,  4}, {0, 1,  6}, {0, 1,  8}, {0, 1, 10}, {1, 0,  0}, {1, 0,  1},
  {1, 0,  2}, {1, 0,  3}, {1, 0,  4}, {1, 0,  5}, {1, 0,  6}, {1, 0,  7}, {1, 0,  8}, {1, 0,  9},
  {1, 0, 10}, {1, 0, 11}, {1, 0, 12}, {1, 0, 13}, {1, 0, 14}, {1, 0, 15}, {1, 1,  0}, {1, 1,  2},
  {1, 1,  4}, {1, 1,  6}, {1, 1,  8}, {1, 1, 10}, {1, 1, 12}, {1, 1, 14}, {1, 2,  0}, {1, 2,  1},
  {1, 2,  2}, {1, 2,  3}, {1, 2,  4}, {1, 2,  5}, {1, 2,  6}, {1, 2,  7}, {1, 2,  8}, {1, 2,  9},
  {1, 2, 10}, {1, 2, 11}, {1, 2, 12}, {1, 2, 13}, {1, 2, 14}, {1, 2, 15}, {1, 3,  0}, {1, 3,  1},
  {1, 3,  2}, {1, 3,  3}, {1, 3,  4}, {1, 3,  5}, {1, 3,  6}, {1, 3,  7}, {1, 3,  8}, {1, 3,  9},
  {1, 3, 10}, {1, 3, 11}, {1, 3, 12}, {1, 3, 13}, {1, 3, 14}, {1, 3, 15}, {1, 4,  0}, {1, 4,  2},
  {1, 4,  4}, {1, 4,  6}, {1, 4,  8}, {1, 4, 10}, {1, 4, 12}, {1, 4, 14},
};

/// set ar0230 analog gain function
void AR0239_Set_Analog_Gain(sensor_apply_exp_param_t *pExpParam, drv_regs_array_info_t *pRegInfo)
{
    u32 againTableAddHalf = (sizeof(AR0239_gain_table)/sizeof(unsigned long))/2;
    u32 exposureGain = pExpParam->analogGain;
    u32 againIndex   = againTableAddHalf;
    u32 uiRetryTwo   = 0;
    u32 reg3100Value = 0;
    u32 reg3060Value = 0;

    if(exposureGain <= AR0239_gain_table[0])
    {
        againIndex = 0;
    }
    else if(exposureGain >= AR0239_gain_table[(sizeof(AR0239_gain_table)/sizeof(unsigned long))-1])
    {
        againIndex = (sizeof(AR0239_gain_table)/sizeof(unsigned long))-1;
    }
    else
    {
        while(againTableAddHalf > 0)
        {
            if(againTableAddHalf <= 1)
            {
                againTableAddHalf = 0;
                break;
            }
            else
            {
                if((againTableAddHalf == 2)&&(uiRetryTwo == 0))
                {
                    uiRetryTwo  = 1;
                }
                else
               {
                    againTableAddHalf = againTableAddHalf / 2;
               }
            }

            if((exposureGain >= AR0239_gain_table[againIndex - 1])&&(exposureGain <= AR0239_gain_table[againIndex]))
            {
                if(exposureGain != AR0239_gain_table[againIndex])
                {
                    againIndex = againIndex - 1;
                }
                break;
            }
            else if( exposureGain > AR0239_gain_table[againIndex] )
            {
                againIndex += againTableAddHalf;
            }
            else if( exposureGain < AR0239_gain_table[againIndex] )
            {
                againIndex -= againTableAddHalf;
            }
            //printf("%d ",againIndex);
        }
    }

    pExpParam->analogGain = AR0239_gain_table[againIndex];

    reg3100Value |= (u32)AR0239_gain_reg_setting_table[againIndex][0] << 2;
    reg3060Value |= (u32)AR0239_gain_reg_setting_table[againIndex][1] << 4;
    reg3060Value |= (u32)AR0239_gain_reg_setting_table[againIndex][2];

    //printf("gain 3100=%x 3060=%x\n",reg3100Value,reg3060Value);

    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x3100;    ///
    pRegInfo->settings[pRegInfo->settingSize].regData = reg3100Value;
    pRegInfo->settingSize ++;

    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x3060;    ///
    pRegInfo->settings[pRegInfo->settingSize].regData = reg3060Value;
    pRegInfo->settingSize ++;

}

/// fill exposure related register function
isp_ret_t AR0239_Fill_Exposure_Reg(sensor_apply_exp_param_t *pExpParam, drv_regs_array_info_t *pRegInfo)
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
            pRegInfo->regAddrType = DRV_ADDR_TYPE_WORD_REVERSE;
            pRegInfo->regDataType = DRV_DATA_TYPE_WORD_REVERSE;
            pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x3012;
            pRegInfo->settings[pRegInfo->settingSize].regData = pExpParam->expLineLong ;
            pRegInfo->settingSize ++;

            pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x300A;
            pRegInfo->settings[pRegInfo->settingSize].regData = pExpParam->frameLengthLines ;
            pRegInfo->settingSize ++;

            AR0239_Set_Analog_Gain(pExpParam, pRegInfo);

        }

    }

    return ret;
}

/// load AR0239 sensor driver
sensor_drv_params_t* Load_AR0239_Sensor_Driver(void)
{
    return &ar0239_sensor_params;
}

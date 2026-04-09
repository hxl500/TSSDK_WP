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

#include "hi556_sensor.h"

#define HI556_DOL2_SHORT_1_RHS (509)

#define HI556_DOL3_SHORT_1_RHS (805)   /// Mid frame max line count, should satisfy 3*n + 7
#define HI556_DOL3_SHORT_2_RHS (1112)  /// M +S Max line count, should satisfy 3*n + 14

static u32 HI556_Reg_Gain_Table[241] =
{
  1024,  1088,  1152,  1216,  1280,  1344,  1408,  1472,  1536,  1600,  1664,  1728,  1792,  1856,  1920,
  1984,  2048,  2112,  2176,  2240,  2304,  2368,  2432,  2496,  2560,  2624,  2688,  2752,  2816,  2880,
  2944,  3008,  3072,  3136,  3200,  3264,  3328,  3392,  3456,  3520,  3584,  3648,  3712,  3776,  3840,
  3904,  3968,  4032,  4096,  4160,  4224,  4288,  4352,  4416,  4480,  4544,  4608,  4672,  4736,  4800,
  4864,  4928,  4992,  5056,  5120,  5184,  5248,  5312,  5376,  5440,  5504,  5568,  5632,  5696,  5760,
  5824,  5888,  5952,  6016,  6080,  6144,  6208,  6272,  6336,  6400,  6464,  6528,  6592,  6656,  6720,
  6784,  6848,  6912,  6976,  7040,  7104,  7168,  7232,  7296,  7360,  7424,  7488,  7552,  7616,  7680,
  7744,  7808,  7872,  7936,  8000,  8064,  8128,  8192,  8256,  8320,  8384,  8448,  8512,  8576,  8640,
  8704,  8768,  8832,  8896,  8960,  9024,  9088,  9152,  9216,  9280,  9344,  9408,  9472,  9536,  9600,
  9664,  9728,  9792,  9856,  9920,  9984, 10048, 10112, 10176, 10240, 10304, 10368, 10432, 10496, 10560,
 10624, 10688, 10752, 10816, 10880, 10944, 11008, 11072, 11136, 11200, 11264, 11328, 11392, 11456, 11520,
 11584, 11648, 11712, 11776, 11840, 11904, 11968, 12032, 12096, 12160, 12224, 12288, 12352, 12416, 12480,
 12544, 12608, 12672, 12736, 12800, 12864, 12928, 12992, 13056, 13120, 13184, 13248, 13312, 13376, 13440,
 13504, 13568, 13632, 13696, 13760, 13824, 13888, 13952, 14016, 14080, 14144, 14208, 14272, 14336, 14400,
 14464, 14528, 14592, 14656, 14720, 14784, 14848, 14912, 14976, 15040, 15104, 15168, 15232, 15296, 15360,
 15424, 15488, 15552, 15616, 15680, 15744, 15808, 15872, 15936, 16000, 16064, 16128, 16192, 16256, 16320,
 16384
};
/// This function is used to convert gain value to register value
static u32 HI556_Gain_To_Reg(u32 expoAgain)
{

    u32 i = 0;
    u32 gainRegVal = 0x80;

    if (expoAgain <= HI556_Reg_Gain_Table[0])
    {
        gainRegVal = 0;
    }
    else if (expoAgain >= HI556_Reg_Gain_Table[240])
    {
        gainRegVal = 240;
    }
    else
    {
        while (i < 8)
        {
            if ((expoAgain >= HI556_Reg_Gain_Table[gainRegVal - 1])
                && (expoAgain <= HI556_Reg_Gain_Table[gainRegVal]))
            {
                if (expoAgain != HI556_Reg_Gain_Table[gainRegVal])
                {
                    gainRegVal = gainRegVal - 1;
                }
                break;
            }
            else if (expoAgain > HI556_Reg_Gain_Table[gainRegVal])
            {
                gainRegVal += (0x80 >> (i+1));
            }
            else if (expoAgain < HI556_Reg_Gain_Table[gainRegVal])
            {
                gainRegVal -= (0x80 >> (i+1));
            }
            i++;
        }
    }

    return gainRegVal;
}

static u32 HI556_Reg_To_Gain(u32 regVal)
{
    return HI556_Reg_Gain_Table[regVal];
}
static isp_ret_t HI556_DOL2_Fill_Exposure_Reg(sensor_apply_exp_param_t *pExpParam, drv_regs_array_info_t *pRegInfo)
{

    u32 gainRegValue = 0;
    pRegInfo->regAddrType = DRV_ADDR_TYPE_WORD;
    pRegInfo->regDataType = DRV_DATA_TYPE_BYTE;
    pRegInfo->direction   = DRV_DIRECTION_WRITE;

    /// Hold register
    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x3001;
    pRegInfo->settings[pRegInfo->settingSize].regData = 1;
    pRegInfo->settingSize ++;

    /// Step 1.
    ///    1.1 update frame length
    ///    1.2 update long / mid exposure lines to register
    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x3026;
    pRegInfo->settings[pRegInfo->settingSize].regData = (pExpParam->frameLengthLines >> 16) & 0xFF;
    pRegInfo->settingSize ++;

    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x3025;
    pRegInfo->settings[pRegInfo->settingSize].regData = (pExpParam->frameLengthLines >> 8) & 0xFF;
    pRegInfo->settingSize ++;

    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x3024;
    pRegInfo->settings[pRegInfo->settingSize].regData = pExpParam->frameLengthLines & 0xFF;
    pRegInfo->settingSize ++;

    /// According to hi556 datasheet
    /// expLineLong = pExpParam->frameLengthLines * 2 -1) - SHS2RegVal
    u32 SHS2RegVal = (pExpParam->frameLengthLines * 2) -1- pExpParam->expLineLong;
    if(SHS2RegVal%2==1)
    {
      SHS2RegVal=SHS2RegVal+1;
    }
    /*Set SHS2*/
    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x3052;
    pRegInfo->settings[pRegInfo->settingSize].regData = (SHS2RegVal >> 16) & 0xFF;
    pRegInfo->settingSize ++;

    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x3051;
    pRegInfo->settings[pRegInfo->settingSize].regData = (SHS2RegVal >> 8) & 0xFF;
    pRegInfo->settingSize ++;

    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x3050;
    pRegInfo->settings[pRegInfo->settingSize].regData = SHS2RegVal & 0xFF;
    pRegInfo->settingSize ++;

    u32 RHS1RegVal = HI556_DOL2_SHORT_1_RHS;
    u32 SHS1RegVal = 0;

    if (pExpParam->expLineMid > 500)
    {
        ///  Modify expMidLine to pExpParam
        SENSOR_WARN("expLineMid(%d) is lager than the max value (500), set expLineMid = 500", pExpParam->expLineMid);
        pExpParam->expLineMid = 500;
    }
    if(pExpParam->expLineMid < 4)
    {
       pExpParam->expLineMid = 4;
    }

    SHS1RegVal = RHS1RegVal - 1 - pExpParam->expLineMid;
    if(SHS1RegVal%2==0)
          SHS1RegVal=SHS1RegVal+1;
    /* Set RHS1 */
    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x3062;
    pRegInfo->settings[pRegInfo->settingSize].regData = (RHS1RegVal >> 16) & 0xFF;
    pRegInfo->settingSize ++;

    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x3061;
    pRegInfo->settings[pRegInfo->settingSize].regData = (RHS1RegVal >> 8) & 0xFF;
    pRegInfo->settingSize ++;

    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x3060;
    pRegInfo->settings[pRegInfo->settingSize].regData = RHS1RegVal & 0xFF;
    pRegInfo->settingSize ++;

    /* Set SHS1 */
    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x3056;
    pRegInfo->settings[pRegInfo->settingSize].regData = (SHS1RegVal >> 16) & 0xFF;
    pRegInfo->settingSize ++;

    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x3055;
    pRegInfo->settings[pRegInfo->settingSize].regData = (SHS1RegVal >> 8) & 0xFF;
    pRegInfo->settingSize ++;

    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x3054;
    pRegInfo->settings[pRegInfo->settingSize].regData = SHS1RegVal & 0xFF;
    pRegInfo->settingSize ++;

    /* Set sensor gain */
    gainRegValue = HI556_Gain_To_Reg(pExpParam->analogGain);
    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x3090;
    pRegInfo->settings[pRegInfo->settingSize].regData = gainRegValue & 0xFF;
    pRegInfo->settingSize ++;
    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x3091;
    pRegInfo->settings[pRegInfo->settingSize].regData = (gainRegValue>>8) & 0xFF;
    pRegInfo->settingSize ++;
    pExpParam->analogGain = HI556_Reg_To_Gain(gainRegValue);

   /* if (pExpParam->gainIndividualEn)
    {
        pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x3010;
        pRegInfo->settings[pRegInfo->settingSize].regData = 0x61;
        pRegInfo->settingSize ++;

        /// frame id 1
        gainRegValue = HI556_Gain_To_Reg(pExpParam->analogGain1);
        pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x30F2;
        pRegInfo->settings[pRegInfo->settingSize].regData = gainRegValue & 0xFF;
        pRegInfo->settingSize ++;
        pExpParam->analogGain1 = HI556_Reg_To_Gain(gainRegValue);
    }
    else
    {
        pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x3010;
        pRegInfo->settings[pRegInfo->settingSize].regData = 0x21;
        pRegInfo->settingSize ++;
    }
*/
    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x3001;    /// Hold register
    pRegInfo->settings[pRegInfo->settingSize].regData = 0 ;
    pRegInfo->settingSize ++;

    /// Step 3
    /// Must update the real sensor gain to pExpParam


   // printf(" HI556_DOL2_Fill_Integration_Time_Reg FrameLength 0x%x  SHS2RegVal 0x%x RHS1RegVal 0x%x SHS1RegVal 0x%x gainVal 0x%x \n", pExpParam->frameLengthLines , SHS2RegVal,/ RHS1RegVal, SHS1RegVal ,gainRegValue);
    //printf(" SHS2RegVal 0x%x RHS1RegVal 0x%x SHS1RegVal 0x%x gainVal 0x%x \n", SHS2RegVal, RHS1RegVal, SHS1RegVal ,gainRegValue);

    return 0;
}


static isp_ret_t HI556_Normal_Fill_Exposure_Reg(sensor_apply_exp_param_t *pExpParam, drv_regs_array_info_t *pRegInfo)
{
    isp_ret_t ret = ISP_RET_SUCCESS;

    /// According to hi556 datasheet
    ///     1. 0x3050/0x3051/0x3052(SHS1)          means shutter sweep time
    ///     2. 0x3024/0x3025/0x3026(VMAX)   means the number of lines per frame(FrameLength)
    ///     3. VMax = (SHS1 + 1) + Frame Integration Time(expLine)

    /// so , we can get this formual
    ///    SHS1 = pExpParam->frameLengthLines - pExpParam->expLineLong - 1

    //u32 regLongExpoIntegrationTime = pExpParam->frameLengthLines - pExpParam->expLineLong - 1;
    u32 regLongExpoIntegrationTime = pExpParam->frameLengthLines  - 2;
    u32 gainRegValue=0;
  	u32 AgainRegValue = 0;
  	u32 DgainRegValue_H = 0;
    u32 DgainRegValue_L = 0;
    u32 Again=0;
    u32 Dgain=0;

    pRegInfo->regAddrType = DRV_ADDR_TYPE_WORD;
    pRegInfo->regDataType = DRV_DATA_TYPE_BYTE;
	pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x0046;
    pRegInfo->settings[pRegInfo->settingSize].regData = 0x1;
    /// Step 1
    ///    Update frame length and exposure lines to register

    pRegInfo->settingSize ++;
    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x0006;
    pRegInfo->settings[pRegInfo->settingSize].regData = (pExpParam->frameLengthLines >> 8) & 0xFF;
    pRegInfo->settingSize ++;

    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x0007; 
    pRegInfo->settings[pRegInfo->settingSize].regData = pExpParam->frameLengthLines & 0xFF;
    pRegInfo->settingSize ++;

    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x0073;
    pRegInfo->settings[pRegInfo->settingSize].regData = ( pExpParam->expLineLong >> 16) & 0x0F;
    pRegInfo->settingSize ++;

    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x0074;
    pRegInfo->settings[pRegInfo->settingSize].regData = ( pExpParam->expLineLong >> 8) & 0xFF;
    pRegInfo->settingSize ++;

    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x0075;
    pRegInfo->settings[pRegInfo->settingSize].regData =  pExpParam->expLineLong & 0xFF;
    pRegInfo->settingSize ++;

    /// Step 2
    /// convert gain value to register value
	//Sensor_Again
    //return 0;
    AgainRegValue = HI556_Gain_To_Reg(pExpParam->analogGain);
    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x0077;
    pRegInfo->settings[pRegInfo->settingSize].regData = AgainRegValue & 0xFF;
    pRegInfo->settingSize ++;
	
	//Sensor_Dgain	
	Again = HI556_Reg_To_Gain(AgainRegValue);
	DgainRegValue_H = pExpParam->analogGain/Again;
	if(DgainRegValue_H>=8)
		DgainRegValue_H=8;		
	else	
		DgainRegValue_L = ((pExpParam->analogGain-(DgainRegValue_H*1024))*1024)/Again/4;
	Dgain=(DgainRegValue_H*1024) + (DgainRegValue_L*4);
   
   
    //printf("---AEinfo---pExpParam->analogGain:%d---Again:%d---Dgain:%d---DgainRegValue_H:%d\n",pExpParam->analogGain,Again,Dgain,DgainRegValue_H);
	
    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x0078;
    pRegInfo->settings[pRegInfo->settingSize].regData = (DgainRegValue_H) & 0xF;
    pRegInfo->settingSize ++;
    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x0079;
    pRegInfo->settings[pRegInfo->settingSize].regData = (DgainRegValue_L) & 0xFF;
    pRegInfo->settingSize ++;
    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x007A;
    pRegInfo->settings[pRegInfo->settingSize].regData = (DgainRegValue_H) & 0xF;
    pRegInfo->settingSize ++;
    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x007B;
    pRegInfo->settings[pRegInfo->settingSize].regData = (DgainRegValue_L) & 0xFF;
    pRegInfo->settingSize ++;
    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x007C;
    pRegInfo->settings[pRegInfo->settingSize].regData = (DgainRegValue_H) & 0xF;
    pRegInfo->settingSize ++;
    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x007D;
    pRegInfo->settings[pRegInfo->settingSize].regData = (DgainRegValue_L) & 0xFF;
    pRegInfo->settingSize ++;
    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x007E;
    pRegInfo->settings[pRegInfo->settingSize].regData = (DgainRegValue_H) & 0xF;
    pRegInfo->settingSize ++;	
    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x007F;
    pRegInfo->settings[pRegInfo->settingSize].regData = (DgainRegValue_L) & 0xFF;
    pRegInfo->settingSize ++;	

	  pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x0046;
    pRegInfo->settings[pRegInfo->settingSize].regData = 0x0;
    pRegInfo->settingSize ++;

    /// Step 3
    /// Must update the real sensor gain to pExpParam
    //pExpParam->analogGain = (DgainRegValue_H*1024+DgainRegValue_L*1024/256)* (AgainRegValue*1024/16+1*1024)/1024;
    //printf("DgainRegValue_H:%d DgainRegValue_L:%d AgainRegValue:%d,pExpParam->analogGain:%d\n",DgainRegValue_H,DgainRegValue_L,AgainRegValue,pExpParam->analogGain);

    return ret;
}


isp_ret_t HI556_Fill_Exposure_Reg(sensor_apply_exp_param_t *pExpParam, drv_regs_array_info_t *pRegInfo)
{
    isp_ret_t ret = ISP_RET_SUCCESS;

    if (pExpParam == NULL || pRegInfo == NULL)
    {
        SENSOR_ERR("Input param is NULL ! ");
        ret = ISP_RET_INVALID;
    }

    if (ret == ISP_RET_SUCCESS)
    {
        if (pExpParam->hdrMode == SENSOR_NORMAL_MODE || pExpParam->hdrMode == SENSOR_STITCH_3_MODE || pExpParam->hdrMode == SENSOR_STITCH_2_MODE)
        {
            ret = HI556_Normal_Fill_Exposure_Reg(pExpParam, pRegInfo);
        }
        else if (pExpParam->hdrMode == SENSOR_DOL2_MODE)
        {
            //ret = HI556_DOL2_Fill_Exposure_Reg(pExpParam, pRegInfo);
        }
        else
        {
            SENSOR_ERR("Sensor driver don't support this hdr mode %d now", pExpParam->hdrMode);
        }
    }

    return ret ;
}


sensor_drv_params_t* load_hi556_sensor_driver(void)
{
    return &hi556_sensor_params;
}

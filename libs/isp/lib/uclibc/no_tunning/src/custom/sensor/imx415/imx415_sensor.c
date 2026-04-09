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
#include "imx415_sensor_tx536.h"
#elif defined(TARGET_CHIP_TX5215) || defined(TARGET_CHIP_TX5239) || defined(TARGET_CHIP_TX5112)
#include "imx415_sensor_tx5215.h"
#endif


#define IMX415_DOL2_SHORT_1_RHS (509)

#define IMX415_DOL3_SHORT_1_RHS (805)   /// Mid frame max line count, should satisfy 3*n + 7
#define IMX415_DOL3_SHORT_2_RHS (1112)  /// M +S Max line count, should satisfy 3*n + 14

static u32 IMX415_Reg_Gain_Table[256] =
{
    1024,    1060,    1097,    1136,    1176,    1217,    1260,    1304,    1350,    1397,    1446,    1497,    1550,    1604,    1661,    1719,
    1780,    1842,    1907,    1974,    2043,    2115,    2189,    2266,    2346,    2428,    2514,    2602,    2693,    2788,    2886,    2987,
    3092,    3201,    3314,    3430,    3551,    3675,    3805,    3938,    4077,    4220,    4368,    4522,    4681,    4845,    5015,    5192,
    5374,    5563,    5758,    5961,    6170,    6387,    6611,    6844,    7084,    7333,    7591,    7858,    8134,    8420,    8716,    9022,
    9339,    9667,    10007,   10359,   10723,   11099,   11489,   11893,   12311,   12744,   13192,   13655,   14135,   14632,   15146,   15678,
    16229,   16800,   17390,   18001,   18634,   19289,   19966,   20668,   21394,   22146,   22925,   23730,   24564,   25427,   26321,   27246,
    28203,   29194,   30220,   31282,   32382,   33520,   34698,   35917,   37179,   38486,   39838,   41238,   42687,   44188,   45740,   47348,
    49012,   50734,   52517,   54363,   56273,   58251,   60298,   62417,   64610,   66881,   69231,   71664,   74182,   76789,   79488,   82281,
    85173,   88166,   91264,   94471,   97791,   101228,  104785,  108468,  112279,  116225,  120310,  124537,  128914,  133444,  138134,  142988,
    148013,  153215,  158599,  164172,  169942,  175914,  182096,  188495,  195119,  201976,  209074,  216421,  224027,  231900,  240049,  248485,
    257217,  266256,  275613,  285299,  295325,  305703,  316446,  327567,  339078,  350994,  363329,  376097,  389314,  402995,  417157,  431817,
    446992,  462700,  478961,  495793,  513216,  531251,  549921,  569246,  589250,  609958,  631393,  653582,  676550,  700326,  724936,  750412,
    776783,  804081,  832338,  861589,  891867,  923209,  955652,  989236,  1024000, 1059986, 1097236, 1135795, 1175709, 1217026, 1259795, 1304067,
    1349895, 1397333, 1446438, 1497269, 1549887, 1604353, 1660734, 1719095, 1779508, 1842044, 1906777, 1973786, 2043149, 2114949, 2189273, 2266209,
    2345848, 2428287, 2513622, 2601956, 2693394, 2788046, 2886024, 2987445, 3092431, 3201105, 3313599, 3430046, 3550585, 3675361, 3804521, 3938220,
    4076617, 4219879, 4368174, 4521681, 4680583, 4845069, 5015335, 5191585, 5374028, 5562883, 5758375, 5960737, 6170210, 6387045, 6611499, 6843842,
};


/// This function is used to convert gain value to register value
static u32 IMX415_Gain_To_Reg(u32 expoAgain)
{

    u32 i = 0;
    u32 gainRegVal = 0x80;

    if (expoAgain <= IMX415_Reg_Gain_Table[0])
    {
        gainRegVal = 0;
    }
    else if (expoAgain >= IMX415_Reg_Gain_Table[255])
    {
        gainRegVal = 255;
    }
    else
    {
        while (i < 8)
        {
            if ((expoAgain >= IMX415_Reg_Gain_Table[gainRegVal - 1])
                && (expoAgain <= IMX415_Reg_Gain_Table[gainRegVal]))
            {
                if (expoAgain != IMX415_Reg_Gain_Table[gainRegVal])
                {
                    gainRegVal = gainRegVal - 1;
                }
                break;
            }
            else if (expoAgain > IMX415_Reg_Gain_Table[gainRegVal])
            {
                gainRegVal += (0x80 >> (i+1));
            }
            else if (expoAgain < IMX415_Reg_Gain_Table[gainRegVal])
            {
                gainRegVal -= (0x80 >> (i+1));
            }
            i++;
        }
    }

    return gainRegVal;
}

static u32 IMX415_Reg_To_Gain(u32 regVal)
{
    return IMX415_Reg_Gain_Table[regVal];
}

static isp_ret_t IMX415_DOL3_Fill_Exposure_Reg(sensor_apply_exp_param_t * pExpParam, drv_regs_array_info_t * pRegInfo)
{

    u32 gainRegValue = 0;
    pRegInfo->regAddrType = DRV_ADDR_TYPE_WORD;
    pRegInfo->regDataType = DRV_DATA_TYPE_BYTE;
    pRegInfo->direction   = DRV_DIRECTION_WRITE;
    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x3001;    /// Hold register
    pRegInfo->settings[pRegInfo->settingSize].regData = 1;
    pRegInfo->settingSize ++;
    
    
    /// According to imx415 datasheet
    /// expLineLong = pExpParam->frameLengthLines * 4 -1) - SHS3RegVal
    u32 SHS3RegVal = (pExpParam->frameLengthLines * 4 - 1) - pExpParam->expLineLong;

    /*Set SHS3*/
    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x303a;
    pRegInfo->settings[pRegInfo->settingSize].regData = (SHS3RegVal >> 16) & 0xFF;
    pRegInfo->settingSize ++;


    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x3029;
    pRegInfo->settings[pRegInfo->settingSize].regData = (SHS3RegVal >> 8) & 0xFF;
    pRegInfo->settingSize ++;

    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x3028;
    pRegInfo->settings[pRegInfo->settingSize].regData = SHS3RegVal & 0xFF;
    pRegInfo->settingSize ++;

    u32 RHS1RegVal = IMX415_DOL3_SHORT_1_RHS;
    u32 SHS1RegVal = 0;

    if (pExpParam->expLineMid > 1100 - pExpParam->expLineShort)
    {
        ///  Modify expMidLine to pExpParam
        SENSOR_WARN(" expLineMid %d is lager than the max value , set expLineMid = %d ",
            pExpParam->expLineMid, 1100 - pExpParam->expLineShort);
        pExpParam->expLineMid = 1100 - pExpParam->expLineShort;
    }

    SHS1RegVal = RHS1RegVal - 1 - pExpParam->expLineMid;

    /* Set RHS1 */
    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x3032;
    pRegInfo->settings[pRegInfo->settingSize].regData = (RHS1RegVal >> 16) & 0xFF;
    pRegInfo->settingSize ++;

    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x3031;
    pRegInfo->settings[pRegInfo->settingSize].regData = (RHS1RegVal >> 8) & 0xFF;
    pRegInfo->settingSize ++;

    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x3030;
    pRegInfo->settings[pRegInfo->settingSize].regData = RHS1RegVal & 0xFF;
    pRegInfo->settingSize ++;

    /* Set SHS1 */
    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x3022;
    pRegInfo->settings[pRegInfo->settingSize].regData = (SHS1RegVal >> 16) & 0xFF;
    pRegInfo->settingSize ++;

    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x3021;
    pRegInfo->settings[pRegInfo->settingSize].regData = (SHS1RegVal >> 8) & 0xFF;
    pRegInfo->settingSize ++;

    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x3020;
    pRegInfo->settings[pRegInfo->settingSize].regData = SHS1RegVal & 0xFF;
    pRegInfo->settingSize ++;

    u32 RHS2RegVal = IMX415_DOL3_SHORT_2_RHS;
    u32 SHS2RegVal = 0;

    if (pExpParam->expLineShort > 500)
    {
        ///  Modify expMidLine to pExpParam
        SENSOR_WARN(" expLineShort %d is lager than the max value , set expLineMid = 500 ",
            pExpParam->expLineShort);
        pExpParam->expLineShort = 500;
    }

    SHS2RegVal = RHS2RegVal - 1 - pExpParam->expLineShort;

    /* Set RHS2 */
    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x3036;
    pRegInfo->settings[pRegInfo->settingSize].regData = (RHS2RegVal >> 16) & 0xFF;
    pRegInfo->settingSize ++;

    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x3035;
    pRegInfo->settings[pRegInfo->settingSize].regData = (RHS2RegVal >> 8) & 0xFF;
    pRegInfo->settingSize ++;

    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x3034;
    pRegInfo->settings[pRegInfo->settingSize].regData = RHS2RegVal & 0xFF;
    pRegInfo->settingSize ++;

    /* Set SHS2 */
    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x3026;
    pRegInfo->settings[pRegInfo->settingSize].regData = (SHS2RegVal >> 16) & 0xFF;
    pRegInfo->settingSize ++;

    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x3025;
    pRegInfo->settings[pRegInfo->settingSize].regData = (SHS2RegVal >> 8) & 0xFF;
    pRegInfo->settingSize ++;

    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x3024;
    pRegInfo->settings[pRegInfo->settingSize].regData = SHS2RegVal & 0xFF;
    pRegInfo->settingSize ++;

    /* Set sensor gain */
    gainRegValue = IMX415_Gain_To_Reg(pExpParam->analogGain);
    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x3014;
    pRegInfo->settings[pRegInfo->settingSize].regData = gainRegValue & 0xFF;
    pRegInfo->settingSize ++;
    pExpParam->analogGain = IMX415_Reg_To_Gain(gainRegValue);

    if (pExpParam->gainIndividualEn)
    {
        pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x3010;
        pRegInfo->settings[pRegInfo->settingSize].regData = 0x61;
        pRegInfo->settingSize ++;

        /// frame id 1
        gainRegValue = IMX415_Gain_To_Reg(pExpParam->analogGain1);
        pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x30F2;
        pRegInfo->settings[pRegInfo->settingSize].regData = gainRegValue & 0xFF;
        pRegInfo->settingSize ++;
        pExpParam->analogGain1 = IMX415_Reg_To_Gain(gainRegValue);

        /// frame id 2
        gainRegValue = IMX415_Gain_To_Reg(pExpParam->analogGain2);
        pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x30F6;
        pRegInfo->settings[pRegInfo->settingSize].regData = gainRegValue & 0xFF;
        pRegInfo->settingSize ++;
        pExpParam->analogGain2 = IMX415_Reg_To_Gain(gainRegValue);
    }
    else
    {
        pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x3010;
        pRegInfo->settings[pRegInfo->settingSize].regData = 0x21;
        pRegInfo->settingSize ++;
    }

    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x3001;    /// Hold register
    pRegInfo->settings[pRegInfo->settingSize].regData = 0 ;
    pRegInfo->settingSize ++;


//    SENSOR_INFO("LONG 0x%x  RHS1:0x%x SHS1:0x%x RHS2:0x%x SHS2:0x%x ",
//         SHS3RegVal,
//         RHS1RegVal,
//         SHS1RegVal,
//         RHS2RegVal,
//         SHS2RegVal);

    /// Step 3
    /// Must update the real sensor gain to pExpParam

    return 0;


}



static isp_ret_t IMX415_DOL2_Fill_Exposure_Reg(sensor_apply_exp_param_t *pExpParam, drv_regs_array_info_t *pRegInfo)
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

    /// According to imx415 datasheet
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

    u32 RHS1RegVal = IMX415_DOL2_SHORT_1_RHS;
    u32 SHS1RegVal = 0;

    if (pExpParam->expLineMid > 500)
    {
        ///  Modify expMidLine to pExpParam
        SENSOR_WARN("expLineMid(%d) is lager than the max value (500), set expLineMid = 500", pExpParam->expLineMid);
        pExpParam->expLineMid = 500;
    }
    if(pExpParam->expLineMid < 8)
    {
       pExpParam->expLineMid = 8;
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
    gainRegValue = IMX415_Gain_To_Reg(pExpParam->analogGain);
    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x3090;
    pRegInfo->settings[pRegInfo->settingSize].regData = gainRegValue & 0xFF;
    pRegInfo->settingSize ++;
    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x3091;
    pRegInfo->settings[pRegInfo->settingSize].regData = (gainRegValue>>8) & 0xFF;
    pRegInfo->settingSize ++;
    pExpParam->analogGain = IMX415_Reg_To_Gain(gainRegValue);

   /* if (pExpParam->gainIndividualEn)
    {
        pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x3010;
        pRegInfo->settings[pRegInfo->settingSize].regData = 0x61;
        pRegInfo->settingSize ++;

        /// frame id 1
        gainRegValue = IMX415_Gain_To_Reg(pExpParam->analogGain1);
        pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x30F2;
        pRegInfo->settings[pRegInfo->settingSize].regData = gainRegValue & 0xFF;
        pRegInfo->settingSize ++;
        pExpParam->analogGain1 = IMX415_Reg_To_Gain(gainRegValue);
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


   // printf(" IMX415_DOL2_Fill_Integration_Time_Reg FrameLength 0x%x  SHS2RegVal 0x%x RHS1RegVal 0x%x SHS1RegVal 0x%x gainVal 0x%x \n", pExpParam->frameLengthLines , SHS2RegVal,/ RHS1RegVal, SHS1RegVal ,gainRegValue);
    //printf(" SHS2RegVal 0x%x RHS1RegVal 0x%x SHS1RegVal 0x%x gainVal 0x%x \n", SHS2RegVal, RHS1RegVal, SHS1RegVal ,gainRegValue);

    return 0;
}


static isp_ret_t IMX415_Normal_Fill_Exposure_Reg(sensor_apply_exp_param_t *pExpParam, drv_regs_array_info_t *pRegInfo)
{

    isp_ret_t ret = ISP_RET_SUCCESS;

    /// According to imx415 datasheet
    ///     1. 0x3050/0x3051/0x3052(SHS1)          means shutter sweep time
    ///     2. 0x3024/0x3025/0x3026(VMAX)   means the number of lines per frame(FrameLength)
    ///     3. VMax = (SHS1 + 1) + Frame Integration Time(expLine)

    /// so , we can get this formual
    ///    SHS1 = pExpParam->frameLengthLines - pExpParam->expLineLong - 1

    //u32 regLongExpoIntegrationTime = pExpParam->frameLengthLines - pExpParam->expLineLong - 1;
    
    //printf("pExpParam->frameLengthLines:%d\n",pExpParam->frameLengthLines);
    u32 regLongExpoIntegrationTime = pExpParam->frameLengthLines - pExpParam->expLineLong - 6;
    u32 gainRegValue = 0;

    pRegInfo->regAddrType = DRV_ADDR_TYPE_WORD;
    pRegInfo->regDataType = DRV_DATA_TYPE_BYTE;
    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x3001;    /// Hold register
    pRegInfo->settings[pRegInfo->settingSize].regData = 1;
    pRegInfo->settingSize ++;

    /// Step 1
    ///    Update frame length and exposure lines to register
    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x3026;
    pRegInfo->settings[pRegInfo->settingSize].regData = (pExpParam->frameLengthLines >> 16) & 0x0F;
    pRegInfo->settingSize ++;

    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x3025; 
    pRegInfo->settings[pRegInfo->settingSize].regData = (pExpParam->frameLengthLines >> 8) & 0xFF;
    pRegInfo->settingSize ++;

    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x3024;
    pRegInfo->settings[pRegInfo->settingSize].regData = pExpParam->frameLengthLines & 0xFF;
    pRegInfo->settingSize ++;

    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x3052;
    pRegInfo->settings[pRegInfo->settingSize].regData = (regLongExpoIntegrationTime >> 16) & 0x0F;
    pRegInfo->settingSize ++;

    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x3051;
    pRegInfo->settings[pRegInfo->settingSize].regData = (regLongExpoIntegrationTime >> 8) & 0xFF;
    pRegInfo->settingSize ++;

    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x3050;
    pRegInfo->settings[pRegInfo->settingSize].regData = regLongExpoIntegrationTime & 0xFF;
    pRegInfo->settingSize ++;

	/// Step 2
	/// restrict the analogGain with special maxAgain
	pExpParam->analogGain = pExpParam->analogGain > pExpParam->maxAgain ? pExpParam->maxAgain : pExpParam->analogGain;

    /// Step 3
    /// convert gain value to register value
    gainRegValue = IMX415_Gain_To_Reg(pExpParam->analogGain);
    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x3090;
    pRegInfo->settings[pRegInfo->settingSize].regData = gainRegValue & 0xFF;
    pRegInfo->settingSize ++;
    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x3091;
    pRegInfo->settings[pRegInfo->settingSize].regData = (gainRegValue>>8) & 0xFF;
    pRegInfo->settingSize ++;

    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x3001;    /// Hold register
    pRegInfo->settings[pRegInfo->settingSize].regData = 0 ;
    pRegInfo->settingSize ++;

    /// Step 3
    /// Must update the real sensor gain to pExpParam
    pExpParam->analogGain = IMX415_Reg_To_Gain(gainRegValue);

    pExpParam->lastAgain = pExpParam->analogGain;
    pExpParam->lastDgain = 1024;

    return ret;
}


isp_ret_t IMX415_Fill_Exposure_Reg(sensor_apply_exp_param_t *pExpParam, drv_regs_array_info_t *pRegInfo)
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
            ret = IMX415_Normal_Fill_Exposure_Reg(pExpParam, pRegInfo);
        }
        else if (pExpParam->hdrMode == SENSOR_DOL2_MODE || pExpParam->hdrMode == SENSOR_STAGGER_2_MODE)
        {
            ret = IMX415_DOL2_Fill_Exposure_Reg(pExpParam, pRegInfo);
        }
        else if (pExpParam->hdrMode == SENSOR_DOL3_MODE)
        {
            ret = IMX415_DOL3_Fill_Exposure_Reg(pExpParam, pRegInfo);
        }
        else
        {
            SENSOR_ERR("Sensor driver don't support this hdr mode %d now", pExpParam->hdrMode);
        }
    }

    return ret ;
}


sensor_drv_params_t* load_imx415_sensor_driver(void)
{
    return &imx415_sensor_params;
}

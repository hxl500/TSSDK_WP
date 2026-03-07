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

#include "k17_sensor.h"

#define HDR2_SHORT_EXP_MAX (0xFA * 2 - 4)
#define HDR2_LONG_EXP_MAX (0xFA * 2 + 6)
static isp_ret_t k17_Normal_Fill_Exposure_Reg(sensor_apply_exp_param_t *pExpParam, drv_regs_array_info_t *pRegInfo)
{
    isp_ret_t ret = ISP_RET_SUCCESS;
    u32 gainRegValue = 0;
    if(pExpParam->analogGain<1024)
      pExpParam->analogGain=1024;
     
    //pExpParam->expLineLong=pExpParam->expLineLong-1;
  	// Hold register
    pRegInfo->regAddrType = DRV_ADDR_TYPE_BYTE;
    pRegInfo->regDataType = DRV_DATA_TYPE_BYTE;

    // Step 1
    //Update frame length and exposure lines to register
    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x23; 
    pRegInfo->settings[pRegInfo->settingSize].regData = (pExpParam->frameLengthLines >> 8) & 0xFF;
    pRegInfo->settingSize ++;

    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x22;
    pRegInfo->settings[pRegInfo->settingSize].regData = pExpParam->frameLengthLines & 0xFF;
    pRegInfo->settingSize ++;

    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x02;
    pRegInfo->settings[pRegInfo->settingSize].regData = (pExpParam->expLineLong >> 8) & 0xFF;
    pRegInfo->settingSize ++;

    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x01;
    pRegInfo->settings[pRegInfo->settingSize].regData = pExpParam->expLineLong & 0xFF;
    pRegInfo->settingSize ++;

    /// Step 2
    /// convert gain value to register value
   //printf("frameLengthLines:%d  pExpParam->expLineLong:%d  \n",pExpParam->frameLengthLines,pExpParam->expLineLong);
   int againH = 0;
   int gainidx = 0;
   u32 gain = (pExpParam->analogGain >> 10);
   u32 gain2 = (pExpParam->analogGain >> 6);
   u32 gainphase = (pExpParam->analogGain & 0x3ff) >> 6;

   for ( int i = 1; i < 6; i++)
   {
      
      gainidx = gain >> i;
     
      if (gainidx == 0)
      {
        againH = i - 1;
        break;
      }
   }

   for ( int j = 0; j < 16; j++)
   {
      int regagain;
      if (againH == 0)
      {
        regagain = (16+j);
      }
      else
      {
        regagain = (2 << (againH - 1))*(16+j);       
      }
      
      if (regagain >= gain2)
      {
        gainphase = j;
        break;
      }
   }

  
   pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x00;
   pRegInfo->settings[pRegInfo->settingSize].regData = (againH << 4) + gainphase;
   pRegInfo->settingSize ++;

   u32 blcSunCancelGain = (againH << 4) + gainphase;
   #if 0
  //  sensor_ctrl_t* pSensorCtrl = NULL;
  //  u32 blcSunCancelReg[4] ;
  //  isp_hw_req_t        *pHwReq        = pSensorCtrl->pIspHwReq;
  //  //Sensor_Clear_Regs_Array(pSensorCtrl->pApplySetting);
  //  pSensorCtrl->pApplySetting->regAddrType =pRegInfo->regAddrType;
  //  pSensorCtrl->pApplySetting->regDataType = pRegInfo->regDataType;
  //  pSensorCtrl->pApplySetting->direction   = DRV_DIRECTION_READ;
  //  pSensorCtrl->pApplySetting->settingSize = 4;
  //  pSensorCtrl->pApplySetting->settings[0].regAddr = 0x2F;
  //  pSensorCtrl->pApplySetting->settings[1].regAddr = 0x82;
  //  pSensorCtrl->pApplySetting->settings[2].regAddr = 0x0C;
  //  pSensorCtrl->pApplySetting->settings[3].regAddr = 0x0C;
  //   Isp_Hw_Update_Regs_To_Req(pHwReq, pSensorCtrl->pApplySetting);
  //   pSensorCtrl->pIspHwIf->read(pSensorCtrl->pIspHwIf, pHwReq);

  //   blcSunCancelReg[0] = pSensorCtrl->pApplySetting->settings[0].regData;
  //   blcSunCancelReg[1] = pSensorCtrl->pApplySetting->settings[1].regData;
  //   blcSunCancelReg[2] = pSensorCtrl->pApplySetting->settings[2].regData;
  //   blcSunCancelReg[3] = pSensorCtrl->pApplySetting->settings[3].regData;
  //   printf("BlcCancel  %4x  %4x  %4x  %4x\n",blcSunCancelReg[0],blcSunCancelReg[1],blcSunCancelReg[2],blcSunCancelReg[3]);
  //  if (blcSunCancelGain < 0x20)
  //  {
  //    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x2F;
  //    pRegInfo->settings[pRegInfo->settingSize].regData = (blcSunCancelReg[0] & 0x9f);
  //    pRegInfo->settingSize ++;
  //    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x82;
  //    pRegInfo->settings[pRegInfo->settingSize].regData = (blcSunCancelReg[1] | 0x2);
  //    pRegInfo->settingSize ++;
  //    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x0C;
  //    pRegInfo->settings[pRegInfo->settingSize].regData = (blcSunCancelReg[2] | 0x40);
  //    pRegInfo->settingSize ++;
  //    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x67;
  //    pRegInfo->settings[pRegInfo->settingSize].regData = (blcSunCancelReg[3] | 0x70);
  //    pRegInfo->settingSize ++;
    
  //  }
  //  else
  //  {
  //    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x2F;
  //    pRegInfo->settings[pRegInfo->settingSize].regData = ((blcSunCancelReg[0] | 0x40) & 0xDF);
  //    pRegInfo->settingSize ++;
  //    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x82;
  //    pRegInfo->settings[pRegInfo->settingSize].regData = (blcSunCancelReg[1] & 0xFD);
  //    pRegInfo->settingSize ++;
  //    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x0C;
  //    pRegInfo->settings[pRegInfo->settingSize].regData = (blcSunCancelReg[2] & 0xBF);
  //    pRegInfo->settingSize ++;
  //    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x67;
  //    pRegInfo->settings[pRegInfo->settingSize].regData = ((blcSunCancelReg[3] | 0x60) & 0xEF);
  //    pRegInfo->settingSize ++;
  //  }
   # endif
   if (blcSunCancelGain < 0x20)
   {
     pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x2F;
     pRegInfo->settings[pRegInfo->settingSize].regData = 0x04;
     pRegInfo->settingSize ++;
     pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x82;
     pRegInfo->settings[pRegInfo->settingSize].regData = 0x22;
     pRegInfo->settingSize ++;
     pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x0C;
     pRegInfo->settings[pRegInfo->settingSize].regData = 0x60;
     pRegInfo->settingSize ++;
     pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x67;
     pRegInfo->settings[pRegInfo->settingSize].regData = 0x71;
     pRegInfo->settingSize ++;
    
   }
   else
   {
     pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x2F;
     pRegInfo->settings[pRegInfo->settingSize].regData = 0x44;
     pRegInfo->settingSize ++;
     pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x82;
     pRegInfo->settings[pRegInfo->settingSize].regData = 0x20;
     pRegInfo->settingSize ++;
     pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x0C;
     pRegInfo->settings[pRegInfo->settingSize].regData = 0x20;
     pRegInfo->settingSize ++;
     pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x67;
     pRegInfo->settings[pRegInfo->settingSize].regData = 0x61;
     pRegInfo->settingSize ++;
   }
   if (againH == 0)
   {
     pExpParam->analogGain = ((16+gainphase) << 10) >> 4;
   }
   else
   {
     pExpParam->analogGain = ((2 << (againH - 1))*(16+gainphase) << 10) >> 4;
   }
    return ret;
}

static isp_ret_t k17_Hdr2_Fill_Exposure_Reg(sensor_apply_exp_param_t *pExpParam, drv_regs_array_info_t *pRegInfo)
{
    isp_ret_t ret = ISP_RET_SUCCESS;

    u32 gainRegValue = 0;

    if(pExpParam->analogGain<1024)
      pExpParam->analogGain=1024;
     
    //  
    //pExpParam->expLineLong=pExpParam->expLineLong-1;
    
  	// Hold register
    pRegInfo->regAddrType = DRV_ADDR_TYPE_BYTE;
    pRegInfo->regDataType = DRV_DATA_TYPE_BYTE;
    //printf("frameLengthLines    %d\tExpLong    %d\tExpMid    %d\n",pExpParam->frameLengthLines,pExpParam->expLineLong,pExpParam->expLineMid);

    if (pExpParam->expLineMid > 500 || pExpParam->expLineMid >= HDR2_SHORT_EXP_MAX)
    {
        ///  Modify expMidLine to pExpParam
        //SENSOR_WARN("expLineMid(%d) is lager than the max value (500), set expLineMid = 500", pExpParam->expLineMid);
        pExpParam->expLineMid = ((500 > HDR2_SHORT_EXP_MAX ) ? HDR2_SHORT_EXP_MAX : 500);
    }
    if(pExpParam->expLineMid < 1)
    {
       pExpParam->expLineMid = 1;
    }
    if (pExpParam->expLineLong >= (pExpParam->frameLengthLines - HDR2_LONG_EXP_MAX))
    {
       pExpParam->expLineLong = pExpParam->frameLengthLines - HDR2_LONG_EXP_MAX;
    }
    if ((pExpParam->expLineLong + pExpParam->expLineMid) >= pExpParam->frameLengthLines)
    {
      pExpParam->expLineLong = pExpParam->frameLengthLines;
    }
    
    //printf("Short Frame Time:  %d\n",pExpParam->expLineMid);
    //printf("Long Frame Time:  %d\n",pExpParam->expLineLong);
    // Step 1
    //Update frame length and exposure lines to register
    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x23; 
    pRegInfo->settings[pRegInfo->settingSize].regData = (pExpParam->frameLengthLines >> 8) & 0xFF;
    pRegInfo->settingSize ++;

    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x22;
    pRegInfo->settings[pRegInfo->settingSize].regData = pExpParam->frameLengthLines & 0xFF;
    pRegInfo->settingSize ++;

    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x02;
    pRegInfo->settings[pRegInfo->settingSize].regData = (pExpParam->expLineLong >> 8) & 0xFF;
    pRegInfo->settingSize ++;

    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x01;
    pRegInfo->settings[pRegInfo->settingSize].regData = pExpParam->expLineLong & 0xFF;
    pRegInfo->settingSize ++;

    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x08;
    pRegInfo->settings[pRegInfo->settingSize].regData = (pExpParam->expLineMid >> 8) & 0x1;
    pRegInfo->settingSize ++;

    pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x05;
    pRegInfo->settings[pRegInfo->settingSize].regData = pExpParam->expLineMid & 0xFF;
    pRegInfo->settingSize ++;

    /// Step 2
    /// convert gain value to register value
   //printf("frameLengthLines:%d  pExpParam->expLineLong:%d  \n",pExpParam->frameLengthLines,pExpParam->expLineLong);
   int againH = 0;
   int gainidx = 0;
   u32 gain = (pExpParam->analogGain >> 10);
   u32 gain2 = (pExpParam->analogGain >> 6);
   u32 gainphase = (pExpParam->analogGain & 0x3ff) >> 6;

   for ( int i = 1; i < 6; i++)
   {
      
      gainidx = gain >> i;
     
      if (gainidx == 0)
      {
        againH = i - 1;
        break;
      }
   }

   for ( int j = 0; j < 16; j++)
   {
      int regagain;
      if (againH == 0)
      {
        regagain = (16+j);
      }
      else
      {
        regagain = (2 << (againH - 1))*(16+j);       
      }
      
      if (regagain >= gain2)
      {
        gainphase = j;
        break;
      }
   }

  
   pRegInfo->settings[pRegInfo->settingSize].regAddr = 0x00;
   pRegInfo->settings[pRegInfo->settingSize].regData = (againH << 4) + gainphase;
   pRegInfo->settingSize ++;
   if (againH == 0)
   {
     pExpParam->analogGain = ((16+gainphase) << 10) >> 4;
   }
   else
   {
     pExpParam->analogGain = ((2 << (againH - 1))*(16+gainphase) << 10) >> 4;
   }
    return ret;
}

isp_ret_t k17_Fill_Exposure_Reg(sensor_apply_exp_param_t *pExpParam, drv_regs_array_info_t *pRegInfo)
{
    isp_ret_t ret = ISP_RET_SUCCESS;

    if (pExpParam == NULL || pRegInfo == NULL)
    {
        SENSOR_ERR("Input param is NULL ! ");
        ret = ISP_RET_INVALID;
    }
    //printf("HDR Mode  %d\n", pExpParam->hdrMode);
    if (ret == ISP_RET_SUCCESS)
    {
        if (pExpParam->hdrMode == SENSOR_NORMAL_MODE)
        {
            ret = k17_Normal_Fill_Exposure_Reg(pExpParam, pRegInfo);
        }
        else if(pExpParam->hdrMode == SENSOR_STAGGER_2_MODE)
        {
            ret = k17_Hdr2_Fill_Exposure_Reg(pExpParam, pRegInfo);
        }
        else
        {
            SENSOR_ERR("Sensor driver don't support this hdr mode %d now", pExpParam->hdrMode);
        }
    }

    return ret ;
}


sensor_drv_params_t* load_k17_sensor_driver(void)
{
    printf("load_k17_sensor_driver!!!!!!!!!!!!!!!\n");
    return &k17_sensor_params;
}

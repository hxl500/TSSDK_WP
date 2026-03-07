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

#ifndef __TUNE_ISP_CSP_V0_H__
#define __TUNE_ISP_CSP_V0_H__

#include "tune_utils.h"

/***************************************************
*                      CSP                          *
 ***************************************************/
typedef struct
{
    u32  cspChsEn;          /// default 1, [0:1]
} csp_switch_param_by_gain_t;

typedef struct
{
    u32  cspChsHiGain0;     /// default 1024, [0:4095]
    u32  cspChsHiGain1;     /// default 1024, [0:4095]
    u32  cspChsHiGain2;     /// default 1024, [0:4095]
    u32  cspChsHiGain3;     /// default 1024, [0:4095]
    u32  cspChsLoGain0;     /// default 1024, [0:4095]
    u32  cspChsLoGain1;     /// default 1024, [0:4095]
    u32  cspChsLoGain2;     /// default 1024, [0:4095]
    u32  cspChsLoGain3;     /// default 1024, [0:4095]
    u32  cspChsMaxGain;     /// default 1024, [0:4095]
    u32  cspChsWeightU0;    /// default 0, [0:511]
    u32  cspChsWeightU1;    /// default 0, [0:511]
    u32  cspChsWeightU2;    /// default 0, [0:511]
    u32  cspChsWeightV0;    /// default 0, [0:511]
    u32  cspChsWeightV1;    /// default 0, [0:511]
    u32  cspChsWeightV2;    /// default 0, [0:511]
} csp_core_param_by_gain_t;

typedef struct
{
    /// static parameters
    BOOL cspTuneEnable;     /// default 1, [0:1], CSP tuning enable/disable

    /// dynamic parameters
    DYNAMIC_TABLE(csp_switch_param_by_gain_t, cspGainSwitchTab);
    DYNAMIC_TABLE(csp_core_param_by_gain_t, cspGainInterpTab);
} tune_csp_param_t;

#endif

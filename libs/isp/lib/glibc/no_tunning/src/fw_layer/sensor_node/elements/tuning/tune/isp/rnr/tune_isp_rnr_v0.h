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

#ifndef __TUNE_ISP_RNR_V0_H__
#define __TUNE_ISP_RNR_V0_H__

#if (defined(HW_BUILD_FOR_I5)       ||\
     defined(HW_BUILD_FOR_I5_R1P0)  ||\
     defined(HW_BUILD_FOR_I3)       ||\
     defined(HW_BUILD_FOR_CUST0))

#include "tune_utils.h"

/***************************************************
*                      RNR                          *
 ***************************************************/
///RNR region params auto by Gain
typedef struct
{
    u32    rnrBltLfNpGain;           /// default 100, [0:4095], Noise profile modulation gain
    u32    rnrBltLfNpStr;            /// default 256, [0:256], 2DNR strength
} rnr_core_param_by_gain_t;

typedef struct
{
    u32    rnrRadialLutTab[33];
} rnr_radial_lut_param_by_gain_t;

typedef struct
{
    u32    rnrWeightLutTab[65];        /// default 0, [0:100], NLM filter strength control
} rnr_weight_lut_param_by_gain_t;

typedef struct
{
    /// static parameters
    BOOL rnrTuneEnable;   /// default 1, [0:1], RNR tuning enable/disable

    /// dynamic parameters
    DYNAMIC_TABLE(rnr_core_param_by_gain_t, rnrGainInterpTab);
    DYNAMIC_TABLE(rnr_radial_lut_param_by_gain_t, rnrRadialLutTab);
    DYNAMIC_TABLE(rnr_weight_lut_param_by_gain_t, rnrWeightLutTab);
} tune_rnr_param_t;

#endif

#endif

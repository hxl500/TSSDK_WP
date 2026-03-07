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

#ifndef __TUNE_ISP_NP_V0_H__
#define __TUNE_ISP_NP_V0_H__

#include "tune_utils.h"

/***************************************************
*                      NP                           *
 ***************************************************/
///NP region params auto by Gain
typedef struct
{
    u32    NPLutTab[128];
} np_lut_param_by_gain_t;

typedef struct
{
    /// static parameters
    BOOL npTuneEnable;  /// default 1, [0:1], NP tuning enable/disable
    u32  LGain;         /// default 128, [1:4096], NP moduation gain for L frame, 128 for 1x
    u32  MGain;         /// default 182, [1:4096], NP moduation gain for M frame, 128 for 1x
    u32  TransGain;     /// default 0, [0:1024], blending weighting for trans zone, 1024 for 100%
    u32  FitOrder;      /// default 4, [1:5], blending curve fitting order of trans zone

    /// dynamic parameters
    DYNAMIC_TABLE(np_lut_param_by_gain_t, npLutTab);
} tune_np_param_t;

#endif

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

#ifndef __TUNE_ISP_CCM_V0_H__
#define __TUNE_ISP_CCM_V0_H__

#include "tune_utils.h"

/***************************************************
*                      CCM                          *
 ***************************************************/
typedef struct
{
    u32 ccmSaturation;      /// default 256, [0:512],
} ccm_core_param_by_gain_t;

typedef struct
{
    u32 ccmSaturation;      /// default 256, [0:512],
} ccm_core_param_by_lux_t;

/// It's for color api configuration
typedef struct
{
    /// static parameters
    BOOL ccmTuneEnable;       /// default 1, [0:1], CCM tuning enable/disable

    /// dynamic parameters
    DYNAMIC_TABLE(ccm_core_param_by_gain_t, ccmGainInterpTab);
    DYNAMIC_TABLE(ccm_core_param_by_lux_t, ccmLuxInterpTab);
} tune_ccm_param_t;

#endif

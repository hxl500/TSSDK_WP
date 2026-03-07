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

#ifndef __TUNE_ISP_SDE_V0_H__
#define __TUNE_ISP_SDE_V0_H__

#include "tune_utils.h"


/***************************************************
*                      SDE                          *
 ***************************************************/
/// SDE Params auto by lux
typedef struct
{
    u32  sdeContrastY;       /// default 1024, [0:4096]
} sde_core_param_by_lux_t;

/// SDE Params auto interpolate by gain
typedef struct
{
    u32  sdeSatRatioU;       /// sde staturation ratio for u, default 1024, [0,4096]
    u32  sdeSatRatioV;       /// sde staturation ratio for v, default 1024, [0,4096]
} sde_core_param_by_gain_t;
typedef struct
{
    /// static parameters
    BOOL sdeTuneEnable;   /// default 1, [0:1], SDE tuning enable/disable
    BOOL sdeNegEn;        /// default 0,[0:1]
    u16  sdeBrightness;   /// default 1024, [0:4096]
    u16  sdeSaturation;   /// default 1024, [0:4096]
    u16  sdeHue;          /// default 0, [0:63]
    u16  sdeClipYLow;     /// default 0, [0:1023]
    u16  sdeClipYHigh;    /// default 0, [0:1023]
    u16  sdeContrastY;    /// default 1024, [0:4096]
    u16  sdeContrastU;    /// default 1024, [0:4096]
    u16  sdeContrastV;    /// default 1024, [0:4096]

    DYNAMIC_TABLE(sde_core_param_by_lux_t, sdeLuxInterpTab);
    DYNAMIC_TABLE(sde_core_param_by_gain_t, sdeGainSwitchTab);
} tune_sde_param_t;

#endif

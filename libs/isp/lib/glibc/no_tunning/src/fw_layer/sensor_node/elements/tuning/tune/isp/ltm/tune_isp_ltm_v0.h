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

#ifndef __TUNE_ISP_LTM_V0_H__
#define __TUNE_ISP_LTM_V0_H__

#include "tune_utils.h"

/***************************************************
*                        LTM                        *
 ***************************************************/
/// LTM Params Auto by Ratio
typedef struct
{
    u32  ltmFilterTime;      /// default 6, [5:10], Hist filter time
    u32  ltmGlobalGain;      /// default 4095, [356:512], LTM global gain
} ltm_core_param_by_ratio_t;

/// LTM Params Auto by Gain
typedef struct
{
    u32  ltmBggen;           /// default 1, [0:1], LTM globel gain tuning enable/disable (to be used)
    u32  ltmAmplifyalpha;    /// default 4095, [0:4095], LTM amplify strength
} ltm_core_param_by_gain_t;

/// LTM Params auto by lux
typedef struct
{
    u32  ltmMaxGain;         /// default 220, [0:1024], Dark area brighten strength
    u32  ltmAmplifyalpha;    /// default 4095, [0:4095], LTM amplify strength
    u32  ltmDarkPrc;         /// default 10, [0:1000], Hist percent to calculate dart point
    u32  ltmBrightPrc;       /// default 99, [0:1000], Hist percent to calculate bright point
    u32  ltmDarkContrastMin; /// default 2, [0:1023], Min contrast to calculate proper gain/lighten
    u32  ltmDarkContrastMax; /// default 30, [0:1023], Max contrast to calculate proper gain/lighten
    u32  ltmMinLighten;      /// default 0, [0:255], LTM min lighten value
    u32  ltmMaxLighten;      /// default 120, [0:255], LTM max lighten value
    u32  ltmGdgEnable;       /// default 1, [0:1], enable global digital gain
    u32  ltmTargetLdr;       /// default 184, [1:1023], Set the target SDR value, which is the value LTM try to boot the local image patch
    u32  ltmHiTargetPrc;     /// default 99, [0:100], Control how many percentage of pixels should not exposed by LTM digital gain
    u32  ltmDarkLevel;       /// default 5, [0:1023], LTM dark level
    u32  ltmDarkMinPrc;      /// default 30, [0:100], LTM dark percent min threshold
    u32  ltmDarkMaxPrc;      /// default 50, [0:100], LTM dark percent max threshold
    u32  ltmDarkGainRatio;   /// default 100, [100:1000], LTM max gain ratio for dark percent larger than ltmDarkMaxPrc
    u32  ltmAmplifyalphaMax; /// default 4095, [0:4095], LTM amplify strength for dark percent larger than ltmDarkMaxPrc
    u32  ltmHistOffsetMin;   /// use [ltmHistOffsetMin, ltmHistOffsetMax] and constast to interpolate current hist_offset
    u32  ltmHistOffsetMax;   /// the bigger hist_offset, the grayscale histogram moves to the right
} ltm_core_param_by_lux_t;

/// LTM asymmetry lut by dark percent
typedef struct
{
    u32    ltmCurveInterpTab[65];        /// default 0, [0:65536], ltm asymmetry lut
} ltm_curve_lut_by_ratio_t;

typedef struct
{
    /// static parameters
    BOOL ltmTuneEnable;              /// default 1, [0:1], LTM tuning enable/disable
    u8   ltmSpatialFilterTime;     /// default 6, [0:31], Hist filter time

    /// dynamic parameters
    DYNAMIC_TABLE(ltm_core_param_by_ratio_t, ltmRatioInterpTab);
    DYNAMIC_TABLE(ltm_core_param_by_gain_t, ltmGainInterpTab);
    DYNAMIC_TABLE(ltm_core_param_by_lux_t, ltmLuxInterpTab);
    DYNAMIC_TABLE(ltm_core_param_by_lux_t, ltmHlSupLuxInterpTab);
    DYNAMIC_TABLE(ltm_curve_lut_by_ratio_t, ltmCurveInterpTab);
} tune_ltm_param_t;

#endif

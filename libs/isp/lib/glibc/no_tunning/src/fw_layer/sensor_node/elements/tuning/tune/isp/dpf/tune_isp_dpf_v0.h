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

#ifndef __TUNE_ISP_DPF_V0_H__
#define __TUNE_ISP_DPF_V0_H__

#include "tune_utils.h"

/***************************************************
*                      DPF                          *
 ***************************************************/
typedef struct
{
    u32 dpfByp;           /// default 0, [0:1], DPF bypass mode enable/disable
} dpf_switch_param_by_gain_t;

typedef struct
{
    u32  dpfSadThr      ; /// default 1200, [0:4095], SAD map : threshold value
    u32  dpfSadSlp      ; /// default 1024, [0:4095], SAD map : slope value
    u32  dpfHue1Th      ; /// default 1690, [0:4095], Hue map : threshold1 value
    u32  dpfHue1Slope   ; /// default 284, [0:4095], Hue map : slope1 value
    u32  dpfHue2Th      ; /// default 2150, [0:4095], Hue map : threshold2 value
    u32  dpfHue2Slope   ; /// default 1422, [0:4095], Hue map : slope2 value
    u32  dpfHueStr      ; /// default 768, [0:4095], Hue map strength
    u32  dpfSat1Th      ; /// default 164, [0:4095], Saturation map : threshold1 value
    u32  dpfSat1Slope   ; /// default 123, [0:4095], Saturation map : slope1 value
    u32  dpfSatStr      ; /// default 512, [0:4095], Saturation map strength
    u32  dpfLuma11Th    ; /// default 0, [0:4095], Contrast map : Luma1/threshold1 value (This will be Manual in future version)
    u32  dpfLuma11Slope ; /// default 1024, [0:4095], Contrast map : Luma1/slope1 value
    u32  dpfLuma12Th    ; /// default 1500, [0:4095], Contrast map: Luma1/threshold2 value
    u32  dpfLuma12Slope ; /// default 0, [0:4095], Contrast map : Luma1/slope2 value (This will be Manual in future version)
    u32  dpfLuma21Th    ; /// default 3450, [0:4095], Contrast map : Luma2/threshold1 value
    u32  dpfLuma21Slope ; /// default 1024, [0:4095], Contrast map : Luma2/slope1 value
    u32  dpfLuma22Th    ; /// default 3900, [0:4095], Contrast map : Luma2/threshold2 value
    u32  dpfLuma22Slope ; /// default 0, [0:4095], Contrast map : Luma2/slope2 value (This will be Manual in future version)
    u32  dpfLumaStr     ; /// default 512, [0:4095], Contrast map strength
    u32  dpfHslTh       ; /// default 0, [0:4095], Hue/Saturation/Contrast merged mask map: threshold value
    u32  dpfHslSlope    ; /// default 36, [0:4095], Hue/Saturation/Contrast merged mask map: slope value
    u32  dpfPurpleStr   ; /// default 511, [0:4095], Final DPF mask map strength
    u32  dpfSaturStr    ; /// default 0, [0:255], DPF strength
}dpf_core_param_by_gain_t;

/// DPF Strength params auto by Gain
typedef struct
{
    u32 dpfStrLutTab[33];
} dpf_str_lut_by_gain_t;

typedef struct
{
    /// static parameters
    BOOL dpfTuneEnable;     /// default 1, [0:1], DPF tuning enable/disable
    u32 dpfOffCenMultS;   /// default 4, [0:31]
    u32 dpfOffCenMult;    /// default 45252, [0:65535]
    u32 dpfCenX;          /// default 100, [0:8191]
    u32 dpfCenY;          /// default 100, [0:8191]

    /// dynamic parameters
    DYNAMIC_TABLE(dpf_switch_param_by_gain_t, dpfGainSwitchTab);
    DYNAMIC_TABLE(dpf_core_param_by_gain_t, dpfGainInterpTab);
    DYNAMIC_TABLE(dpf_str_lut_by_gain_t, dpfStrLutTab);
} tune_dpf_param_t;

#endif

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

#ifndef __TUNE_ISP_CNR_V0_H__
#define __TUNE_ISP_CNR_V0_H__

#include "tune_utils.h"

/***************************************************
*                      CNR                          *
 ***************************************************/
typedef struct
{
    u32  cnrLumaNoiseMid     ; /// default 10, [0:11]
    u32  cnrLumaNoiseEnd     ; /// default 10, [0:11]
    u32  cnrRfiltSatMid      ; /// default 5, [0:10]
    u32  cnrRfiltSatEnd      ; /// default 5, [0:10]
    u32  cnrRfiltDiffMid     ; /// default 5, [0:11]
    u32  cnrRfiltDiffEnd     ; /// default 5, [0:11]
    u32  cnrUvBlendMid       ; /// default 9, [0:11]
    u32  cnrUvBlendEnd       ; /// default 9, [0:11]
    u32  cnrFcSupMid         ; /// default 5, [0:11]
    u32  cnrFcSupEnd         ; /// default 5, [0:11]
} cnr_switch_param_by_gain_t;

typedef struct
{
    u32  cnrMotionNoiseGain  ; /// default 256, [0:16383]
    u32  cnrLumaNoiseOffset  ; /// default 0, [0:1024]
    u32  cnrLumaNoiseY1      ; /// default 10, [0:1023]
    u32  cnrLumaNoiseY2      ; /// default 10, [0:1023]
    u32  cnrLumaNoiseY3      ; /// default 10, [0:1023]
    u32  cnrRfiltSatOffset   ; /// default 0, [0:512]
    u32  cnrRfiltSatY1       ; /// default 256, [0:256]
    u32  cnrRfiltSatY2       ; /// default 128, [0:256]
    u32  cnrRfiltSatY3       ; /// default 0, [0:256]
    u32  cnrRfiltDiffOffset  ; /// default 0, [0:1024]
    u32  cnrRfiltDiffY1      ; /// default 200, [0:256]
    u32  cnrRfiltDiffY2      ; /// default 100, [0:256]
    u32  cnrRfiltDiffY3      ; /// default 0, [0:256]
    u32  cnrGaussNoiseOffset ; /// default 0, [0:128]
    u32  cnrGaussNoiseGain   ; /// default 256, [0:16383]
    u32  cnrUvdiffSft        ; /// default 2, [0:15]
    u32  cnrUvBlendOffset    ; /// default 512, [0:1024]
    u32  cnrUvBlendY1        ; /// default 256, [0:256]
    u32  cnrUvBlendY2        ; /// default 230, [0:256]
    u32  cnrUvBlendY3        ; /// default 156, [0:256]
    u32  cnrFcSupOffset      ; /// default 32, [0:1024]
    u32  cnrFcSupY1          ; /// default 200, [0:256]
    u32  cnrFcSupY2          ; /// default 100, [0:256]
    u32  cnrFcSupY3          ; /// default 0, [0:256]
} cnr_core_param_by_gain_t;

typedef struct
{
    u32 cnrRadialLutTab[33];  /// default 2, [0:100]
} cnr_radial_lut_param_t;

typedef struct
{
    /// static parameters
    BOOL cnrTuneEnable;        /// default 1, [0:1], CNR tuning enable/disable

    /// dynamic parameters
    DYNAMIC_TABLE(cnr_switch_param_by_gain_t, cnrGainSwitchTab);
    DYNAMIC_TABLE(cnr_core_param_by_gain_t, cnrGainInterpTab);
    DYNAMIC_TABLE(cnr_radial_lut_param_t, cnrRadialLutTab);
} tune_cnr_param_t;

#endif

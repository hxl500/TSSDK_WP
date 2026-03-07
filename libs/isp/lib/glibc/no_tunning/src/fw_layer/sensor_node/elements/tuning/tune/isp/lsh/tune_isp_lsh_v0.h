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

#ifndef __TUNE_ISP_LSH_V0_H__
#define __TUNE_ISP_LSH_V0_H__

#include "tune_utils.h"

/***************************************************
*                      LSH                          *
 ***************************************************/
typedef struct
{
    u32  lshLumMid            ; /// default 6, [0:11]
    u32  lshLumEnd            ; /// default 6, [0:11]
    u32  lshEdgeThinMid       ; /// default 5, [0:11]
    u32  lshEdgeThinEnd       ; /// default 6, [0:11]
    u32  lshNrFreqMid         ; /// default 4, [0:11]
    u32  lshNrFreqEnd         ; /// default 4, [0:11]
    u32  lshDetailType        ; /// default 0, [0:1]
    u32  lshDetailFreqMid     ; /// default 4, [0:11]
    u32  lshDetailFreqEnd     ; /// default 4, [0:11]
    u32  lshDetailLumMid      ; /// default 8, [0:11]
    u32  lshDetailLumEnd      ; /// default 8, [0:11]
    u32  lshDltiEn            ; /// default 1, [0:1] (not used)
    u32  lshDltiPosMid        ; /// default 7, [0:11]
    u32  lshDltiPosEnd        ; /// default 6, [0:11]
    u32  lshDltiNegMid        ; /// default 7, [0:11]
    u32  lshDltiNegEnd        ; /// default 8, [0:11]
} lsh_switch_param_by_gain_t;

typedef struct
{
    u32  lshDetailType        ; /// default 0, [0:1]
    u32  lshDetailFreqMid     ; /// default 4, [0:11]
    u32  lshDetailFreqEnd     ; /// default 4, [0:11]
    u32  lshDetailLumMid      ; /// default 8, [0:11]
    u32  lshDetailLumEnd      ; /// default 8, [0:11]
    u32  lshDltiPosMid        ; /// default 7, [0:11]
    u32  lshDltiPosEnd        ; /// default 6, [0:11]
    u32  lshDltiNegMid        ; /// default 7, [0:11]
    u32  lshDltiNegEnd        ; /// default 8, [0:11]
} lsh_switch_param_by_lux_t;

typedef struct
{
    u32  lshMotionR           ; /// default 256, [0:256
    u32  lshLumOffset         ; /// default 32, [0:1024]
    u32  lshLumY1             ; /// default 0, [0:1023]
    u32  lshLumY2             ; /// default 0, [0:1023]
    u32  lshLumY3             ; /// default 0, [0:1023]
    u32  lshEdgeNlmSlp        ; /// default 256, [0:4095]
    u32  lshEdgeN2wNormR      ; /// default 512, [0:4095]
    u32  lshEdgeThinOffset    ; /// default 0, [0:1024]
    u32  lshEdgeThinY1        ; /// default 150, [0:256]
    u32  lshEdgeThinY2        ; /// default 100, [0:256]
    u32  lshEdgeThinY3        ; /// default 0, [0:256]
    u32  lshNrFreqOffset      ; /// default 8, [0:1024]
    u32  lshNrFreqY1          ; /// default 0, [0:256]
    u32  lshNrFreqY2          ; /// default 0, [0:256]
    u32  lshNrFreqY3          ; /// default 0, [0:256]
    u32  lshNegStr1           ; /// default 256, [0:16383]
    u32  lshPosStr1           ; /// default 256, [0:16383]
    u32  lshNegMinOffset1     ; /// default 0, [0:1023]
    u32  lshPosMaxOffset1     ; /// default 0, [0:1023]
    u32  lshNegClipRatio1     ; /// default 100, [0:1023] (This will be Manual in future version)
    u32  lshPosClipRatio1     ; /// default 100, [0:1023] (This will be Manual in future version)
    u32  lshDetailNwAlpha     ; /// default 70, [0:256] (This will be Manual in future version)
    u32  lshDetailNrR         ; /// default 100, [0:256]  (This will be Manual in future version)
    u32  lshDetailFreqOffset  ; /// default 0, [0:1024]
    u32  lshDetailFreqY1      ; /// default 256, [0:4095]
    u32  lshDetailFreqY2      ; /// default 256, [0:4095]
    u32  lshDetailFreqY3      ; /// default 256, [0:4095]
    u32  lshDetailLumOffset   ; /// default 128, [0:1024]
    u32  lshDetailLumY1       ; /// default 256, [0:4095]
    u32  lshDetailLumY2       ; /// default 256, [0:4095]
    u32  lshDetailLumY3       ; /// default 256, [0:4095]
    u32  lshNegStr2           ; /// default 500, [0:16383]
    u32  lshPosStr2           ; /// default 500, [0:16383]
    u32  lshNegMinOffset2     ; /// default 1023, [0:1023]
    u32  lshPosMaxOffset2     ; /// default 1023, [0:1023]
    u32  lshNegClipRatio2     ; /// default 0, [0:1023] (This will be Manual in future version)
    u32  lshPosClipRatio2     ; /// default 0, [0:1023] (This will be Manual in future version)
    u32  lshDltiPosOffset     ; /// default 64, [0:1024]
    u32  lshDltiFinalPosY1    ; /// default 256, [0:256]
    u32  lshDltiFinalPosY2    ; /// default 180, [0:256]
    u32  lshDltiFinalPosY3    ; /// default 50, [0:256]
    u32  lshDltiNegOffset     ; /// default 64, [0:1024]
    u32  lshDltiFinalNegY1    ; /// default 256, [0:256]
    u32  lshDltiFinalNegY2    ; /// default 200, [0:256]
    u32  lshDltiFinalNegY3    ; /// default 150, [0:256]
} lsh_core_param_by_gain_t;

typedef struct
{
    u32  lshEdgeNlmWeightTab[32]; /// default 2, [0:100]
} lsh_weight_lut_param_by_gain_t;

typedef struct
{
    u32  lshDetailFreqOffset  ; /// default 0, [0:1024]
    u32  lshDetailFreqY1      ; /// default 256, [0:4095]
    u32  lshDetailFreqY2      ; /// default 256, [0:4095]
    u32  lshDetailFreqY3      ; /// default 256, [0:4095]
    u32  lshDetailLumOffset   ; /// default 128, [0:1024]
    u32  lshDetailLumY1       ; /// default 256, [0:4095]
    u32  lshDetailLumY2       ; /// default 256, [0:4095]
    u32  lshDetailLumY3       ; /// default 256, [0:4095]
    u32  lshDltiPosOffset     ; /// default 64, [0:1024]
    u32  lshDltiFinalPosY1    ; /// default 256, [0:256]
    u32  lshDltiFinalPosY2    ; /// default 180, [0:256]
    u32  lshDltiFinalPosY3    ; /// default 50, [0:256]
    u32  lshDltiNegOffset     ; /// default 64, [0:1024]
    u32  lshDltiFinalNegY1    ; /// default 256, [0:256]
    u32  lshDltiFinalNegY2    ; /// default 200, [0:256]
    u32  lshDltiFinalNegY3    ; /// default 150, [0:256]
} lsh_core_param_by_lux_t;

typedef struct
{
    /// static parameters
    BOOL lshTuneEnable;     /// default 1, [0:1], LSH tuning enable/disable

    /// dynamic parameters
    DYNAMIC_TABLE(lsh_switch_param_by_gain_t, lshGainSwitchTab);
    DYNAMIC_TABLE(lsh_core_param_by_gain_t, lshGainInterpTab);
    DYNAMIC_TABLE(lsh_weight_lut_param_by_gain_t, lshWeightLutTab);
    DYNAMIC_TABLE(lsh_switch_param_by_lux_t, lshLuxSwitchTab);
    DYNAMIC_TABLE(lsh_core_param_by_lux_t, lshLuxInterpTab);
} tune_lsh_param_t;

#endif

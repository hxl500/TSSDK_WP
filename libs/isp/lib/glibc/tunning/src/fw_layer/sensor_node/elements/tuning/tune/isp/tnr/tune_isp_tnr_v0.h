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

#ifndef __TUNE_ISP_TNR_V0_H__
#define __TUNE_ISP_TNR_V0_H__

#if (defined(HW_BUILD_FOR_I5)       ||\
     defined(HW_BUILD_FOR_I5_R1P0)  ||\
     defined(HW_BUILD_FOR_I3)       ||\
     defined(HW_BUILD_FOR_CUST0))

#include "tune_utils.h"

/***************************************************
*                      TNR                          *
 ***************************************************/
typedef struct
{
    u32  tnrNr2dPyramidWt       ; /// default 2, [0:2], 2DNR low frequency processing strength. Should be checked with Algorithm if 1 parameter controlls all
    u32  tnrMotionCurveMid      ; /// default 6, [0:11], Control motion tail.
    u32  tnrMotionCurveEnd      ; /// default 6, [0:11], Control motion tail.
    u32  tnrNr2dCurveMid        ; /// default 7, [0:9], 2DNR blending by motion.
    u32  tnrNr2dCurveEnd        ; /// default 7, [0:9], 2DNR blending by motion.
    u32  tnrNr3dCurveMid        ; /// default 7, [0:9], 3DNR blending by motion.
    u32  tnrNr3dCurveEnd        ; /// default 7, [0:9], 3DNR blending by motion.
} tnr_switch_param_by_gain_t;

typedef struct
{
    u32  tnrGlobalNoiseThr      ; /// default 160, [0:1023], Adaptive noise threshold. Notes: [0] Disable, less adaptive noise threshold.
    u32  tnrNr2dGradSlope       ; /// default 32, [0:1023], 2DNR edge gradient detection. Notes: smoother 2DNR edge.
    u32  tnrNr2dYnoiseThr       ; /// default 20, [0:1023], 2DNR Y noise threshold. Notes: [0] Disable; Most blur, less Y noise.
    u32  tnrNr2dUVnoiseThr      ; /// default 20, [0:1023], 2DNR UV noise threshold. Notes: [0] Disable; Most blur, less UV noise.
    u32  tnrNr2dNoiseBackRatio  ; /// default 40, [0:64], motion noise addback ratio Notes: [0] no noise addback
    u32  tnrMotionThr           ; /// default 4, [0:13], Make sure the motion area has less hole.
    u32  tnrMotionCurveOffset   ; /// default 0, [0:1024], Control motion tail.
    u32  tnrMotionCurveY1       ; /// default 20, [0:64], Blending ratio. Notes: less motion ghost tail.
    u32  tnrMotionCurveY2       ; /// default 32, [0:64], Blending ratio. Notes: less motion ghost tail.
    u32  tnrMotionCurveY3       ; /// default 64, [0:64], Blending ratio. Notes: less motion ghost tail.
    u32  tnrNr2dCurveOffset     ; /// default 0, [0:256], 2DNR blending by motion.
    u32  tnrNr2dCurveY1         ; /// default 0, [0:64], 2DNR denoise strength for Y Notes: more detail and more noise.
    u32  tnrNr2dCurveY2         ; /// default 16, [0:64], 2DNR denoise strength for Y Notes: more detail and more noise.
    u32  tnrNr2dCurveY3         ; /// default 32, [0:64], 2DNR denoise strength for Y Notes: more detail and more noise.
    u32  tnrNr2dCurveC1         ; /// default 0, [0:64], 2DNR denoise strength for C Notes: more detail and more noise.
    u32  tnrNr2dCurveC2         ; /// default 16, [0:64], 2DNR denoise strength for C Notes: more detail and more noise.
    u32  tnrNr2dCurveC3         ; /// default 32, [0:64], 2DNR denoise strength for C Notes: more detail and more noise.
    u32  tnrNr3dCurveOffset     ; /// default 0, [0:256], 3DNR blending by motion.
    u32  tnrNr3dCurveY1         ; /// default 48, [0:64], 3DNR denoise strength for Y Notes: less and stiller noise.
    u32  tnrNr3dCurveY2         ; /// default 22, [0:64], 3DNR denoise strength for Y Notes: less and stiller noise.
    u32  tnrNr3dCurveY3         ; /// default 12, [0:64], 3DNR denoise strength for Y Notes: less and stiller noise.
    u32  tnrNr3dCurveC1         ; /// default 44, [0:64], 3DNR denoise strength for C Notes: less and stiller noise.
    u32  tnrNr3dCurveC2         ; /// default 22, [0:64], 3DNR denoise strength for C Notes: less and stiller noise.
    u32  tnrNr3dCurveC3         ; /// default 0, [0:64], 3DNR denoise strength for C Notes: less and stiller noise.
} tnr_core_param_by_gain_t;

typedef struct
{
    /// static parameters
    BOOL tnrTuneEnable;       /// default 1, [0:1], TNR tuning enable/disable

    /// dynamic parameters
    DYNAMIC_TABLE(tnr_switch_param_by_gain_t, tnrGainSwitchTab);
    DYNAMIC_TABLE(tnr_core_param_by_gain_t, tnrGainInterpTab);
} tune_tnr_param_t;

#endif

#endif

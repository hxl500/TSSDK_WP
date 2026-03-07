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

#ifndef __TUNE_ISP_MVD_V0_H__
#define __TUNE_ISP_MVD_V0_H__

#if (defined(HW_BUILD_FOR_I5)       ||\
     defined(HW_BUILD_FOR_I5_R1P0)  ||\
     defined(HW_BUILD_FOR_I3)       ||\
     defined(HW_BUILD_FOR_CUST0))

#include "tune_utils.h"

/***************************************************
*                      MVD                          *
 ***************************************************/
typedef struct
{
    u32  mvdPipeBErode1;        /// default 0, [0:5], Erode 1 processing size. Notes: [0] Disable: less noise.
    u32  mvdPipeBDilate;        /// default 15, [0:7], Dilate processing size. Notes: [0] Disable: better foreground.
    u32  mvdPipeBErode2;        /// default 9, [0:7], Erode 2 processing size. Notes: [0] Disable: better foreground and larger ghost tail.
    u32  mvdPipeBMapRefineThr;  /// default 5, [0:8], Map refine threshold Notes: less foreground hole.
} mvd_switch_param_by_gain_t;

typedef struct
{
    u32  mvdNpGain;             /// default 16, [0:1023], Noise Profile gain control. Notes: Less Noise but foreground may be incomplete
    u32  mvdNpOffset;           /// default 5, [0:4095], Noise Profile offset control. Notes: All still, no motion detected. ��: smoother motion vector and less noise.
    u32  mvdPipeaThr;           /// default 10, [0:25], The motion threshold in Pipe A. Notes: Previous motion. smoother motion vector and less noise.
    u32  mvdPipeABTriDirStr;    /// default 0, [0:256], Make Motion map smoother
} mvd_core_param_by_gain_t;

typedef struct
{
    /// static parameters
    BOOL mvdTuneEnable;             /// default 1, [0:1], MVD tuning enable/disable
    u32  mvdPipeABSadBitdepth;    /// default 12, [0:15]
    u32  mvdLosstCompMode;        /// default 3, [0:3]
    u32  mvdPipeABRGB2YCR;        /// default 77, [0:511]
    u32  mvdPipeABRGB2YCG;        /// default 150, [0:511]
    u32  mvdPipeABRGB2YCB;        /// default 29, [0:511]

    /// dynamic parameters
    DYNAMIC_TABLE(mvd_switch_param_by_gain_t, mvdGainSwitchTab);
    DYNAMIC_TABLE(mvd_core_param_by_gain_t, mvdGainInterpTab);
} tune_mvd_param_t;

#endif

#endif

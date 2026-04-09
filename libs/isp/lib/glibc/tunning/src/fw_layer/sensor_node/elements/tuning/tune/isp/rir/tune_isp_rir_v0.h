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

#ifndef __TUNE_ISP_RIR_V0_H__
#define __TUNE_ISP_RIR_V0_H__

#include "tune_utils.h"


/***************************************************
*                      RIR                          *
 ***************************************************/

typedef struct
{
    /// static parameters
    BOOL rirTuneEnable;            /// default 1,    [0:1], RIR tuning enable/disable
    BOOL rirDirMeanEn;             /// default 0,    [0:1]
    BOOL rirGradFilterEn;          /// default 1,    [0:1]
    u16  rirInArray;               /// default 5,    [0:7]
    u16  rirGradMeanThd1;          /// default 50,   [0:4096]
    u16  rirGradMeanThd2;          /// default 250,  [0:4096]
    u16  rirGradVarThd1;           /// default 50,   [0:4096]
    u16  rirGradVarThd2;           /// default 225,  [0:4096]
    u16  rirGradSimVar;            /// default 100,  [0:4096]
    BOOL rirOverExpEn;             /// default 1  ,  [0:1]
    u16  rirOverExpThd1;           /// default 3500, [0:4096]
    u16  rirOverExpThd2;           /// default 3800, [0:4096]
    u16  rirOverExpRGain;          /// default 256,  [0:4096]
    u16  rirOverExpBGain;          /// default 256,  [0:4096]
    u16  rirOverExpDesatStr;       /// default 256,  [0:384]
    u16  rirOverExpDifHueMin;      /// default 100,  [60:180]
    BOOL rirBlcInEn;               /// default 1,    [0:1]
    BOOL rirBlcOutEn;              /// default 1,    [0:1]
    u16  rirDebugMode;             /// default 0,    [0:6]
} tune_rir_param_t;

#endif

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

#ifndef __TUNE_ISP_DPC_V0_H__
#define __TUNE_ISP_DPC_V0_H__

#include "tune_utils.h"

/***************************************************
*                      DPC                          *
 ***************************************************/
typedef struct
{
    u32  dpcByp;          /// default 0, [0:1], DPC bypass mode enable/disable
    u32  dpcGeEn;         /// default 0, [0:1], Green equalization tuning enable/disable
} dpc_switch_param_by_gain_t;

typedef struct
{
    u32  dpcIntplAlpha;   /// default 0, [0:256], Alpha value from correction value to median value for defective pixel
    u32  dpcDivSlp;       /// default 1, [0:4095], Deviation  adjust slope
    u32  dpcDivThr;       /// default 0, [0:4095], Deviation  adjust threshold (This will be Manual in future version)
    u32  dpcStdevMaskThr; /// default 10, [0:4095], Deviation  judgement threshold
    u32  dpcDpDisThr;     /// default 200, [0:4095], Edge(not defective) judgement threshold
    u32  dpcCurMedThr;    /// default 16, [0:4095], Current pixel ratio threshold to median value for judgement
    u32  dpcGeStr;        /// default 100, [0:4095], Green equalization strength
    u32  dpcGeThr;        /// default 0, [0:255], GE judgement mask threshold (This will be Manual in future version)
    u32  dpcGeSen;        /// default 256, [0:4095], GE local deviation  sensitivity (This will be Manual in future version)
    u32  dpcGeSlope;      /// default 256, [0:4095], GE judgement mask slope (This will be Manual in future version)
    u32  dpcNpGain;       /// default 64, [0:4095], GE noise profile modulation gain
    u32  dpcNpOffset;     /// default 0, [0:4095], GE noise profile modulation offset
} dpc_core_param_by_gain_t;

/// DPC tune param
typedef struct
{
    /// static parameters
    BOOL dpcTuneEnable;          /// default 1, [0:1], DPC tuning enable/disable

    /// dynamic parameters
    DYNAMIC_TABLE(dpc_switch_param_by_gain_t, dpcGainSwitchTab);
    DYNAMIC_TABLE(dpc_core_param_by_gain_t, dpcGainInterpTab);
} tune_dpc_param_t;

#endif

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

#ifndef __TUNE_ISP_BLC_V0_H__
#define __TUNE_ISP_BLC_V0_H__

#include "tune_utils.h"

/***************************************************
*                      BLC                          *
 ***************************************************/
typedef struct
{
    u32 normalBlcR;      /// default 0, [0:65535]
    u32 normalBlcGr;     /// default 0, [0:65535]
    u32 normalBlcGb;     /// default 0, [0:65535]
    u32 normalBlcB;      /// default 0, [0:65535]
    u32 lncBlcL;         /// default 0, [0:4095]
    u32 lncBlcM;         /// default 0, [0:4095]
    u32 lncBlcS;         /// default 0, [0:4095]
    u32 lncBlcVs;        /// default 0, [0:4095]
    u32 dolLBlcR;        /// default 0, [0:4095]
    u32 dolLBlcGr;       /// default 0, [0:4095]
    u32 dolLBlcGb;       /// default 0, [0:4095]
    u32 dolLBlcB;        /// default 0, [0:4095]
    u32 dolMBlcR;        /// default 0, [0:4095]
    u32 dolMBlcGr;       /// default 0, [0:4095]
    u32 dolMBlcGb;       /// default 0, [0:4095]
    u32 dolMBlcB;        /// default 0, [0:4095]
    u32 dolSBlcR;        /// default 0, [0:4095]
    u32 dolSBlcGr;       /// default 0, [0:4095]
    u32 dolSBlcGb;       /// default 0, [0:4095]
    u32 dolSBlcB;        /// default 0, [0:4095]
} blc_core_param_by_gain_t;

typedef struct
{
    /// static parameters
    BOOL blcTuneEnable;    /// default 0, [0:1], BLC tuning enable/disable;
                         /// tuning blc will override calibration blc if blcTuneEnable is TRUE
    /// dynamic parameters
    DYNAMIC_TABLE(blc_core_param_by_gain_t, blcGainInterpTab);
} tune_blc_param_t;

#endif

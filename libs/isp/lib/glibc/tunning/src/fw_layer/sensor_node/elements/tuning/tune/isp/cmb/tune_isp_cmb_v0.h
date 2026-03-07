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

#ifndef __TUNE_ISP_CMB_V0_H__
#define __TUNE_ISP_CMB_V0_H__

#if (defined(HW_BUILD_FOR_I5)       ||\
     defined(HW_BUILD_FOR_I5_R1P0)  ||\
     defined(HW_BUILD_FOR_I3)       ||\
     defined(HW_BUILD_FOR_CUST0))

#include "tune_utils.h"

/***************************************************
*                      CMB                          *
 ***************************************************/

typedef struct
{
    u32  cmbNpMultLm       ; /// default 32, [0:65535]
    u32  cmbNpMultMs       ; /// default 32, [0:65535]
    u32  cmbThresh1Lm      ; /// default 3072, [0:4095]
    u32  cmbThresh2Lm      ; /// default 3840, [0:4095]
} cmb_core_param_by_gain_t;

typedef struct
{
    u32  cmbNpMultLm       ; /// default 32, [0:65535]
    u32  cmbNpMultMs       ; /// default 32, [0:65535]
    u32  cmbThresh1Lm      ; /// default 3072, [0:4095]
    u32  cmbThresh2Lm      ; /// default 3840, [0:4095]
} cmb_core_param_by_lux_t;

typedef struct
{
    u32                 cmbLutTab[128];
} cmb_Lut_param_by_gain_t;


typedef struct
{
    /// static parameters
    BOOL cmbTuneEnable       ; /// default 1, [0:1], CMB tuning enable/disable
    u16  cmbThresh1Lm      ; /// default 3072, [0:4095]
    u16  cmbThresh2Lm      ; /// default 3840, [0:4095]
    u16  cmbThresh1Ms      ; /// default 3072, [0:4095]
    u16  cmbThresh2Ms      ; /// default 3840, [0:4095]
    u16  cmbMovLmMult      ; /// default 400, [0:65535]
    u16  cmbMovMsMult      ; /// default 400, [0:65535]

    /// dynamic parameters
    DYNAMIC_TABLE(cmb_core_param_by_gain_t, cmbGainInterpTab);
    DYNAMIC_TABLE(cmb_core_param_by_lux_t, cmbLuxInterpTab);
    DYNAMIC_TABLE(cmb_Lut_param_by_gain_t, cmbLutTab);

} tune_cmb_param_t;

#endif

#endif

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

#ifndef __TUNE_COMMON_H__
#define __TUNE_COMMON_H__

#include "tune_utils.h"
#include "tune_sw3a_common.h"
#include "tune_isp_common.h"

typedef struct
{
    u32 reserved[10];
} tune_reserved_params_t;

/// root tuning params structure
typedef struct
{
    tune_version_t         tuneVersion;   /// tunning version, initial is 0.1.1.2
    tune_sw3a_params_t     sw3aParams[TUNE_ALG_SW3A_NUM_MAX];    /// 3a tunning parameters structure
    tune_isp_params_t      ispParams;     /// isp tuning parameters structure
    tune_reserved_params_t reserved;      /// reserved tunning paramters
} tune_params_t;


/*
* load_tuning
*
* @brief  Entor point for load tuning parameters
*
* @param filename: json filename, max_len: 8 + 3
*
* @return point to tune params
*
*/
tune_params_t* load_tuning(const char *filename);

typedef tune_params_t* (*load_tuning_func)(const char *filename);

#endif

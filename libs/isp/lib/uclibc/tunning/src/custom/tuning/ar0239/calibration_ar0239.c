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
#include "log_utils.h"
#include "calibration_common.h"
#include "params_printer.h"
#include "json_to_params.h"

static calibration_params_t g_calParams =
{
    #include "calibration_ar0239.h"
};

static BOOL g_isJsonLoaded = FALSE;

/// Load offline json setting
static void Load_Ar0239_Calibration_JSON(const char *filename)
{
    if (ISP_RET_SUCCESS == Json_To_Params_From_File(filename, JSON_CALIBRATION, (void*)&g_calParams))
    {
        g_isJsonLoaded = TRUE;
    }
}

/*
* Load_Ar0239_Calibration
*
* @brief Load calibration parameters of ar0239 from static headers or json
*
* @param filename: json filename, max_len: 8 + 3
*
* @return pointer of calibration_params_t
*
*/
calibration_params_t* Load_Ar0239_Calibration(const char *filename)
{
    FW_FUNC_ENTER();

    if (!g_isJsonLoaded)
    {
        Load_Ar0239_Calibration_JSON(filename);
    }

    return &g_calParams;
}

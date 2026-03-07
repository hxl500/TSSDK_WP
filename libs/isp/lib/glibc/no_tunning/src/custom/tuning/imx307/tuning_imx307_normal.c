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
#include "tune_common.h"
#include "params_printer.h"
#include "json_to_params.h"

static tune_params_t g_tuningParams =
{
    #if (defined(HW_BUILD_FOR_I5)       ||\
         defined(HW_BUILD_FOR_I5_R1P0)  ||\
         defined(HW_BUILD_FOR_I3)       ||\
         defined(HW_BUILD_FOR_CUST0))
    #include "tuning_imx307_normal_oct.h"
    #endif
};

static BOOL g_isJsonLoaded = FALSE;

/// Load offline json setting
static void Load_Imx307_Normal_Tuning_JSON(const char8 *filename)
{
    if (ISP_RET_SUCCESS == Json_To_Params_From_File(filename, JSON_TUNING, (void*)&g_tuningParams))
    {
        g_isJsonLoaded = TRUE;
        //Print_Struct(&g_tuningParams, PRINT_TYPE_TUNING, "307_nor.h");
    }
}

/*
* Load_Imx307_Normal_Tuning
*
* @brief Load tuning parameters of imx307 from static headers or json
*
* @param filename: json filename, max_len: 8 + 3
*
* @return pointer of tune_params_t
*
*/
tune_params_t* Load_Imx307_Normal_Tuning(const char8 *filename)
{
    FW_FUNC_ENTER();

    if (!g_isJsonLoaded)
    {
        Load_Imx307_Normal_Tuning_JSON(filename);
    }

    return &g_tuningParams;
}

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
  #include "tuning_k17_dol2.h"
};

static BOOL g_isJsonLoaded = FALSE;

/// Load offline json setting
static void load_k17_dol2_tuning_json(const char8 *filename)
{
    if (ISP_RET_SUCCESS == Json_To_Params_From_File(filename, JSON_TUNING, (void*)&g_tuningParams))
    {
        g_isJsonLoaded = TRUE;

        #ifdef PC_SIMU
        Print_Struct(&g_tuningParams, PRINT_TYPE_TUNING, "k17_dol2.h");
        #endif
    }
    else
    {
        #ifdef PC_SIMU
        Params_To_Json_File("tuning_dol2.json", JSON_TUNING, (void*)&g_tuningParams);
        #endif
    }
}

/*
* Load_k17_Dol2_Tuning
*
* @brief Load tuning parameters of imx415 from static headers or json
*
* @param filename: json filename, max_len: 8 + 3
*
* @return pointer of tune_params_t
*
*/
tune_params_t* load_k17_dol2_tuning(const char8 *filename)
{
    FW_FUNC_ENTER();

    if (!g_isJsonLoaded)
    {
        load_k17_dol2_tuning_json(filename);
    }

    return &g_tuningParams;
}


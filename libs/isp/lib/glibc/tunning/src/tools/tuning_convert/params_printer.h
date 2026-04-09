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
 * Copyright (C) 2020-2021 Arm Technology (China) Co. Ltd. All rights reserved.
 *
 ***********************************************************************************************/

#ifndef __PARAMS_PRINTER_H__
#define __PARAMS_PRINTER_H__

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include "tune_common.h"
#include "calibration_common.h"
#include "fs_osutils.h"

typedef struct
{
    u32             num;
    BOOL            isLut;
    const char8*    ppStrComment[64];
} tab_info_t;

typedef struct
{
    BOOL           isSaveToFile;
    FileHandle*    pFile;
    const char8*   filename;
    size_t         bytesWritten;
    /// Make sure buffer is large engouh to store liscence string
    char8          buffer[2048];
} params_save_info_t;

typedef enum
{
    PRINT_TYPE_TUNING,
    PRINT_TYPE_CALIBRATION,
} params_print_type_t;

/*
* Print_Struct
*
* @brief Print tuning or calibration struct to terminal or file
*
* @param pTuneParams: point to tuning or calibration params struct
*
* @param type: type of struct
*
* @param filename: filename path for struct storing, set it NULL if print to terminal
*
* @return ISP_RET_SUCCESS if success, others if failed
*
*/
isp_ret_t Print_Struct(void* pParams, params_print_type_t type, const char8* filename);

#endif

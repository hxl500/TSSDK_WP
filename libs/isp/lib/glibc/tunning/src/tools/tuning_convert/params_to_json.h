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

#ifndef __PARAMS_TO_JSON_H__
#define __PARAMS_TO_JSON_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tune_common.h"
#include "calibration_common.h"
#include "json_wrapper.h"
#include "fs_osutils.h"
#include "mem_osutils.h"

/*
* Params_To_Json
*
* @brief Parse tuning or calibration parameters from json file
*
* @param filename: json filename path
*
* @param type: json file type
*
* @param pParams: pointer to struct of tune or calibration
*
* @return ISP_RET_SUCCESS if success, others if failed
*
*/
#ifdef __cplusplus
extern "C"
{
#endif
    isp_ret_t Params_To_Json_File(const char8* filename, json_type_t type, void* pParams);
#ifdef __cplusplus
}
#endif

#endif

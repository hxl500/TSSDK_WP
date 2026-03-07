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

#ifndef __APP_USECASE_H__
#define __APP_USECASE_H__

#include "types_utils.h"
#include "log_utils.h"
#include "device.h"

typedef enum __app_usecase_type
{
    CAM_USECASE_SINGLE_STREAM = 1,          /// 0 --> 1 Stream
  //CAM_USECASE_DUAL_STREAM,                /// 1 --> 2 Streams
  //CAM_USECASE_TRIPLE_STREAM,              /// 2 --> 3 Streams
  //CAM_USECASE_QUATERNATE_STREAM,          /// 4 --> 4 Streams
} app_usecase_type_t;

typedef struct __app_usecase app_usecase_t;

typedef isp_ret_t (*app_usecase_initialize_func)(app_usecase_t* thiz,
                                                         callbacks_ops_t* pCbs);
typedef isp_ret_t (*app_usecase_execute_func)(app_usecase_t* thiz,
                                                     app_config_t* pAppCfg);
typedef isp_ret_t (*app_usecase_deinitialize_func)(app_usecase_t* thiz);

typedef enum __app_usecase_status
{
    CAM_USECASE_STATUS_UNINITIALIZED = 0,
    CAM_USECASE_STATUS_INITIALIZED,
    ///
} app_usecase_status_t;

struct __app_usecase
{
    cbs_t                                  usecaseCbs;
    app_usecase_status_t                   status;
    u32                                    camId;
    app_usecase_type_t                     usecaseType;
    app_config_t*                          pAppCfg;
    device_t*                              pDev;

    app_usecase_initialize_func            Initialize;
    app_usecase_execute_func               Execute;
    app_usecase_deinitialize_func          Deinitialize;
};

app_usecase_t* App_Usecase_Select_And_Get_Instance(app_config_t* pAppCfg);

#endif

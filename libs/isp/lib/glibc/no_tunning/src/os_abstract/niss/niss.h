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

#ifndef __NISS_H__
#define __NISS_H__

#include <stdio.h>
#include "types_utils.h"
#include "log_utils.h"

typedef struct __niss niss_t;

typedef isp_ret_t (*niss_initialize_func)(niss_t*      thiz);
typedef isp_ret_t (*niss_deinitialize_func)(niss_t* thiz);
typedef isp_ret_t (*niss_configure_func)(niss_t*      thiz, void* params);
typedef isp_ret_t (*niss_read_func)(niss_t* thiz, void* pReadBuf, u32* len);
typedef isp_ret_t (*niss_write_func)(niss_t* thiz, void* pWriteBuf, u32 len);
typedef isp_ret_t (*niss_execute_func)(niss_t* thiz, void* arg);


typedef enum __niss_type
{
    NISS_PLATFORM = 0,
    NISS_UART,
    NISS_ETHERNET,
    NISS_USB,
    NISS_HDMI,
    NISS_ETH_USB,
    NISS_TCPIP,
} niss_type_t;

typedef enum __niss_data_type
{
    NISS_TYPE_INVALID = 0,
    NISS_TYPE_STRING,
    NISS_TYPE_JSON,
    NISS_TYPE_SYSTEM_REBOOT,
    NISS_TYPE_ISP_REG,
    NISS_TYPE_SENSOR_REG = 5,
    NISS_TYPE_RESULT,
} niss_data_type_t;

struct __niss
{
    niss_type_t                     type;

    niss_initialize_func            Initialize;
    niss_deinitialize_func          Deinitialize;

    niss_configure_func             Configure;

    BOOL                            IsBigData;
    niss_read_func                  Read;
    niss_write_func                 Write;
    niss_execute_func               Execute;
};

niss_t* Niss_Get_Instance(niss_type_t type);

#endif

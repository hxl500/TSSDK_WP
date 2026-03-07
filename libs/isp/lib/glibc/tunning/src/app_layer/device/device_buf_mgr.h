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

#ifndef __Device_Buf_MGR_H__
#define __Device_Buf_MGR_H__

#include "types_utils.h"
#include "log_utils.h"
#include "hal_wrapper.h"

typedef struct __device_buf_mgr device_buf_mgr_t;

typedef stream_buffer_t* (*device_buf_get_func)(device_buf_mgr_t* thiz);

typedef isp_ret_t (*device_buf_return_func)(device_buf_mgr_t* thiz,
                                                  stream_buffer_t* pStreamBuf);

/// Device buffer manager descripor
struct __device_buf_mgr
{
    stream_info_t*                   pStreamInfo;

    device_buf_get_func              Get_Buf;
    device_buf_return_func           Return_Buf;
};

/// define struct to store device allocation information
typedef struct __device_buf_alloc_info
{
    stream_info_t*   pStreamInfo;
    u32              bufNum;
} device_buf_alloc_info_t;

/*
* Device_Buf_Mgr_Acquire
*
* @brief Acquire device buffer manager
*
* @param pStreamInfo: pointer to stream_info_t
*
* @param ppDevBufMgr: dimensional pointer to device buffer manager,
*                     which will be used to return buffer manager after acquring
*
* @return ISP_RET_SUCCESS if success, others if failed
*/
isp_ret_t Device_Buf_Mgr_Acquire(stream_info_t*        pStreamInfo,
                                       device_buf_mgr_t** ppDevBufMgr);

/*
* Device_Buf_Mgr_Release
*
* @brief Release device buffer manager
*
* @param ppDevBufMgr: dimensional pointer to device buffer manager
*
* @return ISP_RET_SUCCESS if success, others if failed
*/
isp_ret_t Device_Buf_Mgr_Release(device_buf_mgr_t** ppDevBufMgr);

#endif

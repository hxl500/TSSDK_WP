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

#ifndef __STREAM_BUF_MGR_H__
#define __STREAM_BUF_MGR_H__

#include "types_utils.h"
#include "log_utils.h"
#include "stream_types.h"
#include "obj_queue_utils.h"
#include "time_osutils.h"
#include "isp_stream_if.h"

#define MAX_BUFS_PER_STREAM (32)

typedef struct __stream_buf_mgr stream_buf_mgr_t;

typedef stream_buffer_t* (*stream_buf_get_func)(stream_buf_mgr_t* thiz);

typedef isp_ret_t (*stream_buf_return_func)(stream_buf_mgr_t*       thiz,
                                                  stream_buffer_t* pStreamBuf);

/// Stream buffer manager descriptor
struct __stream_buf_mgr
{
    u32                         id;
    stream_info_t               streamInfo;
    obj_queue_t*                pFreeBufQ;
    isp_stream_if_t*            pStreamIf;

    stream_buf_get_func         Get_Buf;
    stream_buf_return_func      Return_Buf;

	pthread_mutex_t             qlock;
};

/// Stream buffer allocate information struct
typedef struct __stream_buf_alloc_info
{
    stream_info_t*       pStreamInfo;
    u32                  bufNum;
} stream_buf_alloc_info_t;

/*
* Stream_Buf_Mgr_Acquire
*
* @brief Acquire stream buffer manager
*
* @param pStreamInfo: pointer to stream information
*
* @param ppBufMgr: dimensional pointer to stream_buf_mgr_t
*
* @return ISP_RET_SUCCESS if success, others if failed
*/
isp_ret_t Stream_Buf_Mgr_Acquire(stream_info_t*      pStreamInfo,
                                       stream_buf_mgr_t** ppBufMgr);

/*
* Stream_Buf_Mgr_Release
*
* @brief Release stream buffer manager
*
* @param ppBufMgr: dimensional pointer to stream_buf_mgr_t
*
* @return ISP_RET_SUCCESS if success, others if failed
*/
isp_ret_t Stream_Buf_Mgr_Release(stream_buf_mgr_t** ppBufMgr);

#endif

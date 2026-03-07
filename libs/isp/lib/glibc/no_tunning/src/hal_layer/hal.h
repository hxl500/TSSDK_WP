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

#ifndef __HAL_H__
#define __HAL_H__

#include "types_utils.h"
#include "log_utils.h"
#include "hal_types.h"
#include "stream_buf_mgr.h"

typedef struct __hal hal_t;

/// Hal ops
typedef isp_ret_t (*hal_probe_func)(obj_list_t* listProbeInfo);

typedef isp_ret_t (*hal_open_func)(hal_t* thiz,
                                       u32 camId);

typedef isp_ret_t (*hal_get_capability_func)(hal_t*       thiz,
                                                    u32 camId,
                                                    metadata_t* pMetadata);

typedef isp_ret_t (*hal_initialize_func)(hal_t*      thiz,
                                               u32 camId,
                                               callbacks_ops_t* pCbs);

typedef isp_ret_t (*hal_configure_streams_func)(hal_t*        thiz,
                                                       u32 camId,
                                                       stream_config_t* pStreamCfg);

typedef isp_ret_t (*hal_process_request_func)(hal_t* thiz,
                                                     u32 camId,
                                                     request_t* pRequest);

typedef isp_ret_t (*hal_flush_func)(hal_t* thiz,
                                        u32 camId);

typedef isp_ret_t (*hal_dump_func)(hal_t*   thiz,
                                       u32 camId,
                                       u64 dumpFlag);


typedef isp_ret_t (*hal_close_func)(hal_t* thiz,
                                        u32 camId);

/// Buf Mgr ops
typedef isp_ret_t (*hal_buf_mgr_acquire_func)(hal_t*        thiz,
                                                     stream_info_t* pStreamInfo);

typedef isp_ret_t (*hal_buf_mgr_release_func)(hal_t* thiz,
                                                     stream_info_t* pStreamInfo);

typedef stream_buffer_t* (*hal_buf_get_func)(hal_t*     thiz,
                                                 stream_info_t* pStreamInfo);

typedef isp_ret_t (*hal_buf_return_func)(hal_t*      thiz,
                                              stream_buffer_t* pStreamBuf);

/// HAL operations
typedef struct __hal_ops
{
    hal_probe_func                   Probe;
    hal_open_func                    Open;
    hal_get_capability_func          Get_Capability;
    hal_initialize_func              Initialize;
    hal_configure_streams_func       Configure_Streams;
    hal_process_request_func         Process_Request;
    hal_flush_func                   Flush;
    hal_dump_func                    Dump;
    hal_close_func                   Close;
} hal_ops_t;

/// HAL buffer operations
typedef struct __hal_buf_ops
{
    hal_buf_mgr_acquire_func         Acquire;
    hal_buf_mgr_release_func         Release;
    hal_buf_get_func                 Get;
    hal_buf_return_func              Return;
} hal_buf_ops_t;

/// HAL descriptor
struct __hal
{
    cbs_t                       halCbs[MAX_CAMERA];

    u32                         nextFrameId[MAX_CAMERA];
    void*                       ppContext[MAX_CAMERA];
    void*                       bufMgr[MAX_CAMERA][BUF_STREAM_IDX_MAX];

    u8                          activeContext;

    //bus_t                     pBus;

    hal_ops_t*                  pHalOps;
    hal_buf_ops_t*              pBufOps;
};


/*
* Hal_Get_Instance
*
* @brief Get HAL instance
*
* @return pointer to hal_t
*/
hal_t* Hal_Get_Instance();

#endif

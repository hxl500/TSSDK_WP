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

#ifndef __DEVICE_H__
#define __DEVICE_H__

#include "types_utils.h"
#include "log_utils.h"
#ifndef MPP_COMP_VIPIPE_OPEN
#include "app_config.h"
#endif
#include "hal_wrapper.h"
#include "device_buf_mgr.h"
#include "obj_queue_utils.h"

typedef struct __device device_t;

typedef isp_ret_t (*device_open_func)(device_t* thiz);

typedef isp_ret_t (*device_query_func)(device_t* thiz);

typedef isp_ret_t (*device_initialize_func)(device_t*       thiz,
                                                  callbacks_ops_t* pCbs);

typedef isp_ret_t (*device_configure_func)(device_t* thiz,
                                                 stream_config_t* pStreamCfg);

typedef isp_ret_t (*device_process_request_func)(device_t* thiz,
                                                         metadata_t* pMetadata);

typedef isp_ret_t (*device_flush_func)(device_t* thiz);

typedef isp_ret_t (*device_close_func)(device_t* thiz);

typedef isp_ret_t (*device_dump_func)(device_t* thiz, u64 dumpFlag);


/// Device status
typedef enum __device_status
{
    DEV_STATUS_UNINITIALIZED = 0,
    DEV_STATUS_INITIALIZED,
    DEV_STATUS_CONFIGURED,
    DEV_STATUS_STREAMING,
} device_status_t;

/// Device pending information
typedef struct __device_pending
{
    request_t  request;
    u32        numPendMeta;
    u32        numPendBuf;
} device_pending_t;

/// Device descriptor
struct __device
{
    cbs_t                        deviceCbs;
    device_status_t              status;
    u32                          camId;
    u32                          frameId;
    stream_config_t*             pStreamCfg;
    device_buf_mgr_t*            pBufMgr[BUF_STREAM_IDX_MAX];
    metadata_t*                  pCapMeta;
    obj_list_t*                  inflightList;
    device_pending_t*            pPend;

    device_open_func             Open;
    device_query_func            Query;
    device_initialize_func       Initialize;
    device_configure_func        Configure;
    device_process_request_func  Process_Request;
    device_flush_func            Flush;
    device_close_func            Close;
    device_dump_func             Dump;

	pthread_mutex_t              lock;
};

/*
* Device_Get_Instance
*
* @brief Get device instance according to camera ID
*
* @param camId: camera ID
*
* @return pointer to device_t
*/
device_t* Device_Get_Instance(u32 camId);

/*
* Device_Probe
*
* @brief Probe camera device
*
* @param listProbeInfo: list to store probe information
*
* @return ISP_RET_SUCCESS if success, others if failed
*/
isp_ret_t Device_Probe(obj_list_t* listProbeInfo);

device_t *Device_Acquire(int camid);

#endif

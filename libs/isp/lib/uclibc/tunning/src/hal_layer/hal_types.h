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

#ifndef __HAL_TYPES_H__
#define __HAL_TYPES_H__

#include "types_utils.h"
#include "stream_types.h"
#include "metadata/metadata.h"

#define MAX_CAMERA              (4)
#define MAX_FPS                 (120.0)
#define MAX_SENSOR_MODE         (10)

/// 5 buffers: at least 1 in APP and max 4 in HAL
#define DEFAULT_REQUEST_DEPTH   (4)
#define DRV_SENSOR_SETTING_MAX  (16)


/// define frame request struct
typedef struct __request
{
    u32                           camId;
    u32                           frameId;
    metadata_t*                   pMetadata;
    u32                           inputStreamNum;
    stream_buffer_t*              ppInputStreamBuffers[BUF_STREAM_IDX_MAX];
    u32                           outputStreamNum;
    stream_buffer_t*              ppOutputStreamBuffers[BUF_STREAM_IDX_MAX];
} request_t;

/// define frame result struct
typedef struct __result
{
    u32                           camId;
    u32                           frameId;
    metadata_t*                   pMetadata;
    u32                           inputStreamNum;
    stream_buffer_t*              ppInputStreamBuffers[BUF_STREAM_IDX_MAX];
    u32                           outputStreamNum;
    stream_buffer_t*              ppOutputStreamBuffers[BUF_STREAM_IDX_MAX];
} result_t;

/// define stream config struct
typedef struct __stream_config
{
    f32                           fps; /// @TODO: range
    u32                           inputStreamNum;
    stream_info_t                 inputStreamInfo[BUF_STREAM_IDX_MAX];
    u32                           outputStreamNum;
    stream_info_t                 outputStreamInfo[BUF_STREAM_IDX_MAX];
} stream_config_t;

/// define recovery type for notify
typedef enum __notify_rcvy_type
{
    NOTIFY_RCVY_COMMON            = 0,
    NOTIFY_RCVY_HDR_SWITCH,
    NOTIFY_RCVY_UNEXPECTED,
    NOTIFY_RCVY_MAX,
} notify_rcvy_type_t;

/// define notify message type
typedef enum __notify_msg_type
{
    NOTIFY_MSG_NOP                = 0,
    NOTIFY_MSG_SHUTTER,
    NOTIFY_MSG_ERROR,
    NOTIFY_MSG_EVT_ACK,
    NOTIFY_MSG_RECOVERY,
    NOTIFY_MSG_VOUT_DONE,
	NOTIFY_MSG_AAA_STATS,
    NOTIFY_MSG_FRAME_DONE,
    NOTIFY_MSG_FPS,
    NOTIFY_MSG_AAA_INVALID,
    NOTIFY_MSG_INPUT_INVALID,
    NOTIFY_MSG_INPUT_RECOVERY,
    NOTIFY_MSG_MAX,
} notify_msg_type_t;

typedef struct _aaa_invalid_msg_s
{
    u64 timestamp_ts;
    u32 frame_num;
    u32 last_num;
    u32 reserve[2];
} aaa_invalid_msg_t;

/// define notify message struct
typedef struct _notify
{
    notify_msg_type_t  type;
    u32                camId;
    u32                frameId;
    u32                msg[10];
} notify_t;

typedef struct __callbacks_ops callbacks_ops_t;

typedef void (*process_result_func)(callbacks_ops_t* pCb, result_t* pResult);
typedef void (*notify_func)(callbacks_ops_t* pCb, notify_t* pNotifyMsg);

/// define callback operationss
struct __callbacks_ops
{
    process_result_func            Process_Result;
    notify_func                    Notify;
};

/// define callback struct:
///     cbs       : store callback operations of current layer
///     pUpperCbs : pointer to callback operations of upper layer
typedef struct _cbs
{
    callbacks_ops_t                cbs;
    callbacks_ops_t*               pUpperCbs;
} cbs_t;

#endif

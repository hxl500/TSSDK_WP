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

#ifndef __HAL_STREAM_H__
#define __HAL_STREAM_H__

#include "types_utils.h"

#define MAX_BUF_PER_STREAM      (5)

typedef enum __stream_idx
{
    BUF_STREAM_IDX_PREVIEW   = 0,
    BUF_STREAM_IDX_VIDEO,
    BUF_STREAM_IDX_VIDEO_1,
    BUF_STREAM_IDX_VIDEO_2,
    BUF_STREAM_IDX_MAX,
    STATS_STREAM_IDX         = 0x10,
    CMD_STREAM_IDX           = 0x20,
} stream_idx_t;

/// define stream allocate types
typedef enum __stream_alloc_type
{
    STREAM_ALLOC_NONE            = 0,
    STREAM_ALLOC_MALLOC          = 1,
    STREAM_ALLOC_CONTINUE_BUF    = 2,
    STREAM_ALLOC_CONTINUE_STATS  = 3,
    STREAM_ALLOC_CONTINUE_CMD    = 4,
    STREAM_ALLOC_ION             = 5,
    STREAM_ALLOC_MAX,
} stream_alloc_t;

#define GEN_ENUM(ENUM) ENUM,
#define GEN_STR(STR) #STR,
#define FOREACH_STRM_TYPE(CMD) \
  CMD(STREAM_FMT_NV12) \
  CMD(STREAM_FMT_NV21) \
  CMD(STREAM_FMT_YUV422) \
  CMD(STREAM_FMT_YUV444) \
  CMD(STREAM_FMT_RGB888) \
  CMD(STREAM_FMT_RGB24) \
  CMD(STREAM_FMT_RGB565) \
  CMD(STREAM_FMT_RAW10) \
  CMD(STREAM_FMT_RAW12) \
  CMD(STREAM_FMT_STATS) \
  CMD(STREAM_FMT_CMD) \
  CMD(STREAM_FMT_VIN) \
  CMD(STREAM_FMT_VINDL) \
  CMD(STREAM_FMT_MVD) \
  CMD(STREAM_FMT_YTNR) \
  CMD(STREAM_FMT_TTNR) \
  CMD(STREAM_FMT_VOUT) \
  CMD(STREAM_FMT_MAX) \

typedef enum __stream_fmt {
  FOREACH_STRM_TYPE(GEN_ENUM)
} stream_fmt_t;

/// define stream format types
// typedef enum __stream_fmt
// {
//     STREAM_FMT_NV12   = 0,
//     STREAM_FMT_NV21,
//     STREAM_FMT_YUV422,
//     STREAM_FMT_YUV444,
//     STREAM_FMT_RGB888,
//     STREAM_FMT_RGB24,
//     STREAM_FMT_RGB565,
//     STREAM_FMT_RAW10,
//     STREAM_FMT_RAW12,
//     STREAM_FMT_STATS,
//     STREAM_FMT_CMD,
//
//     STREAM_FMT_TTNR,
//
//     STREAM_FMT_MAX,
// } stream_fmt_t;

/// define stream identity
typedef union __stream_identity
{
    struct
    {
        u16 camId;
        u16 streamId;
    };

    u32 id[1];
} stream_identity_t;

/// define stream usage types
typedef enum __stream_usage
{
    STREAM_USAGE_NONE       = 0x0,
    STREAM_USAGE_PREVIEW    = 0x1,
    STREAM_USAGE_VIDEO      = 0x2,
    STREAM_USAGE_ENCODE     = 0x4,

    STREAM_USAGE_DBUF = 0x5,
} stream_usage_t;

/// define stream user information struct to store params updated by user
typedef struct __stream_user_info
{
    stream_identity_t             identity;
    u32                           width;
    u32                           height;
    stream_fmt_t                  fmt;
    stream_usage_t                usage;
    u32                           maxBufNum;
    stream_alloc_t                allocType;
} stream_user_info_t;

/// define stream internal information struct to store params filled by buf mgr
typedef struct __stream_internal_info
{
    u32                           stride;
    u32                           slice;
    size_t                        size;
    u32                           freeBufNum;
} stream_internal_info_t;

/// define stream information struct
typedef struct __stream_info
{
    stream_user_info_t     user;
    stream_internal_info_t internal;
} stream_info_t;

/// define buffer status types
typedef enum __stream_buffer_status_t
{
    STREAM_BUFFER_STATUS_INVALID     = 0,
    STREAM_BUFFER_STATUS_READY,
    STREAM_BUFFER_STATUS_FREE,
    STREAM_BUFFER_STATUS_BUSY,
} stream_buffer_status_t;


/// define stream buffer identity
typedef union __stream_buffer_identity
{
    struct
    {
        u16   camId;
        u16   streamId;
        u32   frameId;
        u32   bufId;

        char8 *name;
    };

    u32 id[3];
} stream_buffer_identity_t;


/// define stream buffer struct
typedef s32 buf_handle;
typedef struct __stream_buffer
{
    stream_buffer_identity_t      identity;
    stream_buffer_status_t        status;
    u64                           timestamp;
    stream_info_t*                info;
    union
    {
        void*                     bufAddr;
        buf_handle                bufHandle;
    } buffer;
    s32 fd;
	u64 phyAddr;

    void*                         private;
} stream_buffer_t;

#endif

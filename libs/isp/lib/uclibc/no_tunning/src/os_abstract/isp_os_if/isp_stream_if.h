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
#ifndef __ISR_STREAM_IF_H__
#define __ISR_STREAM_IF_H__

#include "types_utils.h"
#include "stream_types.h"
#include "log_utils.h"
#include "mem_osutils.h"
#include "isp_mem.h"
#include "obj_list_utils.h"
#include "obj_queue_utils.h"
#include "isp_dma_if.h"


/// typedef isp stream interface type
typedef enum __isp_stream_if_type
{
    ISP_STREAM_IF_TYPE_ALLOCATOR     = 0,
    ISP_STREAM_IF_TYPE_HW            = 1,
} isp_stream_if_type_t;

typedef struct __isp_stream_if isp_stream_if_t;

typedef isp_ret_t (*isp_stream_if_alloc_func)(isp_stream_if_t*                thiz,
                                                     stream_info_t*           pStreamInfo,
                                                     stream_buffer_t**        ppStreamBuffer);

typedef isp_ret_t (*isp_stream_if_free_func)(isp_stream_if_t*             thiz,
                                                    stream_buffer_t**     ppStreamBuffer);

typedef isp_ret_t (*isp_stream_if_enqueue_func)(isp_stream_if_t* thiz,
                                                       stream_buffer_t*  pStreamBuffer);

typedef stream_buffer_t* (*isp_stream_if_dequeue_func)(isp_stream_if_t* thiz,
                                                              stream_buffer_identity_t id);

typedef u32 (*isp_stream_if_queryqueue_func)(isp_stream_if_t* thiz);

typedef struct __isp_stream_allocator_if
{
    isp_stream_if_alloc_func   Alloc;
    isp_stream_if_free_func    Free;
} isp_stream_allocator_if_t;

typedef struct __isp_stream_hw_if
{
    isp_stream_if_enqueue_func      Enqueue;
    isp_stream_if_dequeue_func      Dequeue;
    isp_stream_if_queryqueue_func   Queryqueue;
} isp_stream_hw_if_t;

struct __isp_stream_if
{
    u32                         camId;
    isp_stream_if_type_t        ifType;

    const isp_buffer_address_t* pIspBufAddr;

    obj_queue_t*                pendingHwBufQ;
    obj_list_t*                 readyHwBufList;

    union
    {
        isp_stream_allocator_if_t allocator;
        isp_stream_hw_if_t        hw;
    } ops;
};

/*
* ISP_Stream_IF_Acquire
*
* @brief Acquire ISP stream interface ops
*
* @param ppStreamIf: dimensional pointer to isp_stream_if_t
*
* @param camId: camera id
*
* @param ifType: isp stream interface type
*
* @return success / failure
*/
isp_ret_t ISP_Stream_IF_Acquire(isp_stream_if_t** ppStreamIf, u32 camId, isp_stream_if_type_t ifType);

/*
* ISP_Stream_IF_Release
*
* @brief Release ISP stream interface ops
*
* @param ppStreamIf: dimensional pointer to isp_stream_if_t
*
* @return success / failure
*/
isp_ret_t ISP_Stream_IF_Release(isp_stream_if_t** ppStreamIf);

#endif

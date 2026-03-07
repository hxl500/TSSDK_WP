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

#include "device_buf_mgr.h"

/// Get one free buffer from device bufer manager
stream_buffer_t* Device_Buf_Get(device_buf_mgr_t* thiz)
{
    isp_ret_t        res        = ISP_RET_SUCCESS;
    stream_buffer_t* pStreamBuf = NULL;

    if (NULL == thiz)
    {
        res = ISP_RET_INVALID;
        APP_ERR("Error! res = %d", res);
    }

    if (ISP_RET_SUCCESS == res)
    {
        pStreamBuf = Hal_Wrapper_Buf_Get(thiz->pStreamInfo);
        if (NULL == pStreamBuf)
        {
            res = ISP_RET_INGORED_FAILURE;
            APP_ERR("Error! res = %d", res);
        }
    }

    return pStreamBuf;
}

/// Return one buffer to device bufer manager
static isp_ret_t Device_Buf_Return(device_buf_mgr_t* thiz,
                                        stream_buffer_t* pStreamBuf)
{
    isp_ret_t res = ISP_RET_SUCCESS;

    if (NULL == thiz ||
        NULL == pStreamBuf)
    {
        res = ISP_RET_INVALID;
        APP_ERR("Error! res = %d", res);
    }

    if (ISP_RET_SUCCESS == res)
    {
        res = Hal_Wrapper_Buf_Return(pStreamBuf);
        if (ISP_RET_SUCCESS != res)
        {
            res = ISP_RET_INGORED_FAILURE;
            APP_ERR("Error! res = %d", res);
        }
    }

    return res;
}

/// Acquire device buffer manager
isp_ret_t Device_Buf_Mgr_Acquire(stream_info_t*         pStreamInfo,
                                        device_buf_mgr_t** ppDevBufMgr)

{
    isp_ret_t         res        = ISP_RET_SUCCESS;
    device_buf_mgr_t* pNewBufMgr = NULL;

    if (NULL == pStreamInfo ||
        NULL == ppDevBufMgr)
    {
        res = ISP_RET_INVALID;
        APP_ERR("Error! res = %d", res);
    }

    if (ISP_RET_SUCCESS == res)
    {
        pNewBufMgr = (device_buf_mgr_t*)MEM_OS_MALLOC(sizeof(device_buf_mgr_t));
        if (NULL == pNewBufMgr)
        {
            res = ISP_RET_NOMEM;
            APP_ERR("Error! res = %d", res);
        }
    }

    if (ISP_RET_SUCCESS == res)
    {
        pNewBufMgr->pStreamInfo        = pStreamInfo;
        pNewBufMgr->Get_Buf            = Device_Buf_Get;
        pNewBufMgr->Return_Buf         = Device_Buf_Return;

        res = Hal_Wrapper_Buf_Mgr_Acquire(pStreamInfo);
        if (ISP_RET_SUCCESS != res)
        {
            res = ISP_RET_ERROR;
            APP_ERR("Error! res = %d", res);
        }
    }

    if (ISP_RET_SUCCESS == res)
    {
        *ppDevBufMgr = pNewBufMgr;
    }

    return res;
}

/// Release device buffer manager
isp_ret_t Device_Buf_Mgr_Release(device_buf_mgr_t** ppDevBufMgr)
{
    isp_ret_t res = ISP_RET_SUCCESS;

    if (NULL == ppDevBufMgr ||
        NULL == *ppDevBufMgr)
    {
        res = ISP_RET_INVALID;
        APP_ERR("Error! res = %d", res);
    }

    if (ISP_RET_SUCCESS == res)
    {
        res = Hal_Wrapper_Buf_Mgr_Release((*ppDevBufMgr)->pStreamInfo);
        if (ISP_RET_SUCCESS != res)
        {
            res = ISP_RET_ERROR;
            APP_ERR("Error! res = %d", res);
        }
    }

    if (ISP_RET_SUCCESS == res)
    {
        MEM_OS_FREE(ppDevBufMgr);
    }

    return res;
}

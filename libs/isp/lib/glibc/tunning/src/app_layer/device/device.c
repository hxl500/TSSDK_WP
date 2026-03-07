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

#include "device.h"

static device_t g_device[MAX_CAMERA];

/// Check whether pending is valid
static inline BOOL Device_Check_Pending(device_pending_t* pPend)
{
    BOOL isValid  = TRUE;

    do
    {
        if (NULL == pPend)
        {
            isValid = FALSE;
            APP_ERR("pPend %p", pPend);
            break;
        }

        if (0    == pPend->numPendMeta &&
            0    == pPend->numPendBuf)
        {
            isValid = FALSE;
            APP_ERR("numPendMeta %u, numPendBuf%u",
                    pPend->numPendMeta,
                    pPend->numPendBuf);
            break;
        }

        if (NULL == pPend->request.pMetadata ||
            //0    == pPend->request.outputStreamNum ||
            NULL == pPend->request.ppOutputStreamBuffers)
        {
            isValid = FALSE;
            APP_ERR("pMetadata %p, outputStreamNum %u, ppOutputStreamBuffers %p",
                    pPend->request.pMetadata,
                    pPend->request.outputStreamNum,
                    pPend->request.ppOutputStreamBuffers);
            break;
        }

        for (u32 i = 0; i < pPend->request.outputStreamNum; i++)
        {
            if (NULL == pPend->request.ppOutputStreamBuffers[i])
            {
                isValid = FALSE;
                APP_ERR("ppOutputStreamBuffers[%u] = %p", i, pPend->request.ppOutputStreamBuffers[i]);
            }
        }
    } while (0);

    return isValid;
}

/// Destroy pending
static void Device_Destroy_Pending(device_t* thiz, device_pending_t** ppPend)
{
    APP_FUNC_ENTER();

    if (thiz && ppPend && (*ppPend))
    {
        device_pending_t* pPend = *ppPend;

        for (u32 i = 0; i < pPend->request.outputStreamNum; i++)
        {
            if (pPend->request.ppOutputStreamBuffers[i])
            {
                u32 streamId = pPend->request.ppOutputStreamBuffers[i]->identity.streamId;

                isp_ret_t res = thiz->pBufMgr[streamId]->Return_Buf(thiz->pBufMgr[streamId],
                                                                    pPend->request.ppOutputStreamBuffers[i]);
                if (ISP_RET_SUCCESS != res)
                {
                    APP_ERR("Return buffer failed for stream %u ! res = %d", i, res);
                }

                pPend->request.ppOutputStreamBuffers[i] = NULL;
            }
        }

        if (pPend->request.pMetadata)
        {
            Metadata_Destroy(&(pPend->request.pMetadata));
        }

        MEM_OS_FREE(ppPend);
    }


    APP_FUNC_EXIT();
}

/// Create pending with request according to device params and metadata
static device_pending_t* Device_Create_Pending(device_t* thiz, metadata_t* pMetadata)
{
    isp_ret_t                 res               = ISP_RET_SUCCESS;
    device_pending_t*         pPend             = NULL;
    metadata_t*               pNewMetadata      = NULL;
    u32                       numBufRequired    = 0;
    stream_config_t*          pStreamCfg        = NULL;

    APP_FUNC_ENTER();

    if (NULL == thiz ||
        NULL == pMetadata)
    {
        res = ISP_RET_INVALID;
        APP_ERR("Error! res = %d", res);
    }
    else
    {
        pStreamCfg = META_GET_ADDR(pMetadata, METADATA_USER_STREAM_CFG, stream_config_t);

        numBufRequired = pStreamCfg->outputStreamNum - 1;
        if (numBufRequired > BUF_STREAM_IDX_MAX)
        {
            res = ISP_RET_INVALID;
            APP_ERR("Error! numBufRequired = %u", numBufRequired);
        }
    }

    if (ISP_RET_SUCCESS == res)
    {
        pPend = (device_pending_t *)MEM_OS_MALLOC(sizeof(device_pending_t));
        if (NULL == pPend)
        {
            res = ISP_RET_NOMEM;
            APP_ERR("Error! res = %d", res);
        }
    }

    if (ISP_RET_SUCCESS == res)
    {
        pNewMetadata = Metadata_Create(); /// @TODO: pre-allocate
        if (NULL == pNewMetadata)
        {
            res = ISP_RET_NOMEM;
            APP_ERR("Error! res = %d", res);
        }
        else
        {
            Metadata_Copy(pNewMetadata, pMetadata);
        }
    }

    if (ISP_RET_SUCCESS == res)
    {
        pPend->request.camId            = thiz->camId;
        pPend->request.frameId          = thiz->frameId;
        pPend->request.pMetadata        = pNewMetadata;
        pPend->request.outputStreamNum  = 0;

        for (u32 i = 0; (i < pStreamCfg->outputStreamNum) && (0 <= numBufRequired) ; i++)
        {
            ///u32 streamId = pStreamCfg->outputStreamInfo[i].user.identity.streamId;

            if (thiz->pBufMgr[i])
            {
                stream_buffer_t *pStreamBuffers = thiz->pBufMgr[i]->Get_Buf(thiz->pBufMgr[i]);

                if (NULL == pStreamBuffers)
                {
                    res = ISP_RET_INGORED_FAILURE;
                    APP_WARN("No free buffers for stream %u", i);

                    /* APP_CFG("====== <create> pipe[%d].stream[%d]: {failure , } ======",  */
                    /*     thiz->camId, i); */
                }
                else
                {
                    pPend->request.ppOutputStreamBuffers[pPend->request.outputStreamNum] = pStreamBuffers;
                    pPend->request.outputStreamNum++;

                    /* APP_CFG("====== <create> pipe[%d].stream[%d]: {maddr=%p, size=%d}  ======", */
                    /*   thiz->camId, i, pStreamBuffers->buffer.bufAddr, pStreamBuffers->info->internal.size); */
                }

                numBufRequired--;
            }

            if (ISP_RET_SUCCESS != res)
            {
                break;
            }
        }
    }

    if (ISP_RET_SUCCESS == res)
    {
        pPend->numPendMeta = 1;
        pPend->numPendBuf  = pPend->request.outputStreamNum;
    }
    else
    {
        Device_Destroy_Pending(thiz, &pPend);
    }

    APP_FUNC_EXIT();

    return pPend;
}

/// Device result callback function
static void Device_Process_Result(callbacks_ops_t* pCb, result_t* pResult)
{
    isp_ret_t         res   = ISP_RET_SUCCESS;
    device_t*         thiz  = NULL;
    device_pending_t* pPend = NULL;

    APP_FUNC_ENTER();

    if (NULL == pCb ||
        NULL == pResult)
    {
        res = ISP_RET_INVALID;
        APP_ERR("Error! res = %d", res);
    }
    else
    {
        thiz = (device_t*) pCb;
        if (NULL == thiz->inflightList)
        {
            res = ISP_RET_INVALID;
            APP_ERR("Error! res = %d", res);
        }
    }

    if (ISP_RET_SUCCESS == res)
    {
		pthread_mutex_lock(&thiz->lock);
        BOOL found = thiz->inflightList->Find(thiz->inflightList,
                                              (void*)pResult,
                                              ISP_lIST_NODE_MATCH_INT64,
                                              (void**)&pPend);
		pthread_mutex_unlock(&thiz->lock);
        if (found == FALSE || NULL == pPend)
        {
            res = ISP_RET_ERROR;

            APP_ERR("found = %u, pPend = %p, camId = %u, frameId = %u",
                found, pPend, pResult->camId, pResult->frameId);
        }
    }

    if (ISP_RET_SUCCESS == res)
    {
        /// match metadata
        if (pResult->pMetadata &&
            pResult->pMetadata == pPend->request.pMetadata)
        {
            pPend->numPendMeta--;
            APP_DBG("this is metadata: camId = %u, frameId = %u !!",
                pResult->camId, pResult->frameId);
        }

        /// match stream buffers
        if (pResult->outputStreamNum > 0 &&
            NULL != pResult->ppOutputStreamBuffers &&
            pPend->request.outputStreamNum > 0 &&
            NULL !=  pPend->request.ppOutputStreamBuffers)
        {
            u32 i = 0;
            u32 j = 0;

            for (i = 0; i < pResult->outputStreamNum; i++)
            {
                for (j = 0; j < pPend->request.outputStreamNum; j++)
                {
                    //APP_DBG("ppOutputStreamBuffers %p -- %p, bufAddr %p --- %p",
                    //    pResult->ppOutputStreamBuffers[i], pPend->request.ppOutputStreamBuffers[j],
                    //    pResult->ppOutputStreamBuffers[i]->buffer.bufAddr, pPend->request.ppOutputStreamBuffers[j]->buffer.bufAddr);

                    if (pResult->ppOutputStreamBuffers[i] &&
                        pPend->request.ppOutputStreamBuffers[j] &&
                        pResult->ppOutputStreamBuffers[i] == pPend->request.ppOutputStreamBuffers[j])
                    {
                        pPend->numPendBuf--;
                        APP_DBG("this is buffer: camId = %u, frameId = %u !!", pResult->camId, pResult->frameId);

                        /* APP_CFG("======  <check> pipe[%d]: {{result.ch[%d], request.ch[%d]}: frameId=%04d, stream_buffer: {idt.frameId=%d, buf_addr=%p}} ======", */
                        /*     i, j, pResult->camId, pResult->frameId, */
                        /*     pResult->ppOutputStreamBuffers[i]->identity.frameId, */
                        /*     pResult->ppOutputStreamBuffers[i]->buffer.bufAddr); */

                        continue;
                    }
                }
            }
        }

        /// call upper cb
        callbacks_ops_t* pUpperCbs = thiz->deviceCbs.pUpperCbs;
        if (pUpperCbs &&
            pUpperCbs->Process_Result)
        {
            pUpperCbs->Process_Result(pUpperCbs, pResult);
        }

        /// remove pending
        if (0 == pPend->numPendBuf &&
            0 == pPend->numPendMeta)
        {

			pthread_mutex_lock(&thiz->lock);
            thiz->inflightList->Remove(thiz->inflightList, pPend);
			pthread_mutex_unlock(&thiz->lock);

            Device_Destroy_Pending(thiz, &pPend);

            APP_DBG("Del Request: camId %u, frameId %u, length %u",
                pResult->camId, pResult->frameId, thiz->inflightList->length);
        }

        if (DEV_STATUS_CONFIGURED == thiz->status) thiz->status = DEV_STATUS_STREAMING;
    }

    APP_FUNC_EXIT();

    return;
}

/// Device notify callback function
static void Device_Notify(callbacks_ops_t* pCb, notify_t* pNotifyMsg)
{
    isp_ret_t    res = ISP_RET_SUCCESS;
    device_t*   thiz = NULL;

    APP_FUNC_ENTER();

    if (NULL == pCb ||
        NULL == pNotifyMsg)
    {
        res = ISP_RET_INVALID;
        APP_ERR("Error! res = %d", res);
    }
    else
    {
        thiz = (device_t*) pCb;
    }

    if (ISP_RET_SUCCESS == res)
    {
        callbacks_ops_t* pUpperCbs = thiz->deviceCbs.pUpperCbs;

        if (pUpperCbs &&
            pUpperCbs->Notify)
        {
            pUpperCbs->Notify(pUpperCbs, pNotifyMsg);
        }
    }

    APP_FUNC_EXIT();

    return;
}

/// Allocate buffers from buffer manager
static isp_ret_t Device_Allocate_Bufs(device_t* thiz)
{
    isp_ret_t    res = ISP_RET_SUCCESS;

    if (NULL == thiz ||
        NULL == thiz->pStreamCfg)
    {
        res = ISP_RET_INVALID;
        APP_ERR("Error! res = %d", res);
    }

    if (ISP_RET_SUCCESS == res)
    {
        for (u32 i = 0; i < thiz->pStreamCfg->outputStreamNum; i++)
        {
            if (0 == thiz->pStreamCfg->outputStreamInfo[i].user.maxBufNum)
            {
                continue;
            }

            res = Device_Buf_Mgr_Acquire(&(thiz->pStreamCfg->outputStreamInfo[i]),
                                         &(thiz->pBufMgr[i]));
            if (ISP_RET_SUCCESS != res)
            {
                APP_ERR("Error! res = %d", res);
                break;
            }
        }
    }

    return res;
}

/// Release all buffers from buffer manager
static isp_ret_t Device_Release_Bufs(device_t* thiz)
{
    isp_ret_t    res = ISP_RET_SUCCESS;
    u32          i   = 0;

    if (NULL == thiz ||
        NULL == thiz->pStreamCfg)
    {
        res = ISP_RET_INVALID;
        APP_ERR("Error! res = %d", res);
    }

    if (ISP_RET_SUCCESS == res)
    {
        for (i = 0; i < thiz->pStreamCfg->outputStreamNum; i++)
        {
            if (NULL == thiz->pBufMgr[i])
            {
                continue;
            }

            res = Device_Buf_Mgr_Release(&(thiz->pBufMgr[i]));
            if (ISP_RET_SUCCESS != res)
            {
                APP_ERR("Error! res = %d", res);
                break;
            }
        }
    }

    return res;
}

/// Interface to open device
static isp_ret_t Device_Open(device_t* thiz)
{
    isp_ret_t res = ISP_RET_SUCCESS;

    APP_FUNC_ENTER();

    if ((NULL == thiz) || (DEV_STATUS_INITIALIZED != thiz->status))
    {
        res = ISP_RET_INVALID;
        APP_ERR("Error! res = %d", res);
    }

    if (ISP_RET_SUCCESS == res)
    {
        thiz->inflightList = Obj_List_Create();
    }

    if (ISP_RET_SUCCESS == res)
    {
        res = Hal_Wrapper_Open(thiz->camId);
        if (ISP_RET_SUCCESS != res)
        {
            APP_ERR("Error! res = %d", res);
        }
    }

    if (ISP_RET_SUCCESS == res)
    {
        thiz->pCapMeta = Metadata_Create();
        if (NULL == thiz->pCapMeta)
        {
            res = ISP_RET_NOMEM;
            APP_ERR("Error! res = %d", res);
        }
    }

    APP_FUNC_EXIT();

    return res;
}

/// Interface to query device capability
static isp_ret_t Device_Query(device_t* thiz)
{
    isp_ret_t res = ISP_RET_SUCCESS;

    APP_FUNC_ENTER();

    if ((NULL == thiz) || (DEV_STATUS_INITIALIZED != thiz->status))
    {
        res = ISP_RET_INVALID;
        APP_ERR("Error! res = %d", res);
    }

    if (ISP_RET_SUCCESS == res)
    {
        res = Hal_Wrapper_Get_Capability(thiz->camId, thiz->pCapMeta);
        if (ISP_RET_SUCCESS != res)
        {
            APP_ERR("Error! res = %d", res);
        }
    }

    if (ISP_RET_SUCCESS == res)
    {
        /// Get and compare camera id of capability
        u32 camId = META_GET_VALUE(thiz->pCapMeta,
                                   METADATA_CAP_CAMERA_ID,
                                   u32);
        if (camId != thiz->camId)
        {
            APP_ERR("camId mismatch : %d vs %d", camId, thiz->camId);
            res = ISP_RET_ERROR;
        }
    }

    APP_FUNC_EXIT();

    return res;
}

/// Interface to initialize device and register callback functions
static isp_ret_t Device_Initialize(device_t*       thiz,
                                        callbacks_ops_t* pCbs)
{
    isp_ret_t res = ISP_RET_SUCCESS;

    APP_FUNC_ENTER();

    if (NULL == thiz ||
        NULL == pCbs || DEV_STATUS_INITIALIZED != thiz->status)
    {
        res = ISP_RET_INVALID;
        APP_ERR("Error! res = %d", res);
    }

    if (ISP_RET_SUCCESS == res)
    {
        thiz->deviceCbs.cbs.Process_Result   = Device_Process_Result;
        thiz->deviceCbs.cbs.Notify           = Device_Notify;
        thiz->deviceCbs.pUpperCbs            = pCbs;

        res = Hal_Wrapper_Initialize(thiz->camId, (callbacks_ops_t*)thiz);
        if (ISP_RET_SUCCESS != res)
        {
            APP_ERR("Error! res = %d", res);
        }
        else
        {
            thiz->status = DEV_STATUS_INITIALIZED;
            thiz->pPend  = NULL;
        }
    }

    APP_FUNC_EXIT();

    return res;
}

/// Interface to configure stream parameters of device
static isp_ret_t Device_Configure(device_t* thiz,
                                       stream_config_t* pStreamCfg)
{
    isp_ret_t res = ISP_RET_SUCCESS;

    APP_FUNC_ENTER();

    if (NULL == thiz ||
        pStreamCfg->outputStreamNum > BUF_STREAM_IDX_MAX ||
        DEV_STATUS_INITIALIZED != thiz->status)
    {
        res = ISP_RET_INVALID;
        APP_ERR("Error! res = %d", res);
    }

    if (ISP_RET_SUCCESS == res)
    {
        thiz->pStreamCfg = pStreamCfg;

        res = Device_Allocate_Bufs(thiz);
        if (ISP_RET_SUCCESS != res)
        {
            APP_ERR("Error! res = %d", res);
        }
    }

    if (ISP_RET_SUCCESS == res)
    {
        res = Hal_Wrapper_Configure_Streams(thiz->camId, pStreamCfg);
        if (ISP_RET_SUCCESS != res)
        {
            APP_ERR("Error! res = %d", res);
        }
    }

    if (ISP_RET_SUCCESS == res)
    {
        thiz->status = DEV_STATUS_CONFIGURED;
    }

    APP_FUNC_EXIT();

    return res;
}

/// Interface to execute request of device
static isp_ret_t Device_Process_Request(device_t* thiz,
                                               metadata_t* pMetadata)
{
    isp_ret_t                     res           = ISP_RET_SUCCESS;
    device_pending_t*             pPend         = NULL;
    BOOL                          isNewReq      = FALSE;

    APP_FUNC_ENTER();

    if (NULL == thiz ||
        NULL == thiz->inflightList ||
        NULL == thiz->inflightList->Find ||
        NULL == pMetadata ||
        DEV_STATUS_CONFIGURED > thiz->status)
    {
        res = ISP_RET_INVALID;
        APP_ERR("Error! res = %d", res);
    }
    else
    {
        pPend = thiz->pPend;
    }

    if (ISP_RET_SUCCESS == res)
    {
        if (NULL == pPend)
        {
            pPend = Device_Create_Pending(thiz, pMetadata);

            isNewReq = TRUE;
        }

        if (FALSE == Device_Check_Pending(pPend))
        {
            res = ISP_RET_INGORED_FAILURE;
        }
        else
        {
            Metadata_Transfer_Event(pPend->request.pMetadata, pMetadata);
        }
    }

    if (ISP_RET_SUCCESS == res)
    {
        if (isNewReq)
        {
			pthread_mutex_lock(&thiz->lock);
            thiz->inflightList->Append(thiz->inflightList, (void*)pPend);
			pthread_mutex_unlock(&thiz->lock);

            APP_DBG("New Request: camId %u, frameId %u, length %u",
                pPend->request.camId, pPend->request.frameId, thiz->inflightList->length);

            thiz->pPend = pPend;
        }

        /// Unblocking execute request
        res = Hal_Wrapper_Process_Request(thiz->camId, &pPend->request);
    }

    if (ISP_RET_SUCCESS == res)
    {
        thiz->frameId++;
        thiz->pPend = NULL;
    }
    else
    {
        if (ISP_RET_INGORED_FAILURE == res ||
            ISP_RET_BUSY == res)
        {
            res = ISP_RET_SUCCESS;
        }

        if (ISP_RET_SUCCESS != res)
        {
            APP_ERR("Error! res = %d", res);

			pthread_mutex_lock(&thiz->lock);
            thiz->inflightList->Remove(thiz->inflightList, pPend);
			pthread_mutex_unlock(&thiz->lock);

            Device_Destroy_Pending(thiz, &pPend);
        }
    }

    APP_FUNC_EXIT();

    return res;
}

/// Interface to flush device
static isp_ret_t Device_Flush(device_t* thiz)
{
    isp_ret_t res = ISP_RET_SUCCESS;

    APP_FUNC_ENTER();

    if (NULL == thiz || DEV_STATUS_STREAMING != thiz->status)
    {
        res = ISP_RET_INVALID;
        APP_ERR("Error! res = %d", res);
    }

    if (ISP_RET_SUCCESS == res)
    {
        APP_CFG("Flushing cam %u", thiz->camId);

        res = Hal_Wrapper_Flush(thiz->camId);
        if (ISP_RET_SUCCESS != res)
        {
            APP_ERR("Error! res = %d", res);
        }

        OBJ_LIST_TRAVERSE_DATA(thiz->inflightList, device_pending_t*, pPend)
        {
            Device_Destroy_Pending(thiz, &pPend);
        }

        thiz->inflightList->Clear(thiz->inflightList);

        APP_CFG("Flush done");
    }

    APP_FUNC_EXIT();

    return res;
}

/// Interface to close device
static isp_ret_t Device_Close(device_t* thiz)
{
    isp_ret_t res = ISP_RET_SUCCESS;

    APP_FUNC_ENTER();

    if (NULL == thiz || DEV_STATUS_STREAMING != thiz->status)
    {
        res |= ISP_RET_INVALID;
        APP_ERR("Error! res = %d", res);
    }

    APP_INFO("destroy camera %u %p", thiz->camId, thiz->inflightList);

    if (ISP_RET_SUCCESS == res)
    {
        if (thiz->inflightList)
        {
            /// wait
            u32 curTime = 0;
            while (thiz->inflightList->length > 0)
            {
                if (curTime >= DEFAULT_TIMEOUT_MS)
                {
                    res = ISP_RET_TIMEOUT;
                    APP_ERR("ISP_RET_TIMEOUT!");
                    break;
                }

                Time_OS_Msleep(10);
                curTime += 10;
            }

            Obj_List_Destroy(&thiz->inflightList);
            pthread_mutex_destroy(&thiz->lock);
        }

        res = Device_Release_Bufs(thiz);
        if (ISP_RET_SUCCESS != res)
        {
            APP_ERR("Error! res = %d", res);
        }

        {
            res = Hal_Wrapper_Close(thiz->camId);
            if (ISP_RET_SUCCESS != res)
            {
                APP_ERR("Error! res = %d", res);
            }
        }

        thiz->frameId = 0;
    }

    if (NULL != thiz->pCapMeta)
    {
        Metadata_Destroy(&thiz->pCapMeta);
    }

    thiz->status = DEV_STATUS_UNINITIALIZED;

    APP_FUNC_EXIT();

    return res;
}

/// Interface to dump status / parameters of device
static isp_ret_t Device_Dump(device_t* thiz, u64 dumpFlag)
{
    isp_ret_t res = ISP_RET_SUCCESS;

    APP_FUNC_ENTER();

    if (NULL == thiz || DEV_STATUS_STREAMING != thiz->status)
    {
        res = ISP_RET_INVALID;
        APP_ERR("Error! res = %d", res);
    }

    if (ISP_RET_SUCCESS == res)
    {
        res = Hal_Wrapper_Dump(thiz->camId, dumpFlag);
        if (ISP_RET_SUCCESS != res)
        {
            APP_ERR("Error! res = %d", res);
        }
    }

    APP_FUNC_EXIT();

    return ISP_RET_SUCCESS;
}

/// Get device instance according to camera ID
device_t* Device_Get_Instance(u32 camId)
{
    device_t* pDevice = NULL;

    if (camId < MAX_CAMERA)
    {
        pDevice = &g_device[camId];

        if (DEV_STATUS_UNINITIALIZED == pDevice->status)
        {
            pDevice->status              = DEV_STATUS_INITIALIZED;
            pDevice->camId               = camId;
            pDevice->frameId             = 0;
            pDevice->pStreamCfg          = NULL;
            pDevice->inflightList        = NULL;

            for (u32 i = 0; i < BUF_STREAM_IDX_MAX; i++)
            {
                pDevice->pBufMgr[i] = NULL;
            }

            pDevice->Open                = Device_Open;
            pDevice->Query               = Device_Query;
            pDevice->Initialize          = Device_Initialize;
            pDevice->Configure           = Device_Configure;
            pDevice->Process_Request     = Device_Process_Request;
            pDevice->Flush               = Device_Flush;
            pDevice->Close               = Device_Close;
            pDevice->Dump                = Device_Dump;

			pthread_mutex_init(&pDevice->lock, NULL);
        }
    }

    return pDevice;
}

/// Probe camera device
isp_ret_t Device_Probe(obj_list_t* listProbeInfo)
{
    isp_ret_t res = ISP_RET_SUCCESS;

    APP_FUNC_ENTER();

    if (NULL == listProbeInfo)
    {
        res = ISP_RET_INVALID;

        APP_ERR("Error! res = %d", res);
    }

    if (ISP_RET_SUCCESS == res)
    {
        res = Hal_Wrapper_Probe(listProbeInfo);

        if (ISP_RET_SUCCESS != res)
        {
            APP_ERR("Error! res = %d", res);
        }
    }

    APP_FUNC_EXIT();

    return res;
}

device_t *Device_Acquire(int camid)
{
	return &g_device[camid];
}

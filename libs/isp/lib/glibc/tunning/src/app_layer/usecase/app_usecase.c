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

#include "app_usecase.h"

static app_usecase_t g_usecase[MAX_CAMERA];

static void App_Usecase_Data_Callback(callbacks_ops_t* pCb,
                                             result_t* pResult)
{
    isp_ret_t        res       = ISP_RET_SUCCESS;
    app_usecase_t*   thiz      = NULL;
    callbacks_ops_t* pUpperCbs = NULL;

    if (NULL == pCb ||
        NULL == pResult)
    {
        res = ISP_RET_INVALID;
        APP_ERR("Error! res = %d", res);
    }
    else
    {
        thiz = (app_usecase_t*) pCb;
        pUpperCbs = thiz->usecaseCbs.pUpperCbs;
    }

    if (ISP_RET_SUCCESS == res &&
        pUpperCbs &&
        pUpperCbs->Process_Result)
    {
        pUpperCbs->Process_Result(pUpperCbs, pResult);
    }

    return;
}

static void App_Usecase_Notify(callbacks_ops_t* pCb,
                                    notify_t* pNotifyMsg)
{
    isp_ret_t        res       = ISP_RET_SUCCESS;
    app_usecase_t*   thiz      = NULL;
    callbacks_ops_t* pUpperCbs = NULL;

    if (NULL == pCb ||
        NULL == pNotifyMsg)
    {
        res = ISP_RET_INVALID;
        APP_ERR("Error! res = %d", res);
    }
    else
    {
        thiz = (app_usecase_t*) pCb;
        pUpperCbs = thiz->usecaseCbs.pUpperCbs;
    }

    if (ISP_RET_SUCCESS == res &&
        pUpperCbs &&
        pUpperCbs->Notify)
    {
        pUpperCbs->Notify(pUpperCbs, pNotifyMsg);
    }

    return;
}

static isp_ret_t App_Usecase_Initialize(app_usecase_t* thiz,
                                              callbacks_ops_t* pCbs)
{
    isp_ret_t     res          = ISP_RET_SUCCESS;
    device_t*     pDev         = NULL;

    if (NULL == thiz ||
        NULL ==  thiz->pAppCfg ||
        NULL ==  thiz->pDev)
    {
        res = ISP_RET_INVALID;
        APP_ERR("Error! res = %d", res);
    }
    else
    {
        pDev = thiz->pDev;
    }

    if (ISP_RET_SUCCESS == res &&
        pDev->Open)
    {
        res = pDev->Open(pDev);
        if (ISP_RET_SUCCESS != res)
        {
            APP_ERR("Error! res = %d", res);
        }
    }

    if (ISP_RET_SUCCESS == res &&
        pDev->Initialize)
    {
        thiz->usecaseCbs.cbs.Process_Result   = App_Usecase_Data_Callback;
        thiz->usecaseCbs.cbs.Notify           = App_Usecase_Notify;
        thiz->usecaseCbs.pUpperCbs            = pCbs;
        res = pDev->Initialize(pDev, (callbacks_ops_t*)&thiz->usecaseCbs);
        if (ISP_RET_SUCCESS != res)
        {
            APP_ERR("Error! res = %d", res);
        }
    }

    if (ISP_RET_SUCCESS == res &&
        pDev->Configure)
    {
        res = pDev->Configure(pDev, (stream_config_t*)&thiz->pAppCfg->streamCfg);
        if (ISP_RET_SUCCESS != res)
        {
            APP_ERR("Error! res = %d", res);
        }
    }

    return res;
}

static isp_ret_t App_Usecase_Execute(app_usecase_t* thiz,
                                          app_config_t* pAppCfg)
{
    isp_ret_t     res      = ISP_RET_SUCCESS;
    device_t*     pDev     = NULL;

    if (NULL == thiz ||
        NULL == pAppCfg ||
        NULL == thiz->pDev)
    {
        res = ISP_RET_INVALID;
        APP_ERR("Error! res = %d", res);
    }
    else
    {
        pDev = thiz->pDev;
    }

    if (ISP_RET_SUCCESS == res &&
        pDev->Process_Request)
    {
        res = pDev->Process_Request(pDev, pAppCfg->pMetadata);
        if (ISP_RET_SUCCESS != res)
        {
            APP_ERR("Error! res = %d", res);
        }
    }

    return res;
}

static isp_ret_t App_Usecase_Deinitialize(app_usecase_t* thiz)
{
    isp_ret_t     res      = ISP_RET_SUCCESS;
    device_t*     pDev     = NULL;

    if (NULL == thiz ||
        NULL == thiz->pDev)
    {
        res = ISP_RET_INVALID;
        APP_ERR("Error! res = %d", res);
    }
    else
    {
        pDev = thiz->pDev;
    }

    if (ISP_RET_SUCCESS == res &&
        pDev->Flush)
    {
        res = pDev->Flush(pDev);
        if (ISP_RET_SUCCESS != res)
        {
            APP_ERR("Error! res = %d", res);
        }
    }

    if (ISP_RET_SUCCESS == res &&
        pDev->Close)
    {
        res = pDev->Close(pDev);
        if (ISP_RET_SUCCESS != res)
        {
            APP_ERR("Error! res = %d", res);
        }
    }

    return res;
}

app_usecase_t* App_Usecase_Select_And_Get_Instance(app_config_t*           pAppCfg)
{
    app_usecase_t* pAppUsecase = NULL;

    if (pAppCfg)
    {
        pAppUsecase = &g_usecase[pAppCfg->camId];

        pAppUsecase->usecaseType           = (app_usecase_type_t)pAppCfg->streamCfg.outputStreamNum;
        pAppUsecase->pDev                  = Device_Get_Instance(pAppUsecase->camId);

        if (CAM_USECASE_STATUS_UNINITIALIZED == pAppUsecase->status)
        {
            pAppUsecase->status            = CAM_USECASE_STATUS_INITIALIZED;
            pAppUsecase->camId             = pAppCfg->camId;
            pAppUsecase->Initialize        = App_Usecase_Initialize;
            pAppUsecase->Execute           = App_Usecase_Execute;
            pAppUsecase->Deinitialize      = App_Usecase_Deinitialize;
        }
    }

    return pAppUsecase;
}

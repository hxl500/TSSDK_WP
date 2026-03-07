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
#include "app_layer.h"

extern char8* optarg;

/// APP show fps function
static inline void APP_Layer_Show_FPS(u32 camId,
                                           u32 streamId,
                                           u32 frameId,
                                           stream_info_t* pStreamInfo)
{
    static volatile u64 s_lastTimeStamp[MAX_CAMERA][BUF_STREAM_IDX_MAX];
    static volatile u32 s_frameCount[MAX_CAMERA][BUF_STREAM_IDX_MAX];

    volatile u64 nowTs = Time_OS_Get_Cur_Timestamp();

    if (frameId == 0)
    {
        s_lastTimeStamp[camId][streamId] = nowTs;
        s_frameCount[camId][streamId] = 0;
    }

    s_frameCount[camId][streamId]++;

    volatile u64 diff = nowTs - s_lastTimeStamp[camId][streamId];

    if (diff >= SEC_2_US(3))
    {
        f32 fps = ((f32)s_frameCount[camId][streamId]) * ((f32)SEC_2_US(1)) / ((f32)diff);
        s_frameCount[camId][streamId]++;

        s_lastTimeStamp[camId][streamId] = nowTs;
        s_frameCount[camId][streamId]    = 0;

        if (pStreamInfo)
        {
            PRINTF("ShowFps: Camera[%u], Stream[%u], Frame[%u], Fmt[%u], W x H [%u x %u] ------ FPS: %.2lf",
                camId,
                streamId,
                frameId,
                pStreamInfo->user.fmt,
                pStreamInfo->user.width,
                pStreamInfo->user.height,
                fps);
        }
        else
        {
            PRINTF("ShowFps: Camera[%u], Stream[%u], Frame[%u] ------ FPS: %.2lf",
                camId,
                streamId,
                frameId,
                fps);
        }
    }
}

/// APP dump image
static void APP_Layer_Dump_Stream_Buffer(stream_buffer_t* pStreamBuffers)
{
    static char8 fname[256];
    char8* fextension = NULL;


    switch (pStreamBuffers->info->user.fmt)
    {
        case STREAM_FMT_NV12:
        case STREAM_FMT_NV21:
        case STREAM_FMT_YUV422:
        case STREAM_FMT_YUV444:
            fextension = "yuv";
            break;
        case STREAM_FMT_RGB888:
        case STREAM_FMT_RGB24:
        case STREAM_FMT_RGB565:
        case STREAM_FMT_RAW10:
        case STREAM_FMT_RAW12:
            fextension = "rgb";
            break;
        default:
            fextension = "yuv";
            break;
    }
#ifdef __linux__
    snprintf(fname, sizeof(fname), STORAGE_ROOT_DIR"img_w[%u]_h[%u]_id[%u-%u-%u].%s",
        pStreamBuffers->info->user.width,
        pStreamBuffers->info->user.height,
        pStreamBuffers->identity.camId,
        pStreamBuffers->identity.streamId,
        pStreamBuffers->identity.frameId,
        fextension);
#else
    snprintf(fname, sizeof(fname), STORAGE_ROOT_DIR"img_%u.%s",
        pStreamBuffers->identity.frameId,
        fextension);
#endif

    FileHandle*  pFile = FS_OS_Open(fname, "wb");
    if (NULL == pFile)
    {
        APP_ERR("Open file %s failed", fname);
    }

    if (pFile)
    {
        FS_OS_Write(pFile,
                    (const void*)pStreamBuffers->buffer.bufAddr,
                     pStreamBuffers->info->internal.size);

        FS_OS_Close(pFile);
        pFile = NULL;

        APP_CFG("Dumped image: %s, bufAddr %p, size "FMT_SIZE,
            fname,
            pStreamBuffers->buffer.bufAddr,
            pStreamBuffers->info->internal.size);
    }
}

/// APP data callback function
static void App_Layer_Data_Callback(callbacks_ops_t* pCb,
                                                result_t* pResult)
{
    isp_ret_t       res           = ISP_RET_SUCCESS;
    app_desc_t*     thiz          = NULL;
    device_t*       pDev          = NULL;
    app_config_t*   pAppConfig    = NULL;
    BOOL            needDump      = FALSE;

    if (NULL == pCb ||
        NULL == pResult ||
        pResult->camId >= MAX_CAMERA)
    {
        res = ISP_RET_INVALID;
        APP_ERR("Error! res = %d", res);
    }
    else
    {
        thiz        = (app_desc_t*) pCb;
        pDev        = thiz->ppDev[pResult->camId];
        pAppConfig  = thiz->ppAppConfig[pResult->camId];

        if (NULL == pDev ||
            NULL == pAppConfig)
        {
            res = ISP_RET_INVALID;
            APP_ERR("Error! res = %d, camId = %d, pDev = %p, pAppConfig = %p",
                res, pResult->camId, pDev, pAppConfig);
        }
    }

    if (ISP_RET_SUCCESS == res)
    {
        /// process data
        if (pResult->pMetadata)
        {
            APP_DBG("Received Metadata: Camera %u, frameId %u",
                pResult->camId, pResult->frameId);

			App_Layer_Upper_Data_Callback(pResult->pMetadata, NULL);
		}

		if (pResult->outputStreamNum > 0)
		{
			for (u32 i = 0; i < pResult->outputStreamNum; i++)
			{
				if (pResult->ppOutputStreamBuffers[i])
				{
					APP_DBG("Received buffers: Camera %u, frameId %u, bufAddr %p",
							pResult->camId,
							pResult->frameId,
							pResult->ppOutputStreamBuffers[i]->buffer.bufAddr);

					if (thiz->showFps)
					{
						APP_Layer_Show_FPS(pResult->camId,
								i,
								pResult->frameId,
								pResult->ppOutputStreamBuffers[i]->info);
					}

					if (pAppConfig->dumpSettings.period[i] == 0)
					{
						if (pAppConfig->dumpSettings.num[i] > 0)
						{
							needDump = TRUE;

							if (i == (pResult->outputStreamNum - 1))
							{
								pAppConfig->dumpSettings.num[i]--;
							}
						}
					}
					else if (0 == pResult->frameId % pAppConfig->dumpSettings.period[i])
					{
						needDump = TRUE;
					}

					if (needDump)
					{
						APP_Layer_Dump_Stream_Buffer(pResult->ppOutputStreamBuffers[i]);
					}

					App_Layer_Upper_Data_Callback(NULL, pResult->ppOutputStreamBuffers[i]);
				}
			}
		}
		else if (pAppConfig->streamCfg.outputStreamNum == 1)
		{
			/// Preview only

			if (thiz->showFps)
			{
				APP_Layer_Show_FPS(pResult->camId,
						0,
						pResult->frameId,
						NULL);
            }
        }
    }

    return;
}

/// Check if idle
static BOOL App_Layer_Check_Nonblocking_Sleep(app_desc_t* pAppDesc)
{
    BOOL shouldSleep = FALSE;

    if (pAppDesc &&
        pAppDesc->sleepInfo.startTimeUs > 0 &&
        pAppDesc->sleepInfo.sleepTimeUs > 0)
    {
        u64 curTime =  Time_OS_Get_Cur_Timestamp();

        if (curTime - pAppDesc->sleepInfo.startTimeUs < pAppDesc->sleepInfo.sleepTimeUs)
        {
            shouldSleep = TRUE;
        }
        else
        {
            shouldSleep = FALSE;
            pAppDesc->sleepInfo.startTimeUs = 0;
            pAppDesc->sleepInfo.sleepTimeUs = 0;

            APP_VERB("wait up!!!");
        }
    }

    return shouldSleep;
}

/// APP notify callback function
static void App_Layer_Notify(callbacks_ops_t* pCb,
                                  notify_t* pNotifyMsg)
{
    isp_ret_t       res           = ISP_RET_SUCCESS;
    app_desc_t*     thiz          = NULL;

    if (NULL == pCb ||
        NULL == pNotifyMsg)
    {
        res = ISP_RET_INVALID;
        APP_ERR("Error! res = %d", res);
    }
    else
    {
        thiz = (app_desc_t*) pCb;
    }

    if (ISP_RET_SUCCESS == res)
    {
        /// process msg
        switch (pNotifyMsg->type)
        {
            case NOTIFY_MSG_VOUT_DONE:
            {
                u64 timestamp = 0;

                memcpy(&timestamp, pNotifyMsg->msg, sizeof(u64));
                APP_DBG("NOTIFY_MSG_VOUT_DONE: Camera %u, frameId %u, timestamp %llu",
                    pNotifyMsg->camId, pNotifyMsg->frameId, timestamp);
                break;
            }
            case NOTIFY_MSG_NOP:
            {
                APP_DBG("");
                break;
            }
            case NOTIFY_MSG_FRAME_DONE:
            {
                u64 timestamp = 0;
                memcpy(&timestamp, pNotifyMsg->msg, sizeof(u64));
                APP_DBG("NOTIFY_MSG_FRAME_DONE: Camera %u, frameId %u, timestamp %llu",
                    pNotifyMsg->camId, pNotifyMsg->frameId, timestamp);
                break;
            }
            case NOTIFY_MSG_SHUTTER:
            {
                u64 timestamp = 0;

                memcpy(&timestamp, pNotifyMsg->msg, sizeof(u64));

                APP_DBG("NOTIFY_MSG_SHUTTER: Camera %u, frameId %u, timestamp %llu",
                    pNotifyMsg->camId, pNotifyMsg->frameId, timestamp);

                break;
            }
            case NOTIFY_MSG_ERROR:
            {
              /* #<{(| if (pNotifyMsg->msg[1] % 30 == 0) |)}># */
              /*   APP_ERR("NOTIFY_MSG_ERROR: Camera %u, frameId %u, Msg: 0x%x, FrmNum: %d", */
              /*       pNotifyMsg->camId, pNotifyMsg->frameId, pNotifyMsg->msg[0], pNotifyMsg->msg[1]); */

                break;
            }
            case NOTIFY_MSG_EVT_ACK:
            {
                metadata_event_t evt = { 0 };
                memcpy(&evt, pNotifyMsg->msg, sizeof(metadata_event_t));

				if(thiz->cur_cmd_src_type == CMD_SRC_MPI)
				{
					App_Layer_Upper_Event_Callback(&evt);
				}
				else
				{
					if (evt.status == META_EVT_STATUS_PROCESSED)
					{
						static char8 ackBuf[128] = { 0 };
						snprintf(ackBuf, sizeof(ackBuf), "camId = %u, type = %u, id = %u, dirt = %u, data = %u",
								evt.camId, evt.type, evt.evtId, evt.direction, evt.data);

						tapi_print_event_buffer(&evt);

						if (evt.direction == META_EVT_DIRT_READ)
						{
							APP_INFO("META_EVT_DIRT_READ %s", ackBuf);
						}
						else
						{
							APP_INFO("META_EVT_DIRT_WRITE %s", ackBuf);
							snprintf(ackBuf, sizeof(ackBuf), "OKOK");
						}
#ifdef _BUILD_ISP_TUNNING
						if (thiz->pNissEthernet &&
								thiz->pNissEthernet->Write)
						{
							thiz->pNissEthernet->Write(thiz->pNissEthernet, ackBuf, strlen(ackBuf) + 1);
						}
						if (NULL != thiz->pNissUsb &&
								NULL != thiz->pNissUsb->Write )
						{
							thiz->pNissUsb->Write(thiz->pNissUsb, ackBuf, strlen(ackBuf) + 1);
						}
#endif
					}
				}
                break;
            }
            case NOTIFY_MSG_RECOVERY:
            {
                cmd_t*             pCmd     = NULL;
                notify_rcvy_type_t rcvyType = (notify_rcvy_type_t)pNotifyMsg->msg[0];

                APP_CFG("NOTIFY_MSG_RECOVERY %u", (u32)rcvyType);

                char8 cmdLine[APP_CONFIG_CMDLINE_MAX_LEN] = { 0 };

                {
                    if (NOTIFY_RCVY_HDR_SWITCH == rcvyType)
                    {
                        snprintf(cmdLine, sizeof(cmdLine), "set:camid=%u,hdr=%u",
                                 pNotifyMsg->camId, pNotifyMsg->msg[1]);

                    }
                    else
                    {
                        snprintf(cmdLine, sizeof(cmdLine), "set:camid=%u,restart=1",
                                 pNotifyMsg->camId);
                    }
                    pCmd = App_Config_Parse_Cmdline(cmdLine, strlen(cmdLine) + 1);

                    if (pCmd &&
                        pCmd->cmdId == CMD_SET)
                    {
                        Thread_OS_Msg_Enqueue(thiz->pInputThread, (void*)pCmd, 0);
                    }
                }

                {
                    snprintf(cmdLine, sizeof(cmdLine), "start");
                    pCmd = App_Config_Parse_Cmdline(cmdLine, strlen(cmdLine) + 1);

                    if (pCmd &&
                        pCmd->cmdId == CMD_START)
                    {
                        Thread_OS_Msg_Enqueue(thiz->pInputThread, (void*)pCmd, 0);
                    }
                }

                break;
            }
            case NOTIFY_MSG_AAA_STATS:
                break;
            case NOTIFY_MSG_AAA_INVALID:
                 PRINTF("AAA STATS INVALID");
                break;
            case NOTIFY_MSG_FPS:
            {
                u32 fps = pNotifyMsg->msg[0];
                PRINTF("NOTIFY_MSG_FPS: Camera %u, frameId %u, fps %.2f",
                    pNotifyMsg->camId, pNotifyMsg->frameId, fps/100.0);
                break;
            }
            default:
            {
                res = ISP_RET_INVALID;
                APP_ERR("invalid msg from thiz %p", thiz);

                break;
            }
        }
    }

    return;
}


/* static u32 dev_req_cnt[MAX_CAMERA] = {0}; */
/// Repeat Camera Requests
static isp_ret_t App_Layer_Camera_Repeat_Request(device_t* pDev,
                                                          app_config_t* pAppConfig,
                                                          u32 times)
{
    isp_ret_t       res           = ISP_RET_SUCCESS;

    if (NULL == pDev ||
        NULL == pDev->Process_Request ||
        NULL == pDev->Dump ||
        NULL == pAppConfig)
    {
        res = ISP_RET_INVALID;
        APP_ERR("Error! res = %d", res);
    }

    if (ISP_RET_SUCCESS == res)
    {
        for (u32 i = 0; i < times; i++)
        {
            /* if (0 == (dev_req_cnt[pDev->camId]++ % 500)) { */
            /*   pAppConfig->pMetadata->userSettings.camSession = */
            /*     (META_CAM_SESSION_VISIBLE == pAppConfig->pMetadata->userSettings.camSession) ? META_CAM_SESSION_IR:META_CAM_SESSION_VISIBLE; */
            /* printf("====== app.dev[%d]: {camSession=%d,reqcnt=%d} ======\n", */
            /*   pDev->camId, pAppConfig->pMetadata->userSettings.camSession, dev_req_cnt[pDev->camId] - 1); */
            /* } */

            res = pDev->Process_Request(pDev, pAppConfig->pMetadata);

            if (ISP_RET_SUCCESS         != res)
            {
                APP_WARN("Handle request[%u] failed, res = %d", i, res);

                pDev->Dump(pDev, APP_CONFIG_DUMP_ALL);

                break;
            }
        }
    }

    return res;
}

/// Stop Camera
static isp_ret_t App_Layer_Camera_Stop(device_t* pDev)
{
    isp_ret_t       res           = ISP_RET_SUCCESS;

    APP_FUNC_ENTER();

    if (NULL == pDev ||
        NULL == pDev->Flush ||
        NULL == pDev->Close ||
        NULL == pDev->Dump ||
        DEV_STATUS_UNINITIALIZED == pDev->status)
    {
        res = ISP_RET_INVALID;
        APP_ERR("Error! res = %d",
            res, pDev);
    }

    if (ISP_RET_SUCCESS == res) {
      APP_INFO("camera %u stopping", pDev->camId);

      u32 curTime = 0;
      while (DEV_STATUS_STREAMING != pDev->status) {
        if (curTime >= DEFAULT_TIMEOUT_MS) {
          pDev->status = DEV_STATUS_STREAMING;
          break;
        }
        Time_OS_Msleep(10);
        curTime +=10;
      }
    }

    if (ISP_RET_SUCCESS == res)
    {
        res = pDev->Flush(pDev);
        if (ISP_RET_SUCCESS != res)
        {
            APP_ERR("Error! res = %d", res);

            pDev->Dump(pDev, APP_CONFIG_DUMP_ALL);
        }

        res = pDev->Close(pDev);
        if (ISP_RET_SUCCESS != res)
        {
            APP_ERR("Error! res = %d", res);
        }

        APP_INFO("camera %u stopped", pDev->camId);
    }

    APP_FUNC_EXIT();

    return res;
}

/// Start Camera
static isp_ret_t App_Layer_Camera_Start(device_t* pDev,
                                              app_config_t* pAppConfig,
                                              cbs_t* pAppCbs)
{
    isp_ret_t       res           = ISP_RET_SUCCESS;

    APP_FUNC_ENTER();

    if (NULL == pAppConfig ||
        NULL == pDev ||
        NULL == pAppCbs )
    {
        res = ISP_RET_INVALID;
        APP_ERR("Error! res = %d", res);
    }

    if (DEV_STATUS_INITIALIZED != pDev->status) res = ISP_RET_INGORED_FAILURE;

    if (ISP_RET_SUCCESS == res &&
        pDev->Open)
    {
        APP_INFO("camera %u starting ", pDev->camId);

        res = pDev->Open(pDev);
        if (ISP_RET_SUCCESS != res)
        {
            APP_ERR("Error! res = %d", res);
        }
    }

    if (ISP_RET_SUCCESS == res &&
        pDev->Query)
    {
        res = pDev->Query(pDev);
        if (ISP_RET_SUCCESS != res)
        {
            APP_ERR("Error! res = %d", res);
        }
    }

    if (ISP_RET_SUCCESS == res)
    {
        res = App_Config_Update_Capability_Metadata(pAppConfig, pDev->pCapMeta);
        if (ISP_RET_SUCCESS != res)
        {
            APP_ERR("Error! res = %d", res);
        }
    }

    if (ISP_RET_SUCCESS == res &&
        pDev->Initialize)
    {
        res = pDev->Initialize(pDev, (callbacks_ops_t*)pAppCbs);
        if (ISP_RET_SUCCESS != res)
        {
            APP_ERR("Error! res = %d", res);
        }
    }

    if (ISP_RET_SUCCESS == res &&
        pDev->Configure)
    {
        res = pDev->Configure(pDev, (stream_config_t*)(&pAppConfig->streamCfg));
        if (ISP_RET_SUCCESS != res)
        {
            APP_ERR("Error! res = %d", res);
        }

        APP_INFO("camera %u started", pDev->camId);
    }

    APP_FUNC_EXIT();

    return res;
}

/// APP Layer input loop task
static void APP_Layer_Loop_Get_Input_Cmd(void * argv)
{
    app_desc_t*        pAppDesc                              = (app_desc_t*)argv;
    static char8       cmdLine[APP_CONFIG_CMDLINE_MAX_LEN]   = { 0 };
    u32                readLen                               = 0;
    cmd_t*             pCmd                                  = NULL;

    memset(cmdLine, 0, sizeof(cmdLine));

    if (pAppDesc)
    {
        if (readLen == 0 &&
            pAppDesc->cfgFile.pFile)
        {
            /// Get cmd from file
            readLen = FS_OS_ReadLine(pAppDesc->cfgFile.pFile, cmdLine, sizeof(cmdLine));

            APP_DBG("readLen %u", readLen);

            if (readLen == 0)
            {
                FS_OS_Close(pAppDesc->cfgFile.pFile);
                pAppDesc->cfgFile.pFile = NULL;
            }
        }
#ifdef _BUILD_ISP_TUNNING
        if (readLen == 0 &&
            pAppDesc->pNissUart &&
            pAppDesc->pNissUart->Read)
        {
            /// Get cmd from Uart or terminal
            pAppDesc->pNissUart->Read(pAppDesc->pNissUart, cmdLine, &readLen);
            if (readLen != 0)
            {
                APP_DBG("Applayer accept NISS uart string(%lu): %s", readLen, cmdLine);
            }

        }

        if (readLen == 0 &&
            pAppDesc->pNissEthernet &&
            pAppDesc->pNissEthernet->Read)
        {
            /// Get cmd from Ethernet
            pAppDesc->pNissEthernet->Read(pAppDesc->pNissEthernet, cmdLine, &readLen);
            if (readLen != 0)
            {
                APP_DBG("Applayer accept NISS eth string(%lu) : %s", readLen, cmdLine);
            }
        }

        if (readLen == 0 &&
            pAppDesc->pNissUsb &&
            pAppDesc->pNissUsb->Read)
        {
            /// Get cmd from Usb
            pAppDesc->pNissUsb->Read(pAppDesc->pNissUsb, cmdLine, &readLen);
            if (readLen != 0)
            {
                APP_DBG("Applayer accept NISS usb string(%lu) : %s", readLen, cmdLine);
            }
        }
#endif
        pCmd = App_Config_Parse_Cmdline(cmdLine, readLen);

        if (pCmd && pCmd->cmdId != CMD_NOP)
        {
            PRINTF("Got valid cmdline(%u): %s", readLen, cmdLine);

            Thread_OS_Msg_Enqueue(pAppDesc->pInputThread, (void*)pCmd, 0);
        }
    }
}

/// Get input cmd from queue
static cmd_id_t APP_Layer_Get_And_Parse_Cmd(app_desc_t* pAppDesc, app_config_t** ppAppConfig)
{
	cmd_id_t      cmdId     = CMD_NOP;
	cmd_t* pCmd;

	Thread_OS_Trigger_Loop(pAppDesc->pInputThread, NULL, NULL);

	BOOL shouldSleep = App_Layer_Check_Nonblocking_Sleep(pAppDesc);

	if (FALSE == shouldSleep)
	{
		pCmd = App_Layer_Cmd_Pop(&cmdId, ppAppConfig);
		if(pCmd == NULL)
		{
			pCmd = (cmd_t*)Thread_OS_Msg_Dequeue(pAppDesc->pInputThread, 0);
			if(pCmd)
			{
				pAppDesc->cur_cmd_src_type = CMD_SRC_ETH;
			}
		}
		else
		{
			pAppDesc->cur_cmd_src_type = CMD_SRC_MPI;
		}

		if (pCmd)
		{
			cmdId = pCmd->cmdId;

			/// cmd will be parsed and destroied
			App_Config_Create_By_Cmd(pCmd, ppAppConfig);
		}
	}
    else
    {
        cmdId = CMD_NOP;
    }

    return cmdId;
}

/// Initialize function
static isp_ret_t App_Layer_Init(app_desc_t* pAppDesc)
{
    isp_ret_t     res = ISP_RET_SUCCESS;

    APP_FUNC_ENTER();

    if (NULL == pAppDesc)
    {
        res = ISP_RET_INVALID;
        APP_ERR("Error! res = %d", res);
    }

    if (ISP_RET_SUCCESS == res)
    {
        pAppDesc->showFps = TRUE;
    }

    if (ISP_RET_SUCCESS == res)
    {
#ifdef _BUILD_ISP_TUNNING
        pAppDesc->pNissUart         = Niss_Get_Instance(NISS_UART);
        pAppDesc->pNissEthernet     = Niss_Get_Instance(NISS_ETHERNET);
#else
        pAppDesc->pNissUart         = NULL; //Niss_Get_Instance(NISS_UART);
        pAppDesc->pNissEthernet     = NULL; //Niss_Get_Instance(NISS_ETHERNET);
#endif
        pAppDesc->pNissUsb          = NULL; //Niss_Get_Instance(NISS_USB);
        pAppDesc->pNissPlatform     = NULL; //Niss_Get_Instance(NISS_PLATFORM);
        pAppDesc->pNissHdmi         = NULL; //Niss_Get_Instance(NISS_HDMI);

#ifdef _BUILD_ISP_TUNNING
        if (pAppDesc->pNissUart &&
            pAppDesc->pNissUart->Initialize)
        {
            res = pAppDesc->pNissUart->Initialize(pAppDesc->pNissUart);
            if (ISP_RET_SUCCESS != res)
            {
                APP_ERR("Error! res = %d", res);
            }
        }

        if (pAppDesc->pNissEthernet &&
            pAppDesc->pNissEthernet->Initialize)
        {
            res = pAppDesc->pNissEthernet->Initialize(pAppDesc->pNissEthernet);
            if (ISP_RET_SUCCESS != res)
            {
                APP_ERR("Error! res = %d", res);
            }
        }

        if (pAppDesc->pNissUsb &&
            pAppDesc->pNissUsb->Initialize)
        {
            res = pAppDesc->pNissUsb->Initialize(pAppDesc->pNissUsb);
            if (ISP_RET_SUCCESS != res)
            {
                APP_ERR("Error! res = %d", res);
            }
        }

        /// Note: platform must be after ethernet on bare-metal
        if (pAppDesc->pNissPlatform &&
            pAppDesc->pNissPlatform->Initialize)
        {
            res = pAppDesc->pNissPlatform->Initialize(pAppDesc->pNissPlatform);
            if (ISP_RET_SUCCESS != res)
            {
                APP_ERR("Error! res = %d", res);
            }
        }

        if (pAppDesc->pNissHdmi &&
            pAppDesc->pNissHdmi->Initialize)
        {
            res = pAppDesc->pNissHdmi->Initialize(pAppDesc->pNissHdmi);
            if (ISP_RET_SUCCESS != res)
            {
                APP_ERR("Error! res = %d", res);
            }
        }
#endif
    }

    if (ISP_RET_SUCCESS == res)
    {
        const char8* filename = (const char8*)pAppDesc->cfgFile.sFilename;
        if (strlen(filename))
        {
            pAppDesc->cfgFile.pFile = FS_OS_Open(filename, "rb");

            if (pAppDesc->cfgFile.pFile)
            {
                size_t fileSize = FS_OS_Size(pAppDesc->cfgFile.pFile);

                PRINTF("Read cfg from %s , size "FMT_SIZE,
                    filename, fileSize);
            }
            else
            {
                APP_ERR("failed to open %s", filename);
            }
        }
    }

    if (ISP_RET_SUCCESS == res)
    {
        thread_os_create_info_t threadCreateInfo =
        {
            .name     = "APP_Input_Thread",
            .loop     = APP_Layer_Loop_Get_Input_Cmd,
            .blocking = FALSE,
            .msgQNum  = 1,
            .sleepUs  = 33 * 1000,
            .arg      = pAppDesc,

        };

        pAppDesc->pInputThread = Thread_OS_Create(&threadCreateInfo);
    }

    PRINTF("Please enter cmd:");

    APP_FUNC_EXIT();

    return res;
}

/// Deinitialize function
static isp_ret_t App_Layer_Deinit(app_desc_t* pAppDesc)
{
    isp_ret_t    res = ISP_RET_SUCCESS;

    APP_FUNC_ENTER();

    if (NULL == pAppDesc)
    {
        res = ISP_RET_INVALID;
        APP_ERR("Error! res = %d", res);
    }

    if (ISP_RET_SUCCESS == res)
    {
        /// Destroy all devices
        for (s32 i = MAX_CAMERA - 1; i >= 0; i--)
        {
            if (pAppDesc->ppAppConfig[i] &&
                pAppDesc->ppDev[i])
            {
                res = App_Layer_Camera_Stop(pAppDesc->ppDev[i]);
                if (ISP_RET_SUCCESS != res)
                {
                    APP_ERR("stop failed for camera %u! res = %d", pAppDesc->ppAppConfig[i]->camId, res);
                }

                res = App_Config_Destroy(&pAppDesc->ppAppConfig[i]);
                if (ISP_RET_SUCCESS != res)
                {
                    APP_ERR("Error! res = %d", res);
                }

                pAppDesc->ppDev[i]       = NULL;
            }
        }
    }
#ifdef _BUILD_ISP_TUNNING
    if (ISP_RET_SUCCESS == res &&
        pAppDesc->pNissUart)
    {
        if (pAppDesc->pNissUart->Deinitialize)
        {
            res = pAppDesc->pNissUart->Deinitialize(pAppDesc->pNissUart);
            if (ISP_RET_SUCCESS != res)
            {
                APP_ERR("Error! res = %d", res);
            }
        }

        pAppDesc->pNissUart = NULL;
    }

    if (ISP_RET_SUCCESS == res &&
        pAppDesc->pNissEthernet)
    {
        if (pAppDesc->pNissEthernet->Deinitialize)
        {
            res = pAppDesc->pNissEthernet->Deinitialize(pAppDesc->pNissEthernet);
            if (ISP_RET_SUCCESS != res)
            {
                APP_ERR("Error! res = %d", res);
            }
        }

        pAppDesc->pNissEthernet = NULL;
    }

    if (ISP_RET_SUCCESS == res &&
        pAppDesc->pNissUsb)
    {
        if (pAppDesc->pNissUsb->Deinitialize)
        {
            res = pAppDesc->pNissUsb->Deinitialize(pAppDesc->pNissUsb);
            if (ISP_RET_SUCCESS != res)
            {
                APP_ERR("Error! res = %d", res);
            }
        }

        pAppDesc->pNissUsb = NULL;
    }

    if (ISP_RET_SUCCESS == res &&
        pAppDesc->pNissHdmi)
    {
        if (pAppDesc->pNissHdmi->Deinitialize)
        {
            res = pAppDesc->pNissHdmi->Deinitialize(pAppDesc->pNissHdmi);
            if (ISP_RET_SUCCESS != res)
            {
                APP_ERR("Error! res = %d", res);
            }
        }

        pAppDesc->pNissHdmi = NULL;
    }

    if (ISP_RET_SUCCESS == res &&
        pAppDesc->pNissPlatform)
    {
        if (pAppDesc->pNissPlatform->Deinitialize)
        {
            res = pAppDesc->pNissPlatform->Deinitialize(pAppDesc->pNissPlatform);
            if (ISP_RET_SUCCESS != res)
            {
                APP_ERR("Error! res = %d", res);
            }
        }

        pAppDesc->pNissPlatform = NULL;
    }
#endif

    if (ISP_RET_SUCCESS == res &&
        pAppDesc->cfgFile.pFile)
    {
        FS_OS_Close(pAppDesc->cfgFile.pFile);
        pAppDesc->cfgFile.pFile = NULL;
    }

    if (ISP_RET_SUCCESS == res &&
        pAppDesc->pInputThread)
    {
        Thread_OS_Destroy(&pAppDesc->pInputThread);
    }

    Log_Set_Path(NULL, NULL);

    APP_FUNC_EXIT();

    return res;
}

/// APP loop function
static isp_ret_t App_Layer_Loop(app_desc_t* pAppDesc)
{
    isp_ret_t       res           = ISP_RET_SUCCESS;

    if (NULL == pAppDesc)
    {
        res = ISP_RET_INVALID;
        APP_ERR("Error! res = %d", res);
    }

    if (ISP_RET_SUCCESS == res)
    {
        pAppDesc->isRunning = TRUE;

        PRINTF(" --- App start to run ---");

        while (pAppDesc->isRunning)
        {
            app_config_t*   pAppConfig    = NULL;
            cmd_id_t        cmdId         = CMD_NOP;
            res                           = ISP_RET_SUCCESS;

            /// 1. Parse command
            cmdId = APP_Layer_Get_And_Parse_Cmd(pAppDesc, &pAppConfig);

            switch(cmdId)
            {
                case CMD_SET:
                {
                    if (NULL == pAppConfig ||
                        pAppConfig->camId >= MAX_CAMERA)
                    {
                        res = ISP_RET_INGORED_FAILURE;
                        APP_ERR("Error! res = %d", res);
                    }

                    if (ISP_RET_SUCCESS == res)
                    {
                        pAppDesc->ppAppConfig[pAppConfig->camId] = pAppConfig;
                    }

                    break;
                }
                case CMD_START:
                {
                    /// Check restart
                    BOOL needRestartAll = FALSE;

                    for (s32 i = MAX_CAMERA - 1; i >= 0; i--)
                    {
                        if (pAppDesc->ppAppConfig[i] &&
                            TRUE == pAppDesc->ppAppConfig[i]->needRestart &&
                            pAppDesc->ppDev[i] &&
                            DEV_STATUS_STREAMING == pAppDesc->ppDev[i]->status)
                        {
                            needRestartAll = TRUE;

                            break;
                        }
                    }

                    /// Stop all cameras by inversed sequence
                    if (needRestartAll)
                    {
                        for (s32 i = MAX_CAMERA - 1; i >= 0; i--)
                        {
                            if (pAppDesc->ppAppConfig[i] &&
                                pAppDesc->ppDev[i] &&
                                DEV_STATUS_STREAMING == pAppDesc->ppDev[i]->status)
                            {
                                res = App_Layer_Camera_Stop(pAppDesc->ppDev[i]);
                                if (ISP_RET_SUCCESS != res)
                                {
                                    APP_ERR("stop failed for camera %u! res = %d", i, res);
                                }
                            }
                        }
                    }

                    /// Start all cameras
                    for (s32 i = MAX_CAMERA - 1; i >= 0; i--)
                    {
                        if (pAppDesc->ppAppConfig[i])
                        {
                            if (NULL == pAppDesc->ppDev[i] ||
                                DEV_STATUS_UNINITIALIZED == pAppDesc->ppDev[i]->status)
                            {
                                /// New camera
                                pAppDesc->ppDev[i] = Device_Get_Instance(i);
                                if (NULL == pAppDesc->ppDev[i])
                                {
                                    res = ISP_RET_INVALID;
                                    APP_ERR("Error! res = %d", res);
                                }
                            }

                            if (ISP_RET_SUCCESS == res &&
                                DEV_STATUS_INITIALIZED == pAppDesc->ppDev[i]->status)
                            {
                                pAppDesc->appCbs.cbs.Process_Result      = App_Layer_Data_Callback;
                                pAppDesc->appCbs.cbs.Notify              = App_Layer_Notify;
                                pAppDesc->appCbs.pUpperCbs               = NULL;

                                res = App_Layer_Camera_Start(pAppDesc->ppDev[i], pAppDesc->ppAppConfig[i], &(pAppDesc->appCbs));
                                if (ISP_RET_SUCCESS != res)
                                {
                                    APP_ERR("start failed for camera %u! res = %d", i, res);
                                }
                            }

                            if (ISP_RET_SUCCESS != res)
                            {
                                res = App_Config_Destroy(&pAppDesc->ppAppConfig[i]);
                                if (ISP_RET_SUCCESS != res)
                                {
                                    APP_ERR("Error! res = %d", res);
                                }

                                pAppDesc->ppDev[i]       = NULL;
                            }
                        }
                    }

                    break;
                }
                case CMD_DUMP:
                {
                    if (NULL == pAppConfig ||
                        pAppConfig->camId >= MAX_CAMERA)
                    {
                        res = ISP_RET_INGORED_FAILURE;
                        APP_ERR("Error! res = %d", res);
                    }

                    if (ISP_RET_SUCCESS == res)
                    {
                        device_t* pDev = pAppDesc->ppDev[pAppConfig->camId];

                        if (pAppConfig->dumpSettings.dumpAnalysis &&
                            pDev->Dump)
                        {
                            pDev->Dump(pDev, pAppConfig->dumpSettings.dumpAnalysis);
                        }
                    }

                    break;
                }
                case CMD_EVT:
                {
                    if (NULL == pAppConfig ||
                        pAppConfig->camId >= MAX_CAMERA)
                    {
                        res = ISP_RET_INGORED_FAILURE;
                        APP_ERR("Error! res = %d", res);
                    }

                    /// metadata updated

                    break;
                }
                case CMD_STOP:
                {
                    /// Stop all cameras
                    for (s32 i = MAX_CAMERA - 1; i >= 0; i--)
                    {
                        if (pAppDesc->ppAppConfig[i] &&
                            pAppDesc->ppDev[i])
                        {
                            res = App_Layer_Camera_Stop(pAppDesc->ppDev[i]);
                            if (ISP_RET_SUCCESS != res)
                            {
                                APP_ERR("stop failed for camera %u! res = %d", pAppDesc->ppAppConfig[i]->camId, res);
                            }

                            res = App_Config_Destroy(&pAppDesc->ppAppConfig[i]);
                            if (ISP_RET_SUCCESS != res)
                            {
                                APP_ERR("Error! res = %d", res);
                            }

                            pAppDesc->ppDev[i]       = NULL;
                        }
                    }

                    break;
                }
                case CMD_SLEEP:
                {
                    if (NULL == pAppConfig ||
                        pAppConfig->camId >= MAX_CAMERA)
                    {
                        res = ISP_RET_INGORED_FAILURE;
                        APP_ERR("Error! res = %d", res);
                    }

                    if (ISP_RET_SUCCESS == res &&
                        pAppDesc->sleepInfo.sleepTimeUs == 0)
                    {
                        u32 sleepTime = pAppConfig->sleepSecond;

                        pAppDesc->sleepInfo.startTimeUs = Time_OS_Get_Cur_Timestamp();
                        pAppDesc->sleepInfo.sleepTimeUs = (u64)(sleepTime) * 1e6;

                        APP_INFO("FAKE SLEEP: startTimeUs = %lld, sleepTimeUs = %lld",
                            pAppDesc->sleepInfo.startTimeUs,
                            pAppDesc->sleepInfo.sleepTimeUs);

                        //sleep(sleepTime);
                    }

                    break;
                }
                case CMD_LOG:
                {
                    if (NULL == pAppConfig ||
                        pAppConfig->camId >= MAX_CAMERA)
                    {
                        res = ISP_RET_INGORED_FAILURE;
                        APP_ERR("Error! res = %d", res);
                    }

                    if (ISP_RET_SUCCESS == res)
                    {
                        app_config_log_t* pLogCfg = &pAppConfig->logCfg;

                        if (pLogCfg->showFpsValid)
                        {
                            pAppDesc->showFps = pLogCfg->showFps;
                        }

                        if (pLogCfg->levelMaskValid ||
                            pLogCfg->ispMaskValid ||
                            pLogCfg->sw3aMaskValid)
                        {
                            Log_Set_Level(pLogCfg->levelMask, pLogCfg->ispMask, pLogCfg->sw3aMask);
                        }

                        if (pLogCfg->simpleModeValid)
                        {
                            Log_Set_Simple_Mode(pLogCfg->simpleMode);
                        }

                        if (pLogCfg->logPathValid ||
                            pLogCfg->tracePathValid)
                        {
                            Log_Set_Path(pLogCfg->logPath, pLogCfg->tracePath);
                        }
#ifdef _BUILD_ISP_TUNNING
                        if (pLogCfg->baudRateValid &&
                            pAppDesc->pNissUart &&
                            pAppDesc->pNissUart->Configure)
                        {
                            pAppDesc->pNissUart->Configure(pAppDesc->pNissUart, &(pLogCfg->baudRate));
                        }

                        if (pAppDesc->pNissEthernet &&
                            pAppDesc->pNissEthernet->Write)
                        {
                            pAppDesc->pNissEthernet->Write(pAppDesc->pNissEthernet, "OK", 3);
                        }

                        if (pAppDesc->pNissUsb &&
                            pAppDesc->pNissUsb->Write)
                        {
                            pAppDesc->pNissUsb->Write(pAppDesc->pNissUsb, "OK", 3);
                        }
#endif
                    }

                    break;
                }
                case CMD_EXIT:
                {
                    pAppDesc->isRunning = FALSE;

                    break;
                }
#ifdef ARMCB_STEST_ENABLE
                case CMD_STEST:
                {
                    sanity_test_params_t* pStestCfg = &pAppConfig->stestCfg;
                    res = Stest_Update_Glb_Params(pStestCfg);
                    if (ISP_RET_SUCCESS != res)
                    {
                        APP_ERR("Error! res = %d sCamId(%d) sTCaseId(%d)",
                                res, pStestCfg->sCamId, pStestCfg->sTCaseId);
                    }
                    break;
                }
#endif
                case CMD_NOP:
                default:
                {
                    break;
                }
            }

            /// 2. Execute request
            if (ISP_RET_SUCCESS == res)
            {
				App_Layer_Upper_Request_Fast_Callback();

                for (s32 i = MAX_CAMERA - 1; i >= 0; i--)
                {
                    if (pAppDesc->ppDev[i] &&
                        DEV_STATUS_CONFIGURED <= pAppDesc->ppDev[i]->status &&
                        pAppDesc->ppAppConfig[i])
                    {
                        res = App_Layer_Camera_Repeat_Request(pAppDesc->ppDev[i], pAppDesc->ppAppConfig[i], 1);
                        if (ISP_RET_SUCCESS != res)
                        {
                            APP_WARN("Handle request failed for camera %u, res = %d", i, res);
                        }
                    }
                }
            }
#ifdef _BUILD_ISP_TUNNING
            if (ISP_RET_SUCCESS == res ||
                ISP_RET_INGORED_FAILURE == res)
            {
                /// Network process
                if (pAppDesc->pNissEthernet &&
                    pAppDesc->pNissEthernet->Execute)
                {
                    pAppDesc->pNissEthernet->Execute(pAppDesc->pNissEthernet, NULL);

                }

                /// Usb process
                if (pAppDesc->pNissUsb &&
                    pAppDesc->pNissUsb->Execute)
                {
                    pAppDesc->pNissUsb->Execute(pAppDesc->pNissUsb, NULL);

                }

                /// Uart process
                if (pAppDesc->pNissUart &&
                    pAppDesc->pNissUart->Execute)
                {
                    pAppDesc->pNissUart->Execute(pAppDesc->pNissUart, NULL);

                }
            }
#endif

            /// 3. check running
            if (ISP_RET_SUCCESS != res &&
                ISP_RET_INGORED_FAILURE != res)
            {
                pAppDesc->isRunning = FALSE;
            }
            else
            {
				//TODO sleep(per frame time - used time before)
                /* Time_OS_Msleep(10); */
            }
        }
    }

    return res;
}

/// APP process entry
s32 App_Layer_Process(app_desc_t *pAppDesc)
{
    isp_ret_t       res = ISP_RET_SUCCESS;

    res = App_Layer_Init(pAppDesc);
    if (ISP_RET_SUCCESS != res)
    {
        APP_ERR("Error! res = %d", res);
    }

    if (ISP_RET_SUCCESS == res)
    {
        res = App_Layer_Loop(pAppDesc);
        if (ISP_RET_SUCCESS != res)
        {
            APP_ERR("Error! res = %d", res);
        }
    }

    if (ISP_RET_SUCCESS == res)
    {
        res = App_Layer_Deinit(pAppDesc);
        if (ISP_RET_SUCCESS != res)
        {
            APP_ERR("Error! res = %d", res);
        }
    }

    APP_CFG("exiting...");

    return res;
}


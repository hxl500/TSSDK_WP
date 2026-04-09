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
#include "app_config.h"

static app_config_t         g_config[MAX_CAMERA];

static meta_ife_data_t ifeTune = { 0 };
static tapi_buffer_t tbuffer; //4kB
tapi_buffer_t *get_tbuf(void)
{
  return &tbuffer;
}


/// define cmd token list
static char8* const g_cmdToken[] =
{
    [CMD_SET]              = (char8* const)"set",
    [CMD_START]            = (char8* const)"start",
    [CMD_DUMP]             = (char8* const)"dump",
    [CMD_STOP]             = (char8* const)"stop",
    [CMD_EXIT]             = (char8* const)"exit",
    [CMD_SLEEP]            = (char8* const)"sleep",
    [CMD_LOG]              = (char8* const)"log",
    [CMD_EVT]              = (char8* const)"evt",
#ifdef ARMCB_STEST_ENABLE
    [CMD_STEST]            = (char8* const)"stest",
#endif
    [CMD_MAX]              = NULL,
};

/// define format token list
static char8* const g_fmtToken[] =
{
    [STREAM_FMT_NV12]      = (char8* const)"NV12",
    [STREAM_FMT_NV21]      = (char8* const)"NV21",
    [STREAM_FMT_RGB888]    = (char8* const)"RGB888",
    [STREAM_FMT_RGB24]     = (char8* const)"RGB24",
    [STREAM_FMT_RGB565]     = (char8* const)"RGB565",
    [STREAM_FMT_RAW10]     = (char8* const)"RAW10",
    [STREAM_FMT_RAW12]     = (char8* const)"RAW12",
    [STREAM_FMT_MAX]       = NULL,
};

/// define set param token list
static char8* const g_paramSetToken[] =
{
    [PARAM_SET_CAMID]               = (char8* const)"camid",
    [PARAM_SET_FPS]                 = (char8* const)"fps",
    [PARAM_SET_PREVIEW_WIDTH]       = (char8* const)"pWidth",
    [PARAM_SET_PREVIEW_HEIGHT]      = (char8* const)"pHeight",
    [PARAM_SET_VIDEO_WIDTH]         = (char8* const)"vWidth",
    [PARAM_SET_VIDEO_HEIGHT]        = (char8* const)"vHeight",
    [PARAM_SET_VIDEO_FMT]           = (char8* const)"vFmt",
    [PARAM_SET_HDR]                 = (char8* const)"hdr",
    [PARAM_SET_AEC_MODE]            = (char8* const)"aecMode",
    [PARAM_SET_AWB_MODE]            = (char8* const)"awbMode",
    [PARAM_SET_AF_MODE]             = (char8* const)"afMode",
    [PARAM_SET_SEN_TEST_MODE]       = (char8* const)"senTestMode",
    [PARAM_SET_FORCE_RESTART]       = (char8* const)"restart",
    [PARAM_SET_MAX_SEN_GAIN]        = (char8* const)"maxSenGain",

    [PARAM_SET_ALG_REG]             = (char8* const)"algReg",
    [PARAM_SET_ALG_BIND]            = (char8* const)"algBind",
    [PARAM_SET_AAA_COMBINE]         = (char8* const)"aaaCombine",
    [PARAM_SET_CAM_SESSION]         = (char8* const)"camSession",
    [PARAM_SET_CAM_ONLINE]          = (char8* const)"camOnline",
    [PARAM_SET_RTP_ONLINE]         = (char8* const)"rtpOnline",
    [PARAM_SET_DUALCAM_DYFPS]       = (char8* const)"dualCamDyfps",
    [PARAM_SET_MAX]                 = NULL,
};

/// define dump param token list
static char8* const g_paramDumpToken[] =
{
    [PARAM_DUMP_CAMID]              = (char8* const)"camid",
    [PARAM_DUMP_ANALYSIS]           = (char8* const)"analysis",
    [PARAM_DUMP_NUM]                = (char8* const)"num",
    [PARAM_DUMP_PERIOD]             = (char8* const)"period",
    [PARAM_DUMP_MAX]                = NULL,
};

/// define sleep param token list
static char8* const g_paramSleeppToken[] =
{
    [PARAM_SLEEP_TIME]              = (char8* const)"time",
    [PARAM_SLEEP_MAX]               = NULL,
};

/// define log param token list
static char8* const g_paramLogToken[] =
{
    [PARAM_LOG_LEVEL_MASK]          = (char8* const)"levelMask",
    [PARAM_LOG_ISP_MASK]            = (char8* const)"ispMask",
    [PARAM_LOG_3A_MASK]             = (char8* const)"sw3aMask",
    [PARAM_LOG_SIMPLE_MODE]         = (char8* const)"logSimple",
    [PARAM_LOG_PATH]                = (char8* const)"logPath",
    [PARAM_LOG_TRACE_PATH]          = (char8* const)"tracePath",
    [PARAM_LOG_SHOW_FPS]            = (char8* const)"showFps",
    [PARAM_LOG_UART_BAUDRATE]       = (char8* const)"baudRate",
    [PARAM_LOG_MAX]                 = NULL,
};

/// define event param token list
static char8* const g_paramEvtToken[] =
{
    [PARAM_EVT_CAMID]               = (char8* const)"camid",
    [PARAM_EVT_TYPE]                = (char8* const)"type",
    [PARAM_EVT_ID]                  = (char8* const)"id",
    [PARAM_EVT_DIRECTION]           = (char8* const)"dirt",
    [PARAM_EVT_DATA]                = (char8* const)"data",
    [PARAM_EVT_MAX]                 = NULL,
};

/// Update APP internal metadata with device capability
isp_ret_t App_Config_Update_Capability_Metadata(app_config_t* thiz, metadata_t* pCapMetadata)
{
    isp_ret_t   res   = ISP_RET_SUCCESS;

    if (NULL == thiz ||
        NULL == pCapMetadata)
    {
        res = ISP_RET_INVALID;
        APP_ERR("Error! res = %d", res);
    }

    if (ISP_RET_SUCCESS == res)
    {
        u32 camId = META_GET_VALUE(pCapMetadata,
                                   METADATA_CAP_CAMERA_ID,
                                   u32);

        res = META_SET(thiz->pMetadata,
                       METADATA_CAP_CAMERA_ID,
                       camId);
    }

    /// @TODO: Compare capability with config params

    return res;
}

/// Update APP internal metadata
isp_ret_t App_Config_Update_Metadata(app_config_t* thiz)
{
    isp_ret_t   res   = ISP_RET_SUCCESS;
    BOOL cam_online[MAX_CAMERA]={FALSE};
    if (NULL == thiz)
    {
        res = ISP_RET_INVALID;
        APP_ERR("Error! res = %d", res);
    }

    if (ISP_RET_SUCCESS == res)
    {
        res = META_SET(thiz->pMetadata,
                       METADATA_USER_STREAM_CFG,
                       thiz->streamCfg);
    }

    if (ISP_RET_SUCCESS == res)
    {
        res = META_SET(thiz->pMetadata,
                       METADATA_USER_HDR_MODE,
                       thiz->advSettings.hdrMode);
    }

    if (ISP_RET_SUCCESS == res)
    {
        res = META_SET(thiz->pMetadata,
                       METADATA_USER_AEC_MODE,
                       thiz->advSettings.sw3a.aecMode);
    }

    if (ISP_RET_SUCCESS == res)
    {
        res = META_SET(thiz->pMetadata,
                       METADATA_USER_AWB_MODE,
                       thiz->advSettings.sw3a.awbMode);
    }

    if (ISP_RET_SUCCESS == res)
    {
        res = META_SET(thiz->pMetadata,
                       METADATA_USER_AF_MODE,
                       thiz->advSettings.sw3a.afMode);
    }

    if (ISP_RET_SUCCESS == res)
    {
        res = META_SET(thiz->pMetadata,
                       METADATA_USER_SEN_TEST_MODE,
                       thiz->advSettings.senTestMode);
    }

    if (ISP_RET_SUCCESS == res)
    {
            res = META_SET(thiz->pMetadata,
                           METADATA_USER_MAX_SEN_GAIN,
                           thiz->advSettings.maxSenGain);

    }

    if (ISP_RET_SUCCESS == res) {
      res = META_SET(thiz->pMetadata, METADATA_USER_ALG_REG, thiz->advSettings.algreg);
    }

    if (ISP_RET_SUCCESS == res) {
      res = META_SET(thiz->pMetadata, METADATA_USER_ALG_BIND, thiz->advSettings.algbind);
    }

    if (ISP_RET_SUCCESS == res) {
      res = META_SET(thiz->pMetadata, METADATA_USER_CAM_SESSION, thiz->advSettings.camSession);
    }

    if (ISP_RET_SUCCESS == res) {
      res = META_SET(thiz->pMetadata, METADATA_USER_PREFERENCE, thiz->advSettings.preference);
    }

    if (ISP_RET_SUCCESS == res) {
      res = META_SET(thiz->pMetadata, METADATA_USER_DUALCAM_DYFPS, thiz->advSettings.dynamicFps);
    }
	
	if (ISP_RET_SUCCESS == res) {
		res = META_SET(thiz->pMetadata, METADATA_USER_ISP_USR_EXP, thiz->advSettings.usr_exp);
	}
	
	if (ISP_RET_SUCCESS == res) {
		res = META_SET(thiz->pMetadata, METADATA_USER_ISP_USR_FPS, thiz->advSettings.usr_fps);
	}
    if (ISP_RET_SUCCESS == res)
    {
        if (thiz->event.status == META_EVT_STATUS_PROCESSING)
        {
            res = Metadata_Push_Event(thiz->pMetadata, &thiz->event);
        }
    }

    if (ISP_RET_SUCCESS == res)
    {
        /// Global settings for all cameras

        u8 activeCfg = 0;
        for (u32 i = 0; i < MAX_CAMERA; i++)
        {
            if (g_config[i].status != APP_CFG_STATUS_UNINITIALIZED)
            {
                activeCfg++;
            }
        }

        meta_multiply_cam_mode_t multiCamMode = META_MULTICAM_MODE_SINGLE_CAM;
        meta_aaa_comb_type_t aaaCombine = META_AAA_COMB_TYPE_IVL;

        if (1 == activeCfg)
        {
            multiCamMode = META_MULTICAM_MODE_SINGLE_CAM;
        }
        else if (2 == activeCfg)
        {
            if (META_HDR_MODE_DISABLE == g_config[0].advSettings.hdrMode &&
                META_HDR_MODE_DISABLE == g_config[1].advSettings.hdrMode)
            {
                multiCamMode = META_MULTICAM_MODE_DUAL_CAM_LINEAR_LINEAR;
            }
            else if ((META_HDR_DOL2_MODE == g_config[0].advSettings.hdrMode ||
                     META_HDR_STAGGER_2_MODE == g_config[0].advSettings.hdrMode) &&
                     META_HDR_MODE_DISABLE == g_config[1].advSettings.hdrMode)
            {
                multiCamMode = META_MULTICAM_MODE_DUAL_CAM_HDR2_LINEAR;
            }
            else
            {
                res = ISP_RET_INVALID;
                APP_ERR("Unsupport multiply cam mode!", res);
            }

            if (META_AAA_COMB_TYPE_STITCH == g_config[0].advSettings.aaaCombine &&
                META_AAA_COMB_TYPE_STITCH == g_config[1].advSettings.aaaCombine) {
              aaaCombine = g_config[0].advSettings.aaaCombine;
            }
            if (TRUE == g_config[0].advSettings.camOnline)
            {
                cam_online[0] = TRUE;
            }
            if(TRUE == g_config[1].advSettings.camOnline)
            {
                cam_online[1] = TRUE;
            }
        }
        else if (3 == activeCfg)
        {
          if (META_HDR_MODE_DISABLE == g_config[0].advSettings.hdrMode &&
              META_HDR_MODE_DISABLE == g_config[1].advSettings.hdrMode &&
              META_HDR_MODE_DISABLE == g_config[2].advSettings.hdrMode) {
            multiCamMode = META_MULTICAM_MODE_TRIP_CAM_LINEAR_LINEAR;
          } else {
            res = ISP_RET_INVALID;
            APP_ERR("Unsupport multiply cam mode!", res);
          }

          if ((g_config[0].advSettings.aaaCombine == g_config[1].advSettings.aaaCombine) &&
              (g_config[2].advSettings.aaaCombine == g_config[1].advSettings.aaaCombine)) {
            if (META_AAA_COMB_TYPE_STITCH == g_config[0].advSettings.aaaCombine ||
                META_AAA_COMB_TYPE_HYBIRD == g_config[0].advSettings.aaaCombine)
              aaaCombine = g_config[0].advSettings.aaaCombine;
          }
        }
        else if (4 == activeCfg)
        {
          if (META_HDR_MODE_DISABLE == g_config[0].advSettings.hdrMode &&
              META_HDR_MODE_DISABLE == g_config[1].advSettings.hdrMode &&
              META_HDR_MODE_DISABLE == g_config[2].advSettings.hdrMode &&
              META_HDR_MODE_DISABLE == g_config[3].advSettings.hdrMode) {
            multiCamMode = META_MULTICAM_MODE_QUAD_CAM_LINEAR_LINEAR;
          } else {
            res = ISP_RET_INVALID;
            APP_ERR("Unsupport multiply cam mode!", res);
          }
        }

        for (u32 i = 0; i < MAX_CAMERA; i++)
        {
            if (g_config[i].status != APP_CFG_STATUS_UNINITIALIZED)
            {
                res = META_SET(g_config[i].pMetadata,
                               METADATA_GLOBAL_ACTIVE_CAM_NUM,
                               activeCfg);

                res = META_SET(g_config[i].pMetadata,
                               METADATA_GLOBAL_MULTICAM_MODE,
                               multiCamMode);

                res = META_SET(g_config[i].pMetadata,
                               METADATA_GLOBAL_AAA_COMBINE,
                               aaaCombine);
                res = META_SET(g_config[i].pMetadata,
                               METADATA_USER_DUALCAM_ONLINE,
                               cam_online[i]);

                if (thiz->advSettings.ifeTune.field.m_rtpipe) {
                  thiz->pMetadata->global.ifeTune.field.m_rtpipe =
                    thiz->advSettings.ifeTune.field.m_rtpipe;
                  for (u32 j = 0; j < META_IFE_PIPE_NUM_MAX; ++j) {
                    g_config[i].pMetadata->global.ifeTune.rtpipe[j].try_online =
                      ifeTune.rtpipe[j].try_online;

                    g_config[i].pMetadata->global.ifeTune.rtpipe[j].fbc_en =
                      ifeTune.rtpipe[j].fbc_en;
                    g_config[i].pMetadata->global.ifeTune.rtpipe[j].outstand =
                      ifeTune.rtpipe[j].outstand;
                  }
                }
            }
        }
    }

    return res;
}

/// Parse command string to cmd_t struct
cmd_t* App_Config_Parse_Cmdline(char8* sCmdLine, u32 len)
{
    isp_ret_t       res     = ISP_RET_SUCCESS;
    s32             cmdId   = -1;
    cmd_t*          pCmd    = NULL;

    if (NULL == sCmdLine)
    {
        res = ISP_RET_INVALID;
        APP_ERR("Error! res = %d", res);
    }

    if (len == 0)
    {
        /// len will be 0 if all commands were read from config file.
        res = ISP_RET_INGORED_FAILURE;
    }
    else if (sCmdLine[0] == '#')
    {
        /// comment line
        res = ISP_RET_INGORED_FAILURE;
    }
    else
    {
        if (ISP_RET_SUCCESS == res)
        {
            char8 sTemp[APP_CONFIG_CMD_MAX_LEN] = { 0 };
            BOOL  foundCmd = FALSE;

            if (len == strlen(sCmdLine))
            {
                /// Handle case of string without end mask
                len++;
            }

            //APP_DBG("len %u strlen %u", len, strlen(sCmdLine));

            for (u32 i = 0; i < len; i++)
            {
                if (i <= (APP_CONFIG_CMD_MAX_LEN - 1) &&
                    FALSE == foundCmd)
                {
                    /// find the first ':' ,'\r' , '\n', ' ', '\0'
                    if (sCmdLine[i] == ':'  ||
                        sCmdLine[i] == '\r' ||
                        sCmdLine[i] == '\n' ||
                        sCmdLine[i] == ' '  ||
                        sCmdLine[i] == '\0')
                    {
                        foundCmd = TRUE;
                        memcpy(sTemp, sCmdLine, i);
                        //APP_DBG("i = %u, sTemp: %s", i, sTemp);
                    }
                }

                /// find the last '\r', '\n'
                if (sCmdLine[i] == '\r' || sCmdLine[i] == '\n')
                {
                    sCmdLine[i] = '\0';
                }

            }

            for (u32 i = 0; i < CMD_MAX; i++)
            {
                if (!strcmp(sTemp, g_cmdToken[i]))
                {
                    cmdId = i;
                    break;
                }
            }

            if (cmdId < 0)
            {
                res = ISP_RET_INVALID;
            }
        }

        if (ISP_RET_SUCCESS == res)
        {
            pCmd = MEM_OS_MALLOC(sizeof(cmd_t));

            if (pCmd)
            {
                pCmd->cmdId     = (cmd_id_t)cmdId;

                if (len > strlen(g_cmdToken[cmdId]) + 2)
                {
                    strncpy(pCmd->sParam, sCmdLine + strlen(g_cmdToken[cmdId]) + 1, sizeof(pCmd->sParam)-1);
                    pCmd->paramLen  = strlen(pCmd->sParam) + 1;
                }
                else
                {
                    pCmd->paramLen    = 0;
                }
            }

        }
    }

    return pCmd;
}

/// Parse camer ID
static u32 App_Config_Parse_Camid(cmd_t* pCmd)
{
    isp_ret_t            res      = ISP_RET_SUCCESS;
    u32                  camId    = MAX_CAMERA;
    char8*               sParam   = NULL;
    char8*               pParam   = NULL;
    s32                  paramId  = -1;
    char8*               sValue   = NULL;

    if (NULL == pCmd->sParam)
    {
        res = ISP_RET_INVALID;
        APP_ERR("Error! res = %d", res);
    }

    if (ISP_RET_SUCCESS == res)
    {
        size_t strSize = strlen(pCmd->sParam) + 1;

        sParam = MEM_OS_MALLOC(strSize);

        if (NULL == sParam)
        {
            res = ISP_RET_NOMEM;
            APP_ERR("Error! res = %d", res);
        }
        else
        {
            memcpy(sParam, pCmd->sParam, strSize);
            pParam = sParam;
        }
    }

    if (ISP_RET_SUCCESS == res)
    {
        /// define camid token
        static char8* const g_paramCamIdToken[2] =
        {
            [0] = (char8* const)"camid",
            [1] = NULL,
        };

        paramId = getsubopt(&pParam, g_paramCamIdToken, &sValue);
        if (paramId == 0)
        {
            camId = atoi(sValue);

            if (camId >= MAX_CAMERA)
            {
                res = ISP_RET_INVALID;
                APP_ERR("invalid camid = %u", camId);
            }
        }
        else
        {
            res = ISP_RET_INVALID;
            APP_ERR("Please set camid first");
        }
    }

    if (sParam)
    {
        MEM_OS_FREE(&sParam);
    }

    return camId;
}

/// Parse set command from cmd_t
static isp_ret_t App_Config_Parse_Cmd_Set(cmd_t* pCmd, app_config_t** ppConfig)
{
    isp_ret_t            res      = ISP_RET_SUCCESS;
    s32                  paramId  = -1;
    char8*               pParam   = pCmd->sParam;
    char8*               sValue   = NULL;
    app_config_t*        pConfig  = &g_config[0];

    if (CMD_SET != pCmd->cmdId)
    {
        res = ISP_RET_INVALID;
        APP_ERR("Error! res = %d", res);
    }

    if (ISP_RET_SUCCESS == res)
    {
        u32 camId = App_Config_Parse_Camid(pCmd);

        if (camId >= MAX_CAMERA)
        {
            res = ISP_RET_INVALID;
            APP_ERR("Error! res = %d", res);
        }
        else
        {
            pConfig = &g_config[camId];

            if (APP_CFG_STATUS_UNINITIALIZED == pConfig->status)
            {
                pConfig->status                                                             = APP_CFG_STATUS_UNINITIALIZED;
                pConfig->camId                                                              = camId;

                pConfig->streamCfg.fps                                                      = 0.0; /// auto fps
                pConfig->streamCfg.inputStreamNum                                           = 0;
                pConfig->streamCfg.outputStreamNum                                          = 1;
                pConfig->streamCfg.outputStreamInfo[BUF_STREAM_IDX_PREVIEW].user.width      = 1920;
                pConfig->streamCfg.outputStreamInfo[BUF_STREAM_IDX_PREVIEW].user.height     = 1080;

                pConfig->advSettings.sw3a.aecMode                                           = META_AEC_MODE_AUTO;
                pConfig->advSettings.sw3a.awbMode                                           = META_AWB_MODE_AUTO;
                pConfig->advSettings.sw3a.afMode                                            = META_AF_MODE_OFF;

                pConfig->advSettings.algbind.bind[ALG_LIB_AEC].enable = TRUE;
                pConfig->advSettings.algbind.bind[ALG_LIB_AWB].enable = TRUE;
                pConfig->advSettings.algbind.bind[ALG_LIB_AFC].enable = TRUE;
                strncpy(pConfig->advSettings.algbind.bind[ALG_LIB_AEC].attr.name, "algo.sw3a.default", ALG_LIB_NAME_SIZE_MAX);
                strncpy(pConfig->advSettings.algbind.bind[ALG_LIB_AWB].attr.name, "algo.sw3a.default", ALG_LIB_NAME_SIZE_MAX);
                strncpy(pConfig->advSettings.algbind.bind[ALG_LIB_AFC].attr.name, "algo.sw3a.default", ALG_LIB_NAME_SIZE_MAX);
                pConfig->advSettings.aaaCombine                                            = META_AAA_COMB_TYPE_IVL;
                pConfig->advSettings.camSession                                            = META_CAM_SESSION_VISIBLE;
                pConfig->advSettings.camOnline                                             = FALSE;
                pConfig->advSettings.dynamicFps                                          = FALSE;

                for (u32 i = 0; i < META_USR_PREF_SCV_NUM; ++i) {
                  pConfig->advSettings.preference.scv_rows[i].strength  = 128;
                }

                pConfig->advSettings.usr_exp.again_range.min = 1024;
                pConfig->advSettings.usr_exp.again_range.max = 10240000;
                pConfig->advSettings.usr_exp.dgain_range.min = 1024;
                pConfig->advSettings.usr_exp.dgain_range.max = 10240000;
                pConfig->advSettings.usr_exp.ispgain_range.min = 1024;
                pConfig->advSettings.usr_exp.ispgain_range.max = 10240000;
                pConfig->advSettings.usr_exp.sysgain_range.min = 1024;
                pConfig->advSettings.usr_exp.sysgain_range.max = 10240000;
                pConfig->advSettings.usr_exp.exptime_range.min = 1;
                pConfig->advSettings.usr_exp.exptime_range.max = 1000000;

                pConfig->advSettings.usr_fps.min = 100;
                pConfig->advSettings.usr_fps.max = 1000000;

                memset(&pConfig->event, 0, sizeof(app_config_event_t));

                if (NULL == pConfig->pMetadata)
                {
                    pConfig->pMetadata = Metadata_Create();
                    if (NULL == pConfig->pMetadata)
                    {
                        res = ISP_RET_NOMEM;
                        APP_ERR("Error! res = %d", res);
                        pConfig = NULL;
                    }
                }
            }
			
			if(pConfig)
	            pConfig->needRestart = FALSE;
        }
    }

    if (ISP_RET_SUCCESS == res)
    {
        while ('\0' != *pParam)
        {
            paramId = getsubopt(&pParam, g_paramSetToken, &sValue);
            switch (paramId)
            {
                case PARAM_SET_CAMID:
                {
                    break;
                }
                case PARAM_SET_FPS:
                {
                    f32 fps = atof(sValue);
                    if (fps <= MAX_FPS)
                    {
                        pConfig->streamCfg.fps = fps;
                    }
                    else
                    {
                        res = ISP_RET_INVALID;
                        APP_ERR("Error! res = %d", res);
                    }

                    break;
                }
                case PARAM_SET_PREVIEW_WIDTH:
                {
                    u32 width    = atoi(sValue);
                    u32 streamId = BUF_STREAM_IDX_PREVIEW;

                    if (APP_CFG_STATUS_UNINITIALIZED != pConfig->status &&
                        pConfig->streamCfg.outputStreamInfo[streamId].user.width != width)
                    {
                        pConfig->needRestart = TRUE;
                    }

                    pConfig->streamCfg.outputStreamInfo[streamId].user.width = width;

                    break;
                }
                case PARAM_SET_PREVIEW_HEIGHT:
                {
                    u32 height   = atoi(sValue);
                    u32 streamId = BUF_STREAM_IDX_PREVIEW;

                    if (APP_CFG_STATUS_UNINITIALIZED != pConfig->status &&
                        pConfig->streamCfg.outputStreamInfo[streamId].user.height != height)
                    {
                        pConfig->needRestart = TRUE;
                    }

                    pConfig->streamCfg.outputStreamInfo[streamId].user.height = height;

                    break;
                }

                case PARAM_SET_VIDEO_WIDTH:
                {
                    u32 width    = atoi(sValue);
                    u32 streamId = BUF_STREAM_IDX_VIDEO;

                    if (APP_CFG_STATUS_UNINITIALIZED != pConfig->status &&
                        pConfig->streamCfg.outputStreamInfo[streamId].user.width != width)
                    {
                        pConfig->needRestart = TRUE;
                    }

                    pConfig->streamCfg.outputStreamInfo[streamId].user.width = width;

                    pConfig->streamCfg.outputStreamNum = 2;

                    break;
                }
                case PARAM_SET_VIDEO_HEIGHT:
                {
                    u32 height   = atoi(sValue);
                    u32 streamId = BUF_STREAM_IDX_VIDEO;

                    if (APP_CFG_STATUS_UNINITIALIZED != pConfig->status &&
                        pConfig->streamCfg.outputStreamInfo[streamId].user.height != height)
                    {
                        pConfig->needRestart = TRUE;
                    }

                    pConfig->streamCfg.outputStreamInfo[streamId].user.height = height;

                    pConfig->streamCfg.outputStreamNum = 2;

                    break;
                }
                case PARAM_SET_HDR:
                {
                    u32 hdrMode = atoi(sValue);

                    if (hdrMode >= META_HDR_MAX_MODE)
                    {
                        res = ISP_RET_INVALID;
                        APP_ERR("Error! res = %d", res);
                        pConfig = NULL;
                    }

                    if (ISP_RET_SUCCESS == res)
                    {
                        if (APP_CFG_STATUS_UNINITIALIZED != pConfig->status &&
                            pConfig->advSettings.hdrMode != hdrMode)
                        {
                            pConfig->needRestart = TRUE;
                        }

                        pConfig->advSettings.hdrMode = hdrMode;
                    }

                    break;
                }
                case PARAM_SET_VIDEO_FMT:
                {
                    u32 streamId = BUF_STREAM_IDX_VIDEO;
                    u32 listSize = STREAM_FMT_MAX;

                    for (u32 fmtId = 0; fmtId < listSize; fmtId++)
                    {
                        if (g_fmtToken[fmtId] != NULL && !strcmp(sValue, g_fmtToken[fmtId]))
                        {
                            if (APP_CFG_STATUS_UNINITIALIZED != pConfig->status &&
                                pConfig->streamCfg.outputStreamInfo[streamId].user.fmt != ((stream_fmt_t)fmtId))
                            {
                                pConfig->needRestart = TRUE;
                            }
                            pConfig->streamCfg.outputStreamInfo[streamId].user.fmt = (stream_fmt_t)fmtId;

                            break;
                        }
                    }

                    break;
                }
                case PARAM_SET_AEC_MODE:
                {
                    u32 mode   = atoi(sValue);

                    pConfig->advSettings.sw3a.aecMode = mode;

                    break;
                }
                case PARAM_SET_AWB_MODE:
                {
                    u32 mode   = atoi(sValue);

                    pConfig->advSettings.sw3a.awbMode = mode;

                    break;
                }
                case PARAM_SET_AF_MODE:
                {
                    u32 mode   = atoi(sValue);

                    pConfig->advSettings.sw3a.afMode = mode;

                    break;
                }
                case PARAM_SET_SEN_TEST_MODE:
                {
                    u32 mode   = atoi(sValue);

                    pConfig->advSettings.senTestMode = mode;

                    break;
                }
                case PARAM_SET_FORCE_RESTART:
                {
                    u32 restart   = atoi(sValue);

                    pConfig->needRestart = (restart > 0 ? TRUE : FALSE);

                    break;
                }
                case PARAM_SET_MAX_SEN_GAIN:
                {
                    u32 maxSenGain = atoi(sValue);
                    pConfig->advSettings.maxSenGain = maxSenGain;
                    break;
                }

                case PARAM_SET_AAA_COMBINE:
                {
                    u32 combine = atoi(sValue);
                    pConfig->advSettings.aaaCombine = combine;
                    break;
                }

                case PARAM_SET_CAM_SESSION:
                {
                    u32 session = atoi(sValue);
                    pConfig->advSettings.camSession = session;
                    break;
                }
                case PARAM_SET_RTP_ONLINE:
                {
                    u32 online = atoi(sValue);
                    /* if (online) { */
                      pConfig->advSettings.ifeTune.field.m_rtpipe = TRUE;
                      ifeTune.rtpipe[pConfig->camId].try_online = online;
                    /* } */
                    ifeTune.rtpipe[pConfig->camId].fbc_en = 1;
                    ifeTune.rtpipe[pConfig->camId].outstand = 6;
                    break;
                }
                case PARAM_SET_CAM_ONLINE:
                {
                    u32 online = (atoi(sValue))&0x1;
                    pConfig->advSettings.camOnline = (BOOL)online;
                    break;
                }
                case PARAM_SET_DUALCAM_DYFPS:
                {
                    u32 dyfps = (atoi(sValue))&0x1;
                    pConfig->advSettings.dynamicFps = (BOOL)dyfps;
                    break;
                }

                default:
                {
                    break;
                }
            }
        }

        for (u32 i = 0; i < pConfig->streamCfg.outputStreamNum; i++)
        {
            pConfig->streamCfg.outputStreamInfo[i].user.identity.camId    = pConfig->camId;
            pConfig->streamCfg.outputStreamInfo[i].user.identity.streamId = i;

            if (i == BUF_STREAM_IDX_PREVIEW)
            {
                pConfig->streamCfg.outputStreamInfo[i].user.fmt        = STREAM_FMT_RGB888;
                pConfig->streamCfg.outputStreamInfo[i].user.maxBufNum  = 0;
                pConfig->streamCfg.outputStreamInfo[i].user.allocType  = STREAM_ALLOC_NONE;
                pConfig->streamCfg.outputStreamInfo[i].user.usage      = STREAM_USAGE_PREVIEW;
            }
            else
            {
                pConfig->streamCfg.outputStreamInfo[i].user.maxBufNum  = MAX_BUF_PER_STREAM;
                pConfig->streamCfg.outputStreamInfo[i].user.allocType  = STREAM_ALLOC_CONTINUE_BUF;
                pConfig->streamCfg.outputStreamInfo[i].user.usage      = STREAM_USAGE_VIDEO;
            }
        }
    }

    if (ISP_RET_SUCCESS == res)
    {
        if (APP_CFG_STATUS_UNINITIALIZED == pConfig->status)
            pConfig->status      = APP_CFG_STATUS_INITIALIZED;
		else
        	pConfig->status      = APP_CFG_STATUS_UPDATED;

        res = App_Config_Update_Metadata(pConfig);
        if (ISP_RET_SUCCESS != res)
        {
            APP_ERR("Error! res = %d", res);
        }
    }

    if (ISP_RET_SUCCESS == res)
    {
        *ppConfig = pConfig;
    }
    else
    {
        *ppConfig = NULL;
    }

    return res;
}

/// App_Config_Parse_Cmd_Dump
static isp_ret_t App_Config_Parse_Cmd_Dump(cmd_t* pCmd, app_config_t** ppConfig)
{
    isp_ret_t                res                = ISP_RET_SUCCESS;
    s32                      paramId            = -1;
    char8*                   pParam             = pCmd->sParam;
    char8*                   sValue             = NULL;
    app_config_t*            pConfig            = NULL;
    app_config_dump_t*       pDumpSettings      = NULL;
    u32                      i                  = 0;

    if (CMD_DUMP != pCmd->cmdId)
    {
        res = ISP_RET_INVALID;
        APP_ERR("Error! res = %d", res);
    }

    if (ISP_RET_SUCCESS == res)
    {

        u32 camId = App_Config_Parse_Camid(pCmd);

        if (camId >= MAX_CAMERA)
        {
            res = ISP_RET_INVALID;
            APP_ERR("invalid camera id = %u", camId);
        }
        else
        {
            pConfig         = &g_config[camId];
            pDumpSettings   = &g_config[camId].dumpSettings;

            if (APP_CFG_STATUS_UNINITIALIZED == pConfig->status)
            {
                res = ISP_RET_UNINITIALIZED;
                APP_ERR("Error! res = %d", res);
                pConfig = NULL;
            }

            memset(pDumpSettings, 0, sizeof(app_config_dump_t));
        }
    }

    if (ISP_RET_SUCCESS == res)
    {
        while ('\0' != *pParam)
        {
            paramId = getsubopt(&pParam, g_paramDumpToken, &sValue);

            switch (paramId)
            {
                case PARAM_DUMP_CAMID:
                {
                    break;
                }
                case PARAM_DUMP_ANALYSIS:
                {
                    sscanf(sValue, FMT_X64, &pDumpSettings->dumpAnalysis);

                    APP_CFG("dumpAnalysis 0x%llx", pDumpSettings->dumpAnalysis);

                    break;
                }
                case PARAM_DUMP_NUM:
                {
                    u32 value = atoi(sValue);
                    for (i = 0; i < BUF_STREAM_IDX_MAX; i++)
                    {
                        pDumpSettings->num[i] = value;
                    }

                    break;
                }
                case PARAM_DUMP_PERIOD:
                {
                    u32 value = atoi(sValue);
                    for (i = 0; i < BUF_STREAM_IDX_MAX; i++)
                    {
                        pDumpSettings->period[i] = value;
                    }

                    break;
                }
                default:
                {
                    break;
                }
            }
        }
    }

    if (ISP_RET_SUCCESS == res)
    {
        *ppConfig = pConfig;
    }
    else
    {
        *ppConfig = NULL;
    }

    return res;
}

/// Parse sleep command from cmd_t
static isp_ret_t App_Config_Parse_Cmd_Sleep(cmd_t* pCmd, app_config_t** ppConfig)
{
    isp_ret_t                          res       = ISP_RET_SUCCESS;
    s32                                paramId   = -1;
    char8*                             pParam    = pCmd->sParam;
    char8*                             sValue    = NULL;
    app_config_t*                      pConfig   = &g_config[0];

    if (CMD_SLEEP != pCmd->cmdId)
    {
        res = ISP_RET_INVALID;
        APP_ERR("Error! res = %d", res);
    }

    if (ISP_RET_SUCCESS == res)
    {
        while ('\0' != *pParam)
        {
            paramId = getsubopt(&pParam, g_paramSleeppToken, &sValue);
            switch (paramId)
            {
                case PARAM_SLEEP_TIME:
                {
                    pConfig->sleepSecond = atoi(sValue);
                    break;
                }
                default:
                {
                    break;
                }
            }
        }
    }

    if (ISP_RET_SUCCESS == res)
    {
        *ppConfig = pConfig;
    }
    else
    {
        *ppConfig = NULL;
    }

    return res;
}

/// Parse log command from cmd_t
static isp_ret_t App_Config_Parse_Cmd_Log(cmd_t* pCmd, app_config_t** ppConfig)
{
    isp_ret_t                          res       = ISP_RET_SUCCESS;
    s32                                paramId   = -1;
    char8*                             pParam    = pCmd->sParam;
    char8*                             sValue    = NULL;
    app_config_t*                      pConfig   = &g_config[0];
    app_config_log_t*                  pLogCfg   = &pConfig->logCfg;

    if (CMD_LOG != pCmd->cmdId)
    {
        res = ISP_RET_INVALID;
        APP_ERR("Error! res = %d", res);
        pLogCfg = NULL;
    }

    if (ISP_RET_SUCCESS == res)
    {
        memset(pLogCfg->logPath,   0, sizeof(pLogCfg->logPath));
        memset(pLogCfg->tracePath, 0, sizeof(pLogCfg->tracePath));
        pLogCfg->logPathValid    = TRUE;
        pLogCfg->tracePathValid  = TRUE;

        pLogCfg->levelMaskValid  = FALSE;
        pLogCfg->ispMaskValid    = FALSE;
        pLogCfg->sw3aMaskValid   = FALSE;
        pLogCfg->simpleModeValid = FALSE;
        pLogCfg->showFpsValid    = FALSE;
        pLogCfg->baudRateValid   = FALSE;

        while ('\0' != *pParam)
        {
            paramId = getsubopt(&pParam, g_paramLogToken, &sValue);
            switch (paramId)
            {
                case PARAM_LOG_LEVEL_MASK:
                {
                    pLogCfg->levelMaskValid = TRUE;

                    sscanf(sValue, FMT_X64, &pLogCfg->levelMask);

                    APP_CFG("levelMask: "FMT_X64, pLogCfg->levelMask);

                    break;
                }
                case PARAM_LOG_ISP_MASK:
                {
                    pLogCfg->ispMaskValid    = TRUE;

                    sscanf(sValue, FMT_X64, &pLogCfg->ispMask);

                    APP_CFG("ispMask: "FMT_X64, pLogCfg->ispMask);

                    break;
                }
                case PARAM_LOG_3A_MASK:
                {
                    pLogCfg->sw3aMaskValid   = TRUE;

                    sscanf(sValue, FMT_X64, &pLogCfg->sw3aMask);

                    APP_CFG("sw3aMask: "FMT_X64, pLogCfg->sw3aMask);

                    break;
                }
                case PARAM_LOG_SIMPLE_MODE:
                {
                    pLogCfg->simpleModeValid = TRUE;

                    u32 mode = 0;

                    sscanf(sValue, "%x", &mode);

                    pLogCfg->simpleMode = (BOOL) mode;

                    APP_CFG("simpleMode: %u", pLogCfg->simpleMode);

                    break;
                }
                case PARAM_LOG_PATH:
                {
                    pLogCfg->logPathValid    = TRUE;

                    memcpy(pLogCfg->logPath, sValue, (strlen(sValue) + 1));

                    APP_CFG("logPath: %s", pLogCfg->logPath);

                    break;
                }
                case PARAM_LOG_TRACE_PATH:
                {
                    pLogCfg->tracePathValid  = TRUE;

                    memcpy(pLogCfg->tracePath, sValue, (strlen(sValue) + 1));

                    APP_CFG("tracePath: %s", pLogCfg->tracePath);

                    break;
                }
                case PARAM_LOG_SHOW_FPS:
                {
                    pLogCfg->showFpsValid    = TRUE;

                    pLogCfg->showFps = atoi(sValue);

                    APP_CFG("showFps: %u", pLogCfg->showFps);

                    break;
                }
                case PARAM_LOG_UART_BAUDRATE:
                {
                    pLogCfg->baudRateValid   = TRUE;

                    pLogCfg->baudRate = atoi(sValue);

                    APP_CFG("baudRate: %u", pLogCfg->baudRate);

                    break;
                }
                default:
                {
                    break;
                }
            }
        }
    }

    if (ISP_RET_SUCCESS == res)
    {
        *ppConfig = pConfig;
    }
    else
    {
        *ppConfig = NULL;
    }

    return res;
}

/// Parse evt command from cmd_t
static isp_ret_t App_Config_Parse_Cmd_Evt(cmd_t* pCmd, app_config_t** ppConfig)
{
    isp_ret_t                res                = ISP_RET_SUCCESS;
    s32                      paramId            = -1;
    char8*                   pParam             = pCmd->sParam;
    char8*                   sValue             = NULL;
    app_config_t*            pConfig            = NULL;
    app_config_event_t*      pEvt               = NULL;

    if (CMD_EVT != pCmd->cmdId)
    {
        res = ISP_RET_INVALID;
        APP_ERR("Error! res = %d", res);
    }

    if (ISP_RET_SUCCESS == res)
    {
        u32 camId = App_Config_Parse_Camid(pCmd);

        if (camId >= MAX_CAMERA)
        {
            res = ISP_RET_INVALID;
            APP_ERR("Error! res = %d", res);
        }
        else
        {
            pConfig = &g_config[camId];

            if (APP_CFG_STATUS_UNINITIALIZED == pConfig->status)
            {
                res = ISP_RET_UNINITIALIZED;
                APP_ERR("Error! res = %d", res);
                pConfig = NULL;
            }

            if (ISP_RET_SUCCESS == res)
            {
                pEvt    = &g_config[camId].event;
                memset(pEvt, 0, sizeof(app_config_event_t));

                pEvt->status = META_EVT_STATUS_INVALID;
                pEvt->camId  = camId;
            }
        }
    }

    if (ISP_RET_SUCCESS == res)
    {
        while ('\0' != *pParam)
        {
            paramId = getsubopt(&pParam, g_paramEvtToken, &sValue);

            switch (paramId)
            {
                case PARAM_EVT_CAMID:
                {
                    break;
                }
                case PARAM_EVT_TYPE:
                {
                    u32 value = atoi(sValue);

                    pEvt->type = value;

                    break;
                }
                case PARAM_EVT_ID:
                {
                    u32 value = atoi(sValue);

                    pEvt->evtId = value;

                    break;
                }
                case PARAM_EVT_DIRECTION:
                {
                    u32 value = atoi(sValue);

                    pEvt->direction = (u8)value;

                    break;
                }
                case PARAM_EVT_DATA:
                {
                    char8 *sValueEnd = NULL;

                    u32 idata = strtol(sValue, &sValueEnd, 0);
                    if ((sValueEnd - sValue) && ('\0' == *sValueEnd)) {
                      pEvt->data = idata;
                      break;
                    }

                    f32 fdata = strtod(sValue, &sValueEnd);
                    if ((sValueEnd - sValue) && ('\0' == *sValueEnd)) {
                      memcpy(&pEvt->data, &fdata, sizeof(u32));
                      break;
                    }

                    break;
                }
                default:
                {
                    break;
                }
            }
        }
    }

    if (ISP_RET_SUCCESS == res)
    {
        pEvt->needRestart = FALSE;
        pEvt->status      = META_EVT_STATUS_PROCESSING;

        tapi_bind_event_buffer(pEvt);

        res = App_Config_Update_Metadata(pConfig);
        if (ISP_RET_SUCCESS != res)
        {
            APP_ERR("Error! res = %d", res);
        }
        else
        {
            APP_DBG("camId = %u, type = %u, evtId = %u, evtDirection = %u, evtData = %u",
                pEvt->camId, pEvt->type, pEvt->evtId, pEvt->direction, pEvt->data);
        }

        memset(pEvt, 0, sizeof(app_config_event_t));
    }

    if (ISP_RET_SUCCESS == res)
    {
        *ppConfig = pConfig;
    }
    else
    {
        *ppConfig = NULL;
    }

    return res;
}

#ifdef ARMCB_STEST_ENABLE
/// Parse set command from cmd_t
static isp_ret_t App_Config_Parse_Sanity_Test(cmd_t* pCmd, app_config_t** ppConfig)
{
    isp_ret_t                res      = ISP_RET_SUCCESS;
    s32                      paramId  = -1;
    char8*                   pParam   = pCmd->sParam;
    char8*                   sValue   = NULL;
    app_config_t*            pConfig  = &g_config[0];
    sanity_test_params_t*    pStest   = NULL;

    if (CMD_STEST != pCmd->cmdId)
    {
        res = ISP_RET_INVALID;
        APP_ERR("Error! res = %d", res);
    }

    if (ISP_RET_SUCCESS == res)
    {
        u32 camId = App_Config_Parse_Camid(pCmd);

        if (camId >= MAX_CAMERA)
        {
            res = ISP_RET_INVALID;
            APP_ERR("Error! res = %d", res);
        }
        else
        {
            pConfig = &g_config[camId];

            if (APP_CFG_STATUS_UNINITIALIZED == pConfig->status)
            {
                res = ISP_RET_UNINITIALIZED;
                APP_ERR("Error! res = %d", res);
                pConfig = NULL;
            }

            if (ISP_RET_SUCCESS == res)
            {
                pStest    = &g_config[camId].stestCfg;
                memset(pStest, 0, sizeof(sanity_test_params_t));

                pStest->sTCaseId = STEST_CASE_INVALID;
                pStest->sCamId   = camId;
            }
        }
    }

    if (ISP_RET_SUCCESS == res)
    {
        while ('\0' != *pParam)
        {
            paramId = getsubopt(&pParam, g_paramStestToken, &sValue);
            switch (paramId)
            {
                case PARAM_STEST_CAMID:
                {
                    u32 camId = atoi(sValue);
                    if (camId < MAX_CAMERA)
                    {
                        pStest->sCamId = camId;
                    }
                    break;
                }
                case PARAM_STEST_CASE:
                {
                    u32 caseId = atoi(sValue);
                    if (caseId < STEST_CASE_NUM_MAX)
                    {
                        pStest->sTCaseId = caseId;
                    }
                    break;
                }

                default:
                {
                    break;
                }
            }
        }
    }

    if (ISP_RET_SUCCESS == res)
    {
        *ppConfig = pConfig;
    }
    else
    {
        *ppConfig = NULL;
    }

    return res;
}
#endif

/// Create config from cmd
isp_ret_t App_Config_Create_By_Cmd(cmd_t* pCmd, app_config_t** ppConfig)
{
    isp_ret_t  res = ISP_RET_SUCCESS;

    if (NULL == pCmd ||
        NULL == ppConfig)
    {
        res = ISP_RET_INVALID;
        APP_ERR("Error! res = %d", res);
    }

    if (ISP_RET_SUCCESS == res)
    {
        switch(pCmd->cmdId)
        {
            case CMD_SET:
            {
                res = App_Config_Parse_Cmd_Set(pCmd, ppConfig);

                break;
            }
            case CMD_DUMP:
            {
                res = App_Config_Parse_Cmd_Dump(pCmd, ppConfig);

                break;
            }
            case CMD_EVT:
            {
                res = App_Config_Parse_Cmd_Evt(pCmd, ppConfig);

                break;
            }
            case CMD_SLEEP:
            {
                res = App_Config_Parse_Cmd_Sleep(pCmd, ppConfig);

                break;
            }
            case CMD_LOG:
            {
                res = App_Config_Parse_Cmd_Log(pCmd, ppConfig);

                break;
            }
       #ifdef ARMCB_STEST_ENABLE
            case CMD_STEST:
            {
                res = App_Config_Parse_Sanity_Test(pCmd, ppConfig);

                break;
            }
       #endif
            case CMD_START:
            case CMD_STOP:
            case CMD_EXIT:
            case CMD_NOP:
            default:
            {
                *ppConfig = NULL;
                break;
            }
        }

        MEM_OS_FREE(&pCmd);
    }

    return res;
}

/// Destroy config
isp_ret_t App_Config_Destroy(app_config_t** ppConfig)
{
    isp_ret_t  res = ISP_RET_SUCCESS;

    if (NULL == ppConfig ||
        NULL == *ppConfig)
    {
        res = ISP_RET_INVALID;
        APP_ERR("Error! res = %d", res);
    }

    if (ISP_RET_SUCCESS == res)
    {
        app_config_t *pConfig = *ppConfig;

        pConfig->status = APP_CFG_STATUS_UNINITIALIZED;
        if (pConfig->pMetadata)
        {
            Metadata_Destroy(&pConfig->pMetadata);
        }

        memset(pConfig, 0, sizeof(app_config_t));

        *ppConfig = NULL;
    }

    return res;
}

app_config_t *App_Config_Acquire(int camId)
{
	return &g_config[camId];
}


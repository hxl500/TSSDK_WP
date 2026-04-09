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

#ifndef __APP_CONFIG_H__
#define __APP_CONFIG_H__

#define _GNU_SOURCE
#include <stdlib.h>
#include <string.h>

#include "types_utils.h"
#include "log_utils.h"
#include "hal_types.h"
#ifdef ARMCB_STEST_ENABLE
#include "sanity_test.h"
#endif

#include "app_config_buffer.h"

#define APP_CONFIG_CMD_MAX_LEN      (12)
#define APP_CONFIG_CMDLINE_MAX_LEN  (256+6528)
#define APP_CONFIG_DUMP_ALL         (0xFFFFFFFFFFFFFFFF)

/// APP command ID
typedef enum __cmd_id
{
    CMD_SET        = 0,
    CMD_START,
    CMD_DUMP,
    CMD_SLEEP,
    CMD_LOG,
    CMD_EVT,
    CMD_STOP,
    CMD_EXIT,
#ifdef ARMCB_STEST_ENABLE
    CMD_STEST,
#endif
    CMD_NOP,
    CMD_MAX        = CMD_NOP,
} cmd_id_t;

/// APP set parameters ID
typedef enum __param_set_id
{
    PARAM_SET_CAMID    = 0,
    PARAM_SET_FPS,
    PARAM_SET_PREVIEW_WIDTH,
    PARAM_SET_PREVIEW_HEIGHT,
    PARAM_SET_VIDEO_WIDTH,
    PARAM_SET_VIDEO_HEIGHT,
    PARAM_SET_VIDEO_FMT,
    PARAM_SET_HDR,
    PARAM_SET_AEC_MODE,
    PARAM_SET_AWB_MODE,
    PARAM_SET_AF_MODE,
    PARAM_SET_SEN_TEST_MODE,
    PARAM_SET_FORCE_RESTART,
    PARAM_SET_MAX_SEN_GAIN,

    PARAM_SET_ALG_REG,
    PARAM_SET_ALG_BIND,
    PARAM_SET_AAA_COMBINE,
    PARAM_SET_CAM_SESSION,
    PARAM_SET_RTP_ONLINE,
    PARAM_SET_CAM_ONLINE,
    PARAM_SET_DUALCAM_DYFPS,
    PARAM_SET_MAX,
} param_set_id_t;

/// APP log parameters ID
typedef enum __param_log_id
{
    PARAM_LOG_LEVEL_MASK    = 0,
    PARAM_LOG_ISP_MASK,
    PARAM_LOG_3A_MASK,
    PARAM_LOG_SIMPLE_MODE,
    PARAM_LOG_PATH,
    PARAM_LOG_TRACE_PATH,
    PARAM_LOG_SHOW_FPS,
    PARAM_LOG_UART_BAUDRATE,
    PARAM_LOG_MAX,
} param_log_id_t;

/// APP dump parameters ID
typedef enum __param_dump_id
{
    PARAM_DUMP_CAMID = 0,
    PARAM_DUMP_ANALYSIS,
    PARAM_DUMP_NUM,
    PARAM_DUMP_PERIOD,
    PARAM_DUMP_MAX,
} param_dump_id_t;

/// APP sleep parameters ID
typedef enum __param_sleep_id
{
    PARAM_SLEEP_TIME  = 0,
    PARAM_SLEEP_MAX,
} param_sleep_id_t;

/// APP event parameters ID
typedef enum __param_event_id
{
    PARAM_EVT_CAMID    = 0,
    //PARAM_EVT_NODE,
    //PARAM_EVT_ELEMENT,
    PARAM_EVT_TYPE,
    PARAM_EVT_ID,
    PARAM_EVT_DIRECTION,
    PARAM_EVT_DATA,
    PARAM_EVT_BUFFER,
    PARAM_EVT_MAX,
} param_event_id_t;

#ifdef ARMCB_STEST_ENABLE
/// APP sanity test parameters ID
typedef enum __param_stest_id
{
    PARAM_STEST_CAMID    = 0,
    PARAM_STEST_CASE,

    PARAM_STEST_MAX,
} param_stest_id_t;
#endif

typedef enum __cmd_src_type
{
	CMD_SRC_MPI = 0,
	CMD_SRC_UART,
	CMD_SRC_ETH,
	CMD_SRC_USB,
} cmd_src_type_e;

/// define APP command struct
typedef struct __cmd
{
    cmd_id_t    cmdId;
    char8       sParam[APP_CONFIG_CMDLINE_MAX_LEN];
    u32         paramLen;
} cmd_t;

/// define APP stream params struct
typedef metadata_stream_config_t app_config_streams_t;

/// define APP event
typedef metadata_event_t app_config_event_t;

/// define APP 3A params struct
typedef struct __app_config_3A
{
    meta_aec_mode_t    aecMode;
    meta_awb_mode_t    awbMode;
    meta_af_mode_t     afMode;
    u32                gain;
    u32                expTime;
    u8                 antiBandingMode;
} app_config_3A_t;

/// define APP zoom params struct
typedef struct __app_config_zoom
{
    f32           zoom;
    coordinate_t  start;
    coordinate_t  end;
} app_config_zoom_t;

typedef meta_alg_reg_t app_config_alreg_t;
typedef meta_alg_bind_t app_config_albind_t;

typedef	struct __isp_user_exposure_attr_t
{
	range_t		again_range;
	range_t		dgain_range;
	range_t		ispgain_range;
	range_t		sysgain_range;
	range_t		exptime_range;
}isp_user_exposure_attr_t;

typedef	struct __isp_user_fps_t
{
	range_t	fps_range;
}isp_user_fps_t;

/// define APP advance params struct
typedef struct __app_config_advance
{
    app_config_3A_t     sw3a;
    app_config_zoom_t   zoom;
    meta_hdr_mode_t     hdrMode;
    BOOL                asdEnable;
    u32                 senTestMode;
    u32                 maxSenGain;

    app_config_alreg_t algreg;
    app_config_albind_t algbind;
    meta_aaa_comb_type_t aaaCombine;
    meta_camera_session_t camSession;
    meta_user_preference_t preference;
    BOOL                  camOnline;
    BOOL                  dynamicFps;

	usr_exposure_attr_t	usr_exp;
	range_t				usr_fps;	/// multiply with 100

    meta_ife_data_t       ifeTune;
} app_config_advance_t;

/// define APP log params struct
typedef struct __app_config_log
{
    BOOL    levelMaskValid;
    u64     levelMask;

    BOOL    ispMaskValid;
    u64     ispMask;

    BOOL    sw3aMaskValid;
    u64     sw3aMask;

    BOOL    simpleModeValid;
    BOOL    simpleMode;

    BOOL    logPathValid;
    char8   logPath[128];

    BOOL    tracePathValid;
    char8   tracePath[128];

    BOOL    showFpsValid;
    BOOL    showFps;

    BOOL    baudRateValid;
    u32     baudRate;
} app_config_log_t;

/// define APP dump params struct
typedef struct __app_config_dump
{
    u64          dumpAnalysis;

    u8           num[BUF_STREAM_IDX_MAX];
    u8           period[BUF_STREAM_IDX_MAX];
} app_config_dump_t;

/// APP status
typedef enum __app_config_status
{
    APP_CFG_STATUS_UNINITIALIZED = 0,
    APP_CFG_STATUS_INITIALIZED,
    APP_CFG_STATUS_UPDATED,
} app_config_status_t;

/// config struct to store all APP params
typedef struct __app_config
{
    app_config_status_t        status;
    u32                        camId;
    BOOL                       needRestart;
    app_config_streams_t       streamCfg;
    app_config_advance_t       advSettings;
    app_config_event_t         event;
    app_config_dump_t          dumpSettings;
    metadata_t*                pMetadata;

    /// log & sleep will be used with config[0]
    app_config_log_t           logCfg;
    u32                        sleepSecond;
#ifdef ARMCB_STEST_ENABLE
    sanity_test_params_t       stestCfg;
#endif
} app_config_t;

/*
* App_Config_Update_Capability_Metadata
*
* @brief Update APP internal metadata with device capability
*
* @param thiz: pointer to APP config
*
* @param pCapMetadata: pointer to ddevice capability metadata
*
* @return success or failure
*/
isp_ret_t App_Config_Update_Capability_Metadata(app_config_t* thiz, metadata_t* pCapMetadata);

/*
* App_Config_Parse_Cmdline
*
* @brief Parse command string to cmd_t struct
*
* @param sCmdLine: command string
*
* @param len: command string lemgth
*
* @return pointer to cmd_t
*/
cmd_t* App_Config_Parse_Cmdline(char8* sCmdLine, u32 len);

/*
* App_Config_Create_By_Cmd
*
* @brief Create config from cmd
*
* @param pCmd: pointer to config command struct
*
* @param ppConfig: dimensional pointer to app_config_t
*
* @return success or failure
*/
isp_ret_t App_Config_Create_By_Cmd(cmd_t* pCmd, app_config_t** ppConfig);

/*
* App_Config_Destroy
*
* @brief Destroy config
*
* @param ppConfig: dimensional pointer to app_config_t
*
* @return success or failure
*/
isp_ret_t App_Config_Destroy(app_config_t** ppConfig);

app_config_t *App_Config_Acquire(int camId);

isp_ret_t App_Config_Update_Metadata(app_config_t* thiz);

#endif

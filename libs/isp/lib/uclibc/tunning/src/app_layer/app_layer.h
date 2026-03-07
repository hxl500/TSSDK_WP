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
#ifndef __APP_LAYER_H__
#define __APP_LAYER_H__

#include <stdio.h>
#include <string.h>
#include "types_utils.h"
#include "log_utils.h"
#include "fs_osutils.h"
#include "trace_osutils.h"
#include "time_osutils.h"
#include "app_config.h"
#include "app_usecase.h"
#include "app_upper.h"
#include "niss.h"
#include "obj_queue_utils.h"
#include "thread_osutils.h"

#if 1
static const char8 g_usage[] =
{
"nothing\n"
};
#else
static const char8 g_usage[] =
{
"---------------------------------------------------------------------------------------------\n"
"Usage: isp_app  [options]                                                                    \n"
"---------------------------------------------------------------------------------------------\n"
"-h               print help usage                                                            \n"
"-g               enabled debug mode for signal monitor                                       \n"
"-f [filename]    load commands from file                                                     \n"
"---------------------------------------------------------------------------------------------\n"
"Commands List:                                                                               \n"
"---------------------------------------------------------------------------------------------\n"
"set:    (set camera parameters)                                                              \n"
"    @camid        camera index                                                               \n"
"                  - N (0 ~ MAX:15)                                                           \n"
"    @fps          frames per second for all streams                                          \n"
"                  - 0 (auto fps)                                                             \n"
"                  - 7.5                                                                      \n"
"                  - 15                                                                       \n"
"                  - 30                                                                       \n"
"                  - 60                                                                       \n"
"    @pWidth       preview width                                                              \n"
"                  - N (pixels)                                                               \n"
"    @pHeight      preview height                                                             \n"
"                  - N (pixels)                                                               \n"
"    @vWidth       video width                                                                \n"
"                  - N (pixels)                                                               \n"
"    @vHeight      video height                                                               \n"
"                  - N (pixels)                                                               \n"
"    @vFmt         video format                                                               \n"
"                  - NV12                                                                     \n"
"                  - NV21                                                                     \n"
"                  - RGB888                                                                   \n"
"                  - RAW10                                                                    \n"
"                  - RAW12                                                                    \n"
"    @hdr          HDR mode                                                                   \n"
"                  - 0 (Normal)                                                               \n"
"                  - 1 (DOL2)                                                                 \n"
"                  - 2 (DOL3)                                                                 \n"
"                  - 3 (Stitch3)                                                              \n"
"                  - 4 (Stitch2)                                                              \n"
"    @aecMode      AEC mode                                                                   \n"
"                  - 0 (off)                                                                  \n"
"                  - 1 (auto)                                                                 \n"
"    @awbMode      AWB mode                                                                   \n"
"                  - 0 (off)                                                                  \n"
"                  - 1 (auto)                                                                 \n"
"                  - 2 (MWB)                                                                  \n"
"    @afMode       AF mode                                                                    \n"
"                  - 0 (off)                                                                  \n"
"                  - 1 (continue)                                                             \n"
"                  - 2 (once)                                                                 \n"
"    @senTestMode  Sensor test mode for private use                                           \n"
"                  - 0 (off)                                                                  \n"
"                  - 1 (test sensor gain how many frames delay to take effect, check by log)  \n"
"                  - 2 (test sensor exposure how many frames delay                            \n"
"                       to take effect, check by log)                                         \n"
"                  - 3 (test sensor gain and exposure can the same frame take effect,         \n"
"                       no flicker is OK)                                                     \n"
"                  - 4 (test sensor gain pre-isp gain and exposure can the same               \n"
"                       frame take effect,no flicker is ok)                                   \n"
"                  - 5 (test DOL2 HDR ratio effect time, no flicker is ok)                    \n"
"                  - 6 (test gain linear, gain start from sensor gain init,                   \n"
"                       increase by gain step)                                                \n"
"                  - 7 (test exposure time linear, exp line start from exposure count         \n"
"                       init value and increase by exp step)                                  \n"
"    @maxSenGain   configure maximum sensor gain                                              \n"
"                  - N (maximum sensor gain)                                                  \n"
"[e.g.]                                                                                       \n"
"set:camid=0,fps=30,pWidth=1920,pHeight=1080                                                  \n"
"set:camid=0,fps=15,pWidth0=1920,pHeight=1080,vWidth=1920,vHeight=1080,vFmt=NV12              \n"
"---------------------------------------------------------------------------------------------\n"
"start:    (start all camera)                                                                 \n"
"[e.g.]                                                                                       \n"
"start                                                                                        \n"
"---------------------------------------------------------------------------------------------\n"
"log:    (configure logging settings)                                                         \n"
"    @levelMask   logging level mask (Every 4 bits to store log level of each group)          \n"
"                  - Default: 0x3333333333333333 (enable all config logs)                     \n"
"                  - Level (0 ~ 7)                                                            \n"
"                    0 : LOG_LEVEL_NULL                                                       \n"
"                    1 : LOG_LEVEL_ERR                                                        \n"
"                    2 : LOG_LEVEL_WARN                                                       \n"
"                    3 : LOG_LEVEL_CFG                                                        \n"
"                    4 : LOG_LEVEL_INFO                                                       \n"
"                    5 : LOG_LEVEL_DBG                                                        \n"
"                    6 : LOG_LEVEL_VERB                                                       \n"
"                    7 : LOG_LEVEL_TRACE                                                      \n"
"                  - Bit mask                                                                 \n"
"                    bits  0 ~  3 :  LOG_GROUP_APP                                            \n"
"                    bits  4 ~  7 :  LOG_GROUP_HAL                                            \n"
"                    bits  8 ~ 11 :  LOG_GROUP_FW                                             \n"
"                    bits 12 ~ 15 :  LOG_GROUP_SENSOR                                         \n"
"                    bits 16 ~ 19 :  LOG_GROUP_ISP                                            \n"
"                    bits 20 ~ 23 :  LOG_GROUP_3A                                             \n"
"                    bits 20 ~ 23 :  LOG_GROUP_UTILS                                          \n"
"                    bits 24 ~ 27 :  LOG_GROUP_MGR                                            \n"
"                    bits 28 ~ 31 :  LOG_GROUP_PPROC                                          \n"
"                    bits 32 ~ 35 :  LOG_GROUP_OSIF                                           \n"
"                    bits 60 ~ 63 :  LOG_GROUP_PRINTF                                         \n"
"    @ispMask      ISP logging mask (Every bit to enable/disable logs of each isp module)     \n"
"                  - Default: 0xffffffffffffffff                                              \n"
"                    bit  0  :  LOG_ISP_GROUP_STATS                                           \n"
"                    bit  1  :  LOG_ISP_GROUP_BLC                                             \n"
"                    bit  2  :  LOG_ISP_GROUP_CAC                                             \n"
"                    bit  3  :  LOG_ISP_GROUP_CCM                                             \n"
"                    bit  4  :  LOG_ISP_GROUP_CMB                                             \n"
"                    bit  5  :  LOG_ISP_GROUP_CNR                                             \n"
"                    bit  6  :  LOG_ISP_GROUP_CSC                                             \n"
"                    bit  7  :  LOG_ISP_GROUP_CSC2                                            \n"
"                    bit  8  :  LOG_ISP_GROUP_DCP                                             \n"
"                    bit  9  :  LOG_ISP_GROUP_DFG                                             \n"
"                    bit  10 :  LOG_ISP_GROUP_DGN                                             \n"
"                    bit  11 :  LOG_ISP_GROUP_DMS                                             \n"
"                    bit  12 :  LOG_ISP_GROUP_DPC                                             \n"
"                    bit  13 :  LOG_ISP_GROUP_DPF                                             \n"
"                    bit  14 :  LOG_ISP_GROUP_GDC                                             \n"
"                    bit  15 :  LOG_ISP_GROUP_GGC                                             \n"
"                    bit  16 :  LOG_ISP_GROUP_IFBC                                            \n"
"                    bit  17 :  LOG_ISP_GROUP_IFBD                                            \n"
"                    bit  18 :  LOG_ISP_GROUP_L2Q                                             \n"
"                    bit  19 :  LOG_ISP_GROUP_LNC                                             \n"
"                    bit  20 :  LOG_ISP_GROUP_LSC                                             \n"
"                    bit  21 :  LOG_ISP_GROUP_LSH                                             \n"
"                    bit  22 :  LOG_ISP_GROUP_LTM                                             \n"
"                    bit  23 :  LOG_ISP_GROUP_MVD                                             \n"
"                    bit  24 :  LOG_ISP_GROUP_NP                                              \n"
"                    bit  25 :  LOG_ISP_GROUP_NRP                                             \n"
"                    bit  26 :  LOG_ISP_GROUP_PSC                                             \n"
"                    bit  27 :  LOG_ISP_GROUP_PTM                                             \n"
"                    bit  28 :  LOG_ISP_GROUP_Q2L                                             \n"
"                    bit  29 :  LOG_ISP_GROUP_R2B                                             \n"
"                    bit  30 :  LOG_ISP_GROUP_RIR                                             \n"
"                    bit  31 :  LOG_ISP_GROUP_RNR                                             \n"
"                    bit  32 :  LOG_ISP_GROUP_RTNR                                            \n"
"                    bit  33 :  LOG_ISP_GROUP_SCAL                                            \n"
"                    bit  34 :  LOG_ISP_GROUP_SDE                                             \n"
"                    bit  35 :  LOG_ISP_GROUP_TOP                                             \n"
"                    bit  36 :  LOG_ISP_GROUP_TPG                                             \n"
"                    bit  37 :  LOG_ISP_GROUP_VIN                                             \n"
"                    bit  38 :  LOG_ISP_GROUP_VOUT                                            \n"
"                    bit  39 :  LOG_ISP_GROUP_WBG                                             \n"
"                    bit  40 :  LOG_ISP_GROUP_YNR                                             \n"
"                    bit  41 :  LOG_ISP_GROUP_YTNR                                            \n"
"    @sw3aMask     3A logging mask (Every bit to enable/disable logs of each 3A module)       \n"
"                  - Default: 0xffffffffffffffff                                              \n"
"                    bit  0  :  LOG_ISP_GROUP_AEC                                             \n"
"                    bit  1  :  LOG_ISP_GROUP_AWB                                             \n"
"                    bit  2  :  LOG_ISP_GROUP_AF                                              \n"
"    @logSimple    flag to enable/disbale simple mode                                         \n"
"                  - 0x0  :  disable simple mode (default)                                    \n"
"                  - 0x1  :  enable simple mode                                               \n"
"    @logPath      file path to save all log                                                  \n"
"                  - fileName (string)                                                        \n"
"    @tracePath    file path to save trace log                                                \n"
"                  - fileName (string)                                                        \n"
"    @showFps      enable / disable show fps log                                              \n"
"                  - 0 (disable)                                                              \n"
"                  - 1 (enable, default)                                                      \n"
"    @baudRate     Uart baud rate                                                             \n"
"                  - 115200 (default)                                                         \n"
"                  - 921600 (MAX)                                                             \n"
"[e.g.]                                                                                       \n"
"log:levelMask=0x3333333333,ispMask=0xfffffffffffffffff,sw3aMask=0x7,baudRate=921600          \n"
"---------------------------------------------------------------------------------------------\n"
"dump:    (dump images or status)                                                             \n"
"    @camid        camera index (0 ~ MAX:15)                                                  \n"
"                  - N (0 ~ MAX:15)                                                           \n"
"    @analysis     dump camera status (64 bits flag)                                          \n"
"                  - bits 0 ~ 58    (resv data)                                               \n"
"                  - bits 59        (save to file flag)                                       \n"
"                  - bits 60 ~ 63   (dump group)                                              \n"
"                    0 - DUMP_GROUP_NONE                                                      \n"
"                    1 - DUMP_GROUP_APP                                                       \n"
"                    2 - DUMP_GROUP_HAL                                                       \n"
"                    3 - DUMP_GROUP_FW                                                        \n"
"                    4 - DUMP_GROUP_SENSOR                                                    \n"
"                    5 - DUMP_GROUP_ISP                                                       \n"
"                    6 - DUMP_GROUP_3A                                                        \n"
"    @num          number of frames to dump at once                                           \n"
"                  - N (1 ~ MAX:30)                                                           \n"
"    @period       image dump automatically per period                                        \n"
"                  - N (>=1)                                                                  \n"
"[e.g.]                                                                                       \n"
"dump:camid=0,num=1                          (dump image)                                     \n"
"dump:camid=0,analysis=0x4400000000000000    (dump sensor json)                               \n"
"---------------------------------------------------------------------------------------------\n"
"evt:     (send event)                                                                        \n"
"    @camid        camera index (0 ~ MAX:15)                                                  \n"
"                  - N (0 ~ MAX:15)                                                           \n"
"    @type         event type                                                                 \n"
"                  - 0 (motor)                                                                \n"
"                  - 1 (sensor)                                                               \n"
"                  - 2 (isp)                                                                  \n"
"                  - 3 (special)                                                              \n"
"    @id           event identity                                                             \n"
"                  - N (0 ~ MAX)                                                              \n"
"    @dirt         event direction                                                            \n"
"                  - 0 (write)                                                                \n"
"                  - 1 (read)                                                                 \n"
"    @data         event data                                                                 \n"
"                  - N (0 ~ MAX)                                                              \n"
"[e.g.]                                                                                       \n"
"evt:camid=0,type=0,id=0,dirt=0,data=1                                                        \n"
"evt:camid=0,type=0,id=0,dirt=1                                                               \n"
"---------------------------------------------------------------------------------------------\n"
"stop:    (stop all camera)                                                                   \n"
"[e.g.]                                                                                       \n"
"stop                                                                                         \n"
"---------------------------------------------------------------------------------------------\n"
"exit     (exit application)                                                                  \n"
"[e.g.]                                                                                       \n"
"exit                                                                                         \n"
"---------------------------------------------------------------------------------------------\n"
"sleep    (sleep X seconds)                                                                   \n"
"    @time         sleep time                                                                 \n"
"                  - N (seconds)                                                              \n"
"[e.g.]                                                                                       \n"
"sleep:time=1                                                                                 \n"
"---------------------------------------------------------------------------------------------\n"
};
#endif

#define APP_ENABLE_USECASE (0)

/// APP sleep information
typedef struct __app_sleep_info
{
    u64 startTimeUs;
    u64 sleepTimeUs;
} app_sleep_info_t;

/// APP file information struct
typedef struct __app_file_cfg
{
    char8*        sFilename[128];
    FileHandle*   pFile;
} app_file_cfg;

/// APP descriptor struct
typedef struct __app_desc
{
    cbs_t             appCbs;
    BOOL              isRunning;
    obj_list_t*       listProbeInfo;

    thread_os_t*      pInputThread;

    niss_t*           pNissUart;
    niss_t*           pNissPlatform;
    niss_t*           pNissEthernet;
    niss_t*           pNissUsb;
    niss_t*           pNissHdmi;

    app_file_cfg      cfgFile;

    BOOL              showFps;
    app_config_t*     ppAppConfig[MAX_CAMERA];
    device_t*         ppDev[MAX_CAMERA];
    app_sleep_info_t  sleepInfo;

#if APP_ENABLE_USECASE
    app_usecase_t*    ppAppUsecase[MAX_CAMERA];
#endif
	cmd_src_type_e    cur_cmd_src_type;
} app_desc_t;

/*
* App_Layer_Process
*
* @brief APP process entry
*
* @param app desc
*
* @return 0 if success,  others if failed
*/
s32 App_Layer_Process(app_desc_t *pAppDesc);

#endif

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

#ifndef __LOG_UTILS_H__
#define __LOG_UTILS_H__

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>

#include "types_utils.h"
#include "fs_osutils.h"
#include "time_osutils.h"
#include "string_fmt_osutils.h"
#include "thread_osutils.h"

///Define assert debug
#ifndef NDEBUG
#include <assert.h>
#define ISP_ASSERT(condition) assert(condition)
#else
#define ISP_ASSERT(condition) do{}while(0)
#endif

#define PRINT_CUSTOM_BY_MPP
/// Define Log Level
typedef enum
{
    LOG_LEVEL_NULL      = 0,  /// Disable all log
    LOG_LEVEL_ERR       = 1,  /// Always output error log
    LOG_LEVEL_WARN      = 2,  /// Debug version will always output warning
    LOG_LEVEL_CFG       = 3,  /// Debug version will always output configuration
    LOG_LEVEL_INFO      = 4,  /// Debug version default open info
    LOG_LEVEL_DBG       = 5,  /// Debug version default close debug
    LOG_LEVEL_VERB      = 6,  /// Debug version default close verbose
    LOG_LEVEL_TRACE     = 7,  /// Debuf version default close, for function call backtrace
    LOG_LEVEL_MAX
} log_level_t;

/// Define Log Group
typedef enum
{
    LOG_GROUP_APP       = 0,   /// APP Layer Group log
    LOG_GROUP_HAL       = 1,   /// HAL Layer Group log
    LOG_GROUP_FW        = 2,   /// FW Layer Group log
    LOG_GROUP_SENSOR    = 3,   /// Sensor Group log
    LOG_GROUP_ISP       = 4,   /// ISP Group Log
    LOG_GROUP_3A        = 5,   /// 3A Group Log
    LOG_GROUP_UTILS     = 6,   /// UTILS Group log
    LOG_GROUP_MGR       = 7,   /// Frame manager group log
    LOG_GROUP_PPROC     = 8,   /// PProc group log
    LOG_GROUP_OSIF      = 9,   /// OS interface log
    LOG_GROUP_STEST     = 10,  /// sanity test group log
    LOG_GROUP_PERFT     = 11,  /// perf test group log
    LOG_GROUP_PRINTF,          /// print to console but not save to file

    LOG_GROUP_MAX
} log_group_t;

typedef enum
{
    LOG_ISP_GROUP_STATS = 0,  /// Stats modules: AAC / AAY / AEP
    LOG_ISP_GROUP_BLC,        /// Black level correction
    LOG_ISP_GROUP_CAC,        /// Chroma aberration correction
    LOG_ISP_GROUP_CCM,        /// Colour correction matrix
    LOG_ISP_GROUP_CMB,        /// Combine
    LOG_ISP_GROUP_CNR,        /// Chroma noise reduction
    LOG_ISP_GROUP_CSC,        /// Colour space converter
    LOG_ISP_GROUP_CSC2,       /// Colour space converter 2
    LOG_ISP_GROUP_DCP,        /// De companding
    LOG_ISP_GROUP_DFG,        /// De-fog
    LOG_ISP_GROUP_DGN,        /// Digital gain
    LOG_ISP_GROUP_DMS,        /// Demosaic
    LOG_ISP_GROUP_DPC,        /// Defect pixel correction
    LOG_ISP_GROUP_DPF,        /// De-purple fringe
    LOG_ISP_GROUP_GDC,        /// Geometry distortion correction
    LOG_ISP_GROUP_GGC,        /// Global gamma curve
    LOG_ISP_GROUP_IFBC,       /// IFBC
    LOG_ISP_GROUP_IFBD,       /// IFBD
    LOG_ISP_GROUP_L2Q,        /// Linear to square
    LOG_ISP_GROUP_LNC,        /// Linearization correction
    LOG_ISP_GROUP_LSC,        /// Lens shadding correction
    LOG_ISP_GROUP_LSH,        /// Luma sharpening
    LOG_ISP_GROUP_LTM,        /// Local tone mapping
    LOG_ISP_GROUP_MVD,        /// Motion vector detection
    LOG_ISP_GROUP_NP,         /// Noise profile
    LOG_ISP_GROUP_NRP,        /// Pre noise reduction
    LOG_ISP_GROUP_PSC,        /// Pre scale: down scaleing behaivour
    LOG_ISP_GROUP_PTM,        /// Post tone mapping
    LOG_ISP_GROUP_Q2L,        /// Square to linear
    LOG_ISP_GROUP_R2B,        /// Bridge R2B
    LOG_ISP_GROUP_RIR,        /// RGBIR remosaic
    LOG_ISP_GROUP_RNR,        /// RAW noise reduction
    LOG_ISP_GROUP_RTNR,       /// RAW domain temporal noise reduction
    LOG_ISP_GROUP_SCAL,       /// Scaling engine A/B/D/E
    LOG_ISP_GROUP_SDE,        /// Special digital effect
    LOG_ISP_GROUP_TOP,        /// Top level configuration
    LOG_ISP_GROUP_TPG,        /// Test pattern generation
    LOG_ISP_GROUP_VIN,        /// Video input
    LOG_ISP_GROUP_VOUT,       /// Video output
    LOG_ISP_GROUP_WBG,        /// White balance gain
    LOG_ISP_GROUP_YNR,        /// YUV noise reduction: YUV section 5
    LOG_ISP_GROUP_YTNR,       /// YUV domain temporal noise reduction YUV section 1

    LOG_ISP_GROUP_TTNR,       /// TS 536 TTNR log mask
    
    LOG_ISP_GROUP_MAX
} log_isp_group_t;

typedef enum
{
    LOG_3A_GROUP_AEC  = 0,   /// AEC logs
    LOG_3A_GROUP_AWB  = 1,   /// AWB logs
    LOG_3A_GROUP_AF   = 2,   /// AF  logs
    LOG_3A_GROUP_MAX,
} log_3a_group_t;

#define LOG_BUF_MAX_SIZE (2048)

/// Define struct for log saving
typedef struct
{
    BOOL           isSaveToFile;
    FileHandle*    pFile;
    const char8*   sFilename;
    u32            saveCnt;
} log_save_t;

/// Define struct for logging
typedef struct
{
    BOOL                logLevel[LOG_GROUP_MAX][LOG_LEVEL_MAX];
    BOOL                logIspLevel[LOG_ISP_GROUP_MAX][LOG_LEVEL_MAX];
    BOOL                log3aLevel[LOG_3A_GROUP_MAX][LOG_LEVEL_MAX];
    BOOL                isSimple;

    log_save_t          globalLogSave;
    log_save_t          traceLogSave;

    char8               logBuf[LOG_BUF_MAX_SIZE];
    char8               tsBuf[32];

    thread_os_mutex_t   lock;
    BOOL                lockInit;
#ifdef PRINT_CUSTOM_BY_MPP
    void*               pfunc_custom;
#endif
} log_info_t;

/*
* Log_Set_Level
*
* @brief Set logging level of each group by logMask
*        Meanwhile, set logIspMask / log3aMask to determind which sub module logs will be enabled
*
* @param levelMask: Every 4 bits to store log level of each group
*                   - Level (0 ~ 7)
*                      0 : LOG_LEVEL_NULL
*                      1 : LOG_LEVEL_ERR
*                      2 : LOG_LEVEL_WARN
*                      3 : LOG_LEVEL_CFG
*                      4 : LOG_LEVEL_INFO
*                      5 : LOG_LEVEL_DBG
*                      6 : LOG_LEVEL_VERB
*                      7 : LOG_LEVEL_TRACE
*                   - Bit mask
*                      bits  0  ~ 3  :  LOG_GROUP_APP
*                      bits  4  ~ 7  :  LOG_GROUP_HAL
*                      bits  8  ~ 11 :  LOG_GROUP_FW
*                      bits  12 ~ 15 :  LOG_GROUP_SENSOR
*                      bits  16 ~ 19 :  LOG_GROUP_ISP
*                      bits  20 ~ 23 :  LOG_GROUP_3A
*                      bits  24 ~ 27 :  LOG_GROUP_UTILS
*                      bits  28 ~ 31 :  LOG_GROUP_MGR
*                      bits  60 ~ 63 :  LOG_GROUP_PRINTF
*
* @param ispMask: Every bit to store enable flag of each ISP group
*                      bit  0  :  LOG_ISP_GROUP_STATS
*                      bit  1  :  LOG_ISP_GROUP_BLC
*                      bit  2  :  LOG_ISP_GROUP_CAC
*                      bit  3  :  LOG_ISP_GROUP_CCM
*                      bit  4  :  LOG_ISP_GROUP_CMB
*                      bit  5  :  LOG_ISP_GROUP_CNR
*                      bit  6  :  LOG_ISP_GROUP_CSC
*                      bit  7  :  LOG_ISP_GROUP_CSC2
*                      bit  8  :  LOG_ISP_GROUP_DCP
*                      bit  9  :  LOG_ISP_GROUP_DFG
*                      bit  10 :  LOG_ISP_GROUP_DGN
*                      bit  11 :  LOG_ISP_GROUP_DMS
*                      bit  12 :  LOG_ISP_GROUP_DPC
*                      bit  13 :  LOG_ISP_GROUP_DPF
*                      bit  14 :  LOG_ISP_GROUP_GDC
*                      bit  15 :  LOG_ISP_GROUP_GGC
*                      bit  16 :  LOG_ISP_GROUP_L2Q
*                      bit  17 :  LOG_ISP_GROUP_LNC
*                      bit  18 :  LOG_ISP_GROUP_LSC
*                      bit  19 :  LOG_ISP_GROUP_LSH
*                      bit  20 :  LOG_ISP_GROUP_LTM
*                      bit  21 :  LOG_ISP_GROUP_MVD
*                      bit  22 :  LOG_ISP_GROUP_NP
*                      bit  23 :  LOG_ISP_GROUP_NRP
*                      bit  24 :  LOG_ISP_GROUP_PSC
*                      bit  25 :  LOG_ISP_GROUP_PTM
*                      bit  26 :  LOG_ISP_GROUP_Q2L
*                      bit  27 :  LOG_ISP_GROUP_R2B
*                      bit  28 :  LOG_ISP_GROUP_RIR
*                      bit  29 :  LOG_ISP_GROUP_RNR
*                      bit  30 :  LOG_ISP_GROUP_RTNR
*                      bit  31 :  LOG_ISP_GROUP_SCAL
*                      bit  32 :  LOG_ISP_GROUP_SDE
*                      bit  33 :  LOG_ISP_GROUP_TPG
*                      bit  34 :  LOG_ISP_GROUP_VIN
*                      bit  35 :  LOG_ISP_GROUP_VOUT
*                      bit  36 :  LOG_ISP_GROUP_WBG
*                      bit  37 :  LOG_ISP_GROUP_YNR
*                      bit  38 :  LOG_ISP_GROUP_YTNR
*
* @param sw3aMask: Every bit to store enable flag of each 3A group
*                      bit 0 :  LOG_3A_GROUP_AEC
*                      bit 1 :  LOG_3A_GROUP_AWB
*                      bit 2 :  LOG_3A_GROUP_AF
*
* @return void
*/
void Log_Set_Level(u64 levelMask, u64 ispMask, u64 sw3aMask);

/*
* Log_Set_Simple_Mode
*
* @brief Set logging simple mode
*
* @param isSample: flag to enable/disbale simple mode
*
* @return void
*/
void Log_Set_Simple_Mode(BOOL isSample);

/*
* Log_Set_Path
*
* @brief Set path for log saving
*
* @param sGlobalPath: filename to save all logs
*
* @param sTracePath: filename to save trace logs
*
* @return void
*/
void Log_Set_Path(char8* sGlobalPath, char8* sTracePath);

/*
* Log_Check_Subgroup_Level
*
* @brief Check whether sub module logs are enabled
*
* @param group:       debug group for log
* @param subGroup:    subgroup of isp or 3a
* @param level:       log output level
*
* @return TRUE if enabled, FALSE if disable
*/
BOOL Log_Check_Subgroup_Level (log_group_t group, u32 subGroup, log_level_t level);

/*
* Log_Print
*
* @brief output format logs for debug
*
* @param group:    debug group for log
* @param level:    log output level
* @param filename: string point to filename
* @param lineNum:  line number
* @param funcName: string point to function name
* @param format:   log input format
* @param ...   :   dynamic variable paramters
*
* @return void
*/
#ifdef __cplusplus
extern "C"
{
#endif
    void Log_Print(log_group_t      group,
        log_level_t  level,
        const char8* fileName,
        u32          lineNum,
        const char8* funcName,
        const char8* format, ...);
#ifdef __cplusplus
}
#endif

#if defined (__linux__)
#else
#define PRINTALL
#endif

#ifdef PRINT_CUSTOM_BY_MPP
/*
* Log_Set_Custom_Print
*
* @brief Set custom print function for log print
*
* @param pFuncPrint: custom function
*
* @return void
*/
typedef void (*custom_log_print)(s32 level, const char8 *content);
void Log_Set_Custom_Print(void* pFuncPrint);
#endif

/// PRINTALL is a Macro control by compile define
#ifdef PRINTALL
/// log format will like below
/// 2021_04_10 11:27:28.012345  1012  1013 <ISP> [ERROR] isp_node.c ISP_Create(19): Finished create pipeine 15
#define PRINT(group, level, fmt, ...) \
    Log_Print(group, level, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)
#else
#define PRINT(group, level, fmt, ...) do{}while(0)
#endif

#define PRINT_CHECK_SUBMOD(group, subgroup, level, fmt, ...)                         \
    if (Log_Check_Subgroup_Level(group, subgroup, level))                            \
    {                                                                                \
        Log_Print(group, level, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__);   \
    }

/// Simple printf function
//#define PRINTF(fmt, ...) printf(fmt"\n", ##__VA_ARGS__)
#define PRINTF(fmt, ...) PRINT(LOG_GROUP_PRINTF, LOG_LEVEL_INFO, fmt, ##__VA_ARGS__)

/// Common Error log
#define LOGE(group, fmt, ...) PRINT(group, LOG_LEVEL_ERR,   fmt, ##__VA_ARGS__)

/// Common Warning log
#define LOGW(group, fmt, ...) PRINT(group, LOG_LEVEL_WARN,  fmt, ##__VA_ARGS__)

/// Common configuration log
#define LOGC(group, fmt, ...) PRINT(group, LOG_LEVEL_CFG,   fmt, ##__VA_ARGS__)

/// Common information log
#define LOGI(group, fmt, ...) PRINT(group, LOG_LEVEL_INFO,  fmt, ##__VA_ARGS__)

/// Common debug log
#define LOGD(group, fmt, ...) PRINT(group, LOG_LEVEL_DBG,   fmt, ##__VA_ARGS__)

/// Common verbose log
#define LOGV(group, fmt, ...) PRINT(group, LOG_LEVEL_VERB,  fmt, ##__VA_ARGS__)

/// Common trace log
#define LOGT(group, fmt, ...) PRINT(group, LOG_LEVEL_TRACE, fmt, ##__VA_ARGS__)

/*
* APP log print function
*
*/
/// APP Group ERR Log
#define APP_ERR(fmt, ...)   PRINT(LOG_GROUP_APP, LOG_LEVEL_ERR, fmt, ##__VA_ARGS__)

/// APP Group Warning Log
#define APP_WARN(fmt, ...)  PRINT(LOG_GROUP_APP, LOG_LEVEL_WARN, fmt, ##__VA_ARGS__)

/// APP Group CFG Log
#define APP_CFG(fmt, ...)   PRINT(LOG_GROUP_APP, LOG_LEVEL_CFG, fmt, ##__VA_ARGS__)

/// APP Group Info log
#define APP_INFO(fmt, ...)  PRINT(LOG_GROUP_APP, LOG_LEVEL_INFO, fmt, ##__VA_ARGS__)

/// APP Group debug log
#define APP_DBG(fmt, ...)   PRINT(LOG_GROUP_APP, LOG_LEVEL_DBG, fmt, ##__VA_ARGS__)

/// APP Group verbos log
#define APP_VERB(fmt, ...)  PRINT(LOG_GROUP_APP, LOG_LEVEL_VERB, fmt, ##__VA_ARGS__)

/// APP Group verbos log
#define APP_TRACE(fmt, ...) PRINT(LOG_GROUP_APP, LOG_LEVEL_TRACE, fmt, ##__VA_ARGS__)

/// APP Group function enter trace
#define APP_FUNC_ENTER()    PRINT(LOG_GROUP_APP, LOG_LEVEL_TRACE, "%s", "Enter")

/// APP Group function exit trace
#define APP_FUNC_EXIT()     PRINT(LOG_GROUP_APP, LOG_LEVEL_TRACE, "%s", "Exit")

/*
* HAL log print function
*
*/
/// HAL Group ERR Log
#define HAL_ERR(fmt, ...)   PRINT(LOG_GROUP_HAL, LOG_LEVEL_ERR, fmt, ##__VA_ARGS__)

/// HAL Group Warning Log
#define HAL_WARN(fmt, ...)  PRINT(LOG_GROUP_HAL, LOG_LEVEL_WARN, fmt, ##__VA_ARGS__)

/// HAL Group CFG Log
#define HAL_CFG(fmt, ...)   PRINT(LOG_GROUP_HAL, LOG_LEVEL_CFG, fmt, ##__VA_ARGS__)

/// HAL Group Info log
#define HAL_INFO(fmt, ...)  PRINT(LOG_GROUP_HAL, LOG_LEVEL_INFO, fmt, ##__VA_ARGS__)

/// HAL Group debug log
#define HAL_DBG(fmt, ...)   PRINT(LOG_GROUP_HAL, LOG_LEVEL_DBG, fmt, ##__VA_ARGS__)

/// HAL Group verbos log
#define HAL_VERB(fmt, ...)  PRINT(LOG_GROUP_HAL, LOG_LEVEL_VERB, fmt, ##__VA_ARGS__)

/// HAL Group trace log
#define HAL_TRACE(fmt, ...) PRINT(LOG_GROUP_HAL, LOG_LEVEL_TRACE, fmt, ##__VA_ARGS__)

/// HAL Group function enter trace
#define HAL_FUNC_ENTER()    PRINT(LOG_GROUP_HAL, LOG_LEVEL_TRACE, "%s", "Enter")

/// HAL Group function exit trace
#define HAL_FUNC_EXIT()     PRINT(LOG_GROUP_HAL, LOG_LEVEL_TRACE, "%s", "Exit")

/*
* FW log print function
*
*/
/// FW Group ERR Log
#define FW_ERR(fmt, ...)    PRINT(LOG_GROUP_FW, LOG_LEVEL_ERR, fmt, ##__VA_ARGS__)

/// FW Group Warning Log
#define FW_WARN(fmt, ...)   PRINT(LOG_GROUP_FW, LOG_LEVEL_WARN, fmt, ##__VA_ARGS__)

/// FW Group CFG Log
#define FW_CFG(fmt, ...)    PRINT(LOG_GROUP_FW, LOG_LEVEL_CFG, fmt, ##__VA_ARGS__)

/// FW Group Info log
#define FW_INFO(fmt, ...)   PRINT(LOG_GROUP_FW, LOG_LEVEL_INFO, fmt, ##__VA_ARGS__)

/// FW Group debug log
#define FW_DBG(fmt, ...)    PRINT(LOG_GROUP_FW, LOG_LEVEL_DBG, fmt, ##__VA_ARGS__)

/// FW Group verbos log
#define FW_VERB(fmt, ...)   PRINT(LOG_GROUP_FW, LOG_LEVEL_VERB, fmt, ##__VA_ARGS__)

/// FW Group trace log
#define FW_TRACE(fmt, ...)  PRINT(LOG_GROUP_FW, LOG_LEVEL_TRACE, fmt, ##__VA_ARGS__)

/// FW Group function enter trace
#define FW_FUNC_ENTER()     PRINT(LOG_GROUP_FW, LOG_LEVEL_TRACE, "%s", "Enter")

/// FW Group function exit trace
#define FW_FUNC_EXIT()      PRINT(LOG_GROUP_FW, LOG_LEVEL_TRACE, "%s", "Exit")

/*
* SENSOR log print function
*
*/
/// SENSOR Group ERR Log
#define SENSOR_ERR(fmt, ...)    PRINT(LOG_GROUP_SENSOR, LOG_LEVEL_ERR, fmt, ##__VA_ARGS__)

/// SENSOR Group Warning Log
#define SENSOR_WARN(fmt, ...)   PRINT(LOG_GROUP_SENSOR, LOG_LEVEL_WARN, fmt, ##__VA_ARGS__)

/// SENSOR Group CFG Log
#define SENSOR_CFG(fmt, ...)    PRINT(LOG_GROUP_SENSOR, LOG_LEVEL_CFG, fmt, ##__VA_ARGS__)

/// SENSOR Group Info log
#define SENSOR_INFO(fmt, ...)   PRINT(LOG_GROUP_SENSOR, LOG_LEVEL_INFO, fmt, ##__VA_ARGS__)

/// SENSOR Group debug log
#define SENSOR_DBG(fmt, ...)    PRINT(LOG_GROUP_SENSOR, LOG_LEVEL_DBG, fmt, ##__VA_ARGS__)

/// SENSOR Group verbos log
#define SENSOR_VERB(fmt, ...)   PRINT(LOG_GROUP_SENSOR, LOG_LEVEL_VERB, fmt, ##__VA_ARGS__)

/// SENSOR Group trace log
#define SENSOR_TRACE(fmt, ...)  PRINT(LOG_GROUP_SENSOR, LOG_LEVEL_TRACE, fmt, ##__VA_ARGS__)

/// SENSOR Group function enter trace
#define SENSOR_FUNC_ENTER()     PRINT(LOG_GROUP_SENSOR, LOG_LEVEL_TRACE, "%s", "Enter")

/// SENSOR Group function exit trace
#define SENSOR_FUNC_EXIT()      PRINT(LOG_GROUP_SENSOR, LOG_LEVEL_TRACE, "%s", "Exit")

/*
* ISP log print function
*
*/
/// ISP Group ERR Log
#define ISP_ERR(fmt, ...)   PRINT(LOG_GROUP_ISP, LOG_LEVEL_ERR, fmt, ##__VA_ARGS__)

/// ISP Group Warning Log
#define ISP_WARN(fmt, ...)  PRINT(LOG_GROUP_ISP, LOG_LEVEL_WARN, fmt, ##__VA_ARGS__)

/// ISP Group CFG Log
#define ISP_CFG(fmt, ...)   PRINT(LOG_GROUP_ISP, LOG_LEVEL_CFG, fmt, ##__VA_ARGS__)

/// ISP Group Info log
#define ISP_INFO(fmt, ...)  PRINT(LOG_GROUP_ISP, LOG_LEVEL_INFO, fmt, ##__VA_ARGS__)

/// ISP Group debug log
#define ISP_DBG(fmt, ...)   PRINT(LOG_GROUP_ISP, LOG_LEVEL_DBG, fmt, ##__VA_ARGS__)

/// ISP Group verbos log
#define ISP_VERB(fmt, ...)  PRINT(LOG_GROUP_ISP, LOG_LEVEL_VERB, fmt, ##__VA_ARGS__)

/// ISP Group trace log
#define ISP_TRACE(fmt, ...) PRINT(LOG_GROUP_ISP, LOG_LEVEL_TRACE, fmt, ##__VA_ARGS__)

/// ISP Group function enter trace
#define ISP_FUNC_ENTER()    PRINT(LOG_GROUP_ISP, LOG_LEVEL_TRACE, "%s", "Enter")

/// ISP Group function exit trace
#define ISP_FUNC_EXIT()     PRINT(LOG_GROUP_ISP, LOG_LEVEL_TRACE, "%s", "Exit")

/// ISP Sub-module ERR Log
#define ISP_MODULE_ERR(ispGroup, fmt, ...)   PRINT_CHECK_SUBMOD(LOG_GROUP_ISP, ispGroup, LOG_LEVEL_ERR, fmt, ##__VA_ARGS__)

/// ISP Sub-module Warning Log
#define ISP_MODULE_WARN(ispGroup, fmt, ...)  PRINT_CHECK_SUBMOD(LOG_GROUP_ISP, ispGroup, LOG_LEVEL_WARN, fmt, ##__VA_ARGS__)

/// ISP Sub-module CFG Log
#define ISP_MODULE_CFG(ispGroup, fmt, ...)   PRINT_CHECK_SUBMOD(LOG_GROUP_ISP, ispGroup, LOG_LEVEL_CFG, fmt, ##__VA_ARGS__)

/// ISP Sub-module Info log
#define ISP_MODULE_INFO(ispGroup, fmt, ...)  PRINT_CHECK_SUBMOD(LOG_GROUP_ISP, ispGroup, LOG_LEVEL_INFO, fmt, ##__VA_ARGS__)

/// ISP Sub-module debug log
#define ISP_MODULE_DBG(ispGroup, fmt, ...)   PRINT_CHECK_SUBMOD(LOG_GROUP_ISP, ispGroup, LOG_LEVEL_DBG, fmt, ##__VA_ARGS__)

/// ISP Sub-module verbos log
#define ISP_MODULE_VERB(ispGroup, fmt, ...)  PRINT_CHECK_SUBMOD(LOG_GROUP_ISP, ispGroup, LOG_LEVEL_VERB, fmt, ##__VA_ARGS__)

/// ISP Sub-module trace log
#define ISP_MODULE_TRACE(ispGroup, fmt, ...) PRINT_CHECK_SUBMOD(LOG_GROUP_ISP, ispGroup, LOG_LEVEL_TRACE, fmt, ##__VA_ARGS__)

/// ISP Sub-module function enter trace
#define ISP_MODULE_FUNC_ENTER(ispGroup)      PRINT_CHECK_SUBMOD(LOG_GROUP_ISP, ispGroup, LOG_LEVEL_TRACE, "%s", "Enter")

/// ISP Sub-module function exit trace
#define ISP_MODULE_FUNC_EXIT(ispGroup)       PRINT_CHECK_SUBMOD(LOG_GROUP_ISP, ispGroup, LOG_LEVEL_TRACE, "%s", "Exit")

/*
* SW3A log print function
*
*/
/// SW3A Group ERR Log
#define SW3A_ERR(fmt, ...)   PRINT(LOG_GROUP_3A, LOG_LEVEL_ERR, fmt, ##__VA_ARGS__)

/// SW3A Group Warning Log
#define SW3A_WARN(fmt, ...)  PRINT(LOG_GROUP_3A, LOG_LEVEL_WARN, fmt, ##__VA_ARGS__)

/// SW3A Group CFG Log
#define SW3A_CFG(fmt, ...)   PRINT(LOG_GROUP_3A, LOG_LEVEL_CFG, fmt, ##__VA_ARGS__)

/// SW3A Group Info log
#define SW3A_INFO(fmt, ...)  PRINT(LOG_GROUP_3A, LOG_LEVEL_INFO, fmt, ##__VA_ARGS__)

/// SW3A Group debug log
#define SW3A_DBG(fmt, ...)   PRINT(LOG_GROUP_3A, LOG_LEVEL_DBG, fmt, ##__VA_ARGS__)

/// SW3A Group verbos log
#define SW3A_VERB(fmt, ...)  PRINT(LOG_GROUP_3A, LOG_LEVEL_VERB, fmt, ##__VA_ARGS__)

/// SW3A Group trace log
#define SW3A_TRACE(fmt, ...) PRINT(LOG_GROUP_3A, LOG_LEVEL_TRACE, fmt, ##__VA_ARGS__)

/// SW3A Group function enter trace
#define SW3A_FUNC_ENTER()    PRINT(LOG_GROUP_3A, LOG_LEVEL_TRACE, "%s", "Enter")

/// SW3A Group function exit trace
#define SW3A_FUNC_EXIT()     PRINT(LOG_GROUP_3A, LOG_LEVEL_TRACE, "%s", "Exit")

/// SW3A Sub-module ERR Log
#define SW3A_MODULE_ERR(sw3aGroup, fmt, ...)   PRINT_CHECK_SUBMOD(LOG_GROUP_3A, sw3aGroup, LOG_LEVEL_ERR, fmt, ##__VA_ARGS__)

/// SW3A Sub-module Warning Log
#define SW3A_MODULE_WARN(sw3aGroup, fmt, ...)  PRINT_CHECK_SUBMOD(LOG_GROUP_3A, sw3aGroup, LOG_LEVEL_WARN, fmt, ##__VA_ARGS__)

/// SW3A Sub-module CFG Log
#define SW3A_MODULE_CFG(sw3aGroup, fmt, ...)   PRINT_CHECK_SUBMOD(LOG_GROUP_3A, sw3aGroup, LOG_LEVEL_CFG, fmt, ##__VA_ARGS__)

/// SW3A Sub-module Info log
#define SW3A_MODULE_INFO(sw3aGroup, fmt, ...)  PRINT_CHECK_SUBMOD(LOG_GROUP_3A, sw3aGroup, LOG_LEVEL_INFO, fmt, ##__VA_ARGS__)

/// SW3A Sub-module debug log
#define SW3A_MODULE_DBG(sw3aGroup, fmt, ...)   PRINT_CHECK_SUBMOD(LOG_GROUP_3A, sw3aGroup, LOG_LEVEL_DBG, fmt, ##__VA_ARGS__)

/// SW3A Sub-module verbos log
#define SW3A_MODULE_VERB(sw3aGroup, fmt, ...)  PRINT_CHECK_SUBMOD(LOG_GROUP_3A, sw3aGroup, LOG_LEVEL_VERB, fmt, ##__VA_ARGS__)

/// SW3A Sub-module trace log
#define SW3A_MODULE_TRACE(sw3aGroup, fmt, ...) PRINT_CHECK_SUBMOD(LOG_GROUP_3A, sw3aGroup, LOG_LEVEL_TRACE, fmt, ##__VA_ARGS__)

/// SW3A Sub-module function enter trace
#define SW3A_MODULE_FUNC_ENTER(sw3aGroup)      PRINT_CHECK_SUBMOD(LOG_GROUP_3A, sw3aGroup, LOG_LEVEL_TRACE, "%s", "Enter")

/// SW3A Sub-module function exit trace
#define SW3A_MODULE_FUNC_EXIT(sw3aGroup)       PRINT_CHECK_SUBMOD(LOG_GROUP_3A, sw3aGroup, LOG_LEVEL_TRACE, "%s", "Exit")

/*
* UTILS log print function
*
*/
/// UTILS Group ERR Log
#define UTILS_ERR(fmt, ...)     PRINT(LOG_GROUP_UTILS, LOG_LEVEL_ERR, fmt, ##__VA_ARGS__)

/// UTILS Group Warning Log
#define UTILS_WARN(fmt, ...)    PRINT(LOG_GROUP_UTILS, LOG_LEVEL_WARN, fmt, ##__VA_ARGS__)

/// UTILS Group CFG Log
#define UTILS_CFG(fmt, ...)     PRINT(LOG_GROUP_UTILS, LOG_LEVEL_CFG, fmt, ##__VA_ARGS__)

/// UTILS Group Info log
#define UTILS_INFO(fmt, ...)    PRINT(LOG_GROUP_UTILS, LOG_LEVEL_INFO, fmt, ##__VA_ARGS__)

/// UTILS Group debug log
#define UTILS_DBG(fmt, ...)     PRINT(LOG_GROUP_UTILS, LOG_LEVEL_DBG, fmt, ##__VA_ARGS__)

/// UTILS Group verbos log
#define UTILS_VERB(fmt, ...)    PRINT(LOG_GROUP_UTILS, LOG_LEVEL_VERB, fmt, ##__VA_ARGS__)

/// UTILS Group trace log
#define UTILS_TRACE(fmt, ...)   PRINT(LOG_GROUP_UTILS, LOG_LEVEL_TRACE, fmt, ##__VA_ARGS__)

/// UTILS Group function enter trace
#define UTILS_FUNC_ENTER()      PRINT(LOG_GROUP_UTILS, LOG_LEVEL_TRACE, "%s", "Enter")

/// UTILS Group function exit trace
#define UTILS_FUNC_EXIT()       PRINT(LOG_GROUP_UTILS, LOG_LEVEL_TRACE, "%s", "Exit")

/*
* MGR log print function
*
*/
/// MGR Group ERR Log
#define MGR_ERR(fmt, ...)   PRINT(LOG_GROUP_MGR, LOG_LEVEL_ERR, fmt, ##__VA_ARGS__)

/// MGR Group Warning Log
#define MGR_WARN(fmt, ...)  PRINT(LOG_GROUP_MGR, LOG_LEVEL_WARN, fmt, ##__VA_ARGS__)

/// MGR Group CFG Log
#define MGR_CFG(fmt, ...)   PRINT(LOG_GROUP_MGR, LOG_LEVEL_CFG, fmt, ##__VA_ARGS__)

/// MGR Group Info log
#define MGR_INFO(fmt, ...)  PRINT(LOG_GROUP_MGR, LOG_LEVEL_INFO, fmt, ##__VA_ARGS__)

/// MGR Group debug log
#define MGR_DBG(fmt, ...)   PRINT(LOG_GROUP_MGR, LOG_LEVEL_DBG, fmt, ##__VA_ARGS__)

/// MGR Group verbos log
#define MGR_VERB(fmt, ...)  PRINT(LOG_GROUP_MGR, LOG_LEVEL_VERB, fmt, ##__VA_ARGS__)

/// MGR Group trace log
#define MGR_TRACE(fmt, ...) PRINT(LOG_GROUP_MGR, LOG_LEVEL_TRACE, fmt, ##__VA_ARGS__)

/*
* PPROC log print function
*
*/
/// PPROC Group ERR Log
#define PPROC_ERR(fmt, ...)   PRINT(LOG_GROUP_PPROC, LOG_LEVEL_ERR, fmt, ##__VA_ARGS__)

/// PPROC Group Warning Log
#define PPROC_WARN(fmt, ...)  PRINT(LOG_GROUP_PPROC, LOG_LEVEL_WARN, fmt, ##__VA_ARGS__)

/// PPROC Group CFG Log
#define PPROC_CFG(fmt, ...)   PRINT(LOG_GROUP_PPROC, LOG_LEVEL_CFG, fmt, ##__VA_ARGS__)

/// PPROC Group Info log
#define PPROC_INFO(fmt, ...)  PRINT(LOG_GROUP_PPROC, LOG_LEVEL_INFO, fmt, ##__VA_ARGS__)

/// PPROC Group debug log
#define PPROC_DBG(fmt, ...)   PRINT(LOG_GROUP_PPROC, LOG_LEVEL_DBG, fmt, ##__VA_ARGS__)

/// PPROC Group verbos log
#define PPROC_VERB(fmt, ...)  PRINT(LOG_GROUP_PPROC, LOG_LEVEL_VERB, fmt, ##__VA_ARGS__)

/// PPROC Group trace log
#define PPROC_TRACE(fmt, ...) PRINT(LOG_GROUP_PPROC, LOG_LEVEL_TRACE, fmt, ##__VA_ARGS__)

/// PPROC Group function enter trace
#define PPROC_FUNC_ENTER()    PRINT(LOG_GROUP_PPROC, LOG_LEVEL_TRACE, "%s", "Enter")

/// PPROC Group function exit trace
#define PPROC_FUNC_EXIT()     PRINT(LOG_GROUP_PPROC, LOG_LEVEL_TRACE, "%s", "Exit")

/*
* OSIF log print function
*
*/
/// OSIF Group ERR Log
#define OSIF_ERR(fmt, ...)   PRINT(LOG_GROUP_OSIF, LOG_LEVEL_ERR, fmt, ##__VA_ARGS__)

/// OSIF Group Warning Log
#define OSIF_WARN(fmt, ...)  PRINT(LOG_GROUP_OSIF, LOG_LEVEL_WARN, fmt, ##__VA_ARGS__)

/// OSIF Group CFG Log
#define OSIF_CFG(fmt, ...)   PRINT(LOG_GROUP_OSIF, LOG_LEVEL_CFG, fmt, ##__VA_ARGS__)

/// OSIF Group Info log
#define OSIF_INFO(fmt, ...)  PRINT(LOG_GROUP_OSIF, LOG_LEVEL_INFO, fmt, ##__VA_ARGS__)

/// OSIF Group debug log
#define OSIF_DBG(fmt, ...)   PRINT(LOG_GROUP_OSIF, LOG_LEVEL_DBG, fmt, ##__VA_ARGS__)

/// OSIF Group verbos log
#define OSIF_VERB(fmt, ...)  PRINT(LOG_GROUP_OSIF, LOG_LEVEL_VERB, fmt, ##__VA_ARGS__)

/// OSIF Group trace log
#define OSIF_TRACE(fmt, ...) PRINT(LOG_GROUP_OSIF, LOG_LEVEL_TRACE, fmt, ##__VA_ARGS__)

/// OSIF Group function enter trace
#define OSIF_FUNC_ENTER()    PRINT(LOG_GROUP_OSIF, LOG_LEVEL_TRACE, "%s", "Enter")

/// OSIF Group function exit trace
#define OSIF_FUNC_EXIT()     PRINT(LOG_GROUP_OSIF, LOG_LEVEL_TRACE, "%s", "Exit")

/*
* STEST log print function
*
*/
/// STEST Group ERR Log
#define STEST_ERR(fmt, ...)   PRINT(LOG_GROUP_STEST, LOG_LEVEL_ERR, fmt, ##__VA_ARGS__)

/// STEST Group Warning Log
#define STEST_WARN(fmt, ...)  PRINT(LOG_GROUP_STEST, LOG_LEVEL_WARN, fmt, ##__VA_ARGS__)

/// STEST Group CFG Log
#define STEST_CFG(fmt, ...)   PRINT(LOG_GROUP_STEST, LOG_LEVEL_CFG, fmt, ##__VA_ARGS__)

/// STEST Group Info log
#define STEST_INFO(fmt, ...)  PRINT(LOG_GROUP_STEST, LOG_LEVEL_INFO, fmt, ##__VA_ARGS__)

/// STEST Group debug log
#define STEST_DBG(fmt, ...)   PRINT(LOG_GROUP_STEST, LOG_LEVEL_DBG, fmt, ##__VA_ARGS__)

/// STEST Group verbos log
#define STEST_VERB(fmt, ...)  PRINT(LOG_GROUP_STEST, LOG_LEVEL_VERB, fmt, ##__VA_ARGS__)

/// STEST Group trace log
#define STEST_TRACE(fmt, ...) PRINT(LOG_GROUP_STEST, LOG_LEVEL_TRACE, fmt, ##__VA_ARGS__)

/// STEST Group function enter trace
#define STEST_FUNC_ENTER()    PRINT(LOG_GROUP_STEST, LOG_LEVEL_TRACE, "%s", "Enter")

/// STEST Group function exit trace
#define STEST_FUNC_EXIT()     PRINT(LOG_GROUP_STEST, LOG_LEVEL_TRACE, "%s", "Exit")

/*
* PERFT log print function
*
*/
/// PERFT Group ERR Log
#define PERFT_ERR(fmt, ...)   PRINT(LOG_GROUP_PERFT, LOG_LEVEL_ERR, fmt, ##__VA_ARGS__)

/// PERFT Group Warning Log
#define PERFT_WARN(fmt, ...)  PRINT(LOG_GROUP_PERFT, LOG_LEVEL_WARN, fmt, ##__VA_ARGS__)

/// PERFT Group CFG Log
#define PERFT_CFG(fmt, ...)   PRINT(LOG_GROUP_PERFT, LOG_LEVEL_CFG, fmt, ##__VA_ARGS__)

/// PERFT Group Info log
#define PERFT_INFO(fmt, ...)  PRINT(LOG_GROUP_PERFT, LOG_LEVEL_INFO, fmt, ##__VA_ARGS__)

/// PERFT Group debug log
#define PERFT_DBG(fmt, ...)   PRINT(LOG_GROUP_PERFT, LOG_LEVEL_DBG, fmt, ##__VA_ARGS__)

/// PERFT Group verbos log
#define PERFT_VERB(fmt, ...)  PRINT(LOG_GROUP_PERFT, LOG_LEVEL_VERB, fmt, ##__VA_ARGS__)

/// PERFT Group trace log
#define PERFT_TRACE(fmt, ...) PRINT(LOG_GROUP_PERFT, LOG_LEVEL_TRACE, fmt, ##__VA_ARGS__)

/// PERFT Group function enter trace
#define PERFT_FUNC_ENTER()    PRINT(LOG_GROUP_PERFT, LOG_LEVEL_TRACE, "%s", "Enter")

/// PERFT Group function exit trace
#define PERFT_FUNC_EXIT()     PRINT(LOG_GROUP_PERFT, LOG_LEVEL_TRACE, "%s", "Exit")


#endif

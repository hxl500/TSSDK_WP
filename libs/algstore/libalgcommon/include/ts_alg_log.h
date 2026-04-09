/*
 * Copyright (c) Beijing Tsing Micro Co.,Ltd. All rights reserved.
 * Description: base type definitions of alg
 * Author: Tsing Micro solution-application group
 * Create: 2022/08/18
 */
#ifndef _TS_ALG_LOG_H_
#define _TS_ALG_LOG_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stdbool.h>
#include <stdio.h>

enum { ALG_LOG_TRACE, ALG_LOG_DEBUG, ALG_LOG_INFO, ALG_LOG_WARN, ALG_LOG_ERROR, ALG_LOG_FATAL };

#define ALG_LOGT(...) ts_alg_log_print(ALG_LOG_TRACE, __FILE__, __LINE__, __VA_ARGS__)
#define ALG_LOGD(...) ts_alg_log_print(ALG_LOG_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define ALG_LOGI(...) ts_alg_log_print(ALG_LOG_INFO,  __FILE__, __LINE__, __VA_ARGS__)
#define ALG_LOGW(...) ts_alg_log_print(ALG_LOG_WARN,  __FILE__, __LINE__, __VA_ARGS__)
#define ALG_LOGE(...) ts_alg_log_print(ALG_LOG_ERROR, __FILE__, __LINE__, __VA_ARGS__)
#define ALG_LOGF(...) ts_alg_log_print(ALG_LOG_FATAL, __FILE__, __LINE__, __VA_ARGS__)
void ts_alg_log_print(int level, const char *file, int line, const char *fmt, ...);

/**
 * @brief the interface of set log level
 * @param[in] level, the log level
 * @return none
 */
void TS_ALG_SetLogLevel(int level);

/**
 * @brief the interface of get log level
 * @param none
 * @return log level
 */
int TS_ALG_GetLogLevel(void);

/**
 * @brief the interface of set log quiet
 * @param[in] enable, set log quiet
 * @return none
 */
void TS_ALG_SetLogQuiet(bool enable);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif


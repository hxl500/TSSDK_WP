/*
 * Copyright © 2019 TSING MICRO.com.cn All Rights Reserved
 * Description: ts_debug.h
 * Author:TsingMicro multimedia software group
 * Create: 2022/06/02
 */

#ifndef __TS_DEBUG_H__
#define __TS_DEBUG_H__

#ifndef __KERNEL__
#include <stdio.h>
#include <stdarg.h>
#include <assert.h>
#endif

#include "ts_type.h"
#include "ts_common.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define _EX__FILE_LINE(fxx,lxx) "[File]:"fxx"\n[Line]:"#lxx"\n[Info]:"
#define EX__FILE_LINE(fxx,lxx) _EX__FILE_LINE(fxx,lxx)
#define __FILE_LINE__ EX__FILE_LINE(__FILE__, __LINE__)

#define TS_DBG_EMERG      0   /* system is unusable                   */
#define TS_DBG_ALERT      1   /* action must be taken immediately     */
#define TS_DBG_CRIT       2   /* critical conditions                  */
#define TS_DBG_ERR        3   /* error conditions                     */
#define TS_DBG_WARN       4   /* warning conditions                   */
#define TS_DBG_NOTICE     5   /* normal but significant condition     */
#define TS_DBG_INFO       6   /* informational                        */
#define TS_DBG_DEBUG      7   /* debug-level messages                 */

typedef struct tsLOG_LEVEL_CONF_S {
    MOD_ID_E  enModId;
    TS_S32    s32Level;
    TS_CHAR   cModName[16];
} LOG_LEVEL_CONF_S;

#ifndef __KERNEL__
/******************************************************************************
** For User Mode : TS_PRINT, TS_ASSERT, TS_TRACE
******************************************************************************/

#define TS_PRINT      printf

/* #ifdef TS_DEBUG */
#ifdef CONFIG_TS_LOG_TRACE_SUPPORT
    /* Using samples:   TS_ASSERT(x>y); */
    #define TS_ASSERT(expr)               \
    do{                                   \
        if (!(expr)) {                    \
            printf("\nASSERT at:\n"       \
                   "  >Function : %s\n"   \
                   "  >Line No. : %d\n"   \
                   "  >Condition: %s\n",  \
                   __FUNCTION__, __LINE__, #expr);\
            assert(0);\
        } \
    }while(0)

    /* Using samples:
    ** TS_TRACE(TS_DBG_DEBUG, TS_ID_CMPI, "Test %d, %s\n", 12, "Test");
    **/
    #define TS_TRACE(level, enModId, fmt...) \
        do {                                 \
            if (level <= TS_DBG_ERR)         \
                fprintf(stderr,##fmt);       \
        }while(0)

#else
    #define TS_ASSERT(expr)
    #define TS_TRACE(level, enModId, fmt...)
#endif

#else
/******************************************************************************
** For Linux Kernel : TS_PRINT, TS_ASSERT, TS_TRACE
******************************************************************************/
#define TS_PRINT      osal_printk

int TS_LOG(TS_S32 level, MOD_ID_E enModId, const char *fmt, ...) __attribute__((format(printf, 3, 4)));

/* #ifdef TS_DEBUG */
#ifdef CONFIG_TS_LOG_TRACE_SUPPORT
    /* Using samples:   TS_ASSERT(x>y); */
    #define TS_ASSERT(expr)               \
    do{                                   \
        if (!(expr)) {                    \
            osal_panic("\nASSERT at:\n"   \
                  "  >Function : %s\n"    \
                  "  >Line No. : %d\n"    \
                  "  >Condition: %s\n",   \
                  __FUNCTION__, __LINE__, #expr);\
        } \
    }while(0)

    /* Using samples:
    ** TS_TRACE(TS_DBG_DEBUG, TS_ID_CMPI, "Test %d, %s\n", 12, "Test");
    **/
    #define TS_TRACE TS_LOG
#else
    #define TS_ASSERT(expr)
    #define TS_TRACE(level, enModId, fmt...)
#endif

#endif /* end of __KERNEL__ */

#if (CONFIG_TS_LOG_TRACE_LEVEL >= TS_DBG_EMERG)

#define TS_EMERG_TRACE(mod, fmt...)\
do {\
    TS_TRACE(TS_DBG_EMERG, mod, fmt);\
} while (0)\

#else
#define TS_EMERG_TRACE(mod, fmt...)
#endif

#if (CONFIG_TS_LOG_TRACE_LEVEL >= TS_DBG_ALERT)

#define TS_ALERT_TRACE(mod, fmt...)\
do {\
    TS_TRACE(TS_DBG_ALERT, mod, fmt);\
} while (0)\

#else
#define TS_ALERT_TRACE(mod, fmt...)
#endif

#if (CONFIG_TS_LOG_TRACE_LEVEL >= TS_DBG_CRIT)

#define TS_CRIT_TRACE(mod, fmt...)\
do {\
    TS_TRACE(TS_DBG_CRIT, mod, fmt);\
} while (0)\

#else
#define TS_CRIT_TRACE(mod, fmt...)
#endif

#if (CONFIG_TS_LOG_TRACE_LEVEL >= TS_DBG_ERR)

#define TS_ERR_TRACE(mod, fmt...)\
do {\
    TS_TRACE(TS_DBG_ERR, mod, fmt);\
} while (0)\

#else
#define TS_ERR_TRACE(mod, fmt...)
#endif

#if (CONFIG_TS_LOG_TRACE_LEVEL >= TS_DBG_WARN)

#define TS_WARN_TRACE(mod, fmt...)\
do {\
    TS_TRACE(TS_DBG_WARN, mod, fmt);\
} while (0)\

#else
#define TS_WARN_TRACE(mod, fmt...)
#endif

#if (CONFIG_TS_LOG_TRACE_LEVEL >= TS_DBG_NOTICE)

#define TS_NOTICE_TRACE(mod, fmt...)\
do {\
    TS_TRACE(TS_DBG_NOTICE, mod, fmt);\
} while (0)\

#else
#define TS_NOTICE_TRACE(mod, fmt...)
#endif

#if (CONFIG_TS_LOG_TRACE_LEVEL >= TS_DBG_INFO)

#define TS_INFO_TRACE(mod, fmt...)\
do {\
    TS_TRACE(TS_DBG_INFO, mod, fmt);\
} while (0)\

#else
#define TS_INFO_TRACE(mod, fmt...)
#endif

#if (CONFIG_TS_LOG_TRACE_LEVEL >= TS_DBG_DEBUG)

#define TS_DEBUG_TRACE(mod, fmt...)\
do {\
    TS_TRACE(TS_DBG_DEBUG, mod, fmt);\
} while (0)\

#else
#define TS_DEBUG_TRACE(mod, fmt...)
#endif


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __TS_DEBUG_H__ */


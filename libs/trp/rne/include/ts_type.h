/***************************************************************************
 *                      COPYRIGHT NOTICE
 *               Copyright 2022 Tsingmicro, Inc.
 *                     All rights reserved.
 ***************************************************************************/

#ifndef __TS_TYPE_H__
#define __TS_TYPE_H__

#ifdef __KERNEL__

#include <linux/types.h>
#else

#include <stdint.h>
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/*--------------------------------------------------------------------------------------------------------------*
 * Defintion of basic data types. The data types are applicable to both the application layer and kernel codes. *
 *--------------------------------------------------------------------------------------------------------------*/
/*************************** Structure Definition ****************************/
/** \addtogroup      Common_TYPE */
/** @{ */ /** <!-- [Common_TYPE] */

typedef unsigned char TS_UCHAR;
typedef unsigned char TS_U8;
typedef unsigned short TS_U16;
typedef unsigned int TS_U32;
typedef unsigned long TS_UL;
typedef TS_UL TS_ULONG;
typedef uintptr_t TS_UINTPTR_T;

typedef char TS_CHAR;
typedef signed char TS_S8;
typedef short TS_S16;
typedef int TS_S32;
typedef long TS_SL;

typedef float TS_FLOAT;
typedef double TS_DOUBLE;

#ifndef _M_IX86
typedef unsigned long long TS_U64;
typedef long long TS_S64;
#else
typedef unsigned __int64 TS_U64;
typedef __int64 TS_S64;
#endif

typedef unsigned long TS_SIZE_T;
typedef unsigned long TS_LENGTH_T;
typedef unsigned long int TS_PHYS_ADDR_T;

typedef unsigned int TS_HANDLE;

/*----------------------------------------------*
 * const defination                             *
 *----------------------------------------------*/
typedef enum {
	TS_FALSE = 0,
	TS_TRUE = 1,
} TS_BOOL;

#ifndef NULL
#define NULL 0L
#endif

#define TS_NULL 0L
#define TS_SUCCESS 0
#define TS_FAILURE (-1)

#define TS_VOID void

typedef unsigned char ts_uchar;
typedef unsigned char ts_u8;
typedef unsigned short ts_u16;
typedef unsigned int ts_u32;
typedef unsigned long ts_ulong;

typedef char ts_char;
typedef signed char ts_s8;
typedef short ts_s16;
typedef int ts_s32;
typedef long ts_slong;

typedef float ts_float;
typedef double ts_double;

typedef void ts_void;

#ifndef _M_IX86
typedef unsigned long long ts_u64;
typedef long long ts_s64;
#else
typedef unsigned __int64 ts_u64;
typedef __int64 ts_s64;
#endif

typedef unsigned long ts_size_t;
typedef unsigned long ts_length_t;

typedef ts_u32 ts_handle;

typedef TS_BOOL ts_bool;
typedef TS_UINTPTR_T ts_uintptr_t;
typedef unsigned long int ts_phys_addr_t;

/** @} */ /** <!-- ==== Structure Definition end ==== */

#define IN
#define OUT

#define TS_UNUSED(x) ((x) = (x))
/*
#define DEBUG_PRT(fmt...)                                                                                              \
	do {                                                                                                           \
		printf("[%s]-%d: ", __FUNCTION__, __LINE__);                                                           \
		printf(fmt);                                                                                           \
	} while (0)
*/
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __TS_TYPE_H__ */

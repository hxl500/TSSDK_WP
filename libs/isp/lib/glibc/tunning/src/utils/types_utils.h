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

#ifndef __TYPES_UTILS_H__
#define __TYPES_UTILS_H__

#include <stdint.h>
#include <stddef.h>
#include <errno.h>

/// @TODO: global_def.h should not be included here ?
#include "global_def.h"

/// Define isp software type acronym

/*
*                    int32_t         uint32_t               uintptr_t
* bare metal GCC     long            unsigned long          unsigned int
* glibc GCC          int             unsigned int           unsigned int
* kernel             int             unsigned int           unsigned long
*/

typedef double   f64;
typedef float    f32;
typedef int16_t  s16;
typedef int32_t  s32;
typedef int64_t  s64;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

/*
* In case of int, there are just two types of it: signed int or unsigned int, and int is always signed int.
* But in case of char, there are 3 types of defination: char, signed char, unsigned char.
* Char could be signed or unsigned according to gcc compiler,
* and we regard it as a different type from signed char / unsigned char.
*/

typedef char     char8;
typedef int8_t   s8;
typedef uint8_t  u8;

typedef intptr_t INTPTR;
typedef uintptr_t UINTPTR;
typedef ptrdiff_t PTRDIFF;

/// Define common type
#ifndef BOOL
#define BOOL u8
#endif

#ifndef TRUE
#define TRUE  1U
#endif

#ifndef FALSE
#define FALSE 0U
#endif

#ifndef NULL
#define NULL  0U
#endif

/// Define ISP function return type
typedef enum
{
    ISP_RET_SUCCESS          = 0,         /// 0 Return Success
    ISP_RET_ERROR            = 0x01,      /// 1 Common Error, stop and restart need
    ISP_RET_INGORED_FAILURE  = 0x02,      /// 2 Calculator failure, ignore return value
    ISP_RET_INVALID          = 0x04,      /// 3 Invalid input parameters
    ISP_RET_NOMEM            = 0x08,      /// 4 Memory allocate and relative error
    ISP_RET_OVERFLOW         = 0x10,      /// 5 Array or memory or type overflow
    ISP_RET_UNINITIALIZED    = 0x20,      /// 6 Uninitialized error
    ISP_RET_TIMEOUT          = 0x40,      /// 7 Timeout error
    ISP_RET_BUSY             = 0x80,      /// 8 Resource busy
} isp_ret_t;

#define IS_RET_NO_FAILURE(ret) (ISP_RET_SUCCESS == ret || ISP_RET_INGORED_FAILURE == ret)

typedef struct
{
    u32 width;
    u32 height;
} dim_t;

typedef struct
{
    s32 min;
    s32 max;
} range_t;

typedef struct
{
    s32 x;
    s32 y;
} coordinate_t;

typedef enum
{
    RES720P30FPS   = 1,
    RES720P60FPS,
    RES1080P30FPS,
    RES1080P60FPS,
    RES1080I50FPS,
    RES1080P50FPS,
    RES960P30FPS,
    RES_TOTAL_SIZE,
} res_mode_t;

#endif

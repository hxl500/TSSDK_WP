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
#ifndef __TRACE_OSUTILS_H__
#define __TRACE_OSUTILS_H__

#include "thread_osutils.h"
#include "fs_osutils.h"
#include "log_utils.h"
#include "types_utils.h"

#ifdef __linux__
#include <signal.h>
#include <execinfo.h>
#endif

#define TRACE_MAX_SIG   (10)
#define TRACE_MAX_DEPTH (64)

#ifdef __linux__
typedef struct
{
    u32              sigNum;
    s32              sigArray[TRACE_MAX_SIG];
    volatile BOOL    sigHandling[TRACE_MAX_SIG];
    char8            stackBody[64*1024];

    stack_t          stack;
    struct sigaction sig;
} trace_os_t;
#endif

/*
* Trace_OS_Register_Signal
*
* @brief Start signal monitor for backtrace dumps
*
* @return void
*/
void Trace_OS_Register_Signal();

#endif

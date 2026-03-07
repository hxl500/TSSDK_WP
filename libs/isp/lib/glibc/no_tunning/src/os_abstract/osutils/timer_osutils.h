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

#ifndef __TIMER_OSUTILS_H__
#define __TIMER_OSUTILS_H__

#include "types_utils.h"
#include "obj_list_utils.h"

#if defined (__linux__)
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <syscall.h>
#include <sys/types.h>

#elif defined(PC_SIMU)
#include <time.h>
#else
#include "xscutimer_ctrl.h"
#endif

////////////////////////////////////////////// Timer ///////////////////////////////////////////////////////
/// typedef timer cb function
typedef void (*timer_callback_func)(void* arg);

typedef struct
{
    timer_callback_func cb;
    u32                 intervalUs;
    void*               arg;
} os_timer_cb_t;

/// define timer struct
typedef struct
{
    u32             minIntervalUs;
    u32             maxIntervalUs;
    volatile u32    curIntervalCnt;

    obj_list_t*     timerCbList;
} os_timer;


/*
* Time_OS_Start_Timer
*
* @brief start timer
*
* @param pTimerCb: point to os_timer_cb_t including timer cb function and interval time
*
* @return void
*
*/
void Time_OS_Start_Timer(os_timer_cb_t* pTimerCb);

/*
* Time_OS_Stop_Timer
*
* @brief start timer
*
* @param pTimerCb: point to os_timer_cb_t including timer cb function and interval time
*
* @return void
*
*/
void Time_OS_Stop_Timer(os_timer_cb_t* pTimerCb);

#endif

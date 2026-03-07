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

#ifndef __TIME_OSUTILS_H__
#define __TIME_OSUTILS_H__

#include "types_utils.h"
#include "log_utils.h"

#if defined (__linux__)
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>

#elif defined(PC_SIMU)
#include <time.h>
#else
#include "xscutimer_ctrl.h"
#include "sleep.h"
#endif

#define MS_2_US(msec) ((u64)(msec) * 1000)

#define SEC_2_US(sec) ((u64)(sec) * 1000000)

#define NS_2_SEC(nsec) ((u64)(nsec) / 1000000000)

#define TS_2_NS(ts) ((u64)(ts.tv_sec) * 1000000000 + (u64)ts.tv_nsec)

#ifndef RUNNING_ON_HAPS
#define DEFAULT_TIMEOUT_MS (2000)
#else
#define DEFAULT_TIMEOUT_MS (3000)
#endif

/// define isp time struct
typedef struct __isp_datetime
{
    u32 year;
    u32 mon;
    u32 day;
    u32 hour;
    u32 min;
    u32 sec;
    u64 usec;
} isp_datetime_t;

/*
* Time_OS_Msleep
*
* @brief sleep with certain milliseconds
*
* @param ms: milliseconds
*
* @return void
*
*/
void Time_OS_Msleep(u32 ms);

/*
* Time_OS_Usleep
*
* @brief sleep with certain mirroseconds
*
* @param us: mirroseconds
*
* @return void
*
*/
void Time_OS_Usleep(u32 us);

/*
* Time_OS_Get_Cur_Datetime
*
* @brief Get current date and time
*
* @param void
*
* @return isp_datetime_t
*
*/
isp_datetime_t Time_OS_Get_Cur_Datetime();

/*
* Time_OS_Get_Cur_Timestamp
*
* @brief Get current timestamp in us
*
* @param void
*
* @return current timestamp
*
*/
u64 Time_OS_Get_Cur_Timestamp();


#endif

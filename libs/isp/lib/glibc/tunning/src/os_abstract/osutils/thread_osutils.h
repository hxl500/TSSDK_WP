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

#ifndef __THREAD_OSUTILS_H__
#define __THREAD_OSUTILS_H__

#include "types_utils.h"
#include "timer_osutils.h"
#include "obj_queue_utils.h"

#ifdef __linux__
#include <pthread.h>

#define gettid() (syscall(__NR_gettid))
typedef pthread_t       thread_os_id_t;
typedef pthread_attr_t  thread_os_attr_t;
typedef pthread_mutex_t thread_os_mutex_t;
typedef pthread_cond_t  thread_os_cond_t;

#else

#define gettid() (1)

typedef u32             thread_os_id_t;
typedef u64             thread_os_attr_t;
typedef u32             thread_os_mutex_t;
typedef u32             thread_os_cond_t;

#endif

typedef void (*thread_os_task_func)(void * argv);
typedef void (*thread_os_trigger_func)(void * argv);

typedef struct
{
    char8                   name[32];
    thread_os_task_func     loop;
    BOOL                    blocking;
    u32                     sleepUs;
    u32                     msgQNum;
    void*                   arg;
} thread_os_create_info_t;

typedef struct
{
    /// params configured by user
    char8                   name[32];
    thread_os_task_func     loop;
    BOOL                    blocking;
    u32                     sleepUs;
    u32                     msgQNum;
    void*                   arg;

    /// params configured internal
    thread_os_id_t          threadId;
    u32                     tid;
    /* const */ thread_os_attr_t  attr;
    volatile BOOL           isRunning;
    os_timer_cb_t           timerCb;
    obj_queue_t           **msgQ;
    thread_os_mutex_t       msglock;
    thread_os_mutex_t       threadlock;
    thread_os_cond_t        cond;
} thread_os_t;


/*
* Thread_OS_Mutex_Lock
*
* @brief thread lock
*
* @param lock: point to thread_os_mutex_t
*
* @return void
*/
void Thread_OS_Mutex_Lock(thread_os_mutex_t* lock);

/*
* Thread_OS_Mutex_Unlock
*
* @brief thread unlock
*
* @param lock: point to thread_os_mutex_t
*
* @return void
*/
void Thread_OS_Mutex_Unlock(thread_os_mutex_t* lock);

/*
* Thread_OS_Mutex_Init
*
* @brief thread lock init
*
* @param lock: point to thread_os_mutex_t
*
* @return void
*/
void Thread_OS_Mutex_Init(thread_os_mutex_t* lock);

/*
* Thread_OS_Mutex_Destroy
*
* @brief thread lock destroy
*
* @param lock: point to thread_os_mutex_t
*
* @return void
*/
void Thread_OS_Mutex_Destroy(thread_os_mutex_t* lock);


/*
* Thread_OS_Cond_Init
*
* @brief thread cond init
*
* @param cond: point to thread_os_cond_t
*
* @return void
*/
void Thread_OS_Cond_Init(thread_os_cond_t* cond);

/*
* Thread_OS_Cond_Destroy
*
* @brief thread cond destroy
*
* @param cond: point to thread_os_cond_t
*
* @return void
*/
void Thread_OS_Cond_Destroy(thread_os_cond_t* cond);

/*
* Thread_OS_Cond_Timedwait
*
* @brief Wait cond with certain time
*
* @param cond: point to thread_os_cond_t
*
* @param lock: point to thread_os_mutex_t
*
* @param waitUs: timeout by us, < SEC_2_US(1)
*
* @return 0 if success, other if failed
*/
s32 Thread_OS_Cond_Timedwait(thread_os_cond_t* cond, thread_os_mutex_t* lock, u32 waitUs);

/*
* Thread_OS_Cond_Signal
*
* @brief Send signal to wait up thread woth cond and lock
*
* @param cond: point to thread_os_cond_t
*
* @return void
*/
void Thread_OS_Cond_Signal(thread_os_cond_t* cond);


/*
* Thread_OS_Trigger_Loop
*
* @brief Wait up thread loop
*
* @param threadHandle: thread handle
*
* @param trigger: trigger cb
*
* @param arg: trigger param
*
* @return void
*/
void Thread_OS_Trigger_Loop(thread_os_t* threadHandle, thread_os_trigger_func trigger, void* arg);

/*
* Thread_OS_Msg_Enqueue
*
* @brief Enqueue thread message
*
* @param threadHandle: thread handle
*
* @param pData: data to enqueue
*
* @param msgQIdx: msg Q index
*
* @return void
*/
void Thread_OS_Msg_Enqueue(thread_os_t* threadHandle, void* pData, u32 msgQIdx);

/*
* Thread_OS_Msg_Try_Dequeue
*
* @brief Try to dequeue thread message
*
* @param threadHandle: thread handle
*
* @param msgQIdx: msg Q index
*
* @return data try dequeued
*/
void* Thread_OS_Msg_Try_Dequeue(thread_os_t* threadHandle, u32 msgQIdx);

/*
* Thread_OS_Msg_Dequeue
*
* @brief Dequeue thread message
*
* @param threadHandle: thread handle
*
* @param msgQIdx: msg Q index
*
* @return data dequeued
*/
void* Thread_OS_Msg_Dequeue(thread_os_t* threadHandle, u32 msgQIdx);

/*
* Thread_OS_Msg_Length
*
* @brief Acquire length of msg Q
*
* @param threadHandle: thread handle
*
* @param msgQIdx: msg Q index
*
* @return length of msg Q
*/
u32 Thread_OS_Msg_Length(thread_os_t* threadHandle, u32 msgQIdx);

/*
* Thread_OS_Create
*
* @brief Create thread function
*
* @param threadCreateInfo: thread create onfo
*
* @return thread handle
*/
thread_os_t* Thread_OS_Create(thread_os_create_info_t     * threadCreateInfo);

/*
* Thread_OS_Destroy
*
* @brief Destroy thread function
*
* @param pThreadHandle: point to thread handle
*
* @return void
*/
void Thread_OS_Destroy(thread_os_t** pThreadHandle);


#endif

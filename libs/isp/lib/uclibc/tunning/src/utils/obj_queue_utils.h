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

#ifndef __OBJ_QUEUE_UTILS_H__
#define __OBJ_QUEUE_UTILS_H__

#include "types_utils.h"
#include "obj_list_utils.h"

typedef struct __obj_queue obj_queue_t;

typedef void (*obj_queue_push_func)(obj_queue_t* thiz, void* pData);
typedef void* (*obj_queue_pop_func)(obj_queue_t* thiz);
typedef void* (*obj_queue_try_pop_func)(obj_queue_t* thiz);
typedef BOOL (*obj_queue_find_item_func)(obj_queue_t* thiz,
                                                void* pSrcData,
                                                obj_list_node_match_t type,
                                                void** ppDstData);

/// ISP Queue descriptor
typedef struct __obj_queue
{
    obj_list_node_t*            pHead;
    obj_list_node_t*            pTail;
    u32                         length;

    obj_queue_push_func         Push;
    obj_queue_pop_func          Pop;
    obj_queue_try_pop_func      Try_Pop;
    obj_queue_find_item_func    Find;
} obj_queue_t;

/*
* Obj_Queue_Create
*
* @brief Create ISP queue
*
* @return pointer to obj_queue_t
*/
obj_queue_t* Obj_Queue_Create();

/*
* Obj_Queue_Destroy
*
* @brief Destroy ISP queue
*
* @param ppQueue: dimensional pointer to obj_queue_t
*
* @return void
*/
void  Obj_Queue_Destroy(obj_queue_t** ppQueue);

#endif

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

#ifndef __LIST_UTILS_H__
#define __LIST_UTILS_H__

/// @brief Doulbe List for frame msg control
typedef struct _double_list_t
{
    struct _double_list_t *next, *prev;
}list_t;

/// @brief Initial double list
static inline void List_Init(list_t* list)
{
    list->prev = list;
    list->next = list;
}

/// @brief add entry to list tail
static inline void List_Add_Tail(list_t* head, list_t* entry)
{
    entry->prev      = head->prev;
    entry->next      = head;
    head->prev->next = entry;
    head->prev       = entry;
}

/// @brief delete list node
static inline list_t* List_Del_Node(list_t *node)
{
    node->next->prev = node->prev;
    node->prev->next = node->next;
    List_Init(node);
    return node;
}

/// @brief return tail list node
static inline list_t* List_Del_Tail(list_t *head)
{
    head             = head->prev;
    head->next->prev = head->prev;
    head->prev->next = head->next;
    List_Init(head);
    return head;
}

/// @brief move list node from source list to dest list
static inline void List_Node_Move(list_t* node, list_t* destListHead)
{
    node->prev->next = node->next;
    node->next->prev = node->prev;
    List_Add_Tail(destListHead, node);
}

/// @brief return true is list is empty
static inline BOOL List_Empty(const list_t* listHead)
{
    return listHead->next == listHead || listHead->prev == listHead;
}

/// Return Structure point by structure member name and member pointer
#define STRUCTURE_PTR(ptr, type, member)\
    ((type*)((char*)(ptr)-(char*)&((type*)0)->member))

/// Return frame message structure point
#define MSG_PTR(listPtr) STRUCTURE_PTR(listPtr, frame_msg_t, linked)

#endif

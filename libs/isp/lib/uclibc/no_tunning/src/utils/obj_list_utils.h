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

#ifndef __OBJ_LIST_UTILS_H__
#define __OBJ_LIST_UTILS_H__

#include "types_utils.h"
#include "mem_osutils.h"

/// List Node
typedef struct __obj_list_node obj_list_node_t;

/// Define node struct
struct __obj_list_node
{
    void*                pData;
    obj_list_node_t*     pPre;
    obj_list_node_t*     pNext;
};

/// Define node match types
typedef enum __obj_list_node_match
{
    ISP_lIST_NODE_MATCH_POINTER,
    ISP_lIST_NODE_MATCH_INT32,
    ISP_lIST_NODE_MATCH_INT64,
} obj_list_node_match_t;

/*
* Obj_List_Node_Create
*
* @brief Create node with data
*
* @param pData: pointer to void, node data
*
* @return pointer to obj_list_node_t
*/
obj_list_node_t* Obj_List_Node_Create(void* pData);

/*
* Obj_List_Node_Delete
*
* @brief Delete node from node list
*
* @param pNode: pointer to obj_list_node_t
*
* @return void
*/
void Obj_List_Node_Delete(obj_list_node_t* pNode);

/*
* Obj_List_Node_Delete_All
*
* @brief Delete all nodes from node list
*
* @param pHead: pointer to obj_list_node_t
*
* @return void
*/
void Obj_List_Node_Delete_All(obj_list_node_t* pHead);

/*
* Obj_List_Node_Append
*
* @brief Append node to node list according to data
*
* @param pNode: pointer to obj_list_node_t
*
* @param pData: pointer to void, node data
*
* @return pointer of obj_list_node_t, header of list
*/
obj_list_node_t* Obj_List_Node_Append(obj_list_node_t* pNode, void* pData);

/*
* Obj_List_Node_Remove
*
* @brief Remove node according to data from node list
*
* @param pNode: pointer to obj_list_node_t
*
* @param pData: pointer to void, node data
*
* @return pointer of obj_list_node_t, header of list
*/
obj_list_node_t* Obj_List_Node_Remove(obj_list_node_t* pNode, void* pData);


/*
* Obj_List_Node_Find_Item
*
* @brief Find node data from list
*
* @param pNode: pointer to obj_list_node_t
*
* @param pSrcData: pointer to void, data used to search
*
* @param type: type of pSrcData
*
* @param ppDstData: dimensional pointer to svoid, data used to save result
*
* @return TRUE if found, FALSE if failed
*/
BOOL Obj_List_Node_Find_Item(obj_list_node_t* pNode,
                                    void* pSrcData,
                                    obj_list_node_match_t type,
                                    void** ppDstData);




/// ISP List Abstract

typedef struct __obj_list obj_list_t;

typedef obj_list_t* (*obj_list_append_func)(obj_list_t* thiz, void* pData);

typedef obj_list_t* (*obj_list_remove_func)(obj_list_t* thiz, void* pData);

typedef obj_list_t* (*obj_list_clear_func)(obj_list_t* thiz);

typedef BOOL (*obj_list_find_item_func)(obj_list_t* thiz,
                                              void* pSrcData,
                                              obj_list_node_match_t type,
                                              void** ppDstData);

/// ISP List descriptor
struct __obj_list
{
    obj_list_node_t*           pHead;
    u32                        length;

    obj_list_append_func       Append;
    obj_list_remove_func       Remove;
    obj_list_clear_func        Clear;
    obj_list_find_item_func    Find;
};

/*
* Obj_List_Create
*
* @brief Create ISP list
*
* @return pointer to obj_list_t
*/
obj_list_t* Obj_List_Create();


/*
* Obj_List_Destroy
*
* @brief Destroy ISP list
*
* @param pList: dimensional pointer to obj_list_t
*
* @return void
*/
void Obj_List_Destroy(obj_list_t** ppList);

/// OBJ list traverse MACRO
#define OBJ_LIST_TRAVERSE_DATA(list, type, p)                     \
    type p = NULL;                                                \
	obj_list_node_t* pNode;              \
	obj_list_node_t* pNode_next;         \
    for ( pNode= (list)->pHead, pNode_next=(pNode)?pNode->pNext:NULL; \
         (NULL != pNode) && (NULL != (p = (type)pNode->pData));  \
         pNode = pNode_next, pNode_next=(pNode)?pNode->pNext:NULL)


#endif

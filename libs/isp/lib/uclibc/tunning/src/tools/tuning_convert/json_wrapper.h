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
 * Copyright (C) 2020-2021 Arm Technology (China) Co. Ltd. All rights reserved.
 *
 ***********************************************************************************************/

#ifndef __JSON_WRAPPER_H__
#define __JSON_WRAPPER_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "types_utils.h"

#define CJSON_ENABLE 1

#ifdef CJSON_ENABLE
#include "cJSON.h"
typedef cJSON json;

#else

typedef struct
{
    u32 reserved;
} custom_json_t;

typedef custom_json_t json;

#endif

typedef enum
{
    JSON_TUNING,
    JSON_CALIBRATION,
} json_type_t;

/*
* Json_Get_Item
*
* @brief Get json item according to variable name
*
* @param pJson: pointer to json
*
* @param sName: variable name
*
* @return pointer to json
*
*/
json* Json_Get_Item(const json *pJson, const char8* sName);

const char8 *Json_Get_String(const json *object, const char8 *name);

/*
* Json_Get_Int_Val
*
* @brief Get int value according to variable name
*
* @param pJson: pointer to json
*
* @param sName: variable name
*
* @return int value
*
*/
s32 Json_Get_Int_Val(const json *pJson, const char8* sName);

/*
* Json_Get_Array_Item
*
* @brief Get array item according to array index
*
* @param pJson: pointer to json
*
* @param index: array index
*
* @return pointer to json
*
*/
json* Json_Get_Array_Item(const json *pJson, s32 index);

/*
* Json_Get_Array_Int_Val
*
* @brief Get int value according to array index
*
* @param pJson: pointer to json
*
* @param index: array index
*
* @return int value
*
*/
s32 Json_Get_Array_Int_Val(const json *pJson, s32 index);

/*
* Json_Get_Array_Size
*
* @brief Get array size from json array item
*
* @param pJson: pointer to json
*
* @return size of array
*
*/
size_t Json_Get_Array_Size(const json *pJson);

/*
* Json_Get_Error_String
*
* @brief Get error string info to check wheter parsed failed
*
* @param pJson: pointer to json
*
* @return error string (NULL if no error)
*
*/
const char8* Json_Get_Error_String (const json *pJson);

/*
* Json_Parse
*
* @brief Parse string to json
*
* @param text: string input
*
* @return pointer to json
*
*/
json* Json_Parse(const char8* text);

BOOL Json_Add_String_To_Object(json* const object, const char * const name, const char * const string);

/*
* Json_Add_Int_To_Object
*
* @brief Add int variable to parent json object
*
* @param pParent: pointer to parent json
*
* @param sName: new object name
*
* @param num: int variable
*
* @return TRUE if success, FALSE if failed
*
*/
BOOL Json_Add_Int_To_Object(json* const pParent, const char * const sName, s32 num);

/*
* Json_Add_Int_Array_To_Array
*
* @brief Add array to parent json array
*
* @param pParent: pointer to parent json
*
* @param pArray: pointer to array
*
* @param size: array size
*
* @return TRUE if success, FALSE if failed
*
*/
BOOL Json_Add_Int_Array_To_Array(json* const pParent, u32* pArray, size_t size);

/*
* Json_Add_Int_Array_To_Object
*
* @brief Add array to parent json object
*
* @param pParent: pointer to parent json
*
* @param sName: new object name
*
* @param pArray: pointer to array
*
* @param size: array size
*
* @return TRUE if success, FALSE if failed
*
*/
BOOL Json_Add_Int_Array_To_Object(json* const pParent, const char * const sName, u32* pArray, size_t size);

/*
* Json_Add_Item_To_Array
*
* @brief Add object or array item to parent json object
*
* @param pParent: pointer to parent json
*
* @param pChild: pointer to child json
*
* @return TRUE if success, FALSE if failed
*
*/
BOOL Json_Add_Item_To_Array(json* const pParent, json* const pChild);

/*
* Json_Add_Item_To_Object
*
* @brief Add object or array item to parent json object
*
* @param pParent: pointer to parent json
*
* @param sName: new item name
*
* @param pChild: pointer to child json
*
* @return TRUE if success, FALSE if failed
*
*/
BOOL Json_Add_Item_To_Object(json* const pParent, const char * const sName, json* const pChild);

/*
* Json_Create_Array_To_Object
*
* @brief Create array to parent json object
*
* @param pParent: pointer to parent json
*
* @param sName: new array name
*
* @return pointer to json
*
*/
json* Json_Create_Array_To_Object(json* const pParent, const char * const sName);

/*
* Json_Create_Object_To_Array
*
* @brief Create object to parent json array
*
* @param pParent: pointer to parent json
*
* @return pointer to json
*
*/
json* Json_Create_Object_To_Array(json* const pParent);

/*
* Json_Create_Object_To_Object
*
* @brief Create object to parent json object
*
* @param pParent: pointer to parent json
*
* @param sName: new object name
*
* @return pointer to json
*
*/
json* Json_Create_Object_To_Object(json* const pParent, const char * const sName);


/*
* Json_Create_Array
*
* @brief Create json array
*
* @param void
*
* @return pointer to json
*
*/
json* Json_Create_Array();

/*
* Json_Create_Object
*
* @brief Create json object
*
* @param void
*
* @return pointer to json
*
*/
json* Json_Create_Object();

/*
* Json_Delete
*
* @brief Destroy json
*
* @param ppJson: dimensional pointer to json
*
* @return void
*
*/
void Json_Delete(json** ppJson);

/*
* Json_Print
*
* @brief Convert json to string
*
* @param pJson: pointer to json
*
* @return parsed string
*
*/
char8* Json_Print(const json* pJson);

/*
* Json_Set_Err_Status
*
* @brief Set json err status, called when init or error occurs
*
* @param pJson: pointer to json
*
* @return parsed string
*
*/
void Json_Set_Err_Status(BOOL error);



#endif

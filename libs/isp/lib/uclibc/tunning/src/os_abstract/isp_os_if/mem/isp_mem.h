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

#ifndef __ISP_MEM_H__
#define __ISP_MEM_H__

#include "types_utils.h"
#include "memory_map.h"
#include "isp_buf_def.h"


/*
* Isp_Get_Buffer_Address
*
* @brief Get ISP Buffer Address infomation
*
* @params contextId: camera index
*
* @return point to buffer address struct
*
*/
const isp_buffer_address_t* Isp_Get_Buffer_Address(u32 contextId);

void Isp_Update_Buffer_Address(u32 contextId, u64 cfg);

#endif

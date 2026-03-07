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

#ifndef __TUNING_ISP_COMMON_H__
#define __TUNING_ISP_COMMON_H__

#include "tune_utils.h"

#if (defined(HW_BUILD_FOR_I5)       ||\
     defined(HW_BUILD_FOR_I5_R1P0)  ||\
     defined(HW_BUILD_FOR_I3)       ||\
     defined(HW_BUILD_FOR_CUST0))

#include "tune_isp_octob.h"

#elif defined (HW_BUILD_FOR_TITIAN)

#include "tune_isp_titian.h"

#endif

#endif

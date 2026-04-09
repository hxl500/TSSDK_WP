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

#ifndef __CSI_COMMON_H__
#define __CSI_COMMON_H__

#include "types_utils.h"
#include "sensor_common.h"

/// Define data type
typedef enum
{
    MIPI_CLK_24M = 0,   // Mipi clock 24M HZ
    MIPI_CLK_37M,       // Mipi clock 37.125M HZ
} mipi_clk_type_t;

/// Define raw bit type
typedef enum
{
    SENSOR_RAW8    = 0,
    SENSOR_RAW10   = 1,
    SENSOR_RAW12   = 2,
    SENSOR_RAW14   = 3
} raw_bits_type_t;

/// Define raw bit type
typedef enum
{
    MIPI_1LANE_EN   = 0x1,
    MIPI_2LANES_EN  = 0x3,
    MIPI_4LANES_EN  = 0xf,
} lane_mask_type_t;


typedef struct
{
    u8                 mipiChannel;
    raw_bits_type_t    rawBits;
    lane_mask_type_t   laneMask;
    u8                 laneMap;
    mipi_clk_type_t    mipiCLk;
    u32 resv;
} csi_drv_params_t;

typedef csi_drv_params_t* (*load_csi_drv_func)();

#endif

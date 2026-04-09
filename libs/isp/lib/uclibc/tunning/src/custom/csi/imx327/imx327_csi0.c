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

#include "imx327_csi0.h"

static csi_drv_params_t imx327_csi0_params =
{
    .mipiChannel   = 0,
    .rawBits       = SENSOR_RAW12,
    .laneMask      = MIPI_4LANES_EN,
    .laneMap       = 0xff,
    .mipiCLk       = MIPI_CLK_37M,

};

csi_drv_params_t * Load_Imx327_Csi0_Driver(void)
{
    return &imx327_csi0_params;
}

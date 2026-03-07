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

#ifndef __TUNE_ISP_GAIN_RANGE_H__
#define __TUNE_ISP_GAIN_RANGE_H__

/***************************************************
*                 ISP gain range                    *
 ***************************************************/
typedef struct
{
    u32 maxDigitalGain;         /// Q12 1x: 0x1000
    u32 minDigitalGain;         /// Q12 1x: 0x1000
    u32 maxRChannelGain;        /// Q8  1x: 0x100
    u32 minRChannelGain;        /// Q8  1x: 0x100
    u32 maxGChannelGain;        /// Q8  1x: 0x100
    u32 minGChannelGain;        /// Q8  1x: 0x100
    u32 maxBChannelGain;        /// Q8  1x: 0x100
    u32 minBChannelGain;        /// Q8  1x: 0x100
} tune_isp_gain_range_t;

#endif

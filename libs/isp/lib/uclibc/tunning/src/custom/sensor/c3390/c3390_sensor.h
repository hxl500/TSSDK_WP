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

#ifndef __c3390_SENSOR_H__
#define __c3390_SENSOR_H__

#include "sensor_common.h"
#include "log_utils.h"

isp_ret_t c3390_Fill_Exposure_Reg(sensor_apply_exp_param_t * pExpParam, drv_regs_array_info_t * pRegInfo);
//c3390 All-pixel scan CSI-2_2lane 24Mhz AD:10bit Output:10bit 660Mbps Master Mode 25.00fps Integration Time: Xx.xxms Gain:XdB vts = 1650 row time=xx.xxus
static drv_sensor_reg_info_t  c3390_4mp_setting[] =
{	
#if 1
	{ .regAddr=0x32a9, .regData= 0x10, .delayUs=0 },
	{ .regAddr=0x32aa, .regData= 0x06, .delayUs=0 },
	{ .regAddr=0x32ab, .regData= 0x08, .delayUs=0 },
	{ .regAddr=0x32ac, .regData= 0xff, .delayUs=0 },
	{ .regAddr=0x3291, .regData= 0x04, .delayUs=0 },
	{ .regAddr=0x3211, .regData= 0x10, .delayUs=0 },
	{ .regAddr=0x3215, .regData= 0x1f, .delayUs=0 },
	{ .regAddr=0x3216, .regData= 0x20, .delayUs=0 },
	{ .regAddr=0x3217, .regData= 0x20, .delayUs=0 },
	{ .regAddr=0x3286, .regData= 0x77, .delayUs=0 },
	{ .regAddr=0x3287, .regData= 0x4e, .delayUs=0 },
	{ .regAddr=0x3280, .regData= 0x8b, .delayUs=0 },
	{ .regAddr=0x3290, .regData= 0xe7, .delayUs=0 },
	{ .regAddr=0x0340, .regData= 0x0a, .delayUs=0 },
	{ .regAddr=0x0341, .regData= 0x60, .delayUs=0 },
	{ .regAddr=0x0342, .regData= 0x0b, .delayUs=0 },
	{ .regAddr=0x0343, .regData= 0x20, .delayUs=0 },
	{ .regAddr=0x034b, .regData= 0x1f, .delayUs=0 },
	{ .regAddr=0x32c8, .regData= 0x22, .delayUs=0 },
	{ .regAddr=0x32ca, .regData= 0x22, .delayUs=0 },
	{ .regAddr=0x3d1f, .regData= 0x22, .delayUs=0 },
	{ .regAddr=0x3d21, .regData= 0x22, .delayUs=0 },
	{ .regAddr=0x3885, .regData= 0x22, .delayUs=0 },
	{ .regAddr=0x3607, .regData= 0x22, .delayUs=0 },
	{ .regAddr=0x3605, .regData= 0x22, .delayUs=0 },
	{ .regAddr=0x3288, .regData= 0x10, .delayUs=0 }, 
	{ .regAddr=0x0401, .regData= 0x3b, .delayUs=0 },
	{ .regAddr=0x0403, .regData= 0x00, .delayUs=0 },
	{ .regAddr=0x3584, .regData= 0x02, .delayUs=0 },
	{ .regAddr=0x3087, .regData= 0x87, .delayUs=0 },
	{ .regAddr=0x3180, .regData= 0x20, .delayUs=0 },
	{ .regAddr=0x3187, .regData= 0x04, .delayUs=0 },
	{ .regAddr=0x3187, .regData= 0x14, .delayUs=0 },
	{ .regAddr=0x3114, .regData= 0x4a, .delayUs=0 },
	{ .regAddr=0x3115, .regData= 0x00, .delayUs=0 },
	{ .regAddr=0x3126, .regData= 0x04, .delayUs=0 },
	{ .regAddr=0x3c01, .regData= 0x17, .delayUs=0 },
	{ .regAddr=0x0202, .regData= 0x02, .delayUs=0 },
	{ .regAddr=0x0203, .regData= 0x30, .delayUs=0 },
	{ .regAddr=0x0205, .regData= 0x00, .delayUs=0 },
	{ .regAddr=0x3584, .regData= 0x22, .delayUs=0 },
	{ .regAddr=0x3108, .regData= 0xcf, .delayUs=0 },
	{ .regAddr=0x3123, .regData= 0x4c, .delayUs=0 },
	{ .regAddr=0x3112, .regData= 0xe4, .delayUs=0 },
	{ .regAddr=0x3113, .regData= 0xfb, .delayUs=0 },
	{ .regAddr=0x300b, .regData= 0x10, .delayUs=0 },
	{ .regAddr=0x3181, .regData= 0x50, .delayUs=0 },
	{ .regAddr=0x3182, .regData= 0x40, .delayUs=0 },
	{ .regAddr=0x3211, .regData= 0x10, .delayUs=0 },
	{ .regAddr=0x3c18, .regData= 0x20, .delayUs=0 },
	{ .regAddr=0x308c, .regData= 0x70, .delayUs=0 },
	{ .regAddr=0x308d, .regData= 0x71, .delayUs=0 },
	{ .regAddr=0x3403, .regData= 0x00, .delayUs=0 },
	{ .regAddr=0x3407, .regData= 0x06, .delayUs=0 },
	{ .regAddr=0x3410, .regData= 0x04, .delayUs=0 },
	{ .regAddr=0x3414, .regData= 0x06, .delayUs=0 },
	{ .regAddr=0x3600, .regData= 0x08, .delayUs=0 },
	{ .regAddr=0x3500, .regData= 0x10, .delayUs=0 },
	{ .regAddr=0x3584, .regData= 0x02, .delayUs=0 },
	{ .regAddr=0x3411, .regData= 0x08, .delayUs=0 },
	{ .regAddr=0x3412, .regData= 0x09, .delayUs=0 },
	{ .regAddr=0x3415, .regData= 0x01, .delayUs=0 },
	{ .regAddr=0x3416, .regData= 0x01, .delayUs=0 },
	{ .regAddr=0xe060, .regData= 0x31, .delayUs=0 },
	{ .regAddr=0xe061, .regData= 0x12, .delayUs=0 },
	{ .regAddr=0xe062, .regData= 0xe4, .delayUs=0 },
	{ .regAddr=0xe06c, .regData= 0x31, .delayUs=0 },
	{ .regAddr=0xe06d, .regData= 0x12, .delayUs=0 },
	{ .regAddr=0xe06e, .regData= 0xe4, .delayUs=0 },
	{ .regAddr=0x0400, .regData= 0x47, .delayUs=0 },
	{ .regAddr=0x0404, .regData= 0x05, .delayUs=0 },
	{ .regAddr=0x0405, .regData= 0x00, .delayUs=0 },
	{ .regAddr=0x0406, .regData= 0x05, .delayUs=0 },
	{ .regAddr=0x0407, .regData= 0x00, .delayUs=0 },
	{ .regAddr=0xe000, .regData= 0x31, .delayUs=0 },
	{ .regAddr=0xe001, .regData= 0x0a, .delayUs=0 },
	{ .regAddr=0xe002, .regData= 0x88, .delayUs=0 },
	{ .regAddr=0xe003, .regData= 0x31, .delayUs=0 },
	{ .regAddr=0xe004, .regData= 0x0b, .delayUs=0 },
	{ .regAddr=0xe005, .regData= 0x88, .delayUs=0 },
	{ .regAddr=0xe006, .regData= 0x31, .delayUs=0 },
	{ .regAddr=0xe007, .regData= 0x0c, .delayUs=0 },
	{ .regAddr=0xe008, .regData= 0x88, .delayUs=0 },
	{ .regAddr=0xe009, .regData= 0x31, .delayUs=0 },
	{ .regAddr=0xe00a, .regData= 0x0d, .delayUs=0 },
	{ .regAddr=0xe00b, .regData= 0x88, .delayUs=0 },
	{ .regAddr=0xe00c, .regData= 0x32, .delayUs=0 },
	{ .regAddr=0xe00d, .regData= 0xac, .delayUs=0 },
	{ .regAddr=0xe00e, .regData= 0xff, .delayUs=0 },
	{ .regAddr=0xe00f, .regData= 0x31, .delayUs=0 },
	{ .regAddr=0xe010, .regData= 0x87, .delayUs=0 },
	{ .regAddr=0xe011, .regData= 0x04, .delayUs=0 },
	{ .regAddr=0xe030, .regData= 0x31, .delayUs=0 },
	{ .regAddr=0xe031, .regData= 0x0a, .delayUs=0 },
	{ .regAddr=0xe032, .regData= 0x90, .delayUs=0 },
	{ .regAddr=0xe033, .regData= 0x31, .delayUs=0 },
	{ .regAddr=0xe034, .regData= 0x0b, .delayUs=0 },
	{ .regAddr=0xe035, .regData= 0x90, .delayUs=0 },
	{ .regAddr=0xe036, .regData= 0x31, .delayUs=0 },
	{ .regAddr=0xe037, .regData= 0x0c, .delayUs=0 },
	{ .regAddr=0xe038, .regData= 0x90, .delayUs=0 },
	{ .regAddr=0xe039, .regData= 0x31, .delayUs=0 },
	{ .regAddr=0xe03a, .regData= 0x0d, .delayUs=0 },
	{ .regAddr=0xe03b, .regData= 0x90, .delayUs=0 },
	{ .regAddr=0xe03c, .regData= 0x32, .delayUs=0 },
	{ .regAddr=0xe03d, .regData= 0xac, .delayUs=0 },
	{ .regAddr=0xe03e, .regData= 0xff, .delayUs=0 },
	{ .regAddr=0xe03f, .regData= 0x31, .delayUs=0 },
	{ .regAddr=0xe040, .regData= 0x87, .delayUs=0 },
	{ .regAddr=0xe041, .regData= 0x14, .delayUs=0 },
	{ .regAddr=0x3500, .regData= 0x00, .delayUs=0 },
	{ .regAddr=0x310a, .regData= 0x88, .delayUs=0 },
	{ .regAddr=0x310b, .regData= 0x88, .delayUs=0 },
	{ .regAddr=0x310c, .regData= 0x88, .delayUs=0 },
	{ .regAddr=0x310d, .regData= 0x88, .delayUs=0 },
	{ .regAddr=0x0202, .regData= 0x05, .delayUs=0 },
	{ .regAddr=0x0203, .regData= 0x30, .delayUs=0 },
	{ .regAddr=0x3584, .regData= 0x22, .delayUs=0 },
	{ .regAddr=0x0309, .regData= 0x10, .delayUs=0 },
	{ .regAddr=0x0307, .regData= 0x47, .delayUs=0 },
	{ .regAddr=0x3517, .regData= 0x47, .delayUs=0 },
	{ .regAddr=0x3805, .regData= 0x07, .delayUs=0 },
	{ .regAddr=0x3806, .regData= 0x05, .delayUs=0 },
	{ .regAddr=0x3807, .regData= 0x05, .delayUs=0 },
	{ .regAddr=0x3808, .regData= 0x15, .delayUs=0 },
	{ .regAddr=0x3809, .regData= 0x75, .delayUs=0 },
	{ .regAddr=0x380a, .regData= 0x6c, .delayUs=0 },
	{ .regAddr=0x380b, .regData= 0xa8, .delayUs=0 },
	{ .regAddr=0x0101, .regData= 0x02, .delayUs=0 },
	{ .regAddr=0x0100, .regData= 0x00, .delayUs=0 },
#else
	{ .regAddr=0x32a9, .regData=0x10, .delayUs=0 },
	{ .regAddr=0x32aa, .regData=0x06, .delayUs=0 },
	{ .regAddr=0x32ab, .regData=0x08, .delayUs=0 },
	{ .regAddr=0x32ac, .regData=0xff, .delayUs=0 },
	{ .regAddr=0x3291, .regData=0x04, .delayUs=0 },
	{ .regAddr=0x3211, .regData=0x10, .delayUs=0 },
	{ .regAddr=0x3215, .regData=0x1f, .delayUs=0 },
	{ .regAddr=0x3216, .regData=0x20, .delayUs=0 },
	{ .regAddr=0x3217, .regData=0x20, .delayUs=0 },
	{ .regAddr=0x3286, .regData=0x77, .delayUs=0 },
	{ .regAddr=0x3287, .regData=0x4e, .delayUs=0 },
	{ .regAddr=0x3280, .regData=0x8b, .delayUs=0 },
	{ .regAddr=0x3290, .regData=0xe7, .delayUs=0 },
	{ .regAddr=0x0342, .regData=0x0b, .delayUs=0 },
	{ .regAddr=0x0343, .regData=0x20, .delayUs=0 },
	{ .regAddr=0x034b, .regData=0x1f, .delayUs=0 },
	{ .regAddr=0x32c8, .regData=0x22, .delayUs=0 },
	{ .regAddr=0x32ca, .regData=0x22, .delayUs=0 },
	{ .regAddr=0x3d1f, .regData=0x22, .delayUs=0 },
	{ .regAddr=0x3d21, .regData=0x22, .delayUs=0 },
	{ .regAddr=0x3885, .regData=0x22, .delayUs=0 },
	{ .regAddr=0x3607, .regData=0x22, .delayUs=0 },
	{ .regAddr=0x3605, .regData=0x22, .delayUs=0 },
	{ .regAddr=0x3288, .regData=0x10, .delayUs=0 }, 
	{ .regAddr=0x0401, .regData=0x3b, .delayUs=0 },
	{ .regAddr=0x0403, .regData=0x00, .delayUs=0 },
	{ .regAddr=0x3584, .regData=0x02, .delayUs=0 },
	{ .regAddr=0x3087, .regData=0x87, .delayUs=0 },
	{ .regAddr=0x3180, .regData=0x20, .delayUs=0 },
	{ .regAddr=0x3187, .regData=0x04, .delayUs=0 },
	{ .regAddr=0x3187, .regData=0x14, .delayUs=0 },
	{ .regAddr=0x3114, .regData=0x4a, .delayUs=0 },
	{ .regAddr=0x3115, .regData=0x00, .delayUs=0 },
	{ .regAddr=0x3126, .regData=0x04, .delayUs=0 },
	{ .regAddr=0x3c01, .regData=0x17, .delayUs=0 },
	{ .regAddr=0x0202, .regData=0x02, .delayUs=0 },
	{ .regAddr=0x0203, .regData=0x30, .delayUs=0 },
	{ .regAddr=0x0205, .regData=0x00, .delayUs=0 },
	{ .regAddr=0x3584, .regData=0x22, .delayUs=0 },
	{ .regAddr=0x3108, .regData=0xcf, .delayUs=0 },
	{ .regAddr=0x3123, .regData=0x4c, .delayUs=0 },
	{ .regAddr=0x3112, .regData=0xe4, .delayUs=0 },
	{ .regAddr=0x3113, .regData=0xfb, .delayUs=0 },
	{ .regAddr=0x300b, .regData=0x10, .delayUs=0 },
	{ .regAddr=0x3181, .regData=0x50, .delayUs=0 },
	{ .regAddr=0x3182, .regData=0x40, .delayUs=0 },
	{ .regAddr=0x3211, .regData=0x10, .delayUs=0 },
	{ .regAddr=0x3c18, .regData=0x20, .delayUs=0 },
	{ .regAddr=0x308c, .regData=0x70, .delayUs=0 },
	{ .regAddr=0x308d, .regData=0x71, .delayUs=0 },
	{ .regAddr=0x3403, .regData=0x00, .delayUs=0 },
	{ .regAddr=0x3407, .regData=0x06, .delayUs=0 },
	{ .regAddr=0x3410, .regData=0x04, .delayUs=0 },
	{ .regAddr=0x3414, .regData=0x06, .delayUs=0 },
	{ .regAddr=0x3600, .regData=0x08, .delayUs=0 },
	{ .regAddr=0x3500, .regData=0x10, .delayUs=0 },
	{ .regAddr=0x3584, .regData=0x02, .delayUs=0 },
	{ .regAddr=0x3411, .regData=0x08, .delayUs=0 },
	{ .regAddr=0x3412, .regData=0x09, .delayUs=0 },
	{ .regAddr=0x3415, .regData=0x01, .delayUs=0 },
	{ .regAddr=0x3416, .regData=0x01, .delayUs=0 },
	{ .regAddr=0xe060, .regData=0x31, .delayUs=0 },
	{ .regAddr=0xe061, .regData=0x12, .delayUs=0 },
	{ .regAddr=0xe062, .regData=0xe4, .delayUs=0 },
	{ .regAddr=0xe06c, .regData=0x31, .delayUs=0 },
	{ .regAddr=0xe06d, .regData=0x12, .delayUs=0 },
	{ .regAddr=0xe06e, .regData=0xe4, .delayUs=0 },
	{ .regAddr=0x0400, .regData=0x47, .delayUs=0 },
	{ .regAddr=0x0404, .regData=0x05, .delayUs=0 },
	{ .regAddr=0x0405, .regData=0x00, .delayUs=0 },
	{ .regAddr=0x0406, .regData=0x05, .delayUs=0 },
	{ .regAddr=0x0407, .regData=0x00, .delayUs=0 },
	{ .regAddr=0xe000, .regData=0x31, .delayUs=0 },
	{ .regAddr=0xe001, .regData=0x0a, .delayUs=0 },
	{ .regAddr=0xe002, .regData=0x88, .delayUs=0 },
	{ .regAddr=0xe003, .regData=0x31, .delayUs=0 },
	{ .regAddr=0xe004, .regData=0x0b, .delayUs=0 },
	{ .regAddr=0xe005, .regData=0x88, .delayUs=0 },
	{ .regAddr=0xe006, .regData=0x31, .delayUs=0 },
	{ .regAddr=0xe007, .regData=0x0c, .delayUs=0 },
	{ .regAddr=0xe008, .regData=0x88, .delayUs=0 },
	{ .regAddr=0xe009, .regData=0x31, .delayUs=0 },
	{ .regAddr=0xe00a, .regData=0x0d, .delayUs=0 },
	{ .regAddr=0xe00b, .regData=0x88, .delayUs=0 },
	{ .regAddr=0xe00c, .regData=0x32, .delayUs=0 },
	{ .regAddr=0xe00d, .regData=0xac, .delayUs=0 },
	{ .regAddr=0xe00e, .regData=0xff, .delayUs=0 },
	{ .regAddr=0xe00f, .regData=0x31, .delayUs=0 },
	{ .regAddr=0xe010, .regData=0x87, .delayUs=0 },
	{ .regAddr=0xe011, .regData=0x04, .delayUs=0 },
	{ .regAddr=0xe030, .regData=0x31, .delayUs=0 },
	{ .regAddr=0xe031, .regData=0x0a, .delayUs=0 },
	{ .regAddr=0xe032, .regData=0x90, .delayUs=0 },
	{ .regAddr=0xe033, .regData=0x31, .delayUs=0 },
	{ .regAddr=0xe034, .regData=0x0b, .delayUs=0 },
	{ .regAddr=0xe035, .regData=0x90, .delayUs=0 },
	{ .regAddr=0xe036, .regData=0x31, .delayUs=0 },
	{ .regAddr=0xe037, .regData=0x0c, .delayUs=0 },
	{ .regAddr=0xe038, .regData=0x90, .delayUs=0 },
	{ .regAddr=0xe039, .regData=0x31, .delayUs=0 },
	{ .regAddr=0xe03a, .regData=0x0d, .delayUs=0 },
	{ .regAddr=0xe03b, .regData=0x90, .delayUs=0 },
	{ .regAddr=0xe03c, .regData=0x32, .delayUs=0 },
	{ .regAddr=0xe03d, .regData=0xac, .delayUs=0 },
	{ .regAddr=0xe03e, .regData=0xff, .delayUs=0 },
	{ .regAddr=0xe03f, .regData=0x31, .delayUs=0 },
	{ .regAddr=0xe040, .regData=0x87, .delayUs=0 },
	{ .regAddr=0xe041, .regData=0x14, .delayUs=0 },
	{ .regAddr=0x3500, .regData=0x00, .delayUs=0 },
	{ .regAddr=0x310a, .regData=0x88, .delayUs=0 },
	{ .regAddr=0x310b, .regData=0x88, .delayUs=0 },
	{ .regAddr=0x310c, .regData=0x88, .delayUs=0 },
	{ .regAddr=0x310d, .regData=0x88, .delayUs=0 },
	{ .regAddr=0x0202, .regData=0x05, .delayUs=0 },
	{ .regAddr=0x0203, .regData=0x30, .delayUs=0 },
	{ .regAddr=0x3584, .regData=0x22, .delayUs=0 },
	{ .regAddr=0x0309, .regData=0x10, .delayUs=0 },
	{ .regAddr=0x0307, .regData=0x47, .delayUs=0 },
	{ .regAddr=0x3517, .regData=0x47, .delayUs=0 },
	{ .regAddr=0x3805, .regData=0x05, .delayUs=0 },
	{ .regAddr=0x3806, .regData=0x03, .delayUs=0 },
	{ .regAddr=0x3807, .regData=0x03, .delayUs=0 },
	{ .regAddr=0x3808, .regData=0x10, .delayUs=0 },
	{ .regAddr=0x3809, .regData=0x53, .delayUs=0 },
	{ .regAddr=0x380a, .regData=0x4a, .delayUs=0 },
	{ .regAddr=0x380b, .regData=0xa5, .delayUs=0 },
	{ .regAddr=0x0100, .regData=0x01, .delayUs=0 },

#endif
};


static drv_sensor_reg_info_t  c3390_streamOn_setting[] =
{
	{ .regAddr=0x0100, .regData= 0x01, .delayUs=0 },
};

static drv_sensor_reg_info_t  c3390_streamOff_setting[] =
{
	{ .regAddr=0x0100, .regData= 0x00, .delayUs=500000 }, //500ms
};


static drv_sensor_exp_param_t c3390_15fps_exparams =
{
    .maxIntegrationTime = 2656,//2980,
    .minIntegrationTime = 0,
    .maxAnalogGain      = 32536, //266300, //65536,//133300,
    .minAnalogGain      = 1024,
    .lineTime           = 102811,
    .limitShortMid      = 0,
    .limitLong          = 0,
    .expLineMargin      = 1,
};

static drv_power_setting_t c3390_power_up_setting[] =
{
    {.type = DRV_POWER_REG, .regAddr = 0x83c40004, .bitMask  = 0x01, .bitVal = 0x00, .delayUs = 0}, /// 100ms
    {.type = DRV_POWER_REG, .regAddr = 0x83c40004, .bitMask  = 0x01, .bitVal = 0x01, .delayUs = 0}  /// APB2 SENSOR_CTRL_REG 0x83c40004
};

static drv_power_setting_t c3390_power_down_setting[] =
{
    {.type = DRV_POWER_REG, .regAddr = 0x83c40004, .bitMask  = 0x01, .bitVal = 0x00, .delayUs = 0},
    {.type = DRV_POWER_REG, .regAddr = 0x83c40004, .bitMask  = 0x01, .bitVal = 0x01, .delayUs = 0}
};

static sensor_drv_params_t c3390_sensor_params =
{
    .sensorId     = 0x01,
    .sensorIdAddr = 0x0001,
    .name         = "c3390",
    .slaveInfo =
    {
        .hwType      = DRV_HW_I2C,
        .slaveAddr   = 0x36,// 0x10,//0x20 >> 1 //0x36,
        .channel     = 0,
        .regDataType = DRV_ADDR_TYPE_BYTE,
        .regAddrType = DRV_DATA_TYPE_WORD,
    },
    .powerUpSettings  =
    {
        .powerSetting = c3390_power_up_setting,
        .size = sizeof(c3390_power_up_setting) / sizeof(c3390_power_up_setting[0]),
    },
    .powerDownSettings =
    {
        .powerSetting  = c3390_power_down_setting,
        .size = sizeof(c3390_power_down_setting) / sizeof(c3390_power_down_setting[0]),
    },
    .activesize =
    {
        .width  = 2312,
        .height = 1296
    },
    .initSettings =
    {
        .regDataType = DRV_DATA_TYPE_BYTE,
        .regAddrType = DRV_ADDR_TYPE_WORD,
        .direction   = DRV_DIRECTION_WRITE,
        .settings    = NULL,
        .settingSize = 0,
    },
	.modeSettings[0] =
	{
		.maxFps = 15,
		.minFps = 15,
		.frameLength = 0xa60,
		.expLineMargin = 1,
		.dim =
		{
			.width  = 2304,
			.height = 1296,
		},
		.hdrMode   = SENSOR_NORMAL_MODE,
		.regConfig =
		{
			.regDataType = DRV_DATA_TYPE_BYTE,
			.regAddrType = DRV_ADDR_TYPE_WORD,
			.direction   = DRV_DIRECTION_WRITE,
			.settings    = c3390_4mp_setting,
			.settingSize = sizeof(c3390_4mp_setting) / sizeof (c3390_4mp_setting[0]),
		},
		.expParams   = &c3390_15fps_exparams,

		.intf = {
			.ln_cfg = {
				.dat_num = 2,
			},
			.link_freq = 284.0f,
		},
	},
    .streamOnSetting =
    {
        .regDataType = DRV_DATA_TYPE_BYTE,
        .regAddrType = DRV_ADDR_TYPE_WORD,
        .direction   = DRV_DIRECTION_WRITE,
        .settings    = c3390_streamOn_setting,
        .settingSize = sizeof(c3390_streamOn_setting) / sizeof (c3390_streamOn_setting[0]),
    },

    .streamOffSetting =
    {
        .regDataType = DRV_DATA_TYPE_BYTE,
        .regAddrType = DRV_ADDR_TYPE_WORD,
        .direction   = DRV_DIRECTION_WRITE,
        .settings    = c3390_streamOff_setting,
        .settingSize = sizeof(c3390_streamOff_setting) / sizeof (c3390_streamOff_setting[0]),
    },

    .fill_exposure_reg = c3390_Fill_Exposure_Reg,
};


sensor_drv_params_t* load_c3390_sensor_driver(void);

#endif

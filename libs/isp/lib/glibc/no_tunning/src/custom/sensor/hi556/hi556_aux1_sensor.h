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

#ifndef __HI556_SENSOR_H__
#define __HI556_SENSOR_H__

#include "sensor_common.h"
#include "log_utils.h"

isp_ret_t HI556_Fill_Exposure_Reg(sensor_apply_exp_param_t * pExpParam, drv_regs_array_info_t * pRegInfo);


//Sensor Information////////////////////////////
//Sensor	  : Hi-556
//Date		  : 2016-10-19
//Customer        : MTK_validation
//Image size	  : 2592x1944
//MCLK		  : 24MHz
//MIPI speed(Mbps): 880Mbps x 2Lane
//Frame Length	  : 4166
//Line Length 	  : 2816
//Max Fps 	  : 15.0fps
//Pixel order 	  : Green 1st (=GB)
//X/Y-flip	  : X-flip
//BLC offset	  : 64code
  
static drv_sensor_reg_info_t   Hi556_normal_15fps_setting_2lane[] =
{
  {.regAddr = 0x0a00, .regData = 0x0000, .delayUs = 0x0},
  {.regAddr = 0x0e00, .regData = 0x0102, .delayUs = 0x0},
  {.regAddr = 0x0e02, .regData = 0x0102, .delayUs = 0x0},
  {.regAddr = 0x0e0c, .regData = 0x0100, .delayUs = 0x0},
  {.regAddr = 0x2000, .regData = 0x7400, .delayUs = 0x0},
  {.regAddr = 0x2002, .regData = 0x001c, .delayUs = 0x0},
  {.regAddr = 0x2004, .regData = 0x0242, .delayUs = 0x0},
  {.regAddr = 0x2006, .regData = 0x0942, .delayUs = 0x0},
  {.regAddr = 0x2008, .regData = 0x7007, .delayUs = 0x0},
  {.regAddr = 0x200a, .regData = 0x0fd9, .delayUs = 0x0},
  {.regAddr = 0x200c, .regData = 0x0259, .delayUs = 0x0},
  {.regAddr = 0x200e, .regData = 0x7008, .delayUs = 0x0},
  {.regAddr = 0x2010, .regData = 0x160e, .delayUs = 0x0},
  {.regAddr = 0x2012, .regData = 0x0047, .delayUs = 0x0},
  {.regAddr = 0x2014, .regData = 0x2118, .delayUs = 0x0},
  {.regAddr = 0x2016, .regData = 0x0041, .delayUs = 0x0},
  {.regAddr = 0x2018, .regData = 0x00d8, .delayUs = 0x0},
  {.regAddr = 0x201a, .regData = 0x0145, .delayUs = 0x0},
  {.regAddr = 0x201c, .regData = 0x0006, .delayUs = 0x0},
  {.regAddr = 0x201e, .regData = 0x0181, .delayUs = 0x0},
  {.regAddr = 0x2020, .regData = 0x13cc, .delayUs = 0x0},
  {.regAddr = 0x2022, .regData = 0x2057, .delayUs = 0x0},
  {.regAddr = 0x2024, .regData = 0x7001, .delayUs = 0x0},
  {.regAddr = 0x2026, .regData = 0x0fca, .delayUs = 0x0},
  {.regAddr = 0x2028, .regData = 0x00cb, .delayUs = 0x0},
  {.regAddr = 0x202a, .regData = 0x009f, .delayUs = 0x0},
  {.regAddr = 0x202c, .regData = 0x7002, .delayUs = 0x0},
  {.regAddr = 0x202e, .regData = 0x13cc, .delayUs = 0x0},
  {.regAddr = 0x2030, .regData = 0x019b, .delayUs = 0x0},
  {.regAddr = 0x2032, .regData = 0x014d, .delayUs = 0x0},
  {.regAddr = 0x2034, .regData = 0x2987, .delayUs = 0x0},
  {.regAddr = 0x2036, .regData = 0x2766, .delayUs = 0x0},
  {.regAddr = 0x2038, .regData = 0x0020, .delayUs = 0x0},
  {.regAddr = 0x203a, .regData = 0x2060, .delayUs = 0x0},
  {.regAddr = 0x203c, .regData = 0x0e5d, .delayUs = 0x0},
  {.regAddr = 0x203e, .regData = 0x181d, .delayUs = 0x0},
  {.regAddr = 0x2040, .regData = 0x2066, .delayUs = 0x0},
  {.regAddr = 0x2042, .regData = 0x20c4, .delayUs = 0x0},
  {.regAddr = 0x2044, .regData = 0x5000, .delayUs = 0x0},
  {.regAddr = 0x2046, .regData = 0x0005, .delayUs = 0x0},
  {.regAddr = 0x2048, .regData = 0x0000, .delayUs = 0x0},
  {.regAddr = 0x204a, .regData = 0x01db, .delayUs = 0x0},
  {.regAddr = 0x204c, .regData = 0x025a, .delayUs = 0x0},
  {.regAddr = 0x204e, .regData = 0x00c0, .delayUs = 0x0},
  {.regAddr = 0x2050, .regData = 0x0005, .delayUs = 0x0},
  {.regAddr = 0x2052, .regData = 0x0006, .delayUs = 0x0},
  {.regAddr = 0x2054, .regData = 0x0ad9, .delayUs = 0x0},
  {.regAddr = 0x2056, .regData = 0x0259, .delayUs = 0x0},
  {.regAddr = 0x2058, .regData = 0x0618, .delayUs = 0x0},
  {.regAddr = 0x205a, .regData = 0x0258, .delayUs = 0x0},
  {.regAddr = 0x205c, .regData = 0x2266, .delayUs = 0x0},
  {.regAddr = 0x205e, .regData = 0x20c8, .delayUs = 0x0},
  {.regAddr = 0x2060, .regData = 0x2060, .delayUs = 0x0},
  {.regAddr = 0x2062, .regData = 0x707b, .delayUs = 0x0},
  {.regAddr = 0x2064, .regData = 0x0fdd, .delayUs = 0x0},
  {.regAddr = 0x2066, .regData = 0x81b8, .delayUs = 0x0},
  {.regAddr = 0x2068, .regData = 0x5040, .delayUs = 0x0},
  {.regAddr = 0x206a, .regData = 0x0020, .delayUs = 0x0},
  {.regAddr = 0x206c, .regData = 0x5060, .delayUs = 0x0},
  {.regAddr = 0x206e, .regData = 0x3143, .delayUs = 0x0},
  {.regAddr = 0x2070, .regData = 0x5081, .delayUs = 0x0},
  {.regAddr = 0x2072, .regData = 0x025c, .delayUs = 0x0},
  {.regAddr = 0x2074, .regData = 0x7800, .delayUs = 0x0},
  {.regAddr = 0x2076, .regData = 0x7400, .delayUs = 0x0},
  {.regAddr = 0x2078, .regData = 0x001c, .delayUs = 0x0},
  {.regAddr = 0x207a, .regData = 0x0242, .delayUs = 0x0},
  {.regAddr = 0x207c, .regData = 0x0942, .delayUs = 0x0},
  {.regAddr = 0x207e, .regData = 0x0bd9, .delayUs = 0x0},
  {.regAddr = 0x2080, .regData = 0x0259, .delayUs = 0x0},
  {.regAddr = 0x2082, .regData = 0x7008, .delayUs = 0x0},
  {.regAddr = 0x2084, .regData = 0x160e, .delayUs = 0x0},
  {.regAddr = 0x2086, .regData = 0x0047, .delayUs = 0x0},
  {.regAddr = 0x2088, .regData = 0x2118, .delayUs = 0x0},
  {.regAddr = 0x208a, .regData = 0x0041, .delayUs = 0x0},
  {.regAddr = 0x208c, .regData = 0x00d8, .delayUs = 0x0},
  {.regAddr = 0x208e, .regData = 0x0145, .delayUs = 0x0},
  {.regAddr = 0x2090, .regData = 0x0006, .delayUs = 0x0},
  {.regAddr = 0x2092, .regData = 0x0181, .delayUs = 0x0},
  {.regAddr = 0x2094, .regData = 0x13cc, .delayUs = 0x0},
  {.regAddr = 0x2096, .regData = 0x2057, .delayUs = 0x0},
  {.regAddr = 0x2098, .regData = 0x7001, .delayUs = 0x0},
  {.regAddr = 0x209a, .regData = 0x0fca, .delayUs = 0x0},
  {.regAddr = 0x209c, .regData = 0x00cb, .delayUs = 0x0},
  {.regAddr = 0x209e, .regData = 0x009f, .delayUs = 0x0},
  {.regAddr = 0x20a0, .regData = 0x7002, .delayUs = 0x0},
  {.regAddr = 0x20a2, .regData = 0x13cc, .delayUs = 0x0},
  {.regAddr = 0x20a4, .regData = 0x019b, .delayUs = 0x0},
  {.regAddr = 0x20a6, .regData = 0x014d, .delayUs = 0x0},
  {.regAddr = 0x20a8, .regData = 0x2987, .delayUs = 0x0},
  {.regAddr = 0x20aa, .regData = 0x2766, .delayUs = 0x0},
  {.regAddr = 0x20ac, .regData = 0x0020, .delayUs = 0x0},
  {.regAddr = 0x20ae, .regData = 0x2060, .delayUs = 0x0},
  {.regAddr = 0x20b0, .regData = 0x0e5d, .delayUs = 0x0},
  {.regAddr = 0x20b2, .regData = 0x181d, .delayUs = 0x0},
  {.regAddr = 0x20b4, .regData = 0x2066, .delayUs = 0x0},
  {.regAddr = 0x20b6, .regData = 0x20c4, .delayUs = 0x0},
  {.regAddr = 0x20b8, .regData = 0x50a0, .delayUs = 0x0},
  {.regAddr = 0x20ba, .regData = 0x0005, .delayUs = 0x0},
  {.regAddr = 0x20bc, .regData = 0x0000, .delayUs = 0x0},
  {.regAddr = 0x20be, .regData = 0x01db, .delayUs = 0x0},
  {.regAddr = 0x20c0, .regData = 0x025a, .delayUs = 0x0},
  {.regAddr = 0x20c2, .regData = 0x00c0, .delayUs = 0x0},
  {.regAddr = 0x20c4, .regData = 0x0005, .delayUs = 0x0},
  {.regAddr = 0x20c6, .regData = 0x0006, .delayUs = 0x0},
  {.regAddr = 0x20c8, .regData = 0x0ad9, .delayUs = 0x0},
  {.regAddr = 0x20ca, .regData = 0x0259, .delayUs = 0x0},
  {.regAddr = 0x20cc, .regData = 0x0618, .delayUs = 0x0},
  {.regAddr = 0x20ce, .regData = 0x0258, .delayUs = 0x0},
  {.regAddr = 0x20d0, .regData = 0x2266, .delayUs = 0x0},
  {.regAddr = 0x20d2, .regData = 0x20c8, .delayUs = 0x0},
  {.regAddr = 0x20d4, .regData = 0x2060, .delayUs = 0x0},
  {.regAddr = 0x20d6, .regData = 0x707b, .delayUs = 0x0},
  {.regAddr = 0x20d8, .regData = 0x0fdd, .delayUs = 0x0},
  {.regAddr = 0x20da, .regData = 0x86b8, .delayUs = 0x0},
  {.regAddr = 0x20dc, .regData = 0x50e0, .delayUs = 0x0},
  {.regAddr = 0x20de, .regData = 0x0020, .delayUs = 0x0},
  {.regAddr = 0x20e0, .regData = 0x5100, .delayUs = 0x0},
  {.regAddr = 0x20e2, .regData = 0x3143, .delayUs = 0x0},
  {.regAddr = 0x20e4, .regData = 0x5121, .delayUs = 0x0},
  {.regAddr = 0x20e6, .regData = 0x7800, .delayUs = 0x0},
  {.regAddr = 0x20e8, .regData = 0x3140, .delayUs = 0x0},
  {.regAddr = 0x20ea, .regData = 0x01c4, .delayUs = 0x0},
  {.regAddr = 0x20ec, .regData = 0x01c1, .delayUs = 0x0},
  {.regAddr = 0x20ee, .regData = 0x01c0, .delayUs = 0x0},
  {.regAddr = 0x20f0, .regData = 0x01c4, .delayUs = 0x0},
  {.regAddr = 0x20f2, .regData = 0x2700, .delayUs = 0x0},
  {.regAddr = 0x20f4, .regData = 0x3d40, .delayUs = 0x0},
  {.regAddr = 0x20f6, .regData = 0x7800, .delayUs = 0x0},
  {.regAddr = 0x20f8, .regData = 0xffff, .delayUs = 0x0},
  {.regAddr = 0x27fe, .regData = 0xe000, .delayUs = 0x0},
  {.regAddr = 0x3000, .regData = 0x60f8, .delayUs = 0x0},
  {.regAddr = 0x3002, .regData = 0x187f, .delayUs = 0x0},
  {.regAddr = 0x3004, .regData = 0x7060, .delayUs = 0x0},
  {.regAddr = 0x3006, .regData = 0x0114, .delayUs = 0x0},
  {.regAddr = 0x3008, .regData = 0x60b0, .delayUs = 0x0},
  {.regAddr = 0x300a, .regData = 0x1473, .delayUs = 0x0},
  {.regAddr = 0x300c, .regData = 0x0013, .delayUs = 0x0},
  {.regAddr = 0x300e, .regData = 0x140f, .delayUs = 0x0},
  {.regAddr = 0x3010, .regData = 0x0040, .delayUs = 0x0},
  {.regAddr = 0x3012, .regData = 0x100f, .delayUs = 0x0},
  {.regAddr = 0x3014, .regData = 0x60f8, .delayUs = 0x0},
  {.regAddr = 0x3016, .regData = 0x187f, .delayUs = 0x0},
  {.regAddr = 0x3018, .regData = 0x7060, .delayUs = 0x0},
  {.regAddr = 0x301a, .regData = 0x0114, .delayUs = 0x0},
  {.regAddr = 0x301c, .regData = 0x60b0, .delayUs = 0x0},
  {.regAddr = 0x301e, .regData = 0x1473, .delayUs = 0x0},
  {.regAddr = 0x3020, .regData = 0x0013, .delayUs = 0x0},
  {.regAddr = 0x3022, .regData = 0x140f, .delayUs = 0x0},
  {.regAddr = 0x3024, .regData = 0x0040, .delayUs = 0x0},
  {.regAddr = 0x3026, .regData = 0x000f, .delayUs = 0x0},
  
  
  
  {.regAddr = 0x0b00, .regData = 0x0000, .delayUs = 0x0},
  {.regAddr = 0x0b02, .regData = 0x0045, .delayUs = 0x0},
  {.regAddr = 0x0b04, .regData = 0xb405, .delayUs = 0x0},
  {.regAddr = 0x0b06, .regData = 0xc403, .delayUs = 0x0},
  {.regAddr = 0x0b08, .regData = 0x0081, .delayUs = 0x0},
  {.regAddr = 0x0b0a, .regData = 0x8252, .delayUs = 0x0},
  {.regAddr = 0x0b0c, .regData = 0xf814, .delayUs = 0x0},
  {.regAddr = 0x0b0e, .regData = 0xc618, .delayUs = 0x0},
  {.regAddr = 0x0b10, .regData = 0xa828, .delayUs = 0x0},
  {.regAddr = 0x0b12, .regData = 0x004c, .delayUs = 0x0},
  {.regAddr = 0x0b14, .regData = 0x4068, .delayUs = 0x0},
  {.regAddr = 0x0b16, .regData = 0x0000, .delayUs = 0x0},
  {.regAddr = 0x0f30, .regData = 0x6e25, .delayUs = 0x0},
  {.regAddr = 0x0f32, .regData = 0x7067, .delayUs = 0x0},
  {.regAddr = 0x0954, .regData = 0x0009, .delayUs = 0x0},
  {.regAddr = 0x0956, .regData = 0x1100, .delayUs = 0x0},
  {.regAddr = 0x0958, .regData = 0xcc80, .delayUs = 0x0},
  {.regAddr = 0x095a, .regData = 0x0000, .delayUs = 0x0},
  {.regAddr = 0x0c00, .regData = 0x1110, .delayUs = 0x0},
  {.regAddr = 0x0c02, .regData = 0x0011, .delayUs = 0x0},
  {.regAddr = 0x0c04, .regData = 0x0000, .delayUs = 0x0},
  {.regAddr = 0x0c06, .regData = 0x0200, .delayUs = 0x0},
  {.regAddr = 0x0c10, .regData = 0x0040, .delayUs = 0x0},
  {.regAddr = 0x0c12, .regData = 0x0040, .delayUs = 0x0},
  {.regAddr = 0x0c14, .regData = 0x0040, .delayUs = 0x0},
  {.regAddr = 0x0c16, .regData = 0x0040, .delayUs = 0x0},
  {.regAddr = 0x0a10, .regData = 0x4000, .delayUs = 0x0},
  {.regAddr = 0x3068, .regData = 0xf800, .delayUs = 0x0},
  {.regAddr = 0x306a, .regData = 0xf876, .delayUs = 0x0},
  {.regAddr = 0x006c, .regData = 0x0000, .delayUs = 0x0},
  {.regAddr = 0x005e, .regData = 0x0200, .delayUs = 0x0},
  {.regAddr = 0x000e, .regData = 0x0100, .delayUs = 0x0},
  {.regAddr = 0x0e0a, .regData = 0x0001, .delayUs = 0x0},
  {.regAddr = 0x004a, .regData = 0x0100, .delayUs = 0x0},
  {.regAddr = 0x004c, .regData = 0x0000, .delayUs = 0x0},
  {.regAddr = 0x004e, .regData = 0x0100, .delayUs = 0x0},
  {.regAddr = 0x000c, .regData = 0x0022, .delayUs = 0x0},
  {.regAddr = 0x0008, .regData = 0x0b00, .delayUs = 0x0},
  {.regAddr = 0x005a, .regData = 0x0202, .delayUs = 0x0},
  {.regAddr = 0x0012, .regData = 0x000e, .delayUs = 0x0},
  {.regAddr = 0x0018, .regData = 0x0a31, .delayUs = 0x0},
  {.regAddr = 0x0022, .regData = 0x0008, .delayUs = 0x0},
  {.regAddr = 0x0028, .regData = 0x0017, .delayUs = 0x0},
  {.regAddr = 0x0024, .regData = 0x0028, .delayUs = 0x0},
  {.regAddr = 0x002a, .regData = 0x002d, .delayUs = 0x0},
  {.regAddr = 0x0026, .regData = 0x0030, .delayUs = 0x0},
  {.regAddr = 0x002c, .regData = 0x07c7, .delayUs = 0x0},
  {.regAddr = 0x002e, .regData = 0x1111, .delayUs = 0x0},
  {.regAddr = 0x0030, .regData = 0x1111, .delayUs = 0x0},
  {.regAddr = 0x0032, .regData = 0x1111, .delayUs = 0x0},
  {.regAddr = 0x0006, .regData = 0x07bc, .delayUs = 0x0}, //0x07bbc 15fps
  {.regAddr = 0x0a22, .regData = 0x0000, .delayUs = 0x0},
  {.regAddr = 0x0a12, .regData = 0x0a20, .delayUs = 0x0},
  {.regAddr = 0x0a14, .regData = 0x0798, .delayUs = 0x0},
  {.regAddr = 0x003e, .regData = 0x0000, .delayUs = 0x0},
  {.regAddr = 0x0074, .regData = 0x080e, .delayUs = 0x0},  //080e
  {.regAddr = 0x0070, .regData = 0x0407, .delayUs = 0x0},  //0407
  {.regAddr = 0x0002, .regData = 0x0000, .delayUs = 0x0},
  {.regAddr = 0x0a02, .regData = 0x0100, .delayUs = 0x0},
  {.regAddr = 0x0a24, .regData = 0x0100, .delayUs = 0x0},
  {.regAddr = 0x0046, .regData = 0x0000, .delayUs = 0x0},
  {.regAddr = 0x0076, .regData = 0x0000, .delayUs = 0x0},
  {.regAddr = 0x0060, .regData = 0x0000, .delayUs = 0x0},
  {.regAddr = 0x0062, .regData = 0x0530, .delayUs = 0x0},
  {.regAddr = 0x0064, .regData = 0x0500, .delayUs = 0x0},
  {.regAddr = 0x0066, .regData = 0x0530, .delayUs = 0x0},
  {.regAddr = 0x0068, .regData = 0x0500, .delayUs = 0x0},
  {.regAddr = 0x0122, .regData = 0x0300, .delayUs = 0x0},
  {.regAddr = 0x015a, .regData = 0xff08, .delayUs = 0x0},
  {.regAddr = 0x0804, .regData = 0x0200, .delayUs = 0x0},
  {.regAddr = 0x005c, .regData = 0x0102, .delayUs = 0x0},
  {.regAddr = 0x0a1a, .regData = 0x0800, .delayUs = 0x0},
  
  
   //   {.regAddr = 0x003c, .regData = 0x0101, .delayUs = 0x0}, //fix framerate
     {.regAddr = 0x003c, .regData = 0x0101, .delayUs = 0x0}, //fix framerate
     {.regAddr = 0xFFFF, .regData = 0x0000, .delayUs = 0x0},
    /******/
  {.regAddr = 0x0a00, .regData = 0x0000, .delayUs = 0x0},
  {.regAddr = 0x0b0a, .regData = 0x8252, .delayUs = 0x0},
  {.regAddr = 0x0f30, .regData = 0x6e25, .delayUs = 0x0},
  {.regAddr = 0x0f32, .regData = 0x7067, .delayUs = 0x0},
  {.regAddr = 0x004a, .regData = 0x0100, .delayUs = 0x0},
  {.regAddr = 0x004c, .regData = 0x0000, .delayUs = 0x0},
  {.regAddr = 0x004e, .regData = 0x0100, .delayUs = 0x0}, //perframe enable
  {.regAddr = 0x000c, .regData = 0x0022, .delayUs = 0x0},
  {.regAddr = 0x0008, .regData = 0x0b00, .delayUs = 0x0},
  {.regAddr = 0x005a, .regData = 0x0202, .delayUs = 0x0},
  {.regAddr = 0x0012, .regData = 0x000e, .delayUs = 0x0},
  {.regAddr = 0x0018, .regData = 0x0a31, .delayUs = 0x0},
  {.regAddr = 0x0022, .regData = 0x0008, .delayUs = 0x0},
  {.regAddr = 0x0028, .regData = 0x0017, .delayUs = 0x0},
  {.regAddr = 0x0024, .regData = 0x0028, .delayUs = 0x0},
  {.regAddr = 0x002a, .regData = 0x002d, .delayUs = 0x0},
  {.regAddr = 0x0026, .regData = 0x0030, .delayUs = 0x0},
  {.regAddr = 0x002c, .regData = 0x07c7, .delayUs = 0x0},
  {.regAddr = 0x002e, .regData = 0x1111, .delayUs = 0x0},
  {.regAddr = 0x0030, .regData = 0x1111, .delayUs = 0x0},
  {.regAddr = 0x0032, .regData = 0x1111, .delayUs = 0x0},
  {.regAddr = 0x0008, .regData = 0x0b08, .delayUs = 0x0},
//   {.regAddr = 0x0006, .regData = 0x10cc, .delayUs = 0x0}, //update vmax
  {.regAddr = 0x0006, .regData = 0x1046, .delayUs = 0x0}, //update vmax
//   {.regAddr = 0x0006, .regData = 0x114c, .delayUs = 0x0}, //update vmax
  {.regAddr = 0x0a22, .regData = 0x0000, .delayUs = 0x0},
  {.regAddr = 0x0a12, .regData = 0x0a20, .delayUs = 0x0},
  {.regAddr = 0x0a14, .regData = 0x0798, .delayUs = 0x0},
  {.regAddr = 0x003e, .regData = 0x0000, .delayUs = 0x0},
  {.regAddr = 0x0074, .regData = 0x1044, .delayUs = 0x0}, //add
  {.regAddr = 0x0070, .regData = 0x0411, .delayUs = 0x0}, //add
  {.regAddr = 0x0804, .regData = 0x0200, .delayUs = 0x0},
  {.regAddr = 0x0a04, .regData = 0x014a, .delayUs = 0x0},
  {.regAddr = 0x090c, .regData = 0x0fdc, .delayUs = 0x0},
  {.regAddr = 0x090e, .regData = 0x002d, .delayUs = 0x0},
  {.regAddr = 0x0902, .regData = 0x4319, .delayUs = 0x0},
  {.regAddr = 0x0914, .regData = 0xc10a, .delayUs = 0x0},
  {.regAddr = 0x0916, .regData = 0x071f, .delayUs = 0x0},
  {.regAddr = 0x0918, .regData = 0x0408, .delayUs = 0x0},
  {.regAddr = 0x091a, .regData = 0x0c0d, .delayUs = 0x0},
  {.regAddr = 0x091c, .regData = 0x0f09, .delayUs = 0x0},
  {.regAddr = 0x091e, .regData = 0x0a00, .delayUs = 0x0},
  {.regAddr = 0xFFFF, .regData = 0x00, .delayUs = 0x0}, //?
};

/*
 * Xclk 24Mhz
 * Pclk 176Mhz
 * linelength 2816(0xb00)
 * framelength 1988(0x7c0)
 * grabwindow_width 2592
 * grabwindow_height 1944
 * max_framerate 30fps
 * MIPI speed(Mbps) : 840Mbps x 2Lane
 */
static drv_sensor_reg_info_t  Hi556_normal_setting_2lane[] =
{
  {.regAddr = 0x0a00, .regData = 0x0000, .delayUs = 0x0},
  {.regAddr = 0x0e00, .regData = 0x0102, .delayUs = 0x0},
  {.regAddr = 0x0e02, .regData = 0x0102, .delayUs = 0x0},
  {.regAddr = 0x0e0c, .regData = 0x0100, .delayUs = 0x0},
  {.regAddr = 0x2000, .regData = 0x7400, .delayUs = 0x0},
  {.regAddr = 0x2002, .regData = 0x001c, .delayUs = 0x0},
  {.regAddr = 0x2004, .regData = 0x0242, .delayUs = 0x0},
  {.regAddr = 0x2006, .regData = 0x0942, .delayUs = 0x0},
  {.regAddr = 0x2008, .regData = 0x7007, .delayUs = 0x0},
  {.regAddr = 0x200a, .regData = 0x0fd9, .delayUs = 0x0},
  {.regAddr = 0x200c, .regData = 0x0259, .delayUs = 0x0},
  {.regAddr = 0x200e, .regData = 0x7008, .delayUs = 0x0},
  {.regAddr = 0x2010, .regData = 0x160e, .delayUs = 0x0},
  {.regAddr = 0x2012, .regData = 0x0047, .delayUs = 0x0},
  {.regAddr = 0x2014, .regData = 0x2118, .delayUs = 0x0},
  {.regAddr = 0x2016, .regData = 0x0041, .delayUs = 0x0},
  {.regAddr = 0x2018, .regData = 0x00d8, .delayUs = 0x0},
  {.regAddr = 0x201a, .regData = 0x0145, .delayUs = 0x0},
  {.regAddr = 0x201c, .regData = 0x0006, .delayUs = 0x0},
  {.regAddr = 0x201e, .regData = 0x0181, .delayUs = 0x0},
  {.regAddr = 0x2020, .regData = 0x13cc, .delayUs = 0x0},
  {.regAddr = 0x2022, .regData = 0x2057, .delayUs = 0x0},
  {.regAddr = 0x2024, .regData = 0x7001, .delayUs = 0x0},
  {.regAddr = 0x2026, .regData = 0x0fca, .delayUs = 0x0},
  {.regAddr = 0x2028, .regData = 0x00cb, .delayUs = 0x0},
  {.regAddr = 0x202a, .regData = 0x009f, .delayUs = 0x0},
  {.regAddr = 0x202c, .regData = 0x7002, .delayUs = 0x0},
  {.regAddr = 0x202e, .regData = 0x13cc, .delayUs = 0x0},
  {.regAddr = 0x2030, .regData = 0x019b, .delayUs = 0x0},
  {.regAddr = 0x2032, .regData = 0x014d, .delayUs = 0x0},
  {.regAddr = 0x2034, .regData = 0x2987, .delayUs = 0x0},
  {.regAddr = 0x2036, .regData = 0x2766, .delayUs = 0x0},
  {.regAddr = 0x2038, .regData = 0x0020, .delayUs = 0x0},
  {.regAddr = 0x203a, .regData = 0x2060, .delayUs = 0x0},
  {.regAddr = 0x203c, .regData = 0x0e5d, .delayUs = 0x0},
  {.regAddr = 0x203e, .regData = 0x181d, .delayUs = 0x0},
  {.regAddr = 0x2040, .regData = 0x2066, .delayUs = 0x0},
  {.regAddr = 0x2042, .regData = 0x20c4, .delayUs = 0x0},
  {.regAddr = 0x2044, .regData = 0x5000, .delayUs = 0x0},
  {.regAddr = 0x2046, .regData = 0x0005, .delayUs = 0x0},
  {.regAddr = 0x2048, .regData = 0x0000, .delayUs = 0x0},
  {.regAddr = 0x204a, .regData = 0x01db, .delayUs = 0x0},
  {.regAddr = 0x204c, .regData = 0x025a, .delayUs = 0x0},
  {.regAddr = 0x204e, .regData = 0x00c0, .delayUs = 0x0},
  {.regAddr = 0x2050, .regData = 0x0005, .delayUs = 0x0},
  {.regAddr = 0x2052, .regData = 0x0006, .delayUs = 0x0},
  {.regAddr = 0x2054, .regData = 0x0ad9, .delayUs = 0x0},
  {.regAddr = 0x2056, .regData = 0x0259, .delayUs = 0x0},
  {.regAddr = 0x2058, .regData = 0x0618, .delayUs = 0x0},
  {.regAddr = 0x205a, .regData = 0x0258, .delayUs = 0x0},
  {.regAddr = 0x205c, .regData = 0x2266, .delayUs = 0x0},
  {.regAddr = 0x205e, .regData = 0x20c8, .delayUs = 0x0},
  {.regAddr = 0x2060, .regData = 0x2060, .delayUs = 0x0},
  {.regAddr = 0x2062, .regData = 0x707b, .delayUs = 0x0},
  {.regAddr = 0x2064, .regData = 0x0fdd, .delayUs = 0x0},
  {.regAddr = 0x2066, .regData = 0x81b8, .delayUs = 0x0},
  {.regAddr = 0x2068, .regData = 0x5040, .delayUs = 0x0},
  {.regAddr = 0x206a, .regData = 0x0020, .delayUs = 0x0},
  {.regAddr = 0x206c, .regData = 0x5060, .delayUs = 0x0},
  {.regAddr = 0x206e, .regData = 0x3143, .delayUs = 0x0},
  {.regAddr = 0x2070, .regData = 0x5081, .delayUs = 0x0},
  {.regAddr = 0x2072, .regData = 0x025c, .delayUs = 0x0},
  {.regAddr = 0x2074, .regData = 0x7800, .delayUs = 0x0},
  {.regAddr = 0x2076, .regData = 0x7400, .delayUs = 0x0},
  {.regAddr = 0x2078, .regData = 0x001c, .delayUs = 0x0},
  {.regAddr = 0x207a, .regData = 0x0242, .delayUs = 0x0},
  {.regAddr = 0x207c, .regData = 0x0942, .delayUs = 0x0},
  {.regAddr = 0x207e, .regData = 0x0bd9, .delayUs = 0x0},
  {.regAddr = 0x2080, .regData = 0x0259, .delayUs = 0x0},
  {.regAddr = 0x2082, .regData = 0x7008, .delayUs = 0x0},
  {.regAddr = 0x2084, .regData = 0x160e, .delayUs = 0x0},
  {.regAddr = 0x2086, .regData = 0x0047, .delayUs = 0x0},
  {.regAddr = 0x2088, .regData = 0x2118, .delayUs = 0x0},
  {.regAddr = 0x208a, .regData = 0x0041, .delayUs = 0x0},
  {.regAddr = 0x208c, .regData = 0x00d8, .delayUs = 0x0},
  {.regAddr = 0x208e, .regData = 0x0145, .delayUs = 0x0},
  {.regAddr = 0x2090, .regData = 0x0006, .delayUs = 0x0},
  {.regAddr = 0x2092, .regData = 0x0181, .delayUs = 0x0},
  {.regAddr = 0x2094, .regData = 0x13cc, .delayUs = 0x0},
  {.regAddr = 0x2096, .regData = 0x2057, .delayUs = 0x0},
  {.regAddr = 0x2098, .regData = 0x7001, .delayUs = 0x0},
  {.regAddr = 0x209a, .regData = 0x0fca, .delayUs = 0x0},
  {.regAddr = 0x209c, .regData = 0x00cb, .delayUs = 0x0},
  {.regAddr = 0x209e, .regData = 0x009f, .delayUs = 0x0},
  {.regAddr = 0x20a0, .regData = 0x7002, .delayUs = 0x0},
  {.regAddr = 0x20a2, .regData = 0x13cc, .delayUs = 0x0},
  {.regAddr = 0x20a4, .regData = 0x019b, .delayUs = 0x0},
  {.regAddr = 0x20a6, .regData = 0x014d, .delayUs = 0x0},
  {.regAddr = 0x20a8, .regData = 0x2987, .delayUs = 0x0},
  {.regAddr = 0x20aa, .regData = 0x2766, .delayUs = 0x0},
  {.regAddr = 0x20ac, .regData = 0x0020, .delayUs = 0x0},
  {.regAddr = 0x20ae, .regData = 0x2060, .delayUs = 0x0},
  {.regAddr = 0x20b0, .regData = 0x0e5d, .delayUs = 0x0},
  {.regAddr = 0x20b2, .regData = 0x181d, .delayUs = 0x0},
  {.regAddr = 0x20b4, .regData = 0x2066, .delayUs = 0x0},
  {.regAddr = 0x20b6, .regData = 0x20c4, .delayUs = 0x0},
  {.regAddr = 0x20b8, .regData = 0x50a0, .delayUs = 0x0},
  {.regAddr = 0x20ba, .regData = 0x0005, .delayUs = 0x0},
  {.regAddr = 0x20bc, .regData = 0x0000, .delayUs = 0x0},
  {.regAddr = 0x20be, .regData = 0x01db, .delayUs = 0x0},
  {.regAddr = 0x20c0, .regData = 0x025a, .delayUs = 0x0},
  {.regAddr = 0x20c2, .regData = 0x00c0, .delayUs = 0x0},
  {.regAddr = 0x20c4, .regData = 0x0005, .delayUs = 0x0},
  {.regAddr = 0x20c6, .regData = 0x0006, .delayUs = 0x0},
  {.regAddr = 0x20c8, .regData = 0x0ad9, .delayUs = 0x0},
  {.regAddr = 0x20ca, .regData = 0x0259, .delayUs = 0x0},
  {.regAddr = 0x20cc, .regData = 0x0618, .delayUs = 0x0},
  {.regAddr = 0x20ce, .regData = 0x0258, .delayUs = 0x0},
  {.regAddr = 0x20d0, .regData = 0x2266, .delayUs = 0x0},
  {.regAddr = 0x20d2, .regData = 0x20c8, .delayUs = 0x0},
  {.regAddr = 0x20d4, .regData = 0x2060, .delayUs = 0x0},
  {.regAddr = 0x20d6, .regData = 0x707b, .delayUs = 0x0},
  {.regAddr = 0x20d8, .regData = 0x0fdd, .delayUs = 0x0},
  {.regAddr = 0x20da, .regData = 0x86b8, .delayUs = 0x0},
  {.regAddr = 0x20dc, .regData = 0x50e0, .delayUs = 0x0},
  {.regAddr = 0x20de, .regData = 0x0020, .delayUs = 0x0},
  {.regAddr = 0x20e0, .regData = 0x5100, .delayUs = 0x0},
  {.regAddr = 0x20e2, .regData = 0x3143, .delayUs = 0x0},
  {.regAddr = 0x20e4, .regData = 0x5121, .delayUs = 0x0},
  {.regAddr = 0x20e6, .regData = 0x7800, .delayUs = 0x0},
  {.regAddr = 0x20e8, .regData = 0x3140, .delayUs = 0x0},
  {.regAddr = 0x20ea, .regData = 0x01c4, .delayUs = 0x0},
  {.regAddr = 0x20ec, .regData = 0x01c1, .delayUs = 0x0},
  {.regAddr = 0x20ee, .regData = 0x01c0, .delayUs = 0x0},
  {.regAddr = 0x20f0, .regData = 0x01c4, .delayUs = 0x0},
  {.regAddr = 0x20f2, .regData = 0x2700, .delayUs = 0x0},
  {.regAddr = 0x20f4, .regData = 0x3d40, .delayUs = 0x0},
  {.regAddr = 0x20f6, .regData = 0x7800, .delayUs = 0x0},
  {.regAddr = 0x20f8, .regData = 0xffff, .delayUs = 0x0},
  {.regAddr = 0x27fe, .regData = 0xe000, .delayUs = 0x0},
  {.regAddr = 0x3000, .regData = 0x60f8, .delayUs = 0x0},
  {.regAddr = 0x3002, .regData = 0x187f, .delayUs = 0x0},
  {.regAddr = 0x3004, .regData = 0x7060, .delayUs = 0x0},
  {.regAddr = 0x3006, .regData = 0x0114, .delayUs = 0x0},
  {.regAddr = 0x3008, .regData = 0x60b0, .delayUs = 0x0},
  {.regAddr = 0x300a, .regData = 0x1473, .delayUs = 0x0},
  {.regAddr = 0x300c, .regData = 0x0013, .delayUs = 0x0},
  {.regAddr = 0x300e, .regData = 0x140f, .delayUs = 0x0},
  {.regAddr = 0x3010, .regData = 0x0040, .delayUs = 0x0},
  {.regAddr = 0x3012, .regData = 0x100f, .delayUs = 0x0},
  {.regAddr = 0x3014, .regData = 0x60f8, .delayUs = 0x0},
  {.regAddr = 0x3016, .regData = 0x187f, .delayUs = 0x0},
  {.regAddr = 0x3018, .regData = 0x7060, .delayUs = 0x0},
  {.regAddr = 0x301a, .regData = 0x0114, .delayUs = 0x0},
  {.regAddr = 0x301c, .regData = 0x60b0, .delayUs = 0x0},
  {.regAddr = 0x301e, .regData = 0x1473, .delayUs = 0x0},
  {.regAddr = 0x3020, .regData = 0x0013, .delayUs = 0x0},
  {.regAddr = 0x3022, .regData = 0x140f, .delayUs = 0x0},
  {.regAddr = 0x3024, .regData = 0x0040, .delayUs = 0x0},
  {.regAddr = 0x3026, .regData = 0x000f, .delayUs = 0x0},
  {.regAddr = 0x0b00, .regData = 0x0000, .delayUs = 0x0},
  {.regAddr = 0x0b02, .regData = 0x0045, .delayUs = 0x0},
  {.regAddr = 0x0b04, .regData = 0xb405, .delayUs = 0x0},
  {.regAddr = 0x0b06, .regData = 0xc403, .delayUs = 0x0},
  {.regAddr = 0x0b08, .regData = 0x0081, .delayUs = 0x0},
  {.regAddr = 0x0b0a, .regData = 0x8252, .delayUs = 0x0},
  {.regAddr = 0x0b0c, .regData = 0xf814, .delayUs = 0x0},
  {.regAddr = 0x0b0e, .regData = 0xc618, .delayUs = 0x0},
  {.regAddr = 0x0b10, .regData = 0xa828, .delayUs = 0x0},
  {.regAddr = 0x0b12, .regData = 0x004c, .delayUs = 0x0},
  {.regAddr = 0x0b14, .regData = 0x4068, .delayUs = 0x0},
  {.regAddr = 0x0b16, .regData = 0x0000, .delayUs = 0x0},
  {.regAddr = 0x0f30, .regData = 0x6e25, .delayUs = 0x0},
  {.regAddr = 0x0f32, .regData = 0x7067, .delayUs = 0x0},
  {.regAddr = 0x0954, .regData = 0x0009, .delayUs = 0x0},
  {.regAddr = 0x0956, .regData = 0x1100, .delayUs = 0x0},
  {.regAddr = 0x0958, .regData = 0xcc80, .delayUs = 0x0},
  {.regAddr = 0x095a, .regData = 0x0000, .delayUs = 0x0},
  {.regAddr = 0x0c00, .regData = 0x1110, .delayUs = 0x0},
  {.regAddr = 0x0c02, .regData = 0x0011, .delayUs = 0x0},
  {.regAddr = 0x0c04, .regData = 0x0000, .delayUs = 0x0},
  {.regAddr = 0x0c06, .regData = 0x0200, .delayUs = 0x0},
  {.regAddr = 0x0c10, .regData = 0x0040, .delayUs = 0x0},
  {.regAddr = 0x0c12, .regData = 0x0040, .delayUs = 0x0},
  {.regAddr = 0x0c14, .regData = 0x0040, .delayUs = 0x0},
  {.regAddr = 0x0c16, .regData = 0x0040, .delayUs = 0x0},
  {.regAddr = 0x0a10, .regData = 0x4000, .delayUs = 0x0},
  {.regAddr = 0x3068, .regData = 0xf800, .delayUs = 0x0},
  {.regAddr = 0x306a, .regData = 0xf876, .delayUs = 0x0},
  {.regAddr = 0x006c, .regData = 0x0000, .delayUs = 0x0},
  {.regAddr = 0x005e, .regData = 0x0200, .delayUs = 0x0},
  {.regAddr = 0x000e, .regData = 0x0100, .delayUs = 0x0},
  {.regAddr = 0x0e0a, .regData = 0x0001, .delayUs = 0x0},
  {.regAddr = 0x004a, .regData = 0x0100, .delayUs = 0x0},
  {.regAddr = 0x004c, .regData = 0x0000, .delayUs = 0x0},
  {.regAddr = 0x004e, .regData = 0x0100, .delayUs = 0x0},
  {.regAddr = 0x000c, .regData = 0x0022, .delayUs = 0x0},
  {.regAddr = 0x0008, .regData = 0x0b00, .delayUs = 0x0},
  {.regAddr = 0x005a, .regData = 0x0202, .delayUs = 0x0},
  {.regAddr = 0x0012, .regData = 0x000e, .delayUs = 0x0},
  {.regAddr = 0x0018, .regData = 0x0a31, .delayUs = 0x0},
  {.regAddr = 0x0022, .regData = 0x0008, .delayUs = 0x0},
  {.regAddr = 0x0028, .regData = 0x0017, .delayUs = 0x0},
  {.regAddr = 0x0024, .regData = 0x0028, .delayUs = 0x0},
  {.regAddr = 0x002a, .regData = 0x002d, .delayUs = 0x0},
  {.regAddr = 0x0026, .regData = 0x0030, .delayUs = 0x0},
  {.regAddr = 0x002c, .regData = 0x07c7, .delayUs = 0x0},
  {.regAddr = 0x002e, .regData = 0x1111, .delayUs = 0x0},
  {.regAddr = 0x0030, .regData = 0x1111, .delayUs = 0x0},
  {.regAddr = 0x0032, .regData = 0x1111, .delayUs = 0x0},
  {.regAddr = 0x0006, .regData = 0x0823, .delayUs = 0x0},
  {.regAddr = 0x0a22, .regData = 0x0000, .delayUs = 0x0},
  {.regAddr = 0x0a12, .regData = 0x0a20, .delayUs = 0x0},
  {.regAddr = 0x0a14, .regData = 0x0798, .delayUs = 0x0},
  {.regAddr = 0x003e, .regData = 0x0000, .delayUs = 0x0},
  {.regAddr = 0x0074, .regData = 0x0821, .delayUs = 0x0},
  {.regAddr = 0x0070, .regData = 0x0411, .delayUs = 0x0},
  {.regAddr = 0x0002, .regData = 0x0000, .delayUs = 0x0},
  {.regAddr = 0x0a02, .regData = 0x0100, .delayUs = 0x0},
  {.regAddr = 0x0a24, .regData = 0x0100, .delayUs = 0x0},
  {.regAddr = 0x0076, .regData = 0x0000, .delayUs = 0x0},
  {.regAddr = 0x0060, .regData = 0x0000, .delayUs = 0x0},
  {.regAddr = 0x0062, .regData = 0x0530, .delayUs = 0x0},
  {.regAddr = 0x0064, .regData = 0x0500, .delayUs = 0x0},
  {.regAddr = 0x0066, .regData = 0x0530, .delayUs = 0x0},
  {.regAddr = 0x0068, .regData = 0x0500, .delayUs = 0x0},
  {.regAddr = 0x0122, .regData = 0x0300, .delayUs = 0x0},
  {.regAddr = 0x015a, .regData = 0xff08, .delayUs = 0x0},
  {.regAddr = 0x0804, .regData = 0x0200, .delayUs = 0x0},
  {.regAddr = 0x005c, .regData = 0x0102, .delayUs = 0x0},
  {.regAddr = 0x0a1a, .regData = 0x0800, .delayUs = 0x0},
  {.regAddr = 0x003c, .regData = 0x0101, .delayUs = 0x0}, //fix framerate
  {.regAddr = 0xFFFF, .regData = 0x0000, .delayUs = 0x0},
 /******/
  {.regAddr = 0x0a00, .regData = 0x0000, .delayUs = 0x0},
  {.regAddr = 0x0b0a, .regData = 0x8252, .delayUs = 0x0},
  {.regAddr = 0x0f30, .regData = 0x6e25, .delayUs = 0x0},
  {.regAddr = 0x0f32, .regData = 0x7067, .delayUs = 0x0},
  {.regAddr = 0x004a, .regData = 0x0100, .delayUs = 0x0},
  {.regAddr = 0x004c, .regData = 0x0000, .delayUs = 0x0},
  {.regAddr = 0x004e, .regData = 0x0000, .delayUs = 0x0},
  {.regAddr = 0x000c, .regData = 0x0022, .delayUs = 0x0},
  {.regAddr = 0x0008, .regData = 0x0b00, .delayUs = 0x0},
  {.regAddr = 0x005a, .regData = 0x0202, .delayUs = 0x0},
  {.regAddr = 0x0012, .regData = 0x000e, .delayUs = 0x0},
  {.regAddr = 0x0018, .regData = 0x0a31, .delayUs = 0x0},
  {.regAddr = 0x0022, .regData = 0x0008, .delayUs = 0x0},
  {.regAddr = 0x0028, .regData = 0x0017, .delayUs = 0x0},
  {.regAddr = 0x0024, .regData = 0x0028, .delayUs = 0x0},
  {.regAddr = 0x002a, .regData = 0x002d, .delayUs = 0x0},
  {.regAddr = 0x0026, .regData = 0x0030, .delayUs = 0x0},
  {.regAddr = 0x002c, .regData = 0x07c7, .delayUs = 0x0},
  {.regAddr = 0x002e, .regData = 0x1111, .delayUs = 0x0},
  {.regAddr = 0x0030, .regData = 0x1111, .delayUs = 0x0},
  {.regAddr = 0x0032, .regData = 0x1111, .delayUs = 0x0},
  {.regAddr = 0x0006, .regData = 0x0c34, .delayUs = 0x0},
  {.regAddr = 0x0a22, .regData = 0x0000, .delayUs = 0x0},
  {.regAddr = 0x0a12, .regData = 0x0a20, .delayUs = 0x0},
  {.regAddr = 0x0a14, .regData = 0x0798, .delayUs = 0x0},
  {.regAddr = 0x003e, .regData = 0x0000, .delayUs = 0x0},
  {.regAddr = 0x0804, .regData = 0x0200, .delayUs = 0x0},
  {.regAddr = 0x0a04, .regData = 0x014a, .delayUs = 0x0},
  {.regAddr = 0x090c, .regData = 0x0fdc, .delayUs = 0x0},
  {.regAddr = 0x090e, .regData = 0x002d, .delayUs = 0x0},
  {.regAddr = 0x0902, .regData = 0x4319, .delayUs = 0x0},
  {.regAddr = 0x0914, .regData = 0xc10a, .delayUs = 0x0},
  {.regAddr = 0x0916, .regData = 0x071f, .delayUs = 0x0},
  {.regAddr = 0x0918, .regData = 0x0408, .delayUs = 0x0},
  {.regAddr = 0x091a, .regData = 0x0c0d, .delayUs = 0x0},
  {.regAddr = 0x091c, .regData = 0x0f09, .delayUs = 0x0},
  {.regAddr = 0x091e, .regData = 0x0a00, .delayUs = 0x0},
  {.regAddr = 0xFFFF, .regData = 0x00, .delayUs = 0x0},
};

static drv_sensor_reg_info_t  hi556_streamOn_setting[] =
{
	{.regAddr= 0xa00, .regData= 0x0100, .delayUs=0 }, /* enter normal mode*/
};

static drv_sensor_reg_info_t  hi556_streamOff_setting[] =
{
	{ .regAddr= 0xa00, .regData= 0x0, .delayUs=0 }, /* enter standby mode*/
};

static drv_sensor_exp_param_t hi556_normal_exparams =
{
    .maxIntegrationTime = 1980  ,
    .minIntegrationTime = 4     ,
    .maxAnalogGain      = 33532,
    .minAnalogGain      = 1024  ,
    .lineTime           = 68956,
    .limitShortMid      = 0     ,
    .limitLong          = 0     ,
    .expLineMargin      = 1     ,
};

static drv_sensor_exp_param_t hi556_normal_15ps_exparams =
{
    .maxIntegrationTime = 1980,
    .minIntegrationTime = 4     ,
    .maxAnalogGain      = 33532,
    .minAnalogGain      = 1024  ,
    .lineTime           = 63500,
    .limitShortMid      = 0     ,
    .limitLong          = 0     ,
    .expLineMargin      = 1     ,
};

static drv_power_setting_t hi556_power_up_setting[] =
{
    {.type = DRV_POWER_REG, .regAddr = 0x83c40004, .bitMask  = 0x01, .bitVal = 0x00, .delayUs = 10000}, /// 10ms
    {.type = DRV_POWER_REG, .regAddr = 0x83c40004, .bitMask  = 0x01, .bitVal = 0x01, .delayUs = 10000}  /// APB2 SENSOR_CTRL_REG 0x83c40004
};


static drv_power_setting_t hi556_power_down_setting[] =
{
    {.type = DRV_POWER_REG, .regAddr = 0x83c40004, .bitMask  = 0x01, .bitVal = 0x00, .delayUs = 10000},
    {.type = DRV_POWER_REG, .regAddr = 0x83c40004, .bitMask  = 0x01, .bitVal = 0x01, .delayUs = 10000}
};

static sensor_drv_params_t hi556_aux1_sensor_params =
{
    .sensorId     = 0x0556, //0x145
    .sensorIdAddr = 0x0f16,
    .name         = "hi556",
    .slaveInfo =
    {
        .hwType      = DRV_HW_I2C,
        .slaveAddr   = (0x40 >> 1),
        .channel     = 1,
        .regDataType = DRV_ADDR_TYPE_BYTE,
        .regAddrType = DRV_DATA_TYPE_WORD,
    },
    .powerUpSettings  =
    {
        .powerSetting = hi556_power_up_setting,
        .size = sizeof(hi556_power_up_setting) / sizeof(hi556_power_up_setting[0]),
    },
    .powerDownSettings =
    {
        .powerSetting  = hi556_power_down_setting,
        .size = sizeof(hi556_power_down_setting) / sizeof(hi556_power_down_setting[0]),
    },
    .activesize =
    {
        .width  = 2592,
        .height = 1944
    },
    .initSettings =
    {
        .regDataType = DRV_DATA_TYPE_WORD,//DRV_DATA_TYPE_BYTE,
        .regAddrType = DRV_ADDR_TYPE_WORD,
        .direction   = DRV_DIRECTION_WRITE,
        .settings    = NULL,
        .settingSize = 0,
    },
    .modeSettings[0] =
    {
        .maxFps = 15,
        .minFps = 15,
        .frameLength = 0x10d0,//0x7c0,
        .expLineMargin = 1,
        .dim =
        {
            .width  = 2592,
            .height = 1944
        },
        .hdrMode   = SENSOR_NORMAL_MODE,
        .regConfig =
        {
            .regDataType = DRV_DATA_TYPE_WORD,//DRV_DATA_TYPE_BYTE,
            .regAddrType = DRV_ADDR_TYPE_WORD,
            .direction   = DRV_DIRECTION_WRITE,
            .settings    = Hi556_normal_15fps_setting_2lane,
            .settingSize = sizeof(Hi556_normal_15fps_setting_2lane) / sizeof (Hi556_normal_15fps_setting_2lane[0]),

        },
        .expParams   = &hi556_normal_15ps_exparams,

        .intf = {
          .ln_cfg = {
            .dat_num = 2,
          },
          .link_freq = 840.0f,
        },
    },
    .streamOnSetting =
    {
        .regDataType = DRV_DATA_TYPE_WORD,//DRV_DATA_TYPE_BYTE,
        .regAddrType = DRV_ADDR_TYPE_WORD,
        .direction   = DRV_DIRECTION_WRITE,
        .settings    = hi556_streamOn_setting,
        .settingSize = sizeof(hi556_streamOn_setting) / sizeof (hi556_streamOn_setting[0]),
    },

    .streamOffSetting =
    {
        .regDataType = DRV_DATA_TYPE_WORD,//DRV_DATA_TYPE_BYTE,
        .regAddrType = DRV_ADDR_TYPE_WORD,
        .direction   = DRV_DIRECTION_WRITE,
        .settings    = hi556_streamOff_setting,
        .settingSize = sizeof(hi556_streamOff_setting) / sizeof (hi556_streamOff_setting[0]),
    },

    .fill_exposure_reg = HI556_Fill_Exposure_Reg,
};


sensor_drv_params_t* load_aux1_hi556_sensor_driver(void);

#endif

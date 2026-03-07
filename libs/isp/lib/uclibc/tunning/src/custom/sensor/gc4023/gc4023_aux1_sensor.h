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

#ifndef __GC4023_SENSOR_H__
#define __GC4023_SENSOR_H__

#include "sensor_common.h"
#include "log_utils.h"

#define				FRAME_RATE				(20)

isp_ret_t GC4023_Fill_Exposure_Reg(sensor_apply_exp_param_t * pExpParam, drv_regs_array_info_t * pRegInfo);
//GC4023 All-pixel scan CSI-2_2lane 27Mhz AD:10bit Output:10bit 704Mbps Master Mode 30.00fps Integration Time: Xx.xxms Gain:XdB vts = 1500 row time=22.22us
static drv_sensor_reg_info_t  gc4023_4mp_setting[] =
{	
  	{ .regAddr=0x03fe, .regData= 0xf0, .delayUs=0 },
	{ .regAddr=0x03fe, .regData= 0x00, .delayUs=0 },
	{ .regAddr=0x03fe, .regData= 0x10, .delayUs=0 },
	{ .regAddr=0x03fe, .regData= 0x00, .delayUs=0 },
	{ .regAddr=0x0a38, .regData= 0x00, .delayUs=0 },
	{ .regAddr=0x0a38, .regData= 0x01, .delayUs=0 },
	{ .regAddr=0x0a20, .regData= 0x17, .delayUs=0 },
	{ .regAddr=0x061c, .regData= 0x50, .delayUs=0 },
	{ .regAddr=0x061d, .regData= 0x21, .delayUs=0 },
	{ .regAddr=0x061e, .regData= 0x6c, .delayUs=0 },
	{ .regAddr=0x061f, .regData= 0x06, .delayUs=0 },
	{ .regAddr=0x0a21, .regData= 0x10, .delayUs=0 },
	{ .regAddr=0x0a34, .regData= 0x40, .delayUs=0 },
	{ .regAddr=0x0a35, .regData= 0x01, .delayUs=0 },
	{ .regAddr=0x0a36, .regData= 0x58, .delayUs=0 },
	{ .regAddr=0x0a37, .regData= 0x06, .delayUs=0 },
	{ .regAddr=0x0314, .regData= 0x50, .delayUs=0 },
	{ .regAddr=0x0315, .regData= 0x00, .delayUs=0 },
	{ .regAddr=0x031c, .regData= 0xce, .delayUs=0 },
	{ .regAddr=0x0219, .regData= 0x47, .delayUs=0 },
	{ .regAddr=0x0342, .regData= 0x04, .delayUs=0 }, //HMAX
	{ .regAddr=0x0343, .regData= 0xb0, .delayUs=0 },
	{ .regAddr=0x0259, .regData= 0x05, .delayUs=0 },
	{ .regAddr=0x025a, .regData= 0xa0, .delayUs=0 },
	#if (FRAME_RATE == 15)
	{ .regAddr=0x0340, .regData= 0x05, .delayUs=0 }, //VMAX
	{ .regAddr=0x0341, .regData= 0xdc, .delayUs=0 },
	#elif (FRAME_RATE == 20)
	{ .regAddr=0x0340, .regData= 0x08, .delayUs=0 }, //VMAX
	{ .regAddr=0x0341, .regData= 0xca, .delayUs=0 },
	#endif
	{ .regAddr=0x0347, .regData= 0x02, .delayUs=0 },
	{ .regAddr=0x0348, .regData= 0x0a, .delayUs=0 }, //2560+8
	{ .regAddr=0x0349, .regData= 0x08, .delayUs=0 },
	{ .regAddr=0x034a, .regData= 0x05, .delayUs=0 }, //1440+8
	{ .regAddr=0x034b, .regData= 0xa8, .delayUs=0 },
	{ .regAddr=0x0094, .regData= 0x0a, .delayUs=0 }, //2560
	{ .regAddr=0x0095, .regData= 0x00, .delayUs=0 },
	{ .regAddr=0x0096, .regData= 0x05, .delayUs=0 }, //1440
	{ .regAddr=0x0097, .regData= 0xa0, .delayUs=0 },
	{ .regAddr=0x0099, .regData= 0x04, .delayUs=0 },
	{ .regAddr=0x009b, .regData= 0x04, .delayUs=0 },
	{ .regAddr=0x060c, .regData= 0x01, .delayUs=0 },//
	{ .regAddr=0x060e, .regData= 0x08, .delayUs=0 },
	{ .regAddr=0x060f, .regData= 0x05, .delayUs=0 },
	{ .regAddr=0x070c, .regData= 0x01, .delayUs=0 },//
	{ .regAddr=0x070e, .regData= 0x08, .delayUs=0 },
	{ .regAddr=0x070f, .regData= 0x05, .delayUs=0 },
	{ .regAddr=0x0909, .regData= 0x03, .delayUs=0 },
	{ .regAddr=0x0902, .regData= 0x04, .delayUs=0 },
	{ .regAddr=0x0904, .regData= 0x0b, .delayUs=0 },
	{ .regAddr=0x0907, .regData= 0x54, .delayUs=0 },
	{ .regAddr=0x0908, .regData= 0x06, .delayUs=0 },
	{ .regAddr=0x0903, .regData= 0x9d, .delayUs=0 },
	{ .regAddr=0x072a, .regData= 0x18, .delayUs=0 },
	{ .regAddr=0x0724, .regData= 0x0a, .delayUs=0 },
	{ .regAddr=0x0727, .regData= 0x0a, .delayUs=0 },
	{ .regAddr=0x072a, .regData= 0x1c, .delayUs=0 },
	{ .regAddr=0x072b, .regData= 0x0a, .delayUs=0 },
	{ .regAddr=0x1466, .regData= 0x10, .delayUs=0 },
	{ .regAddr=0x1468, .regData= 0x15, .delayUs=0 },
	{ .regAddr=0x1467, .regData= 0x0d, .delayUs=0 },
	{ .regAddr=0x1469, .regData= 0x80, .delayUs=0 },
	{ .regAddr=0x146a, .regData= 0xe8, .delayUs=0 },
	{ .regAddr=0x0707, .regData= 0x07, .delayUs=0 },
	{ .regAddr=0x0737, .regData= 0x0f, .delayUs=0 },
	{ .regAddr=0x0704, .regData= 0x01, .delayUs=0 },
	{ .regAddr=0x0706, .regData= 0x02, .delayUs=0 },
	{ .regAddr=0x0716, .regData= 0x02, .delayUs=0 },
	{ .regAddr=0x0708, .regData= 0xc8, .delayUs=0 },
	{ .regAddr=0x0718, .regData= 0xc8, .delayUs=0 },
	{ .regAddr=0x061a, .regData= 0x00, .delayUs=0 },
	{ .regAddr=0x1430, .regData= 0x80, .delayUs=0 },
	{ .regAddr=0x1407, .regData= 0x10, .delayUs=0 },
	{ .regAddr=0x1408, .regData= 0x16, .delayUs=0 },
	{ .regAddr=0x1409, .regData= 0x03, .delayUs=0 },
	{ .regAddr=0x146d, .regData= 0x0e, .delayUs=0 },
	{ .regAddr=0x146e, .regData= 0x42, .delayUs=0 },
	{ .regAddr=0x146f, .regData= 0x43, .delayUs=0 },
	{ .regAddr=0x1470, .regData= 0x3c, .delayUs=0 },
	{ .regAddr=0x1471, .regData= 0x3d, .delayUs=0 },
	{ .regAddr=0x1472, .regData= 0x3a, .delayUs=0 },
	{ .regAddr=0x1473, .regData= 0x3a, .delayUs=0 },
	{ .regAddr=0x1474, .regData= 0x40, .delayUs=0 },
	{ .regAddr=0x1475, .regData= 0x46, .delayUs=0 },
	{ .regAddr=0x1420, .regData= 0x14, .delayUs=0 },
	{ .regAddr=0x1464, .regData= 0x15, .delayUs=0 },
	{ .regAddr=0x146c, .regData= 0x40, .delayUs=0 },
	{ .regAddr=0x146d, .regData= 0x40, .delayUs=0 },
	{ .regAddr=0x1423, .regData= 0x08, .delayUs=0 },
	{ .regAddr=0x1428, .regData= 0x10, .delayUs=0 },
	{ .regAddr=0x1462, .regData= 0x18, .delayUs=0 },
	{ .regAddr=0x02ce, .regData= 0x04, .delayUs=0 },
	{ .regAddr=0x143a, .regData= 0x0f, .delayUs=0 },
	{ .regAddr=0x142b, .regData= 0x88, .delayUs=0 },
	{ .regAddr=0x0245, .regData= 0xc9, .delayUs=0 },
	{ .regAddr=0x023a, .regData= 0x08, .delayUs=0 },
	{ .regAddr=0x02cd, .regData= 0x99, .delayUs=0 },
	{ .regAddr=0x0612, .regData= 0x02, .delayUs=0 },
	{ .regAddr=0x0613, .regData= 0xc7, .delayUs=0 },
	{ .regAddr=0x0243, .regData= 0x03, .delayUs=0 },
	{ .regAddr=0x021b, .regData= 0x09, .delayUs=0 },
	{ .regAddr=0x0089, .regData= 0x03, .delayUs=0 },
	{ .regAddr=0x0040, .regData= 0xa3, .delayUs=0 },
	{ .regAddr=0x0075, .regData= 0x64, .delayUs=0 },
	{ .regAddr=0x0004, .regData= 0x0f, .delayUs=0 },
	{ .regAddr=0x0002, .regData= 0xab, .delayUs=0 },
	{ .regAddr=0x0053, .regData= 0x0a, .delayUs=0 },
	{ .regAddr=0x0205, .regData= 0x0c, .delayUs=0 },
	{ .regAddr=0x0202, .regData= 0x06, .delayUs=0 },
	{ .regAddr=0x0203, .regData= 0x27, .delayUs=0 },
	{ .regAddr=0x0614, .regData= 0x00, .delayUs=0 },
	{ .regAddr=0x0615, .regData= 0x00, .delayUs=0 },
	{ .regAddr=0x0181, .regData= 0x0c, .delayUs=0 },
	{ .regAddr=0x0182, .regData= 0x05, .delayUs=0 },
	{ .regAddr=0x0185, .regData= 0x01, .delayUs=0 },
	{ .regAddr=0x0180, .regData= 0x46, .delayUs=0 },
	{ .regAddr=0x0100, .regData= 0x08, .delayUs=0 },
	{ .regAddr=0x0106, .regData= 0x38, .delayUs=0 },
	{ .regAddr=0x010d, .regData= 0x80, .delayUs=0 },
	{ .regAddr=0x010e, .regData= 0x0c, .delayUs=0 },
	{ .regAddr=0x0113, .regData= 0x02, .delayUs=0 },
	{ .regAddr=0x0114, .regData= 0x01, .delayUs=0 },
	{ .regAddr=0x0115, .regData= 0x10, .delayUs=0 },
	//{ .regAddr=0x0100, .regData= 0x09, .delayUs=0 },
	{ .regAddr=0x0052, .regData= 0x02, .delayUs=0 },
	{ .regAddr=0x0076, .regData= 0x01, .delayUs=0 },
	{ .regAddr=0x021a, .regData= 0x10, .delayUs=0 },
	{ .regAddr=0x0434, .regData= 0x75, .delayUs=0 },
	{ .regAddr=0x0435, .regData= 0x75, .delayUs=0 },
	{ .regAddr=0x0436, .regData= 0x75, .delayUs=0 },
	{ .regAddr=0x0437, .regData= 0x75, .delayUs=0 },
	{ .regAddr=0x0430, .regData= 0x0a, .delayUs=0 },
	{ .regAddr=0x0431, .regData= 0x0a, .delayUs=0 },
	{ .regAddr=0x0432, .regData= 0x0a, .delayUs=0 },
	{ .regAddr=0x0433, .regData= 0x0a, .delayUs=0 },
	{ .regAddr=0x0458, .regData= 0x00, .delayUs=0 },
	{ .regAddr=0x0459, .regData= 0x00, .delayUs=0 },
	{ .regAddr=0x045a, .regData= 0x00, .delayUs=0 },
	{ .regAddr=0x045b, .regData= 0x00, .delayUs=0 },

	{ .regAddr=0x0a67, .regData= 0x80, .delayUs=0 },
	{ .regAddr=0x0a54, .regData= 0x0e, .delayUs=0 },
	{ .regAddr=0x0a65, .regData= 0x10, .delayUs=0 },
	{ .regAddr=0x0a98, .regData= 0x10, .delayUs=0 },
	{ .regAddr=0x05be, .regData= 0x00, .delayUs=0 },
	{ .regAddr=0x05a9, .regData= 0x01, .delayUs=0 },
	{ .regAddr=0x0029, .regData= 0x08, .delayUs=0 },
	{ .regAddr=0x002b, .regData= 0xa8, .delayUs=0 },
	{ .regAddr=0x0a83, .regData= 0xe0, .delayUs=0 },
	{ .regAddr=0x0a72, .regData= 0x02, .delayUs=0 },
	{ .regAddr=0x0a75, .regData= 0x41, .delayUs=0 },
	{ .regAddr=0x0a70, .regData= 0x03, .delayUs=0 },
  //Horizontal mirror and vertical FLIP 
 	//{ .regAddr=0x022c, .regData= 0x03, .delayUs=0 },
	{ .regAddr=0x0a5a, .regData= 0x80, .delayUs=20000 },
	/*wait 20ms*/
	{ .regAddr=0x05be, .regData= 0x01, .delayUs=0 },
	{ .regAddr=0x0a70, .regData= 0x00, .delayUs=0 },
	{ .regAddr=0x0080, .regData= 0x02, .delayUs=0 },
	{ .regAddr=0x0a67, .regData= 0x00, .delayUs=0 },
};

//GC4023 All-pixel scan CSI-2_2lane 24Mhz AD:10bit Output:10bit 352Mbps Master Mode 15.00fps Integration Time: Xx.xxms Gain:XdB vts = 1500 row time=44.44us
static drv_sensor_reg_info_t  gc4023_4mp_15fps_setting[] =
{
	{ .regAddr=0x03fe, .regData= 0xf0, .delayUs=0 },
	{ .regAddr=0x03fe, .regData= 0x00, .delayUs=0 },
	{ .regAddr=0x03fe, .regData= 0x10, .delayUs=0 },
	{ .regAddr=0x03fe, .regData= 0x00, .delayUs=0 },
	{ .regAddr=0x0a38, .regData= 0x00, .delayUs=0 },
	{ .regAddr=0x0a38, .regData= 0x03, .delayUs=0 },
	{ .regAddr=0x0a20, .regData= 0x17, .delayUs=0 },
	{ .regAddr=0x061c, .regData= 0x50, .delayUs=0 },
	{ .regAddr=0x061d, .regData= 0x21, .delayUs=0 },
	{ .regAddr=0x061e, .regData= 0x6c, .delayUs=0 },
	{ .regAddr=0x061f, .regData= 0x06, .delayUs=0 },
	{ .regAddr=0x0a21, .regData= 0x10, .delayUs=0 },
	{ .regAddr=0x0a34, .regData= 0x40, .delayUs=0 },
	{ .regAddr=0x0a35, .regData= 0x01, .delayUs=0 },
	{ .regAddr=0x0a36, .regData= 0x58, .delayUs=0 },
	{ .regAddr=0x0a37, .regData= 0x06, .delayUs=0 },
	{ .regAddr=0x0314, .regData= 0x50, .delayUs=0 },
	{ .regAddr=0x0315, .regData= 0x00, .delayUs=0 },
	{ .regAddr=0x031c, .regData= 0xce, .delayUs=0 },
	{ .regAddr=0x0219, .regData= 0x47, .delayUs=0 },
	{ .regAddr=0x0259, .regData= 0x05, .delayUs=0 },
	{ .regAddr=0x025a, .regData= 0xa0, .delayUs=0 },

	{ .regAddr=0x0340, .regData= 0x05, .delayUs=0 }, //VMAX
	{ .regAddr=0x0341, .regData= 0xdc, .delayUs=0 },
	{ .regAddr=0x0342, .regData= 0x04, .delayUs=0 }, //HMAX
	{ .regAddr=0x0343, .regData= 0xb0, .delayUs=0 },
	{ .regAddr=0x0346, .regData= 0x00, .delayUs=0 }, //row start
	{ .regAddr=0x0347, .regData= 0x02, .delayUs=0 },
	{ .regAddr=0x060c, .regData= 0x01, .delayUs=0 }, //col start
	{ .regAddr=0x070c, .regData= 0x01, .delayUs=0 },
	{ .regAddr=0x0348, .regData= 0x0a, .delayUs=0 }, //win_width 2560+8
	{ .regAddr=0x0349, .regData= 0x08, .delayUs=0 },
	{ .regAddr=0x034a, .regData= 0x05, .delayUs=0 }, //win_height 1440+8
	{ .regAddr=0x034b, .regData= 0xa8, .delayUs=0 },
	{ .regAddr=0x0094, .regData= 0x0a, .delayUs=0 }, //out_win_width 2560
	{ .regAddr=0x0095, .regData= 0x00, .delayUs=0 },
	{ .regAddr=0x0096, .regData= 0x05, .delayUs=0 }, //out_win_height 1440
	{ .regAddr=0x0097, .regData= 0xa0, .delayUs=0 },
	{ .regAddr=0x0099, .regData= 0x04, .delayUs=0 },
	{ .regAddr=0x009b, .regData= 0x04, .delayUs=0 },

	{ .regAddr=0x0709, .regData= 0x40, .delayUs=0 },
	{ .regAddr=0x0719, .regData= 0x40, .delayUs=0 },
	{ .regAddr=0x060e, .regData= 0x08, .delayUs=0 },
	{ .regAddr=0x060f, .regData= 0x05, .delayUs=0 },
	{ .regAddr=0x070e, .regData= 0x08, .delayUs=0 },
	{ .regAddr=0x070f, .regData= 0x05, .delayUs=0 },
	{ .regAddr=0x0909, .regData= 0x03, .delayUs=0 },
	{ .regAddr=0x0902, .regData= 0x04, .delayUs=0 },
	{ .regAddr=0x0904, .regData= 0x0b, .delayUs=0 },
	{ .regAddr=0x0907, .regData= 0x54, .delayUs=0 },
	{ .regAddr=0x0908, .regData= 0x06, .delayUs=0 },
	{ .regAddr=0x0903, .regData= 0x9d, .delayUs=0 },
	{ .regAddr=0x072a, .regData= 0x18, .delayUs=0 },
	{ .regAddr=0x0724, .regData= 0x0a, .delayUs=0 },
	{ .regAddr=0x0727, .regData= 0x0a, .delayUs=0 },
	{ .regAddr=0x072a, .regData= 0x1c, .delayUs=0 },
	{ .regAddr=0x072b, .regData= 0x0a, .delayUs=0 },
	{ .regAddr=0x1466, .regData= 0x10, .delayUs=0 },
	{ .regAddr=0x1468, .regData= 0x18, .delayUs=0 },
	{ .regAddr=0x1467, .regData= 0x18, .delayUs=0 },
	{ .regAddr=0x1469, .regData= 0x80, .delayUs=0 },
	{ .regAddr=0x146a, .regData= 0xe8, .delayUs=0 },
	{ .regAddr=0x0707, .regData= 0x07, .delayUs=0 },
	{ .regAddr=0x0737, .regData= 0x0f, .delayUs=0 },
	{ .regAddr=0x0704, .regData= 0x01, .delayUs=0 },
	{ .regAddr=0x0706, .regData= 0x02, .delayUs=0 },
	{ .regAddr=0x0716, .regData= 0x02, .delayUs=0 },
	{ .regAddr=0x0708, .regData= 0xc8, .delayUs=0 },
	{ .regAddr=0x0718, .regData= 0xc8, .delayUs=0 },

	{ .regAddr=0x061a, .regData= 0x00, .delayUs=0 },
	{ .regAddr=0x1430, .regData= 0x80, .delayUs=0 },
	{ .regAddr=0x1407, .regData= 0x10, .delayUs=0 },
	{ .regAddr=0x1408, .regData= 0x16, .delayUs=0 },
	{ .regAddr=0x1409, .regData= 0x03, .delayUs=0 },
	{ .regAddr=0x146d, .regData= 0x0e, .delayUs=0 },
	{ .regAddr=0x146e, .regData= 0x42, .delayUs=0 },
	{ .regAddr=0x146f, .regData= 0x43, .delayUs=0 },
	{ .regAddr=0x1470, .regData= 0x3c, .delayUs=0 },
	{ .regAddr=0x1471, .regData= 0x3d, .delayUs=0 },
	{ .regAddr=0x1472, .regData= 0x3a, .delayUs=0 },
	{ .regAddr=0x1473, .regData= 0x3a, .delayUs=0 },
	{ .regAddr=0x1474, .regData= 0x40, .delayUs=0 },
	{ .regAddr=0x1475, .regData= 0x46, .delayUs=0 },
	{ .regAddr=0x1420, .regData= 0x14, .delayUs=0 },
	{ .regAddr=0x1464, .regData= 0x15, .delayUs=0 },
	{ .regAddr=0x146c, .regData= 0x40, .delayUs=0 },
	{ .regAddr=0x146d, .regData= 0x40, .delayUs=0 },
	{ .regAddr=0x1423, .regData= 0x08, .delayUs=0 },
	{ .regAddr=0x1428, .regData= 0x10, .delayUs=0 },
	{ .regAddr=0x1462, .regData= 0x18, .delayUs=0 },
	{ .regAddr=0x02ce, .regData= 0x04, .delayUs=0 },
	{ .regAddr=0x143a, .regData= 0x0f, .delayUs=0 },
	{ .regAddr=0x142b, .regData= 0x88, .delayUs=0 },
	{ .regAddr=0x0245, .regData= 0xc9, .delayUs=0 },
	{ .regAddr=0x023a, .regData= 0x08, .delayUs=0 },
	{ .regAddr=0x02cd, .regData= 0x92, .delayUs=0 },
	{ .regAddr=0x0612, .regData= 0x02, .delayUs=0 },
	{ .regAddr=0x0613, .regData= 0xc7, .delayUs=0 },
	{ .regAddr=0x0243, .regData= 0x03, .delayUs=0 },
	{ .regAddr=0x021b, .regData= 0x09, .delayUs=0 },
	{ .regAddr=0x0089, .regData= 0x03, .delayUs=0 },
	{ .regAddr=0x0040, .regData= 0xa3, .delayUs=0 },
	{ .regAddr=0x0075, .regData= 0x64, .delayUs=0 },
	{ .regAddr=0x0004, .regData= 0x0f, .delayUs=0 },
	{ .regAddr=0x0002, .regData= 0xab, .delayUs=0 },
	{ .regAddr=0x0053, .regData= 0x0a, .delayUs=0 },
	{ .regAddr=0x0205, .regData= 0x0c, .delayUs=0 },
	{ .regAddr=0x0202, .regData= 0x06, .delayUs=0 },
	{ .regAddr=0x0203, .regData= 0x27, .delayUs=0 },
	{ .regAddr=0x0614, .regData= 0x00, .delayUs=0 },
	{ .regAddr=0x0615, .regData= 0x00, .delayUs=0 },
	{ .regAddr=0x0181, .regData= 0x0c, .delayUs=0 },
	{ .regAddr=0x0182, .regData= 0x05, .delayUs=0 },
	{ .regAddr=0x0185, .regData= 0x01, .delayUs=0 },
	{ .regAddr=0x0180, .regData= 0x46, .delayUs=0 },
	{ .regAddr=0x0100, .regData= 0x08, .delayUs=0 },
	{ .regAddr=0x0106, .regData= 0x38, .delayUs=0 },
	{ .regAddr=0x010d, .regData= 0x80, .delayUs=0 },
	{ .regAddr=0x010e, .regData= 0x0c, .delayUs=0 },
	{ .regAddr=0x0113, .regData= 0x02, .delayUs=0 },
	{ .regAddr=0x0114, .regData= 0x01, .delayUs=0 },
	{ .regAddr=0x0115, .regData= 0x10, .delayUs=0 },
	//{ .regAddr=0x0100, .regData= 0x09, .delayUs=0 },

	{ .regAddr=0x0052, .regData= 0x02, .delayUs=0 },
	{ .regAddr=0x0076, .regData= 0x01, .delayUs=0 },
	{ .regAddr=0x021a, .regData= 0x10, .delayUs=0 },
	{ .regAddr=0x0434, .regData= 0x75, .delayUs=0 },
	{ .regAddr=0x0435, .regData= 0x75, .delayUs=0 },
	{ .regAddr=0x0436, .regData= 0x75, .delayUs=0 },
	{ .regAddr=0x0437, .regData= 0x75, .delayUs=0 },
	{ .regAddr=0x0430, .regData= 0x0a, .delayUs=0 },
	{ .regAddr=0x0431, .regData= 0x0a, .delayUs=0 },
	{ .regAddr=0x0432, .regData= 0x0a, .delayUs=0 },
	{ .regAddr=0x0433, .regData= 0x0a, .delayUs=0 },
	{ .regAddr=0x0458, .regData= 0x00, .delayUs=0 },
	{ .regAddr=0x0459, .regData= 0x00, .delayUs=0 },
	{ .regAddr=0x045a, .regData= 0x00, .delayUs=0 },
	{ .regAddr=0x045b, .regData= 0x00, .delayUs=0 },

	{ .regAddr=0x0a67, .regData= 0x80, .delayUs=0 },
	{ .regAddr=0x0a54, .regData= 0x0e, .delayUs=0 },
	{ .regAddr=0x0a65, .regData= 0x10, .delayUs=0 },
	{ .regAddr=0x0a98, .regData= 0x10, .delayUs=0 },
	{ .regAddr=0x05be, .regData= 0x00, .delayUs=0 },
	{ .regAddr=0x05a9, .regData= 0x01, .delayUs=0 },
	{ .regAddr=0x0029, .regData= 0x08, .delayUs=0 },
	{ .regAddr=0x002b, .regData= 0xa8, .delayUs=0 },
	{ .regAddr=0x0a83, .regData= 0xe0, .delayUs=0 },
	{ .regAddr=0x0a72, .regData= 0x02, .delayUs=0 },
	{ .regAddr=0x0a73, .regData= 0x60, .delayUs=0 },
	{ .regAddr=0x0a75, .regData= 0x41, .delayUs=0 },
	{ .regAddr=0x0a70, .regData= 0x03, .delayUs=0 },
	{ .regAddr=0x0a5a, .regData= 0x80, .delayUs=20000 },
	/*wait 20ms*/
	{ .regAddr=0x05be, .regData= 0x01, .delayUs=0 },
	{ .regAddr=0x0a70, .regData= 0x00, .delayUs=0 },
	{ .regAddr=0x0080, .regData= 0x02, .delayUs=0 },
	{ .regAddr=0x0a67, .regData= 0x00, .delayUs=0 },
};



//GC4023 All-pixel scan CSI-2_2lane 27Mhz AD:10bit Output:10bit 704Mbps Master Mode 30.00fps Integration Time: Xx.xxms Gain:XdB vts = 1500 row time=22.22us
static drv_sensor_reg_info_t  gc4023_1080p_setting[] =
{
	{ .regAddr=0x03fe, .regData= 0xf0, .delayUs=0 },
	{ .regAddr=0x03fe, .regData= 0x00, .delayUs=0 },
	{ .regAddr=0x03fe, .regData= 0x10, .delayUs=0 },
	{ .regAddr=0x03fe, .regData= 0x00, .delayUs=0 },
	{ .regAddr=0x0a38, .regData= 0x00, .delayUs=0 },
	{ .regAddr=0x0a38, .regData= 0x01, .delayUs=0 },
	{ .regAddr=0x0a20, .regData= 0x17, .delayUs=0 },
	{ .regAddr=0x061c, .regData= 0x50, .delayUs=0 },
	{ .regAddr=0x061d, .regData= 0x21, .delayUs=0 },
	{ .regAddr=0x061e, .regData= 0x6c, .delayUs=0 },
	{ .regAddr=0x061f, .regData= 0x06, .delayUs=0 },
	{ .regAddr=0x0a21, .regData= 0x10, .delayUs=0 },
	{ .regAddr=0x0a34, .regData= 0x40, .delayUs=0 },
	{ .regAddr=0x0a35, .regData= 0x01, .delayUs=0 },
	{ .regAddr=0x0a36, .regData= 0x58, .delayUs=0 },
	{ .regAddr=0x0a37, .regData= 0x06, .delayUs=0 },
	{ .regAddr=0x0314, .regData= 0x50, .delayUs=0 },
	{ .regAddr=0x0315, .regData= 0x00, .delayUs=0 },
	{ .regAddr=0x031c, .regData= 0xce, .delayUs=0 },
	{ .regAddr=0x0219, .regData= 0x47, .delayUs=0 },
	{ .regAddr=0x0259, .regData= 0x05, .delayUs=0 },
	{ .regAddr=0x025a, .regData= 0xa0, .delayUs=0 },
	{ .regAddr=0x0340, .regData= 0x05, .delayUs=0 }, //VMAX
	{ .regAddr=0x0341, .regData= 0xdc, .delayUs=0 },
	{ .regAddr=0x0342, .regData= 0x04, .delayUs=0 }, //HMAX
	{ .regAddr=0x0343, .regData= 0xb0, .delayUs=0 },
	{ .regAddr=0x0346, .regData= 0x00, .delayUs=0 }, //row start
	{ .regAddr=0x0347, .regData= 0x02, .delayUs=0 },
	{ .regAddr=0x060c, .regData= 0x01, .delayUs=0 }, //col start
	{ .regAddr=0x070c, .regData= 0x01, .delayUs=0 },
	{ .regAddr=0x0348, .regData= 0x0a, .delayUs=0 }, //win_width 2560+8
	{ .regAddr=0x0349, .regData= 0x08, .delayUs=0 },
	{ .regAddr=0x034a, .regData= 0x05, .delayUs=0 }, //win_height 1440+8
	{ .regAddr=0x034b, .regData= 0xa8, .delayUs=0 },
	//{ .regAddr=0x0094, .regData= 0x0a, .delayUs=0 }, //out_win_width 2560
	//{ .regAddr=0x0095, .regData= 0x00, .delayUs=0 },
	{ .regAddr=0x0094, .regData= 0x07, .delayUs=0 },  //1920
	{ .regAddr=0x0095, .regData= 0x80, .delayUs=0 },
	//{ .regAddr=0x0096, .regData= 0x05, .delayUs=0 }, //out_win_height 1440
	//{ .regAddr=0x0097, .regData= 0xa0, .delayUs=0 },
	{ .regAddr=0x0096, .regData= 0x04, .delayUs=0 },  //1080
	{ .regAddr=0x0097, .regData= 0x38, .delayUs=0 },
	{ .regAddr=0x0099, .regData= 0x04, .delayUs=0 },
	{ .regAddr=0x009b, .regData= 0x04, .delayUs=0 },

	{ .regAddr=0x060e, .regData= 0x08, .delayUs=0 },
	{ .regAddr=0x060f, .regData= 0x05, .delayUs=0 },
	{ .regAddr=0x070e, .regData= 0x08, .delayUs=0 },
	{ .regAddr=0x070f, .regData= 0x05, .delayUs=0 },
	{ .regAddr=0x0909, .regData= 0x03, .delayUs=0 },
	{ .regAddr=0x0902, .regData= 0x04, .delayUs=0 },
	{ .regAddr=0x0904, .regData= 0x0b, .delayUs=0 },
	{ .regAddr=0x0907, .regData= 0x54, .delayUs=0 },
	{ .regAddr=0x0908, .regData= 0x06, .delayUs=0 },
	{ .regAddr=0x0903, .regData= 0x9d, .delayUs=0 },
	{ .regAddr=0x072a, .regData= 0x18, .delayUs=0 },
	{ .regAddr=0x0724, .regData= 0x0a, .delayUs=0 },
	{ .regAddr=0x0727, .regData= 0x0a, .delayUs=0 },
	{ .regAddr=0x072a, .regData= 0x1c, .delayUs=0 },
	{ .regAddr=0x072b, .regData= 0x0a, .delayUs=0 },
	{ .regAddr=0x1466, .regData= 0x10, .delayUs=0 },
	{ .regAddr=0x1468, .regData= 0x15, .delayUs=0 },
	{ .regAddr=0x1467, .regData= 0x0d, .delayUs=0 },
	{ .regAddr=0x1469, .regData= 0x80, .delayUs=0 },
	{ .regAddr=0x146a, .regData= 0xe8, .delayUs=0 },
	{ .regAddr=0x0707, .regData= 0x07, .delayUs=0 },
	{ .regAddr=0x0737, .regData= 0x0f, .delayUs=0 },
	{ .regAddr=0x0704, .regData= 0x01, .delayUs=0 },
	{ .regAddr=0x0706, .regData= 0x02, .delayUs=0 },
	{ .regAddr=0x0716, .regData= 0x02, .delayUs=0 },
	{ .regAddr=0x0708, .regData= 0xc8, .delayUs=0 },
	{ .regAddr=0x0718, .regData= 0xc8, .delayUs=0 },

	{ .regAddr=0x061a, .regData= 0x00, .delayUs=0 },
	{ .regAddr=0x1430, .regData= 0x80, .delayUs=0 },
	{ .regAddr=0x1407, .regData= 0x10, .delayUs=0 },
	{ .regAddr=0x1408, .regData= 0x16, .delayUs=0 },
	{ .regAddr=0x1409, .regData= 0x03, .delayUs=0 },
	{ .regAddr=0x146d, .regData= 0x0e, .delayUs=0 },
	{ .regAddr=0x146e, .regData= 0x42, .delayUs=0 },
	{ .regAddr=0x146f, .regData= 0x43, .delayUs=0 },
	{ .regAddr=0x1470, .regData= 0x3c, .delayUs=0 },
	{ .regAddr=0x1471, .regData= 0x3d, .delayUs=0 },
	{ .regAddr=0x1472, .regData= 0x3a, .delayUs=0 },
	{ .regAddr=0x1473, .regData= 0x3a, .delayUs=0 },
	{ .regAddr=0x1474, .regData= 0x40, .delayUs=0 },
	{ .regAddr=0x1475, .regData= 0x46, .delayUs=0 },
	{ .regAddr=0x1420, .regData= 0x14, .delayUs=0 },
	{ .regAddr=0x1464, .regData= 0x15, .delayUs=0 },
	{ .regAddr=0x146c, .regData= 0x40, .delayUs=0 },
	{ .regAddr=0x146d, .regData= 0x40, .delayUs=0 },
	{ .regAddr=0x1423, .regData= 0x08, .delayUs=0 },
	{ .regAddr=0x1428, .regData= 0x10, .delayUs=0 },
	{ .regAddr=0x1462, .regData= 0x18, .delayUs=0 },
	{ .regAddr=0x02ce, .regData= 0x04, .delayUs=0 },
	{ .regAddr=0x143a, .regData= 0x0f, .delayUs=0 },
	{ .regAddr=0x142b, .regData= 0x88, .delayUs=0 },
	{ .regAddr=0x0245, .regData= 0xc9, .delayUs=0 },
	{ .regAddr=0x023a, .regData= 0x08, .delayUs=0 },
	{ .regAddr=0x02cd, .regData= 0x99, .delayUs=0 },
	{ .regAddr=0x0612, .regData= 0x02, .delayUs=0 },
	{ .regAddr=0x0613, .regData= 0xc7, .delayUs=0 },
	{ .regAddr=0x0243, .regData= 0x03, .delayUs=0 },
	{ .regAddr=0x021b, .regData= 0x09, .delayUs=0 },
	{ .regAddr=0x0089, .regData= 0x03, .delayUs=0 },
	{ .regAddr=0x0040, .regData= 0xa3, .delayUs=0 },
	{ .regAddr=0x0075, .regData= 0x64, .delayUs=0 },
	{ .regAddr=0x0004, .regData= 0x0f, .delayUs=0 },
	{ .regAddr=0x0002, .regData= 0xab, .delayUs=0 },
	{ .regAddr=0x0053, .regData= 0x0a, .delayUs=0 },
	{ .regAddr=0x0205, .regData= 0x0c, .delayUs=0 },
	{ .regAddr=0x0202, .regData= 0x01, .delayUs=0 },
	{ .regAddr=0x0203, .regData= 0x27, .delayUs=0 },
	{ .regAddr=0x0614, .regData= 0x00, .delayUs=0 },
	{ .regAddr=0x0615, .regData= 0x00, .delayUs=0 },
	{ .regAddr=0x0181, .regData= 0x0c, .delayUs=0 },
	{ .regAddr=0x0182, .regData= 0x05, .delayUs=0 },
	{ .regAddr=0x0185, .regData= 0x01, .delayUs=0 },
	{ .regAddr=0x0180, .regData= 0x46, .delayUs=0 },
	{ .regAddr=0x0100, .regData= 0x08, .delayUs=0 },
	{ .regAddr=0x0106, .regData= 0x38, .delayUs=0 },
	//buffer size should be equal with 1920x1.25=2400=0x960
	//{ .regAddr=0x010d, .regData= 0x80, .delayUs=0 },
	//{ .regAddr=0x010e, .regData= 0x0c, .delayUs=0 },
	{ .regAddr=0x010d, .regData= 0x60, .delayUs=0 },  
	{ .regAddr=0x010e, .regData= 0x09, .delayUs=0 },
	{ .regAddr=0x0113, .regData= 0x02, .delayUs=0 },
	{ .regAddr=0x0114, .regData= 0x01, .delayUs=0 },
	{ .regAddr=0x0115, .regData= 0x10, .delayUs=0 },
	//{ .regAddr=0x0100, .regData= 0x09, .delayUs=0 },

	{ .regAddr=0x0052, .regData= 0x02, .delayUs=0 },
	{ .regAddr=0x0076, .regData= 0x01, .delayUs=0 },
	{ .regAddr=0x021a, .regData= 0x10, .delayUs=0 },
	{ .regAddr=0x0434, .regData= 0x75, .delayUs=0 },
	{ .regAddr=0x0435, .regData= 0x75, .delayUs=0 },
	{ .regAddr=0x0436, .regData= 0x75, .delayUs=0 },
	{ .regAddr=0x0437, .regData= 0x75, .delayUs=0 },
	{ .regAddr=0x0430, .regData= 0x0a, .delayUs=0 },
	{ .regAddr=0x0431, .regData= 0x0a, .delayUs=0 },
	{ .regAddr=0x0432, .regData= 0x0a, .delayUs=0 },
	{ .regAddr=0x0433, .regData= 0x0a, .delayUs=0 },
	{ .regAddr=0x0458, .regData= 0x00, .delayUs=0 },
	{ .regAddr=0x0459, .regData= 0x00, .delayUs=0 },
	{ .regAddr=0x045a, .regData= 0x00, .delayUs=0 },
	{ .regAddr=0x045b, .regData= 0x00, .delayUs=0 },

	{ .regAddr=0x0a67, .regData= 0x80, .delayUs=0 },
	{ .regAddr=0x0a54, .regData= 0x0e, .delayUs=0 },
	{ .regAddr=0x0a65, .regData= 0x10, .delayUs=0 },
	{ .regAddr=0x0a98, .regData= 0x10, .delayUs=0 },
	{ .regAddr=0x05be, .regData= 0x00, .delayUs=0 },
	{ .regAddr=0x05a9, .regData= 0x01, .delayUs=0 },
	{ .regAddr=0x0029, .regData= 0x08, .delayUs=0 },
	{ .regAddr=0x002b, .regData= 0xa8, .delayUs=0 },
	{ .regAddr=0x0a83, .regData= 0xe0, .delayUs=0 },
	{ .regAddr=0x0a72, .regData= 0x02, .delayUs=0 },
	{ .regAddr=0x0a75, .regData= 0x41, .delayUs=0 },
	{ .regAddr=0x0a70, .regData= 0x03, .delayUs=0 },
	{ .regAddr=0x0a5a, .regData= 0x80, .delayUs=20000 },
	/*wait 20ms*/
	{ .regAddr=0x05be, .regData= 0x01, .delayUs=0 },
	{ .regAddr=0x0a70, .regData= 0x00, .delayUs=0 },
	{ .regAddr=0x0080, .regData= 0x02, .delayUs=0 },
	{ .regAddr=0x0a67, .regData= 0x00, .delayUs=0 },
#if 0
	/*fsync master*/
	{ .regAddr=0x027f, .regData= 0x03, .delayUs=0 },
	{ .regAddr=0x02f7, .regData= 0x81, .delayUs=0 },
#endif
};

//GC4023 All-pixel scan CSI-2_2lane 24Mhz AD:10bit Output:10bit 352Mbps Master Mode 15.00fps Integration Time: Xx.xxms Gain:XdB vts = 1500 row time=44.44us
static drv_sensor_reg_info_t  gc4023_1080p_15fps_setting[] =
{
	{ .regAddr=0x03fe, .regData= 0xf0, .delayUs=0 },
	{ .regAddr=0x03fe, .regData= 0x00, .delayUs=0 },
	{ .regAddr=0x03fe, .regData= 0x10, .delayUs=0 },
	{ .regAddr=0x03fe, .regData= 0x00, .delayUs=0 },
	{ .regAddr=0x0a38, .regData= 0x00, .delayUs=0 },
	{ .regAddr=0x0a38, .regData= 0x03, .delayUs=0 },
	{ .regAddr=0x0a20, .regData= 0x17, .delayUs=0 },
	{ .regAddr=0x061c, .regData= 0x50, .delayUs=0 },
	{ .regAddr=0x061d, .regData= 0x21, .delayUs=0 },
	{ .regAddr=0x061e, .regData= 0x6c, .delayUs=0 },
	{ .regAddr=0x061f, .regData= 0x06, .delayUs=0 },
	{ .regAddr=0x0a21, .regData= 0x10, .delayUs=0 },
	{ .regAddr=0x0a34, .regData= 0x40, .delayUs=0 },
	{ .regAddr=0x0a35, .regData= 0x01, .delayUs=0 },
	{ .regAddr=0x0a36, .regData= 0x58, .delayUs=0 },
	{ .regAddr=0x0a37, .regData= 0x06, .delayUs=0 },
	{ .regAddr=0x0314, .regData= 0x50, .delayUs=0 },
	{ .regAddr=0x0315, .regData= 0x00, .delayUs=0 },
	{ .regAddr=0x031c, .regData= 0xce, .delayUs=0 },
	{ .regAddr=0x0219, .regData= 0x47, .delayUs=0 },
	{ .regAddr=0x0259, .regData= 0x05, .delayUs=0 },
	{ .regAddr=0x025a, .regData= 0xa0, .delayUs=0 },

	{ .regAddr=0x0340, .regData= 0x05, .delayUs=0 }, //VMAX
	{ .regAddr=0x0341, .regData= 0xdc, .delayUs=0 },
	{ .regAddr=0x0342, .regData= 0x04, .delayUs=0 }, //HMAX
	{ .regAddr=0x0343, .regData= 0xb0, .delayUs=0 },
	{ .regAddr=0x0346, .regData= 0x00, .delayUs=0 }, //row start
	{ .regAddr=0x0347, .regData= 0x02, .delayUs=0 },
	{ .regAddr=0x060c, .regData= 0x01, .delayUs=0 }, //col start
	{ .regAddr=0x070c, .regData= 0x01, .delayUs=0 },
	{ .regAddr=0x0348, .regData= 0x0a, .delayUs=0 }, //win_width 2560+8
	{ .regAddr=0x0349, .regData= 0x08, .delayUs=0 },
	{ .regAddr=0x034a, .regData= 0x05, .delayUs=0 }, //win_height 1440+8
	{ .regAddr=0x034b, .regData= 0xa8, .delayUs=0 },
	//{ .regAddr=0x0094, .regData= 0x0a, .delayUs=0 }, //out_win_width 2560
	//{ .regAddr=0x0095, .regData= 0x00, .delayUs=0 },
	//{ .regAddr=0x0096, .regData= 0x05, .delayUs=0 }, //out_win_height 1440
	//{ .regAddr=0x0097, .regData= 0xa0, .delayUs=0 },
	{ .regAddr=0x0094, .regData= 0x07, .delayUs=0 },  //1920
	{ .regAddr=0x0095, .regData= 0x80, .delayUs=0 },
	{ .regAddr=0x0096, .regData= 0x04, .delayUs=0 },  //1080
	{ .regAddr=0x0097, .regData= 0x38, .delayUs=0 },

	{ .regAddr=0x0099, .regData= 0x04, .delayUs=0 },
	{ .regAddr=0x009b, .regData= 0x04, .delayUs=0 },

	{ .regAddr=0x0709, .regData= 0x40, .delayUs=0 },
	{ .regAddr=0x0719, .regData= 0x40, .delayUs=0 },
	{ .regAddr=0x060e, .regData= 0x08, .delayUs=0 },
	{ .regAddr=0x060f, .regData= 0x05, .delayUs=0 },
	{ .regAddr=0x070e, .regData= 0x08, .delayUs=0 },
	{ .regAddr=0x070f, .regData= 0x05, .delayUs=0 },
	{ .regAddr=0x0909, .regData= 0x03, .delayUs=0 },
	{ .regAddr=0x0902, .regData= 0x04, .delayUs=0 },
	{ .regAddr=0x0904, .regData= 0x0b, .delayUs=0 },
	{ .regAddr=0x0907, .regData= 0x54, .delayUs=0 },
	{ .regAddr=0x0908, .regData= 0x06, .delayUs=0 },
	{ .regAddr=0x0903, .regData= 0x9d, .delayUs=0 },
	{ .regAddr=0x072a, .regData= 0x18, .delayUs=0 },
	{ .regAddr=0x0724, .regData= 0x0a, .delayUs=0 },
	{ .regAddr=0x0727, .regData= 0x0a, .delayUs=0 },
	{ .regAddr=0x072a, .regData= 0x1c, .delayUs=0 },
	{ .regAddr=0x072b, .regData= 0x0a, .delayUs=0 },
	{ .regAddr=0x1466, .regData= 0x10, .delayUs=0 },
	{ .regAddr=0x1468, .regData= 0x18, .delayUs=0 },
	{ .regAddr=0x1467, .regData= 0x18, .delayUs=0 },
	{ .regAddr=0x1469, .regData= 0x80, .delayUs=0 },
	{ .regAddr=0x146a, .regData= 0xe8, .delayUs=0 },
	{ .regAddr=0x0707, .regData= 0x07, .delayUs=0 },
	{ .regAddr=0x0737, .regData= 0x0f, .delayUs=0 },
	{ .regAddr=0x0704, .regData= 0x01, .delayUs=0 },
	{ .regAddr=0x0706, .regData= 0x02, .delayUs=0 },
	{ .regAddr=0x0716, .regData= 0x02, .delayUs=0 },
	{ .regAddr=0x0708, .regData= 0xc8, .delayUs=0 },
	{ .regAddr=0x0718, .regData= 0xc8, .delayUs=0 },

	{ .regAddr=0x061a, .regData= 0x00, .delayUs=0 },
	{ .regAddr=0x1430, .regData= 0x80, .delayUs=0 },
	{ .regAddr=0x1407, .regData= 0x10, .delayUs=0 },
	{ .regAddr=0x1408, .regData= 0x16, .delayUs=0 },
	{ .regAddr=0x1409, .regData= 0x03, .delayUs=0 },
	{ .regAddr=0x146d, .regData= 0x0e, .delayUs=0 },
	{ .regAddr=0x146e, .regData= 0x42, .delayUs=0 },
	{ .regAddr=0x146f, .regData= 0x43, .delayUs=0 },
	{ .regAddr=0x1470, .regData= 0x3c, .delayUs=0 },
	{ .regAddr=0x1471, .regData= 0x3d, .delayUs=0 },
	{ .regAddr=0x1472, .regData= 0x3a, .delayUs=0 },
	{ .regAddr=0x1473, .regData= 0x3a, .delayUs=0 },
	{ .regAddr=0x1474, .regData= 0x40, .delayUs=0 },
	{ .regAddr=0x1475, .regData= 0x46, .delayUs=0 },
	{ .regAddr=0x1420, .regData= 0x14, .delayUs=0 },
	{ .regAddr=0x1464, .regData= 0x15, .delayUs=0 },
	{ .regAddr=0x146c, .regData= 0x40, .delayUs=0 },
	{ .regAddr=0x146d, .regData= 0x40, .delayUs=0 },
	{ .regAddr=0x1423, .regData= 0x08, .delayUs=0 },
	{ .regAddr=0x1428, .regData= 0x10, .delayUs=0 },
	{ .regAddr=0x1462, .regData= 0x18, .delayUs=0 },
	{ .regAddr=0x02ce, .regData= 0x04, .delayUs=0 },
	{ .regAddr=0x143a, .regData= 0x0f, .delayUs=0 },
	{ .regAddr=0x142b, .regData= 0x88, .delayUs=0 },
	{ .regAddr=0x0245, .regData= 0xc9, .delayUs=0 },
	{ .regAddr=0x023a, .regData= 0x08, .delayUs=0 },
	{ .regAddr=0x02cd, .regData= 0x92, .delayUs=0 },
	{ .regAddr=0x0612, .regData= 0x02, .delayUs=0 },
	{ .regAddr=0x0613, .regData= 0xc7, .delayUs=0 },
	{ .regAddr=0x0243, .regData= 0x03, .delayUs=0 },
	{ .regAddr=0x021b, .regData= 0x09, .delayUs=0 },
	{ .regAddr=0x0089, .regData= 0x03, .delayUs=0 },
	{ .regAddr=0x0040, .regData= 0xa3, .delayUs=0 },
	{ .regAddr=0x0075, .regData= 0x64, .delayUs=0 },
	{ .regAddr=0x0004, .regData= 0x0f, .delayUs=0 },
	{ .regAddr=0x0002, .regData= 0xab, .delayUs=0 },
	{ .regAddr=0x0053, .regData= 0x0a, .delayUs=0 },
	{ .regAddr=0x0205, .regData= 0x0c, .delayUs=0 },
	{ .regAddr=0x0202, .regData= 0x06, .delayUs=0 },
	{ .regAddr=0x0203, .regData= 0x27, .delayUs=0 },
	{ .regAddr=0x0614, .regData= 0x00, .delayUs=0 },
	{ .regAddr=0x0615, .regData= 0x00, .delayUs=0 },
	{ .regAddr=0x0181, .regData= 0x0c, .delayUs=0 },
	{ .regAddr=0x0182, .regData= 0x05, .delayUs=0 },
	{ .regAddr=0x0185, .regData= 0x01, .delayUs=0 },
	{ .regAddr=0x0180, .regData= 0x46, .delayUs=0 },
	{ .regAddr=0x0100, .regData= 0x08, .delayUs=0 },
	{ .regAddr=0x0106, .regData= 0x38, .delayUs=0 },
	//buffer size should be equal with 1920x1.25=2400=0x960
	//{ .regAddr=0x010d, .regData= 0x80, .delayUs=0 },
	//{ .regAddr=0x010e, .regData= 0x0c, .delayUs=0 },
	{ .regAddr=0x010d, .regData= 0x60, .delayUs=0 },  
	{ .regAddr=0x010e, .regData= 0x09, .delayUs=0 },

	{ .regAddr=0x0113, .regData= 0x02, .delayUs=0 },
	{ .regAddr=0x0114, .regData= 0x01, .delayUs=0 },
	{ .regAddr=0x0115, .regData= 0x10, .delayUs=0 },

	//{ .regAddr=0x0100, .regData= 0x09, .delayUs=0 },

	{ .regAddr=0x0052, .regData= 0x02, .delayUs=0 },
	{ .regAddr=0x0076, .regData= 0x01, .delayUs=0 },
	{ .regAddr=0x021a, .regData= 0x10, .delayUs=0 },
	{ .regAddr=0x0434, .regData= 0x75, .delayUs=0 },
	{ .regAddr=0x0435, .regData= 0x75, .delayUs=0 },
	{ .regAddr=0x0436, .regData= 0x75, .delayUs=0 },
	{ .regAddr=0x0437, .regData= 0x75, .delayUs=0 },
	{ .regAddr=0x0430, .regData= 0x0a, .delayUs=0 },
	{ .regAddr=0x0431, .regData= 0x0a, .delayUs=0 },
	{ .regAddr=0x0432, .regData= 0x0a, .delayUs=0 },
	{ .regAddr=0x0433, .regData= 0x0a, .delayUs=0 },
	{ .regAddr=0x0458, .regData= 0x00, .delayUs=0 },
	{ .regAddr=0x0459, .regData= 0x00, .delayUs=0 },
	{ .regAddr=0x045a, .regData= 0x00, .delayUs=0 },
	{ .regAddr=0x045b, .regData= 0x00, .delayUs=0 },

	{ .regAddr=0x0a67, .regData= 0x80, .delayUs=0 },
	{ .regAddr=0x0a54, .regData= 0x0e, .delayUs=0 },
	{ .regAddr=0x0a65, .regData= 0x10, .delayUs=0 },
	{ .regAddr=0x0a98, .regData= 0x10, .delayUs=0 },
	{ .regAddr=0x05be, .regData= 0x00, .delayUs=0 },
	{ .regAddr=0x05a9, .regData= 0x01, .delayUs=0 },
	{ .regAddr=0x0029, .regData= 0x08, .delayUs=0 },
	{ .regAddr=0x002b, .regData= 0xa8, .delayUs=0 },
	{ .regAddr=0x0a83, .regData= 0xe0, .delayUs=0 },
	{ .regAddr=0x0a72, .regData= 0x02, .delayUs=0 },
	{ .regAddr=0x0a73, .regData= 0x60, .delayUs=0 },
	{ .regAddr=0x0a75, .regData= 0x41, .delayUs=0 },
	{ .regAddr=0x0a70, .regData= 0x03, .delayUs=0 },
	{ .regAddr=0x0a5a, .regData= 0x80, .delayUs=20000 },
	/*wait 20ms*/
	{ .regAddr=0x05be, .regData= 0x01, .delayUs=0 },
	{ .regAddr=0x0a70, .regData= 0x00, .delayUs=0 },
	{ .regAddr=0x0080, .regData= 0x02, .delayUs=0 },
	{ .regAddr=0x0a67, .regData= 0x00, .delayUs=0 },
};

//GC4023 All-pixel scan CSI-2_2lane 24Mhz AD:10bit Output:10bit 704Mbps Master Mode 30.00fps Integration Time: Xx.xxms Gain:XdB vts = 1500 row time=22.22us
static drv_sensor_reg_info_t  gc4023_4mp_10fps_setting[] =
{
	{ .regAddr=0x03fe, .regData= 0xf0, .delayUs=0 },
	{ .regAddr=0x03fe, .regData= 0x00, .delayUs=0 },
	{ .regAddr=0x03fe, .regData= 0x10, .delayUs=0 },
	{ .regAddr=0x03fe, .regData= 0x00, .delayUs=0 },
	{ .regAddr=0x0a38, .regData= 0x00, .delayUs=0 },
	{ .regAddr=0x0a38, .regData= 0x01, .delayUs=0 },
	{ .regAddr=0x0a20, .regData= 0x17, .delayUs=0 },
	{ .regAddr=0x061c, .regData= 0x50, .delayUs=0 },
	{ .regAddr=0x061d, .regData= 0x21, .delayUs=0 },
	{ .regAddr=0x061e, .regData= 0x6c, .delayUs=0 },
	{ .regAddr=0x061f, .regData= 0x06, .delayUs=0 },
	{ .regAddr=0x0a21, .regData= 0x10, .delayUs=0 },
	{ .regAddr=0x0a34, .regData= 0x40, .delayUs=0 },
	{ .regAddr=0x0a35, .regData= 0x01, .delayUs=0 },
	{ .regAddr=0x0a36, .regData= 0x58, .delayUs=0 },
	{ .regAddr=0x0a37, .regData= 0x06, .delayUs=0 },
	{ .regAddr=0x0314, .regData= 0x50, .delayUs=0 },
	{ .regAddr=0x0315, .regData= 0x00, .delayUs=0 },
	{ .regAddr=0x031c, .regData= 0xce, .delayUs=0 },
	{ .regAddr=0x0219, .regData= 0x47, .delayUs=0 },
	{ .regAddr=0x0342, .regData= 0x0e, .delayUs=0 }, //HMAX
	{ .regAddr=0x0343, .regData= 0x00, .delayUs=0 },
	{ .regAddr=0x0259, .regData= 0x05, .delayUs=0 },
	{ .regAddr=0x025a, .regData= 0xa0, .delayUs=0 },
	{ .regAddr=0x0340, .regData= 0x05, .delayUs=0 }, //VMAX
	{ .regAddr=0x0341, .regData= 0xdc, .delayUs=0 },
	{ .regAddr=0x0347, .regData= 0x02, .delayUs=0 },
	{ .regAddr=0x0348, .regData= 0x0a, .delayUs=0 }, //2560+8
	{ .regAddr=0x0349, .regData= 0x08, .delayUs=0 },
	{ .regAddr=0x034a, .regData= 0x05, .delayUs=0 }, //1440+8
	{ .regAddr=0x034b, .regData= 0xa8, .delayUs=0 },
	{ .regAddr=0x0094, .regData= 0x0a, .delayUs=0 }, //2560
	{ .regAddr=0x0095, .regData= 0x00, .delayUs=0 },
	{ .regAddr=0x0096, .regData= 0x05, .delayUs=0 }, //1440
	{ .regAddr=0x0097, .regData= 0xa0, .delayUs=0 },
	{ .regAddr=0x0099, .regData= 0x04, .delayUs=0 },
	{ .regAddr=0x009b, .regData= 0x04, .delayUs=0 },
	{ .regAddr=0x060c, .regData= 0x01, .delayUs=0 },//
	{ .regAddr=0x060e, .regData= 0x08, .delayUs=0 },
	{ .regAddr=0x060f, .regData= 0x05, .delayUs=0 },
	{ .regAddr=0x070c, .regData= 0x01, .delayUs=0 },//
	{ .regAddr=0x070e, .regData= 0x08, .delayUs=0 },
	{ .regAddr=0x070f, .regData= 0x05, .delayUs=0 },
	{ .regAddr=0x0909, .regData= 0x03, .delayUs=0 },
	{ .regAddr=0x0902, .regData= 0x04, .delayUs=0 },
	{ .regAddr=0x0904, .regData= 0x0b, .delayUs=0 },
	{ .regAddr=0x0907, .regData= 0x54, .delayUs=0 },
	{ .regAddr=0x0908, .regData= 0x06, .delayUs=0 },
	{ .regAddr=0x0903, .regData= 0x9d, .delayUs=0 },
	{ .regAddr=0x072a, .regData= 0x18, .delayUs=0 },
	{ .regAddr=0x0724, .regData= 0x0a, .delayUs=0 },
	{ .regAddr=0x0727, .regData= 0x0a, .delayUs=0 },
	{ .regAddr=0x072a, .regData= 0x1c, .delayUs=0 },
	{ .regAddr=0x072b, .regData= 0x0a, .delayUs=0 },
	{ .regAddr=0x1466, .regData= 0x10, .delayUs=0 },
	{ .regAddr=0x1468, .regData= 0x15, .delayUs=0 },
	{ .regAddr=0x1467, .regData= 0x0d, .delayUs=0 },
	{ .regAddr=0x1469, .regData= 0x80, .delayUs=0 },
	{ .regAddr=0x146a, .regData= 0xe8, .delayUs=0 },
	{ .regAddr=0x0707, .regData= 0x07, .delayUs=0 },
	{ .regAddr=0x0737, .regData= 0x0f, .delayUs=0 },
	{ .regAddr=0x0704, .regData= 0x01, .delayUs=0 },
	{ .regAddr=0x0706, .regData= 0x02, .delayUs=0 },
	{ .regAddr=0x0716, .regData= 0x02, .delayUs=0 },
	{ .regAddr=0x0708, .regData= 0xc8, .delayUs=0 },
	{ .regAddr=0x0718, .regData= 0xc8, .delayUs=0 },
	{ .regAddr=0x061a, .regData= 0x00, .delayUs=0 },
	{ .regAddr=0x1430, .regData= 0x80, .delayUs=0 },
	{ .regAddr=0x1407, .regData= 0x10, .delayUs=0 },
	{ .regAddr=0x1408, .regData= 0x16, .delayUs=0 },
	{ .regAddr=0x1409, .regData= 0x03, .delayUs=0 },
	{ .regAddr=0x146d, .regData= 0x0e, .delayUs=0 },
	{ .regAddr=0x146e, .regData= 0x42, .delayUs=0 },
	{ .regAddr=0x146f, .regData= 0x43, .delayUs=0 },
	{ .regAddr=0x1470, .regData= 0x3c, .delayUs=0 },
	{ .regAddr=0x1471, .regData= 0x3d, .delayUs=0 },
	{ .regAddr=0x1472, .regData= 0x3a, .delayUs=0 },
	{ .regAddr=0x1473, .regData= 0x3a, .delayUs=0 },
	{ .regAddr=0x1474, .regData= 0x40, .delayUs=0 },
	{ .regAddr=0x1475, .regData= 0x46, .delayUs=0 },
	{ .regAddr=0x1420, .regData= 0x14, .delayUs=0 },
	{ .regAddr=0x1464, .regData= 0x15, .delayUs=0 },
	{ .regAddr=0x146c, .regData= 0x40, .delayUs=0 },
	{ .regAddr=0x146d, .regData= 0x40, .delayUs=0 },
	{ .regAddr=0x1423, .regData= 0x08, .delayUs=0 },
	{ .regAddr=0x1428, .regData= 0x10, .delayUs=0 },
	{ .regAddr=0x1462, .regData= 0x18, .delayUs=0 },
	{ .regAddr=0x02ce, .regData= 0x04, .delayUs=0 },
	{ .regAddr=0x143a, .regData= 0x0f, .delayUs=0 },
	{ .regAddr=0x142b, .regData= 0x88, .delayUs=0 },
	{ .regAddr=0x0245, .regData= 0xc9, .delayUs=0 },
	{ .regAddr=0x023a, .regData= 0x08, .delayUs=0 },
	{ .regAddr=0x02cd, .regData= 0x99, .delayUs=0 },
	{ .regAddr=0x0612, .regData= 0x02, .delayUs=0 },
	{ .regAddr=0x0613, .regData= 0xc7, .delayUs=0 },
	{ .regAddr=0x0243, .regData= 0x03, .delayUs=0 },
	{ .regAddr=0x021b, .regData= 0x09, .delayUs=0 },
	{ .regAddr=0x0089, .regData= 0x03, .delayUs=0 },
	{ .regAddr=0x0040, .regData= 0xa3, .delayUs=0 },
	{ .regAddr=0x0075, .regData= 0x64, .delayUs=0 },
	{ .regAddr=0x0004, .regData= 0x0f, .delayUs=0 },
	{ .regAddr=0x0002, .regData= 0xab, .delayUs=0 },
	{ .regAddr=0x0053, .regData= 0x0a, .delayUs=0 },
	{ .regAddr=0x0205, .regData= 0x0c, .delayUs=0 },
	{ .regAddr=0x0202, .regData= 0x06, .delayUs=0 },
	{ .regAddr=0x0203, .regData= 0x27, .delayUs=0 },
	{ .regAddr=0x0614, .regData= 0x00, .delayUs=0 },
	{ .regAddr=0x0615, .regData= 0x00, .delayUs=0 },
	{ .regAddr=0x0181, .regData= 0x0c, .delayUs=0 },
	{ .regAddr=0x0182, .regData= 0x05, .delayUs=0 },
	{ .regAddr=0x0185, .regData= 0x01, .delayUs=0 },
	{ .regAddr=0x0180, .regData= 0x46, .delayUs=0 },
	{ .regAddr=0x0100, .regData= 0x08, .delayUs=0 },
	{ .regAddr=0x0106, .regData= 0x38, .delayUs=0 },
	{ .regAddr=0x010d, .regData= 0x80, .delayUs=0 },
	{ .regAddr=0x010e, .regData= 0x0c, .delayUs=0 },
	{ .regAddr=0x0113, .regData= 0x02, .delayUs=0 },
	{ .regAddr=0x0114, .regData= 0x01, .delayUs=0 },
	{ .regAddr=0x0115, .regData= 0x10, .delayUs=0 },
	//{ .regAddr=0x0100, .regData= 0x09, .delayUs=0 },

	{ .regAddr=0x0052, .regData= 0x02, .delayUs=0 },
	{ .regAddr=0x0076, .regData= 0x01, .delayUs=0 },
	{ .regAddr=0x021a, .regData= 0x10, .delayUs=0 },
	{ .regAddr=0x0434, .regData= 0x75, .delayUs=0 },
	{ .regAddr=0x0435, .regData= 0x75, .delayUs=0 },
	{ .regAddr=0x0436, .regData= 0x75, .delayUs=0 },
	{ .regAddr=0x0437, .regData= 0x75, .delayUs=0 },
	{ .regAddr=0x0430, .regData= 0x0a, .delayUs=0 },
	{ .regAddr=0x0431, .regData= 0x0a, .delayUs=0 },
	{ .regAddr=0x0432, .regData= 0x0a, .delayUs=0 },
	{ .regAddr=0x0433, .regData= 0x0a, .delayUs=0 },
	{ .regAddr=0x0458, .regData= 0x00, .delayUs=0 },
	{ .regAddr=0x0459, .regData= 0x00, .delayUs=0 },
	{ .regAddr=0x045a, .regData= 0x00, .delayUs=0 },
	{ .regAddr=0x045b, .regData= 0x00, .delayUs=0 },

	{ .regAddr=0x0a67, .regData= 0x80, .delayUs=0 },
	{ .regAddr=0x0a54, .regData= 0x0e, .delayUs=0 },
	{ .regAddr=0x0a65, .regData= 0x10, .delayUs=0 },
	{ .regAddr=0x0a98, .regData= 0x10, .delayUs=0 },
	{ .regAddr=0x05be, .regData= 0x00, .delayUs=0 },
	{ .regAddr=0x05a9, .regData= 0x01, .delayUs=0 },
	{ .regAddr=0x0029, .regData= 0x08, .delayUs=0 },
	{ .regAddr=0x002b, .regData= 0xa8, .delayUs=0 },
	{ .regAddr=0x0a83, .regData= 0xe0, .delayUs=0 },
	{ .regAddr=0x0a72, .regData= 0x02, .delayUs=0 },
	{ .regAddr=0x0a75, .regData= 0x41, .delayUs=0 },
	{ .regAddr=0x0a70, .regData= 0x03, .delayUs=0 },
#if defined(TARGET_CHIP_TX5239) && defined(TARGET_BOARD_PTZ)
  //Horizontal mirror and vertical FLIP 
 	{ .regAddr=0x022c, .regData= 0x03, .delayUs=0 },
#endif
	{ .regAddr=0x0a5a, .regData= 0x80, .delayUs=20000 },
	/*wait 20ms*/
	{ .regAddr=0x05be, .regData= 0x01, .delayUs=0 },
	{ .regAddr=0x0a70, .regData= 0x00, .delayUs=0 },
	{ .regAddr=0x0080, .regData= 0x02, .delayUs=0 },
	{ .regAddr=0x0a67, .regData= 0x00, .delayUs=0 },
};

static drv_sensor_reg_info_t  gc4023_streamOn_setting[] =
{
	{ .regAddr=0x0100, .regData= 0x09, .delayUs=0 },
};

static drv_sensor_reg_info_t  gc4023_streamOff_setting[] =
{
	{ .regAddr=0x0100, .regData= 0x00, .delayUs=50000 },
};

static drv_sensor_exp_param_t gc4023_30fps_exparams =
{
    .maxIntegrationTime = 1492,
    .minIntegrationTime = 8,
    .maxAnalogGain      = 65536,
    .minAnalogGain      = 1024,
    .lineTime           = 91022,
    .limitShortMid      = 0,
    .limitLong          = 0,
    .expLineMargin      = 1,
};
static drv_sensor_exp_param_t gc4023_10fps_exparams =
{
    .maxIntegrationTime = 1432,
    .minIntegrationTime = 8,
    .maxAnalogGain      = 33532,
    .minAnalogGain      = 1024,
    .lineTime           = 273066,
    .limitShortMid      = 0,
    .limitLong          = 0,
    .expLineMargin      = 1,
};
static drv_sensor_exp_param_t gc4023_15fps_exparams =
{
    .maxIntegrationTime = 2980,
    .minIntegrationTime = 0,
    .maxAnalogGain      = 65536,
    .minAnalogGain      = 1024,
    .lineTime           = 91022,
    .limitShortMid      = 0,
    .limitLong          = 0,
    .expLineMargin      = 1,
};
static drv_sensor_exp_param_t gc4023_1080p_15fps_exparams =
{
    .maxIntegrationTime = 1432,
    .minIntegrationTime = 8,
    .maxAnalogGain      = 33532,
    .minAnalogGain      = 1024,
    .lineTime           = 182044,
    .limitShortMid      = 0,
    .limitLong          = 0,
    .expLineMargin      = 1,
};

static drv_power_setting_t gc4023_power_up_setting[] =
{
    {.type = DRV_POWER_REG, .regAddr = 0x83c40004, .bitMask  = 0x04, .bitVal = 0x00, .delayUs = 10000},
    {.type = DRV_POWER_REG, .regAddr = 0x83c40004, .bitMask  = 0x04, .bitVal = 0x01, .delayUs = 10000},
    {.type = DRV_POWER_REG, .regAddr = 0x83c40004, .bitMask  = 0x01, .bitVal = 0x00, .delayUs = 10000},
    {.type = DRV_POWER_REG, .regAddr = 0x83c40004, .bitMask  = 0x01, .bitVal = 0x01, .delayUs = 10000},
};

static drv_power_setting_t gc4023_power_down_setting[] =
{
    {.type = DRV_POWER_REG, .regAddr = 0x83c40004, .bitMask  = 0x01, .bitVal = 0x00, .delayUs = 0},
};

static sensor_drv_params_t gc4023_aux1_sensor_params =
{
    .sensorId     = 0x23,
    .sensorIdAddr = 0x3f1,
    .name         = "gc4023",
    .slaveInfo =
    {
        .hwType      = DRV_HW_I2C,
        .slaveAddr   = (0x52 >> 1),
        .channel     = 1,
        .regDataType = DRV_ADDR_TYPE_BYTE,
        .regAddrType = DRV_DATA_TYPE_WORD,
    },
    .powerUpSettings  =
    {
        .powerSetting = gc4023_power_up_setting,
        .size = sizeof(gc4023_power_up_setting) / sizeof(gc4023_power_up_setting[0]),
    },
    .powerDownSettings =
    {
        .powerSetting  = gc4023_power_down_setting,
        .size = sizeof(gc4023_power_down_setting) / sizeof(gc4023_power_down_setting[0]),
    },
    .activesize =
    {
        .width  = 2560,
        .height = 1440
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
		.frameLength = 0xBA8,
		.expLineMargin = 1,
		.dim =
		{
			.width  = 2560,
			.height = 1440,
		},
		.hdrMode   = SENSOR_NORMAL_MODE,
		.regConfig =
		{
			.regDataType = DRV_DATA_TYPE_BYTE,
			.regAddrType = DRV_ADDR_TYPE_WORD,
			.direction   = DRV_DIRECTION_WRITE,
			.settings    = gc4023_4mp_setting,
			.settingSize = sizeof(gc4023_4mp_setting) / sizeof (gc4023_4mp_setting[0]),
		},
		.expParams   = &gc4023_15fps_exparams,

		.intf = {
			.ln_cfg = {
				.dat_num = 2,
			},
			.link_freq = 704.0f,
		},
	},
	.modeSettings[1] =
	{
		#if (FRAME_RATE == 15)
		.maxFps = 30,
		.minFps = 15,
		.frameLength = 0x5dc,
		#elif (FRAME_RATE == 20)
		.maxFps = 20,
		.minFps = 20,
		.frameLength = 0x8ca,
		#endif
		.expLineMargin = 1,
		.dim =
		{
			.width  = 1920,
			.height = 1080,
		},
		.hdrMode   = SENSOR_NORMAL_MODE,
		.regConfig =
		{
			.regDataType = DRV_DATA_TYPE_BYTE,
			.regAddrType = DRV_ADDR_TYPE_WORD,
			.direction   = DRV_DIRECTION_WRITE,
			.settings    = gc4023_1080p_setting,
			.settingSize = sizeof(gc4023_1080p_setting) / sizeof (gc4023_1080p_setting[0]),
		},
		.expParams   = &gc4023_30fps_exparams,

		.intf = {
			.ln_cfg = {
				.dat_num = 2,
			},
			.link_freq = 704.0f,
		},
	},
	.modeSettings[2] =
	{
		.maxFps = 10,
		.minFps = 5,
		.frameLength = 0x5dc,
		.expLineMargin = 1,
		.dim =
		{
			.width  = 2560,
			.height = 1440,
		},
		.hdrMode   = SENSOR_NORMAL_MODE,
		.regConfig =
		{
			.regDataType = DRV_DATA_TYPE_BYTE,
			.regAddrType = DRV_ADDR_TYPE_WORD,
			.direction   = DRV_DIRECTION_WRITE,
			.settings    = gc4023_4mp_10fps_setting,
			.settingSize = sizeof(gc4023_4mp_10fps_setting) / sizeof (gc4023_4mp_10fps_setting[0]),
		},
		.expParams   = &gc4023_10fps_exparams,

		.intf = {
			.ln_cfg = {
				.dat_num = 2,
			},
			.link_freq = 704.0f,
		},
	},
	.modeSettings[3] =
	{
		#if (FRAME_RATE == 15)
		.maxFps = 10,
		.minFps = 5,
		.frameLength = 0x5dc,
		#elif (FRAME_RATE == 20)
		.maxFps = 20,
		.minFps = 20,
		.frameLength = 0x8ca,
		#endif
		.expLineMargin = 1,
		.dim =
		{
			.width  = 1920,
			.height = 1080,
		},
		.hdrMode   = SENSOR_NORMAL_MODE,
		.regConfig =
		{
			.regDataType = DRV_DATA_TYPE_BYTE,
			.regAddrType = DRV_ADDR_TYPE_WORD,
			.direction   = DRV_DIRECTION_WRITE,
			.settings    = gc4023_1080p_15fps_setting,
			.settingSize = sizeof(gc4023_1080p_15fps_setting) / sizeof (gc4023_1080p_15fps_setting[0]),
		},
		.expParams   = &gc4023_1080p_15fps_exparams,

		.intf = {
			.ln_cfg = {
				.dat_num = 2,
			},
			.link_freq = 352.0f,
		},
	},

    .streamOnSetting =
    {
        .regDataType = DRV_DATA_TYPE_BYTE,
        .regAddrType = DRV_ADDR_TYPE_WORD,
        .direction   = DRV_DIRECTION_WRITE,
        .settings    = gc4023_streamOn_setting,
        .settingSize = sizeof(gc4023_streamOn_setting) / sizeof (gc4023_streamOn_setting[0]),
    },

    .streamOffSetting =
    {
        .regDataType = DRV_DATA_TYPE_BYTE,
        .regAddrType = DRV_ADDR_TYPE_WORD,
        .direction   = DRV_DIRECTION_WRITE,
        .settings    = gc4023_streamOff_setting,
        .settingSize = sizeof(gc4023_streamOff_setting) / sizeof (gc4023_streamOff_setting[0]),
    },

    .fill_exposure_reg = GC4023_Fill_Exposure_Reg,
};


sensor_drv_params_t* load_gc4023_sensor_driver(void);

#endif

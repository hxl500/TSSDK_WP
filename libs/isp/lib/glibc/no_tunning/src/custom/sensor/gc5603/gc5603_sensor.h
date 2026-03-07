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

 #ifndef __GC5603_SENSOR_H__
 #define __GC5603_SENSOR_H__
 
 #include "sensor_common.h"
 #include "log_utils.h"
 
 #if defined(TARGET_CHIP_TX5215) && defined(TARGET_BOARD_ANSJERC182)
 #define SET_FPS        (20)  // FPS : 25、20、15
 #define SENSOR_FPS_CFG 1
 #elif defined(TARGET_CHIP_TX5215) && defined(TARGET_BOARD_ANSJERC225)
 #define SET_FPS        (20)  // FPS : 25、20、15
 #define SENSOR_FPS_CFG 1
 #else
 #define SET_FPS        (14)  // FPS : 20、15、14、12.5
 #define SENSOR_FPS_CFG 2
 #endif
 
 #if SET_FPS == 25
	 #define FRAME_LENTH             (0x6df)
	 #define MAX_INTEGRATION_TIME    (1759)
	 #define LINE_TIME               (93569)
 #elif SET_FPS == 20
	 #define FRAME_LENTH             (0x898)
	 #define MAX_INTEGRATION_TIME    (2200)
	 #define LINE_TIME               (93090)
 #elif SET_FPS == 15
	 #define FRAME_LENTH             (0x0dac)
	 #define MAX_INTEGRATION_TIME    (3500)
	 #define LINE_TIME               (78019)
 #elif SET_FPS == 14
	 #define FRAME_LENTH             (0x0ea6)
	 #define MAX_INTEGRATION_TIME    (3750)
	 #define LINE_TIME               (72817)
 #elif SET_FPS == 125
	 #define FRAME_LENTH             (0x1068)
	 #define MAX_INTEGRATION_TIME    (4200)
	 #define LINE_TIME               (65015)
 #endif
 
 
 isp_ret_t GC5603_Fill_Exposure_Reg1(sensor_apply_exp_param_t * pExpParam, drv_regs_array_info_t * pRegInfo);
 isp_ret_t GC5603_Fill_Exposure_Reg2(sensor_apply_exp_param_t * pExpParam, drv_regs_array_info_t * pRegInfo);
 //2688*1512 BGGR GC5603 All-pixel scan CSI-2_2lane 24Mhz AD:10bit Output:10bit 508Mbps Master Mode 20.00fps Integration Time: Xx.xxms Gain:XdB vts = 2200,row_time=22.727us
 static drv_sensor_reg_info_t  gc5603_5mp_setting[] =
 {
	 { .regAddr=0x03fe, .regData=0xf0, .delayUs = 0 },
	 { .regAddr=0x03fe, .regData=0x00, .delayUs = 0 },
	 { .regAddr=0x03fe, .regData=0x10, .delayUs = 0 },
	 { .regAddr=0x03fe, .regData=0x00, .delayUs = 0 },
	 { .regAddr=0x0a38, .regData=0x02, .delayUs = 0 },
	 { .regAddr=0x0a38, .regData=0x03, .delayUs = 0 },
	 { .regAddr=0x0a20, .regData=0x07, .delayUs = 0 },
	 { .regAddr=0x061b, .regData=0x03, .delayUs = 0 },
	 { .regAddr=0x061c, .regData=0x58, .delayUs = 0 },
	 { .regAddr=0x061d, .regData=0x05, .delayUs = 0 },
	 { .regAddr=0x061e, .regData=0x84, .delayUs = 0 },
	 { .regAddr=0x061f, .regData=0x03, .delayUs = 0 },
	 { .regAddr=0x0a21, .regData=0x08, .delayUs = 0 },
	 { .regAddr=0x0a34, .regData=0x40, .delayUs = 0 },
	 { .regAddr=0x0a35, .regData=0x50, .delayUs = 0 },
	 { .regAddr=0x0a36, .regData=0x7f, .delayUs = 0 },
	 { .regAddr=0x0a37, .regData=0x03, .delayUs = 0 },
	 { .regAddr=0x0314, .regData=0x50, .delayUs = 0 },
	 { .regAddr=0x0315, .regData=0x32, .delayUs = 0 },
	 { .regAddr=0x031c, .regData=0xce, .delayUs = 0 },
	 { .regAddr=0x0219, .regData=0x47, .delayUs = 0 },
	 { .regAddr=0x0342, .regData=0x05, .delayUs = 0 },//
	 { .regAddr=0x0343, .regData=0xdc, .delayUs = 0 },//
	 #if SET_FPS == 15
	 { .regAddr=0x0340, .regData=0x0b, .delayUs = 0 },//15fps
	 { .regAddr=0x0341, .regData=0x75, .delayUs = 0 },//15fps
	 #elif SET_FPS == 20
	 { .regAddr=0x0340, .regData=0x08, .delayUs = 0 },//20fps
	 { .regAddr=0x0341, .regData=0x98, .delayUs = 0 },//20fps
	 #elif SET_FPS == 25
	 { .regAddr=0x0340, .regData=0x06, .delayUs = 0 },//25fps
	 { .regAddr=0x0341, .regData=0xdf, .delayUs = 0 },//25fps
	 #endif
	 { .regAddr=0x029d, .regData=0x08, .delayUs = 0 },
	 { .regAddr=0x0345, .regData=0x02, .delayUs = 0 },
	 { .regAddr=0x0347, .regData=0x02, .delayUs = 0 },
	 { .regAddr=0x0348, .regData=0x0b, .delayUs = 0 },
	 { .regAddr=0x0349, .regData=0x98, .delayUs = 0 },//2968
	 { .regAddr=0x034a, .regData=0x06, .delayUs = 0 },
	 { .regAddr=0x034b, .regData=0x8a, .delayUs = 0 },//1674
	 { .regAddr=0x0094, .regData=0x0a, .delayUs = 0 },
	 { .regAddr=0x0095, .regData=0x80, .delayUs = 0 }, //2688
	 { .regAddr=0x0096, .regData=0x05, .delayUs = 0 },
	 { .regAddr=0x0097, .regData=0xea, .delayUs = 0 }, //1512
 
	 { .regAddr=0x0099, .regData=0x88, .delayUs = 0 },
	 { .regAddr=0x009b, .regData=0x4d, .delayUs = 0 },
	 { .regAddr=0x060c, .regData=0x01, .delayUs = 0 },
	 { .regAddr=0x060e, .regData=0xd2, .delayUs = 0 },
	 { .regAddr=0x060f, .regData=0x05, .delayUs = 0 },
	 { .regAddr=0x070c, .regData=0x01, .delayUs = 0 },
	 { .regAddr=0x070e, .regData=0xd2, .delayUs = 0 },
	 { .regAddr=0x070f, .regData=0x05, .delayUs = 0 },
	 { .regAddr=0x0709, .regData=0x40, .delayUs = 0 },
	 { .regAddr=0x0719, .regData=0x40, .delayUs = 0 },
	 { .regAddr=0x0909, .regData=0x07, .delayUs = 0 },
	 { .regAddr=0x0902, .regData=0x04, .delayUs = 0 },
	 { .regAddr=0x0904, .regData=0x0b, .delayUs = 0 },
	 { .regAddr=0x0907, .regData=0x54, .delayUs = 0 },
	 { .regAddr=0x0908, .regData=0x06, .delayUs = 0 },
	 { .regAddr=0x0903, .regData=0x9d, .delayUs = 0 },
	 { .regAddr=0x072a, .regData=0x1c, .delayUs = 0 },
	 { .regAddr=0x072b, .regData=0x1c, .delayUs = 0 },
	 { .regAddr=0x0724, .regData=0x2b, .delayUs = 0 },
	 { .regAddr=0x0727, .regData=0x2b, .delayUs = 0 },
	 { .regAddr=0x1466, .regData=0x18, .delayUs = 0 },
	 { .regAddr=0x1467, .regData=0x15, .delayUs = 0 },
	 { .regAddr=0x1468, .regData=0x15, .delayUs = 0 },
	 { .regAddr=0x1469, .regData=0x70, .delayUs = 0 },
	 { .regAddr=0x146a, .regData=0xe8, .delayUs = 0 },
	 { .regAddr=0x0707, .regData=0x07, .delayUs = 0 },
	 { .regAddr=0x0737, .regData=0x0f, .delayUs = 0 },
	 { .regAddr=0x0704, .regData=0x01, .delayUs = 0 },
	 { .regAddr=0x0706, .regData=0x02, .delayUs = 0 },
	 { .regAddr=0x0716, .regData=0x02, .delayUs = 0 },
	 { .regAddr=0x0708, .regData=0xc8, .delayUs = 0 },
	 { .regAddr=0x0718, .regData=0xc8, .delayUs = 0 },
	 { .regAddr=0x061a, .regData=0x02, .delayUs = 0 },
	 { .regAddr=0x1430, .regData=0x80, .delayUs = 0 },
	 { .regAddr=0x1407, .regData=0x10, .delayUs = 0 },
	 { .regAddr=0x1408, .regData=0x16, .delayUs = 0 },
	 { .regAddr=0x1409, .regData=0x03, .delayUs = 0 },
	 { .regAddr=0x1438, .regData=0x01, .delayUs = 0 },
	 { .regAddr=0x02ce, .regData=0x03, .delayUs = 0 },
	 { .regAddr=0x0245, .regData=0xc9, .delayUs = 0 },
	 { .regAddr=0x023a, .regData=0x08, .delayUs = 0 },
	 { .regAddr=0x02cd, .regData=0x88, .delayUs = 0 },
	 { .regAddr=0x0612, .regData=0x02, .delayUs = 0 },
	 { .regAddr=0x0613, .regData=0xc7, .delayUs = 0 },
	 { .regAddr=0x0243, .regData=0x03, .delayUs = 0 },
	 { .regAddr=0x0089, .regData=0x03, .delayUs = 0 },
	 { .regAddr=0x0002, .regData=0xab, .delayUs = 0 },
	 { .regAddr=0x0040, .regData=0xa3, .delayUs = 0 },
	 { .regAddr=0x0075, .regData=0x64, .delayUs = 0 },
	 { .regAddr=0x0004, .regData=0x0f, .delayUs = 0 },
	 { .regAddr=0x0053, .regData=0x0a, .delayUs = 0 },
	 { .regAddr=0x0205, .regData=0x0c, .delayUs = 0 },
	 { .regAddr=0x0a67, .regData=0x80, .delayUs = 0 },
	 { .regAddr=0x0a54, .regData=0x0e, .delayUs = 0 },
	 { .regAddr=0x0a65, .regData=0x10, .delayUs = 0 },
	 { .regAddr=0x0a98, .regData=0x04, .delayUs = 0 },
	 { .regAddr=0x05be, .regData=0x00, .delayUs = 0 },
	 { .regAddr=0x05a9, .regData=0x01, .delayUs = 0 },
	 { .regAddr=0x0023, .regData=0x00, .delayUs = 0 },
	 { .regAddr=0x0022, .regData=0x00, .delayUs = 0 },
	 { .regAddr=0x0025, .regData=0x00, .delayUs = 0 },
	 { .regAddr=0x0024, .regData=0x00, .delayUs = 0 },
	 { .regAddr=0x0028, .regData=0x0b, .delayUs = 0 },
	 { .regAddr=0x0029, .regData=0x98, .delayUs = 0 },
	 { .regAddr=0x002a, .regData=0x06, .delayUs = 0 },
	 { .regAddr=0x002b, .regData=0x86, .delayUs = 0 },
	 { .regAddr=0x0a83, .regData=0xe0, .delayUs = 0 },
	 { .regAddr=0x0a72, .regData=0x02, .delayUs = 0 },
	 { .regAddr=0x0a73, .regData=0x60, .delayUs = 0 },
	 { .regAddr=0x0a75, .regData=0x41, .delayUs = 0 },
	 { .regAddr=0x0a70, .regData=0x03, .delayUs = 0 },
	 { .regAddr=0x0a5a, .regData=0x80, .delayUs = 0 },
	 { .regAddr=0x0181, .regData=0x30, .delayUs = 0 },
	 { .regAddr=0x0182, .regData=0x05, .delayUs = 0 },
	 { .regAddr=0x0185, .regData=0x01, .delayUs = 0 },
	 { .regAddr=0x0180, .regData=0x46, .delayUs = 0 },
	 { .regAddr=0x0100, .regData=0x08, .delayUs = 0 },
	 { .regAddr=0x010d, .regData=0x20, .delayUs = 0 },
	 { .regAddr=0x010e, .regData=0x0d, .delayUs = 0 },
	 { .regAddr=0x0113, .regData=0x02, .delayUs = 0 },
	 { .regAddr=0x0114, .regData=0x01, .delayUs = 0 },
	 { .regAddr=0x0115, .regData=0x10, .delayUs = 0 },
	 { .regAddr=0x0a70, .regData=0x00, .delayUs = 0 },
	 { .regAddr=0x0080, .regData=0x02, .delayUs = 0 },
	 { .regAddr=0x0a67, .regData=0x00, .delayUs = 0 },
	 { .regAddr=0x0052, .regData=0x02, .delayUs = 0 },
	 { .regAddr=0x0076, .regData=0x01, .delayUs = 0 },
	 { .regAddr=0x021a, .regData=0x10, .delayUs = 0 },
	 { .regAddr=0x0049, .regData=0x0f, .delayUs = 0 }, //darkrow select
	 { .regAddr=0x004a, .regData=0x3c, .delayUs = 0 },
	 { .regAddr=0x004b, .regData=0x00, .delayUs = 0 },
	 { .regAddr=0x0430, .regData=0x25, .delayUs = 0 },
	 { .regAddr=0x0431, .regData=0x25, .delayUs = 0 },
	 { .regAddr=0x0432, .regData=0x25, .delayUs = 0 },
	 { .regAddr=0x0433, .regData=0x25, .delayUs = 0 },
	 { .regAddr=0x0434, .regData=0x59, .delayUs = 0 },
	 { .regAddr=0x0435, .regData=0x59, .delayUs = 0 },
	 { .regAddr=0x0436, .regData=0x59, .delayUs = 0 },
	 { .regAddr=0x0437, .regData=0x59, .delayUs = 0 },
	 { .regAddr=0x0060, .regData=0x30, .delayUs = 0 },
	 { .regAddr=0x0063, .regData=0x00, .delayUs = 0 },
	 { .regAddr=0x022c, .regData=0x00, .delayUs = 0 },
 
	 { .regAddr=0x0a67, .regData=0x80, .delayUs = 0 },
	 { .regAddr=0x0a54, .regData=0x0e, .delayUs = 0 },
	 { .regAddr=0x0a65, .regData=0x10, .delayUs = 0 },
	 { .regAddr=0x0a98, .regData=0x04, .delayUs = 0 },
	 { .regAddr=0x05be, .regData=0x00, .delayUs = 0 },
	 { .regAddr=0x05a9, .regData=0x01, .delayUs = 0 },
	 { .regAddr=0x0023, .regData=0x00, .delayUs = 0 },
	 { .regAddr=0x0022, .regData=0x00, .delayUs = 0 },
	 { .regAddr=0x0025, .regData=0x00, .delayUs = 0 },
	 { .regAddr=0x0024, .regData=0x00, .delayUs = 0 },
	 { .regAddr=0x0028, .regData=0x0b, .delayUs = 0 },
	 { .regAddr=0x0029, .regData=0x98, .delayUs = 0 },
	 { .regAddr=0x002a, .regData=0x06, .delayUs = 0 },
	 { .regAddr=0x002b, .regData=0x86, .delayUs = 0 },
	 { .regAddr=0x0a83, .regData=0xe0, .delayUs = 0 },
	 { .regAddr=0x0a72, .regData=0x02, .delayUs = 0 },
	 { .regAddr=0x0a73, .regData=0x60, .delayUs = 0 },
	 { .regAddr=0x0a75, .regData=0x41, .delayUs = 0 },
	 { .regAddr=0x0a70, .regData=0x03, .delayUs = 0 },
	 { .regAddr=0x0a5a, .regData=0x80, .delayUs = 0 },
	 { .regAddr=0x0a70, .regData=0x00, .delayUs = 0 },
	 { .regAddr=0x0080, .regData=0x02, .delayUs = 0 },
	 { .regAddr=0x0a67, .regData=0x00, .delayUs = 0 },
	 //{ .regAddr=0x0100, .regData=0x09, .delayUs = 0 },
 };
 
 static drv_sensor_reg_info_t  gc5603_5mp_30fps_setting[] =
 {
	 { .regAddr=0x03fe, .regData= 0xf0, .delayUs=0 },
	 { .regAddr=0x03fe, .regData= 0x00, .delayUs=0 },
	 { .regAddr=0x03fe, .regData= 0x10, .delayUs=0 },
	 { .regAddr=0x03fe, .regData= 0x00, .delayUs=0 },
	 { .regAddr=0x0a38, .regData= 0x02, .delayUs=0 },
	 { .regAddr=0x0a38, .regData= 0x03, .delayUs=0 },
	 { .regAddr=0x0a20, .regData= 0x07, .delayUs=0 },
	 { .regAddr=0x061b, .regData= 0x03, .delayUs=0 },
	 { .regAddr=0x061c, .regData= 0x50, .delayUs=0 },
	 { .regAddr=0x061d, .regData= 0x05, .delayUs=0 },
	 { .regAddr=0x061e, .regData= 0x7e, .delayUs=0 },
	 { .regAddr=0x061f, .regData= 0x03, .delayUs=0 },
	 { .regAddr=0x0a21, .regData= 0x08, .delayUs=0 },
	 { .regAddr=0x0a34, .regData= 0x40, .delayUs=0 },
	 { .regAddr=0x0a35, .regData= 0x11, .delayUs=0 },
	 { .regAddr=0x0a36, .regData= 0x6a, .delayUs=0 },
	 { .regAddr=0x0a37, .regData= 0x03, .delayUs=0 },
	 { .regAddr=0x0314, .regData= 0x50, .delayUs=0 },
	 { .regAddr=0x0315, .regData= 0x32, .delayUs=0 },
	 { .regAddr=0x031c, .regData= 0xce, .delayUs=0 },
	 { .regAddr=0x0219, .regData= 0x47, .delayUs=0 },
	 { .regAddr=0x0342, .regData= 0x04, .delayUs=0 },
	 { .regAddr=0x0343, .regData= 0xb0, .delayUs=0 },
 #if SET_FPS == 20
	 { .regAddr=0x0340, .regData= 0x08, .delayUs=0 },
	 { .regAddr=0x0341, .regData= 0x98, .delayUs=0 },
 #elif SET_FPS == 15
	 { .regAddr=0x0340, .regData= 0x0d, .delayUs=0 },
	 { .regAddr=0x0341, .regData= 0xac, .delayUs=0 },
 #elif SET_FPS == 14
	 { .regAddr=0x0340, .regData= 0x0e, .delayUs=0 },
	 { .regAddr=0x0341, .regData= 0xa6, .delayUs=0 },
 #elif SET_FPS == 125
	 { .regAddr=0x0340, .regData= 0x10, .delayUs=0 },
	 { .regAddr=0x0341, .regData= 0x68, .delayUs=0 },
 #endif
	 { .regAddr=0x0345, .regData= 0x02, .delayUs=0 },
	 { .regAddr=0x0347, .regData= 0x02, .delayUs=0 },
	 { .regAddr=0x0348, .regData= 0x0b, .delayUs=0 },
	 { .regAddr=0x0349, .regData= 0x98, .delayUs=0 },
	 { .regAddr=0x034a, .regData= 0x06, .delayUs=0 },
	 { .regAddr=0x034b, .regData= 0x8a, .delayUs=0 },
	 { .regAddr=0x0094, .regData= 0x0a, .delayUs=0 },
	 { .regAddr=0x0095, .regData= 0x80, .delayUs=0 },
	 { .regAddr=0x0096, .regData= 0x05, .delayUs=0 },
	 { .regAddr=0x0097, .regData= 0xf0, .delayUs=0 },
	 { .regAddr=0x0099, .regData= 0x88, .delayUs=0 },
	 { .regAddr=0x009b, .regData= 0x4d, .delayUs=0 },
	 { .regAddr=0x060c, .regData= 0x01, .delayUs=0 },
	 { .regAddr=0x060e, .regData= 0xd2, .delayUs=0 },
	 { .regAddr=0x060f, .regData= 0x05, .delayUs=0 },
	 { .regAddr=0x070c, .regData= 0x01, .delayUs=0 },
	 { .regAddr=0x070e, .regData= 0xd2, .delayUs=0 },
	 { .regAddr=0x070f, .regData= 0x05, .delayUs=0 },
	 { .regAddr=0x0709, .regData= 0x40, .delayUs=0 },
	 { .regAddr=0x0719, .regData= 0x40, .delayUs=0 },
	 { .regAddr=0x0909, .regData= 0x07, .delayUs=0 },
	 { .regAddr=0x0902, .regData= 0x04, .delayUs=0 },
	 { .regAddr=0x0904, .regData= 0x0b, .delayUs=0 },
	 { .regAddr=0x0907, .regData= 0x54, .delayUs=0 },
	 { .regAddr=0x0908, .regData= 0x06, .delayUs=0 },
	 { .regAddr=0x0903, .regData= 0x9d, .delayUs=0 },
	 { .regAddr=0x072a, .regData= 0x1c, .delayUs=0 },
	 { .regAddr=0x072b, .regData= 0x1c, .delayUs=0 },
	 { .regAddr=0x0724, .regData= 0x2b, .delayUs=0 },
	 { .regAddr=0x0727, .regData= 0x2b, .delayUs=0 },
	 { .regAddr=0x1466, .regData= 0x18, .delayUs=0 },
	 { .regAddr=0x1467, .regData= 0x15, .delayUs=0 },
	 { .regAddr=0x1468, .regData= 0x15, .delayUs=0 },
	 { .regAddr=0x1469, .regData= 0x70, .delayUs=0 },
	 { .regAddr=0x146a, .regData= 0xe8, .delayUs=0 },
	 { .regAddr=0x0707, .regData= 0x07, .delayUs=0 },
	 { .regAddr=0x0737, .regData= 0x0f, .delayUs=0 },
	 { .regAddr=0x0704, .regData= 0x01, .delayUs=0 },
	 { .regAddr=0x0706, .regData= 0x02, .delayUs=0 },
	 { .regAddr=0x0716, .regData= 0x02, .delayUs=0 },
	 { .regAddr=0x0708, .regData= 0xc8, .delayUs=0 },
	 { .regAddr=0x0718, .regData= 0xc8, .delayUs=0 },
	 { .regAddr=0x061a, .regData= 0x02, .delayUs=0 },
	 { .regAddr=0x1430, .regData= 0x80, .delayUs=0 },
	 { .regAddr=0x1407, .regData= 0x10, .delayUs=0 },
	 { .regAddr=0x1408, .regData= 0x16, .delayUs=0 },
	 { .regAddr=0x1409, .regData= 0x03, .delayUs=0 },
	 { .regAddr=0x1438, .regData= 0x01, .delayUs=0 },
	 { .regAddr=0x02ce, .regData= 0x03, .delayUs=0 },
	 { .regAddr=0x0245, .regData= 0xc9, .delayUs=0 },
	 { .regAddr=0x023a, .regData= 0x08, .delayUs=0 },
	 { .regAddr=0x02cd, .regData= 0x88, .delayUs=0 },
	 { .regAddr=0x0612, .regData= 0x02, .delayUs=0 },
	 { .regAddr=0x0613, .regData= 0xc7, .delayUs=0 },
	 { .regAddr=0x0243, .regData= 0x03, .delayUs=0 },
	 { .regAddr=0x0089, .regData= 0x03, .delayUs=0 },
	 { .regAddr=0x0002, .regData= 0xab, .delayUs=0 },
	 { .regAddr=0x0040, .regData= 0xa3, .delayUs=0 },
	 { .regAddr=0x0075, .regData= 0x64, .delayUs=0 },
	 { .regAddr=0x0004, .regData= 0x0f, .delayUs=0 },
	 { .regAddr=0x0053, .regData= 0x0a, .delayUs=0 },
	 { .regAddr=0x0205, .regData= 0x0c, .delayUs=0 },
	 { .regAddr=0x0a67, .regData= 0x80, .delayUs=0 },
	 { .regAddr=0x0a54, .regData= 0x0e, .delayUs=0 },
	 { .regAddr=0x0a65, .regData= 0x10, .delayUs=0 },
	 { .regAddr=0x0a98, .regData= 0x04, .delayUs=0 },
	 { .regAddr=0x05be, .regData= 0x00, .delayUs=0 },
	 { .regAddr=0x05a9, .regData= 0x01, .delayUs=0 },
	 { .regAddr=0x0023, .regData= 0x00, .delayUs=0 },
	 { .regAddr=0x0022, .regData= 0x00, .delayUs=0 },
	 { .regAddr=0x0025, .regData= 0x00, .delayUs=0 },
	 { .regAddr=0x0024, .regData= 0x00, .delayUs=0 },
	 { .regAddr=0x0028, .regData= 0x0b, .delayUs=0 },
	 { .regAddr=0x0029, .regData= 0x98, .delayUs=0 },
	 { .regAddr=0x002a, .regData= 0x06, .delayUs=0 },
	 { .regAddr=0x002b, .regData= 0x86, .delayUs=0 },
	 { .regAddr=0x0a83, .regData= 0xe0, .delayUs=0 },
	 { .regAddr=0x0a72, .regData= 0x02, .delayUs=0 },
	 { .regAddr=0x0a73, .regData= 0x60, .delayUs=0 },
	 { .regAddr=0x0a75, .regData= 0x41, .delayUs=0 },
	 { .regAddr=0x0a70, .regData= 0x03, .delayUs=0 },
	 { .regAddr=0x0a5a, .regData= 0x80, .delayUs=0 },
	 { .regAddr=0x0123, .regData= 0x30, .delayUs=0 },
	 { .regAddr=0x0124, .regData= 0x04, .delayUs=0 },
	 { .regAddr=0x0125, .regData= 0x30, .delayUs=0 },
	 { .regAddr=0x0129, .regData= 0x0c, .delayUs=0 },
	 { .regAddr=0x012a, .regData= 0x18, .delayUs=0 },
	 { .regAddr=0x012b, .regData= 0x18, .delayUs=0 },
	 { .regAddr=0x0181, .regData= 0x30, .delayUs=0 },
	 { .regAddr=0x0182, .regData= 0x05, .delayUs=0 },
	 { .regAddr=0x0185, .regData= 0x01, .delayUs=0 },
	 { .regAddr=0x0180, .regData= 0x46, .delayUs=0 },
	 { .regAddr=0x0100, .regData= 0x08, .delayUs=0 },
	 { .regAddr=0x010d, .regData= 0x20, .delayUs=0 },
	 { .regAddr=0x010e, .regData= 0x0d, .delayUs=0 },
	 { .regAddr=0x0113, .regData= 0x02, .delayUs=0 },
	 { .regAddr=0x0114, .regData= 0x01, .delayUs=0 },
	 { .regAddr=0x0115, .regData= 0x10, .delayUs=0 },
	 { .regAddr=0x05be, .regData= 0x01, .delayUs=0 },
	 { .regAddr=0x0a70, .regData= 0x00, .delayUs=0 },
	 { .regAddr=0x0080, .regData= 0x02, .delayUs=0 },
	 { .regAddr=0x0a67, .regData= 0x00, .delayUs=0 },
	 { .regAddr=0x0052, .regData= 0x02, .delayUs=0 },
	 { .regAddr=0x0076, .regData= 0x01, .delayUs=0 },
	 { .regAddr=0x021a, .regData= 0x10, .delayUs=0 },
	 { .regAddr=0x0049, .regData= 0x0f, .delayUs=0 },
	 { .regAddr=0x004a, .regData= 0x3c, .delayUs=0 },
	 { .regAddr=0x004b, .regData= 0x00, .delayUs=0 },
	 { .regAddr=0x0430, .regData= 0x25, .delayUs=0 },
	 { .regAddr=0x0431, .regData= 0x25, .delayUs=0 },
	 { .regAddr=0x0432, .regData= 0x25, .delayUs=0 },
	 { .regAddr=0x0433, .regData= 0x25, .delayUs=0 },
	 { .regAddr=0x0434, .regData= 0x59, .delayUs=0 },
	 { .regAddr=0x0435, .regData= 0x59, .delayUs=0 },
	 { .regAddr=0x0436, .regData= 0x59, .delayUs=0 },
	 { .regAddr=0x0437, .regData= 0x59, .delayUs=0 },
 
	 { .regAddr=0x0060, .regData=0x30, .delayUs = 0 },
	 { .regAddr=0x0063, .regData=0x00, .delayUs = 0 },
	 { .regAddr=0x022c, .regData=0x00, .delayUs = 0 },
 
	 { .regAddr=0x0a67, .regData=0x80, .delayUs = 0 },
	 { .regAddr=0x0a54, .regData=0x0e, .delayUs = 0 },
	 { .regAddr=0x0a65, .regData=0x10, .delayUs = 0 },
	 { .regAddr=0x0a98, .regData=0x04, .delayUs = 0 },
	 { .regAddr=0x05be, .regData=0x00, .delayUs = 0 },
	 { .regAddr=0x05a9, .regData=0x01, .delayUs = 0 },
	 { .regAddr=0x0023, .regData=0x00, .delayUs = 0 },
	 { .regAddr=0x0022, .regData=0x00, .delayUs = 0 },
	 { .regAddr=0x0025, .regData=0x00, .delayUs = 0 },
	 { .regAddr=0x0024, .regData=0x00, .delayUs = 0 },
	 { .regAddr=0x0028, .regData=0x0b, .delayUs = 0 },
	 { .regAddr=0x0029, .regData=0x98, .delayUs = 0 },
	 { .regAddr=0x002a, .regData=0x06, .delayUs = 0 },
	 { .regAddr=0x002b, .regData=0x86, .delayUs = 0 },
	 { .regAddr=0x0a83, .regData=0xe0, .delayUs = 0 },
	 { .regAddr=0x0a72, .regData=0x02, .delayUs = 0 },
	 { .regAddr=0x0a73, .regData=0x60, .delayUs = 0 },
	 { .regAddr=0x0a75, .regData=0x41, .delayUs = 0 },
	 { .regAddr=0x0a70, .regData=0x03, .delayUs = 0 },
	 { .regAddr=0x0a5a, .regData=0x80, .delayUs = 0 },
	 { .regAddr=0x0a70, .regData=0x00, .delayUs = 0 },
	 { .regAddr=0x0080, .regData=0x02, .delayUs = 0 },
	 { .regAddr=0x0a67, .regData=0x00, .delayUs = 0 },
	 //{ .regAddr=0x0100, .regData=0x09, .delayUs = 0 },
 };
 
 static drv_sensor_reg_info_t  gc5603_full5mp_setting[] =
 {
	 { .regAddr=0x03fe, .regData=0xf0, .delayUs = 0 },
	 { .regAddr=0x03fe, .regData=0x00, .delayUs = 0 },
	 { .regAddr=0x03fe, .regData=0x10, .delayUs = 0 },
	 { .regAddr=0x03fe, .regData=0x00, .delayUs = 0 },
	 { .regAddr=0x0a38, .regData=0x02, .delayUs = 0 },
	 { .regAddr=0x0a38, .regData=0x03, .delayUs = 0 },
	 { .regAddr=0x0a20, .regData=0x07, .delayUs = 0 },
	 { .regAddr=0x061b, .regData=0x03, .delayUs = 0 },
	 { .regAddr=0x061c, .regData=0x58, .delayUs = 0 },
	 { .regAddr=0x061d, .regData=0x05, .delayUs = 0 },
	 { .regAddr=0x061e, .regData=0x84, .delayUs = 0 },
	 { .regAddr=0x061f, .regData=0x03, .delayUs = 0 },
	 { .regAddr=0x0a21, .regData=0x08, .delayUs = 0 },
	 { .regAddr=0x0a34, .regData=0x40, .delayUs = 0 },
	 { .regAddr=0x0a35, .regData=0x50, .delayUs = 0 },
	 { .regAddr=0x0a36, .regData=0x7f, .delayUs = 0 },
	 { .regAddr=0x0a37, .regData=0x03, .delayUs = 0 },
	 { .regAddr=0x0314, .regData=0x50, .delayUs = 0 },
	 { .regAddr=0x0315, .regData=0x32, .delayUs = 0 },
	 { .regAddr=0x031c, .regData=0xce, .delayUs = 0 },
	 { .regAddr=0x0219, .regData=0x47, .delayUs = 0 },
	 { .regAddr=0x0342, .regData=0x05, .delayUs = 0 },//
	 { .regAddr=0x0343, .regData=0xdc, .delayUs = 0 },//
	 { .regAddr=0x0340, .regData=0x08, .delayUs = 0 },//20fps
	 { .regAddr=0x0341, .regData=0x98, .delayUs = 0 },//20fps
	 { .regAddr=0x029d, .regData=0x08, .delayUs = 0 },
	 { .regAddr=0x0345, .regData=0x02, .delayUs = 0 },
	 { .regAddr=0x0347, .regData=0x02, .delayUs = 0 },
	 { .regAddr=0x0348, .regData=0x0b, .delayUs = 0 },
	 { .regAddr=0x0349, .regData=0x98, .delayUs = 0 },//2968
	 { .regAddr=0x034a, .regData=0x06, .delayUs = 0 },
	 { .regAddr=0x034b, .regData=0x8a, .delayUs = 0 },//1674
	 { .regAddr=0x0094, .regData=0x0b, .delayUs = 0 },
	 { .regAddr=0x0095, .regData=0x40, .delayUs = 0 }, //2688
	 { .regAddr=0x0096, .regData=0x06, .delayUs = 0 },
	 { .regAddr=0x0097, .regData=0x5a, .delayUs = 0 }, //1512
 
	 { .regAddr=0x0099, .regData=0x28, .delayUs = 0 },
	 { .regAddr=0x009b, .regData=0x14, .delayUs = 0 },
	 { .regAddr=0x060c, .regData=0x01, .delayUs = 0 },
	 { .regAddr=0x060e, .regData=0xd2, .delayUs = 0 },
	 { .regAddr=0x060f, .regData=0x05, .delayUs = 0 },
	 { .regAddr=0x070c, .regData=0x01, .delayUs = 0 },
	 { .regAddr=0x070e, .regData=0xd2, .delayUs = 0 },
	 { .regAddr=0x070f, .regData=0x05, .delayUs = 0 },
	 { .regAddr=0x0709, .regData=0x40, .delayUs = 0 },
	 { .regAddr=0x0719, .regData=0x40, .delayUs = 0 },
	 { .regAddr=0x0909, .regData=0x07, .delayUs = 0 },
	 { .regAddr=0x0902, .regData=0x04, .delayUs = 0 },
	 { .regAddr=0x0904, .regData=0x0b, .delayUs = 0 },
	 { .regAddr=0x0907, .regData=0x54, .delayUs = 0 },
	 { .regAddr=0x0908, .regData=0x06, .delayUs = 0 },
	 { .regAddr=0x0903, .regData=0x9d, .delayUs = 0 },
	 { .regAddr=0x072a, .regData=0x1c, .delayUs = 0 },
	 { .regAddr=0x072b, .regData=0x1c, .delayUs = 0 },
	 { .regAddr=0x0724, .regData=0x2b, .delayUs = 0 },
	 { .regAddr=0x0727, .regData=0x2b, .delayUs = 0 },
	 { .regAddr=0x1466, .regData=0x18, .delayUs = 0 },
	 { .regAddr=0x1467, .regData=0x15, .delayUs = 0 },
	 { .regAddr=0x1468, .regData=0x15, .delayUs = 0 },
	 { .regAddr=0x1469, .regData=0x70, .delayUs = 0 },
	 { .regAddr=0x146a, .regData=0xe8, .delayUs = 0 },
	 { .regAddr=0x0707, .regData=0x07, .delayUs = 0 },
	 { .regAddr=0x0737, .regData=0x0f, .delayUs = 0 },
	 { .regAddr=0x0704, .regData=0x01, .delayUs = 0 },
	 { .regAddr=0x0706, .regData=0x02, .delayUs = 0 },
	 { .regAddr=0x0716, .regData=0x02, .delayUs = 0 },
	 { .regAddr=0x0708, .regData=0xc8, .delayUs = 0 },
	 { .regAddr=0x0718, .regData=0xc8, .delayUs = 0 },
	 { .regAddr=0x061a, .regData=0x02, .delayUs = 0 },
	 { .regAddr=0x1430, .regData=0x80, .delayUs = 0 },
	 { .regAddr=0x1407, .regData=0x10, .delayUs = 0 },
	 { .regAddr=0x1408, .regData=0x16, .delayUs = 0 },
	 { .regAddr=0x1409, .regData=0x03, .delayUs = 0 },
	 { .regAddr=0x1438, .regData=0x01, .delayUs = 0 },
	 { .regAddr=0x02ce, .regData=0x03, .delayUs = 0 },
	 { .regAddr=0x0245, .regData=0xc9, .delayUs = 0 },
	 { .regAddr=0x023a, .regData=0x08, .delayUs = 0 },
	 { .regAddr=0x02cd, .regData=0x88, .delayUs = 0 },
	 { .regAddr=0x0612, .regData=0x02, .delayUs = 0 },
	 { .regAddr=0x0613, .regData=0xc7, .delayUs = 0 },
	 { .regAddr=0x0243, .regData=0x03, .delayUs = 0 },
	 { .regAddr=0x0089, .regData=0x03, .delayUs = 0 },
	 { .regAddr=0x0002, .regData=0xab, .delayUs = 0 },
	 { .regAddr=0x0040, .regData=0xa3, .delayUs = 0 },
	 { .regAddr=0x0075, .regData=0x64, .delayUs = 0 },
	 { .regAddr=0x0004, .regData=0x0f, .delayUs = 0 },
	 { .regAddr=0x0053, .regData=0x0a, .delayUs = 0 },
	 { .regAddr=0x0205, .regData=0x0c, .delayUs = 0 },
	 { .regAddr=0x0a67, .regData=0x80, .delayUs = 0 },
	 { .regAddr=0x0a54, .regData=0x0e, .delayUs = 0 },
	 { .regAddr=0x0a65, .regData=0x10, .delayUs = 0 },
	 { .regAddr=0x0a98, .regData=0x04, .delayUs = 0 },
	 { .regAddr=0x05be, .regData=0x00, .delayUs = 0 },
	 { .regAddr=0x05a9, .regData=0x01, .delayUs = 0 },
	 { .regAddr=0x0023, .regData=0x00, .delayUs = 0 },
	 { .regAddr=0x0022, .regData=0x00, .delayUs = 0 },
	 { .regAddr=0x0025, .regData=0x00, .delayUs = 0 },
	 { .regAddr=0x0024, .regData=0x00, .delayUs = 0 },
	 { .regAddr=0x0028, .regData=0x0b, .delayUs = 0 },
	 { .regAddr=0x0029, .regData=0x98, .delayUs = 0 },
	 { .regAddr=0x002a, .regData=0x06, .delayUs = 0 },
	 { .regAddr=0x002b, .regData=0x86, .delayUs = 0 },
	 { .regAddr=0x0a83, .regData=0xe0, .delayUs = 0 },
	 { .regAddr=0x0a72, .regData=0x02, .delayUs = 0 },
	 { .regAddr=0x0a73, .regData=0x60, .delayUs = 0 },
	 { .regAddr=0x0a75, .regData=0x41, .delayUs = 0 },
	 { .regAddr=0x0a70, .regData=0x03, .delayUs = 0 },
	 { .regAddr=0x0a5a, .regData=0x80, .delayUs = 0 },
	 { .regAddr=0x0181, .regData=0x30, .delayUs = 0 },
	 { .regAddr=0x0182, .regData=0x05, .delayUs = 0 },
	 { .regAddr=0x0185, .regData=0x01, .delayUs = 0 },
	 { .regAddr=0x0180, .regData=0x46, .delayUs = 0 },
	 { .regAddr=0x0100, .regData=0x08, .delayUs = 0 },
	 { .regAddr=0x010d, .regData=0x10, .delayUs = 0 },
	 { .regAddr=0x010e, .regData=0x0e, .delayUs = 0 },
	 { .regAddr=0x0113, .regData=0x02, .delayUs = 0 },
	 { .regAddr=0x0114, .regData=0x01, .delayUs = 0 },
	 { .regAddr=0x0115, .regData=0x10, .delayUs = 0 },
	 { .regAddr=0x0a70, .regData=0x00, .delayUs = 0 },
	 { .regAddr=0x0080, .regData=0x02, .delayUs = 0 },
	 { .regAddr=0x0a67, .regData=0x00, .delayUs = 0 },
	 { .regAddr=0x0052, .regData=0x02, .delayUs = 0 },
	 { .regAddr=0x0076, .regData=0x01, .delayUs = 0 },
	 { .regAddr=0x021a, .regData=0x10, .delayUs = 0 },
	 { .regAddr=0x0049, .regData=0x0f, .delayUs = 0 }, //darkrow select
	 { .regAddr=0x004a, .regData=0x3c, .delayUs = 0 },
	 { .regAddr=0x004b, .regData=0x00, .delayUs = 0 },
	 { .regAddr=0x0430, .regData=0x25, .delayUs = 0 },
	 { .regAddr=0x0431, .regData=0x25, .delayUs = 0 },
	 { .regAddr=0x0432, .regData=0x25, .delayUs = 0 },
	 { .regAddr=0x0433, .regData=0x25, .delayUs = 0 },
	 { .regAddr=0x0434, .regData=0x59, .delayUs = 0 },
	 { .regAddr=0x0435, .regData=0x59, .delayUs = 0 },
	 { .regAddr=0x0436, .regData=0x59, .delayUs = 0 },
	 { .regAddr=0x0437, .regData=0x59, .delayUs = 0 },
 
	 { .regAddr=0x0a67, .regData=0x80, .delayUs = 0 },
	 { .regAddr=0x0a54, .regData=0x0e, .delayUs = 0 },
	 { .regAddr=0x0a65, .regData=0x10, .delayUs = 0 },
	 { .regAddr=0x0a98, .regData=0x04, .delayUs = 0 },
	 { .regAddr=0x05be, .regData=0x00, .delayUs = 0 },
	 { .regAddr=0x05a9, .regData=0x01, .delayUs = 0 },
	 { .regAddr=0x0023, .regData=0x00, .delayUs = 0 },
	 { .regAddr=0x0022, .regData=0x00, .delayUs = 0 },
	 { .regAddr=0x0025, .regData=0x00, .delayUs = 0 },
	 { .regAddr=0x0024, .regData=0x00, .delayUs = 0 },
	 { .regAddr=0x0028, .regData=0x0b, .delayUs = 0 },
	 { .regAddr=0x0029, .regData=0x98, .delayUs = 0 },
	 { .regAddr=0x002a, .regData=0x06, .delayUs = 0 },
	 { .regAddr=0x002b, .regData=0x86, .delayUs = 0 },
	 { .regAddr=0x0a83, .regData=0xe0, .delayUs = 0 },
	 { .regAddr=0x0a72, .regData=0x02, .delayUs = 0 },
	 { .regAddr=0x0a73, .regData=0x60, .delayUs = 0 },
	 { .regAddr=0x0a75, .regData=0x41, .delayUs = 0 },
	 { .regAddr=0x0a70, .regData=0x03, .delayUs = 0 },
	 { .regAddr=0x0a5a, .regData=0x80, .delayUs = 0 },
	 { .regAddr=0x0a70, .regData=0x00, .delayUs = 0 },
	 { .regAddr=0x0080, .regData=0x02, .delayUs = 0 },
	 { .regAddr=0x0a67, .regData=0x00, .delayUs = 0 },
	 //{ .regAddr=0x0100, .regData=0x09, .delayUs = 0 },
 };
 
 
 //GC5603 All-pixel scan CSI-2_2lane 27Mhz AD:10bit Output:10bit 423Mbps Master Mode 20.00fps Integration Time: Xx.xxms Gain:XdB vts = 2100 row time=23.8095us
 static drv_sensor_reg_info_t  gc5603_4mp_setting[] =
 {	
	   { .regAddr=0x03fe, .regData=0xf0, .delayUs=0 },
	 { .regAddr=0x03fe, .regData=0x00, .delayUs=0 },
	 { .regAddr=0x03fe, .regData=0x10, .delayUs=0 },
	 { .regAddr=0x03fe, .regData=0x00, .delayUs=0 },
	 { .regAddr=0x0a38, .regData=0x02, .delayUs=0 },
	 { .regAddr=0x0a38, .regData=0x03, .delayUs=0 },
	 { .regAddr=0x0a20, .regData=0x07, .delayUs=0 },
	 { .regAddr=0x061b, .regData=0x03, .delayUs=0 },
	 { .regAddr=0x061c, .regData=0x58, .delayUs=0 },
	 { .regAddr=0x061d, .regData=0x05, .delayUs=0 },
	 { .regAddr=0x061e, .regData=0x70, .delayUs=0 },
	 { .regAddr=0x061f, .regData=0x03, .delayUs=0 },
	 { .regAddr=0x0a21, .regData=0x08, .delayUs=0 },
	 { .regAddr=0x0a34, .regData=0x40, .delayUs=0 },
	 { .regAddr=0x0a35, .regData=0x50, .delayUs=0 },
	 { .regAddr=0x0a36, .regData=0x5e, .delayUs=0 },
	 { .regAddr=0x0a37, .regData=0x03, .delayUs=0 },
	 { .regAddr=0x0314, .regData=0x50, .delayUs=0 },
	 { .regAddr=0x0315, .regData=0x32, .delayUs=0 },
	 { .regAddr=0x031c, .regData=0xce, .delayUs=0 },
	 { .regAddr=0x0219, .regData=0x47, .delayUs=0 }, 
	 { .regAddr=0x0342, .regData=0x05, .delayUs=0 },//
	 { .regAddr=0x0343, .regData=0xdc, .delayUs=0 },//1500
	 { .regAddr=0x0340, .regData=0x08, .delayUs=0 },//
	 { .regAddr=0x0341, .regData=0x34, .delayUs=0 },//2100
	 
	 { .regAddr=0x0345, .regData=0x02, .delayUs=0 },
	 { .regAddr=0x0347, .regData=0x02, .delayUs=0 },
	 { .regAddr=0x0348, .regData=0x0b, .delayUs=0 }, 
	 { .regAddr=0x0349, .regData=0x98, .delayUs=0 },//2968
	 { .regAddr=0x034a, .regData=0x06, .delayUs=0 },
	 { .regAddr=0x034b, .regData=0x8a, .delayUs=0 },//1674
	 
	 { .regAddr=0x0094, .regData=0x0a, .delayUs=0 }, 
	 { .regAddr=0x0095, .regData=0x00, .delayUs=0 },//2560
	 { .regAddr=0x0096, .regData=0x05, .delayUs=0 }, 
	 { .regAddr=0x0097, .regData=0xa0, .delayUs=0 },//1440
	 { .regAddr=0x0099, .regData=0x75, .delayUs=0 },//117
	 { .regAddr=0x009b, .regData=0xcc, .delayUs=0 },//204
	 
	 { .regAddr=0x060c, .regData=0x01, .delayUs=0 },//
	 { .regAddr=0x060e, .regData=0xd2, .delayUs=0 },
	 { .regAddr=0x060f, .regData=0x05, .delayUs=0 },
	 { .regAddr=0x070c, .regData=0x01, .delayUs=0 },//
	 { .regAddr=0x070e, .regData=0xd2, .delayUs=0 },
	 { .regAddr=0x070f, .regData=0x05, .delayUs=0 },
	 { .regAddr=0x0709, .regData=0x40, .delayUs=0 },
	 { .regAddr=0x0719, .regData=0x40, .delayUs=0 },
	 { .regAddr=0x0909, .regData=0x07, .delayUs=0 },
	 { .regAddr=0x0902, .regData=0x04, .delayUs=0 },
	 { .regAddr=0x0904, .regData=0x0b, .delayUs=0 },
	 { .regAddr=0x0907, .regData=0x54, .delayUs=0 },
	 { .regAddr=0x0908, .regData=0x06, .delayUs=0 },
	 { .regAddr=0x0903, .regData=0x9d, .delayUs=0 },
	 { .regAddr=0x072a, .regData=0x1c, .delayUs=0 },
	 { .regAddr=0x072b, .regData=0x1c, .delayUs=0 },
	 { .regAddr=0x0724, .regData=0x2b, .delayUs=0 },
	 { .regAddr=0x0727, .regData=0x2b, .delayUs=0 },
	 { .regAddr=0x1466, .regData=0x18, .delayUs=0 },
	 { .regAddr=0x1467, .regData=0x15, .delayUs=0 },
	 { .regAddr=0x1468, .regData=0x15, .delayUs=0 },
	 { .regAddr=0x1469, .regData=0x70, .delayUs=0 },
	 { .regAddr=0x146a, .regData=0xe8, .delayUs=0 },
	 { .regAddr=0x0707, .regData=0x07, .delayUs=0 },
	 { .regAddr=0x0737, .regData=0x0f, .delayUs=0 },
	 { .regAddr=0x0704, .regData=0x01, .delayUs=0 },
	 { .regAddr=0x0706, .regData=0x02, .delayUs=0 },
	 { .regAddr=0x0716, .regData=0x02, .delayUs=0 },
	 { .regAddr=0x0708, .regData=0xc8, .delayUs=0 },
	 { .regAddr=0x0718, .regData=0xc8, .delayUs=0 },
	 { .regAddr=0x061a, .regData=0x02, .delayUs=0 },
	 { .regAddr=0x1430, .regData=0x80, .delayUs=0 },
	 { .regAddr=0x1407, .regData=0x10, .delayUs=0 },
	 { .regAddr=0x1408, .regData=0x16, .delayUs=0 },
	 { .regAddr=0x1409, .regData=0x03, .delayUs=0 },
	 { .regAddr=0x1438, .regData=0x01, .delayUs=0 },
	 { .regAddr=0x02ce, .regData=0x03, .delayUs=0 },
	 { .regAddr=0x0245, .regData=0xc9, .delayUs=0 },
	 { .regAddr=0x023a, .regData=0x08, .delayUs=0 },
	 { .regAddr=0x02cd, .regData=0x88, .delayUs=0 },
	 { .regAddr=0x0612, .regData=0x02, .delayUs=0 },
	 { .regAddr=0x0613, .regData=0xc7, .delayUs=0 },
	 { .regAddr=0x0243, .regData=0x03, .delayUs=0 },
	 { .regAddr=0x0089, .regData=0x03, .delayUs=0 },
	 { .regAddr=0x0002, .regData=0xab, .delayUs=0 },
	 { .regAddr=0x0040, .regData=0xa3, .delayUs=0 },
	 { .regAddr=0x0075, .regData=0x64, .delayUs=0 },
	 { .regAddr=0x0004, .regData=0x0f, .delayUs=0 },
	 { .regAddr=0x0053, .regData=0x0a, .delayUs=0 },
	 { .regAddr=0x0205, .regData=0x0c, .delayUs=0 },
	 { .regAddr=0x0a67, .regData=0x80, .delayUs=0 },
	 { .regAddr=0x0a54, .regData=0x0e, .delayUs=0 },
	 { .regAddr=0x0a65, .regData=0x10, .delayUs=0 },
	 { .regAddr=0x0a98, .regData=0x04, .delayUs=0 },
	 { .regAddr=0x05be, .regData=0x00, .delayUs=0 },
	 { .regAddr=0x05a9, .regData=0x01, .delayUs=0 },
	 { .regAddr=0x0023, .regData=0x00, .delayUs=0 },
	 { .regAddr=0x0022, .regData=0x00, .delayUs=0 },
	 { .regAddr=0x0025, .regData=0x00, .delayUs=0 },
	 { .regAddr=0x0024, .regData=0x00, .delayUs=0 },
	 { .regAddr=0x0028, .regData=0x0b, .delayUs=0 },
	 { .regAddr=0x0029, .regData=0x98, .delayUs=0 },
	 { .regAddr=0x002a, .regData=0x06, .delayUs=0 },
	 { .regAddr=0x002b, .regData=0x86, .delayUs=0 },
	 { .regAddr=0x0a83, .regData=0xe0, .delayUs=0 },
	 { .regAddr=0x0a72, .regData=0x02, .delayUs=0 },
	 { .regAddr=0x0a73, .regData=0x60, .delayUs=0 },
	 { .regAddr=0x0a75, .regData=0x41, .delayUs=0 },
	 { .regAddr=0x0a70, .regData=0x03, .delayUs=0 },
	 { .regAddr=0x0a5a, .regData=0x80, .delayUs=0 },
	 { .regAddr=0x0181, .regData=0x30, .delayUs=0 },
	 { .regAddr=0x0182, .regData=0x05, .delayUs=0 },
	 { .regAddr=0x0185, .regData=0x01, .delayUs=0 },
	 { .regAddr=0x0180, .regData=0x46, .delayUs=0 },
	 { .regAddr=0x0100, .regData=0x08, .delayUs=0 },
	 { .regAddr=0x010d, .regData=0x80, .delayUs=0 },//LWC
	 { .regAddr=0x010e, .regData=0x0C, .delayUs=0 },//
	 { .regAddr=0x0113, .regData=0x02, .delayUs=0 },
	 { .regAddr=0x0114, .regData=0x01, .delayUs=0 },
	 { .regAddr=0x0115, .regData=0x10, .delayUs=0 },
	 { .regAddr=0x0a70, .regData=0x00, .delayUs=0 },
	 { .regAddr=0x0080, .regData=0x02, .delayUs=0 },
	 { .regAddr=0x0a67, .regData=0x00, .delayUs=0 },
	 { .regAddr=0x0052, .regData=0x02, .delayUs=0 },
	 { .regAddr=0x0076, .regData=0x01, .delayUs=0 },
	 { .regAddr=0x021a, .regData=0x10, .delayUs=0 },
	 { .regAddr=0x0049, .regData=0x0f, .delayUs=0 },
	 { .regAddr=0x004a, .regData=0x3c, .delayUs=0 },
	 { .regAddr=0x004b, .regData=0x00, .delayUs=0 },
	 { .regAddr=0x0430, .regData=0x25, .delayUs=0 },
	 { .regAddr=0x0431, .regData=0x25, .delayUs=0 },
	 { .regAddr=0x0432, .regData=0x25, .delayUs=0 },
	 { .regAddr=0x0433, .regData=0x25, .delayUs=0 },
	 { .regAddr=0x0434, .regData=0x59, .delayUs=0 },
	 { .regAddr=0x0435, .regData=0x59, .delayUs=0 },
	 { .regAddr=0x0436, .regData=0x59, .delayUs=0 },
	 { .regAddr=0x0437, .regData=0x59, .delayUs=0 },
	 //{ .regAddr=0x0100, .regData=0x09, .delayUs=0 },
 };
 
 //GC5603 All-pixel scan CSI-2_2lane 27Mhz AD:10bit Output:10bit 423Mbps Master Mode 15.00fps Integration Time: Xx.xxms Gain:XdB vts = 2100 row time=31.7460us
 static drv_sensor_reg_info_t  gc5603_4mp_15fps_setting[] =
 {
	 { .regAddr=0x03fe, .regData=0xf0, .delayUs=0 },
	 { .regAddr=0x03fe, .regData=0x00, .delayUs=0 },
	 { .regAddr=0x03fe, .regData=0x10, .delayUs=0 },
	 { .regAddr=0x03fe, .regData=0x00, .delayUs=0 },
	 { .regAddr=0x0a38, .regData=0x02, .delayUs=0 },
	 { .regAddr=0x0a38, .regData=0x03, .delayUs=0 },
	 { .regAddr=0x0a20, .regData=0x07, .delayUs=0 },
	 { .regAddr=0x061b, .regData=0x03, .delayUs=0 },
	 { .regAddr=0x061c, .regData=0x58, .delayUs=0 },
	 { .regAddr=0x061d, .regData=0x05, .delayUs=0 },
	 { .regAddr=0x061e, .regData=0x70, .delayUs=0 },
	 { .regAddr=0x061f, .regData=0x03, .delayUs=0 },
	 { .regAddr=0x0a21, .regData=0x08, .delayUs=0 },
	 { .regAddr=0x0a34, .regData=0x40, .delayUs=0 },
	 { .regAddr=0x0a35, .regData=0x50, .delayUs=0 },
	 { .regAddr=0x0a36, .regData=0x5e, .delayUs=0 },
	 { .regAddr=0x0a37, .regData=0x03, .delayUs=0 },
	 { .regAddr=0x0314, .regData=0x50, .delayUs=0 },
	 { .regAddr=0x0315, .regData=0x32, .delayUs=0 },
	 { .regAddr=0x031c, .regData=0xce, .delayUs=0 },
	 { .regAddr=0x0219, .regData=0x47, .delayUs=0 }, 
	 { .regAddr=0x0342, .regData=0x07, .delayUs=0 },//
	 { .regAddr=0x0343, .regData=0xd0, .delayUs=0 },//2000
	 { .regAddr=0x0340, .regData=0x08, .delayUs=0 },//
	 { .regAddr=0x0341, .regData=0x34, .delayUs=0 },//2100
	 
	 { .regAddr=0x0345, .regData=0x02, .delayUs=0 },
	 { .regAddr=0x0347, .regData=0x02, .delayUs=0 },
	 { .regAddr=0x0348, .regData=0x0b, .delayUs=0 }, 
	 { .regAddr=0x0349, .regData=0x98, .delayUs=0 },//2968
	 { .regAddr=0x034a, .regData=0x06, .delayUs=0 },
	 { .regAddr=0x034b, .regData=0x8a, .delayUs=0 },//1674
	 
	 { .regAddr=0x0094, .regData=0x0a, .delayUs=0 }, 
	 { .regAddr=0x0095, .regData=0x00, .delayUs=0 },//2560
	 { .regAddr=0x0096, .regData=0x05, .delayUs=0 }, 
	 { .regAddr=0x0097, .regData=0xa0, .delayUs=0 },//1440
	 { .regAddr=0x0099, .regData=0x75, .delayUs=0 },//117
	 { .regAddr=0x009b, .regData=0xcc, .delayUs=0 },//204
	 
	 { .regAddr=0x060c, .regData=0x01, .delayUs=0 },//
	 { .regAddr=0x060e, .regData=0xd2, .delayUs=0 },
	 { .regAddr=0x060f, .regData=0x05, .delayUs=0 },
	 { .regAddr=0x070c, .regData=0x01, .delayUs=0 },//
	 { .regAddr=0x070e, .regData=0xd2, .delayUs=0 },
	 { .regAddr=0x070f, .regData=0x05, .delayUs=0 },
	 { .regAddr=0x0709, .regData=0x40, .delayUs=0 },
	 { .regAddr=0x0719, .regData=0x40, .delayUs=0 },
	 { .regAddr=0x0909, .regData=0x07, .delayUs=0 },
	 { .regAddr=0x0902, .regData=0x04, .delayUs=0 },
	 { .regAddr=0x0904, .regData=0x0b, .delayUs=0 },
	 { .regAddr=0x0907, .regData=0x54, .delayUs=0 },
	 { .regAddr=0x0908, .regData=0x06, .delayUs=0 },
	 { .regAddr=0x0903, .regData=0x9d, .delayUs=0 },
	 { .regAddr=0x072a, .regData=0x1c, .delayUs=0 },
	 { .regAddr=0x072b, .regData=0x1c, .delayUs=0 },
	 { .regAddr=0x0724, .regData=0x2b, .delayUs=0 },
	 { .regAddr=0x0727, .regData=0x2b, .delayUs=0 },
	 { .regAddr=0x1466, .regData=0x18, .delayUs=0 },
	 { .regAddr=0x1467, .regData=0x15, .delayUs=0 },
	 { .regAddr=0x1468, .regData=0x15, .delayUs=0 },
	 { .regAddr=0x1469, .regData=0x70, .delayUs=0 },
	 { .regAddr=0x146a, .regData=0xe8, .delayUs=0 },
	 { .regAddr=0x0707, .regData=0x07, .delayUs=0 },
	 { .regAddr=0x0737, .regData=0x0f, .delayUs=0 },
	 { .regAddr=0x0704, .regData=0x01, .delayUs=0 },
	 { .regAddr=0x0706, .regData=0x02, .delayUs=0 },
	 { .regAddr=0x0716, .regData=0x02, .delayUs=0 },
	 { .regAddr=0x0708, .regData=0xc8, .delayUs=0 },
	 { .regAddr=0x0718, .regData=0xc8, .delayUs=0 },
	 { .regAddr=0x061a, .regData=0x02, .delayUs=0 },
	 { .regAddr=0x1430, .regData=0x80, .delayUs=0 },
	 { .regAddr=0x1407, .regData=0x10, .delayUs=0 },
	 { .regAddr=0x1408, .regData=0x16, .delayUs=0 },
	 { .regAddr=0x1409, .regData=0x03, .delayUs=0 },
	 { .regAddr=0x1438, .regData=0x01, .delayUs=0 },
	 { .regAddr=0x02ce, .regData=0x03, .delayUs=0 },
	 { .regAddr=0x0245, .regData=0xc9, .delayUs=0 },
	 { .regAddr=0x023a, .regData=0x08, .delayUs=0 },
	 { .regAddr=0x02cd, .regData=0x88, .delayUs=0 },
	 { .regAddr=0x0612, .regData=0x02, .delayUs=0 },
	 { .regAddr=0x0613, .regData=0xc7, .delayUs=0 },
	 { .regAddr=0x0243, .regData=0x03, .delayUs=0 },
	 { .regAddr=0x0089, .regData=0x03, .delayUs=0 },
	 { .regAddr=0x0002, .regData=0xab, .delayUs=0 },
	 { .regAddr=0x0040, .regData=0xa3, .delayUs=0 },
	 { .regAddr=0x0075, .regData=0x64, .delayUs=0 },
	 { .regAddr=0x0004, .regData=0x0f, .delayUs=0 },
	 { .regAddr=0x0053, .regData=0x0a, .delayUs=0 },
	 { .regAddr=0x0205, .regData=0x0c, .delayUs=0 },
	 { .regAddr=0x0a67, .regData=0x80, .delayUs=0 },
	 { .regAddr=0x0a54, .regData=0x0e, .delayUs=0 },
	 { .regAddr=0x0a65, .regData=0x10, .delayUs=0 },
	 { .regAddr=0x0a98, .regData=0x04, .delayUs=0 },
	 { .regAddr=0x05be, .regData=0x00, .delayUs=0 },
	 { .regAddr=0x05a9, .regData=0x01, .delayUs=0 },
	 { .regAddr=0x0023, .regData=0x00, .delayUs=0 },
	 { .regAddr=0x0022, .regData=0x00, .delayUs=0 },
	 { .regAddr=0x0025, .regData=0x00, .delayUs=0 },
	 { .regAddr=0x0024, .regData=0x00, .delayUs=0 },
	 { .regAddr=0x0028, .regData=0x0b, .delayUs=0 },
	 { .regAddr=0x0029, .regData=0x98, .delayUs=0 },
	 { .regAddr=0x002a, .regData=0x06, .delayUs=0 },
	 { .regAddr=0x002b, .regData=0x86, .delayUs=0 },
	 { .regAddr=0x0a83, .regData=0xe0, .delayUs=0 },
	 { .regAddr=0x0a72, .regData=0x02, .delayUs=0 },
	 { .regAddr=0x0a73, .regData=0x60, .delayUs=0 },
	 { .regAddr=0x0a75, .regData=0x41, .delayUs=0 },
	 { .regAddr=0x0a70, .regData=0x03, .delayUs=0 },
	 { .regAddr=0x0a5a, .regData=0x80, .delayUs=0 },
	 { .regAddr=0x0181, .regData=0x30, .delayUs=0 },
	 { .regAddr=0x0182, .regData=0x05, .delayUs=0 },
	 { .regAddr=0x0185, .regData=0x01, .delayUs=0 },
	 { .regAddr=0x0180, .regData=0x46, .delayUs=0 },
	 { .regAddr=0x0100, .regData=0x08, .delayUs=0 },
	 { .regAddr=0x010d, .regData=0x80, .delayUs=0 },//LWC
	 { .regAddr=0x010e, .regData=0x0C, .delayUs=0 },//
	 { .regAddr=0x0113, .regData=0x02, .delayUs=0 },
	 { .regAddr=0x0114, .regData=0x01, .delayUs=0 },
	 { .regAddr=0x0115, .regData=0x10, .delayUs=0 },
	 { .regAddr=0x0a70, .regData=0x00, .delayUs=0 },
	 { .regAddr=0x0080, .regData=0x02, .delayUs=0 },
	 { .regAddr=0x0a67, .regData=0x00, .delayUs=0 },
	 { .regAddr=0x0052, .regData=0x02, .delayUs=0 },
	 { .regAddr=0x0076, .regData=0x01, .delayUs=0 },
	 { .regAddr=0x021a, .regData=0x10, .delayUs=0 },
	 { .regAddr=0x0049, .regData=0x0f, .delayUs=0 },
	 { .regAddr=0x004a, .regData=0x3c, .delayUs=0 },
	 { .regAddr=0x004b, .regData=0x00, .delayUs=0 },
	 { .regAddr=0x0430, .regData=0x25, .delayUs=0 },
	 { .regAddr=0x0431, .regData=0x25, .delayUs=0 },
	 { .regAddr=0x0432, .regData=0x25, .delayUs=0 },
	 { .regAddr=0x0433, .regData=0x25, .delayUs=0 },
	 { .regAddr=0x0434, .regData=0x59, .delayUs=0 },
	 { .regAddr=0x0435, .regData=0x59, .delayUs=0 },
	 { .regAddr=0x0436, .regData=0x59, .delayUs=0 },
	 { .regAddr=0x0437, .regData=0x59, .delayUs=0 },
	 { .regAddr=0x0100, .regData=0x09, .delayUs=0 },
 };
 
 //GC5603 All-pixel scan CSI-2_2lane 24Mhz AD:10bit Output:10bit 704Mbps Master Mode 30.00fps Integration Time: Xx.xxms Gain:XdB vts = 1500 row time=22.22us
 static drv_sensor_reg_info_t  gc5603_1080p_setting[] =
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
	 { .regAddr=0x0094, .regData= 0x07, .delayUs=0 },  //1920
	 { .regAddr=0x0095, .regData= 0x80, .delayUs=0 },
	 { .regAddr=0x0096, .regData= 0x04, .delayUs=0 },  //1080
	 { .regAddr=0x0097, .regData= 0x38, .delayUs=0 },
	 { .regAddr=0x0099, .regData= 0x04, .delayUs=0 },
	 { .regAddr=0x009b, .regData= 0x04, .delayUs=0 },
 #if 0
	 /*windows*/
	 { .regAddr=0x0351, .regData= 0x00, .delayUs=0 }, //row start
	 { .regAddr=0x0352, .regData= 0x00, .delayUs=0 },
	 { .regAddr=0x0353, .regData= 0x00, .delayUs=0 }, //col start
	 { .regAddr=0x0354, .regData= 0x00, .delayUs=0 },
	 { .regAddr=0x034c, .regData= 0x07, .delayUs=0 }, //width 1920
	 { .regAddr=0x034d, .regData= 0x80, .delayUs=0 },
	 { .regAddr=0x034e, .regData= 0x04, .delayUs=0 }, //height 1080
	 { .regAddr=0x034f, .regData= 0x38, .delayUs=0 },
 #endif
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
	 /*fsync slave*/
	 { .regAddr=0x027f, .regData= 0x03, .delayUs=0 },
	 { .regAddr=0x02f7, .regData= 0x82, .delayUs=0 },
	 { .regAddr=0x02e1, .regData= 0x07, .delayUs=0 },
	 { .regAddr=0x02f1, .regData= 0x1a, .delayUs=0 },
	 { .regAddr=0x02f2, .regData= 0x00, .delayUs=0 },
 #endif
 };
 
 //GC5603 All-pixel scan CSI-2_2lane 24Mhz AD:10bit Output:10bit 352Mbps Master Mode 15.00fps Integration Time: Xx.xxms Gain:XdB vts = 1500 row time=44.44us
 static drv_sensor_reg_info_t  gc5603_1080p_15fps_setting[] =
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
 
 
 //GC5603 All-pixel scan CSI-2_2lane 24Mhz AD:10bit Output:10bit 704Mbps Master Mode 30.00fps Integration Time: Xx.xxms Gain:XdB vts = 1500 row time=22.22us
 static drv_sensor_reg_info_t  gc5603_4mp_10fps_setting[] =
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
 
 static drv_sensor_reg_info_t  gc5603_streamOn_setting[] =
 {
	 { .regAddr=0x0100, .regData= 0x09, .delayUs=0 }, //使能开关
 };
 
 static drv_sensor_reg_info_t  gc5603_streamOff_setting[] =
 {
	 { .regAddr=0x0100, .regData= 0x00, .delayUs=500000 }, //500ms
 };
 
 static drv_sensor_exp_param_t gc5603_20fps_exparams =
 {
	 .maxIntegrationTime = 2925,
	 .minIntegrationTime = 1,
	 .maxAnalogGain      = 262144,//65536,
	 .minAnalogGain      = 1024,
	 .lineTime           = 93091,
	 .limitShortMid      = 0,
	 .limitLong          = 0,
	 .expLineMargin      = 8,
 };
 static drv_sensor_exp_param_t gc5603_15fps_exparams =
 {
	 .maxIntegrationTime = MAX_INTEGRATION_TIME,
	 .minIntegrationTime = 1,
	 .maxAnalogGain      = 262144,
	 .minAnalogGain      = 1024,
	 .lineTime           = LINE_TIME,
	 .limitShortMid      = 0,
	 .limitLong          = 0,
	 .expLineMargin      = 8,
 };
 static drv_sensor_exp_param_t gc5603_15fps_sensor30_exparams =
 {
	 .maxIntegrationTime = MAX_INTEGRATION_TIME,//3750,//3500,		//0x0dac - 8   0x0dac
	 .minIntegrationTime = 1,
	 .maxAnalogGain      = 262144,
	 .minAnalogGain      = 1024,
	 .lineTime           = LINE_TIME,//72817,//78019,		//4096000000/15/3492  4096000000/15/3500
	 .limitShortMid      = 0,
	 .limitLong          = 0,
	 .expLineMargin      = 8,
 };
 static drv_sensor_exp_param_t gc5603_10fps_exparams =
 {
	 .maxIntegrationTime = 2192,//2980,
	 .minIntegrationTime = 0,
	 .maxAnalogGain      = 65536, //266300, //65536,//133300,
	 .minAnalogGain      = 1024,
	 .lineTime           = 93091,//182044,
	 .limitShortMid      = 0,
	 .limitLong          = 0,
	 .expLineMargin      = 1,
 };
 static drv_sensor_exp_param_t gc5603_1080p_15fps_exparams =
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
 
 static drv_power_setting_t gc5603_power_up_setting[] =
 {
	 {.type = DRV_POWER_REG, .regAddr = 0x83c40004, .bitMask  = 0x01, .bitVal = 0x00, .delayUs = 100000}, /// 100ms
	 {.type = DRV_POWER_REG, .regAddr = 0x83c40004, .bitMask  = 0x01, .bitVal = 0x01, .delayUs = 100000}  /// APB2 SENSOR_CTRL_REG 0x83c40004
 };
 
 static drv_power_setting_t gc5603_power_down_setting[] =
 {
	 {.type = DRV_POWER_REG, .regAddr = 0x83c40004, .bitMask  = 0x01, .bitVal = 0x00, .delayUs = 100000},
	 {.type = DRV_POWER_REG, .regAddr = 0x83c40004, .bitMask  = 0x01, .bitVal = 0x01, .delayUs = 100000}
 };
 
 static sensor_drv_params_t gc5603_sensor_params =
 {
	 .sensorId     = 0x03,
	 .sensorIdAddr = 0x3f1,
	 .name         = "gc5603",
	 .slaveInfo =
	 {
		 .hwType      = DRV_HW_I2C,
 #if defined(TARGET_CHIP_TX5215) && defined(TARGET_BOARD_ANSJERC528_DOUBLE5M)
		 .slaveAddr   = (0x20 >> 1),
 #else
		 .slaveAddr   = (0x20 >> 1),
 #endif
		 .channel     = 0,
		 .regDataType = DRV_ADDR_TYPE_BYTE,
		 .regAddrType = DRV_DATA_TYPE_WORD,
	 },
	 .powerUpSettings  =
	 {
		 .powerSetting = gc5603_power_up_setting,
		 .size = sizeof(gc5603_power_up_setting) / sizeof(gc5603_power_up_setting[0]),
	 },
	 .powerDownSettings =
	 {
		 .powerSetting  = gc5603_power_down_setting,
		 .size = sizeof(gc5603_power_down_setting) / sizeof(gc5603_power_down_setting[0]),
	 },
	 .activesize =
	 {
		 .width  = 2688,
		 .height = 1512,
		 //.width  = 2880,
		 //.height = 1626,
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
		 .maxFps = 30,
		 .minFps = 10,
		 .frameLength = FRAME_LENTH,
		 .expLineMargin = 8,
		 .dim =
		 {
			 .width  = 2688,
			 .height = 1512,
			 //.width  = 2880,
			 //.height = 1624,
		 },
		 .hdrMode   = SENSOR_NORMAL_MODE,
		 .regConfig =
		 {
			 .regDataType = DRV_DATA_TYPE_BYTE,
			 .regAddrType = DRV_ADDR_TYPE_WORD,
			 .direction   = DRV_DIRECTION_WRITE,
 #if SENSOR_FPS_CFG == 1
			 .settings    = gc5603_5mp_setting,
			 .settingSize = sizeof(gc5603_5mp_setting) / sizeof (gc5603_5mp_setting[0]),
 #elif SENSOR_FPS_CFG == 2
			 .settings    = gc5603_5mp_30fps_setting,
			 .settingSize = sizeof(gc5603_5mp_30fps_setting) / sizeof (gc5603_5mp_30fps_setting[0]),
 #endif
		 },
 
 #if SENSOR_FPS_CFG == 1
		 .expParams   = &gc5603_15fps_exparams,
 #elif SENSOR_FPS_CFG == 2
		 .expParams   = &gc5603_15fps_sensor30_exparams,
 #endif
		 .intf = {
			 .ln_cfg = {
				 .dat_num = 2,
			 },
 #if SENSOR_FPS_CFG == 1
			 .link_freq = 508.0f,
 #elif SENSOR_FPS_CFG == 2
			 .link_freq = 848.0f,
 #endif
		 },
	 },
	 .modeSettings[1] =
	 {
		 .maxFps = 30,
		 .minFps = 15,
		 .frameLength = 0xb34,
		 .expLineMargin = 1,
		 .dim =
		 {
			 .width  = 2560,
			 .height = 1400,
		 },
		 .hdrMode   = SENSOR_NORMAL_MODE,
		 .regConfig =
		 {
			 .regDataType = DRV_DATA_TYPE_BYTE,
			 .regAddrType = DRV_ADDR_TYPE_WORD,
			 .direction   = DRV_DIRECTION_WRITE,
			 .settings    = gc5603_4mp_15fps_setting,
			 .settingSize = sizeof(gc5603_4mp_15fps_setting) / sizeof (gc5603_4mp_15fps_setting[0]),
		 },
		 .expParams   = &gc5603_15fps_exparams,
 
		 .intf = {
			 .ln_cfg = {
				 .dat_num = 2,
			 },
			 .link_freq = 423.0f,
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
			 .settings    = gc5603_4mp_10fps_setting,
			 .settingSize = sizeof(gc5603_4mp_10fps_setting) / sizeof (gc5603_4mp_10fps_setting[0]),
		 },
		 .expParams   = &gc5603_10fps_exparams,
 
		 .intf = {
			 .ln_cfg = {
				 .dat_num = 2,
			 },
			 .link_freq = 704.0f,
		 },
	 },
	 .modeSettings[3] =
	 {
		 .maxFps = 15,
		 .minFps = 5,
		 .frameLength = 0x5dc,
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
			 .settings    = gc5603_1080p_15fps_setting,
			 .settingSize = sizeof(gc5603_1080p_15fps_setting) / sizeof (gc5603_1080p_15fps_setting[0]),
		 },
		 .expParams   = &gc5603_1080p_15fps_exparams,
 
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
		 .settings    = gc5603_streamOn_setting,
		 .settingSize = sizeof(gc5603_streamOn_setting) / sizeof (gc5603_streamOn_setting[0]),
	 },
 
	 .streamOffSetting =
	 {
		 .regDataType = DRV_DATA_TYPE_BYTE,
		 .regAddrType = DRV_ADDR_TYPE_WORD,
		 .direction   = DRV_DIRECTION_WRITE,
		 .settings    = gc5603_streamOff_setting,
		 .settingSize = sizeof(gc5603_streamOff_setting) / sizeof (gc5603_streamOff_setting[0]),
	 },
 
	 .fill_exposure_reg = GC5603_Fill_Exposure_Reg1,
 };
 
 
 sensor_drv_params_t* load_gc5603_sensor_driver(void);
 
 #endif
 
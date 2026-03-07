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

#ifndef __CV2003_AUX2_SENSOR_H__
#define __CV2003_AUX2_SENSOR_H__

#include "sensor_common.h"
#include "log_utils.h"

isp_ret_t CV2003_Fill_Exposure_Reg(sensor_apply_exp_param_t * pExpParam, drv_regs_array_info_t * pRegInfo);

//CV2003-AAQR Bining mode CSI-2_2lane 37.125Mhz AD:10bit Output:12bit 1782Mbps Master Mode fps Integration Time xx.xxms Gain:6dB
static drv_sensor_reg_info_t  cv2003_1080p_30fps_setting_2lane[] =
{
	{ .regAddr= 0x3000, .regData= 0x01, .delayUs=0 }, /* enter standby */
	{ .regAddr= 0x3002, .regData= 0x01, .delayUs=0 }, /* master stop */
	{ .regAddr= 0x3008, .regData= 0x7F, .delayUs=0 }, // BCWAIT_TIME[9:0]
	{ .regAddr= 0x3009, .regData= 0x00, .delayUs=0 }, // BCWAIT_TIME[9:0]
	{ .regAddr= 0x300A, .regData= 0x5B, .delayUs=0 }, // CPWAIT_TIME[9:0]
	{ .regAddr= 0x300B, .regData= 0x00, .delayUs=0 }, // CPWAIT_TIME[9:0]
	{ .regAddr= 0x301c, .regData= 0x0, .delayUs=0 },  //WINMODE
	{ .regAddr= 0x3020, .regData= 0x1, .delayUs=0 },  //horizontal 2 binning
	{ .regAddr= 0x3021, .regData= 0x1, .delayUs=0 },  //veritical 2 binning
	{ .regAddr= 0x3022, .regData= 0x1, .delayUs=0 },  //ADDMODE 2/2-line binning
	{ .regAddr= 0x3024, .regData= 0xCA, .delayUs=0 }, // VMAX[19:0]
	{ .regAddr= 0x3025, .regData= 0x08, .delayUs=0 }, // VMAX[19:0]
	{ .regAddr= 0x3026, .regData= 0x00, .delayUs=0 }, // VMAX[19:0]
	{ .regAddr= 0x3028, .regData= 0x4C, .delayUs=0 }, // HMAX[15:0]
	{ .regAddr= 0x3029, .regData= 0x04, .delayUs=0 }, // HMAX[15:0]
	/*
	{ .regAddr= 0x302c, .regData= 0x00, .delayUs=0 }, // WDMODE[1:0]
	{ .regAddr= 0x302d, .regData= 0x00, .delayUs=0 }, // WDSEL[1:0]
	{ .regAddr= 0x30CF, .regData= 0x00, .delayUs=0 }, // XVSMSKCNT_INT[1:0]
	*/
    { .regAddr= 0x3030, .regData = 0x0, .delayUs = 0x0},
	{ .regAddr= 0x3031, .regData= 0x00, .delayUs=0 }, // AD:10 bit
	{ .regAddr= 0x3032, .regData= 0x01, .delayUs=0 }, // MD:12 bit
	{ .regAddr= 0x3033, .regData= 0x04, .delayUs=0 }, // SYS_MODE[3:0]
	{ .regAddr= 0x3034, .regData= 0x04, .delayUs=0 }, // SYS_MODE[3:0]
	{ .regAddr= 0x3050, .regData= 0xC6, .delayUs=0 }, // SHR0[19:0]
	{ .regAddr= 0x3051, .regData= 0x07, .delayUs=0 }, // SHR0[19:0]
	{ .regAddr= 0x3052, .regData= 0x00, .delayUs=0 }, // SHR0[19:0]
	{ .regAddr= 0x3090, .regData= 0x00, .delayUs=0 }, // GAIN_PCG_0[8:0]
	{ .regAddr= 0x3091, .regData= 0x00, .delayUs=0 }, // GAIN_PCG_0[8:0]
	{ .regAddr= 0x30C1, .regData= 0x00, .delayUs=0 }, // XVS_DRV[1:0]
	{ .regAddr= 0x30D9, .regData= 0x02, .delayUs=0 }, // DIG_CLP_VSTAET[4:0]
	{ .regAddr= 0x30DA, .regData= 0x01, .delayUs=0 }, // DIG_VLP_VNUM[1:0]
	{ .regAddr= 0x3115, .regData= 0x00, .delayUs=0 }, // INCKSEL1[7:0]
	{ .regAddr= 0x3116, .regData= 0x24, .delayUs=0 }, // INCKSEL2[7:0]
	{ .regAddr= 0x3118, .regData= 0xC0, .delayUs=0 }, // INCKSEL3[10:0]
	{ .regAddr= 0x3119, .regData= 0x00, .delayUs=0 }, // INCKSEL3[10:0]
	{ .regAddr= 0x311A, .regData= 0xE0, .delayUs=0 }, // INCKSEL4[10:0]
	{ .regAddr= 0x311B, .regData= 0x00, .delayUs=0 }, // INCKSEL4[10:0]
	{ .regAddr= 0x311E, .regData= 0x24, .delayUs=0 }, // INCKSEL5[7:0]
	{ .regAddr= 0x32D4, .regData= 0x21, .delayUs=0 },
	{ .regAddr= 0x32EC, .regData= 0xA1, .delayUs=0 },
	/*
	{ .regAddr= 0x344C, .regData= 0x2B, .delayUs=0 },
	{ .regAddr= 0x344D, .regData= 0x01, .delayUs=0 },
	{ .regAddr= 0x344E, .regData= 0xED, .delayUs=0 },
	{ .regAddr= 0x344F, .regData= 0x01, .delayUs=0 },
	{ .regAddr= 0x3450, .regData= 0xF6, .delayUs=0 },
	{ .regAddr= 0x3451, .regData= 0x02, .delayUs=0 },
	*/
	{ .regAddr= 0x3452, .regData= 0x7F, .delayUs=0 },
	{ .regAddr= 0x3453, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x358A, .regData= 0x04, .delayUs=0 },
	{ .regAddr= 0x35A1, .regData= 0x02, .delayUs=0 },
	/*
	{ .regAddr= 0x35EC, .regData= 0x27, .delayUs=0 },
	{ .regAddr= 0x35EE, .regData= 0x8D, .delayUs=0 },
	{ .regAddr= 0x35F0, .regData= 0x8D, .delayUs=0 },
	{ .regAddr= 0x35F2, .regData= 0x29, .delayUs=0 },
	*/
	{ .regAddr= 0x36BC, .regData= 0x0C, .delayUs=0 },
	{ .regAddr= 0x36CC, .regData= 0x53, .delayUs=0 },
	{ .regAddr= 0x36CD, .regData= 0x00, .delayUs=0 },
	{ .regAddr= 0x36CE, .regData= 0x3C, .delayUs=0 },
	{ .regAddr= 0x36D0, .regData= 0x8C, .delayUs=0 },
	{ .regAddr= 0x36D1, .regData= 0x00, .delayUs=0 },
	{ .regAddr= 0x36D2, .regData= 0x71, .delayUs=0 },
	{ .regAddr= 0x36D4, .regData= 0x3C, .delayUs=0 },
	{ .regAddr= 0x36D6, .regData= 0x53, .delayUs=0 },
	{ .regAddr= 0x36D7, .regData= 0x00, .delayUs=0 },
	{ .regAddr= 0x36D8, .regData= 0x71, .delayUs=0 },
	{ .regAddr= 0x36DA, .regData= 0x8C, .delayUs=0 },
	{ .regAddr= 0x36DB, .regData= 0x00, .delayUs=0 },
	{ .regAddr= 0x3701, .regData= 0x00, .delayUs=0 },  //AD 0:10bit, 3:12bit
	//{ .regAddr= 0x3720, .regData= 0x00, .delayUs=0 },
	{ .regAddr= 0x3724, .regData= 0x02, .delayUs=0 },
	{ .regAddr= 0x3726, .regData= 0x02, .delayUs=0 },
	{ .regAddr= 0x3732, .regData= 0x02, .delayUs=0 },
	{ .regAddr= 0x3734, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x3736, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x3742, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x3862, .regData= 0xE0, .delayUs=0 },
	{ .regAddr= 0x38CC, .regData= 0x30, .delayUs=0 },
	{ .regAddr= 0x38CD, .regData= 0x2F, .delayUs=0 },
	{ .regAddr= 0x395C, .regData= 0x0C, .delayUs=0 },
	/*
	{ .regAddr= 0x39A4, .regData= 0x07, .delayUs=0 },
	{ .regAddr= 0x39A8, .regData= 0x32, .delayUs=0 },
	{ .regAddr= 0x39AA, .regData= 0x32, .delayUs=0 },
	{ .regAddr= 0x39AC, .regData= 0x32, .delayUs=0 },
	{ .regAddr= 0x39AE, .regData= 0x32, .delayUs=0 },
	{ .regAddr= 0x39B0, .regData= 0x32, .delayUs=0 },
	{ .regAddr= 0x39B2, .regData= 0x2F, .delayUs=0 },
	{ .regAddr= 0x39B4, .regData= 0x2D, .delayUs=0 },
	{ .regAddr= 0x39B6, .regData= 0x28, .delayUs=0 },
	{ .regAddr= 0x39B8, .regData= 0x30, .delayUs=0 },
	{ .regAddr= 0x39BA, .regData= 0x30, .delayUs=0 },
	{ .regAddr= 0x39BC, .regData= 0x30, .delayUs=0 },
	{ .regAddr= 0x39BE, .regData= 0x30, .delayUs=0 },
	{ .regAddr= 0x39C0, .regData= 0x30, .delayUs=0 },
	{ .regAddr= 0x39C2, .regData= 0x2E, .delayUs=0 },
	{ .regAddr= 0x39C4, .regData= 0x2B, .delayUs=0 },
	{ .regAddr= 0x39C6, .regData= 0x25, .delayUs=0 },
	*/
	{ .regAddr= 0x3A42, .regData= 0xD1, .delayUs=0 },
	{ .regAddr= 0x3A4C, .regData= 0x77, .delayUs=0 },
	{ .regAddr= 0x3AE0, .regData= 0x02, .delayUs=0 },
	{ .regAddr= 0x3AEC, .regData= 0x0C, .delayUs=0 },
	{ .regAddr= 0x3B00, .regData= 0x2E, .delayUs=0 },
	{ .regAddr= 0x3B06, .regData= 0x29, .delayUs=0 },
	{ .regAddr= 0x3B98, .regData= 0x25, .delayUs=0 },
	{ .regAddr= 0x3B99, .regData= 0x21, .delayUs=0 },
	{ .regAddr= 0x3B9B, .regData= 0x13, .delayUs=0 },
	{ .regAddr= 0x3B9C, .regData= 0x13, .delayUs=0 },
	{ .regAddr= 0x3B9D, .regData= 0x13, .delayUs=0 },
	{ .regAddr= 0x3B9E, .regData= 0x13, .delayUs=0 },
	{ .regAddr= 0x3BA1, .regData= 0x00, .delayUs=0 },
	{ .regAddr= 0x3BA2, .regData= 0x06, .delayUs=0 },
	{ .regAddr= 0x3BA3, .regData= 0x0B, .delayUs=0 },
	{ .regAddr= 0x3BA4, .regData= 0x10, .delayUs=0 },
	{ .regAddr= 0x3BA5, .regData= 0x14, .delayUs=0 },
	{ .regAddr= 0x3BA6, .regData= 0x18, .delayUs=0 },
	{ .regAddr= 0x3BA7, .regData= 0x1A, .delayUs=0 },
	{ .regAddr= 0x3BA8, .regData= 0x1A, .delayUs=0 },
	{ .regAddr= 0x3BA9, .regData= 0x1A, .delayUs=0 },
	{ .regAddr= 0x3BAC, .regData= 0xED, .delayUs=0 },
	{ .regAddr= 0x3BAD, .regData= 0x01, .delayUs=0 },
	{ .regAddr= 0x3BAE, .regData= 0xF6, .delayUs=0 },
	{ .regAddr= 0x3BAF, .regData= 0x02, .delayUs=0 },
	{ .regAddr= 0x3BB0, .regData= 0xA2, .delayUs=0 },
	{ .regAddr= 0x3BB1, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x3BB2, .regData= 0xE0, .delayUs=0 },
	{ .regAddr= 0x3BB3, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x3BB4, .regData= 0xE0, .delayUs=0 },
	{ .regAddr= 0x3BB5, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x3BB6, .regData= 0xE0, .delayUs=0 },
	{ .regAddr= 0x3BB7, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x3BB8, .regData= 0xE0, .delayUs=0 },
	{ .regAddr= 0x3BBA, .regData= 0xE0, .delayUs=0 },
	{ .regAddr= 0x3BBC, .regData= 0xDA, .delayUs=0 },
	{ .regAddr= 0x3BBE, .regData= 0x88, .delayUs=0 },
	{ .regAddr= 0x3BC0, .regData= 0x44, .delayUs=0 },
	{ .regAddr= 0x3BC2, .regData= 0x7B, .delayUs=0 },
	{ .regAddr= 0x3BC4, .regData= 0xA2, .delayUs=0 },
	{ .regAddr= 0x3BC8, .regData= 0xBD, .delayUs=0 },
	{ .regAddr= 0x3BCA, .regData= 0xBD, .delayUs=0 },
	{ .regAddr= 0x4001, .regData= 0x01, .delayUs=0 }, // LANEMODE[2:0]
	{ .regAddr= 0x4004, .regData= 0x48, .delayUs=0 }, // TXCLKESC_FREQ[15:0]
	{ .regAddr= 0x4005, .regData= 0x09, .delayUs=0 }, //
	{ .regAddr= 0x400C, .regData= 0x01, .delayUs=0 }, // INCKSEL6
	{ .regAddr= 0x4074, .regData= 0x00, .delayUs=0 }, // INCKSEL7 [2:0]
    { .regAddr = 0x3000, .regData = 0x00, .delayUs = 24000}, /* standby */
};

//CV2003-AAQR All-pixel scan CSI-2_4lane 37.125Mhz AD:10bit Output:12bit 1782Mbps Master Mode fps Integration Time xx.xxms Gain:6dB
static drv_sensor_reg_info_t  cv2003_5MP_23fps_dol2_setting[] =
{
	{ .regAddr= 0x3000, .regData= 0x01, .delayUs=0 }, /* enter standby */
	{ .regAddr= 0x3002, .regData= 0x01, .delayUs=0 }, /* master stop */
	{ .regAddr= 0x3008, .regData= 0x7F, .delayUs=0 }, // BCWAIT_TIME[9:0]
	{ .regAddr= 0x3009, .regData= 0x00, .delayUs=0 }, // BCWAIT_TIME[9:0]
	{ .regAddr= 0x300A, .regData= 0x5B, .delayUs=0 }, // CPWAIT_TIME[9:0]
	{ .regAddr= 0x300B, .regData= 0x00, .delayUs=0 }, // CPWAIT_TIME[9:0]
	{ .regAddr= 0x301c, .regData= 0x4,  .delayUs=0 },  //WIN MODE enable
	{ .regAddr= 0x3020, .regData= 0x0,  .delayUs=0 },  //horizontal 2 binning
	{ .regAddr= 0x3021, .regData= 0x0,  .delayUs=0 },  //veritical 2 binning
	{ .regAddr= 0x3022, .regData= 0x0,  .delayUs=0 },  //ADDMODE 2/2-line binning
	{ .regAddr= 0x3024, .regData= 0xca, .delayUs=0 }, // VMAX[19:0]
	{ .regAddr= 0x3025, .regData= 0x08, .delayUs=0 }, // VMAX[19:0]
	{ .regAddr= 0x3026, .regData= 0x00, .delayUs=0 }, // VMAX[19:0]
	{ .regAddr= 0x3028, .regData= 0x00, .delayUs=0 }, // HMAX[15:0]  44fps
	{ .regAddr= 0x3029, .regData= 0x03, .delayUs=0 }, // HMAX[15:0]
  { .regAddr= 0x3030, .regData= 0x00, .delayUs=0 },  //normal
	{ .regAddr= 0x3031, .regData= 0x01, .delayUs=0 }, // AD:10 bit
	{ .regAddr= 0x3032, .regData= 0x01, .delayUs=0 }, // MD:12 bit
	{ .regAddr= 0x3033, .regData= 0x04, .delayUs=0 }, // SYS_MODE[3:0]
/*crop mode*/
	{ .regAddr= 0x3040, .regData= 0x00, .delayUs=0 }, // PIX_HST[7:0]
	{ .regAddr= 0x3041, .regData= 0x00, .delayUs=0 }, // PIX_HST[11:8]
	{ .regAddr= 0x3044, .regData= 0x00, .delayUs=0 }, // PIX_VST[7:0]
	{ .regAddr= 0x3045, .regData= 0x00, .delayUs=0 }, // PIX_VST[11:8]
/*width 2688x1860*/
	{ .regAddr= 0x3042, .regData= 0x80, .delayUs=0 }, // PIX_HWID[7:0]
	{ .regAddr= 0x3043, .regData= 0x0A, .delayUs=0 }, // PIX_HWID[11:8]
	{ .regAddr= 0x3046, .regData= 0xA8, .delayUs=0 }, // PIX_VWID[7:0]
	{ .regAddr= 0x3047, .regData= 0x0E, .delayUs=0 }, // PIX_VWID[11:8]
	{ .regAddr= 0x3023, .regData= 0x0,  .delayUs=0 }, // VCMODE
	{ .regAddr= 0x302c, .regData= 0x01, .delayUs=0 }, // WDMODE[1:0]
	{ .regAddr= 0x302d, .regData= 0x01, .delayUs=0 }, // WDSEL[1:0]
	{ .regAddr= 0x3050, .regData= 0x26, .delayUs=0 }, // SHR0[19:0]
	{ .regAddr= 0x3051, .regData= 0x0F, .delayUs=0 }, // SHR0[19:0]
	{ .regAddr= 0x3052, .regData= 0x00, .delayUs=0 }, // SHR0[19:0]
	{ .regAddr= 0x3054, .regData= 0x09, .delayUs=0 }, // SHR1[19:0]
	{ .regAddr= 0x3060, .regData= 0x4d, .delayUs=0 }, // RHS1[19:0]
	{ .regAddr= 0x3090, .regData= 0x00, .delayUs=0 }, // GAIN_PCG_0[8:0]
	{ .regAddr= 0x3091, .regData= 0x00, .delayUs=0 }, // GAIN_PCG_0[8:0]
	{ .regAddr= 0x30C1, .regData= 0x00, .delayUs=0 }, // XVS_DRV[1:0]
	{ .regAddr= 0x30CF, .regData= 0x01, .delayUs=0 }, // XVS_DRV[1:0]
	{ .regAddr= 0x3115, .regData= 0x00, .delayUs=0 }, // INCKSEL1[7:0]
	{ .regAddr= 0x3116, .regData= 0x24, .delayUs=0 }, // INCKSEL2[7:0]
	{ .regAddr= 0x3118, .regData= 0xC0, .delayUs=0 }, // INCKSEL3[10:0]
	{ .regAddr= 0x3119, .regData= 0x00, .delayUs=0 }, // INCKSEL3[10:0]
	{ .regAddr= 0x311A, .regData= 0xE0, .delayUs=0 }, // INCKSEL4[10:0]
	{ .regAddr= 0x311B, .regData= 0x00, .delayUs=0 }, // INCKSEL4[10:0]
	{ .regAddr= 0x311E, .regData= 0x24, .delayUs=0 }, // INCKSEL5[7:0]
	{ .regAddr= 0x32D4, .regData= 0x21, .delayUs=0 },
	{ .regAddr= 0x32EC, .regData= 0xA1, .delayUs=0 },
	/*
	{ .regAddr= 0x344C, .regData= 0x2B, .delayUs=0 },
	{ .regAddr= 0x344D, .regData= 0x01, .delayUs=0 },
	{ .regAddr= 0x344E, .regData= 0xED, .delayUs=0 },
	{ .regAddr= 0x344F, .regData= 0x01, .delayUs=0 },
	{ .regAddr= 0x3450, .regData= 0xF6, .delayUs=0 },
	{ .regAddr= 0x3451, .regData= 0x02, .delayUs=0 },
	*/
	{ .regAddr= 0x3452, .regData= 0x7F, .delayUs=0 },
	{ .regAddr= 0x3453, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x358A, .regData= 0x04, .delayUs=0 },
	{ .regAddr= 0x35A1, .regData= 0x02, .delayUs=0 },
	/*
	{ .regAddr= 0x35EC, .regData= 0x27, .delayUs=0 },
	{ .regAddr= 0x35EE, .regData= 0x8D, .delayUs=0 },
	{ .regAddr= 0x35F0, .regData= 0x8D, .delayUs=0 },
	{ .regAddr= 0x35F2, .regData= 0x29, .delayUs=0 },
	*/
	{ .regAddr= 0x36BC, .regData= 0x0C, .delayUs=0 },
	{ .regAddr= 0x36CC, .regData= 0x53, .delayUs=0 },
	{ .regAddr= 0x36CD, .regData= 0x00, .delayUs=0 },
	{ .regAddr= 0x36CE, .regData= 0x3C, .delayUs=0 },
	{ .regAddr= 0x36D0, .regData= 0x8C, .delayUs=0 },
	{ .regAddr= 0x36D1, .regData= 0x00, .delayUs=0 },
	{ .regAddr= 0x36D2, .regData= 0x71, .delayUs=0 },
	{ .regAddr= 0x36D4, .regData= 0x3C, .delayUs=0 },
	{ .regAddr= 0x36D6, .regData= 0x53, .delayUs=0 },
	{ .regAddr= 0x36D7, .regData= 0x00, .delayUs=0 },
	{ .regAddr= 0x36D8, .regData= 0x71, .delayUs=0 },
	{ .regAddr= 0x36DA, .regData= 0x8C, .delayUs=0 },
	{ .regAddr= 0x36DB, .regData= 0x00, .delayUs=0 },
	{ .regAddr= 0x3701, .regData= 0x03, .delayUs=0 },  //AD 0:10bit, 3:12bit
	//{ .regAddr= 0x3720, .regData= 0x00, .delayUs=0 },
	{ .regAddr= 0x3724, .regData= 0x02, .delayUs=0 },
	{ .regAddr= 0x3726, .regData= 0x02, .delayUs=0 },
	{ .regAddr= 0x3732, .regData= 0x02, .delayUs=0 },
	{ .regAddr= 0x3734, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x3736, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x3742, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x3862, .regData= 0xE0, .delayUs=0 },
	{ .regAddr= 0x38CC, .regData= 0x30, .delayUs=0 },
	{ .regAddr= 0x38CD, .regData= 0x2F, .delayUs=0 },
	{ .regAddr= 0x395C, .regData= 0x0C, .delayUs=0 },
	/*
	{ .regAddr= 0x39A4, .regData= 0x07, .delayUs=0 },
	{ .regAddr= 0x39A8, .regData= 0x32, .delayUs=0 },
	{ .regAddr= 0x39AA, .regData= 0x32, .delayUs=0 },
	{ .regAddr= 0x39AC, .regData= 0x32, .delayUs=0 },
	{ .regAddr= 0x39AE, .regData= 0x32, .delayUs=0 },
	{ .regAddr= 0x39B0, .regData= 0x32, .delayUs=0 },
	{ .regAddr= 0x39B2, .regData= 0x2F, .delayUs=0 },
	{ .regAddr= 0x39B4, .regData= 0x2D, .delayUs=0 },
	{ .regAddr= 0x39B6, .regData= 0x28, .delayUs=0 },
	{ .regAddr= 0x39B8, .regData= 0x30, .delayUs=0 },
	{ .regAddr= 0x39BA, .regData= 0x30, .delayUs=0 },
	{ .regAddr= 0x39BC, .regData= 0x30, .delayUs=0 },
	{ .regAddr= 0x39BE, .regData= 0x30, .delayUs=0 },
	{ .regAddr= 0x39C0, .regData= 0x30, .delayUs=0 },
	{ .regAddr= 0x39C2, .regData= 0x2E, .delayUs=0 },
	{ .regAddr= 0x39C4, .regData= 0x2B, .delayUs=0 },
	{ .regAddr= 0x39C6, .regData= 0x25, .delayUs=0 },
	*/
	{ .regAddr= 0x3A42, .regData= 0xD1, .delayUs=0 },
	{ .regAddr= 0x3A4C, .regData= 0x77, .delayUs=0 },
	{ .regAddr= 0x3AE0, .regData= 0x02, .delayUs=0 },
	{ .regAddr= 0x3AEC, .regData= 0x0C, .delayUs=0 },
	{ .regAddr= 0x3B00, .regData= 0x2E, .delayUs=0 },
	{ .regAddr= 0x3B06, .regData= 0x29, .delayUs=0 },
	{ .regAddr= 0x3B98, .regData= 0x25, .delayUs=0 },
	{ .regAddr= 0x3B99, .regData= 0x21, .delayUs=0 },
	{ .regAddr= 0x3B9B, .regData= 0x13, .delayUs=0 },
	{ .regAddr= 0x3B9C, .regData= 0x13, .delayUs=0 },
	{ .regAddr= 0x3B9D, .regData= 0x13, .delayUs=0 },
	{ .regAddr= 0x3B9E, .regData= 0x13, .delayUs=0 },
	{ .regAddr= 0x3BA1, .regData= 0x00, .delayUs=0 },
	{ .regAddr= 0x3BA2, .regData= 0x06, .delayUs=0 },
	{ .regAddr= 0x3BA3, .regData= 0x0B, .delayUs=0 },
	{ .regAddr= 0x3BA4, .regData= 0x10, .delayUs=0 },
	{ .regAddr= 0x3BA5, .regData= 0x14, .delayUs=0 },
	{ .regAddr= 0x3BA6, .regData= 0x18, .delayUs=0 },
	{ .regAddr= 0x3BA7, .regData= 0x1A, .delayUs=0 },
	{ .regAddr= 0x3BA8, .regData= 0x1A, .delayUs=0 },
	{ .regAddr= 0x3BA9, .regData= 0x1A, .delayUs=0 },
	{ .regAddr= 0x3BAC, .regData= 0xED, .delayUs=0 },
	{ .regAddr= 0x3BAD, .regData= 0x01, .delayUs=0 },
	{ .regAddr= 0x3BAE, .regData= 0xF6, .delayUs=0 },
	{ .regAddr= 0x3BAF, .regData= 0x02, .delayUs=0 },
	{ .regAddr= 0x3BB0, .regData= 0xA2, .delayUs=0 },
	{ .regAddr= 0x3BB1, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x3BB2, .regData= 0xE0, .delayUs=0 },
	{ .regAddr= 0x3BB3, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x3BB4, .regData= 0xE0, .delayUs=0 },
	{ .regAddr= 0x3BB5, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x3BB6, .regData= 0xE0, .delayUs=0 },
	{ .regAddr= 0x3BB7, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x3BB8, .regData= 0xE0, .delayUs=0 },
	{ .regAddr= 0x3BBA, .regData= 0xE0, .delayUs=0 },
	{ .regAddr= 0x3BBC, .regData= 0xDA, .delayUs=0 },
	{ .regAddr= 0x3BBE, .regData= 0x88, .delayUs=0 },
	{ .regAddr= 0x3BC0, .regData= 0x44, .delayUs=0 },
	{ .regAddr= 0x3BC2, .regData= 0x7B, .delayUs=0 },
	{ .regAddr= 0x3BC4, .regData= 0xA2, .delayUs=0 },
	{ .regAddr= 0x3BC8, .regData= 0xBD, .delayUs=0 },
	{ .regAddr= 0x3BCA, .regData= 0xBD, .delayUs=0 },
	{ .regAddr= 0x4001, .regData= 0x03, .delayUs=0 }, // LANEMODE[2:0]
	{ .regAddr= 0x4004, .regData= 0x48, .delayUs=0 }, // TXCLKESC_FREQ[15:0]
	{ .regAddr= 0x4005, .regData= 0x09, .delayUs=0 }, //
	{ .regAddr= 0x400C, .regData= 0x01, .delayUs=0 }, // INCKSEL6
	{ .regAddr= 0x4018, .regData= 0xb7, .delayUs=0 }, // TCLKPOST[15:0]
	{ .regAddr= 0x4019, .regData= 0x00, .delayUs=0 }, // TCLKPOST[15:0]
	{ .regAddr= 0x401A, .regData= 0x67, .delayUs=0 }, // TCLKPREPARE[15:0]
	{ .regAddr= 0x401B, .regData= 0x00, .delayUs=0 }, // TCLKPREPARE[15:0]
	{ .regAddr= 0x401C, .regData= 0x6f, .delayUs=0 }, // TCLKTRAI[15:0]
	{ .regAddr= 0x401D, .regData= 0x00, .delayUs=0 }, // TCLKTRAI[15:0]
	{ .regAddr= 0x401E, .regData= 0xdf, .delayUs=0 }, // TCLKZERO[15:0]
	{ .regAddr= 0x401F, .regData= 0x01, .delayUs=0 }, // TCLKZERO[15:0]
	{ .regAddr= 0x4020, .regData= 0x6F, .delayUs=0 }, // THSPREPARE[15:0]
	{ .regAddr= 0x4021, .regData= 0x00, .delayUs=0 }, // THSPREPARE[15:0]
	{ .regAddr= 0x4022, .regData= 0xcf, .delayUs=0 }, // THSZERO[15:0]
	{ .regAddr= 0x4023, .regData= 0x00, .delayUs=0 }, // THSZERO[15:0]
	{ .regAddr= 0x4024, .regData= 0x6F, .delayUs=0 }, // THSTRAI[15:0]
	{ .regAddr= 0x4025, .regData= 0x00, .delayUs=0 }, // THSTRAI[15:0]
	{ .regAddr= 0x4026, .regData= 0xb7, .delayUs=0 }, // THSEXIT[15:0]
	{ .regAddr= 0x4027, .regData= 0x00, .delayUs=0 }, // THSEXIT[15:0]
	{ .regAddr= 0x4028, .regData= 0x5f, .delayUs=0 }, // TLPX[15:0]
	{ .regAddr= 0x4029, .regData= 0x00, .delayUs=0 }, // TLPX[15:0]
	{ .regAddr= 0x4074, .regData= 0x00, .delayUs=0 }, // INCKSEL7 [2:0]
  { .regAddr= 0x3000, .regData = 0x00, .delayUs = 24000}, /* leave standby */
};

  static drv_sensor_reg_info_t  cv2003_4MP_20fps_dol2_setting[] =
{
	{ .regAddr= 0x3000, .regData= 0x01, .delayUs=0 }, /* enter standby */
	{ .regAddr= 0x3002, .regData= 0x01, .delayUs=0 }, /* master stop */
	{ .regAddr= 0x3008, .regData= 0x7F, .delayUs=0 }, // BCWAIT_TIME[9:0]
	{ .regAddr= 0x3009, .regData= 0x00, .delayUs=0 }, // BCWAIT_TIME[9:0]
	{ .regAddr= 0x300A, .regData= 0x5B, .delayUs=0 }, // CPWAIT_TIME[9:0]
	{ .regAddr= 0x300B, .regData= 0x00, .delayUs=0 }, // CPWAIT_TIME[9:0]
	{ .regAddr= 0x301c, .regData= 0x4, .delayUs=0 },  //WIN MODE enable
	{ .regAddr= 0x3020, .regData= 0x0, .delayUs=0 },  //horizontal 2 binning
	{ .regAddr= 0x3021, .regData= 0x0, .delayUs=0 },  //veritical 2 binning
	{ .regAddr= 0x3022, .regData= 0x0, .delayUs=0 },  //ADDMODE 2/2-line binning
	{ .regAddr= 0x3024, .regData= 0xCA, .delayUs=0 }, // VMAX[19:0]
	{ .regAddr= 0x3025, .regData= 0x08, .delayUs=0 }, // VMAX[19:0]
	{ .regAddr= 0x3026, .regData= 0x00, .delayUs=0 }, // VMAX[19:0]

	//{ .regAddr= 0x3028, .regData= 0x26, .delayUs=0 }, // HMAX[15:0]
	//{ .regAddr= 0x3029, .regData= 0x02, .delayUs=0 }, // HMAX[15:0]
 	{ .regAddr= 0x3028, .regData= 0xe0, .delayUs=0 }, // HMAX[15:0]  44fps
	{ .regAddr= 0x3029, .regData= 0x02, .delayUs=0 }, // HMAX[15:0]

//{ .regAddr= 0x3028, .regData= 0x00, .delayUs=0 }, // HMAX[15:0]  44fps
//{ .regAddr= 0x3029, .regData= 0x03, .delayUs=0 }, // HMAX[15:0]
    { .regAddr= 0x3030, .regData= 0x00, .delayUs=0 },  //normal
	{ .regAddr= 0x3031, .regData= 0x00, .delayUs=0 }, // AD:10 bit
	{ .regAddr= 0x3032, .regData= 0x01, .delayUs=0 }, // MD:12 bit
	{ .regAddr= 0x3033, .regData= 0x04, .delayUs=0 }, // SYS_MODE[3:0]
	/*crop mode*/
	{ .regAddr= 0x3040, .regData= 0x00, .delayUs=0 }, // PIX_HST[7:0]
	{ .regAddr= 0x3041, .regData= 0x00, .delayUs=0 }, // PIX_HST[11:8]
	{ .regAddr= 0x3044, .regData= 0x00, .delayUs=0 }, // PIX_VST[7:0]
	{ .regAddr= 0x3045, .regData= 0x00, .delayUs=0 }, // PIX_VST[11:8]
	/*width 2592x1536*/
	{ .regAddr= 0x3042, .regData= 0x20, .delayUs=0 }, // PIX_HWID[7:0]
	{ .regAddr= 0x3043, .regData= 0x0A, .delayUs=0 }, // PIX_HWID[11:8]
	{ .regAddr= 0x3046, .regData= 0x20, .delayUs=0 }, // PIX_VWID[7:0]
	{ .regAddr= 0x3047, .regData= 0x0C, .delayUs=0 }, // PIX_VWID[11:8]
	{ .regAddr= 0x3023, .regData= 0x0, .delayUs=0 },  // VCMODE
	{ .regAddr= 0x302c, .regData= 0x01, .delayUs=0 }, // WDMODE[1:0]
	{ .regAddr= 0x302d, .regData= 0x01, .delayUs=0 }, // WDSEL[1:0]
	{ .regAddr= 0x3050, .regData= 0x26, .delayUs=0 }, // SHR0[19:0]
	{ .regAddr= 0x3051, .regData= 0x0F, .delayUs=0 }, // SHR0[19:0]
	{ .regAddr= 0x3052, .regData= 0x00, .delayUs=0 }, // SHR0[19:0]
	{ .regAddr= 0x3054, .regData= 0x09, .delayUs=0 }, // SHR1[19:0]
	{ .regAddr= 0x3060, .regData= 0x4d, .delayUs=0 }, // RHS1[19:0]
	{ .regAddr= 0x3090, .regData= 0x00, .delayUs=0 }, // GAIN_PCG_0[8:0]
	{ .regAddr= 0x3091, .regData= 0x00, .delayUs=0 }, // GAIN_PCG_0[8:0]
	{ .regAddr= 0x30C1, .regData= 0x00, .delayUs=0 }, // XVS_DRV[1:0]
	{ .regAddr= 0x30CF, .regData= 0x01, .delayUs=0 }, // XVS_DRV[1:0]
	{ .regAddr= 0x3115, .regData= 0x00, .delayUs=0 }, // INCKSEL1[7:0]
	{ .regAddr= 0x3116, .regData= 0x24, .delayUs=0 }, // INCKSEL2[7:0]
	{ .regAddr= 0x3118, .regData= 0xC0, .delayUs=0 }, // INCKSEL3[10:0]
	{ .regAddr= 0x3119, .regData= 0x00, .delayUs=0 }, // INCKSEL3[10:0]
	{ .regAddr= 0x311A, .regData= 0xE0, .delayUs=0 }, // INCKSEL4[10:0]
	{ .regAddr= 0x311B, .regData= 0x00, .delayUs=0 }, // INCKSEL4[10:0]
	{ .regAddr= 0x311E, .regData= 0x24, .delayUs=0 }, // INCKSEL5[7:0]
	{ .regAddr= 0x32D4, .regData= 0x21, .delayUs=0 },
	{ .regAddr= 0x32EC, .regData= 0xA1, .delayUs=0 },
	/*
	{ .regAddr= 0x344C, .regData= 0x2B, .delayUs=0 },
	{ .regAddr= 0x344D, .regData= 0x01, .delayUs=0 },
	{ .regAddr= 0x344E, .regData= 0xED, .delayUs=0 },
	{ .regAddr= 0x344F, .regData= 0x01, .delayUs=0 },
	{ .regAddr= 0x3450, .regData= 0xF6, .delayUs=0 },
	{ .regAddr= 0x3451, .regData= 0x02, .delayUs=0 },
	*/
	{ .regAddr= 0x3452, .regData= 0x7F, .delayUs=0 },
	{ .regAddr= 0x3453, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x358A, .regData= 0x04, .delayUs=0 },
	{ .regAddr= 0x35A1, .regData= 0x02, .delayUs=0 },
	/*
	{ .regAddr= 0x35EC, .regData= 0x27, .delayUs=0 },
	{ .regAddr= 0x35EE, .regData= 0x8D, .delayUs=0 },
	{ .regAddr= 0x35F0, .regData= 0x8D, .delayUs=0 },
	{ .regAddr= 0x35F2, .regData= 0x29, .delayUs=0 },
	*/
	{ .regAddr= 0x36BC, .regData= 0x0C, .delayUs=0 },
	{ .regAddr= 0x36CC, .regData= 0x53, .delayUs=0 },
	{ .regAddr= 0x36CD, .regData= 0x00, .delayUs=0 },
	{ .regAddr= 0x36CE, .regData= 0x3C, .delayUs=0 },
	{ .regAddr= 0x36D0, .regData= 0x8C, .delayUs=0 },
	{ .regAddr= 0x36D1, .regData= 0x00, .delayUs=0 },
	{ .regAddr= 0x36D2, .regData= 0x71, .delayUs=0 },
	{ .regAddr= 0x36D4, .regData= 0x3C, .delayUs=0 },
	{ .regAddr= 0x36D6, .regData= 0x53, .delayUs=0 },
	{ .regAddr= 0x36D7, .regData= 0x00, .delayUs=0 },
	{ .regAddr= 0x36D8, .regData= 0x71, .delayUs=0 },
	{ .regAddr= 0x36DA, .regData= 0x8C, .delayUs=0 },
	{ .regAddr= 0x36DB, .regData= 0x00, .delayUs=0 },
	{ .regAddr= 0x3701, .regData= 0x00, .delayUs=0 },  //AD 0:10bit, 3:12bit
	//{ .regAddr= 0x3720, .regData= 0x00, .delayUs=0 },
	{ .regAddr= 0x3724, .regData= 0x02, .delayUs=0 },
	{ .regAddr= 0x3726, .regData= 0x02, .delayUs=0 },
	{ .regAddr= 0x3732, .regData= 0x02, .delayUs=0 },
	{ .regAddr= 0x3734, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x3736, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x3742, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x3862, .regData= 0xE0, .delayUs=0 },
	{ .regAddr= 0x38CC, .regData= 0x30, .delayUs=0 },
	{ .regAddr= 0x38CD, .regData= 0x2F, .delayUs=0 },
	{ .regAddr= 0x395C, .regData= 0x0C, .delayUs=0 },
	/*
	{ .regAddr= 0x39A4, .regData= 0x07, .delayUs=0 },
	{ .regAddr= 0x39A8, .regData= 0x32, .delayUs=0 },
	{ .regAddr= 0x39AA, .regData= 0x32, .delayUs=0 },
	{ .regAddr= 0x39AC, .regData= 0x32, .delayUs=0 },
	{ .regAddr= 0x39AE, .regData= 0x32, .delayUs=0 },
	{ .regAddr= 0x39B0, .regData= 0x32, .delayUs=0 },
	{ .regAddr= 0x39B2, .regData= 0x2F, .delayUs=0 },
	{ .regAddr= 0x39B4, .regData= 0x2D, .delayUs=0 },
	{ .regAddr= 0x39B6, .regData= 0x28, .delayUs=0 },
	{ .regAddr= 0x39B8, .regData= 0x30, .delayUs=0 },
	{ .regAddr= 0x39BA, .regData= 0x30, .delayUs=0 },
	{ .regAddr= 0x39BC, .regData= 0x30, .delayUs=0 },
	{ .regAddr= 0x39BE, .regData= 0x30, .delayUs=0 },
	{ .regAddr= 0x39C0, .regData= 0x30, .delayUs=0 },
	{ .regAddr= 0x39C2, .regData= 0x2E, .delayUs=0 },
	{ .regAddr= 0x39C4, .regData= 0x2B, .delayUs=0 },
	{ .regAddr= 0x39C6, .regData= 0x25, .delayUs=0 },
	*/
	{ .regAddr= 0x3A42, .regData= 0xD1, .delayUs=0 },
	{ .regAddr= 0x3A4C, .regData= 0x77, .delayUs=0 },
	{ .regAddr= 0x3AE0, .regData= 0x02, .delayUs=0 },
	{ .regAddr= 0x3AEC, .regData= 0x0C, .delayUs=0 },
	{ .regAddr= 0x3B00, .regData= 0x2E, .delayUs=0 },
	{ .regAddr= 0x3B06, .regData= 0x29, .delayUs=0 },
	{ .regAddr= 0x3B98, .regData= 0x25, .delayUs=0 },
	{ .regAddr= 0x3B99, .regData= 0x21, .delayUs=0 },
	{ .regAddr= 0x3B9B, .regData= 0x13, .delayUs=0 },
	{ .regAddr= 0x3B9C, .regData= 0x13, .delayUs=0 },
	{ .regAddr= 0x3B9D, .regData= 0x13, .delayUs=0 },
	{ .regAddr= 0x3B9E, .regData= 0x13, .delayUs=0 },
	{ .regAddr= 0x3BA1, .regData= 0x00, .delayUs=0 },
	{ .regAddr= 0x3BA2, .regData= 0x06, .delayUs=0 },
	{ .regAddr= 0x3BA3, .regData= 0x0B, .delayUs=0 },
	{ .regAddr= 0x3BA4, .regData= 0x10, .delayUs=0 },
	{ .regAddr= 0x3BA5, .regData= 0x14, .delayUs=0 },
	{ .regAddr= 0x3BA6, .regData= 0x18, .delayUs=0 },
	{ .regAddr= 0x3BA7, .regData= 0x1A, .delayUs=0 },
	{ .regAddr= 0x3BA8, .regData= 0x1A, .delayUs=0 },
	{ .regAddr= 0x3BA9, .regData= 0x1A, .delayUs=0 },
	{ .regAddr= 0x3BAC, .regData= 0xED, .delayUs=0 },
	{ .regAddr= 0x3BAD, .regData= 0x01, .delayUs=0 },
	{ .regAddr= 0x3BAE, .regData= 0xF6, .delayUs=0 },
	{ .regAddr= 0x3BAF, .regData= 0x02, .delayUs=0 },
	{ .regAddr= 0x3BB0, .regData= 0xA2, .delayUs=0 },
	{ .regAddr= 0x3BB1, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x3BB2, .regData= 0xE0, .delayUs=0 },
	{ .regAddr= 0x3BB3, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x3BB4, .regData= 0xE0, .delayUs=0 },
	{ .regAddr= 0x3BB5, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x3BB6, .regData= 0xE0, .delayUs=0 },
	{ .regAddr= 0x3BB7, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x3BB8, .regData= 0xE0, .delayUs=0 },
	{ .regAddr= 0x3BBA, .regData= 0xE0, .delayUs=0 },
	{ .regAddr= 0x3BBC, .regData= 0xDA, .delayUs=0 },
	{ .regAddr= 0x3BBE, .regData= 0x88, .delayUs=0 },
	{ .regAddr= 0x3BC0, .regData= 0x44, .delayUs=0 },
	{ .regAddr= 0x3BC2, .regData= 0x7B, .delayUs=0 },
	{ .regAddr= 0x3BC4, .regData= 0xA2, .delayUs=0 },
	{ .regAddr= 0x3BC8, .regData= 0xBD, .delayUs=0 },
	{ .regAddr= 0x3BCA, .regData= 0xBD, .delayUs=0 },

	{ .regAddr= 0x4001, .regData= 0x03, .delayUs=0 }, // LANEMODE[2:0]
	{ .regAddr= 0x4004, .regData= 0x48, .delayUs=0 }, // TXCLKESC_FREQ[15:0]
	{ .regAddr= 0x4005, .regData= 0x09, .delayUs=0 }, //
	{ .regAddr= 0x400C, .regData= 0x01, .delayUs=0 }, // INCKSEL6

	{ .regAddr= 0x4018, .regData= 0xb7, .delayUs=0 }, // TCLKPOST[15:0]
	{ .regAddr= 0x4019, .regData= 0x00, .delayUs=0 }, // TCLKPOST[15:0]
	{ .regAddr= 0x401A, .regData= 0x67, .delayUs=0 }, // TCLKPREPARE[15:0]
	{ .regAddr= 0x401B, .regData= 0x00, .delayUs=0 }, // TCLKPREPARE[15:0]
	{ .regAddr= 0x401C, .regData= 0x6f, .delayUs=0 }, // TCLKTRAI[15:0]
	{ .regAddr= 0x401D, .regData= 0x00, .delayUs=0 }, // TCLKTRAI[15:0]
	{ .regAddr= 0x401E, .regData= 0xdf, .delayUs=0 }, // TCLKZERO[15:0]
	{ .regAddr= 0x401F, .regData= 0x01, .delayUs=0 }, // TCLKZERO[15:0]
	{ .regAddr= 0x4020, .regData= 0x6F, .delayUs=0 }, // THSPREPARE[15:0]
	{ .regAddr= 0x4021, .regData= 0x00, .delayUs=0 }, // THSPREPARE[15:0]
	{ .regAddr= 0x4022, .regData= 0xcf, .delayUs=0 }, // THSZERO[15:0]
	{ .regAddr= 0x4023, .regData= 0x00, .delayUs=0 }, // THSZERO[15:0]
	{ .regAddr= 0x4024, .regData= 0x6F, .delayUs=0 }, // THSTRAI[15:0]
	{ .regAddr= 0x4025, .regData= 0x00, .delayUs=0 }, // THSTRAI[15:0]
	{ .regAddr= 0x4026, .regData= 0xb7, .delayUs=0 }, // THSEXIT[15:0]
	{ .regAddr= 0x4027, .regData= 0x00, .delayUs=0 }, // THSEXIT[15:0]
	{ .regAddr= 0x4028, .regData= 0x5f, .delayUs=0 }, // TLPX[15:0]
	{ .regAddr= 0x4029, .regData= 0x00, .delayUs=0 }, // TLPX[15:0]
	{ .regAddr= 0x4074, .regData= 0x00, .delayUs=0 }, // INCKSEL7 [2:0]
    { .regAddr = 0x3000, .regData = 0x00, .delayUs = 24000}, /* leave standby */
};

  static drv_sensor_reg_info_t  cv2003_4MP_30fps_dol2_setting[] =
{
	{ .regAddr= 0x3000, .regData= 0x01, .delayUs=0 }, /* enter standby */
	{ .regAddr= 0x3002, .regData= 0x01, .delayUs=0 }, /* master stop */
	{ .regAddr= 0x3008, .regData= 0x7F, .delayUs=0 }, // BCWAIT_TIME[9:0]
	{ .regAddr= 0x3009, .regData= 0x00, .delayUs=0 }, // BCWAIT_TIME[9:0]
	{ .regAddr= 0x300A, .regData= 0x5B, .delayUs=0 }, // CPWAIT_TIME[9:0]
	{ .regAddr= 0x300B, .regData= 0x00, .delayUs=0 }, // CPWAIT_TIME[9:0]
	{ .regAddr= 0x301c, .regData= 0x4, .delayUs=0 },  //WIN MODE enable
	{ .regAddr= 0x3020, .regData= 0x0, .delayUs=0 },  //horizontal 2 binning
	{ .regAddr= 0x3021, .regData= 0x0, .delayUs=0 },  //veritical 2 binning
	{ .regAddr= 0x3022, .regData= 0x0, .delayUs=0 },  //ADDMODE 2/2-line binning
	{ .regAddr= 0x3024, .regData= 0xBC, .delayUs=0 }, // VMAX[19:0]
	{ .regAddr= 0x3025, .regData= 0x05, .delayUs=0 }, // VMAX[19:0]
	{ .regAddr= 0x3026, .regData= 0x00, .delayUs=0 }, // VMAX[19:0]

	//{ .regAddr= 0x3028, .regData= 0x26, .delayUs=0 }, // HMAX[15:0]
	//{ .regAddr= 0x3029, .regData= 0x02, .delayUs=0 }, // HMAX[15:0]
 	{ .regAddr= 0x3028, .regData= 0x4c, .delayUs=0 }, // HMAX[15:0]  44fps
	{ .regAddr= 0x3029, .regData= 0x04, .delayUs=0 }, // HMAX[15:0]

//{ .regAddr= 0x3028, .regData= 0x00, .delayUs=0 }, // HMAX[15:0]  44fps
//{ .regAddr= 0x3029, .regData= 0x03, .delayUs=0 }, // HMAX[15:0]
    { .regAddr= 0x3030, .regData= 0x00, .delayUs=0 },  //normal
	{ .regAddr= 0x3031, .regData= 0x00, .delayUs=0 }, // AD:10 bit
	{ .regAddr= 0x3032, .regData= 0x01, .delayUs=0 }, // MD:12 bit
	{ .regAddr= 0x3033, .regData= 0x04, .delayUs=0 }, // SYS_MODE[3:0]

	/*crop mode*/
	{ .regAddr= 0x3040, .regData= 0x00, .delayUs=0 }, // PIX_HST[7:0]
	{ .regAddr= 0x3041, .regData= 0x00, .delayUs=0 }, // PIX_HST[11:8]
	{ .regAddr= 0x3044, .regData= 0x00, .delayUs=0 }, // PIX_VST[7:0]
	{ .regAddr= 0x3045, .regData= 0x00, .delayUs=0 }, // PIX_VST[11:8]
	/*width 2592x1536*/
	{ .regAddr= 0x3042, .regData= 0x20, .delayUs=0 }, // PIX_HWID[7:0]
	{ .regAddr= 0x3043, .regData= 0x0A, .delayUs=0 }, // PIX_HWID[11:8]
	{ .regAddr= 0x3046, .regData= 0x20, .delayUs=0 }, // PIX_VWID[7:0]
	{ .regAddr= 0x3047, .regData= 0x0C, .delayUs=0 }, // PIX_VWID[11:8]

	{ .regAddr= 0x3023, .regData= 0x0, .delayUs=0 },  // VCMODE
	{ .regAddr= 0x302c, .regData= 0x01, .delayUs=0 }, // WDMODE[1:0]
	{ .regAddr= 0x302d, .regData= 0x01, .delayUs=0 }, // WDSEL[1:0]
	{ .regAddr= 0x3050, .regData= 0x26, .delayUs=0 }, // SHR0[19:0]
	{ .regAddr= 0x3051, .regData= 0x0F, .delayUs=0 }, // SHR0[19:0]
	{ .regAddr= 0x3052, .regData= 0x00, .delayUs=0 }, // SHR0[19:0]
	//{ .regAddr= 0x3050, .regData= 0x66, .delayUs=0 }, // SHR0[19:0]
	//{ .regAddr= 0x3051, .regData= 0x00, .delayUs=0 }, //
	//{ .regAddr= 0x3052, .regData= 0x00, .delayUs=0 }, //
	{ .regAddr= 0x3054, .regData= 0x09, .delayUs=0 }, // SHR1[19:0]
	{ .regAddr= 0x3060, .regData= 0x4d, .delayUs=0 }, // RHS1[19:0]
	//{ .regAddr= 0x3090, .regData= 0x14, .delayUs=0 }, // GAIN_PCG_0[8:0]
	//{ .regAddr= 0x3091, .regData= 0x00, .delayUs=0 }, // GAIN_PCG_0[8:0]
	{ .regAddr= 0x3090, .regData= 0x00, .delayUs=0 }, // GAIN_PCG_0[8:0]
	{ .regAddr= 0x3091, .regData= 0x00, .delayUs=0 }, // GAIN_PCG_0[8:0]
	{ .regAddr= 0x30C1, .regData= 0x00, .delayUs=0 }, // XVS_DRV[1:0]
	{ .regAddr= 0x30CF, .regData= 0x01, .delayUs=0 }, // XVS_DRV[1:0]

	{ .regAddr= 0x30D9, .regData= 0x02, .delayUs=0 }, // DIG_CLP_VSTAET[4:0]
	{ .regAddr= 0x30DA, .regData= 0x01, .delayUs=0 }, // DIG_VLP_VNUM[1:0]

	{ .regAddr= 0x3115, .regData= 0x00, .delayUs=0 }, // INCKSEL1[7:0]
	{ .regAddr= 0x3116, .regData= 0x24, .delayUs=0 }, // INCKSEL2[7:0]
	{ .regAddr= 0x3118, .regData= 0xC0, .delayUs=0 }, // INCKSEL3[10:0]
	{ .regAddr= 0x3119, .regData= 0x00, .delayUs=0 }, // INCKSEL3[10:0]
	{ .regAddr= 0x311A, .regData= 0xE0, .delayUs=0 }, // INCKSEL4[10:0]
	{ .regAddr= 0x311B, .regData= 0x00, .delayUs=0 }, // INCKSEL4[10:0]
	{ .regAddr= 0x311E, .regData= 0x24, .delayUs=0 }, // INCKSEL5[7:0]
	{ .regAddr= 0x32D4, .regData= 0x21, .delayUs=0 },
	{ .regAddr= 0x32EC, .regData= 0xA1, .delayUs=0 },
	/*
	{ .regAddr= 0x344C, .regData= 0x2B, .delayUs=0 },
	{ .regAddr= 0x344D, .regData= 0x01, .delayUs=0 },
	{ .regAddr= 0x344E, .regData= 0xED, .delayUs=0 },
	{ .regAddr= 0x344F, .regData= 0x01, .delayUs=0 },
	{ .regAddr= 0x3450, .regData= 0xF6, .delayUs=0 },
	{ .regAddr= 0x3451, .regData= 0x02, .delayUs=0 },
	*/
	{ .regAddr= 0x3452, .regData= 0x7F, .delayUs=0 },
	{ .regAddr= 0x3453, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x358A, .regData= 0x04, .delayUs=0 },
	{ .regAddr= 0x35A1, .regData= 0x02, .delayUs=0 },
	/*
	{ .regAddr= 0x35EC, .regData= 0x27, .delayUs=0 },
	{ .regAddr= 0x35EE, .regData= 0x8D, .delayUs=0 },
	{ .regAddr= 0x35F0, .regData= 0x8D, .delayUs=0 },
	{ .regAddr= 0x35F2, .regData= 0x29, .delayUs=0 },
	*/
	{ .regAddr= 0x36BC, .regData= 0x0C, .delayUs=0 },
	{ .regAddr= 0x36CC, .regData= 0x53, .delayUs=0 },
	{ .regAddr= 0x36CD, .regData= 0x00, .delayUs=0 },
	{ .regAddr= 0x36CE, .regData= 0x3C, .delayUs=0 },
	{ .regAddr= 0x36D0, .regData= 0x8C, .delayUs=0 },
	{ .regAddr= 0x36D1, .regData= 0x00, .delayUs=0 },
	{ .regAddr= 0x36D2, .regData= 0x71, .delayUs=0 },
	{ .regAddr= 0x36D4, .regData= 0x3C, .delayUs=0 },
	{ .regAddr= 0x36D6, .regData= 0x53, .delayUs=0 },
	{ .regAddr= 0x36D7, .regData= 0x00, .delayUs=0 },
	{ .regAddr= 0x36D8, .regData= 0x71, .delayUs=0 },
	{ .regAddr= 0x36DA, .regData= 0x8C, .delayUs=0 },
	{ .regAddr= 0x36DB, .regData= 0x00, .delayUs=0 },
	{ .regAddr= 0x3701, .regData= 0x00, .delayUs=0 },  //AD 0:10bit, 3:12bit
	//{ .regAddr= 0x3720, .regData= 0x00, .delayUs=0 },
	{ .regAddr= 0x3724, .regData= 0x02, .delayUs=0 },
	{ .regAddr= 0x3726, .regData= 0x02, .delayUs=0 },
	{ .regAddr= 0x3732, .regData= 0x02, .delayUs=0 },
	{ .regAddr= 0x3734, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x3736, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x3742, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x3862, .regData= 0xE0, .delayUs=0 },
	{ .regAddr= 0x38CC, .regData= 0x30, .delayUs=0 },
	{ .regAddr= 0x38CD, .regData= 0x2F, .delayUs=0 },
	{ .regAddr= 0x395C, .regData= 0x0C, .delayUs=0 },
	/*
	{ .regAddr= 0x39A4, .regData= 0x07, .delayUs=0 },
	{ .regAddr= 0x39A8, .regData= 0x32, .delayUs=0 },
	{ .regAddr= 0x39AA, .regData= 0x32, .delayUs=0 },
	{ .regAddr= 0x39AC, .regData= 0x32, .delayUs=0 },
	{ .regAddr= 0x39AE, .regData= 0x32, .delayUs=0 },
	{ .regAddr= 0x39B0, .regData= 0x32, .delayUs=0 },
	{ .regAddr= 0x39B2, .regData= 0x2F, .delayUs=0 },
	{ .regAddr= 0x39B4, .regData= 0x2D, .delayUs=0 },
	{ .regAddr= 0x39B6, .regData= 0x28, .delayUs=0 },
	{ .regAddr= 0x39B8, .regData= 0x30, .delayUs=0 },
	{ .regAddr= 0x39BA, .regData= 0x30, .delayUs=0 },
	{ .regAddr= 0x39BC, .regData= 0x30, .delayUs=0 },
	{ .regAddr= 0x39BE, .regData= 0x30, .delayUs=0 },
	{ .regAddr= 0x39C0, .regData= 0x30, .delayUs=0 },
	{ .regAddr= 0x39C2, .regData= 0x2E, .delayUs=0 },
	{ .regAddr= 0x39C4, .regData= 0x2B, .delayUs=0 },
	{ .regAddr= 0x39C6, .regData= 0x25, .delayUs=0 },
	*/
	{ .regAddr= 0x3A42, .regData= 0xD1, .delayUs=0 },
	{ .regAddr= 0x3A4C, .regData= 0x77, .delayUs=0 },
	{ .regAddr= 0x3AE0, .regData= 0x02, .delayUs=0 },
	{ .regAddr= 0x3AEC, .regData= 0x0C, .delayUs=0 },
	{ .regAddr= 0x3B00, .regData= 0x2E, .delayUs=0 },
	{ .regAddr= 0x3B06, .regData= 0x29, .delayUs=0 },
	{ .regAddr= 0x3B98, .regData= 0x25, .delayUs=0 },
	{ .regAddr= 0x3B99, .regData= 0x21, .delayUs=0 },
	{ .regAddr= 0x3B9B, .regData= 0x13, .delayUs=0 },
	{ .regAddr= 0x3B9C, .regData= 0x13, .delayUs=0 },
	{ .regAddr= 0x3B9D, .regData= 0x13, .delayUs=0 },
	{ .regAddr= 0x3B9E, .regData= 0x13, .delayUs=0 },
	{ .regAddr= 0x3BA1, .regData= 0x00, .delayUs=0 },
	{ .regAddr= 0x3BA2, .regData= 0x06, .delayUs=0 },
	{ .regAddr= 0x3BA3, .regData= 0x0B, .delayUs=0 },
	{ .regAddr= 0x3BA4, .regData= 0x10, .delayUs=0 },
	{ .regAddr= 0x3BA5, .regData= 0x14, .delayUs=0 },
	{ .regAddr= 0x3BA6, .regData= 0x18, .delayUs=0 },
	{ .regAddr= 0x3BA7, .regData= 0x1A, .delayUs=0 },
	{ .regAddr= 0x3BA8, .regData= 0x1A, .delayUs=0 },
	{ .regAddr= 0x3BA9, .regData= 0x1A, .delayUs=0 },
	{ .regAddr= 0x3BAC, .regData= 0xED, .delayUs=0 },
	{ .regAddr= 0x3BAD, .regData= 0x01, .delayUs=0 },
	{ .regAddr= 0x3BAE, .regData= 0xF6, .delayUs=0 },
	{ .regAddr= 0x3BAF, .regData= 0x02, .delayUs=0 },
	{ .regAddr= 0x3BB0, .regData= 0xA2, .delayUs=0 },
	{ .regAddr= 0x3BB1, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x3BB2, .regData= 0xE0, .delayUs=0 },
	{ .regAddr= 0x3BB3, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x3BB4, .regData= 0xE0, .delayUs=0 },
	{ .regAddr= 0x3BB5, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x3BB6, .regData= 0xE0, .delayUs=0 },
	{ .regAddr= 0x3BB7, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x3BB8, .regData= 0xE0, .delayUs=0 },
	{ .regAddr= 0x3BBA, .regData= 0xE0, .delayUs=0 },
	{ .regAddr= 0x3BBC, .regData= 0xDA, .delayUs=0 },
	{ .regAddr= 0x3BBE, .regData= 0x88, .delayUs=0 },
	{ .regAddr= 0x3BC0, .regData= 0x44, .delayUs=0 },
	{ .regAddr= 0x3BC2, .regData= 0x7B, .delayUs=0 },
	{ .regAddr= 0x3BC4, .regData= 0xA2, .delayUs=0 },
	{ .regAddr= 0x3BC8, .regData= 0xBD, .delayUs=0 },
	{ .regAddr= 0x3BCA, .regData= 0xBD, .delayUs=0 },

	{ .regAddr= 0x4001, .regData= 0x03, .delayUs=0 }, // LANEMODE[2:0]
	{ .regAddr= 0x4004, .regData= 0x48, .delayUs=0 }, // TXCLKESC_FREQ[15:0]
	{ .regAddr= 0x4005, .regData= 0x09, .delayUs=0 }, //
	{ .regAddr= 0x400C, .regData= 0x01, .delayUs=0 }, // INCKSEL6

	{ .regAddr= 0x4018, .regData= 0xb7, .delayUs=0 }, // TCLKPOST[15:0]
	{ .regAddr= 0x4019, .regData= 0x00, .delayUs=0 }, // TCLKPOST[15:0]
	{ .regAddr= 0x401A, .regData= 0x67, .delayUs=0 }, // TCLKPREPARE[15:0]
	{ .regAddr= 0x401B, .regData= 0x00, .delayUs=0 }, // TCLKPREPARE[15:0]
	{ .regAddr= 0x401C, .regData= 0x6f, .delayUs=0 }, // TCLKTRAI[15:0]
	{ .regAddr= 0x401D, .regData= 0x00, .delayUs=0 }, // TCLKTRAI[15:0]
	{ .regAddr= 0x401E, .regData= 0xdf, .delayUs=0 }, // TCLKZERO[15:0]
	{ .regAddr= 0x401F, .regData= 0x01, .delayUs=0 }, // TCLKZERO[15:0]
	{ .regAddr= 0x4020, .regData= 0x6F, .delayUs=0 }, // THSPREPARE[15:0]
	{ .regAddr= 0x4021, .regData= 0x00, .delayUs=0 }, // THSPREPARE[15:0]
	{ .regAddr= 0x4022, .regData= 0xcf, .delayUs=0 }, // THSZERO[15:0]
	{ .regAddr= 0x4023, .regData= 0x00, .delayUs=0 }, // THSZERO[15:0]
	{ .regAddr= 0x4024, .regData= 0x6F, .delayUs=0 }, // THSTRAI[15:0]
	{ .regAddr= 0x4025, .regData= 0x00, .delayUs=0 }, // THSTRAI[15:0]
	{ .regAddr= 0x4026, .regData= 0xb7, .delayUs=0 }, // THSEXIT[15:0]
	{ .regAddr= 0x4027, .regData= 0x00, .delayUs=0 }, // THSEXIT[15:0]
	{ .regAddr= 0x4028, .regData= 0x5f, .delayUs=0 }, // TLPX[15:0]
	{ .regAddr= 0x4029, .regData= 0x00, .delayUs=0 }, // TLPX[15:0]
	{ .regAddr= 0x4074, .regData= 0x00, .delayUs=0 }, // INCKSEL7 [2:0]
    { .regAddr = 0x3000, .regData = 0x00, .delayUs = 24000}, /* leave standby */
};

static drv_sensor_reg_info_t  cv2003_5MP_DOL2_12fps_setting[] =
{
   /*
CV2003-AAQR Window cropping 2688x1860 CSI-2_4lane 37.125Mhz AD:12bit Output:12bit 891Mbps Master Mode DOL HDR 2frame VC 12.002fps Integration Time LEF:8.003ms SEF:0.536ms
Ver9.0
*/
  { .regAddr= 0x3000, .regData= 0x01, .delayUs=0 }, /* enter standby */
	{ .regAddr= 0x3002, .regData= 0x01, .delayUs=0 }, /* master stop */
  { .regAddr= 0x3008, .regData= 0x7F, .delayUs=0 }, // BCWAIT_TIME[9:0]
  { .regAddr= 0x300A, .regData= 0x5B, .delayUs=0 }, // CPWAIT_TIME[9:0]
  { .regAddr= 0x301C, .regData= 0x04, .delayUs=0 }, // WINMODE[3:0]
  { .regAddr= 0x3023, .regData= 0x00, .delayUs=0 }, // VCMODE[3:0]
  { .regAddr= 0x3024, .regData= 0xFC, .delayUs=0 }, // VMAX[19:0]
  { .regAddr= 0x3025, .regData= 0x0A, .delayUs=0 }, //
  { .regAddr= 0x3028, .regData= 0x4C, .delayUs=0 }, // HMAX[15:0]
  { .regAddr= 0x3029, .regData= 0x04, .delayUs=0 }, //
  { .regAddr= 0x302C, .regData= 0x01, .delayUs=0 }, // WDMODE[1:0]
  { .regAddr= 0x302D, .regData= 0x01, .delayUs=0 }, // WDSEL[1:0]
  { .regAddr= 0x3033, .regData= 0x05, .delayUs=0 }, // SYS_MODE[3:0]
  { .regAddr= 0x3040, .regData= 0x4C, .delayUs=0 }, // PIX_HST[12:0]
  { .regAddr= 0x3041, .regData= 0x02, .delayUs=0 }, //
  { .regAddr= 0x3042, .regData= 0x80, .delayUs=0 }, // PIX_HWIDTH[12:0]
  { .regAddr= 0x3043, .regData= 0x0A, .delayUs=0 }, //
  { .regAddr= 0x3044, .regData= 0x4C, .delayUs=0 }, // PIX_VST[12:0]
  { .regAddr= 0x3045, .regData= 0x01, .delayUs=0 }, //
  { .regAddr= 0x3046, .regData= 0x88, .delayUs=0 }, // PIX_VWIDTH[12:0]
  { .regAddr= 0x3047, .regData= 0x0E, .delayUs=0 }, //
  { .regAddr= 0x3050, .regData= 0xDC, .delayUs=0 }, // SHR0[19:0]
  { .regAddr= 0x3051, .regData= 0x13, .delayUs=0 }, //
  { .regAddr= 0x3054, .regData= 0x09, .delayUs=0 }, // SHR1[19:0]
  { .regAddr= 0x3060, .regData= 0x2D, .delayUs=0 }, // RHS1[19:0]
  { .regAddr= 0x30C1, .regData= 0x00, .delayUs=0 }, // XVS_DRV[1:0]
  { .regAddr= 0x30CF, .regData= 0x01, .delayUs=0 }, // XVSMSKCNT_INT[1:0]
  { .regAddr= 0x3116, .regData= 0x24, .delayUs=0 }, // INCKSEL2[7:0]
  { .regAddr= 0x311E, .regData= 0x24, .delayUs=0 }, // INCKSEL5[7:0]
  { .regAddr= 0x32D4, .regData= 0x21, .delayUs=0 }, // -
  { .regAddr= 0x32EC, .regData= 0xA1, .delayUs=0 }, // -
  /*{ .regAddr= 0x344C, .regData= 0x2B, .delayUs=0 }, // -
  { .regAddr= 0x344D, .regData= 0x01, .delayUs=0 }, // -
  { .regAddr= 0x344E, .regData= 0xED, .delayUs=0 }, // -
  { .regAddr= 0x344F, .regData= 0x01, .delayUs=0 }, // -
  { .regAddr= 0x3450, .regData= 0xF6, .delayUs=0 }, // -
  { .regAddr= 0x3451, .regData= 0x02, .delayUs=0 }, // -*/
  { .regAddr= 0x3452, .regData= 0x7F, .delayUs=0 }, // -
  { .regAddr= 0x3453, .regData= 0x03, .delayUs=0 }, // -
  { .regAddr= 0x358A, .regData= 0x04, .delayUs=0 }, // -
  { .regAddr= 0x35A1, .regData= 0x02, .delayUs=0 }, // -
  /*{ .regAddr= 0x35EC, .regData= 0x27, .delayUs=0 }, // -
  { .regAddr= 0x35EE, .regData= 0x8D, .delayUs=0 }, // -
  { .regAddr= 0x35F0, .regData= 0x8D, .delayUs=0 }, // -
  { .regAddr= 0x35F2, .regData= 0x29, .delayUs=0 }, // -*/
  { .regAddr= 0x36BC, .regData= 0x0C, .delayUs=0 }, // -
  { .regAddr= 0x36CC, .regData= 0x53, .delayUs=0 }, // -
  { .regAddr= 0x36CD, .regData= 0x00, .delayUs=0 }, // -
  { .regAddr= 0x36CE, .regData= 0x3C, .delayUs=0 }, // -
  { .regAddr= 0x36D0, .regData= 0x8C, .delayUs=0 }, // -
  { .regAddr= 0x36D1, .regData= 0x00, .delayUs=0 }, // -
  { .regAddr= 0x36D2, .regData= 0x71, .delayUs=0 }, // -
  { .regAddr= 0x36D4, .regData= 0x3C, .delayUs=0 }, // -
  { .regAddr= 0x36D6, .regData= 0x53, .delayUs=0 }, // -
  { .regAddr= 0x36D7, .regData= 0x00, .delayUs=0 }, // -
  { .regAddr= 0x36D8, .regData= 0x71, .delayUs=0 }, // -
  { .regAddr= 0x36DA, .regData= 0x8C, .delayUs=0 }, // -
  { .regAddr= 0x36DB, .regData= 0x00, .delayUs=0 }, // -
  { .regAddr= 0x3720, .regData= 0x00, .delayUs=0 }, // -
  { .regAddr= 0x3724, .regData= 0x02, .delayUs=0 }, // -
  { .regAddr= 0x3726, .regData= 0x02, .delayUs=0 }, // -
  { .regAddr= 0x3732, .regData= 0x02, .delayUs=0 }, // -
  { .regAddr= 0x3734, .regData= 0x03, .delayUs=0 }, // -
  { .regAddr= 0x3736, .regData= 0x03, .delayUs=0 }, // -
  { .regAddr= 0x3742, .regData= 0x03, .delayUs=0 }, // -
  { .regAddr= 0x3862, .regData= 0xE0, .delayUs=0 }, // -
  { .regAddr= 0x38CC, .regData= 0x30, .delayUs=0 }, // -
  { .regAddr= 0x38CD, .regData= 0x2F, .delayUs=0 }, // -
  { .regAddr= 0x395C, .regData= 0x0C, .delayUs=0 }, // -
  /*{ .regAddr= 0x39A4, .regData= 0x07, .delayUs=0 }, // -
  { .regAddr= 0x39A8, .regData= 0x32, .delayUs=0 }, // -
  { .regAddr= 0x39AA, .regData= 0x32, .delayUs=0 }, // -
  { .regAddr= 0x39AC, .regData= 0x32, .delayUs=0 }, // -
  { .regAddr= 0x39AE, .regData= 0x32, .delayUs=0 }, // -
  { .regAddr= 0x39B0, .regData= 0x32, .delayUs=0 }, // -
  { .regAddr= 0x39B2, .regData= 0x2F, .delayUs=0 }, // -
  { .regAddr= 0x39B4, .regData= 0x2D, .delayUs=0 }, // -
  { .regAddr= 0x39B6, .regData= 0x28, .delayUs=0 }, // -
  { .regAddr= 0x39B8, .regData= 0x30, .delayUs=0 }, // -
  { .regAddr= 0x39BA, .regData= 0x30, .delayUs=0 }, // -
  { .regAddr= 0x39BC, .regData= 0x30, .delayUs=0 }, // -
  { .regAddr= 0x39BE, .regData= 0x30, .delayUs=0 }, // -
  { .regAddr= 0x39C0, .regData= 0x30, .delayUs=0 }, // -
  { .regAddr= 0x39C2, .regData= 0x2E, .delayUs=0 }, // -
  { .regAddr= 0x39C4, .regData= 0x2B, .delayUs=0 }, // -
  { .regAddr= 0x39C6, .regData= 0x25, .delayUs=0 }, // -*/
  { .regAddr= 0x3A42, .regData= 0xD1, .delayUs=0 }, // -
  { .regAddr= 0x3A4C, .regData= 0x77, .delayUs=0 }, // -
  { .regAddr= 0x3AE0, .regData= 0x02, .delayUs=0 }, // -
  { .regAddr= 0x3AEC, .regData= 0x0C, .delayUs=0 }, // -
  { .regAddr= 0x3B00, .regData= 0x2E, .delayUs=0 }, // -
  { .regAddr= 0x3B06, .regData= 0x29, .delayUs=0 }, // -
  { .regAddr= 0x3B98, .regData= 0x25, .delayUs=0 }, // -
  { .regAddr= 0x3B99, .regData= 0x21, .delayUs=0 }, // -
  { .regAddr= 0x3B9B, .regData= 0x13, .delayUs=0 }, // -
  { .regAddr= 0x3B9C, .regData= 0x13, .delayUs=0 }, // -
  { .regAddr= 0x3B9D, .regData= 0x13, .delayUs=0 }, // -
  { .regAddr= 0x3B9E, .regData= 0x13, .delayUs=0 }, // -
  { .regAddr= 0x3BA1, .regData= 0x00, .delayUs=0 }, // -
  { .regAddr= 0x3BA2, .regData= 0x06, .delayUs=0 }, // -
  { .regAddr= 0x3BA3, .regData= 0x0B, .delayUs=0 }, // -
  { .regAddr= 0x3BA4, .regData= 0x10, .delayUs=0 }, // -
  { .regAddr= 0x3BA5, .regData= 0x14, .delayUs=0 }, // -
  { .regAddr= 0x3BA6, .regData= 0x18, .delayUs=0 }, // -
  { .regAddr= 0x3BA7, .regData= 0x1A, .delayUs=0 }, // -
  { .regAddr= 0x3BA8, .regData= 0x1A, .delayUs=0 }, // -
  { .regAddr= 0x3BA9, .regData= 0x1A, .delayUs=0 }, // -
  { .regAddr= 0x3BAC, .regData= 0xED, .delayUs=0 }, // -
  { .regAddr= 0x3BAD, .regData= 0x01, .delayUs=0 }, // -
  { .regAddr= 0x3BAE, .regData= 0xF6, .delayUs=0 }, // -
  { .regAddr= 0x3BAF, .regData= 0x02, .delayUs=0 }, // -
  { .regAddr= 0x3BB0, .regData= 0xA2, .delayUs=0 }, // -
  { .regAddr= 0x3BB1, .regData= 0x03, .delayUs=0 }, // -
  { .regAddr= 0x3BB2, .regData= 0xE0, .delayUs=0 }, // -
  { .regAddr= 0x3BB3, .regData= 0x03, .delayUs=0 }, // -
  { .regAddr= 0x3BB4, .regData= 0xE0, .delayUs=0 }, // -
  { .regAddr= 0x3BB5, .regData= 0x03, .delayUs=0 }, // -
  { .regAddr= 0x3BB6, .regData= 0xE0, .delayUs=0 }, // -
  { .regAddr= 0x3BB7, .regData= 0x03, .delayUs=0 }, // -
  { .regAddr= 0x3BB8, .regData= 0xE0, .delayUs=0 }, // -
  { .regAddr= 0x3BBA, .regData= 0xE0, .delayUs=0 }, // -
  { .regAddr= 0x3BBC, .regData= 0xDA, .delayUs=0 }, // -
  { .regAddr= 0x3BBE, .regData= 0x88, .delayUs=0 }, // -
  { .regAddr= 0x3BC0, .regData= 0x44, .delayUs=0 }, // -
  { .regAddr= 0x3BC2, .regData= 0x7B, .delayUs=0 }, // -
  { .regAddr= 0x3BC4, .regData= 0xA2, .delayUs=0 }, // -
  { .regAddr= 0x3BC8, .regData= 0xBD, .delayUs=0 }, // -
  { .regAddr= 0x3BCA, .regData= 0xBD, .delayUs=0 }, // -
  { .regAddr= 0x4004, .regData= 0x48, .delayUs=0 }, // TXCLKESC_FREQ[15:0]
  { .regAddr= 0x4005, .regData= 0x09, .delayUs=0 }, //
  { .regAddr= 0x400C, .regData= 0x00, .delayUs=0 }, // INCKSEL6
  { .regAddr= 0x4018, .regData= 0x7F, .delayUs=0 }, // TCLKPOST[15:0]
  { .regAddr= 0x401A, .regData= 0x37, .delayUs=0 }, // TCLKPREPARE[15:0]
  { .regAddr= 0x401C, .regData= 0x37, .delayUs=0 }, // TCLKTRAIL[15:0]
  { .regAddr= 0x401E, .regData= 0xF7, .delayUs=0 }, // TCLKZERO[15:0]
  { .regAddr= 0x401F, .regData= 0x00, .delayUs=0 }, //
  { .regAddr= 0x4020, .regData= 0x3F, .delayUs=0 }, // THSPREPARE[15:0]
  { .regAddr= 0x4022, .regData= 0x6F, .delayUs=0 }, // THSZERO[15:0]
  { .regAddr= 0x4024, .regData= 0x3F, .delayUs=0 }, // THSTRAIL[15:0]
  { .regAddr= 0x4026, .regData= 0x5F, .delayUs=0 }, // THSEXIT[15:0]
  { .regAddr= 0x4028, .regData= 0x2F, .delayUs=0 }, // TLPX[15:0]
  { .regAddr= 0x4074, .regData= 0x01, .delayUs=0 }, // INCKSEL7 [2:0]
  { .regAddr = 0x3000, .regData = 0x00, .delayUs = 24000}, /* leave standby */
};
static drv_sensor_reg_info_t  cv2003_4MP_DOL2_12fps_setting[] =
{
   /*
CV2003-AAQR Window cropping 2592x1536 CSI-2_4lane 37.125Mhz AD:12bit Output:12bit 891Mbps Master Mode DOL HDR 2frame VC 12.002fps Integration Time LEF:8.003ms SEF:0.536ms
Ver9.0
*/
  { .regAddr= 0x3000, .regData= 0x01, .delayUs=0 }, /* enter standby */
	{ .regAddr= 0x3002, .regData= 0x01, .delayUs=0 }, /* master stop */
  { .regAddr= 0x3008, .regData= 0x7F, .delayUs=0 }, // BCWAIT_TIME[9:0]
  { .regAddr= 0x300A, .regData= 0x5B, .delayUs=0 }, // CPWAIT_TIME[9:0]
  { .regAddr= 0x301C, .regData= 0x04, .delayUs=0 }, // WINMODE[3:0]
  { .regAddr= 0x3023, .regData= 0x00, .delayUs=0 }, // VCMODE[3:0]

  { .regAddr= 0x3024, .regData= 0xFC, .delayUs=0 }, // VMAX[19:0]
  { .regAddr= 0x3025, .regData= 0x0A, .delayUs=0 }, //
  { .regAddr= 0x3028, .regData= 0x4C, .delayUs=0 }, // HMAX[15:0]
  { .regAddr= 0x3029, .regData= 0x04, .delayUs=0 }, //

  /*
  { .regAddr= 0x3024, .regData= 0xFC, .delayUs=0 }, // VMAX[19:0]
  { .regAddr= 0x3025, .regData= 0x0A, .delayUs=0 }, //
  { .regAddr= 0x3028, .regData= 0x4C, .delayUs=0 }, // HMAX[15:0]
  { .regAddr= 0x3029, .regData= 0x04, .delayUs=0 }, //
  */
  { .regAddr= 0x302C, .regData= 0x01, .delayUs=0 }, // WDMODE[1:0]
  { .regAddr= 0x302D, .regData= 0x01, .delayUs=0 }, // WDSEL[1:0]
  { .regAddr= 0x3033, .regData= 0x05, .delayUs=0 }, // SYS_MODE[3:0]  04:1782M  05:891M
  { .regAddr= 0x3040, .regData= 0x7C, .delayUs=0 }, // PIX_HST[12:0]
  { .regAddr= 0x3041, .regData= 0x02, .delayUs=0 }, //
  { .regAddr= 0x3042, .regData= 0x20, .delayUs=0 }, // PIX_HWIDTH[12:0]
  { .regAddr= 0x3043, .regData= 0x0A, .delayUs=0 }, //
  { .regAddr= 0x3044, .regData= 0x90, .delayUs=0 }, // PIX_VST[12:0]
  { .regAddr= 0x3045, .regData= 0x02, .delayUs=0 }, //
  { .regAddr= 0x3046, .regData= 0x00, .delayUs=0 }, // PIX_VWIDTH[12:0]
  { .regAddr= 0x3047, .regData= 0x0C, .delayUs=0 }, //
  { .regAddr= 0x3050, .regData= 0xDC, .delayUs=0 }, // SHR0[19:0]
  { .regAddr= 0x3051, .regData= 0x13, .delayUs=0 }, //
  { .regAddr= 0x3054, .regData= 0x09, .delayUs=0 }, // SHR1[19:0]
  { .regAddr= 0x3060, .regData= 0x2D, .delayUs=0 }, // RHS1[19:0]
  { .regAddr= 0x30C1, .regData= 0x00, .delayUs=0 }, // XVS_DRV[1:0]
  { .regAddr= 0x30CF, .regData= 0x01, .delayUs=0 }, // XVSMSKCNT_INT[1:0]
  { .regAddr= 0x3116, .regData= 0x24, .delayUs=0 }, // INCKSEL2[7:0]
  { .regAddr= 0x311E, .regData= 0x24, .delayUs=0 }, // INCKSEL5[7:0]
  { .regAddr= 0x32D4, .regData= 0x21, .delayUs=0 }, // -
  { .regAddr= 0x32EC, .regData= 0xA1, .delayUs=0 }, // -
  /*{ .regAddr= 0x344C, .regData= 0x2B, .delayUs=0 }, // -
  { .regAddr= 0x344D, .regData= 0x01, .delayUs=0 }, // -
  { .regAddr= 0x344E, .regData= 0xED, .delayUs=0 }, // -
  { .regAddr= 0x344F, .regData= 0x01, .delayUs=0 }, // -
  { .regAddr= 0x3450, .regData= 0xF6, .delayUs=0 }, // -
  { .regAddr= 0x3451, .regData= 0x02, .delayUs=0 }, // -*/
  { .regAddr= 0x3452, .regData= 0x7F, .delayUs=0 }, // -
  { .regAddr= 0x3453, .regData= 0x03, .delayUs=0 }, // -
  { .regAddr= 0x358A, .regData= 0x04, .delayUs=0 }, // -
  { .regAddr= 0x35A1, .regData= 0x02, .delayUs=0 }, // -
  /*{ .regAddr= 0x35EC, .regData= 0x27, .delayUs=0 }, // -
  { .regAddr= 0x35EE, .regData= 0x8D, .delayUs=0 }, // -
  { .regAddr= 0x35F0, .regData= 0x8D, .delayUs=0 }, // -
  { .regAddr= 0x35F2, .regData= 0x29, .delayUs=0 }, // -*/
  { .regAddr= 0x36BC, .regData= 0x0C, .delayUs=0 }, // -
  { .regAddr= 0x36CC, .regData= 0x53, .delayUs=0 }, // -
  { .regAddr= 0x36CD, .regData= 0x00, .delayUs=0 }, // -
  { .regAddr= 0x36CE, .regData= 0x3C, .delayUs=0 }, // -
  { .regAddr= 0x36D0, .regData= 0x8C, .delayUs=0 }, // -
  { .regAddr= 0x36D1, .regData= 0x00, .delayUs=0 }, // -
  { .regAddr= 0x36D2, .regData= 0x71, .delayUs=0 }, // -
  { .regAddr= 0x36D4, .regData= 0x3C, .delayUs=0 }, // -
  { .regAddr= 0x36D6, .regData= 0x53, .delayUs=0 }, // -
  { .regAddr= 0x36D7, .regData= 0x00, .delayUs=0 }, // -
  { .regAddr= 0x36D8, .regData= 0x71, .delayUs=0 }, // -
  { .regAddr= 0x36DA, .regData= 0x8C, .delayUs=0 }, // -
  { .regAddr= 0x36DB, .regData= 0x00, .delayUs=0 }, // -
  { .regAddr= 0x3720, .regData= 0x00, .delayUs=0 }, // -
  { .regAddr= 0x3724, .regData= 0x02, .delayUs=0 }, // -
  { .regAddr= 0x3726, .regData= 0x02, .delayUs=0 }, // -
  { .regAddr= 0x3732, .regData= 0x02, .delayUs=0 }, // -
  { .regAddr= 0x3734, .regData= 0x03, .delayUs=0 }, // -
  { .regAddr= 0x3736, .regData= 0x03, .delayUs=0 }, // -
  { .regAddr= 0x3742, .regData= 0x03, .delayUs=0 }, // -
  { .regAddr= 0x3862, .regData= 0xE0, .delayUs=0 }, // -
  { .regAddr= 0x38CC, .regData= 0x30, .delayUs=0 }, // -
  { .regAddr= 0x38CD, .regData= 0x2F, .delayUs=0 }, // -
  { .regAddr= 0x395C, .regData= 0x0C, .delayUs=0 }, // -
  /*{ .regAddr= 0x39A4, .regData= 0x07, .delayUs=0 }, // -
  { .regAddr= 0x39A8, .regData= 0x32, .delayUs=0 }, // -
  { .regAddr= 0x39AA, .regData= 0x32, .delayUs=0 }, // -
  { .regAddr= 0x39AC, .regData= 0x32, .delayUs=0 }, // -
  { .regAddr= 0x39AE, .regData= 0x32, .delayUs=0 }, // -
  { .regAddr= 0x39B0, .regData= 0x32, .delayUs=0 }, // -
  { .regAddr= 0x39B2, .regData= 0x2F, .delayUs=0 }, // -
  { .regAddr= 0x39B4, .regData= 0x2D, .delayUs=0 }, // -
  { .regAddr= 0x39B6, .regData= 0x28, .delayUs=0 }, // -
  { .regAddr= 0x39B8, .regData= 0x30, .delayUs=0 }, // -
  { .regAddr= 0x39BA, .regData= 0x30, .delayUs=0 }, // -
  { .regAddr= 0x39BC, .regData= 0x30, .delayUs=0 }, // -
  { .regAddr= 0x39BE, .regData= 0x30, .delayUs=0 }, // -
  { .regAddr= 0x39C0, .regData= 0x30, .delayUs=0 }, // -
  { .regAddr= 0x39C2, .regData= 0x2E, .delayUs=0 }, // -
  { .regAddr= 0x39C4, .regData= 0x2B, .delayUs=0 }, // -
  { .regAddr= 0x39C6, .regData= 0x25, .delayUs=0 }, // -*/
  { .regAddr= 0x3A42, .regData= 0xD1, .delayUs=0 }, // -
  { .regAddr= 0x3A4C, .regData= 0x77, .delayUs=0 }, // -
  { .regAddr= 0x3AE0, .regData= 0x02, .delayUs=0 }, // -
  { .regAddr= 0x3AEC, .regData= 0x0C, .delayUs=0 }, // -
  { .regAddr= 0x3B00, .regData= 0x2E, .delayUs=0 }, // -
  { .regAddr= 0x3B06, .regData= 0x29, .delayUs=0 }, // -
  { .regAddr= 0x3B98, .regData= 0x25, .delayUs=0 }, // -
  { .regAddr= 0x3B99, .regData= 0x21, .delayUs=0 }, // -
  { .regAddr= 0x3B9B, .regData= 0x13, .delayUs=0 }, // -
  { .regAddr= 0x3B9C, .regData= 0x13, .delayUs=0 }, // -
  { .regAddr= 0x3B9D, .regData= 0x13, .delayUs=0 }, // -
  { .regAddr= 0x3B9E, .regData= 0x13, .delayUs=0 }, // -
  { .regAddr= 0x3BA1, .regData= 0x00, .delayUs=0 }, // -
  { .regAddr= 0x3BA2, .regData= 0x06, .delayUs=0 }, // -
  { .regAddr= 0x3BA3, .regData= 0x0B, .delayUs=0 }, // -
  { .regAddr= 0x3BA4, .regData= 0x10, .delayUs=0 }, // -
  { .regAddr= 0x3BA5, .regData= 0x14, .delayUs=0 }, // -
  { .regAddr= 0x3BA6, .regData= 0x18, .delayUs=0 }, // -
  { .regAddr= 0x3BA7, .regData= 0x1A, .delayUs=0 }, // -
  { .regAddr= 0x3BA8, .regData= 0x1A, .delayUs=0 }, // -
  { .regAddr= 0x3BA9, .regData= 0x1A, .delayUs=0 }, // -
  { .regAddr= 0x3BAC, .regData= 0xED, .delayUs=0 }, // -
  { .regAddr= 0x3BAD, .regData= 0x01, .delayUs=0 }, // -
  { .regAddr= 0x3BAE, .regData= 0xF6, .delayUs=0 }, // -
  { .regAddr= 0x3BAF, .regData= 0x02, .delayUs=0 }, // -
  { .regAddr= 0x3BB0, .regData= 0xA2, .delayUs=0 }, // -
  { .regAddr= 0x3BB1, .regData= 0x03, .delayUs=0 }, // -
  { .regAddr= 0x3BB2, .regData= 0xE0, .delayUs=0 }, // -
  { .regAddr= 0x3BB3, .regData= 0x03, .delayUs=0 }, // -
  { .regAddr= 0x3BB4, .regData= 0xE0, .delayUs=0 }, // -
  { .regAddr= 0x3BB5, .regData= 0x03, .delayUs=0 }, // -
  { .regAddr= 0x3BB6, .regData= 0xE0, .delayUs=0 }, // -
  { .regAddr= 0x3BB7, .regData= 0x03, .delayUs=0 }, // -
  { .regAddr= 0x3BB8, .regData= 0xE0, .delayUs=0 }, // -
  { .regAddr= 0x3BBA, .regData= 0xE0, .delayUs=0 }, // -
  { .regAddr= 0x3BBC, .regData= 0xDA, .delayUs=0 }, // -
  { .regAddr= 0x3BBE, .regData= 0x88, .delayUs=0 }, // -
  { .regAddr= 0x3BC0, .regData= 0x44, .delayUs=0 }, // -
  { .regAddr= 0x3BC2, .regData= 0x7B, .delayUs=0 }, // -
  { .regAddr= 0x3BC4, .regData= 0xA2, .delayUs=0 }, // -
  { .regAddr= 0x3BC8, .regData= 0xBD, .delayUs=0 }, // -
  { .regAddr= 0x3BCA, .regData= 0xBD, .delayUs=0 }, // -
  { .regAddr= 0x4004, .regData= 0x48, .delayUs=0 }, // TXCLKESC_FREQ[15:0]
  { .regAddr= 0x4005, .regData= 0x09, .delayUs=0 }, //
  { .regAddr= 0x400C, .regData= 0x00, .delayUs=0 }, // INCKSEL6
  { .regAddr= 0x4018, .regData= 0x7F, .delayUs=0 }, // TCLKPOST[15:0]
  { .regAddr= 0x401A, .regData= 0x37, .delayUs=0 }, // TCLKPREPARE[15:0]
  { .regAddr= 0x401C, .regData= 0x37, .delayUs=0 }, // TCLKTRAIL[15:0]
  { .regAddr= 0x401E, .regData= 0xF7, .delayUs=0 }, // TCLKZERO[15:0]
  { .regAddr= 0x401F, .regData= 0x00, .delayUs=0 }, //
  { .regAddr= 0x4020, .regData= 0x3F, .delayUs=0 }, // THSPREPARE[15:0]
  { .regAddr= 0x4022, .regData= 0x6F, .delayUs=0 }, // THSZERO[15:0]
  { .regAddr= 0x4024, .regData= 0x3F, .delayUs=0 }, // THSTRAIL[15:0]
  { .regAddr= 0x4026, .regData= 0x5F, .delayUs=0 }, // THSEXIT[15:0]
  { .regAddr= 0x4028, .regData= 0x2F, .delayUs=0 }, // TLPX[15:0]
  { .regAddr= 0x4074, .regData= 0x01, .delayUs=0 }, // INCKSEL7 [2:0]
  { .regAddr = 0x3000, .regData = 0x00, .delayUs = 24000}, /* leave standby */
};
static drv_sensor_reg_info_t  cv2003_500W_30fps_setting[] =
{
	{ .regAddr= 0x3000, .regData= 0x01, .delayUs=0 }, /* enter standby */
	{ .regAddr= 0x3002, .regData= 0x01, .delayUs=0 }, /* master stop */
	{ .regAddr= 0x3008, .regData= 0x7F, .delayUs=0 }, // BCWAIT_TIME[9:0]
	{ .regAddr= 0x3009, .regData= 0x00, .delayUs=0 }, // BCWAIT_TIME[9:0]
	{ .regAddr= 0x300A, .regData= 0x5B, .delayUs=0 }, // CPWAIT_TIME[9:0]
	{ .regAddr= 0x300B, .regData= 0x00, .delayUs=0 }, // CPWAIT_TIME[9:0]
	{ .regAddr= 0x301c, .regData= 0x4, .delayUs=0 },  //WIN MODE enable
	{ .regAddr= 0x3020, .regData= 0x0, .delayUs=0 },  //horizontal 2 binning
	{ .regAddr= 0x3021, .regData= 0x0, .delayUs=0 },  //veritical 2 binning
	{ .regAddr= 0x3022, .regData= 0x0, .delayUs=0 },  //ADDMODE 2/2-line binning
	{ .regAddr= 0x3024, .regData= 0xCA, .delayUs=0 }, // VMAX[19:0]
	{ .regAddr= 0x3025, .regData= 0x08, .delayUs=0 }, // VMAX[19:0]
	{ .regAddr= 0x3026, .regData= 0x00, .delayUs=0 }, // VMAX[19:0]

	//{ .regAddr= 0x3028, .regData= 0x4c, .delayUs=0 }, // HMAX[15:0]
	//{ .regAddr= 0x3029, .regData= 0x04, .delayUs=0 }, // HMAX[15:0]
	{ .regAddr= 0x3028, .regData= 0x40, .delayUs=0 }, // HMAX[15:0] 25fps
	{ .regAddr= 0x3029, .regData= 0x05, .delayUs=0 }, // HMAX[15:0]
	//{ .regAddr= 0x3028, .regData= 0xa0, .delayUs=0 }, // HMAX[15:0] 19fps
	//{ .regAddr= 0x3029, .regData= 0x06, .delayUs=0 }, // HMAX[15:0]
    { .regAddr= 0x3030, .regData= 0x00, .delayUs=0 },  //normal
	{ .regAddr= 0x3031, .regData= 0x01, .delayUs=0 }, // AD:12 bit
	{ .regAddr= 0x3032, .regData= 0x01, .delayUs=0 }, // MD:12 bit
	{ .regAddr= 0x3033, .regData= 0x05, .delayUs=0 }, // SYS_MODE[3:0]

	{ .regAddr= 0x302c, .regData= 0x00, .delayUs=0 }, // WDMODE[1:0]
	{ .regAddr= 0x302d, .regData= 0x00, .delayUs=0 }, // WDSEL[1:0]
	{ .regAddr= 0x30CF, .regData= 0x00, .delayUs=0 }, // XVSMSKCNT_INT[1:0]

	/*crop mode*/
	{ .regAddr= 0x3040, .regData= 0x00, .delayUs=0 }, // PIX_HST[7:0]
	{ .regAddr= 0x3041, .regData= 0x00, .delayUs=0 }, // PIX_HST[11:8]
	{ .regAddr= 0x3044, .regData= 0x00, .delayUs=0 }, // PIX_VST[7:0]
	{ .regAddr= 0x3045, .regData= 0x00, .delayUs=0 }, // PIX_VST[11:8]
	/*width 2688x1860*/
	{ .regAddr= 0x3042, .regData= 0x80, .delayUs=0 }, // PIX_HWID[7:0]
	{ .regAddr= 0x3043, .regData= 0x0A, .delayUs=0 }, // PIX_HWID[11:8]
	{ .regAddr= 0x3046, .regData= 0xA8, .delayUs=0 }, // PIX_VWID[7:0]
	{ .regAddr= 0x3047, .regData= 0x0E, .delayUs=0 }, // PIX_VWID[11:8]

	{ .regAddr= 0x3050, .regData= 0xC6, .delayUs=0 }, // SHR0[19:0]
	{ .regAddr= 0x3051, .regData= 0x02, .delayUs=0 }, // SHR0[19:0]
	{ .regAddr= 0x3052, .regData= 0x00, .delayUs=0 }, // SHR0[19:0]
	//{ .regAddr= 0x3050, .regData= 0x66, .delayUs=0 }, // SHR0[19:0]
	//{ .regAddr= 0x3051, .regData= 0x00, .delayUs=0 }, //
	//{ .regAddr= 0x3052, .regData= 0x00, .delayUs=0 }, //
	//{ .regAddr= 0x3090, .regData= 0x14, .delayUs=0 }, // GAIN_PCG_0[8:0]
	//{ .regAddr= 0x3091, .regData= 0x00, .delayUs=0 }, // GAIN_PCG_0[8:0]
	{ .regAddr= 0x3090, .regData= 0x00, .delayUs=0 }, // GAIN_PCG_0[8:0]
	{ .regAddr= 0x3091, .regData= 0x00, .delayUs=0 }, // GAIN_PCG_0[8:0]
	{ .regAddr= 0x30C1, .regData= 0x00, .delayUs=0 }, // XVS_DRV[1:0]

	{ .regAddr= 0x30D9, .regData= 0x02, .delayUs=0 }, // DIG_CLP_VSTAET[4:0]
	{ .regAddr= 0x30DA, .regData= 0x01, .delayUs=0 }, // DIG_VLP_VNUM[1:0]

	{ .regAddr= 0x3115, .regData= 0x00, .delayUs=0 }, // INCKSEL1[7:0]
	{ .regAddr= 0x3116, .regData= 0x24, .delayUs=0 }, // INCKSEL2[7:0]
	{ .regAddr= 0x3118, .regData= 0xC0, .delayUs=0 }, // INCKSEL3[10:0]
	{ .regAddr= 0x3119, .regData= 0x00, .delayUs=0 }, // INCKSEL3[10:0]
	{ .regAddr= 0x311A, .regData= 0xE0, .delayUs=0 }, // INCKSEL4[10:0]
	{ .regAddr= 0x311B, .regData= 0x00, .delayUs=0 }, // INCKSEL4[10:0]
	{ .regAddr= 0x311E, .regData= 0x24, .delayUs=0 }, // INCKSEL5[7:0]
	{ .regAddr= 0x32D4, .regData= 0x21, .delayUs=0 },
	{ .regAddr= 0x32EC, .regData= 0xA1, .delayUs=0 },
	/*
	{ .regAddr= 0x344C, .regData= 0x2B, .delayUs=0 },
	{ .regAddr= 0x344D, .regData= 0x01, .delayUs=0 },
	{ .regAddr= 0x344E, .regData= 0xED, .delayUs=0 },
	{ .regAddr= 0x344F, .regData= 0x01, .delayUs=0 },
	{ .regAddr= 0x3450, .regData= 0xF6, .delayUs=0 },
	{ .regAddr= 0x3451, .regData= 0x02, .delayUs=0 },
	*/
	{ .regAddr= 0x3452, .regData= 0x7F, .delayUs=0 },
	{ .regAddr= 0x3453, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x358A, .regData= 0x04, .delayUs=0 },
	{ .regAddr= 0x35A1, .regData= 0x02, .delayUs=0 },
	/*
	{ .regAddr= 0x35EC, .regData= 0x27, .delayUs=0 },
	{ .regAddr= 0x35EE, .regData= 0x8D, .delayUs=0 },
	{ .regAddr= 0x35F0, .regData= 0x8D, .delayUs=0 },
	{ .regAddr= 0x35F2, .regData= 0x29, .delayUs=0 },
	*/
	{ .regAddr= 0x36BC, .regData= 0x0C, .delayUs=0 },
	{ .regAddr= 0x36CC, .regData= 0x53, .delayUs=0 },
	{ .regAddr= 0x36CD, .regData= 0x00, .delayUs=0 },
	{ .regAddr= 0x36CE, .regData= 0x3C, .delayUs=0 },
	{ .regAddr= 0x36D0, .regData= 0x8C, .delayUs=0 },
	{ .regAddr= 0x36D1, .regData= 0x00, .delayUs=0 },
	{ .regAddr= 0x36D2, .regData= 0x71, .delayUs=0 },
	{ .regAddr= 0x36D4, .regData= 0x3C, .delayUs=0 },
	{ .regAddr= 0x36D6, .regData= 0x53, .delayUs=0 },
	{ .regAddr= 0x36D7, .regData= 0x00, .delayUs=0 },
	{ .regAddr= 0x36D8, .regData= 0x71, .delayUs=0 },
	{ .regAddr= 0x36DA, .regData= 0x8C, .delayUs=0 },
	{ .regAddr= 0x36DB, .regData= 0x00, .delayUs=0 },
	{ .regAddr= 0x3701, .regData= 0x03, .delayUs=0 },  //AD 0:10bit, 3:12bit
	//{ .regAddr= 0x3720, .regData= 0x00, .delayUs=0 },
	{ .regAddr= 0x3724, .regData= 0x02, .delayUs=0 },
	{ .regAddr= 0x3726, .regData= 0x02, .delayUs=0 },
	{ .regAddr= 0x3732, .regData= 0x02, .delayUs=0 },
	{ .regAddr= 0x3734, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x3736, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x3742, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x3862, .regData= 0xE0, .delayUs=0 },
	{ .regAddr= 0x38CC, .regData= 0x30, .delayUs=0 },
	{ .regAddr= 0x38CD, .regData= 0x2F, .delayUs=0 },
	{ .regAddr= 0x395C, .regData= 0x0C, .delayUs=0 },
	/*
	{ .regAddr= 0x39A4, .regData= 0x07, .delayUs=0 },
	{ .regAddr= 0x39A8, .regData= 0x32, .delayUs=0 },
	{ .regAddr= 0x39AA, .regData= 0x32, .delayUs=0 },
	{ .regAddr= 0x39AC, .regData= 0x32, .delayUs=0 },
	{ .regAddr= 0x39AE, .regData= 0x32, .delayUs=0 },
	{ .regAddr= 0x39B0, .regData= 0x32, .delayUs=0 },
	{ .regAddr= 0x39B2, .regData= 0x2F, .delayUs=0 },
	{ .regAddr= 0x39B4, .regData= 0x2D, .delayUs=0 },
	{ .regAddr= 0x39B6, .regData= 0x28, .delayUs=0 },
	{ .regAddr= 0x39B8, .regData= 0x30, .delayUs=0 },
	{ .regAddr= 0x39BA, .regData= 0x30, .delayUs=0 },
	{ .regAddr= 0x39BC, .regData= 0x30, .delayUs=0 },
	{ .regAddr= 0x39BE, .regData= 0x30, .delayUs=0 },
	{ .regAddr= 0x39C0, .regData= 0x30, .delayUs=0 },
	{ .regAddr= 0x39C2, .regData= 0x2E, .delayUs=0 },
	{ .regAddr= 0x39C4, .regData= 0x2B, .delayUs=0 },
	{ .regAddr= 0x39C6, .regData= 0x25, .delayUs=0 },
	*/
	{ .regAddr= 0x3A42, .regData= 0xD1, .delayUs=0 },
	{ .regAddr= 0x3A4C, .regData= 0x77, .delayUs=0 },
	{ .regAddr= 0x3AE0, .regData= 0x02, .delayUs=0 },
	{ .regAddr= 0x3AEC, .regData= 0x0C, .delayUs=0 },
	{ .regAddr= 0x3B00, .regData= 0x2E, .delayUs=0 },
	{ .regAddr= 0x3B06, .regData= 0x29, .delayUs=0 },
	{ .regAddr= 0x3B98, .regData= 0x25, .delayUs=0 },
	{ .regAddr= 0x3B99, .regData= 0x21, .delayUs=0 },
	{ .regAddr= 0x3B9B, .regData= 0x13, .delayUs=0 },
	{ .regAddr= 0x3B9C, .regData= 0x13, .delayUs=0 },
	{ .regAddr= 0x3B9D, .regData= 0x13, .delayUs=0 },
	{ .regAddr= 0x3B9E, .regData= 0x13, .delayUs=0 },
	{ .regAddr= 0x3BA1, .regData= 0x00, .delayUs=0 },
	{ .regAddr= 0x3BA2, .regData= 0x06, .delayUs=0 },
	{ .regAddr= 0x3BA3, .regData= 0x0B, .delayUs=0 },
	{ .regAddr= 0x3BA4, .regData= 0x10, .delayUs=0 },
	{ .regAddr= 0x3BA5, .regData= 0x14, .delayUs=0 },
	{ .regAddr= 0x3BA6, .regData= 0x18, .delayUs=0 },
	{ .regAddr= 0x3BA7, .regData= 0x1A, .delayUs=0 },
	{ .regAddr= 0x3BA8, .regData= 0x1A, .delayUs=0 },
	{ .regAddr= 0x3BA9, .regData= 0x1A, .delayUs=0 },
	{ .regAddr= 0x3BAC, .regData= 0xED, .delayUs=0 },
	{ .regAddr= 0x3BAD, .regData= 0x01, .delayUs=0 },
	{ .regAddr= 0x3BAE, .regData= 0xF6, .delayUs=0 },
	{ .regAddr= 0x3BAF, .regData= 0x02, .delayUs=0 },
	{ .regAddr= 0x3BB0, .regData= 0xA2, .delayUs=0 },
	{ .regAddr= 0x3BB1, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x3BB2, .regData= 0xE0, .delayUs=0 },
	{ .regAddr= 0x3BB3, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x3BB4, .regData= 0xE0, .delayUs=0 },
	{ .regAddr= 0x3BB5, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x3BB6, .regData= 0xE0, .delayUs=0 },
	{ .regAddr= 0x3BB7, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x3BB8, .regData= 0xE0, .delayUs=0 },
	{ .regAddr= 0x3BBA, .regData= 0xE0, .delayUs=0 },
	{ .regAddr= 0x3BBC, .regData= 0xDA, .delayUs=0 },
	{ .regAddr= 0x3BBE, .regData= 0x88, .delayUs=0 },
	{ .regAddr= 0x3BC0, .regData= 0x44, .delayUs=0 },
	{ .regAddr= 0x3BC2, .regData= 0x7B, .delayUs=0 },
	{ .regAddr= 0x3BC4, .regData= 0xA2, .delayUs=0 },
	{ .regAddr= 0x3BC8, .regData= 0xBD, .delayUs=0 },
	{ .regAddr= 0x3BCA, .regData= 0xBD, .delayUs=0 },

	{ .regAddr= 0x4001, .regData= 0x03, .delayUs=0 }, // LANEMODE[2:0]
	{ .regAddr= 0x4004, .regData= 0x48, .delayUs=0 }, // TXCLKESC_FREQ[15:0]
	{ .regAddr= 0x4005, .regData= 0x09, .delayUs=0 }, //
	{ .regAddr= 0x400C, .regData= 0x00, .delayUs=0 }, // INCKSEL6

	{ .regAddr= 0x4018, .regData= 0x7f, .delayUs=0 }, // TCLKPOST[15:0]
	{ .regAddr= 0x4019, .regData= 0x00, .delayUs=0 }, // TCLKPOST[15:0]
	{ .regAddr= 0x401A, .regData= 0x37, .delayUs=0 }, // TCLKPREPARE[15:0]
	{ .regAddr= 0x401B, .regData= 0x00, .delayUs=0 }, // TCLKPREPARE[15:0]
	{ .regAddr= 0x401C, .regData= 0x37, .delayUs=0 }, // TCLKTRAI[15:0]
	{ .regAddr= 0x401D, .regData= 0x00, .delayUs=0 }, // TCLKTRAI[15:0]
	{ .regAddr= 0x401E, .regData= 0xF7, .delayUs=0 }, // TCLKZERO[15:0]
	{ .regAddr= 0x401F, .regData= 0x00, .delayUs=0 }, // TCLKZERO[15:0]
	{ .regAddr= 0x4020, .regData= 0x3F, .delayUs=0 }, // THSPREPARE[15:0]
	{ .regAddr= 0x4021, .regData= 0x00, .delayUs=0 }, // THSPREPARE[15:0]
	{ .regAddr= 0x4022, .regData= 0x6F, .delayUs=0 }, // THSZERO[15:0]
	{ .regAddr= 0x4023, .regData= 0x00, .delayUs=0 }, // THSZERO[15:0]
	{ .regAddr= 0x4024, .regData= 0x3F, .delayUs=0 }, // THSTRAI[15:0]
	{ .regAddr= 0x4025, .regData= 0x00, .delayUs=0 }, // THSTRAI[15:0]
	{ .regAddr= 0x4026, .regData= 0x5f, .delayUs=0 }, // THSEXIT[15:0]
	{ .regAddr= 0x4027, .regData= 0x00, .delayUs=0 }, // THSEXIT[15:0]
	{ .regAddr= 0x4028, .regData= 0x2f, .delayUs=0 }, // TLPX[15:0]
	{ .regAddr= 0x4029, .regData= 0x00, .delayUs=0 }, // TLPX[15:0]
	{ .regAddr= 0x4074, .regData= 0x01, .delayUs=0 }, // INCKSEL7 [2:0]
    { .regAddr = 0x3000, .regData = 0x00, .delayUs = 24000}, /* leave standby */
};
static drv_sensor_reg_info_t  cv2003_400W_30fps_setting[] =
{
	{ .regAddr= 0x3000, .regData= 0x01, .delayUs=0 }, /* enter standby */
	{ .regAddr= 0x3002, .regData= 0x01, .delayUs=0 }, /* master stop */
	{ .regAddr= 0x3008, .regData= 0x7F, .delayUs=0 }, // BCWAIT_TIME[9:0]
	{ .regAddr= 0x3009, .regData= 0x00, .delayUs=0 }, // BCWAIT_TIME[9:0]
	{ .regAddr= 0x300A, .regData= 0x5B, .delayUs=0 }, // CPWAIT_TIME[9:0]
	{ .regAddr= 0x300B, .regData= 0x00, .delayUs=0 }, // CPWAIT_TIME[9:0]
	{ .regAddr= 0x301c, .regData= 0x4, .delayUs=0 },  //WIN MODE enable
	{ .regAddr= 0x3020, .regData= 0x0, .delayUs=0 },  //horizontal 2 binning
	{ .regAddr= 0x3021, .regData= 0x0, .delayUs=0 },  //veritical 2 binning
	{ .regAddr= 0x3022, .regData= 0x0, .delayUs=0 },  //ADDMODE 2/2-line binning
	{ .regAddr= 0x3024, .regData= 0xCA, .delayUs=0 }, // VMAX[19:0]
	{ .regAddr= 0x3025, .regData= 0x08, .delayUs=0 }, // VMAX[19:0]
	{ .regAddr= 0x3026, .regData= 0x00, .delayUs=0 }, // VMAX[19:0]

	//{ .regAddr= 0x3028, .regData= 0x4c, .delayUs=0 }, // HMAX[15:0]
	//{ .regAddr= 0x3029, .regData= 0x04, .delayUs=0 }, // HMAX[15:0]
//	{ .regAddr= 0x3028, .regData= 0x40, .delayUs=0 }, // HMAX[15:0] 25fps
//	{ .regAddr= 0x3029, .regData= 0x05, .delayUs=0 }, // HMAX[15:0]
	{ .regAddr= 0x3028, .regData= 0xa0, .delayUs=0 }, // HMAX[15:0] 19fps
	{ .regAddr= 0x3029, .regData= 0x06, .delayUs=0 }, // HMAX[15:0]
    { .regAddr= 0x3030, .regData= 0x00, .delayUs=0 },  //normal
	{ .regAddr= 0x3031, .regData= 0x01, .delayUs=0 }, // AD:12 bit
	{ .regAddr= 0x3032, .regData= 0x01, .delayUs=0 }, // MD:12 bit
	{ .regAddr= 0x3033, .regData= 0x05, .delayUs=0 }, // SYS_MODE[3:0]

	{ .regAddr= 0x302c, .regData= 0x00, .delayUs=0 }, // WDMODE[1:0]
	{ .regAddr= 0x302d, .regData= 0x00, .delayUs=0 }, // WDSEL[1:0]
	{ .regAddr= 0x30CF, .regData= 0x00, .delayUs=0 }, // XVSMSKCNT_INT[1:0]

	/*crop mode*/
	{ .regAddr= 0x3040, .regData= 0x00, .delayUs=0 }, // PIX_HST[7:0]
	{ .regAddr= 0x3041, .regData= 0x00, .delayUs=0 }, // PIX_HST[11:8]
	{ .regAddr= 0x3044, .regData= 0x00, .delayUs=0 }, // PIX_VST[7:0]
	{ .regAddr= 0x3045, .regData= 0x00, .delayUs=0 }, // PIX_VST[11:8]
	/*width 2592x1536*/
	{ .regAddr= 0x3042, .regData= 0x20, .delayUs=0 }, // PIX_HWID[7:0]
	{ .regAddr= 0x3043, .regData= 0x0A, .delayUs=0 }, // PIX_HWID[11:8]
	{ .regAddr= 0x3046, .regData= 0x20, .delayUs=0 }, // PIX_VWID[7:0]
	{ .regAddr= 0x3047, .regData= 0x0C, .delayUs=0 }, // PIX_VWID[11:8]

	{ .regAddr= 0x3050, .regData= 0xC6, .delayUs=0 }, // SHR0[19:0]
	{ .regAddr= 0x3051, .regData= 0x02, .delayUs=0 }, // SHR0[19:0]
	{ .regAddr= 0x3052, .regData= 0x00, .delayUs=0 }, // SHR0[19:0]
	//{ .regAddr= 0x3050, .regData= 0x66, .delayUs=0 }, // SHR0[19:0]
	//{ .regAddr= 0x3051, .regData= 0x00, .delayUs=0 }, //
	//{ .regAddr= 0x3052, .regData= 0x00, .delayUs=0 }, //
	//{ .regAddr= 0x3090, .regData= 0x14, .delayUs=0 }, // GAIN_PCG_0[8:0]
	//{ .regAddr= 0x3091, .regData= 0x00, .delayUs=0 }, // GAIN_PCG_0[8:0]
	{ .regAddr= 0x3090, .regData= 0x00, .delayUs=0 }, // GAIN_PCG_0[8:0]
	{ .regAddr= 0x3091, .regData= 0x00, .delayUs=0 }, // GAIN_PCG_0[8:0]
	{ .regAddr= 0x30C1, .regData= 0x00, .delayUs=0 }, // XVS_DRV[1:0]

	{ .regAddr= 0x30D9, .regData= 0x02, .delayUs=0 }, // DIG_CLP_VSTAET[4:0]
	{ .regAddr= 0x30DA, .regData= 0x01, .delayUs=0 }, // DIG_VLP_VNUM[1:0]

	{ .regAddr= 0x3115, .regData= 0x00, .delayUs=0 }, // INCKSEL1[7:0]
	{ .regAddr= 0x3116, .regData= 0x24, .delayUs=0 }, // INCKSEL2[7:0]
	{ .regAddr= 0x3118, .regData= 0xC0, .delayUs=0 }, // INCKSEL3[10:0]
	{ .regAddr= 0x3119, .regData= 0x00, .delayUs=0 }, // INCKSEL3[10:0]
	{ .regAddr= 0x311A, .regData= 0xE0, .delayUs=0 }, // INCKSEL4[10:0]
	{ .regAddr= 0x311B, .regData= 0x00, .delayUs=0 }, // INCKSEL4[10:0]
	{ .regAddr= 0x311E, .regData= 0x24, .delayUs=0 }, // INCKSEL5[7:0]
	{ .regAddr= 0x32D4, .regData= 0x21, .delayUs=0 },
	{ .regAddr= 0x32EC, .regData= 0xA1, .delayUs=0 },
	/*
	{ .regAddr= 0x344C, .regData= 0x2B, .delayUs=0 },
	{ .regAddr= 0x344D, .regData= 0x01, .delayUs=0 },
	{ .regAddr= 0x344E, .regData= 0xED, .delayUs=0 },
	{ .regAddr= 0x344F, .regData= 0x01, .delayUs=0 },
	{ .regAddr= 0x3450, .regData= 0xF6, .delayUs=0 },
	{ .regAddr= 0x3451, .regData= 0x02, .delayUs=0 },
	*/
	{ .regAddr= 0x3452, .regData= 0x7F, .delayUs=0 },
	{ .regAddr= 0x3453, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x358A, .regData= 0x04, .delayUs=0 },
	{ .regAddr= 0x35A1, .regData= 0x02, .delayUs=0 },
	/*
	{ .regAddr= 0x35EC, .regData= 0x27, .delayUs=0 },
	{ .regAddr= 0x35EE, .regData= 0x8D, .delayUs=0 },
	{ .regAddr= 0x35F0, .regData= 0x8D, .delayUs=0 },
	{ .regAddr= 0x35F2, .regData= 0x29, .delayUs=0 },
	*/
	{ .regAddr= 0x36BC, .regData= 0x0C, .delayUs=0 },
	{ .regAddr= 0x36CC, .regData= 0x53, .delayUs=0 },
	{ .regAddr= 0x36CD, .regData= 0x00, .delayUs=0 },
	{ .regAddr= 0x36CE, .regData= 0x3C, .delayUs=0 },
	{ .regAddr= 0x36D0, .regData= 0x8C, .delayUs=0 },
	{ .regAddr= 0x36D1, .regData= 0x00, .delayUs=0 },
	{ .regAddr= 0x36D2, .regData= 0x71, .delayUs=0 },
	{ .regAddr= 0x36D4, .regData= 0x3C, .delayUs=0 },
	{ .regAddr= 0x36D6, .regData= 0x53, .delayUs=0 },
	{ .regAddr= 0x36D7, .regData= 0x00, .delayUs=0 },
	{ .regAddr= 0x36D8, .regData= 0x71, .delayUs=0 },
	{ .regAddr= 0x36DA, .regData= 0x8C, .delayUs=0 },
	{ .regAddr= 0x36DB, .regData= 0x00, .delayUs=0 },
	{ .regAddr= 0x3701, .regData= 0x03, .delayUs=0 },  //AD 0:10bit, 3:12bit
	//{ .regAddr= 0x3720, .regData= 0x00, .delayUs=0 },
	{ .regAddr= 0x3724, .regData= 0x02, .delayUs=0 },
	{ .regAddr= 0x3726, .regData= 0x02, .delayUs=0 },
	{ .regAddr= 0x3732, .regData= 0x02, .delayUs=0 },
	{ .regAddr= 0x3734, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x3736, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x3742, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x3862, .regData= 0xE0, .delayUs=0 },
	{ .regAddr= 0x38CC, .regData= 0x30, .delayUs=0 },
	{ .regAddr= 0x38CD, .regData= 0x2F, .delayUs=0 },
	{ .regAddr= 0x395C, .regData= 0x0C, .delayUs=0 },
	/*
	{ .regAddr= 0x39A4, .regData= 0x07, .delayUs=0 },
	{ .regAddr= 0x39A8, .regData= 0x32, .delayUs=0 },
	{ .regAddr= 0x39AA, .regData= 0x32, .delayUs=0 },
	{ .regAddr= 0x39AC, .regData= 0x32, .delayUs=0 },
	{ .regAddr= 0x39AE, .regData= 0x32, .delayUs=0 },
	{ .regAddr= 0x39B0, .regData= 0x32, .delayUs=0 },
	{ .regAddr= 0x39B2, .regData= 0x2F, .delayUs=0 },
	{ .regAddr= 0x39B4, .regData= 0x2D, .delayUs=0 },
	{ .regAddr= 0x39B6, .regData= 0x28, .delayUs=0 },
	{ .regAddr= 0x39B8, .regData= 0x30, .delayUs=0 },
	{ .regAddr= 0x39BA, .regData= 0x30, .delayUs=0 },
	{ .regAddr= 0x39BC, .regData= 0x30, .delayUs=0 },
	{ .regAddr= 0x39BE, .regData= 0x30, .delayUs=0 },
	{ .regAddr= 0x39C0, .regData= 0x30, .delayUs=0 },
	{ .regAddr= 0x39C2, .regData= 0x2E, .delayUs=0 },
	{ .regAddr= 0x39C4, .regData= 0x2B, .delayUs=0 },
	{ .regAddr= 0x39C6, .regData= 0x25, .delayUs=0 },
	*/
	{ .regAddr= 0x3A42, .regData= 0xD1, .delayUs=0 },
	{ .regAddr= 0x3A4C, .regData= 0x77, .delayUs=0 },
	{ .regAddr= 0x3AE0, .regData= 0x02, .delayUs=0 },
	{ .regAddr= 0x3AEC, .regData= 0x0C, .delayUs=0 },
	{ .regAddr= 0x3B00, .regData= 0x2E, .delayUs=0 },
	{ .regAddr= 0x3B06, .regData= 0x29, .delayUs=0 },
	{ .regAddr= 0x3B98, .regData= 0x25, .delayUs=0 },
	{ .regAddr= 0x3B99, .regData= 0x21, .delayUs=0 },
	{ .regAddr= 0x3B9B, .regData= 0x13, .delayUs=0 },
	{ .regAddr= 0x3B9C, .regData= 0x13, .delayUs=0 },
	{ .regAddr= 0x3B9D, .regData= 0x13, .delayUs=0 },
	{ .regAddr= 0x3B9E, .regData= 0x13, .delayUs=0 },
	{ .regAddr= 0x3BA1, .regData= 0x00, .delayUs=0 },
	{ .regAddr= 0x3BA2, .regData= 0x06, .delayUs=0 },
	{ .regAddr= 0x3BA3, .regData= 0x0B, .delayUs=0 },
	{ .regAddr= 0x3BA4, .regData= 0x10, .delayUs=0 },
	{ .regAddr= 0x3BA5, .regData= 0x14, .delayUs=0 },
	{ .regAddr= 0x3BA6, .regData= 0x18, .delayUs=0 },
	{ .regAddr= 0x3BA7, .regData= 0x1A, .delayUs=0 },
	{ .regAddr= 0x3BA8, .regData= 0x1A, .delayUs=0 },
	{ .regAddr= 0x3BA9, .regData= 0x1A, .delayUs=0 },
	{ .regAddr= 0x3BAC, .regData= 0xED, .delayUs=0 },
	{ .regAddr= 0x3BAD, .regData= 0x01, .delayUs=0 },
	{ .regAddr= 0x3BAE, .regData= 0xF6, .delayUs=0 },
	{ .regAddr= 0x3BAF, .regData= 0x02, .delayUs=0 },
	{ .regAddr= 0x3BB0, .regData= 0xA2, .delayUs=0 },
	{ .regAddr= 0x3BB1, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x3BB2, .regData= 0xE0, .delayUs=0 },
	{ .regAddr= 0x3BB3, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x3BB4, .regData= 0xE0, .delayUs=0 },
	{ .regAddr= 0x3BB5, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x3BB6, .regData= 0xE0, .delayUs=0 },
	{ .regAddr= 0x3BB7, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x3BB8, .regData= 0xE0, .delayUs=0 },
	{ .regAddr= 0x3BBA, .regData= 0xE0, .delayUs=0 },
	{ .regAddr= 0x3BBC, .regData= 0xDA, .delayUs=0 },
	{ .regAddr= 0x3BBE, .regData= 0x88, .delayUs=0 },
	{ .regAddr= 0x3BC0, .regData= 0x44, .delayUs=0 },
	{ .regAddr= 0x3BC2, .regData= 0x7B, .delayUs=0 },
	{ .regAddr= 0x3BC4, .regData= 0xA2, .delayUs=0 },
	{ .regAddr= 0x3BC8, .regData= 0xBD, .delayUs=0 },
	{ .regAddr= 0x3BCA, .regData= 0xBD, .delayUs=0 },

	{ .regAddr= 0x4001, .regData= 0x03, .delayUs=0 }, // LANEMODE[2:0]
	{ .regAddr= 0x4004, .regData= 0x48, .delayUs=0 }, // TXCLKESC_FREQ[15:0]
	{ .regAddr= 0x4005, .regData= 0x09, .delayUs=0 }, //
	{ .regAddr= 0x400C, .regData= 0x00, .delayUs=0 }, // INCKSEL6

	{ .regAddr= 0x4018, .regData= 0x7f, .delayUs=0 }, // TCLKPOST[15:0]
	{ .regAddr= 0x4019, .regData= 0x00, .delayUs=0 }, // TCLKPOST[15:0]
	{ .regAddr= 0x401A, .regData= 0x37, .delayUs=0 }, // TCLKPREPARE[15:0]
	{ .regAddr= 0x401B, .regData= 0x00, .delayUs=0 }, // TCLKPREPARE[15:0]
	{ .regAddr= 0x401C, .regData= 0x37, .delayUs=0 }, // TCLKTRAI[15:0]
	{ .regAddr= 0x401D, .regData= 0x00, .delayUs=0 }, // TCLKTRAI[15:0]
	{ .regAddr= 0x401E, .regData= 0xF7, .delayUs=0 }, // TCLKZERO[15:0]
	{ .regAddr= 0x401F, .regData= 0x00, .delayUs=0 }, // TCLKZERO[15:0]
	{ .regAddr= 0x4020, .regData= 0x3F, .delayUs=0 }, // THSPREPARE[15:0]
	{ .regAddr= 0x4021, .regData= 0x00, .delayUs=0 }, // THSPREPARE[15:0]
	{ .regAddr= 0x4022, .regData= 0x6F, .delayUs=0 }, // THSZERO[15:0]
	{ .regAddr= 0x4023, .regData= 0x00, .delayUs=0 }, // THSZERO[15:0]
	{ .regAddr= 0x4024, .regData= 0x3F, .delayUs=0 }, // THSTRAI[15:0]
	{ .regAddr= 0x4025, .regData= 0x00, .delayUs=0 }, // THSTRAI[15:0]
	{ .regAddr= 0x4026, .regData= 0x5f, .delayUs=0 }, // THSEXIT[15:0]
	{ .regAddr= 0x4027, .regData= 0x00, .delayUs=0 }, // THSEXIT[15:0]
	{ .regAddr= 0x4028, .regData= 0x2f, .delayUs=0 }, // TLPX[15:0]
	{ .regAddr= 0x4029, .regData= 0x00, .delayUs=0 }, // TLPX[15:0]
	{ .regAddr= 0x4074, .regData= 0x01, .delayUs=0 }, // INCKSEL7 [2:0]
    { .regAddr = 0x3000, .regData = 0x00, .delayUs = 24000}, /* leave standby */
};

static drv_sensor_reg_info_t  cv2003_4MP_30fps_setting[] =
{
	{ .regAddr= 0x3000, .regData= 0x01, .delayUs=0 }, /* enter standby */
	{ .regAddr= 0x3002, .regData= 0x01, .delayUs=0 }, /* master stop */
	{ .regAddr= 0x3008, .regData= 0x7F, .delayUs=0 }, // BCWAIT_TIME[9:0]
	{ .regAddr= 0x3009, .regData= 0x00, .delayUs=0 }, // BCWAIT_TIME[9:0]
	{ .regAddr= 0x300A, .regData= 0x5B, .delayUs=0 }, // CPWAIT_TIME[9:0]
	{ .regAddr= 0x300B, .regData= 0x00, .delayUs=0 }, // CPWAIT_TIME[9:0]
//cropping
	{ .regAddr= 0x301c, .regData= 0x04, .delayUs=0 },  //WINMODE  4h:window croping mode
	{ .regAddr= 0x3040, .regData= 0x00, .delayUs=0 },  //PIX_HST effective pixel Start position(H direction)
	{ .regAddr= 0x3041, .regData= 0x00, .delayUs=0 },
	{ .regAddr= 0x3042, .regData= 0x20, .delayUs=0 },  //PIX_HWIDTH  4h:window croping mode
	{ .regAddr= 0x3043, .regData= 0x0A, .delayUs=0 },
	{ .regAddr= 0x3044, .regData= 0x00, .delayUs=0 },  //PIX_VST  4h:window croping mode
	{ .regAddr= 0x3045, .regData= 0x00, .delayUs=0 },
        { .regAddr= 0x3046, .regData= 0x00, .delayUs=0 },   //PIX_VWIDTH  4h:window croping mode
        { .regAddr= 0x3047, .regData= 0x06, .delayUs=0 },

	{ .regAddr= 0x3020, .regData= 0x0, .delayUs=0 },  //horizontal 2 binning
	{ .regAddr= 0x3021, .regData= 0x0, .delayUs=0 },  //veritical 2 binning
	{ .regAddr= 0x3022, .regData= 0x0, .delayUs=0 },  //ADDMODE 2/2-line binning
	{ .regAddr= 0x3024, .regData= 0xCA, .delayUs=0 }, // VMAX[19:0]
	{ .regAddr= 0x3025, .regData= 0x08, .delayUs=0 }, // VMAX[19:0]
	{ .regAddr= 0x3026, .regData= 0x00, .delayUs=0 }, // VMAX[19:0]

	//{ .regAddr= 0x3028, .regData= 0x4c, .delayUs=0 }, // HMAX[15:0]
	//{ .regAddr= 0x3029, .regData= 0x04, .delayUs=0 }, // HMAX[15:0]
//	{ .regAddr= 0x3028, .regData= 0x40, .delayUs=0 }, // HMAX[15:0] 25fps
//	{ .regAddr= 0x3029, .regData= 0x05, .delayUs=0 }, // HMAX[15:0]
	{ .regAddr= 0x3028, .regData= 0xa0, .delayUs=0 }, // HMAX[15:0] 19fps
	{ .regAddr= 0x3029, .regData= 0x06, .delayUs=0 }, // HMAX[15:0]
  	{ .regAddr= 0x3030, .regData= 0x00, .delayUs=0 },  //normal
	{ .regAddr= 0x3031, .regData= 0x01, .delayUs=0 }, // AD:12 bit
	{ .regAddr= 0x3032, .regData= 0x01, .delayUs=0 }, // MD:12 bit
	{ .regAddr= 0x3033, .regData= 0x05, .delayUs=0 }, // SYS_MODE[3:0]

	{ .regAddr= 0x302c, .regData= 0x00, .delayUs=0 }, // WDMODE[1:0]
	{ .regAddr= 0x302d, .regData= 0x00, .delayUs=0 }, // WDSEL[1:0]
	{ .regAddr= 0x30CF, .regData= 0x00, .delayUs=0 }, // XVSMSKCNT_INT[1:0]

	{ .regAddr= 0x3050, .regData= 0xC6, .delayUs=0 }, // SHR0[19:0]
	{ .regAddr= 0x3051, .regData= 0x02, .delayUs=0 }, // SHR0[19:0]
	{ .regAddr= 0x3052, .regData= 0x00, .delayUs=0 }, // SHR0[19:0]
	//{ .regAddr= 0x3050, .regData= 0x66, .delayUs=0 }, // SHR0[19:0]
	//{ .regAddr= 0x3051, .regData= 0x00, .delayUs=0 }, //
	//{ .regAddr= 0x3052, .regData= 0x00, .delayUs=0 }, //
	//{ .regAddr= 0x3090, .regData= 0x14, .delayUs=0 }, // GAIN_PCG_0[8:0]
	//{ .regAddr= 0x3091, .regData= 0x00, .delayUs=0 }, // GAIN_PCG_0[8:0]
	{ .regAddr= 0x3090, .regData= 0x00, .delayUs=0 }, // GAIN_PCG_0[8:0]
	{ .regAddr= 0x3091, .regData= 0x00, .delayUs=0 }, // GAIN_PCG_0[8:0]
	{ .regAddr= 0x30C1, .regData= 0x00, .delayUs=0 }, // XVS_DRV[1:0]

	{ .regAddr= 0x30D9, .regData= 0x02, .delayUs=0 }, // DIG_CLP_VSTAET[4:0]
	{ .regAddr= 0x30DA, .regData= 0x01, .delayUs=0 }, // DIG_VLP_VNUM[1:0]

	{ .regAddr= 0x3115, .regData= 0x00, .delayUs=0 }, // INCKSEL1[7:0]
	{ .regAddr= 0x3116, .regData= 0x24, .delayUs=0 }, // INCKSEL2[7:0]
	{ .regAddr= 0x3118, .regData= 0xC0, .delayUs=0 }, // INCKSEL3[10:0]
	{ .regAddr= 0x3119, .regData= 0x00, .delayUs=0 }, // INCKSEL3[10:0]
	{ .regAddr= 0x311A, .regData= 0xE0, .delayUs=0 }, // INCKSEL4[10:0]
	{ .regAddr= 0x311B, .regData= 0x00, .delayUs=0 }, // INCKSEL4[10:0]
	{ .regAddr= 0x311E, .regData= 0x24, .delayUs=0 }, // INCKSEL5[7:0]
	{ .regAddr= 0x32D4, .regData= 0x21, .delayUs=0 },
	{ .regAddr= 0x32EC, .regData= 0xA1, .delayUs=0 },
	/*
	{ .regAddr= 0x344C, .regData= 0x2B, .delayUs=0 },
	{ .regAddr= 0x344D, .regData= 0x01, .delayUs=0 },
	{ .regAddr= 0x344E, .regData= 0xED, .delayUs=0 },
	{ .regAddr= 0x344F, .regData= 0x01, .delayUs=0 },
	{ .regAddr= 0x3450, .regData= 0xF6, .delayUs=0 },
	{ .regAddr= 0x3451, .regData= 0x02, .delayUs=0 },
	*/
	{ .regAddr= 0x3452, .regData= 0x7F, .delayUs=0 },
	{ .regAddr= 0x3453, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x358A, .regData= 0x04, .delayUs=0 },
	{ .regAddr= 0x35A1, .regData= 0x02, .delayUs=0 },
	/*
	{ .regAddr= 0x35EC, .regData= 0x27, .delayUs=0 },
	{ .regAddr= 0x35EE, .regData= 0x8D, .delayUs=0 },
	{ .regAddr= 0x35F0, .regData= 0x8D, .delayUs=0 },
	{ .regAddr= 0x35F2, .regData= 0x29, .delayUs=0 },
	*/
	{ .regAddr= 0x36BC, .regData= 0x0C, .delayUs=0 },
	{ .regAddr= 0x36CC, .regData= 0x53, .delayUs=0 },
	{ .regAddr= 0x36CD, .regData= 0x00, .delayUs=0 },
	{ .regAddr= 0x36CE, .regData= 0x3C, .delayUs=0 },
	{ .regAddr= 0x36D0, .regData= 0x8C, .delayUs=0 },
	{ .regAddr= 0x36D1, .regData= 0x00, .delayUs=0 },
	{ .regAddr= 0x36D2, .regData= 0x71, .delayUs=0 },
	{ .regAddr= 0x36D4, .regData= 0x3C, .delayUs=0 },
	{ .regAddr= 0x36D6, .regData= 0x53, .delayUs=0 },
	{ .regAddr= 0x36D7, .regData= 0x00, .delayUs=0 },
	{ .regAddr= 0x36D8, .regData= 0x71, .delayUs=0 },
	{ .regAddr= 0x36DA, .regData= 0x8C, .delayUs=0 },
	{ .regAddr= 0x36DB, .regData= 0x00, .delayUs=0 },
	{ .regAddr= 0x3701, .regData= 0x03, .delayUs=0 },  //AD 0:10bit, 3:12bit
	//{ .regAddr= 0x3720, .regData= 0x00, .delayUs=0 },
	{ .regAddr= 0x3724, .regData= 0x02, .delayUs=0 },
	{ .regAddr= 0x3726, .regData= 0x02, .delayUs=0 },
	{ .regAddr= 0x3732, .regData= 0x02, .delayUs=0 },
	{ .regAddr= 0x3734, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x3736, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x3742, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x3862, .regData= 0xE0, .delayUs=0 },
	{ .regAddr= 0x38CC, .regData= 0x30, .delayUs=0 },
	{ .regAddr= 0x38CD, .regData= 0x2F, .delayUs=0 },
	{ .regAddr= 0x395C, .regData= 0x0C, .delayUs=0 },
	/*
	{ .regAddr= 0x39A4, .regData= 0x07, .delayUs=0 },
	{ .regAddr= 0x39A8, .regData= 0x32, .delayUs=0 },
	{ .regAddr= 0x39AA, .regData= 0x32, .delayUs=0 },
	{ .regAddr= 0x39AC, .regData= 0x32, .delayUs=0 },
	{ .regAddr= 0x39AE, .regData= 0x32, .delayUs=0 },
	{ .regAddr= 0x39B0, .regData= 0x32, .delayUs=0 },
	{ .regAddr= 0x39B2, .regData= 0x2F, .delayUs=0 },
	{ .regAddr= 0x39B4, .regData= 0x2D, .delayUs=0 },
	{ .regAddr= 0x39B6, .regData= 0x28, .delayUs=0 },
	{ .regAddr= 0x39B8, .regData= 0x30, .delayUs=0 },
	{ .regAddr= 0x39BA, .regData= 0x30, .delayUs=0 },
	{ .regAddr= 0x39BC, .regData= 0x30, .delayUs=0 },
	{ .regAddr= 0x39BE, .regData= 0x30, .delayUs=0 },
	{ .regAddr= 0x39C0, .regData= 0x30, .delayUs=0 },
	{ .regAddr= 0x39C2, .regData= 0x2E, .delayUs=0 },
	{ .regAddr= 0x39C4, .regData= 0x2B, .delayUs=0 },
	{ .regAddr= 0x39C6, .regData= 0x25, .delayUs=0 },
	*/
	{ .regAddr= 0x3A42, .regData= 0xD1, .delayUs=0 },
	{ .regAddr= 0x3A4C, .regData= 0x77, .delayUs=0 },
	{ .regAddr= 0x3AE0, .regData= 0x02, .delayUs=0 },
	{ .regAddr= 0x3AEC, .regData= 0x0C, .delayUs=0 },
	{ .regAddr= 0x3B00, .regData= 0x2E, .delayUs=0 },
	{ .regAddr= 0x3B06, .regData= 0x29, .delayUs=0 },
	{ .regAddr= 0x3B98, .regData= 0x25, .delayUs=0 },
	{ .regAddr= 0x3B99, .regData= 0x21, .delayUs=0 },
	{ .regAddr= 0x3B9B, .regData= 0x13, .delayUs=0 },
	{ .regAddr= 0x3B9C, .regData= 0x13, .delayUs=0 },
	{ .regAddr= 0x3B9D, .regData= 0x13, .delayUs=0 },
	{ .regAddr= 0x3B9E, .regData= 0x13, .delayUs=0 },
	{ .regAddr= 0x3BA1, .regData= 0x00, .delayUs=0 },
	{ .regAddr= 0x3BA2, .regData= 0x06, .delayUs=0 },
	{ .regAddr= 0x3BA3, .regData= 0x0B, .delayUs=0 },
	{ .regAddr= 0x3BA4, .regData= 0x10, .delayUs=0 },
	{ .regAddr= 0x3BA5, .regData= 0x14, .delayUs=0 },
	{ .regAddr= 0x3BA6, .regData= 0x18, .delayUs=0 },
	{ .regAddr= 0x3BA7, .regData= 0x1A, .delayUs=0 },
	{ .regAddr= 0x3BA8, .regData= 0x1A, .delayUs=0 },
	{ .regAddr= 0x3BA9, .regData= 0x1A, .delayUs=0 },
	{ .regAddr= 0x3BAC, .regData= 0xED, .delayUs=0 },
	{ .regAddr= 0x3BAD, .regData= 0x01, .delayUs=0 },
	{ .regAddr= 0x3BAE, .regData= 0xF6, .delayUs=0 },
	{ .regAddr= 0x3BAF, .regData= 0x02, .delayUs=0 },
	{ .regAddr= 0x3BB0, .regData= 0xA2, .delayUs=0 },
	{ .regAddr= 0x3BB1, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x3BB2, .regData= 0xE0, .delayUs=0 },
	{ .regAddr= 0x3BB3, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x3BB4, .regData= 0xE0, .delayUs=0 },
	{ .regAddr= 0x3BB5, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x3BB6, .regData= 0xE0, .delayUs=0 },
	{ .regAddr= 0x3BB7, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x3BB8, .regData= 0xE0, .delayUs=0 },
	{ .regAddr= 0x3BBA, .regData= 0xE0, .delayUs=0 },
	{ .regAddr= 0x3BBC, .regData= 0xDA, .delayUs=0 },
	{ .regAddr= 0x3BBE, .regData= 0x88, .delayUs=0 },
	{ .regAddr= 0x3BC0, .regData= 0x44, .delayUs=0 },
	{ .regAddr= 0x3BC2, .regData= 0x7B, .delayUs=0 },
	{ .regAddr= 0x3BC4, .regData= 0xA2, .delayUs=0 },
	{ .regAddr= 0x3BC8, .regData= 0xBD, .delayUs=0 },
	{ .regAddr= 0x3BCA, .regData= 0xBD, .delayUs=0 },

	{ .regAddr= 0x4001, .regData= 0x03, .delayUs=0 }, // LANEMODE[2:0]
	{ .regAddr= 0x4004, .regData= 0x48, .delayUs=0 }, // TXCLKESC_FREQ[15:0]
	{ .regAddr= 0x4005, .regData= 0x09, .delayUs=0 }, //
	{ .regAddr= 0x400C, .regData= 0x00, .delayUs=0 }, // INCKSEL6

	{ .regAddr= 0x4018, .regData= 0x7f, .delayUs=0 }, // TCLKPOST[15:0]
	{ .regAddr= 0x4019, .regData= 0x00, .delayUs=0 }, // TCLKPOST[15:0]
	{ .regAddr= 0x401A, .regData= 0x37, .delayUs=0 }, // TCLKPREPARE[15:0]
	{ .regAddr= 0x401B, .regData= 0x00, .delayUs=0 }, // TCLKPREPARE[15:0]
	{ .regAddr= 0x401C, .regData= 0x37, .delayUs=0 }, // TCLKTRAI[15:0]
	{ .regAddr= 0x401D, .regData= 0x00, .delayUs=0 }, // TCLKTRAI[15:0]
	{ .regAddr= 0x401E, .regData= 0xF7, .delayUs=0 }, // TCLKZERO[15:0]
	{ .regAddr= 0x401F, .regData= 0x00, .delayUs=0 }, // TCLKZERO[15:0]
	{ .regAddr= 0x4020, .regData= 0x3F, .delayUs=0 }, // THSPREPARE[15:0]
	{ .regAddr= 0x4021, .regData= 0x00, .delayUs=0 }, // THSPREPARE[15:0]
	{ .regAddr= 0x4022, .regData= 0x6F, .delayUs=0 }, // THSZERO[15:0]
	{ .regAddr= 0x4023, .regData= 0x00, .delayUs=0 }, // THSZERO[15:0]
	{ .regAddr= 0x4024, .regData= 0x3F, .delayUs=0 }, // THSTRAI[15:0]
	{ .regAddr= 0x4025, .regData= 0x00, .delayUs=0 }, // THSTRAI[15:0]
	{ .regAddr= 0x4026, .regData= 0x5f, .delayUs=0 }, // THSEXIT[15:0]
	{ .regAddr= 0x4027, .regData= 0x00, .delayUs=0 }, // THSEXIT[15:0]
	{ .regAddr= 0x4028, .regData= 0x2f, .delayUs=0 }, // TLPX[15:0]
	{ .regAddr= 0x4029, .regData= 0x00, .delayUs=0 }, // TLPX[15:0]
	{ .regAddr= 0x4074, .regData= 0x01, .delayUs=0 }, // INCKSEL7 [2:0]
    { .regAddr = 0x3000, .regData = 0x00, .delayUs = 24000}, /* leave standby */
};
//CV2003-AAQR All-pixel scan CSI-2_2lane 37.125Mhz AD:12bit Output:12bit 594Mbps Master Mode 11.201fps Integration Time 10.01ms Gain:6dB
static drv_sensor_reg_info_t  cv2003_normal_setting[] =
{
	{ .regAddr= 0x3000, .regData= 0x01, .delayUs=0 }, /* enter standby */
	{ .regAddr= 0x3002, .regData= 0x01, .delayUs=0 }, /* master stop */
	{ .regAddr= 0x3008, .regData= 0x7F, .delayUs=0 }, // BCWAIT_TIME[9:0]
	{ .regAddr= 0x3009, .regData= 0x00, .delayUs=0 }, // BCWAIT_TIME[9:0]
	{ .regAddr= 0x300A, .regData= 0x5B, .delayUs=0 }, // CPWAIT_TIME[9:0]
	{ .regAddr= 0x300B, .regData= 0x00, .delayUs=0 }, // CPWAIT_TIME[9:0]
	{ .regAddr= 0x301c, .regData= 0x0, .delayUs=0 },  //WINMODE
	{ .regAddr= 0x3022, .regData= 0x0, .delayUs=0 },  //ADDMODE
	//{ .regAddr= 0x3024, .regData= 0xC1, .delayUs=0 }, // VMAX[19:0]
	{ .regAddr= 0x3024, .regData= 0xCA, .delayUs=0 }, // VMAX[19:0]
	{ .regAddr= 0x3025, .regData= 0x08, .delayUs=0 }, // VMAX[19:0]
	{ .regAddr= 0x3026, .regData= 0x00, .delayUs=0 }, // VMAX[19:0]
	{ .regAddr= 0x3028, .regData= 0xE4, .delayUs=0 }, // HMAX[15:0]
	{ .regAddr= 0x3029, .regData= 0x0C, .delayUs=0 }, // HMAX[15:0]
	//{ .regAddr= 0x3028, .regData= 0xc8, .delayUs=0 }, // HMAX[15:0]
	//{ .regAddr= 0x3029, .regData= 0x19, .delayUs=0 }, // HMAX[15:0]
    { .regAddr= 0x3030, .regData = 0x0, .delayUs = 0x0},
	{ .regAddr= 0x3031, .regData= 0x01, .delayUs=0 }, // default:12 bit
	{ .regAddr= 0x3032, .regData= 0x01, .delayUs=0 }, // default:12 bit
	{ .regAddr= 0x3033, .regData= 0x07, .delayUs=0 }, // SYS_MODE[3:0]
	{ .regAddr= 0x3050, .regData= 0xC6, .delayUs=0 }, // SHR0[19:0]
	{ .regAddr= 0x3051, .regData= 0x07, .delayUs=0 }, // SHR0[19:0]
	{ .regAddr= 0x3052, .regData= 0x00, .delayUs=0 }, // SHR0[19:0]
	//{ .regAddr= 0x3050, .regData= 0x66, .delayUs=0 }, // SHR0[19:0]
	//{ .regAddr= 0x3051, .regData= 0x00, .delayUs=0 }, //
	//{ .regAddr= 0x3052, .regData= 0x00, .delayUs=0 }, //
	//{ .regAddr= 0x3090, .regData= 0x14, .delayUs=0 }, // GAIN_PCG_0[8:0]
	//{ .regAddr= 0x3091, .regData= 0x00, .delayUs=0 }, // GAIN_PCG_0[8:0]
	{ .regAddr= 0x3090, .regData= 0x00, .delayUs=0 }, // GAIN_PCG_0[8:0]
	{ .regAddr= 0x3091, .regData= 0x00, .delayUs=0 }, // GAIN_PCG_0[8:0]
	{ .regAddr= 0x30C1, .regData= 0x00, .delayUs=0 }, // XVS_DRV[1:0]
	{ .regAddr= 0x3115, .regData= 0x00, .delayUs=0 }, // INCKSEL1[7:0]
	{ .regAddr= 0x3116, .regData= 0x24, .delayUs=0 }, // INCKSEL2[7:0]
	{ .regAddr= 0x3118, .regData= 0x80, .delayUs=0 }, // INCKSEL3[10:0]
	{ .regAddr= 0x3119, .regData= 0x00, .delayUs=0 }, // INCKSEL3[10:0]
	{ .regAddr= 0x311A, .regData= 0xE0, .delayUs=0 }, // INCKSEL4[10:0]
	{ .regAddr= 0x311B, .regData= 0x00, .delayUs=0 }, // INCKSEL4[10:0]
	{ .regAddr= 0x311E, .regData= 0x24, .delayUs=0 }, // INCKSEL5[7:0]
	{ .regAddr= 0x32D4, .regData= 0x21, .delayUs=0 },
	{ .regAddr= 0x32EC, .regData= 0xA1, .delayUs=0 },
	/*
	{ .regAddr= 0x344C, .regData= 0x2B, .delayUs=0 },
	{ .regAddr= 0x344D, .regData= 0x01, .delayUs=0 },
	{ .regAddr= 0x344E, .regData= 0xED, .delayUs=0 },
	{ .regAddr= 0x344F, .regData= 0x01, .delayUs=0 },
	{ .regAddr= 0x3450, .regData= 0xF6, .delayUs=0 },
	{ .regAddr= 0x3451, .regData= 0x02, .delayUs=0 },
	*/
	{ .regAddr= 0x3452, .regData= 0x7F, .delayUs=0 },
	{ .regAddr= 0x3453, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x358A, .regData= 0x04, .delayUs=0 },
	{ .regAddr= 0x35A1, .regData= 0x02, .delayUs=0 },
	/*
	{ .regAddr= 0x35EC, .regData= 0x27, .delayUs=0 },
	{ .regAddr= 0x35EE, .regData= 0x8D, .delayUs=0 },
	{ .regAddr= 0x35F0, .regData= 0x8D, .delayUs=0 },
	{ .regAddr= 0x35F2, .regData= 0x29, .delayUs=0 },
	*/
	{ .regAddr= 0x36BC, .regData= 0x0C, .delayUs=0 },
	{ .regAddr= 0x36CC, .regData= 0x53, .delayUs=0 },
	{ .regAddr= 0x36CD, .regData= 0x00, .delayUs=0 },
	{ .regAddr= 0x36CE, .regData= 0x3C, .delayUs=0 },
	{ .regAddr= 0x36D0, .regData= 0x8C, .delayUs=0 },
	{ .regAddr= 0x36D1, .regData= 0x00, .delayUs=0 },
	{ .regAddr= 0x36D2, .regData= 0x71, .delayUs=0 },
	{ .regAddr= 0x36D4, .regData= 0x3C, .delayUs=0 },
	{ .regAddr= 0x36D6, .regData= 0x53, .delayUs=0 },
	{ .regAddr= 0x36D7, .regData= 0x00, .delayUs=0 },
	{ .regAddr= 0x36D8, .regData= 0x71, .delayUs=0 },
	{ .regAddr= 0x36DA, .regData= 0x8C, .delayUs=0 },
	{ .regAddr= 0x36DB, .regData= 0x00, .delayUs=0 },
	{ .regAddr= 0x3701, .regData= 0x03, .delayUs=0 },  //AD 0:10bit, 3:12bit
	//{ .regAddr= 0x3720, .regData= 0x00, .delayUs=0 },
	{ .regAddr= 0x3724, .regData= 0x02, .delayUs=0 },
	{ .regAddr= 0x3726, .regData= 0x02, .delayUs=0 },
	{ .regAddr= 0x3732, .regData= 0x02, .delayUs=0 },
	{ .regAddr= 0x3734, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x3736, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x3742, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x3862, .regData= 0xE0, .delayUs=0 },
	{ .regAddr= 0x38CC, .regData= 0x30, .delayUs=0 },
	{ .regAddr= 0x38CD, .regData= 0x2F, .delayUs=0 },
	{ .regAddr= 0x395C, .regData= 0x0C, .delayUs=0 },
	/*
	{ .regAddr= 0x39A4, .regData= 0x07, .delayUs=0 },
	{ .regAddr= 0x39A8, .regData= 0x32, .delayUs=0 },
	{ .regAddr= 0x39AA, .regData= 0x32, .delayUs=0 },
	{ .regAddr= 0x39AC, .regData= 0x32, .delayUs=0 },
	{ .regAddr= 0x39AE, .regData= 0x32, .delayUs=0 },
	{ .regAddr= 0x39B0, .regData= 0x32, .delayUs=0 },
	{ .regAddr= 0x39B2, .regData= 0x2F, .delayUs=0 },
	{ .regAddr= 0x39B4, .regData= 0x2D, .delayUs=0 },
	{ .regAddr= 0x39B6, .regData= 0x28, .delayUs=0 },
	{ .regAddr= 0x39B8, .regData= 0x30, .delayUs=0 },
	{ .regAddr= 0x39BA, .regData= 0x30, .delayUs=0 },
	{ .regAddr= 0x39BC, .regData= 0x30, .delayUs=0 },
	{ .regAddr= 0x39BE, .regData= 0x30, .delayUs=0 },
	{ .regAddr= 0x39C0, .regData= 0x30, .delayUs=0 },
	{ .regAddr= 0x39C2, .regData= 0x2E, .delayUs=0 },
	{ .regAddr= 0x39C4, .regData= 0x2B, .delayUs=0 },
	{ .regAddr= 0x39C6, .regData= 0x25, .delayUs=0 },
	*/
	{ .regAddr= 0x3A42, .regData= 0xD1, .delayUs=0 },
	{ .regAddr= 0x3A4C, .regData= 0x77, .delayUs=0 },
	{ .regAddr= 0x3AE0, .regData= 0x02, .delayUs=0 },
	{ .regAddr= 0x3AEC, .regData= 0x0C, .delayUs=0 },
	{ .regAddr= 0x3B00, .regData= 0x2E, .delayUs=0 },
	{ .regAddr= 0x3B06, .regData= 0x29, .delayUs=0 },
	{ .regAddr= 0x3B98, .regData= 0x25, .delayUs=0 },
	{ .regAddr= 0x3B99, .regData= 0x21, .delayUs=0 },
	{ .regAddr= 0x3B9B, .regData= 0x13, .delayUs=0 },
	{ .regAddr= 0x3B9C, .regData= 0x13, .delayUs=0 },
	{ .regAddr= 0x3B9D, .regData= 0x13, .delayUs=0 },
	{ .regAddr= 0x3B9E, .regData= 0x13, .delayUs=0 },
	{ .regAddr= 0x3BA1, .regData= 0x00, .delayUs=0 },
	{ .regAddr= 0x3BA2, .regData= 0x06, .delayUs=0 },
	{ .regAddr= 0x3BA3, .regData= 0x0B, .delayUs=0 },
	{ .regAddr= 0x3BA4, .regData= 0x10, .delayUs=0 },
	{ .regAddr= 0x3BA5, .regData= 0x14, .delayUs=0 },
	{ .regAddr= 0x3BA6, .regData= 0x18, .delayUs=0 },
	{ .regAddr= 0x3BA7, .regData= 0x1A, .delayUs=0 },
	{ .regAddr= 0x3BA8, .regData= 0x1A, .delayUs=0 },
	{ .regAddr= 0x3BA9, .regData= 0x1A, .delayUs=0 },
	{ .regAddr= 0x3BAC, .regData= 0xED, .delayUs=0 },
	{ .regAddr= 0x3BAD, .regData= 0x01, .delayUs=0 },
	{ .regAddr= 0x3BAE, .regData= 0xF6, .delayUs=0 },
	{ .regAddr= 0x3BAF, .regData= 0x02, .delayUs=0 },
	{ .regAddr= 0x3BB0, .regData= 0xA2, .delayUs=0 },
	{ .regAddr= 0x3BB1, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x3BB2, .regData= 0xE0, .delayUs=0 },
	{ .regAddr= 0x3BB3, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x3BB4, .regData= 0xE0, .delayUs=0 },
	{ .regAddr= 0x3BB5, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x3BB6, .regData= 0xE0, .delayUs=0 },
	{ .regAddr= 0x3BB7, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x3BB8, .regData= 0xE0, .delayUs=0 },
	{ .regAddr= 0x3BBA, .regData= 0xE0, .delayUs=0 },
	{ .regAddr= 0x3BBC, .regData= 0xDA, .delayUs=0 },
	{ .regAddr= 0x3BBE, .regData= 0x88, .delayUs=0 },
	{ .regAddr= 0x3BC0, .regData= 0x44, .delayUs=0 },
	{ .regAddr= 0x3BC2, .regData= 0x7B, .delayUs=0 },
	{ .regAddr= 0x3BC4, .regData= 0xA2, .delayUs=0 },
	{ .regAddr= 0x3BC8, .regData= 0xBD, .delayUs=0 },
	{ .regAddr= 0x3BCA, .regData= 0xBD, .delayUs=0 },
	{ .regAddr= 0x4001, .regData= 0x01, .delayUs=0 }, // LANEMODE[2:0]
	{ .regAddr= 0x4004, .regData= 0x48, .delayUs=0 }, // TXCLKESC_FREQ[15:0]
	{ .regAddr= 0x4005, .regData= 0x09, .delayUs=0 }, //
	{ .regAddr= 0x400C, .regData= 0x00, .delayUs=0 }, // INCKSEL6
	{ .regAddr= 0x4018, .regData= 0x67, .delayUs=0 }, // TCLKPOST[15:0]
	{ .regAddr= 0x4019, .regData= 0x00, .delayUs=0 }, // TCLKPOST[15:0]
	{ .regAddr= 0x401A, .regData= 0x27, .delayUs=0 }, // TCLKPREPARE[15:0]
	{ .regAddr= 0x401C, .regData= 0x27, .delayUs=0 }, // TCLKTRAI[15:0]
	{ .regAddr= 0x401E, .regData= 0xB7, .delayUs=0 }, // TCLKZERO[15:0]
	{ .regAddr= 0x401F, .regData= 0x00, .delayUs=0 }, //
	{ .regAddr= 0x4020, .regData= 0x2F, .delayUs=0 }, // THSPREPARE[15:0]
	{ .regAddr= 0x4022, .regData= 0x4F, .delayUs=0 }, // THSZERO[15:0]
	{ .regAddr= 0x4024, .regData= 0x2F, .delayUs=0 }, // THSTRAI[15:0]
	{ .regAddr= 0x4026, .regData= 0x47, .delayUs=0 }, // THSEXIT[15:0]
	{ .regAddr= 0x4028, .regData= 0x27, .delayUs=0 }, // TLPX[15:0]
	{ .regAddr= 0x4074, .regData= 0x01, .delayUs=0 }, // INCKSEL7 [2:0]
    { .regAddr = 0x3000, .regData = 0x00, .delayUs = 24000}, /* standby */
};

static drv_sensor_reg_info_t  cv2003_1080p_setting[] =
{
	{ .regAddr= 0x3000, .regData= 0x01, .delayUs=0 }, /* enter standby */
	{ .regAddr= 0x3003, .regData= 0x01, .delayUs=0 }, /* slave mode */
	{ .regAddr= 0x3002, .regData= 0x01, .delayUs=0 }, /* master stop */
	{ .regAddr= 0x3008, .regData= 0x7F, .delayUs=0 }, // BCWAIT_TIME[9:0]
	{ .regAddr= 0x3009, .regData= 0x00, .delayUs=0 }, // BCWAIT_TIME[9:0]
	{ .regAddr= 0x300A, .regData= 0x5B, .delayUs=0 }, // CPWAIT_TIME[9:0]
	{ .regAddr= 0x300B, .regData= 0x00, .delayUs=0 }, // CPWAIT_TIME[9:0]
	{ .regAddr= 0x301c, .regData= 0x0, .delayUs=0 },  //WINMODE
	{ .regAddr= 0x3020, .regData= 0x1, .delayUs=0 },  //horizontal 2 binning
	{ .regAddr= 0x3021, .regData= 0x1, .delayUs=0 },  //veritical 2 binning
	{ .regAddr= 0x3022, .regData= 0x1, .delayUs=0 },  //ADDMODE 2/2-line binning
	{ .regAddr= 0x3024, .regData= 0xCA, .delayUs=0 }, // VMAX[19:0]
	{ .regAddr= 0x3025, .regData= 0x08, .delayUs=0 }, // VMAX[19:0]
	{ .regAddr= 0x3026, .regData= 0x00, .delayUs=0 }, // VMAX[19:0]

	{ .regAddr= 0x3028, .regData= 0xE4, .delayUs=0 }, // HMAX[15:0]
	{ .regAddr= 0x3029, .regData= 0x0C, .delayUs=0 }, // HMAX[15:0]
    { .regAddr= 0x3030, .regData = 0x0, .delayUs = 0x0},  //invert
	{ .regAddr= 0x3031, .regData= 0x00, .delayUs=0 }, // AD:10 bit
	{ .regAddr= 0x3032, .regData= 0x01, .delayUs=0 }, // MD:12 bit
	{ .regAddr= 0x3033, .regData= 0x07, .delayUs=0 }, // SYS_MODE[3:0]
	{ .regAddr= 0x3050, .regData= 0xC6, .delayUs=0 }, // SHR0[19:0]
	{ .regAddr= 0x3051, .regData= 0x07, .delayUs=0 }, // SHR0[19:0]
	{ .regAddr= 0x3052, .regData= 0x00, .delayUs=0 }, // SHR0[19:0]
	//{ .regAddr= 0x3050, .regData= 0x66, .delayUs=0 }, // SHR0[19:0]
	//{ .regAddr= 0x3051, .regData= 0x00, .delayUs=0 }, //
	//{ .regAddr= 0x3052, .regData= 0x00, .delayUs=0 }, //
	//{ .regAddr= 0x3090, .regData= 0x14, .delayUs=0 }, // GAIN_PCG_0[8:0]
	//{ .regAddr= 0x3091, .regData= 0x00, .delayUs=0 }, // GAIN_PCG_0[8:0]
	{ .regAddr= 0x3090, .regData= 0x00, .delayUs=0 }, // GAIN_PCG_0[8:0]
	{ .regAddr= 0x3091, .regData= 0x00, .delayUs=0 }, // GAIN_PCG_0[8:0]

	{ .regAddr= 0x30C0, .regData= 0x2a, .delayUs=0,}, //vsync/hsync Output
	{ .regAddr= 0x30C1, .regData= 0x0f, .delayUs=0 }, // XVS_DRV[1:0] master=0, slave=0xf
	{ .regAddr= 0x30CC, .regData= 0x00, .delayUs=0 }, //XVSLNG
	{ .regAddr= 0x30CD, .regData= 0x00, .delayUs=0 }, //XHSLNG

	{ .regAddr= 0x30D9, .regData= 0x02, .delayUs=0 }, // DIG_CLP_VSTAET[4:0]
	{ .regAddr= 0x30DA, .regData= 0x01, .delayUs=0 }, // DIG_VLP_VNUM[1:0]

	{ .regAddr= 0x3115, .regData= 0x00, .delayUs=0 }, // INCKSEL1[7:0]
	{ .regAddr= 0x3116, .regData= 0x24, .delayUs=0 }, // INCKSEL2[7:0]
	{ .regAddr= 0x3118, .regData= 0x80, .delayUs=0 }, // INCKSEL3[10:0]
	{ .regAddr= 0x3119, .regData= 0x00, .delayUs=0 }, // INCKSEL3[10:0]
	{ .regAddr= 0x311A, .regData= 0xE0, .delayUs=0 }, // INCKSEL4[10:0]
	{ .regAddr= 0x311B, .regData= 0x00, .delayUs=0 }, // INCKSEL4[10:0]
	{ .regAddr= 0x311E, .regData= 0x24, .delayUs=0 }, // INCKSEL5[7:0]
	{ .regAddr= 0x32D4, .regData= 0x21, .delayUs=0 },
	{ .regAddr= 0x32EC, .regData= 0xA1, .delayUs=0 },
	/*
	{ .regAddr= 0x344C, .regData= 0x2B, .delayUs=0 },
	{ .regAddr= 0x344D, .regData= 0x01, .delayUs=0 },
	{ .regAddr= 0x344E, .regData= 0xED, .delayUs=0 },
	{ .regAddr= 0x344F, .regData= 0x01, .delayUs=0 },
	{ .regAddr= 0x3450, .regData= 0xF6, .delayUs=0 },
	{ .regAddr= 0x3451, .regData= 0x02, .delayUs=0 },
	*/
	{ .regAddr= 0x3452, .regData= 0x7F, .delayUs=0 },
	{ .regAddr= 0x3453, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x358A, .regData= 0x04, .delayUs=0 },
	{ .regAddr= 0x35A1, .regData= 0x02, .delayUs=0 },
	/*
	{ .regAddr= 0x35EC, .regData= 0x27, .delayUs=0 },
	{ .regAddr= 0x35EE, .regData= 0x8D, .delayUs=0 },
	{ .regAddr= 0x35F0, .regData= 0x8D, .delayUs=0 },
	{ .regAddr= 0x35F2, .regData= 0x29, .delayUs=0 },
	*/
	{ .regAddr= 0x36BC, .regData= 0x0C, .delayUs=0 },
	{ .regAddr= 0x36CC, .regData= 0x53, .delayUs=0 },
	{ .regAddr= 0x36CD, .regData= 0x00, .delayUs=0 },
	{ .regAddr= 0x36CE, .regData= 0x3C, .delayUs=0 },
	{ .regAddr= 0x36D0, .regData= 0x8C, .delayUs=0 },
	{ .regAddr= 0x36D1, .regData= 0x00, .delayUs=0 },
	{ .regAddr= 0x36D2, .regData= 0x71, .delayUs=0 },
	{ .regAddr= 0x36D4, .regData= 0x3C, .delayUs=0 },
	{ .regAddr= 0x36D6, .regData= 0x53, .delayUs=0 },
	{ .regAddr= 0x36D7, .regData= 0x00, .delayUs=0 },
	{ .regAddr= 0x36D8, .regData= 0x71, .delayUs=0 },
	{ .regAddr= 0x36DA, .regData= 0x8C, .delayUs=0 },
	{ .regAddr= 0x36DB, .regData= 0x00, .delayUs=0 },
	{ .regAddr= 0x3701, .regData= 0x00, .delayUs=0 },  //AD 0:10bit, 3:12bit
	//{ .regAddr= 0x3720, .regData= 0x00, .delayUs=0 },
	{ .regAddr= 0x3724, .regData= 0x02, .delayUs=0 },
	{ .regAddr= 0x3726, .regData= 0x02, .delayUs=0 },
	{ .regAddr= 0x3732, .regData= 0x02, .delayUs=0 },
	{ .regAddr= 0x3734, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x3736, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x3742, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x3862, .regData= 0xE0, .delayUs=0 },
	{ .regAddr= 0x38CC, .regData= 0x30, .delayUs=0 },
	{ .regAddr= 0x38CD, .regData= 0x2F, .delayUs=0 },
	{ .regAddr= 0x395C, .regData= 0x0C, .delayUs=0 },
	/*
	{ .regAddr= 0x39A4, .regData= 0x07, .delayUs=0 },
	{ .regAddr= 0x39A8, .regData= 0x32, .delayUs=0 },
	{ .regAddr= 0x39AA, .regData= 0x32, .delayUs=0 },
	{ .regAddr= 0x39AC, .regData= 0x32, .delayUs=0 },
	{ .regAddr= 0x39AE, .regData= 0x32, .delayUs=0 },
	{ .regAddr= 0x39B0, .regData= 0x32, .delayUs=0 },
	{ .regAddr= 0x39B2, .regData= 0x2F, .delayUs=0 },
	{ .regAddr= 0x39B4, .regData= 0x2D, .delayUs=0 },
	{ .regAddr= 0x39B6, .regData= 0x28, .delayUs=0 },
	{ .regAddr= 0x39B8, .regData= 0x30, .delayUs=0 },
	{ .regAddr= 0x39BA, .regData= 0x30, .delayUs=0 },
	{ .regAddr= 0x39BC, .regData= 0x30, .delayUs=0 },
	{ .regAddr= 0x39BE, .regData= 0x30, .delayUs=0 },
	{ .regAddr= 0x39C0, .regData= 0x30, .delayUs=0 },
	{ .regAddr= 0x39C2, .regData= 0x2E, .delayUs=0 },
	{ .regAddr= 0x39C4, .regData= 0x2B, .delayUs=0 },
	{ .regAddr= 0x39C6, .regData= 0x25, .delayUs=0 },
	*/
	{ .regAddr= 0x3A42, .regData= 0xD1, .delayUs=0 },
	{ .regAddr= 0x3A4C, .regData= 0x77, .delayUs=0 },
	{ .regAddr= 0x3AE0, .regData= 0x02, .delayUs=0 },
	{ .regAddr= 0x3AEC, .regData= 0x0C, .delayUs=0 },
	{ .regAddr= 0x3B00, .regData= 0x2E, .delayUs=0 },
	{ .regAddr= 0x3B06, .regData= 0x29, .delayUs=0 },
	{ .regAddr= 0x3B98, .regData= 0x25, .delayUs=0 },
	{ .regAddr= 0x3B99, .regData= 0x21, .delayUs=0 },
	{ .regAddr= 0x3B9B, .regData= 0x13, .delayUs=0 },
	{ .regAddr= 0x3B9C, .regData= 0x13, .delayUs=0 },
	{ .regAddr= 0x3B9D, .regData= 0x13, .delayUs=0 },
	{ .regAddr= 0x3B9E, .regData= 0x13, .delayUs=0 },
	{ .regAddr= 0x3BA1, .regData= 0x00, .delayUs=0 },
	{ .regAddr= 0x3BA2, .regData= 0x06, .delayUs=0 },
	{ .regAddr= 0x3BA3, .regData= 0x0B, .delayUs=0 },
	{ .regAddr= 0x3BA4, .regData= 0x10, .delayUs=0 },
	{ .regAddr= 0x3BA5, .regData= 0x14, .delayUs=0 },
	{ .regAddr= 0x3BA6, .regData= 0x18, .delayUs=0 },
	{ .regAddr= 0x3BA7, .regData= 0x1A, .delayUs=0 },
	{ .regAddr= 0x3BA8, .regData= 0x1A, .delayUs=0 },
	{ .regAddr= 0x3BA9, .regData= 0x1A, .delayUs=0 },
	{ .regAddr= 0x3BAC, .regData= 0xED, .delayUs=0 },
	{ .regAddr= 0x3BAD, .regData= 0x01, .delayUs=0 },
	{ .regAddr= 0x3BAE, .regData= 0xF6, .delayUs=0 },
	{ .regAddr= 0x3BAF, .regData= 0x02, .delayUs=0 },
	{ .regAddr= 0x3BB0, .regData= 0xA2, .delayUs=0 },
	{ .regAddr= 0x3BB1, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x3BB2, .regData= 0xE0, .delayUs=0 },
	{ .regAddr= 0x3BB3, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x3BB4, .regData= 0xE0, .delayUs=0 },
	{ .regAddr= 0x3BB5, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x3BB6, .regData= 0xE0, .delayUs=0 },
	{ .regAddr= 0x3BB7, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x3BB8, .regData= 0xE0, .delayUs=0 },
	{ .regAddr= 0x3BBA, .regData= 0xE0, .delayUs=0 },
	{ .regAddr= 0x3BBC, .regData= 0xDA, .delayUs=0 },
	{ .regAddr= 0x3BBE, .regData= 0x88, .delayUs=0 },
	{ .regAddr= 0x3BC0, .regData= 0x44, .delayUs=0 },
	{ .regAddr= 0x3BC2, .regData= 0x7B, .delayUs=0 },
	{ .regAddr= 0x3BC4, .regData= 0xA2, .delayUs=0 },
	{ .regAddr= 0x3BC8, .regData= 0xBD, .delayUs=0 },
	{ .regAddr= 0x3BCA, .regData= 0xBD, .delayUs=0 },

	{ .regAddr= 0x4001, .regData= 0x01, .delayUs=0 }, // LANEMODE[2:0]
	{ .regAddr= 0x4004, .regData= 0x48, .delayUs=0 }, // TXCLKESC_FREQ[15:0]
	{ .regAddr= 0x4005, .regData= 0x09, .delayUs=0 }, //
	{ .regAddr= 0x400C, .regData= 0x00, .delayUs=0 }, // INCKSEL6

	{ .regAddr= 0x4018, .regData= 0x67, .delayUs=0 }, // TCLKPOST[15:0]
	{ .regAddr= 0x4019, .regData= 0x00, .delayUs=0 }, // TCLKPOST[15:0]
	{ .regAddr= 0x401A, .regData= 0x27, .delayUs=0 }, // TCLKPREPARE[15:0]
	{ .regAddr= 0x401C, .regData= 0x27, .delayUs=0 }, // TCLKTRAI[15:0]
	{ .regAddr= 0x401E, .regData= 0xB7, .delayUs=0 }, // TCLKZERO[15:0]
	{ .regAddr= 0x401F, .regData= 0x00, .delayUs=0 }, //
	{ .regAddr= 0x4020, .regData= 0x2F, .delayUs=0 }, // THSPREPARE[15:0]
	{ .regAddr= 0x4022, .regData= 0x4F, .delayUs=0 }, // THSZERO[15:0]
	{ .regAddr= 0x4024, .regData= 0x2F, .delayUs=0 }, // THSTRAI[15:0]
	{ .regAddr= 0x4026, .regData= 0x47, .delayUs=0 }, // THSEXIT[15:0]
	{ .regAddr= 0x4028, .regData= 0x27, .delayUs=0 }, // TLPX[15:0]
	{ .regAddr= 0x4074, .regData= 0x01, .delayUs=0 }, // INCKSEL7 [2:0]
    { .regAddr = 0x3000, .regData = 0x00, .delayUs = 24000}, /* leave standby */
};

//CV2003-AAQR All-pixel scan CSI-2_4lane 37.125Mhz AD:10bit Output:12bit 891Mbps Master Mode fps Integration Time 1.55ms Gain:6dB
static drv_sensor_reg_info_t  cv2003_1080p_30fps_setting[] =
{
	{ .regAddr= 0x3000, .regData= 0x01, .delayUs=0 }, /* enter standby */
	{ .regAddr= 0x3003, .regData= 0x01, .delayUs=0 }, /* slave mode */
	{ .regAddr= 0x3002, .regData= 0x01, .delayUs=0 }, /* master stop */
	{ .regAddr= 0x3008, .regData= 0x7F, .delayUs=0 }, // BCWAIT_TIME[9:0]
	{ .regAddr= 0x3009, .regData= 0x00, .delayUs=0 }, // BCWAIT_TIME[9:0]
	{ .regAddr= 0x300A, .regData= 0x5B, .delayUs=0 }, // CPWAIT_TIME[9:0]
	{ .regAddr= 0x300B, .regData= 0x00, .delayUs=0 }, // CPWAIT_TIME[9:0]
	{ .regAddr= 0x301c, .regData= 0x0, .delayUs=0 },  //WINMODE
	{ .regAddr= 0x3020, .regData= 0x1, .delayUs=0 },  //horizontal 2 binning
	{ .regAddr= 0x3021, .regData= 0x1, .delayUs=0 },  //veritical 2 binning
	{ .regAddr= 0x3022, .regData= 0x1, .delayUs=0 },  //ADDMODE 2/2-line binning
	{ .regAddr= 0x3024, .regData= 0xCA, .delayUs=0 }, // VMAX[19:0]
	{ .regAddr= 0x3025, .regData= 0x08, .delayUs=0 }, // VMAX[19:0]
	{ .regAddr= 0x3026, .regData= 0x00, .delayUs=0 }, // VMAX[19:0]

	{ .regAddr= 0x3028, .regData= 0x4c, .delayUs=0 }, // HMAX[15:0]
	{ .regAddr= 0x3029, .regData= 0x04, .delayUs=0 }, // HMAX[15:0]
    { .regAddr= 0x3030, .regData= 0x00, .delayUs=0 },  //normal
	{ .regAddr= 0x3031, .regData= 0x00, .delayUs=0 }, // AD:10 bit
	{ .regAddr= 0x3032, .regData= 0x01, .delayUs=0 }, // MD:12 bit
	{ .regAddr= 0x3033, .regData= 0x05, .delayUs=0 }, // SYS_MODE[3:0]

	{ .regAddr= 0x302c, .regData= 0x00, .delayUs=0 }, // WDMODE[1:0]
	{ .regAddr= 0x302d, .regData= 0x00, .delayUs=0 }, // WDSEL[1:0]
	{ .regAddr= 0x30CF, .regData= 0x00, .delayUs=0 }, // XVSMSKCNT_INT[1:0]

	{ .regAddr= 0x3050, .regData= 0xC6, .delayUs=0 }, // SHR0[19:0]
	{ .regAddr= 0x3051, .regData= 0x02, .delayUs=0 }, // SHR0[19:0]
	{ .regAddr= 0x3052, .regData= 0x00, .delayUs=0 }, // SHR0[19:0]
	//{ .regAddr= 0x3050, .regData= 0x66, .delayUs=0 }, // SHR0[19:0]
	//{ .regAddr= 0x3051, .regData= 0x00, .delayUs=0 }, //
	//{ .regAddr= 0x3052, .regData= 0x00, .delayUs=0 }, //
	//{ .regAddr= 0x3090, .regData= 0x14, .delayUs=0 }, // GAIN_PCG_0[8:0]
	//{ .regAddr= 0x3091, .regData= 0x00, .delayUs=0 }, // GAIN_PCG_0[8:0]
	{ .regAddr= 0x3090, .regData= 0x00, .delayUs=0 }, // GAIN_PCG_0[8:0]
	{ .regAddr= 0x3091, .regData= 0x00, .delayUs=0 }, // GAIN_PCG_0[8:0]

	{ .regAddr= 0x30C0, .regData= 0x2a, .delayUs=0,}, //vsync/hsync Output
	{ .regAddr= 0x30C1, .regData= 0x0f, .delayUs=0 }, // XVS_DRV[1:0] master=0, slave=0xf
	{ .regAddr= 0x30CC, .regData= 0x03, .delayUs=0 }, //XVSLNG
	{ .regAddr= 0x30CD, .regData= 0x00, .delayUs=0 }, //XHSLNG

	{ .regAddr= 0x30D9, .regData= 0x02, .delayUs=0 }, // DIG_CLP_VSTAET[4:0]
	{ .regAddr= 0x30DA, .regData= 0x01, .delayUs=0 }, // DIG_VLP_VNUM[1:0]

	{ .regAddr= 0x3115, .regData= 0x00, .delayUs=0 }, // INCKSEL1[7:0]
	{ .regAddr= 0x3116, .regData= 0x24, .delayUs=0 }, // INCKSEL2[7:0]
	{ .regAddr= 0x3118, .regData= 0xC0, .delayUs=0 }, // INCKSEL3[10:0]
	{ .regAddr= 0x3119, .regData= 0x00, .delayUs=0 }, // INCKSEL3[10:0]
	{ .regAddr= 0x311A, .regData= 0xE0, .delayUs=0 }, // INCKSEL4[10:0]
	{ .regAddr= 0x311B, .regData= 0x00, .delayUs=0 }, // INCKSEL4[10:0]
	{ .regAddr= 0x311E, .regData= 0x24, .delayUs=0 }, // INCKSEL5[7:0]
	{ .regAddr= 0x32D4, .regData= 0x21, .delayUs=0 },
	{ .regAddr= 0x32EC, .regData= 0xA1, .delayUs=0 },
	/*
	{ .regAddr= 0x344C, .regData= 0x2B, .delayUs=0 },
	{ .regAddr= 0x344D, .regData= 0x01, .delayUs=0 },
	{ .regAddr= 0x344E, .regData= 0xED, .delayUs=0 },
	{ .regAddr= 0x344F, .regData= 0x01, .delayUs=0 },
	{ .regAddr= 0x3450, .regData= 0xF6, .delayUs=0 },
	{ .regAddr= 0x3451, .regData= 0x02, .delayUs=0 },
	*/
	{ .regAddr= 0x3452, .regData= 0x7F, .delayUs=0 },
	{ .regAddr= 0x3453, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x358A, .regData= 0x04, .delayUs=0 },
	{ .regAddr= 0x35A1, .regData= 0x02, .delayUs=0 },
	/*
	{ .regAddr= 0x35EC, .regData= 0x27, .delayUs=0 },
	{ .regAddr= 0x35EE, .regData= 0x8D, .delayUs=0 },
	{ .regAddr= 0x35F0, .regData= 0x8D, .delayUs=0 },
	{ .regAddr= 0x35F2, .regData= 0x29, .delayUs=0 },
	*/
	{ .regAddr= 0x36BC, .regData= 0x0C, .delayUs=0 },
	{ .regAddr= 0x36CC, .regData= 0x53, .delayUs=0 },
	{ .regAddr= 0x36CD, .regData= 0x00, .delayUs=0 },
	{ .regAddr= 0x36CE, .regData= 0x3C, .delayUs=0 },
	{ .regAddr= 0x36D0, .regData= 0x8C, .delayUs=0 },
	{ .regAddr= 0x36D1, .regData= 0x00, .delayUs=0 },
	{ .regAddr= 0x36D2, .regData= 0x71, .delayUs=0 },
	{ .regAddr= 0x36D4, .regData= 0x3C, .delayUs=0 },
	{ .regAddr= 0x36D6, .regData= 0x53, .delayUs=0 },
	{ .regAddr= 0x36D7, .regData= 0x00, .delayUs=0 },
	{ .regAddr= 0x36D8, .regData= 0x71, .delayUs=0 },
	{ .regAddr= 0x36DA, .regData= 0x8C, .delayUs=0 },
	{ .regAddr= 0x36DB, .regData= 0x00, .delayUs=0 },
	{ .regAddr= 0x3701, .regData= 0x00, .delayUs=0 },  //AD 0:10bit, 3:12bit
	//{ .regAddr= 0x3720, .regData= 0x00, .delayUs=0 },
	{ .regAddr= 0x3724, .regData= 0x02, .delayUs=0 },
	{ .regAddr= 0x3726, .regData= 0x02, .delayUs=0 },
	{ .regAddr= 0x3732, .regData= 0x02, .delayUs=0 },
	{ .regAddr= 0x3734, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x3736, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x3742, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x3862, .regData= 0xE0, .delayUs=0 },
	{ .regAddr= 0x38CC, .regData= 0x30, .delayUs=0 },
	{ .regAddr= 0x38CD, .regData= 0x2F, .delayUs=0 },
	{ .regAddr= 0x395C, .regData= 0x0C, .delayUs=0 },
	/*
	{ .regAddr= 0x39A4, .regData= 0x07, .delayUs=0 },
	{ .regAddr= 0x39A8, .regData= 0x32, .delayUs=0 },
	{ .regAddr= 0x39AA, .regData= 0x32, .delayUs=0 },
	{ .regAddr= 0x39AC, .regData= 0x32, .delayUs=0 },
	{ .regAddr= 0x39AE, .regData= 0x32, .delayUs=0 },
	{ .regAddr= 0x39B0, .regData= 0x32, .delayUs=0 },
	{ .regAddr= 0x39B2, .regData= 0x2F, .delayUs=0 },
	{ .regAddr= 0x39B4, .regData= 0x2D, .delayUs=0 },
	{ .regAddr= 0x39B6, .regData= 0x28, .delayUs=0 },
	{ .regAddr= 0x39B8, .regData= 0x30, .delayUs=0 },
	{ .regAddr= 0x39BA, .regData= 0x30, .delayUs=0 },
	{ .regAddr= 0x39BC, .regData= 0x30, .delayUs=0 },
	{ .regAddr= 0x39BE, .regData= 0x30, .delayUs=0 },
	{ .regAddr= 0x39C0, .regData= 0x30, .delayUs=0 },
	{ .regAddr= 0x39C2, .regData= 0x2E, .delayUs=0 },
	{ .regAddr= 0x39C4, .regData= 0x2B, .delayUs=0 },
	{ .regAddr= 0x39C6, .regData= 0x25, .delayUs=0 },
	*/
	{ .regAddr= 0x3A42, .regData= 0xD1, .delayUs=0 },
	{ .regAddr= 0x3A4C, .regData= 0x77, .delayUs=0 },
	{ .regAddr= 0x3AE0, .regData= 0x02, .delayUs=0 },
	{ .regAddr= 0x3AEC, .regData= 0x0C, .delayUs=0 },
	{ .regAddr= 0x3B00, .regData= 0x2E, .delayUs=0 },
	{ .regAddr= 0x3B06, .regData= 0x29, .delayUs=0 },
	{ .regAddr= 0x3B98, .regData= 0x25, .delayUs=0 },
	{ .regAddr= 0x3B99, .regData= 0x21, .delayUs=0 },
	{ .regAddr= 0x3B9B, .regData= 0x13, .delayUs=0 },
	{ .regAddr= 0x3B9C, .regData= 0x13, .delayUs=0 },
	{ .regAddr= 0x3B9D, .regData= 0x13, .delayUs=0 },
	{ .regAddr= 0x3B9E, .regData= 0x13, .delayUs=0 },
	{ .regAddr= 0x3BA1, .regData= 0x00, .delayUs=0 },
	{ .regAddr= 0x3BA2, .regData= 0x06, .delayUs=0 },
	{ .regAddr= 0x3BA3, .regData= 0x0B, .delayUs=0 },
	{ .regAddr= 0x3BA4, .regData= 0x10, .delayUs=0 },
	{ .regAddr= 0x3BA5, .regData= 0x14, .delayUs=0 },
	{ .regAddr= 0x3BA6, .regData= 0x18, .delayUs=0 },
	{ .regAddr= 0x3BA7, .regData= 0x1A, .delayUs=0 },
	{ .regAddr= 0x3BA8, .regData= 0x1A, .delayUs=0 },
	{ .regAddr= 0x3BA9, .regData= 0x1A, .delayUs=0 },
	{ .regAddr= 0x3BAC, .regData= 0xED, .delayUs=0 },
	{ .regAddr= 0x3BAD, .regData= 0x01, .delayUs=0 },
	{ .regAddr= 0x3BAE, .regData= 0xF6, .delayUs=0 },
	{ .regAddr= 0x3BAF, .regData= 0x02, .delayUs=0 },
	{ .regAddr= 0x3BB0, .regData= 0xA2, .delayUs=0 },
	{ .regAddr= 0x3BB1, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x3BB2, .regData= 0xE0, .delayUs=0 },
	{ .regAddr= 0x3BB3, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x3BB4, .regData= 0xE0, .delayUs=0 },
	{ .regAddr= 0x3BB5, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x3BB6, .regData= 0xE0, .delayUs=0 },
	{ .regAddr= 0x3BB7, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x3BB8, .regData= 0xE0, .delayUs=0 },
	{ .regAddr= 0x3BBA, .regData= 0xE0, .delayUs=0 },
	{ .regAddr= 0x3BBC, .regData= 0xDA, .delayUs=0 },
	{ .regAddr= 0x3BBE, .regData= 0x88, .delayUs=0 },
	{ .regAddr= 0x3BC0, .regData= 0x44, .delayUs=0 },
	{ .regAddr= 0x3BC2, .regData= 0x7B, .delayUs=0 },
	{ .regAddr= 0x3BC4, .regData= 0xA2, .delayUs=0 },
	{ .regAddr= 0x3BC8, .regData= 0xBD, .delayUs=0 },
	{ .regAddr= 0x3BCA, .regData= 0xBD, .delayUs=0 },

	{ .regAddr= 0x4001, .regData= 0x03, .delayUs=0 }, // LANEMODE[2:0]
	{ .regAddr= 0x4004, .regData= 0x48, .delayUs=0 }, // TXCLKESC_FREQ[15:0]
	{ .regAddr= 0x4005, .regData= 0x09, .delayUs=0 }, //
	{ .regAddr= 0x400C, .regData= 0x00, .delayUs=0 }, // INCKSEL6

	{ .regAddr= 0x4018, .regData= 0x7f, .delayUs=0 }, // TCLKPOST[15:0]
	{ .regAddr= 0x4019, .regData= 0x00, .delayUs=0 }, // TCLKPOST[15:0]
	{ .regAddr= 0x401A, .regData= 0x37, .delayUs=0 }, // TCLKPREPARE[15:0]
	{ .regAddr= 0x401B, .regData= 0x00, .delayUs=0 }, // TCLKPREPARE[15:0]
	{ .regAddr= 0x401C, .regData= 0x37, .delayUs=0 }, // TCLKTRAI[15:0]
	{ .regAddr= 0x401D, .regData= 0x00, .delayUs=0 }, // TCLKTRAI[15:0]
	{ .regAddr= 0x401E, .regData= 0xF7, .delayUs=0 }, // TCLKZERO[15:0]
	{ .regAddr= 0x401F, .regData= 0x00, .delayUs=0 }, // TCLKZERO[15:0]
	{ .regAddr= 0x4020, .regData= 0x3F, .delayUs=0 }, // THSPREPARE[15:0]
	{ .regAddr= 0x4021, .regData= 0x00, .delayUs=0 }, // THSPREPARE[15:0]
	{ .regAddr= 0x4022, .regData= 0x6F, .delayUs=0 }, // THSZERO[15:0]
	{ .regAddr= 0x4023, .regData= 0x00, .delayUs=0 }, // THSZERO[15:0]
	{ .regAddr= 0x4024, .regData= 0x3F, .delayUs=0 }, // THSTRAI[15:0]
	{ .regAddr= 0x4025, .regData= 0x00, .delayUs=0 }, // THSTRAI[15:0]
	{ .regAddr= 0x4026, .regData= 0x5f, .delayUs=0 }, // THSEXIT[15:0]
	{ .regAddr= 0x4027, .regData= 0x00, .delayUs=0 }, // THSEXIT[15:0]
	{ .regAddr= 0x4028, .regData= 0x2f, .delayUs=0 }, // TLPX[15:0]
	{ .regAddr= 0x4029, .regData= 0x00, .delayUs=0 }, // TLPX[15:0]
	{ .regAddr= 0x4074, .regData= 0x01, .delayUs=0 }, // INCKSEL7 [2:0]
    { .regAddr = 0x3000, .regData = 0x00, .delayUs = 24000}, /* leave standby */
};

//CV2003-AAQR All-pixel scan CSI-2_4lane 37.125Mhz AD:12bit Output:12bit 891Mbps Master Mode fps Integration Time xx.xxms Gain:6dB
static drv_sensor_reg_info_t  cv2003_4k_30fps_setting[] =
{
	{ .regAddr= 0x3000, .regData= 0x01, .delayUs=0 }, /* enter standby */
	{ .regAddr= 0x3002, .regData= 0x01, .delayUs=0 }, /* master stop */
	{ .regAddr= 0x3008, .regData= 0x7F, .delayUs=0 }, // BCWAIT_TIME[9:0]
	{ .regAddr= 0x3009, .regData= 0x00, .delayUs=0 }, // BCWAIT_TIME[9:0]
	{ .regAddr= 0x300A, .regData= 0x5B, .delayUs=0 }, // CPWAIT_TIME[9:0]
	{ .regAddr= 0x300B, .regData= 0x00, .delayUs=0 }, // CPWAIT_TIME[9:0]
	{ .regAddr= 0x301c, .regData= 0x0, .delayUs=0 },  //WINMODE
	{ .regAddr= 0x3020, .regData= 0x0, .delayUs=0 },  //horizontal 2 binning
	{ .regAddr= 0x3021, .regData= 0x0, .delayUs=0 },  //veritical 2 binning
	{ .regAddr= 0x3022, .regData= 0x0, .delayUs=0 },  //ADDMODE 2/2-line binning
	{ .regAddr= 0x3024, .regData= 0xCA, .delayUs=0 }, // VMAX[19:0]
	{ .regAddr= 0x3025, .regData= 0x08, .delayUs=0 }, // VMAX[19:0]
	{ .regAddr= 0x3026, .regData= 0x00, .delayUs=0 }, // VMAX[19:0]

	//{ .regAddr= 0x3028, .regData= 0x4c, .delayUs=0 }, // HMAX[15:0]
	//{ .regAddr= 0x3029, .regData= 0x04, .delayUs=0 }, // HMAX[15:0]
//	{ .regAddr= 0x3028, .regData= 0x40, .delayUs=0 }, // HMAX[15:0] 25fps
//	{ .regAddr= 0x3029, .regData= 0x05, .delayUs=0 }, // HMAX[15:0]
	{ .regAddr= 0x3028, .regData= 0xa0, .delayUs=0 }, // HMAX[15:0] 19fps
	{ .regAddr= 0x3029, .regData= 0x06, .delayUs=0 }, // HMAX[15:0]
    { .regAddr= 0x3030, .regData= 0x00, .delayUs=0 },  //normal
	{ .regAddr= 0x3031, .regData= 0x01, .delayUs=0 }, // AD:12 bit
	{ .regAddr= 0x3032, .regData= 0x01, .delayUs=0 }, // MD:12 bit
	{ .regAddr= 0x3033, .regData= 0x05, .delayUs=0 }, // SYS_MODE[3:0]

	{ .regAddr= 0x302c, .regData= 0x00, .delayUs=0 }, // WDMODE[1:0]
	{ .regAddr= 0x302d, .regData= 0x00, .delayUs=0 }, // WDSEL[1:0]
	{ .regAddr= 0x30CF, .regData= 0x00, .delayUs=0 }, // XVSMSKCNT_INT[1:0]
	{ .regAddr= 0x3050, .regData= 0xC6, .delayUs=0 }, // SHR0[19:0]
	{ .regAddr= 0x3051, .regData= 0x02, .delayUs=0 }, // SHR0[19:0]
	{ .regAddr= 0x3052, .regData= 0x00, .delayUs=0 }, // SHR0[19:0]
	{ .regAddr= 0x3090, .regData= 0x00, .delayUs=0 }, // GAIN_PCG_0[8:0]
	{ .regAddr= 0x3091, .regData= 0x00, .delayUs=0 }, // GAIN_PCG_0[8:0]
	{ .regAddr= 0x30C1, .regData= 0x00, .delayUs=0 }, // XVS_DRV[1:0]
	{ .regAddr= 0x3115, .regData= 0x00, .delayUs=0 }, // INCKSEL1[7:0]
	{ .regAddr= 0x3116, .regData= 0x24, .delayUs=0 }, // INCKSEL2[7:0]
	{ .regAddr= 0x3118, .regData= 0xC0, .delayUs=0 }, // INCKSEL3[10:0]
	{ .regAddr= 0x3119, .regData= 0x00, .delayUs=0 }, // INCKSEL3[10:0]
	{ .regAddr= 0x311A, .regData= 0xE0, .delayUs=0 }, // INCKSEL4[10:0]
	{ .regAddr= 0x311B, .regData= 0x00, .delayUs=0 }, // INCKSEL4[10:0]
	{ .regAddr= 0x311E, .regData= 0x24, .delayUs=0 }, // INCKSEL5[7:0]
	{ .regAddr= 0x32D4, .regData= 0x21, .delayUs=0 },
	{ .regAddr= 0x32EC, .regData= 0xA1, .delayUs=0 },
	/*
	{ .regAddr= 0x344C, .regData= 0x2B, .delayUs=0 },
	{ .regAddr= 0x344D, .regData= 0x01, .delayUs=0 },
	{ .regAddr= 0x344E, .regData= 0xED, .delayUs=0 },
	{ .regAddr= 0x344F, .regData= 0x01, .delayUs=0 },
	{ .regAddr= 0x3450, .regData= 0xF6, .delayUs=0 },
	{ .regAddr= 0x3451, .regData= 0x02, .delayUs=0 },
	*/
	{ .regAddr= 0x3452, .regData= 0x7F, .delayUs=0 },
	{ .regAddr= 0x3453, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x358A, .regData= 0x04, .delayUs=0 },
	{ .regAddr= 0x35A1, .regData= 0x02, .delayUs=0 },
	/*
	{ .regAddr= 0x35EC, .regData= 0x27, .delayUs=0 },
	{ .regAddr= 0x35EE, .regData= 0x8D, .delayUs=0 },
	{ .regAddr= 0x35F0, .regData= 0x8D, .delayUs=0 },
	{ .regAddr= 0x35F2, .regData= 0x29, .delayUs=0 },
	*/
	{ .regAddr= 0x36BC, .regData= 0x0C, .delayUs=0 },
	{ .regAddr= 0x36CC, .regData= 0x53, .delayUs=0 },
	{ .regAddr= 0x36CD, .regData= 0x00, .delayUs=0 },
	{ .regAddr= 0x36CE, .regData= 0x3C, .delayUs=0 },
	{ .regAddr= 0x36D0, .regData= 0x8C, .delayUs=0 },
	{ .regAddr= 0x36D1, .regData= 0x00, .delayUs=0 },
	{ .regAddr= 0x36D2, .regData= 0x71, .delayUs=0 },
	{ .regAddr= 0x36D4, .regData= 0x3C, .delayUs=0 },
	{ .regAddr= 0x36D6, .regData= 0x53, .delayUs=0 },
	{ .regAddr= 0x36D7, .regData= 0x00, .delayUs=0 },
	{ .regAddr= 0x36D8, .regData= 0x71, .delayUs=0 },
	{ .regAddr= 0x36DA, .regData= 0x8C, .delayUs=0 },
	{ .regAddr= 0x36DB, .regData= 0x00, .delayUs=0 },
	{ .regAddr= 0x3701, .regData= 0x03, .delayUs=0 },  //AD 0:10bit, 3:12bit
	//{ .regAddr= 0x3720, .regData= 0x00, .delayUs=0 },
	{ .regAddr= 0x3724, .regData= 0x02, .delayUs=0 },
	{ .regAddr= 0x3726, .regData= 0x02, .delayUs=0 },
	{ .regAddr= 0x3732, .regData= 0x02, .delayUs=0 },
	{ .regAddr= 0x3734, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x3736, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x3742, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x3862, .regData= 0xE0, .delayUs=0 },
	{ .regAddr= 0x38CC, .regData= 0x30, .delayUs=0 },
	{ .regAddr= 0x38CD, .regData= 0x2F, .delayUs=0 },
	{ .regAddr= 0x395C, .regData= 0x0C, .delayUs=0 },
	/*
	{ .regAddr= 0x39A4, .regData= 0x07, .delayUs=0 },
	{ .regAddr= 0x39A8, .regData= 0x32, .delayUs=0 },
	{ .regAddr= 0x39AA, .regData= 0x32, .delayUs=0 },
	{ .regAddr= 0x39AC, .regData= 0x32, .delayUs=0 },
	{ .regAddr= 0x39AE, .regData= 0x32, .delayUs=0 },
	{ .regAddr= 0x39B0, .regData= 0x32, .delayUs=0 },
	{ .regAddr= 0x39B2, .regData= 0x2F, .delayUs=0 },
	{ .regAddr= 0x39B4, .regData= 0x2D, .delayUs=0 },
	{ .regAddr= 0x39B6, .regData= 0x28, .delayUs=0 },
	{ .regAddr= 0x39B8, .regData= 0x30, .delayUs=0 },
	{ .regAddr= 0x39BA, .regData= 0x30, .delayUs=0 },
	{ .regAddr= 0x39BC, .regData= 0x30, .delayUs=0 },
	{ .regAddr= 0x39BE, .regData= 0x30, .delayUs=0 },
	{ .regAddr= 0x39C0, .regData= 0x30, .delayUs=0 },
	{ .regAddr= 0x39C2, .regData= 0x2E, .delayUs=0 },
	{ .regAddr= 0x39C4, .regData= 0x2B, .delayUs=0 },
	{ .regAddr= 0x39C6, .regData= 0x25, .delayUs=0 },
	*/
	{ .regAddr= 0x3A42, .regData= 0xD1, .delayUs=0 },
	{ .regAddr= 0x3A4C, .regData= 0x77, .delayUs=0 },
	{ .regAddr= 0x3AE0, .regData= 0x02, .delayUs=0 },
	{ .regAddr= 0x3AEC, .regData= 0x0C, .delayUs=0 },
	{ .regAddr= 0x3B00, .regData= 0x2E, .delayUs=0 },
	{ .regAddr= 0x3B06, .regData= 0x29, .delayUs=0 },
	{ .regAddr= 0x3B98, .regData= 0x25, .delayUs=0 },
	{ .regAddr= 0x3B99, .regData= 0x21, .delayUs=0 },
	{ .regAddr= 0x3B9B, .regData= 0x13, .delayUs=0 },
	{ .regAddr= 0x3B9C, .regData= 0x13, .delayUs=0 },
	{ .regAddr= 0x3B9D, .regData= 0x13, .delayUs=0 },
	{ .regAddr= 0x3B9E, .regData= 0x13, .delayUs=0 },
	{ .regAddr= 0x3BA1, .regData= 0x00, .delayUs=0 },
	{ .regAddr= 0x3BA2, .regData= 0x06, .delayUs=0 },
	{ .regAddr= 0x3BA3, .regData= 0x0B, .delayUs=0 },
	{ .regAddr= 0x3BA4, .regData= 0x10, .delayUs=0 },
	{ .regAddr= 0x3BA5, .regData= 0x14, .delayUs=0 },
	{ .regAddr= 0x3BA6, .regData= 0x18, .delayUs=0 },
	{ .regAddr= 0x3BA7, .regData= 0x1A, .delayUs=0 },
	{ .regAddr= 0x3BA8, .regData= 0x1A, .delayUs=0 },
	{ .regAddr= 0x3BA9, .regData= 0x1A, .delayUs=0 },
	{ .regAddr= 0x3BAC, .regData= 0xED, .delayUs=0 },
	{ .regAddr= 0x3BAD, .regData= 0x01, .delayUs=0 },
	{ .regAddr= 0x3BAE, .regData= 0xF6, .delayUs=0 },
	{ .regAddr= 0x3BAF, .regData= 0x02, .delayUs=0 },
	{ .regAddr= 0x3BB0, .regData= 0xA2, .delayUs=0 },
	{ .regAddr= 0x3BB1, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x3BB2, .regData= 0xE0, .delayUs=0 },
	{ .regAddr= 0x3BB3, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x3BB4, .regData= 0xE0, .delayUs=0 },
	{ .regAddr= 0x3BB5, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x3BB6, .regData= 0xE0, .delayUs=0 },
	{ .regAddr= 0x3BB7, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x3BB8, .regData= 0xE0, .delayUs=0 },
	{ .regAddr= 0x3BBA, .regData= 0xE0, .delayUs=0 },
	{ .regAddr= 0x3BBC, .regData= 0xDA, .delayUs=0 },
	{ .regAddr= 0x3BBE, .regData= 0x88, .delayUs=0 },
	{ .regAddr= 0x3BC0, .regData= 0x44, .delayUs=0 },
	{ .regAddr= 0x3BC2, .regData= 0x7B, .delayUs=0 },
	{ .regAddr= 0x3BC4, .regData= 0xA2, .delayUs=0 },
	{ .regAddr= 0x3BC8, .regData= 0xBD, .delayUs=0 },
	{ .regAddr= 0x3BCA, .regData= 0xBD, .delayUs=0 },

	{ .regAddr= 0x4001, .regData= 0x03, .delayUs=0 }, // LANEMODE[2:0]
	{ .regAddr= 0x4004, .regData= 0x48, .delayUs=0 }, // TXCLKESC_FREQ[15:0]
	{ .regAddr= 0x4005, .regData= 0x09, .delayUs=0 }, //
	{ .regAddr= 0x400C, .regData= 0x00, .delayUs=0 }, // INCKSEL6

	{ .regAddr= 0x4018, .regData= 0x7f, .delayUs=0 }, // TCLKPOST[15:0]
	{ .regAddr= 0x4019, .regData= 0x00, .delayUs=0 }, // TCLKPOST[15:0]
	{ .regAddr= 0x401A, .regData= 0x37, .delayUs=0 }, // TCLKPREPARE[15:0]
	{ .regAddr= 0x401B, .regData= 0x00, .delayUs=0 }, // TCLKPREPARE[15:0]
	{ .regAddr= 0x401C, .regData= 0x37, .delayUs=0 }, // TCLKTRAI[15:0]
	{ .regAddr= 0x401D, .regData= 0x00, .delayUs=0 }, // TCLKTRAI[15:0]
	{ .regAddr= 0x401E, .regData= 0xF7, .delayUs=0 }, // TCLKZERO[15:0]
	{ .regAddr= 0x401F, .regData= 0x00, .delayUs=0 }, // TCLKZERO[15:0]
	{ .regAddr= 0x4020, .regData= 0x3F, .delayUs=0 }, // THSPREPARE[15:0]
	{ .regAddr= 0x4021, .regData= 0x00, .delayUs=0 }, // THSPREPARE[15:0]
	{ .regAddr= 0x4022, .regData= 0x6F, .delayUs=0 }, // THSZERO[15:0]
	{ .regAddr= 0x4023, .regData= 0x00, .delayUs=0 }, // THSZERO[15:0]
	{ .regAddr= 0x4024, .regData= 0x3F, .delayUs=0 }, // THSTRAI[15:0]
	{ .regAddr= 0x4025, .regData= 0x00, .delayUs=0 }, // THSTRAI[15:0]
	{ .regAddr= 0x4026, .regData= 0x5f, .delayUs=0 }, // THSEXIT[15:0]
	{ .regAddr= 0x4027, .regData= 0x00, .delayUs=0 }, // THSEXIT[15:0]
	{ .regAddr= 0x4028, .regData= 0x2f, .delayUs=0 }, // TLPX[15:0]
	{ .regAddr= 0x4029, .regData= 0x00, .delayUs=0 }, // TLPX[15:0]
	{ .regAddr= 0x4074, .regData= 0x01, .delayUs=0 }, // INCKSEL7 [2:0]
    { .regAddr = 0x3000, .regData = 0x00, .delayUs = 24000}, /* leave standby */
};
static drv_sensor_reg_info_t  cv2003_1080p_30fps_dol2_setting[] =
{
	{ .regAddr= 0x3000, .regData= 0x01, .delayUs=0 }, /* enter standby */
	{ .regAddr= 0x3002, .regData= 0x01, .delayUs=0 }, /* master stop */
	{ .regAddr= 0x3008, .regData= 0x7F, .delayUs=0 }, // BCWAIT_TIME[9:0]
	{ .regAddr= 0x3009, .regData= 0x00, .delayUs=0 }, // BCWAIT_TIME[9:0]
	{ .regAddr= 0x300A, .regData= 0x5B, .delayUs=0 }, // CPWAIT_TIME[9:0]
	{ .regAddr= 0x300B, .regData= 0x00, .delayUs=0 }, // CPWAIT_TIME[9:0]
	{ .regAddr= 0x301c, .regData= 0x0, .delayUs=0 },  //WINMODE
	{ .regAddr= 0x3020, .regData= 0x1, .delayUs=0 },  //horizontal 2 binning
	{ .regAddr= 0x3021, .regData= 0x1, .delayUs=0 },  //veritical 2 binning
	{ .regAddr= 0x3022, .regData= 0x1, .delayUs=0 },  //ADDMODE 2/2-line binning
	{ .regAddr= 0x3024, .regData= 0xEC, .delayUs=0 }, // VMAX[19:0]
	{ .regAddr= 0x3025, .regData= 0x08, .delayUs=0 }, // VMAX[19:0]
	{ .regAddr= 0x3026, .regData= 0x00, .delayUs=0 }, // VMAX[19:0]

	{ .regAddr= 0x3028, .regData= 0x4c, .delayUs=0 }, // HMAX[15:0]
	{ .regAddr= 0x3029, .regData= 0x04, .delayUs=0 }, // HMAX[15:0]

    { .regAddr= 0x3030, .regData= 0x00, .delayUs=0 },  //normal
	{ .regAddr= 0x3031, .regData= 0x00, .delayUs=0 }, // AD:10 bit
	{ .regAddr= 0x3032, .regData= 0x01, .delayUs=0 }, // MD:12 bit
	{ .regAddr= 0x3033, .regData= 0x05, .delayUs=0 }, // SYS_MODE[3:0]

	{ .regAddr= 0x3023, .regData= 0x0, .delayUs=0 },  //VCMODE
	{ .regAddr= 0x302c, .regData= 0x01, .delayUs=0 }, // WDMODE[1:0]
	{ .regAddr= 0x302d, .regData= 0x01, .delayUs=0 }, // WDSEL[1:0]
	{ .regAddr= 0x3050, .regData= 0x26, .delayUs=0 }, // SHR0[19:0]
	{ .regAddr= 0x3051, .regData= 0x0F, .delayUs=0 }, // SHR0[19:0]
	{ .regAddr= 0x3052, .regData= 0x00, .delayUs=0 }, // SHR0[19:0]
	//{ .regAddr= 0x3050, .regData= 0x66, .delayUs=0 }, // SHR0[19:0]
	//{ .regAddr= 0x3051, .regData= 0x00, .delayUs=0 }, //
	//{ .regAddr= 0x3052, .regData= 0x00, .delayUs=0 }, //
	{ .regAddr= 0x3054, .regData= 0x09, .delayUs=0 }, // SHR1[19:0]
	{ .regAddr= 0x3060, .regData= 0x4d, .delayUs=0 }, // RHS1[19:0]
	//{ .regAddr= 0x3090, .regData= 0x14, .delayUs=0 }, // GAIN_PCG_0[8:0]
	//{ .regAddr= 0x3091, .regData= 0x00, .delayUs=0 }, // GAIN_PCG_0[8:0]
	{ .regAddr= 0x3090, .regData= 0x00, .delayUs=0 }, // GAIN_PCG_0[8:0]
	{ .regAddr= 0x3091, .regData= 0x00, .delayUs=0 }, // GAIN_PCG_0[8:0]
	{ .regAddr= 0x30C1, .regData= 0x00, .delayUs=0 }, // XVS_DRV[1:0]
	{ .regAddr= 0x30CF, .regData= 0x01, .delayUs=0 }, // XVS_DRV[1:0]

	{ .regAddr= 0x30D9, .regData= 0x02, .delayUs=0 }, // DIG_CLP_VSTAET[4:0]
	{ .regAddr= 0x30DA, .regData= 0x01, .delayUs=0 }, // DIG_VLP_VNUM[1:0]

	{ .regAddr= 0x3115, .regData= 0x00, .delayUs=0 }, // INCKSEL1[7:0]
	{ .regAddr= 0x3116, .regData= 0x24, .delayUs=0 }, // INCKSEL2[7:0]
	{ .regAddr= 0x3118, .regData= 0xC0, .delayUs=0 }, // INCKSEL3[10:0]
	{ .regAddr= 0x3119, .regData= 0x00, .delayUs=0 }, // INCKSEL3[10:0]
	{ .regAddr= 0x311A, .regData= 0xE0, .delayUs=0 }, // INCKSEL4[10:0]
	{ .regAddr= 0x311B, .regData= 0x00, .delayUs=0 }, // INCKSEL4[10:0]
	{ .regAddr= 0x311E, .regData= 0x24, .delayUs=0 }, // INCKSEL5[7:0]
	{ .regAddr= 0x32D4, .regData= 0x21, .delayUs=0 },
	{ .regAddr= 0x32EC, .regData= 0xA1, .delayUs=0 },
	/*
	{ .regAddr= 0x344C, .regData= 0x2B, .delayUs=0 },
	{ .regAddr= 0x344D, .regData= 0x01, .delayUs=0 },
	{ .regAddr= 0x344E, .regData= 0xED, .delayUs=0 },
	{ .regAddr= 0x344F, .regData= 0x01, .delayUs=0 },
	{ .regAddr= 0x3450, .regData= 0xF6, .delayUs=0 },
	{ .regAddr= 0x3451, .regData= 0x02, .delayUs=0 },
	*/
	{ .regAddr= 0x3452, .regData= 0x7F, .delayUs=0 },
	{ .regAddr= 0x3453, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x358A, .regData= 0x04, .delayUs=0 },
	{ .regAddr= 0x35A1, .regData= 0x02, .delayUs=0 },
	/*
	{ .regAddr= 0x35EC, .regData= 0x27, .delayUs=0 },
	{ .regAddr= 0x35EE, .regData= 0x8D, .delayUs=0 },
	{ .regAddr= 0x35F0, .regData= 0x8D, .delayUs=0 },
	{ .regAddr= 0x35F2, .regData= 0x29, .delayUs=0 },
	*/
	{ .regAddr= 0x36BC, .regData= 0x0C, .delayUs=0 },
	{ .regAddr= 0x36CC, .regData= 0x53, .delayUs=0 },
	{ .regAddr= 0x36CD, .regData= 0x00, .delayUs=0 },
	{ .regAddr= 0x36CE, .regData= 0x3C, .delayUs=0 },
	{ .regAddr= 0x36D0, .regData= 0x8C, .delayUs=0 },
	{ .regAddr= 0x36D1, .regData= 0x00, .delayUs=0 },
	{ .regAddr= 0x36D2, .regData= 0x71, .delayUs=0 },
	{ .regAddr= 0x36D4, .regData= 0x3C, .delayUs=0 },
	{ .regAddr= 0x36D6, .regData= 0x53, .delayUs=0 },
	{ .regAddr= 0x36D7, .regData= 0x00, .delayUs=0 },
	{ .regAddr= 0x36D8, .regData= 0x71, .delayUs=0 },
	{ .regAddr= 0x36DA, .regData= 0x8C, .delayUs=0 },
	{ .regAddr= 0x36DB, .regData= 0x00, .delayUs=0 },
	{ .regAddr= 0x3701, .regData= 0x00, .delayUs=0 },  //AD 0:10bit, 3:12bit
	//{ .regAddr= 0x3720, .regData= 0x00, .delayUs=0 },
	{ .regAddr= 0x3724, .regData= 0x02, .delayUs=0 },
	{ .regAddr= 0x3726, .regData= 0x02, .delayUs=0 },
	{ .regAddr= 0x3732, .regData= 0x02, .delayUs=0 },
	{ .regAddr= 0x3734, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x3736, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x3742, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x3862, .regData= 0xE0, .delayUs=0 },
	{ .regAddr= 0x38CC, .regData= 0x30, .delayUs=0 },
	{ .regAddr= 0x38CD, .regData= 0x2F, .delayUs=0 },
	{ .regAddr= 0x395C, .regData= 0x0C, .delayUs=0 },
	/*
	{ .regAddr= 0x39A4, .regData= 0x07, .delayUs=0 },
	{ .regAddr= 0x39A8, .regData= 0x32, .delayUs=0 },
	{ .regAddr= 0x39AA, .regData= 0x32, .delayUs=0 },
	{ .regAddr= 0x39AC, .regData= 0x32, .delayUs=0 },
	{ .regAddr= 0x39AE, .regData= 0x32, .delayUs=0 },
	{ .regAddr= 0x39B0, .regData= 0x32, .delayUs=0 },
	{ .regAddr= 0x39B2, .regData= 0x2F, .delayUs=0 },
	{ .regAddr= 0x39B4, .regData= 0x2D, .delayUs=0 },
	{ .regAddr= 0x39B6, .regData= 0x28, .delayUs=0 },
	{ .regAddr= 0x39B8, .regData= 0x30, .delayUs=0 },
	{ .regAddr= 0x39BA, .regData= 0x30, .delayUs=0 },
	{ .regAddr= 0x39BC, .regData= 0x30, .delayUs=0 },
	{ .regAddr= 0x39BE, .regData= 0x30, .delayUs=0 },
	{ .regAddr= 0x39C0, .regData= 0x30, .delayUs=0 },
	{ .regAddr= 0x39C2, .regData= 0x2E, .delayUs=0 },
	{ .regAddr= 0x39C4, .regData= 0x2B, .delayUs=0 },
	{ .regAddr= 0x39C6, .regData= 0x25, .delayUs=0 },
	*/
	{ .regAddr= 0x3A42, .regData= 0xD1, .delayUs=0 },
	{ .regAddr= 0x3A4C, .regData= 0x77, .delayUs=0 },
	{ .regAddr= 0x3AE0, .regData= 0x02, .delayUs=0 },
	{ .regAddr= 0x3AEC, .regData= 0x0C, .delayUs=0 },
	{ .regAddr= 0x3B00, .regData= 0x2E, .delayUs=0 },
	{ .regAddr= 0x3B06, .regData= 0x29, .delayUs=0 },
	{ .regAddr= 0x3B98, .regData= 0x25, .delayUs=0 },
	{ .regAddr= 0x3B99, .regData= 0x21, .delayUs=0 },
	{ .regAddr= 0x3B9B, .regData= 0x13, .delayUs=0 },
	{ .regAddr= 0x3B9C, .regData= 0x13, .delayUs=0 },
	{ .regAddr= 0x3B9D, .regData= 0x13, .delayUs=0 },
	{ .regAddr= 0x3B9E, .regData= 0x13, .delayUs=0 },
	{ .regAddr= 0x3BA1, .regData= 0x00, .delayUs=0 },
	{ .regAddr= 0x3BA2, .regData= 0x06, .delayUs=0 },
	{ .regAddr= 0x3BA3, .regData= 0x0B, .delayUs=0 },
	{ .regAddr= 0x3BA4, .regData= 0x10, .delayUs=0 },
	{ .regAddr= 0x3BA5, .regData= 0x14, .delayUs=0 },
	{ .regAddr= 0x3BA6, .regData= 0x18, .delayUs=0 },
	{ .regAddr= 0x3BA7, .regData= 0x1A, .delayUs=0 },
	{ .regAddr= 0x3BA8, .regData= 0x1A, .delayUs=0 },
	{ .regAddr= 0x3BA9, .regData= 0x1A, .delayUs=0 },
	{ .regAddr= 0x3BAC, .regData= 0xED, .delayUs=0 },
	{ .regAddr= 0x3BAD, .regData= 0x01, .delayUs=0 },
	{ .regAddr= 0x3BAE, .regData= 0xF6, .delayUs=0 },
	{ .regAddr= 0x3BAF, .regData= 0x02, .delayUs=0 },
	{ .regAddr= 0x3BB0, .regData= 0xA2, .delayUs=0 },
	{ .regAddr= 0x3BB1, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x3BB2, .regData= 0xE0, .delayUs=0 },
	{ .regAddr= 0x3BB3, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x3BB4, .regData= 0xE0, .delayUs=0 },
	{ .regAddr= 0x3BB5, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x3BB6, .regData= 0xE0, .delayUs=0 },
	{ .regAddr= 0x3BB7, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x3BB8, .regData= 0xE0, .delayUs=0 },
	{ .regAddr= 0x3BBA, .regData= 0xE0, .delayUs=0 },
	{ .regAddr= 0x3BBC, .regData= 0xDA, .delayUs=0 },
	{ .regAddr= 0x3BBE, .regData= 0x88, .delayUs=0 },
	{ .regAddr= 0x3BC0, .regData= 0x44, .delayUs=0 },
	{ .regAddr= 0x3BC2, .regData= 0x7B, .delayUs=0 },
	{ .regAddr= 0x3BC4, .regData= 0xA2, .delayUs=0 },
	{ .regAddr= 0x3BC8, .regData= 0xBD, .delayUs=0 },
	{ .regAddr= 0x3BCA, .regData= 0xBD, .delayUs=0 },

	{ .regAddr= 0x4001, .regData= 0x03, .delayUs=0 }, // LANEMODE[2:0]
	{ .regAddr= 0x4004, .regData= 0x48, .delayUs=0 }, // TXCLKESC_FREQ[15:0]
	{ .regAddr= 0x4005, .regData= 0x09, .delayUs=0 }, //
	{ .regAddr= 0x400C, .regData= 0x00, .delayUs=0 }, // INCKSEL6

	{ .regAddr= 0x4018, .regData= 0x7f, .delayUs=0 }, // TCLKPOST[15:0]
	{ .regAddr= 0x4019, .regData= 0x00, .delayUs=0 }, // TCLKPOST[15:0]
	{ .regAddr= 0x401A, .regData= 0x37, .delayUs=0 }, // TCLKPREPARE[15:0]
	{ .regAddr= 0x401B, .regData= 0x00, .delayUs=0 }, // TCLKPREPARE[15:0]
	{ .regAddr= 0x401C, .regData= 0x37, .delayUs=0 }, // TCLKTRAI[15:0]
	{ .regAddr= 0x401D, .regData= 0x00, .delayUs=0 }, // TCLKTRAI[15:0]
	{ .regAddr= 0x401E, .regData= 0xF7, .delayUs=0 }, // TCLKZERO[15:0]
	{ .regAddr= 0x401F, .regData= 0x00, .delayUs=0 }, // TCLKZERO[15:0]
	{ .regAddr= 0x4020, .regData= 0x3F, .delayUs=0 }, // THSPREPARE[15:0]
	{ .regAddr= 0x4021, .regData= 0x00, .delayUs=0 }, // THSPREPARE[15:0]
	{ .regAddr= 0x4022, .regData= 0x6F, .delayUs=0 }, // THSZERO[15:0]
	{ .regAddr= 0x4023, .regData= 0x00, .delayUs=0 }, // THSZERO[15:0]
	{ .regAddr= 0x4024, .regData= 0x3F, .delayUs=0 }, // THSTRAI[15:0]
	{ .regAddr= 0x4025, .regData= 0x00, .delayUs=0 }, // THSTRAI[15:0]
	{ .regAddr= 0x4026, .regData= 0x5f, .delayUs=0 }, // THSEXIT[15:0]
	{ .regAddr= 0x4027, .regData= 0x00, .delayUs=0 }, // THSEXIT[15:0]
	{ .regAddr= 0x4028, .regData= 0x2f, .delayUs=0 }, // TLPX[15:0]
	{ .regAddr= 0x4029, .regData= 0x00, .delayUs=0 }, // TLPX[15:0]
	{ .regAddr= 0x4074, .regData= 0x01, .delayUs=0 }, // INCKSEL7 [2:0]
    { .regAddr = 0x3000, .regData = 0x00, .delayUs = 24000}, /* leave standby */
};

//CV2003-AAQR All-pixel scan CSI-2_4lane 37.125Mhz AD:12bit Output:12bit 891Mbps Master Mode fps Integration Time xx.xxms Gain:6dB
static drv_sensor_reg_info_t  cv2003_4k_30fps_dol2_setting[] =
//CV2003-AAQR All-pixel scan CSI-2_4lane 37.125Mhz AD:10bit Output:12bit 1782Mbps Master Mode fps Integration Time xx.xxms
// static drv_sensor_reg_info_t  cv2003_4k_60fps_dol2_setting[] =
{
	{ .regAddr= 0x3000, .regData= 0x01, .delayUs=0 }, /* enter standby */
	{ .regAddr= 0x3002, .regData= 0x01, .delayUs=0 }, /* master stop */
	{ .regAddr= 0x3008, .regData= 0x7F, .delayUs=0 }, // BCWAIT_TIME[9:0]
	{ .regAddr= 0x3009, .regData= 0x00, .delayUs=0 }, // BCWAIT_TIME[9:0]
	{ .regAddr= 0x300A, .regData= 0x5B, .delayUs=0 }, // CPWAIT_TIME[9:0]
	{ .regAddr= 0x300B, .regData= 0x00, .delayUs=0 }, // CPWAIT_TIME[9:0]
	{ .regAddr= 0x301c, .regData= 0x0, .delayUs=0 },  //WINMODE
	{ .regAddr= 0x3020, .regData= 0x0, .delayUs=0 },  //horizontal 2 binning
	{ .regAddr= 0x3021, .regData= 0x0, .delayUs=0 },  //veritical 2 binning
	{ .regAddr= 0x3022, .regData= 0x0, .delayUs=0 },  //ADDMODE 2/2-line binning
	{ .regAddr= 0x3024, .regData= 0xCA, .delayUs=0 }, // VMAX[19:0]
	{ .regAddr= 0x3025, .regData= 0x08, .delayUs=0 }, // VMAX[19:0]
	{ .regAddr= 0x3026, .regData= 0x00, .delayUs=0 }, // VMAX[19:0]

	{ .regAddr= 0x3028, .regData= 0x50, .delayUs=0 }, // HMAX[15:0]  40fps
	{ .regAddr= 0x3029, .regData= 0x03, .delayUs=0 }, // HMAX[15:0]
	//{ .regAddr= 0x3028, .regData= 0xa0, .delayUs=0 }, // HMAX[15:0]
	//{ .regAddr= 0x3029, .regData= 0x02, .delayUs=0 }, // HMAX[15:0]
	//{ .regAddr= 0x3028, .regData= 0x26, .delayUs=0 }, // HMAX[15:0]
	//{ .regAddr= 0x3029, .regData= 0x02, .delayUs=0 }, // HMAX[15:0]
    { .regAddr= 0x3030, .regData= 0x00, .delayUs=0 },  //normal
	{ .regAddr= 0x3031, .regData= 0x00, .delayUs=0 }, // AD:10 bit
	{ .regAddr= 0x3032, .regData= 0x01, .delayUs=0 }, // MD:12 bit
	{ .regAddr= 0x3033, .regData= 0x04, .delayUs=0 }, // SYS_MODE[3:0]

	{ .regAddr= 0x3023, .regData= 0x0, .delayUs=0 },  // VCMODE
	{ .regAddr= 0x302c, .regData= 0x01, .delayUs=0 }, // WDMODE[1:0]
	{ .regAddr= 0x302d, .regData= 0x01, .delayUs=0 }, // WDSEL[1:0]
	{ .regAddr= 0x3050, .regData= 0x26, .delayUs=0 }, // SHR0[19:0]
	{ .regAddr= 0x3051, .regData= 0x0F, .delayUs=0 }, // SHR0[19:0]
	{ .regAddr= 0x3052, .regData= 0x00, .delayUs=0 }, // SHR0[19:0]
	//{ .regAddr= 0x3050, .regData= 0x66, .delayUs=0 }, // SHR0[19:0]
	//{ .regAddr= 0x3051, .regData= 0x00, .delayUs=0 }, //
	//{ .regAddr= 0x3052, .regData= 0x00, .delayUs=0 }, //
	{ .regAddr= 0x3054, .regData= 0x09, .delayUs=0 }, // SHR1[19:0]
	{ .regAddr= 0x3060, .regData= 0x4d, .delayUs=0 }, // RHS1[19:0]
	//{ .regAddr= 0x3090, .regData= 0x14, .delayUs=0 }, // GAIN_PCG_0[8:0]
	//{ .regAddr= 0x3091, .regData= 0x00, .delayUs=0 }, // GAIN_PCG_0[8:0]
	{ .regAddr= 0x3090, .regData= 0x00, .delayUs=0 }, // GAIN_PCG_0[8:0]
	{ .regAddr= 0x3091, .regData= 0x00, .delayUs=0 }, // GAIN_PCG_0[8:0]
	{ .regAddr= 0x30C1, .regData= 0x00, .delayUs=0 }, // XVS_DRV[1:0]
	{ .regAddr= 0x30CF, .regData= 0x01, .delayUs=0 }, // XVS_DRV[1:0]

	{ .regAddr= 0x30D9, .regData= 0x06, .delayUs=0 }, // DIG_CLP_VSTAET[4:0]
	{ .regAddr= 0x30DA, .regData= 0x02, .delayUs=0 }, // DIG_VLP_VNUM[1:0]

	{ .regAddr= 0x3115, .regData= 0x00, .delayUs=0 }, // INCKSEL1[7:0]
	{ .regAddr= 0x3116, .regData= 0x24, .delayUs=0 }, // INCKSEL2[7:0]
	{ .regAddr= 0x3118, .regData= 0xC0, .delayUs=0 }, // INCKSEL3[10:0]
	{ .regAddr= 0x3119, .regData= 0x00, .delayUs=0 }, // INCKSEL3[10:0]
	{ .regAddr= 0x311A, .regData= 0xE0, .delayUs=0 }, // INCKSEL4[10:0]
	{ .regAddr= 0x311B, .regData= 0x00, .delayUs=0 }, // INCKSEL4[10:0]
	{ .regAddr= 0x311E, .regData= 0x24, .delayUs=0 }, // INCKSEL5[7:0]
	{ .regAddr= 0x32D4, .regData= 0x21, .delayUs=0 },
	{ .regAddr= 0x32EC, .regData= 0xA1, .delayUs=0 },
	/*
	{ .regAddr= 0x344C, .regData= 0x2B, .delayUs=0 },
	{ .regAddr= 0x344D, .regData= 0x01, .delayUs=0 },
	{ .regAddr= 0x344E, .regData= 0xED, .delayUs=0 },
	{ .regAddr= 0x344F, .regData= 0x01, .delayUs=0 },
	{ .regAddr= 0x3450, .regData= 0xF6, .delayUs=0 },
	{ .regAddr= 0x3451, .regData= 0x02, .delayUs=0 },
	*/
	{ .regAddr= 0x3452, .regData= 0x7F, .delayUs=0 },
	{ .regAddr= 0x3453, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x358A, .regData= 0x04, .delayUs=0 },
	{ .regAddr= 0x35A1, .regData= 0x02, .delayUs=0 },
	/*
	{ .regAddr= 0x35EC, .regData= 0x27, .delayUs=0 },
	{ .regAddr= 0x35EE, .regData= 0x8D, .delayUs=0 },
	{ .regAddr= 0x35F0, .regData= 0x8D, .delayUs=0 },
	{ .regAddr= 0x35F2, .regData= 0x29, .delayUs=0 },
	*/
	{ .regAddr= 0x36BC, .regData= 0x0C, .delayUs=0 },
	{ .regAddr= 0x36CC, .regData= 0x53, .delayUs=0 },
	{ .regAddr= 0x36CD, .regData= 0x00, .delayUs=0 },
	{ .regAddr= 0x36CE, .regData= 0x3C, .delayUs=0 },
	{ .regAddr= 0x36D0, .regData= 0x8C, .delayUs=0 },
	{ .regAddr= 0x36D1, .regData= 0x00, .delayUs=0 },
	{ .regAddr= 0x36D2, .regData= 0x71, .delayUs=0 },
	{ .regAddr= 0x36D4, .regData= 0x3C, .delayUs=0 },
	{ .regAddr= 0x36D6, .regData= 0x53, .delayUs=0 },
	{ .regAddr= 0x36D7, .regData= 0x00, .delayUs=0 },
	{ .regAddr= 0x36D8, .regData= 0x71, .delayUs=0 },
	{ .regAddr= 0x36DA, .regData= 0x8C, .delayUs=0 },
	{ .regAddr= 0x36DB, .regData= 0x00, .delayUs=0 },
	{ .regAddr= 0x3701, .regData= 0x00, .delayUs=0 },  //AD 0:10bit, 3:12bit
	//{ .regAddr= 0x3720, .regData= 0x00, .delayUs=0 },
	{ .regAddr= 0x3724, .regData= 0x02, .delayUs=0 },
	{ .regAddr= 0x3726, .regData= 0x02, .delayUs=0 },
	{ .regAddr= 0x3732, .regData= 0x02, .delayUs=0 },
	{ .regAddr= 0x3734, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x3736, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x3742, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x3862, .regData= 0xE0, .delayUs=0 },
	{ .regAddr= 0x38CC, .regData= 0x30, .delayUs=0 },
	{ .regAddr= 0x38CD, .regData= 0x2F, .delayUs=0 },
	{ .regAddr= 0x395C, .regData= 0x0C, .delayUs=0 },
	/*
	{ .regAddr= 0x39A4, .regData= 0x07, .delayUs=0 },
	{ .regAddr= 0x39A8, .regData= 0x32, .delayUs=0 },
	{ .regAddr= 0x39AA, .regData= 0x32, .delayUs=0 },
	{ .regAddr= 0x39AC, .regData= 0x32, .delayUs=0 },
	{ .regAddr= 0x39AE, .regData= 0x32, .delayUs=0 },
	{ .regAddr= 0x39B0, .regData= 0x32, .delayUs=0 },
	{ .regAddr= 0x39B2, .regData= 0x2F, .delayUs=0 },
	{ .regAddr= 0x39B4, .regData= 0x2D, .delayUs=0 },
	{ .regAddr= 0x39B6, .regData= 0x28, .delayUs=0 },
	{ .regAddr= 0x39B8, .regData= 0x30, .delayUs=0 },
	{ .regAddr= 0x39BA, .regData= 0x30, .delayUs=0 },
	{ .regAddr= 0x39BC, .regData= 0x30, .delayUs=0 },
	{ .regAddr= 0x39BE, .regData= 0x30, .delayUs=0 },
	{ .regAddr= 0x39C0, .regData= 0x30, .delayUs=0 },
	{ .regAddr= 0x39C2, .regData= 0x2E, .delayUs=0 },
	{ .regAddr= 0x39C4, .regData= 0x2B, .delayUs=0 },
	{ .regAddr= 0x39C6, .regData= 0x25, .delayUs=0 },
	*/
	{ .regAddr= 0x3A42, .regData= 0xD1, .delayUs=0 },
	{ .regAddr= 0x3A4C, .regData= 0x77, .delayUs=0 },
	{ .regAddr= 0x3AE0, .regData= 0x02, .delayUs=0 },
	{ .regAddr= 0x3AEC, .regData= 0x0C, .delayUs=0 },
	{ .regAddr= 0x3B00, .regData= 0x2E, .delayUs=0 },
	{ .regAddr= 0x3B06, .regData= 0x29, .delayUs=0 },
	{ .regAddr= 0x3B98, .regData= 0x25, .delayUs=0 },
	{ .regAddr= 0x3B99, .regData= 0x21, .delayUs=0 },
	{ .regAddr= 0x3B9B, .regData= 0x13, .delayUs=0 },
	{ .regAddr= 0x3B9C, .regData= 0x13, .delayUs=0 },
	{ .regAddr= 0x3B9D, .regData= 0x13, .delayUs=0 },
	{ .regAddr= 0x3B9E, .regData= 0x13, .delayUs=0 },
	{ .regAddr= 0x3BA1, .regData= 0x00, .delayUs=0 },
	{ .regAddr= 0x3BA2, .regData= 0x06, .delayUs=0 },
	{ .regAddr= 0x3BA3, .regData= 0x0B, .delayUs=0 },
	{ .regAddr= 0x3BA4, .regData= 0x10, .delayUs=0 },
	{ .regAddr= 0x3BA5, .regData= 0x14, .delayUs=0 },
	{ .regAddr= 0x3BA6, .regData= 0x18, .delayUs=0 },
	{ .regAddr= 0x3BA7, .regData= 0x1A, .delayUs=0 },
	{ .regAddr= 0x3BA8, .regData= 0x1A, .delayUs=0 },
	{ .regAddr= 0x3BA9, .regData= 0x1A, .delayUs=0 },
	{ .regAddr= 0x3BAC, .regData= 0xED, .delayUs=0 },
	{ .regAddr= 0x3BAD, .regData= 0x01, .delayUs=0 },
	{ .regAddr= 0x3BAE, .regData= 0xF6, .delayUs=0 },
	{ .regAddr= 0x3BAF, .regData= 0x02, .delayUs=0 },
	{ .regAddr= 0x3BB0, .regData= 0xA2, .delayUs=0 },
	{ .regAddr= 0x3BB1, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x3BB2, .regData= 0xE0, .delayUs=0 },
	{ .regAddr= 0x3BB3, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x3BB4, .regData= 0xE0, .delayUs=0 },
	{ .regAddr= 0x3BB5, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x3BB6, .regData= 0xE0, .delayUs=0 },
	{ .regAddr= 0x3BB7, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x3BB8, .regData= 0xE0, .delayUs=0 },
	{ .regAddr= 0x3BBA, .regData= 0xE0, .delayUs=0 },
	{ .regAddr= 0x3BBC, .regData= 0xDA, .delayUs=0 },
	{ .regAddr= 0x3BBE, .regData= 0x88, .delayUs=0 },
	{ .regAddr= 0x3BC0, .regData= 0x44, .delayUs=0 },
	{ .regAddr= 0x3BC2, .regData= 0x7B, .delayUs=0 },
	{ .regAddr= 0x3BC4, .regData= 0xA2, .delayUs=0 },
	{ .regAddr= 0x3BC8, .regData= 0xBD, .delayUs=0 },
	{ .regAddr= 0x3BCA, .regData= 0xBD, .delayUs=0 },

	{ .regAddr= 0x4001, .regData= 0x03, .delayUs=0 }, // LANEMODE[2:0]
	{ .regAddr= 0x4004, .regData= 0x48, .delayUs=0 }, // TXCLKESC_FREQ[15:0]
	{ .regAddr= 0x4005, .regData= 0x09, .delayUs=0 }, //
	{ .regAddr= 0x400C, .regData= 0x01, .delayUs=0 }, // INCKSEL6

	{ .regAddr= 0x4018, .regData= 0xb7, .delayUs=0 }, // TCLKPOST[15:0]
	{ .regAddr= 0x4019, .regData= 0x00, .delayUs=0 }, // TCLKPOST[15:0]
	{ .regAddr= 0x401A, .regData= 0x67, .delayUs=0 }, // TCLKPREPARE[15:0]
	{ .regAddr= 0x401B, .regData= 0x00, .delayUs=0 }, // TCLKPREPARE[15:0]
	{ .regAddr= 0x401C, .regData= 0x6f, .delayUs=0 }, // TCLKTRAI[15:0]
	{ .regAddr= 0x401D, .regData= 0x00, .delayUs=0 }, // TCLKTRAI[15:0]
	{ .regAddr= 0x401E, .regData= 0xdf, .delayUs=0 }, // TCLKZERO[15:0]
	{ .regAddr= 0x401F, .regData= 0x01, .delayUs=0 }, // TCLKZERO[15:0]
	{ .regAddr= 0x4020, .regData= 0x6F, .delayUs=0 }, // THSPREPARE[15:0]
	{ .regAddr= 0x4021, .regData= 0x00, .delayUs=0 }, // THSPREPARE[15:0]
	{ .regAddr= 0x4022, .regData= 0xcf, .delayUs=0 }, // THSZERO[15:0]
	{ .regAddr= 0x4023, .regData= 0x00, .delayUs=0 }, // THSZERO[15:0]
	{ .regAddr= 0x4024, .regData= 0x6F, .delayUs=0 }, // THSTRAI[15:0]
	{ .regAddr= 0x4025, .regData= 0x00, .delayUs=0 }, // THSTRAI[15:0]
	{ .regAddr= 0x4026, .regData= 0xb7, .delayUs=0 }, // THSEXIT[15:0]
	{ .regAddr= 0x4027, .regData= 0x00, .delayUs=0 }, // THSEXIT[15:0]
	{ .regAddr= 0x4028, .regData= 0x5f, .delayUs=0 }, // TLPX[15:0]
	{ .regAddr= 0x4029, .regData= 0x00, .delayUs=0 }, // TLPX[15:0]
	{ .regAddr= 0x4074, .regData= 0x00, .delayUs=0 }, // INCKSEL7 [2:0]
    { .regAddr = 0x3000, .regData = 0x00, .delayUs = 24000}, /* leave standby */
};


//CV2003-AAQR All-pixel scan CSI-2_4lane 37.125Mhz AD:10bit Output:12bit 594Mbps Master Mode fps Integration Time xx.xxms Gain:6dB
static drv_sensor_reg_info_t  cv2003_1080p_20fps_setting[] =
{
	{ .regAddr= 0x3000, .regData= 0x01, .delayUs=0 }, /* enter standby */
	{ .regAddr= 0x3003, .regData= 0x01, .delayUs=0 }, /* slave mode */
	{ .regAddr= 0x3002, .regData= 0x01, .delayUs=0 }, /* master stop */
	{ .regAddr= 0x3008, .regData= 0x7F, .delayUs=0 }, // BCWAIT_TIME[9:0]
	{ .regAddr= 0x3009, .regData= 0x00, .delayUs=0 }, // BCWAIT_TIME[9:0]
	{ .regAddr= 0x300A, .regData= 0x5B, .delayUs=0 }, // CPWAIT_TIME[9:0]
	{ .regAddr= 0x300B, .regData= 0x00, .delayUs=0 }, // CPWAIT_TIME[9:0]
	{ .regAddr= 0x301c, .regData= 0x0, .delayUs=0 },  //WINMODE
	{ .regAddr= 0x3020, .regData= 0x1, .delayUs=0 },  //horizontal 2 binning
	{ .regAddr= 0x3021, .regData= 0x1, .delayUs=0 },  //veritical 2 binning
	{ .regAddr= 0x3022, .regData= 0x1, .delayUs=0 },  //ADDMODE 2/2-line binning
	{ .regAddr= 0x3024, .regData= 0x2F, .delayUs=0 }, // VMAX[19:0]
	{ .regAddr= 0x3025, .regData= 0x0D, .delayUs=0 }, // VMAX[19:0]
	{ .regAddr= 0x3026, .regData= 0x00, .delayUs=0 }, // VMAX[19:0]

	{ .regAddr= 0x3028, .regData= 0x4C, .delayUs=0 }, // HMAX[15:0]
	{ .regAddr= 0x3029, .regData= 0x04, .delayUs=0 }, // HMAX[15:0]
    { .regAddr= 0x3030, .regData= 0x00, .delayUs=0 },  //invert
	{ .regAddr= 0x3031, .regData= 0x00, .delayUs=0 }, // AD:10 bit
	{ .regAddr= 0x3032, .regData= 0x01, .delayUs=0 }, // MD:12 bit
	{ .regAddr= 0x3033, .regData= 0x07, .delayUs=0 }, // SYS_MODE[3:0]
	//{ .regAddr= 0x3034, .regData= 0x07, .delayUs=0 }, // SYS_MODE[3:0]

	{ .regAddr= 0x3050, .regData= 0x08, .delayUs=0 }, // SHR0[19:0]
	{ .regAddr= 0x3051, .regData= 0x00, .delayUs=0 }, // SHR0[19:0]
	{ .regAddr= 0x3052, .regData= 0x00, .delayUs=0 }, // SHR0[19:0]
	//{ .regAddr= 0x3050, .regData= 0x66, .delayUs=0 }, // SHR0[19:0]
	//{ .regAddr= 0x3051, .regData= 0x00, .delayUs=0 }, //
	//{ .regAddr= 0x3052, .regData= 0x00, .delayUs=0 }, //
	//{ .regAddr= 0x3090, .regData= 0x14, .delayUs=0 }, // GAIN_PCG_0[8:0]
	//{ .regAddr= 0x3091, .regData= 0x00, .delayUs=0 }, // GAIN_PCG_0[8:0]
	{ .regAddr= 0x3090, .regData= 0x00, .delayUs=0 }, // GAIN_PCG_0[8:0]
	{ .regAddr= 0x3091, .regData= 0x00, .delayUs=0 }, // GAIN_PCG_0[8:0]

 	{ .regAddr= 0x30C0, .regData= 0x2a, .delayUs=0,}, //vsync/hsync Output
	{ .regAddr= 0x30C1, .regData= 0x0f, .delayUs=0 }, // XVS_DRV[1:0] master=0, slave=0xf
 	{ .regAddr= 0x30CC, .regData= 0x00, .delayUs=0 }, //XVSLNG
 	{ .regAddr= 0x30CD, .regData= 0x00, .delayUs=0 }, //XHSLNG

	{ .regAddr= 0x30D9, .regData= 0x02, .delayUs=0 }, // DIG_CLP_VSTAET[4:0]
	{ .regAddr= 0x30DA, .regData= 0x01, .delayUs=0 }, // DIG_VLP_VNUM[1:0]

	{ .regAddr= 0x3115, .regData= 0x00, .delayUs=0 }, // INCKSEL1[7:0]
	{ .regAddr= 0x3116, .regData= 0x24, .delayUs=0 }, // INCKSEL2[7:0]
	{ .regAddr= 0x3118, .regData= 0x80, .delayUs=0 }, // INCKSEL3[10:0]
	{ .regAddr= 0x3119, .regData= 0x00, .delayUs=0 }, // INCKSEL3[10:0]
	{ .regAddr= 0x311A, .regData= 0xE0, .delayUs=0 }, // INCKSEL4[10:0]
	{ .regAddr= 0x311B, .regData= 0x00, .delayUs=0 }, // INCKSEL4[10:0]
	{ .regAddr= 0x311E, .regData= 0x24, .delayUs=0 }, // INCKSEL5[7:0]
	{ .regAddr= 0x32D4, .regData= 0x21, .delayUs=0 },
	{ .regAddr= 0x32EC, .regData= 0xA1, .delayUs=0 },
	/*
	{ .regAddr= 0x344C, .regData= 0x2B, .delayUs=0 },
	{ .regAddr= 0x344D, .regData= 0x01, .delayUs=0 },
	{ .regAddr= 0x344E, .regData= 0xED, .delayUs=0 },
	{ .regAddr= 0x344F, .regData= 0x01, .delayUs=0 },
	{ .regAddr= 0x3450, .regData= 0xF6, .delayUs=0 },
	{ .regAddr= 0x3451, .regData= 0x02, .delayUs=0 },
	*/
	{ .regAddr= 0x3452, .regData= 0x7F, .delayUs=0 },
	{ .regAddr= 0x3453, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x358A, .regData= 0x04, .delayUs=0 },
	{ .regAddr= 0x35A1, .regData= 0x02, .delayUs=0 },
	/*
	{ .regAddr= 0x35EC, .regData= 0x27, .delayUs=0 },
	{ .regAddr= 0x35EE, .regData= 0x8D, .delayUs=0 },
	{ .regAddr= 0x35F0, .regData= 0x8D, .delayUs=0 },
	{ .regAddr= 0x35F2, .regData= 0x29, .delayUs=0 },
	*/
	{ .regAddr= 0x36BC, .regData= 0x0C, .delayUs=0 },
	{ .regAddr= 0x36CC, .regData= 0x53, .delayUs=0 },
	{ .regAddr= 0x36CD, .regData= 0x00, .delayUs=0 },
	{ .regAddr= 0x36CE, .regData= 0x3C, .delayUs=0 },
	{ .regAddr= 0x36D0, .regData= 0x8C, .delayUs=0 },
	{ .regAddr= 0x36D1, .regData= 0x00, .delayUs=0 },
	{ .regAddr= 0x36D2, .regData= 0x71, .delayUs=0 },
	{ .regAddr= 0x36D4, .regData= 0x3C, .delayUs=0 },
	{ .regAddr= 0x36D6, .regData= 0x53, .delayUs=0 },
	{ .regAddr= 0x36D7, .regData= 0x00, .delayUs=0 },
	{ .regAddr= 0x36D8, .regData= 0x71, .delayUs=0 },
	{ .regAddr= 0x36DA, .regData= 0x8C, .delayUs=0 },
	{ .regAddr= 0x36DB, .regData= 0x00, .delayUs=0 },
	{ .regAddr= 0x3701, .regData= 0x00, .delayUs=0 },  //AD 0:10bit, 3:12bit
	//{ .regAddr= 0x3720, .regData= 0x00, .delayUs=0 },
	{ .regAddr= 0x3724, .regData= 0x02, .delayUs=0 },
	{ .regAddr= 0x3726, .regData= 0x02, .delayUs=0 },
	{ .regAddr= 0x3732, .regData= 0x02, .delayUs=0 },
	{ .regAddr= 0x3734, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x3736, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x3742, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x3862, .regData= 0xE0, .delayUs=0 },
	{ .regAddr= 0x38CC, .regData= 0x30, .delayUs=0 },
	{ .regAddr= 0x38CD, .regData= 0x2F, .delayUs=0 },
	{ .regAddr= 0x395C, .regData= 0x0C, .delayUs=0 },
	/*
	{ .regAddr= 0x39A4, .regData= 0x07, .delayUs=0 },
	{ .regAddr= 0x39A8, .regData= 0x32, .delayUs=0 },
	{ .regAddr= 0x39AA, .regData= 0x32, .delayUs=0 },
	{ .regAddr= 0x39AC, .regData= 0x32, .delayUs=0 },
	{ .regAddr= 0x39AE, .regData= 0x32, .delayUs=0 },
	{ .regAddr= 0x39B0, .regData= 0x32, .delayUs=0 },
	{ .regAddr= 0x39B2, .regData= 0x2F, .delayUs=0 },
	{ .regAddr= 0x39B4, .regData= 0x2D, .delayUs=0 },
	{ .regAddr= 0x39B6, .regData= 0x28, .delayUs=0 },
	{ .regAddr= 0x39B8, .regData= 0x30, .delayUs=0 },
	{ .regAddr= 0x39BA, .regData= 0x30, .delayUs=0 },
	{ .regAddr= 0x39BC, .regData= 0x30, .delayUs=0 },
	{ .regAddr= 0x39BE, .regData= 0x30, .delayUs=0 },
	{ .regAddr= 0x39C0, .regData= 0x30, .delayUs=0 },
	{ .regAddr= 0x39C2, .regData= 0x2E, .delayUs=0 },
	{ .regAddr= 0x39C4, .regData= 0x2B, .delayUs=0 },
	{ .regAddr= 0x39C6, .regData= 0x25, .delayUs=0 },
	*/
	{ .regAddr= 0x3A42, .regData= 0xD1, .delayUs=0 },
	{ .regAddr= 0x3A4C, .regData= 0x77, .delayUs=0 },
	{ .regAddr= 0x3AE0, .regData= 0x02, .delayUs=0 },
	{ .regAddr= 0x3AEC, .regData= 0x0C, .delayUs=0 },
	{ .regAddr= 0x3B00, .regData= 0x2E, .delayUs=0 },
	{ .regAddr= 0x3B06, .regData= 0x29, .delayUs=0 },
	{ .regAddr= 0x3B98, .regData= 0x25, .delayUs=0 },
	{ .regAddr= 0x3B99, .regData= 0x21, .delayUs=0 },
	{ .regAddr= 0x3B9B, .regData= 0x13, .delayUs=0 },
	{ .regAddr= 0x3B9C, .regData= 0x13, .delayUs=0 },
	{ .regAddr= 0x3B9D, .regData= 0x13, .delayUs=0 },
	{ .regAddr= 0x3B9E, .regData= 0x13, .delayUs=0 },
	{ .regAddr= 0x3BA1, .regData= 0x00, .delayUs=0 },
	{ .regAddr= 0x3BA2, .regData= 0x06, .delayUs=0 },
	{ .regAddr= 0x3BA3, .regData= 0x0B, .delayUs=0 },
	{ .regAddr= 0x3BA4, .regData= 0x10, .delayUs=0 },
	{ .regAddr= 0x3BA5, .regData= 0x14, .delayUs=0 },
	{ .regAddr= 0x3BA6, .regData= 0x18, .delayUs=0 },
	{ .regAddr= 0x3BA7, .regData= 0x1A, .delayUs=0 },
	{ .regAddr= 0x3BA8, .regData= 0x1A, .delayUs=0 },
	{ .regAddr= 0x3BA9, .regData= 0x1A, .delayUs=0 },
	{ .regAddr= 0x3BAC, .regData= 0xED, .delayUs=0 },
	{ .regAddr= 0x3BAD, .regData= 0x01, .delayUs=0 },
	{ .regAddr= 0x3BAE, .regData= 0xF6, .delayUs=0 },
	{ .regAddr= 0x3BAF, .regData= 0x02, .delayUs=0 },
	{ .regAddr= 0x3BB0, .regData= 0xA2, .delayUs=0 },
	{ .regAddr= 0x3BB1, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x3BB2, .regData= 0xE0, .delayUs=0 },
	{ .regAddr= 0x3BB3, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x3BB4, .regData= 0xE0, .delayUs=0 },
	{ .regAddr= 0x3BB5, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x3BB6, .regData= 0xE0, .delayUs=0 },
	{ .regAddr= 0x3BB7, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x3BB8, .regData= 0xE0, .delayUs=0 },
	{ .regAddr= 0x3BBA, .regData= 0xE0, .delayUs=0 },
	{ .regAddr= 0x3BBC, .regData= 0xDA, .delayUs=0 },
	{ .regAddr= 0x3BBE, .regData= 0x88, .delayUs=0 },
	{ .regAddr= 0x3BC0, .regData= 0x44, .delayUs=0 },
	{ .regAddr= 0x3BC2, .regData= 0x7B, .delayUs=0 },
	{ .regAddr= 0x3BC4, .regData= 0xA2, .delayUs=0 },
	{ .regAddr= 0x3BC8, .regData= 0xBD, .delayUs=0 },
	{ .regAddr= 0x3BCA, .regData= 0xBD, .delayUs=0 },

	{ .regAddr= 0x4001, .regData= 0x03, .delayUs=0 }, // LANEMODE[2:0]
	{ .regAddr= 0x4004, .regData= 0x48, .delayUs=0 }, // TXCLKESC_FREQ[15:0]
	{ .regAddr= 0x4005, .regData= 0x09, .delayUs=0 }, //
	{ .regAddr= 0x400C, .regData= 0x00, .delayUs=0 }, // INCKSEL6

	{ .regAddr= 0x4018, .regData= 0x67, .delayUs=0 }, // TCLKPOST[15:0]
	{ .regAddr= 0x4019, .regData= 0x00, .delayUs=0 }, // TCLKPOST[15:0]
	{ .regAddr= 0x401A, .regData= 0x27, .delayUs=0 }, // TCLKPREPARE[15:0]
	{ .regAddr= 0x401C, .regData= 0x27, .delayUs=0 }, // TCLKTRAI[15:0]
	{ .regAddr= 0x401E, .regData= 0xB7, .delayUs=0 }, // TCLKZERO[15:0]
	{ .regAddr= 0x401F, .regData= 0x00, .delayUs=0 }, //
	{ .regAddr= 0x4020, .regData= 0x2F, .delayUs=0 }, // THSPREPARE[15:0]
	{ .regAddr= 0x4022, .regData= 0x4F, .delayUs=0 }, // THSZERO[15:0]
	{ .regAddr= 0x4024, .regData= 0x2F, .delayUs=0 }, // THSTRAI[15:0]
	{ .regAddr= 0x4026, .regData= 0x47, .delayUs=0 }, // THSEXIT[15:0]
	{ .regAddr= 0x4028, .regData= 0x27, .delayUs=0 }, // TLPX[15:0]
	{ .regAddr= 0x4074, .regData= 0x01, .delayUs=0 }, // INCKSEL7 [2:0]
    { .regAddr = 0x3000, .regData = 0x00, .delayUs = 24000}, /* leave standby */
};
/*
CV2003-AAQR Window cropping 2688x1860 CSI-2_4lane 37.125Mhz AD:12bit Output:12bit 1782Mbps Master Mode DOL HDR 2frame VC 25fps Integration Time LEF:10.003ms SEF:1.01ms
Ver9.0
*/
static drv_sensor_reg_info_t  cv2003_5MP_25fps_dol2_setting[] =
{
  { .regAddr= 0x3000, .regData= 0x01, .delayUs=0 }, /* enter standby */
	{ .regAddr= 0x3002, .regData= 0x01, .delayUs=0 }, /* master stop */
  { .regAddr= 0x3008,  .regData=  0x7F , .delayUs=0 }, // BCWAIT_TIME[9:0]
  { .regAddr= 0x300A,  .regData=  0x5B , .delayUs=0 }, // CPWAIT_TIME[9:0]
  { .regAddr= 0x301C,  .regData=  0x04 , .delayUs=0 }, // WINMODE[3:0]
  { .regAddr= 0x3023, .regData= 0x0, .delayUs=0 },  // VCMODE
  { .regAddr= 0x3024,  .regData=  0x8C , .delayUs=0 }, // VMAX[19:0]
  { .regAddr= 0x3025,  .regData=  0x0A , .delayUs=0 }, //
  { .regAddr= 0x302C,  .regData=  0x01 , .delayUs=0 }, // WDMODE[1:0]
  { .regAddr= 0x302D,  .regData=  0x01 , .delayUs=0 }, // WDSEL[1:0]
  { .regAddr= 0x3040,  .regData=  0x4C , .delayUs=0 }, // PIX_HST[12:0]
  { .regAddr= 0x3041,  .regData=  0x02 , .delayUs=0 }, //
  { .regAddr= 0x3042,  .regData=  0x80 , .delayUs=0 }, // PIX_HWIDTH[12:0]
  { .regAddr= 0x3043,  .regData=  0x0A , .delayUs=0 }, //
  { .regAddr= 0x3044,  .regData=  0x4C , .delayUs=0 }, // PIX_VST[12:0]
  { .regAddr= 0x3045,  .regData=  0x01 , .delayUs=0 }, //
  { .regAddr= 0x3046,  .regData=  0x88 , .delayUs=0 }, // PIX_VWIDTH[12:0]
  { .regAddr= 0x3047,  .regData=  0x0E , .delayUs=0 }, //
  { .regAddr= 0x3050,  .regData=  0xD2 , .delayUs=0 }, // SHR0[19:0]
  { .regAddr= 0x3051,  .regData=  0x0F , .delayUs=0 }, //
  { .regAddr= 0x3054,  .regData=  0x09 , .delayUs=0 }, // SHR1[19:0]
  { .regAddr= 0x3060,  .regData=  0x91 , .delayUs=0 }, // RHS1[19:0]
  { .regAddr= 0x30C1,  .regData=  0x00 , .delayUs=0 }, // XVS_DRV[1:0]
  { .regAddr= 0x30CF,  .regData=  0x01 , .delayUs=0 }, // XVSMSKCNT_INT[1:0]
  { .regAddr= 0x3116,  .regData=  0x24 , .delayUs=0 }, // INCKSEL2[7:0]
  { .regAddr= 0x311E,  .regData=  0x24 , .delayUs=0 }, // INCKSEL5[7:0]
  { .regAddr= 0x32D4,  .regData=  0x21 , .delayUs=0 }, // -
  { .regAddr= 0x32EC,  .regData=  0xA1 , .delayUs=0 }, // -

  { .regAddr= 0x344C,  .regData=  0x2B , .delayUs=0 }, // -
  { .regAddr= 0x344D,  .regData=  0x01 , .delayUs=0 }, // -
  { .regAddr= 0x344E,  .regData=  0xED , .delayUs=0 }, // -
  { .regAddr= 0x344F,  .regData=  0x01 , .delayUs=0 }, // -
  { .regAddr= 0x3450,  .regData=  0xF6 , .delayUs=0 }, // -
  { .regAddr= 0x3451,  .regData=  0x02 , .delayUs=0 }, // -

  { .regAddr= 0x3452,  .regData=  0x7F , .delayUs=0 }, // -
  { .regAddr= 0x3453,  .regData=  0x03 , .delayUs=0 }, // -
  { .regAddr= 0x358A,  .regData=  0x04 , .delayUs=0 }, // -
  { .regAddr= 0x35A1,  .regData=  0x02 , .delayUs=0 }, // -
 { .regAddr= 0x35EC,  .regData=  0x27 , .delayUs=0 }, // -
  { .regAddr= 0x35EE,  .regData=  0x8D , .delayUs=0 }, // -
  { .regAddr= 0x35F0,  .regData=  0x8D , .delayUs=0 }, // -
  { .regAddr= 0x35F2,  .regData=  0x29 , .delayUs=0 }, // -

  { .regAddr= 0x36BC,  .regData=  0x0C , .delayUs=0 }, // -
  { .regAddr= 0x36CC,  .regData=  0x53 , .delayUs=0 }, // -
  { .regAddr= 0x36CD,  .regData=  0x00 , .delayUs=0 }, // -
  { .regAddr= 0x36CE,  .regData=  0x3C , .delayUs=0 }, // -
  { .regAddr= 0x36D0,  .regData=  0x8C , .delayUs=0 }, // -
  { .regAddr= 0x36D1,  .regData=  0x00 , .delayUs=0 }, // -
  { .regAddr= 0x36D2,  .regData=  0x71 , .delayUs=0 }, // -
  { .regAddr= 0x36D4,  .regData=  0x3C , .delayUs=0 }, // -
  { .regAddr= 0x36D6,  .regData=  0x53 , .delayUs=0 }, // -
  { .regAddr= 0x36D7,  .regData=  0x00 , .delayUs=0 }, // -
  { .regAddr= 0x36D8,  .regData=  0x71 , .delayUs=0 }, // -
  { .regAddr= 0x36DA,  .regData=  0x8C , .delayUs=0 }, // -
  { .regAddr= 0x36DB,  .regData=  0x00 , .delayUs=0 }, // -
//	{ .regAddr= 0x3701, .regData= 0x00, .delayUs=0 },  //AD 0:10bit, 3:12bit
  { .regAddr= 0x3720,  .regData=  0x00 , .delayUs=0 }, // -
  { .regAddr= 0x3724,  .regData=  0x02 , .delayUs=0 }, // -
  { .regAddr= 0x3726,  .regData=  0x02 , .delayUs=0 }, // -
  { .regAddr= 0x3732,  .regData=  0x02 , .delayUs=0 }, // -
  { .regAddr= 0x3734,  .regData=  0x03 , .delayUs=0 }, // -
  { .regAddr= 0x3736,  .regData=  0x03 , .delayUs=0 }, // -
  { .regAddr= 0x3742,  .regData=  0x03 , .delayUs=0 }, // -
  { .regAddr= 0x3862,  .regData=  0xE0 , .delayUs=0 }, // -
  { .regAddr= 0x38CC,  .regData=  0x30 , .delayUs=0 }, // -
  { .regAddr= 0x38CD,  .regData=  0x2F , .delayUs=0 }, // -
  { .regAddr= 0x395C,  .regData=  0x0C , .delayUs=0 }, // -
  { .regAddr= 0x39A4,  .regData=  0x07 , .delayUs=0 }, // -
  { .regAddr= 0x39A8,  .regData=  0x32 , .delayUs=0 }, // -
  { .regAddr= 0x39AA,  .regData=  0x32 , .delayUs=0 }, // -
  { .regAddr= 0x39AC,  .regData=  0x32 , .delayUs=0 }, // -
  { .regAddr= 0x39AE,  .regData=  0x32 , .delayUs=0 }, // -
  { .regAddr= 0x39B0,  .regData=  0x32 , .delayUs=0 }, // -
  { .regAddr= 0x39B2,  .regData=  0x2F , .delayUs=0 }, // -
  { .regAddr= 0x39B4,  .regData=  0x2D , .delayUs=0 }, // -
  { .regAddr= 0x39B6,  .regData=  0x28 , .delayUs=0 }, // -
  { .regAddr= 0x39B8,  .regData=  0x30 , .delayUs=0 }, // -
  { .regAddr= 0x39BA,  .regData=  0x30 , .delayUs=0 }, // -
  { .regAddr= 0x39BC,  .regData=  0x30 , .delayUs=0 }, // -
  { .regAddr= 0x39BE,  .regData=  0x30 , .delayUs=0 }, // -
  { .regAddr= 0x39C0,  .regData=  0x30 , .delayUs=0 }, // -
  { .regAddr= 0x39C2,  .regData=  0x2E , .delayUs=0 }, // -
  { .regAddr= 0x39C4,  .regData=  0x2B , .delayUs=0 }, // -
  { .regAddr= 0x39C6,  .regData=  0x25 , .delayUs=0 }, // -
  { .regAddr= 0x3A42,  .regData=  0xD1 , .delayUs=0 }, // -
  { .regAddr= 0x3A4C,  .regData=  0x77 , .delayUs=0 }, // -
  { .regAddr= 0x3AE0,  .regData=  0x02 , .delayUs=0 }, // -
  { .regAddr= 0x3AEC,  .regData=  0x0C , .delayUs=0 }, // -
  { .regAddr= 0x3B00,  .regData=  0x2E , .delayUs=0 }, // -
  { .regAddr= 0x3B06,  .regData=  0x29 , .delayUs=0 }, // -
  { .regAddr= 0x3B98,  .regData=  0x25 , .delayUs=0 }, // -
  { .regAddr= 0x3B99,  .regData=  0x21 , .delayUs=0 }, // -
  { .regAddr= 0x3B9B,  .regData=  0x13 , .delayUs=0 }, // -
  { .regAddr= 0x3B9C,  .regData=  0x13 , .delayUs=0 }, // -
  { .regAddr= 0x3B9D,  .regData=  0x13 , .delayUs=0 }, // -
  { .regAddr= 0x3B9E,  .regData=  0x13 , .delayUs=0 }, // -
  { .regAddr= 0x3BA1,  .regData=  0x00 , .delayUs=0 }, // -
  { .regAddr= 0x3BA2,  .regData=  0x06 , .delayUs=0 }, // -
  { .regAddr= 0x3BA3,  .regData=  0x0B , .delayUs=0 }, // -
  { .regAddr= 0x3BA4,  .regData=  0x10 , .delayUs=0 }, // -
  { .regAddr= 0x3BA5,  .regData=  0x14 , .delayUs=0 }, // -
  { .regAddr= 0x3BA6,  .regData=  0x18 , .delayUs=0 }, // -
  { .regAddr= 0x3BA7,  .regData=  0x1A , .delayUs=0 }, // -
  { .regAddr= 0x3BA8,  .regData=  0x1A , .delayUs=0 }, // -
  { .regAddr= 0x3BA9,  .regData=  0x1A , .delayUs=0 }, // -
  { .regAddr= 0x3BAC,  .regData=  0xED , .delayUs=0 }, // -
  { .regAddr= 0x3BAD,  .regData=  0x01 , .delayUs=0 }, // -
  { .regAddr= 0x3BAE,  .regData=  0xF6 , .delayUs=0 }, // -
  { .regAddr= 0x3BAF,  .regData=  0x02 , .delayUs=0 }, // -
  { .regAddr= 0x3BB0,  .regData=  0xA2 , .delayUs=0 }, // -
  { .regAddr= 0x3BB1,  .regData=  0x03 , .delayUs=0 }, // -
  { .regAddr= 0x3BB2,  .regData=  0xE0 , .delayUs=0 }, // -
  { .regAddr= 0x3BB3,  .regData=  0x03 , .delayUs=0 }, // -
  { .regAddr= 0x3BB4,  .regData=  0xE0 , .delayUs=0 }, // -
  { .regAddr= 0x3BB5,  .regData=  0x03 , .delayUs=0 }, // -
  { .regAddr= 0x3BB6,  .regData=  0xE0 , .delayUs=0 }, // -
  { .regAddr= 0x3BB7,  .regData=  0x03 , .delayUs=0 }, // -
  { .regAddr= 0x3BB8,  .regData=  0xE0 , .delayUs=0 }, // -
  { .regAddr= 0x3BBA,  .regData=  0xE0 , .delayUs=0 }, // -
  { .regAddr= 0x3BBC,  .regData=  0xDA , .delayUs=0 }, // -
  { .regAddr= 0x3BBE,  .regData=  0x88 , .delayUs=0 }, // -
  { .regAddr= 0x3BC0,  .regData=  0x44 , .delayUs=0 }, // -
  { .regAddr= 0x3BC2,  .regData=  0x7B , .delayUs=0 }, // -
  { .regAddr= 0x3BC4,  .regData=  0xA2 , .delayUs=0 }, // -
  { .regAddr= 0x3BC8,  .regData=  0xBD , .delayUs=0 }, // -
  { .regAddr= 0x3BCA,  .regData=  0xBD , .delayUs=0 }, // -
  { .regAddr= 0x4004,  .regData=  0x48 , .delayUs=0 }, // TXCLKESC_FREQ[15:0]
  { .regAddr= 0x4005,  .regData=  0x09 , .delayUs=0 }, //
  { .regAddr = 0x3000, .regData = 0x00, .delayUs = 24000}, /* leave standby */
};

/*
CV2003-AAQR Window cropping 3840x2160 CSI-2_4lane 37.125Mhz AD:12bit Output:12bit 1782Mbps Master Mode DOL HDR 2frame VC 25fps Integration Time LEF:10.003ms SEF:1.01ms
Ver9.0
*/
static drv_sensor_reg_info_t  cv2003_8MP_20fps_dol2_setting[] =
{
  { .regAddr= 0x3000, .regData= 0x01, .delayUs=0 }, /* enter standby */
	{ .regAddr= 0x3002, .regData= 0x01, .delayUs=0 }, /* master stop */
  { .regAddr= 0x3008,  .regData=  0x7F , .delayUs=0 }, // BCWAIT_TIME[9:0]
  { .regAddr= 0x300A,  .regData=  0x5B , .delayUs=0 }, // CPWAIT_TIME[9:0]
  { .regAddr= 0x3023, .regData= 0x0, .delayUs=0 },  // VCMODE
  //17fps
 	//{ .regAddr= 0x3024, .regData= 0xCA, .delayUs=0 }, // VMAX[19:0]
	//{ .regAddr= 0x3025, .regData= 0x0A, .delayUs=0 }, // VMAX[19:0]

  { .regAddr= 0x3024, .regData= 0x40, .delayUs=0 }, // VMAX[19:0]
	{ .regAddr= 0x3025, .regData= 0x0A, .delayUs=0 }, // VMAX[19:0]
  { .regAddr= 0x302C,  .regData=  0x01 , .delayUs=0 }, // WDMODE[1:0]
  { .regAddr= 0x302D,  .regData=  0x01 , .delayUs=0 }, // WDSEL[1:0]
 	{ .regAddr= 0x3028, .regData= 0x00, .delayUs=0 }, // HMAX[15:0]
	{ .regAddr= 0x3029, .regData= 0x03, .delayUs=0 }, // HMAX[15:0]
  { .regAddr= 0x3050,  .regData=  0xD2 , .delayUs=0 }, // SHR0[19:0]
  { .regAddr= 0x3051,  .regData=  0x0F , .delayUs=0 }, //
  { .regAddr= 0x3054,  .regData=  0x09 , .delayUs=0 }, // SHR1[19:0]
  { .regAddr= 0x3060,  .regData=  0x91 , .delayUs=0 }, // RHS1[19:0]
  { .regAddr= 0x30C1,  .regData=  0x00 , .delayUs=0 }, // XVS_DRV[1:0]
  { .regAddr= 0x30CF,  .regData=  0x01 , .delayUs=0 }, // XVSMSKCNT_INT[1:0]
  { .regAddr= 0x3116,  .regData=  0x24 , .delayUs=0 }, // INCKSEL2[7:0]
  { .regAddr= 0x311E,  .regData=  0x24 , .delayUs=0 }, // INCKSEL5[7:0]
  { .regAddr= 0x32D4,  .regData=  0x21 , .delayUs=0 }, // -
  { .regAddr= 0x32EC,  .regData=  0xA1 , .delayUs=0 }, // -

  { .regAddr= 0x344C,  .regData=  0x2B , .delayUs=0 }, // -
  { .regAddr= 0x344D,  .regData=  0x01 , .delayUs=0 }, // -
  { .regAddr= 0x344E,  .regData=  0xED , .delayUs=0 }, // -
  { .regAddr= 0x344F,  .regData=  0x01 , .delayUs=0 }, // -
  { .regAddr= 0x3450,  .regData=  0xF6 , .delayUs=0 }, // -
  { .regAddr= 0x3451,  .regData=  0x02 , .delayUs=0 }, // -

  { .regAddr= 0x3452,  .regData=  0x7F , .delayUs=0 }, // -
  { .regAddr= 0x3453,  .regData=  0x03 , .delayUs=0 }, // -
  { .regAddr= 0x358A,  .regData=  0x04 , .delayUs=0 }, // -
  { .regAddr= 0x35A1,  .regData=  0x02 , .delayUs=0 }, // -
 { .regAddr= 0x35EC,  .regData=  0x27 , .delayUs=0 }, // -
  { .regAddr= 0x35EE,  .regData=  0x8D , .delayUs=0 }, // -
  { .regAddr= 0x35F0,  .regData=  0x8D , .delayUs=0 }, // -
  { .regAddr= 0x35F2,  .regData=  0x29 , .delayUs=0 }, // -

  { .regAddr= 0x36BC,  .regData=  0x0C , .delayUs=0 }, // -
  { .regAddr= 0x36CC,  .regData=  0x53 , .delayUs=0 }, // -
  { .regAddr= 0x36CD,  .regData=  0x00 , .delayUs=0 }, // -
  { .regAddr= 0x36CE,  .regData=  0x3C , .delayUs=0 }, // -
  { .regAddr= 0x36D0,  .regData=  0x8C , .delayUs=0 }, // -
  { .regAddr= 0x36D1,  .regData=  0x00 , .delayUs=0 }, // -
  { .regAddr= 0x36D2,  .regData=  0x71 , .delayUs=0 }, // -
  { .regAddr= 0x36D4,  .regData=  0x3C , .delayUs=0 }, // -
  { .regAddr= 0x36D6,  .regData=  0x53 , .delayUs=0 }, // -
  { .regAddr= 0x36D7,  .regData=  0x00 , .delayUs=0 }, // -
  { .regAddr= 0x36D8,  .regData=  0x71 , .delayUs=0 }, // -
  { .regAddr= 0x36DA,  .regData=  0x8C , .delayUs=0 }, // -
  { .regAddr= 0x36DB,  .regData=  0x00 , .delayUs=0 }, // -
//	{ .regAddr= 0x3701, .regData= 0x00, .delayUs=0 },  //AD 0:10bit, 3:12bit
  { .regAddr= 0x3720,  .regData=  0x00 , .delayUs=0 }, // -
  { .regAddr= 0x3724,  .regData=  0x02 , .delayUs=0 }, // -
  { .regAddr= 0x3726,  .regData=  0x02 , .delayUs=0 }, // -
  { .regAddr= 0x3732,  .regData=  0x02 , .delayUs=0 }, // -
  { .regAddr= 0x3734,  .regData=  0x03 , .delayUs=0 }, // -
  { .regAddr= 0x3736,  .regData=  0x03 , .delayUs=0 }, // -
  { .regAddr= 0x3742,  .regData=  0x03 , .delayUs=0 }, // -
  { .regAddr= 0x3862,  .regData=  0xE0 , .delayUs=0 }, // -
  { .regAddr= 0x38CC,  .regData=  0x30 , .delayUs=0 }, // -
  { .regAddr= 0x38CD,  .regData=  0x2F , .delayUs=0 }, // -
  { .regAddr= 0x395C,  .regData=  0x0C , .delayUs=0 }, // -
  { .regAddr= 0x39A4,  .regData=  0x07 , .delayUs=0 }, // -
  { .regAddr= 0x39A8,  .regData=  0x32 , .delayUs=0 }, // -
  { .regAddr= 0x39AA,  .regData=  0x32 , .delayUs=0 }, // -
  { .regAddr= 0x39AC,  .regData=  0x32 , .delayUs=0 }, // -
  { .regAddr= 0x39AE,  .regData=  0x32 , .delayUs=0 }, // -
  { .regAddr= 0x39B0,  .regData=  0x32 , .delayUs=0 }, // -
  { .regAddr= 0x39B2,  .regData=  0x2F , .delayUs=0 }, // -
  { .regAddr= 0x39B4,  .regData=  0x2D , .delayUs=0 }, // -
  { .regAddr= 0x39B6,  .regData=  0x28 , .delayUs=0 }, // -
  { .regAddr= 0x39B8,  .regData=  0x30 , .delayUs=0 }, // -
  { .regAddr= 0x39BA,  .regData=  0x30 , .delayUs=0 }, // -
  { .regAddr= 0x39BC,  .regData=  0x30 , .delayUs=0 }, // -
  { .regAddr= 0x39BE,  .regData=  0x30 , .delayUs=0 }, // -
  { .regAddr= 0x39C0,  .regData=  0x30 , .delayUs=0 }, // -
  { .regAddr= 0x39C2,  .regData=  0x2E , .delayUs=0 }, // -
  { .regAddr= 0x39C4,  .regData=  0x2B , .delayUs=0 }, // -
  { .regAddr= 0x39C6,  .regData=  0x25 , .delayUs=0 }, // -
  { .regAddr= 0x3A42,  .regData=  0xD1 , .delayUs=0 }, // -
  { .regAddr= 0x3A4C,  .regData=  0x77 , .delayUs=0 }, // -
  { .regAddr= 0x3AE0,  .regData=  0x02 , .delayUs=0 }, // -
  { .regAddr= 0x3AEC,  .regData=  0x0C , .delayUs=0 }, // -
  { .regAddr= 0x3B00,  .regData=  0x2E , .delayUs=0 }, // -
  { .regAddr= 0x3B06,  .regData=  0x29 , .delayUs=0 }, // -
  { .regAddr= 0x3B98,  .regData=  0x25 , .delayUs=0 }, // -
  { .regAddr= 0x3B99,  .regData=  0x21 , .delayUs=0 }, // -
  { .regAddr= 0x3B9B,  .regData=  0x13 , .delayUs=0 }, // -
  { .regAddr= 0x3B9C,  .regData=  0x13 , .delayUs=0 }, // -
  { .regAddr= 0x3B9D,  .regData=  0x13 , .delayUs=0 }, // -
  { .regAddr= 0x3B9E,  .regData=  0x13 , .delayUs=0 }, // -
  { .regAddr= 0x3BA1,  .regData=  0x00 , .delayUs=0 }, // -
  { .regAddr= 0x3BA2,  .regData=  0x06 , .delayUs=0 }, // -
  { .regAddr= 0x3BA3,  .regData=  0x0B , .delayUs=0 }, // -
  { .regAddr= 0x3BA4,  .regData=  0x10 , .delayUs=0 }, // -
  { .regAddr= 0x3BA5,  .regData=  0x14 , .delayUs=0 }, // -
  { .regAddr= 0x3BA6,  .regData=  0x18 , .delayUs=0 }, // -
  { .regAddr= 0x3BA7,  .regData=  0x1A , .delayUs=0 }, // -
  { .regAddr= 0x3BA8,  .regData=  0x1A , .delayUs=0 }, // -
  { .regAddr= 0x3BA9,  .regData=  0x1A , .delayUs=0 }, // -
  { .regAddr= 0x3BAC,  .regData=  0xED , .delayUs=0 }, // -
  { .regAddr= 0x3BAD,  .regData=  0x01 , .delayUs=0 }, // -
  { .regAddr= 0x3BAE,  .regData=  0xF6 , .delayUs=0 }, // -
  { .regAddr= 0x3BAF,  .regData=  0x02 , .delayUs=0 }, // -
  { .regAddr= 0x3BB0,  .regData=  0xA2 , .delayUs=0 }, // -
  { .regAddr= 0x3BB1,  .regData=  0x03 , .delayUs=0 }, // -
  { .regAddr= 0x3BB2,  .regData=  0xE0 , .delayUs=0 }, // -
  { .regAddr= 0x3BB3,  .regData=  0x03 , .delayUs=0 }, // -
  { .regAddr= 0x3BB4,  .regData=  0xE0 , .delayUs=0 }, // -
  { .regAddr= 0x3BB5,  .regData=  0x03 , .delayUs=0 }, // -
  { .regAddr= 0x3BB6,  .regData=  0xE0 , .delayUs=0 }, // -
  { .regAddr= 0x3BB7,  .regData=  0x03 , .delayUs=0 }, // -
  { .regAddr= 0x3BB8,  .regData=  0xE0 , .delayUs=0 }, // -
  { .regAddr= 0x3BBA,  .regData=  0xE0 , .delayUs=0 }, // -
  { .regAddr= 0x3BBC,  .regData=  0xDA , .delayUs=0 }, // -
  { .regAddr= 0x3BBE,  .regData=  0x88 , .delayUs=0 }, // -
  { .regAddr= 0x3BC0,  .regData=  0x44 , .delayUs=0 }, // -
  { .regAddr= 0x3BC2,  .regData=  0x7B , .delayUs=0 }, // -
  { .regAddr= 0x3BC4,  .regData=  0xA2 , .delayUs=0 }, // -
  { .regAddr= 0x3BC8,  .regData=  0xBD , .delayUs=0 }, // -
  { .regAddr= 0x3BCA,  .regData=  0xBD , .delayUs=0 }, // -
  { .regAddr= 0x4004,  .regData=  0x48 , .delayUs=0 }, // TXCLKESC_FREQ[15:0]
  { .regAddr= 0x4005,  .regData=  0x09 , .delayUs=0 }, //
  { .regAddr = 0x3000, .regData = 0x00, .delayUs = 24000}, /* leave standby */
};

//CV2003-AAQR All-pixel scan CSI-2_4lane 37.125Mhz AD:10bit Output:12bit 1782Mbps Master Mode fps Integration Time xx.xxms Gain:6dB
static drv_sensor_reg_info_t  cv2003_1080p_60fps_setting[] =
{
	{ .regAddr= 0x3000, .regData= 0x01, .delayUs=0 }, /* enter standby */
	{ .regAddr= 0x3003, .regData= 0x01, .delayUs=0 }, /* slave mode */
	{ .regAddr= 0x3002, .regData= 0x01, .delayUs=0 }, /* master stop */
	{ .regAddr= 0x3008, .regData= 0x7F, .delayUs=0 }, // BCWAIT_TIME[9:0]
	{ .regAddr= 0x3009, .regData= 0x00, .delayUs=0 }, // BCWAIT_TIME[9:0]
	{ .regAddr= 0x300A, .regData= 0x5B, .delayUs=0 }, // CPWAIT_TIME[9:0]
	{ .regAddr= 0x300B, .regData= 0x00, .delayUs=0 }, // CPWAIT_TIME[9:0]
	{ .regAddr= 0x301c, .regData= 0x0, .delayUs=0 },  //WINMODE
	{ .regAddr= 0x3020, .regData= 0x1, .delayUs=0 },  //horizontal 2 binning
	{ .regAddr= 0x3021, .regData= 0x1, .delayUs=0 },  //veritical 2 binning
	{ .regAddr= 0x3022, .regData= 0x1, .delayUs=0 },  //ADDMODE 2/2-line binning
	{ .regAddr= 0x3024, .regData= 0xCA, .delayUs=0 }, // VMAX[19:0]
	{ .regAddr= 0x3025, .regData= 0x08, .delayUs=0 }, // VMAX[19:0]
	{ .regAddr= 0x3026, .regData= 0x00, .delayUs=0 }, // VMAX[19:0]

	{ .regAddr= 0x3028, .regData= 0x26, .delayUs=0 }, // HMAX[15:0]
	{ .regAddr= 0x3029, .regData= 0x02, .delayUs=0 }, // HMAX[15:0]
    { .regAddr= 0x3030, .regData= 0x00, .delayUs=0 },  //normal
	{ .regAddr= 0x3031, .regData= 0x00, .delayUs=0 }, // AD:10 bit
	{ .regAddr= 0x3032, .regData= 0x01, .delayUs=0 }, // MD:12 bit
	{ .regAddr= 0x3033, .regData= 0x04, .delayUs=0 }, // SYS_MODE[3:0]

	{ .regAddr= 0x3050, .regData= 0x08, .delayUs=0 }, // SHR0[19:0]
	{ .regAddr= 0x3051, .regData= 0x00, .delayUs=0 }, // SHR0[19:0]
	{ .regAddr= 0x3052, .regData= 0x00, .delayUs=0 }, // SHR0[19:0]
	//{ .regAddr= 0x3050, .regData= 0x66, .delayUs=0 }, // SHR0[19:0]
	//{ .regAddr= 0x3051, .regData= 0x00, .delayUs=0 }, //
	//{ .regAddr= 0x3052, .regData= 0x00, .delayUs=0 }, //
	//{ .regAddr= 0x3090, .regData= 0x14, .delayUs=0 }, // GAIN_PCG_0[8:0]
	//{ .regAddr= 0x3091, .regData= 0x00, .delayUs=0 }, // GAIN_PCG_0[8:0]
	{ .regAddr= 0x3090, .regData= 0x00, .delayUs=0 }, // GAIN_PCG_0[8:0]
	{ .regAddr= 0x3091, .regData= 0x00, .delayUs=0 }, // GAIN_PCG_0[8:0]

 	{ .regAddr= 0x30C0, .regData= 0x2a, .delayUs=0,}, //vsync/hsync Output
	{ .regAddr= 0x30C1, .regData= 0x0f, .delayUs=0 }, // XVS_DRV[1:0] master=0, slave=0xf
 	{ .regAddr= 0x30CC, .regData= 0x00, .delayUs=0 }, //XVSLNG
 	{ .regAddr= 0x30CD, .regData= 0x00, .delayUs=0 }, //XHSLNG

	{ .regAddr= 0x30D9, .regData= 0x02, .delayUs=0 }, // DIG_CLP_VSTAET[4:0]
	{ .regAddr= 0x30DA, .regData= 0x01, .delayUs=0 }, // DIG_VLP_VNUM[1:0]

	{ .regAddr= 0x3115, .regData= 0x00, .delayUs=0 }, // INCKSEL1[7:0]
	{ .regAddr= 0x3116, .regData= 0x24, .delayUs=0 }, // INCKSEL2[7:0]
	{ .regAddr= 0x3118, .regData= 0xC0, .delayUs=0 }, // INCKSEL3[10:0]
	{ .regAddr= 0x3119, .regData= 0x00, .delayUs=0 }, // INCKSEL3[10:0]
	{ .regAddr= 0x311A, .regData= 0xE0, .delayUs=0 }, // INCKSEL4[10:0]
	{ .regAddr= 0x311B, .regData= 0x00, .delayUs=0 }, // INCKSEL4[10:0]
	{ .regAddr= 0x311E, .regData= 0x24, .delayUs=0 }, // INCKSEL5[7:0]
	{ .regAddr= 0x32D4, .regData= 0x21, .delayUs=0 },
	{ .regAddr= 0x32EC, .regData= 0xA1, .delayUs=0 },
	/*
	{ .regAddr= 0x344C, .regData= 0x2B, .delayUs=0 },
	{ .regAddr= 0x344D, .regData= 0x01, .delayUs=0 },
	{ .regAddr= 0x344E, .regData= 0xED, .delayUs=0 },
	{ .regAddr= 0x344F, .regData= 0x01, .delayUs=0 },
	{ .regAddr= 0x3450, .regData= 0xF6, .delayUs=0 },
	{ .regAddr= 0x3451, .regData= 0x02, .delayUs=0 },
	*/
	{ .regAddr= 0x3452, .regData= 0x7F, .delayUs=0 },
	{ .regAddr= 0x3453, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x358A, .regData= 0x04, .delayUs=0 },
	{ .regAddr= 0x35A1, .regData= 0x02, .delayUs=0 },
	/*
	{ .regAddr= 0x35EC, .regData= 0x27, .delayUs=0 },
	{ .regAddr= 0x35EE, .regData= 0x8D, .delayUs=0 },
	{ .regAddr= 0x35F0, .regData= 0x8D, .delayUs=0 },
	{ .regAddr= 0x35F2, .regData= 0x29, .delayUs=0 },
	*/
	{ .regAddr= 0x36BC, .regData= 0x0C, .delayUs=0 },
	{ .regAddr= 0x36CC, .regData= 0x53, .delayUs=0 },
	{ .regAddr= 0x36CD, .regData= 0x00, .delayUs=0 },
	{ .regAddr= 0x36CE, .regData= 0x3C, .delayUs=0 },
	{ .regAddr= 0x36D0, .regData= 0x8C, .delayUs=0 },
	{ .regAddr= 0x36D1, .regData= 0x00, .delayUs=0 },
	{ .regAddr= 0x36D2, .regData= 0x71, .delayUs=0 },
	{ .regAddr= 0x36D4, .regData= 0x3C, .delayUs=0 },
	{ .regAddr= 0x36D6, .regData= 0x53, .delayUs=0 },
	{ .regAddr= 0x36D7, .regData= 0x00, .delayUs=0 },
	{ .regAddr= 0x36D8, .regData= 0x71, .delayUs=0 },
	{ .regAddr= 0x36DA, .regData= 0x8C, .delayUs=0 },
	{ .regAddr= 0x36DB, .regData= 0x00, .delayUs=0 },
	{ .regAddr= 0x3701, .regData= 0x00, .delayUs=0 },  //AD 0:10bit, 3:12bit
	//{ .regAddr= 0x3720, .regData= 0x00, .delayUs=0 },
	{ .regAddr= 0x3724, .regData= 0x02, .delayUs=0 },
	{ .regAddr= 0x3726, .regData= 0x02, .delayUs=0 },
	{ .regAddr= 0x3732, .regData= 0x02, .delayUs=0 },
	{ .regAddr= 0x3734, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x3736, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x3742, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x3862, .regData= 0xE0, .delayUs=0 },
	{ .regAddr= 0x38CC, .regData= 0x30, .delayUs=0 },
	{ .regAddr= 0x38CD, .regData= 0x2F, .delayUs=0 },
	{ .regAddr= 0x395C, .regData= 0x0C, .delayUs=0 },
	/*
	{ .regAddr= 0x39A4, .regData= 0x07, .delayUs=0 },
	{ .regAddr= 0x39A8, .regData= 0x32, .delayUs=0 },
	{ .regAddr= 0x39AA, .regData= 0x32, .delayUs=0 },
	{ .regAddr= 0x39AC, .regData= 0x32, .delayUs=0 },
	{ .regAddr= 0x39AE, .regData= 0x32, .delayUs=0 },
	{ .regAddr= 0x39B0, .regData= 0x32, .delayUs=0 },
	{ .regAddr= 0x39B2, .regData= 0x2F, .delayUs=0 },
	{ .regAddr= 0x39B4, .regData= 0x2D, .delayUs=0 },
	{ .regAddr= 0x39B6, .regData= 0x28, .delayUs=0 },
	{ .regAddr= 0x39B8, .regData= 0x30, .delayUs=0 },
	{ .regAddr= 0x39BA, .regData= 0x30, .delayUs=0 },
	{ .regAddr= 0x39BC, .regData= 0x30, .delayUs=0 },
	{ .regAddr= 0x39BE, .regData= 0x30, .delayUs=0 },
	{ .regAddr= 0x39C0, .regData= 0x30, .delayUs=0 },
	{ .regAddr= 0x39C2, .regData= 0x2E, .delayUs=0 },
	{ .regAddr= 0x39C4, .regData= 0x2B, .delayUs=0 },
	{ .regAddr= 0x39C6, .regData= 0x25, .delayUs=0 },
	*/
	{ .regAddr= 0x3A42, .regData= 0xD1, .delayUs=0 },
	{ .regAddr= 0x3A4C, .regData= 0x77, .delayUs=0 },
	{ .regAddr= 0x3AE0, .regData= 0x02, .delayUs=0 },
	{ .regAddr= 0x3AEC, .regData= 0x0C, .delayUs=0 },
	{ .regAddr= 0x3B00, .regData= 0x2E, .delayUs=0 },
	{ .regAddr= 0x3B06, .regData= 0x29, .delayUs=0 },
	{ .regAddr= 0x3B98, .regData= 0x25, .delayUs=0 },
	{ .regAddr= 0x3B99, .regData= 0x21, .delayUs=0 },
	{ .regAddr= 0x3B9B, .regData= 0x13, .delayUs=0 },
	{ .regAddr= 0x3B9C, .regData= 0x13, .delayUs=0 },
	{ .regAddr= 0x3B9D, .regData= 0x13, .delayUs=0 },
	{ .regAddr= 0x3B9E, .regData= 0x13, .delayUs=0 },
	{ .regAddr= 0x3BA1, .regData= 0x00, .delayUs=0 },
	{ .regAddr= 0x3BA2, .regData= 0x06, .delayUs=0 },
	{ .regAddr= 0x3BA3, .regData= 0x0B, .delayUs=0 },
	{ .regAddr= 0x3BA4, .regData= 0x10, .delayUs=0 },
	{ .regAddr= 0x3BA5, .regData= 0x14, .delayUs=0 },
	{ .regAddr= 0x3BA6, .regData= 0x18, .delayUs=0 },
	{ .regAddr= 0x3BA7, .regData= 0x1A, .delayUs=0 },
	{ .regAddr= 0x3BA8, .regData= 0x1A, .delayUs=0 },
	{ .regAddr= 0x3BA9, .regData= 0x1A, .delayUs=0 },
	{ .regAddr= 0x3BAC, .regData= 0xED, .delayUs=0 },
	{ .regAddr= 0x3BAD, .regData= 0x01, .delayUs=0 },
	{ .regAddr= 0x3BAE, .regData= 0xF6, .delayUs=0 },
	{ .regAddr= 0x3BAF, .regData= 0x02, .delayUs=0 },
	{ .regAddr= 0x3BB0, .regData= 0xA2, .delayUs=0 },
	{ .regAddr= 0x3BB1, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x3BB2, .regData= 0xE0, .delayUs=0 },
	{ .regAddr= 0x3BB3, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x3BB4, .regData= 0xE0, .delayUs=0 },
	{ .regAddr= 0x3BB5, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x3BB6, .regData= 0xE0, .delayUs=0 },
	{ .regAddr= 0x3BB7, .regData= 0x03, .delayUs=0 },
	{ .regAddr= 0x3BB8, .regData= 0xE0, .delayUs=0 },
	{ .regAddr= 0x3BBA, .regData= 0xE0, .delayUs=0 },
	{ .regAddr= 0x3BBC, .regData= 0xDA, .delayUs=0 },
	{ .regAddr= 0x3BBE, .regData= 0x88, .delayUs=0 },
	{ .regAddr= 0x3BC0, .regData= 0x44, .delayUs=0 },
	{ .regAddr= 0x3BC2, .regData= 0x7B, .delayUs=0 },
	{ .regAddr= 0x3BC4, .regData= 0xA2, .delayUs=0 },
	{ .regAddr= 0x3BC8, .regData= 0xBD, .delayUs=0 },
	{ .regAddr= 0x3BCA, .regData= 0xBD, .delayUs=0 },

	{ .regAddr= 0x4001, .regData= 0x03, .delayUs=0 }, // LANEMODE[2:0]
	{ .regAddr= 0x4004, .regData= 0x48, .delayUs=0 }, // TXCLKESC_FREQ[15:0]
	{ .regAddr= 0x4005, .regData= 0x09, .delayUs=0 }, //
	{ .regAddr= 0x400C, .regData= 0x01, .delayUs=0 }, // INCKSEL6

	{ .regAddr= 0x4018, .regData= 0xb7, .delayUs=0 }, // TCLKPOST[15:0]
	{ .regAddr= 0x4019, .regData= 0x00, .delayUs=0 }, // TCLKPOST[15:0]
	{ .regAddr= 0x401A, .regData= 0x67, .delayUs=0 }, // TCLKPREPARE[15:0]
	{ .regAddr= 0x401B, .regData= 0x00, .delayUs=0 }, // TCLKPREPARE[15:0]
	{ .regAddr= 0x401C, .regData= 0x6f, .delayUs=0 }, // TCLKTRAI[15:0]
	{ .regAddr= 0x401D, .regData= 0x00, .delayUs=0 }, // TCLKTRAI[15:0]
	{ .regAddr= 0x401E, .regData= 0xdf, .delayUs=0 }, // TCLKZERO[15:0]
	{ .regAddr= 0x401F, .regData= 0x01, .delayUs=0 }, // TCLKZERO[15:0]
	{ .regAddr= 0x4020, .regData= 0x6F, .delayUs=0 }, // THSPREPARE[15:0]
	{ .regAddr= 0x4021, .regData= 0x00, .delayUs=0 }, // THSPREPARE[15:0]
	{ .regAddr= 0x4022, .regData= 0xcf, .delayUs=0 }, // THSZERO[15:0]
	{ .regAddr= 0x4023, .regData= 0x00, .delayUs=0 }, // THSZERO[15:0]
	{ .regAddr= 0x4024, .regData= 0x6F, .delayUs=0 }, // THSTRAI[15:0]
	{ .regAddr= 0x4025, .regData= 0x00, .delayUs=0 }, // THSTRAI[15:0]
	{ .regAddr= 0x4026, .regData= 0xb7, .delayUs=0 }, // THSEXIT[15:0]
	{ .regAddr= 0x4027, .regData= 0x00, .delayUs=0 }, // THSEXIT[15:0]
	{ .regAddr= 0x4028, .regData= 0x5f, .delayUs=0 }, // TLPX[15:0]
	{ .regAddr= 0x4029, .regData= 0x00, .delayUs=0 }, // TLPX[15:0]
	{ .regAddr= 0x4074, .regData= 0x00, .delayUs=0 }, // INCKSEL7 [2:0]
  { .regAddr = 0x3000, .regData = 0x00, .delayUs = 24000}, /* leave standby */
};

static drv_sensor_reg_info_t  cv2003_streamOn_setting[] =
{
    { .regAddr = 0x3002, .regData = 0x00, .delayUs = 0x0,}, /*master*/
//    { .regAddr = 0x30c0, .regData = 0x2a, .delayUs = 0x0,},
};

static drv_sensor_reg_info_t  cv2003_streamOff_setting[] =
{
	{ .regAddr= 0x3000, .regData= 0x01, .delayUs=0 }, /* enter standby */
    { .regAddr = 0x3002, .regData = 0x01, .delayUs = 0x0,},  /*stop master*/
};

static drv_sensor_exp_param_t cv2003_normal_exparams =
{
    .maxIntegrationTime = 2242  ,
    .minIntegrationTime = 1     ,
    .maxAnalogGain      = 33532,
    .minAnalogGain      = 1024  ,
    .lineTime           = 91022,
    .limitShortMid      = 0     ,
    .limitLong          = 0     ,
    .expLineMargin      = 1     ,
};
static drv_sensor_exp_param_t cv2003_2MP_30fps_exparams =
{
    .maxIntegrationTime = 2242  ,
    .minIntegrationTime = 1     ,
    .maxAnalogGain      = 33532,
    .minAnalogGain      = 1024  ,
    .lineTime           = 60681,
    .limitShortMid      = 0     ,
    .limitLong          = 0     ,
    .expLineMargin      = 1     ,
};

static drv_sensor_exp_param_t cv2003_8MP_20fps_exparams =
{
    .maxIntegrationTime = 2242  ,
    .minIntegrationTime = 1     ,
    .maxAnalogGain      = 33532,
    .minAnalogGain      = 1024  ,
    .lineTime           = 90681,
    .limitShortMid      = 0     ,
    .limitLong          = 0     ,
    .expLineMargin      = 1     ,
};

static drv_sensor_exp_param_t cv2003_1080p_20fps_exparams =
{
    .maxIntegrationTime = 2242  ,
    .minIntegrationTime = 1     ,
    .maxAnalogGain      = 33532,
    .minAnalogGain      = 1024  ,
    .lineTime           = 91022,
    .limitShortMid      = 0     ,
    .limitLong          = 0     ,
    .expLineMargin      = 1     ,
};

static drv_sensor_exp_param_t cv2003_60fps_exparams =
{
    .maxIntegrationTime = 2242  ,
    .minIntegrationTime = 1     ,
    .maxAnalogGain      = 33532,
    .minAnalogGain      = 1024  ,
    .lineTime           = 30340,
    .limitShortMid      = 0     ,
    .limitLong          = 0     ,
    .expLineMargin      = 1     ,
};

static drv_sensor_exp_param_t cv2003_30fps_dol2_exparams =
{
    .maxIntegrationTime = 500  ,//2242
    .minIntegrationTime = 8     ,
    .maxAnalogGain      = 33532,
    .minAnalogGain      = 1024  ,
    .lineTime           = 28139,
    .limitShortMid      = 500     ,
    .limitLong          = 4334     ,
    .expLineMargin      = 1     ,
};
static drv_sensor_exp_param_t cv2003_8MP_20fps_dol2_exparams =
{
    .maxIntegrationTime = 500  ,//2242
    .minIntegrationTime = 8     ,
    .maxAnalogGain      = 33532,
    .minAnalogGain      = 1024  ,
    .lineTime           = 41025,
    .limitShortMid      = 500     ,
    .limitLong          = 4470    ,
    .expLineMargin      = 1     ,
};
static drv_sensor_exp_param_t cv2003_5MP_25fps_dol2_exparams =
{
    .maxIntegrationTime = 500  ,//2242
    .minIntegrationTime = 8     ,
    .maxAnalogGain      = 33532,
    .minAnalogGain      = 1024  ,
    .lineTime           = 30340,
    .limitShortMid      = 500     ,
    .limitLong          = 4870    ,
    .expLineMargin      = 1     ,
};
static drv_sensor_exp_param_t cv2003_5MP_23fps_dol2_exparams =
{
    .maxIntegrationTime = 500  ,//2242
    .minIntegrationTime = 8     ,
    .maxAnalogGain      = 33532,
    .minAnalogGain      = 1024  ,
    .lineTime           = 41034,
    .limitShortMid      = 500     ,
    .limitLong          = 3800    ,
    .expLineMargin      = 1     ,
};
static drv_power_setting_t cv2003_power_up_setting[] =
{
    {.type = DRV_POWER_REG, .regAddr = 0x83c40004, .bitMask  = 0x01, .bitVal = 0x00, .delayUs = 100000}, /// 100ms
    {.type = DRV_POWER_REG, .regAddr = 0x83c40004, .bitMask  = 0x01, .bitVal = 0x01, .delayUs = 100000}  /// APB2 SENSOR_CTRL_REG 0x83c40004
};


static drv_power_setting_t cv2003_power_down_setting[] =
{
    {.type = DRV_POWER_REG, .regAddr = 0x83c40004, .bitMask  = 0x01, .bitVal = 0x00, .delayUs = 100000},
    {.type = DRV_POWER_REG, .regAddr = 0x83c40004, .bitMask  = 0x01, .bitVal = 0x01, .delayUs = 100000}
};

static sensor_drv_params_t cv2003_aux2_sensor_params =
{
    .sensorId     = 0x14, //0x145
    .sensorIdAddr = 0x3f12,
    .name         = "cv2003_aux2",
    .slaveInfo =
    {
        .hwType      = DRV_HW_I2C,
        .slaveAddr   = (0x20 >> 1),
        .channel     = 2,
        .regDataType = DRV_ADDR_TYPE_BYTE,
        .regAddrType = DRV_DATA_TYPE_WORD,
    },
    .powerUpSettings  =
    {
        .powerSetting = cv2003_power_up_setting,
        .size = sizeof(cv2003_power_up_setting) / sizeof(cv2003_power_up_setting[0]),
    },
    .powerDownSettings =
    {
        .powerSetting  = cv2003_power_down_setting,
        .size = sizeof(cv2003_power_down_setting) / sizeof(cv2003_power_down_setting[0]),
    },
    .activesize =
    {
        .width  = 3840,
        .height = 2160
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
        .maxFps = 10,
        .minFps = 10,
        .frameLength = 0x8ca,
        .expLineMargin = 1,
        .dim =
        {
            .width  = 3840,
            .height = 2160
        },
        .hdrMode   = SENSOR_NORMAL_MODE,
        .regConfig =
        {
            .regDataType = DRV_DATA_TYPE_BYTE,
            .regAddrType = DRV_ADDR_TYPE_WORD,
            .direction   = DRV_DIRECTION_WRITE,
            .settings    = cv2003_normal_setting,
            .settingSize = sizeof(cv2003_normal_setting) / sizeof (cv2003_normal_setting[0]),

        },
        .expParams   = &cv2003_normal_exparams,

        .intf = {
          .ln_cfg = {
            .dat_num = 2,
          },
          .link_freq = 594.0f,
        },
    },
    .modeSettings[1] =
    {
        .maxFps = 10,
        .minFps = 10,
        .frameLength = 0x8ca,
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
            .settings    = cv2003_1080p_setting,
            .settingSize = sizeof(cv2003_1080p_setting) / sizeof (cv2003_1080p_setting[0]),
        },
        .expParams   = &cv2003_normal_exparams,

        .intf = {
          .ln_cfg = {
            .dat_num = 4,
          },
          .pixel_rate = 100.0f,
        },
    },
    .modeSettings[2] =
    {
        .maxFps = 30,
        .minFps = 30,
        .frameLength = 0x8ca,
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
            .settings    = cv2003_1080p_30fps_setting,
            .settingSize = sizeof(cv2003_1080p_30fps_setting) / sizeof (cv2003_1080p_30fps_setting[0]),
            // .settings    = cv2003_1080p_30fps_setting_2lane,
            // .settingSize = sizeof(cv2003_1080p_30fps_setting_2lane) / sizeof (cv2003_1080p_30fps_setting_2lane[0]),
        },
        .expParams   = &cv2003_2MP_30fps_exparams,

        .intf = {
          .ln_cfg = {
            .dat_num = 4,
            // .dat_num = 2,
          },
          .link_freq = 891.0f,
          // .link_freq = 1782.0f,
        },
    },
    .modeSettings[3] =
    {
        .maxFps = 25,
        .minFps = 15,
        .frameLength = 0x8ca,
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
            .settings    = cv2003_1080p_20fps_setting,
            .settingSize = sizeof(cv2003_1080p_20fps_setting) / sizeof (cv2003_1080p_20fps_setting[0]),
        },
        .expParams   = &cv2003_2MP_30fps_exparams,

        .intf = {
          .ln_cfg = {
            .dat_num = 4,
          },
          .link_freq = 594.0f,
        },
    },
    .modeSettings[4] =
    {
        .maxFps = 60,
        .minFps = 60,
        .frameLength = 0x8ca,
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
            .settings    = cv2003_1080p_60fps_setting,
            .settingSize = sizeof(cv2003_1080p_60fps_setting) / sizeof (cv2003_1080p_60fps_setting[0]),
        },
        .expParams   = &cv2003_60fps_exparams,

        .intf = {
          .ln_cfg = {
            .dat_num = 4,
          },
          .link_freq = 1782.0f,
        },
    },
	.modeSettings[5] =
    {
        .maxFps = 20,
        .minFps = 20,
        .frameLength = 0x8ca,
        .expLineMargin = 1,
        .dim =
        {
            .width  = 3840,
            .height = 2160,
        },
        .hdrMode   = SENSOR_NORMAL_MODE,
        .regConfig =
        {
            .regDataType = DRV_DATA_TYPE_BYTE,
            .regAddrType = DRV_ADDR_TYPE_WORD,
            .direction   = DRV_DIRECTION_WRITE,
            .settings    = cv2003_4k_30fps_setting,
            .settingSize = sizeof(cv2003_4k_30fps_setting) / sizeof (cv2003_4k_30fps_setting[0]),
        },
        .expParams   = &cv2003_8MP_20fps_exparams,

        .intf = {
          .ln_cfg = {
            .dat_num = 4,
          },
          .link_freq = 891.0f,
        },
    },
	.modeSettings[6] =
    {
        .maxFps = 30,
        .minFps = 30,
        .frameLength = 0x8ec,
        .expLineMargin = 1,
        .dim =
        {
            .width  = 1920,
            .height = 1080,
        },
        .hdrMode   = SENSOR_DOL2_MODE,
        .regConfig =
        {
            .regDataType = DRV_DATA_TYPE_BYTE,
            .regAddrType = DRV_ADDR_TYPE_WORD,
            .direction   = DRV_DIRECTION_WRITE,
            .settings    = cv2003_1080p_30fps_dol2_setting,
            .settingSize = sizeof(cv2003_1080p_30fps_dol2_setting) / sizeof (cv2003_1080p_30fps_dol2_setting[0]),
        },
        .expParams   = &cv2003_30fps_dol2_exparams,

        .intf = {
          .ln_cfg = {
            .dat_num = 4,
          },
          .link_freq = 891.0f,
        },
    },

	.modeSettings[7] =
    {
        .maxFps = 20,
        .minFps = 20,
        .frameLength = 0x9c0,
        .expLineMargin = 1,
        .dim =
        {
            .width  = 3840,
            .height = 2160,
        },
        .hdrMode   = SENSOR_DOL2_MODE,
        .regConfig =
        {
            .regDataType = DRV_DATA_TYPE_BYTE,
            .regAddrType = DRV_ADDR_TYPE_WORD,
            .direction   = DRV_DIRECTION_WRITE,
            .settings    = cv2003_8MP_20fps_dol2_setting,
            .settingSize = sizeof(cv2003_8MP_20fps_dol2_setting) / sizeof (cv2003_8MP_20fps_dol2_setting[0]),
        },
        .expParams   = &cv2003_8MP_20fps_dol2_exparams,

        .intf = {
          .ln_cfg = {
            .dat_num = 4,
          },
          .link_freq = 1782.0f,
        },
    },
	.modeSettings[8] =
    {
        .maxFps = 30,
        .minFps = 30,
        .frameLength = 0x8ca,
        .expLineMargin = 1,
        .dim =
        {
            .width  = 2592,
            .height = 1536,
        },
        .hdrMode   = SENSOR_NORMAL_MODE,
        .regConfig =
        {
            .regDataType = DRV_DATA_TYPE_BYTE,
            .regAddrType = DRV_ADDR_TYPE_WORD,
            .direction   = DRV_DIRECTION_WRITE,
            .settings    = cv2003_400W_30fps_setting,
            .settingSize = sizeof(cv2003_400W_30fps_setting) / sizeof (cv2003_400W_30fps_setting[0]),
        },
        .expParams   = &cv2003_normal_exparams,

        .intf = {
          .ln_cfg = {
            .dat_num = 4,
          },
          .link_freq = 891.0f,
        },
    },
    	.modeSettings[9] =
    {
        .maxFps = 30,
        .minFps = 30,
        .frameLength = 0x8ca,
        .expLineMargin = 1,
        .dim =
        {
            .width  = 2688,
            .height = 1860,
        },
        .hdrMode   = SENSOR_NORMAL_MODE,
        .regConfig =
        {
            .regDataType = DRV_DATA_TYPE_BYTE,
            .regAddrType = DRV_ADDR_TYPE_WORD,
            .direction   = DRV_DIRECTION_WRITE,
            .settings    = cv2003_500W_30fps_setting,
            .settingSize = sizeof(cv2003_500W_30fps_setting) / sizeof (cv2003_500W_30fps_setting[0]),
        },
        .expParams   = &cv2003_normal_exparams,

        .intf = {
          .ln_cfg = {
            .dat_num = 4,
          },
          .link_freq = 891.0f,
        },
    },
    	.modeSettings[10] =
    {
        .maxFps = 30,
        .minFps = 30,
        .frameLength = 0x8CA,
        .expLineMargin = 1,
        .dim =
        {
            .width  = 2592,
            .height = 1536,
        },
        .hdrMode   = SENSOR_DOL2_MODE,
        .regConfig =
        {
            .regDataType = DRV_DATA_TYPE_BYTE,
            .regAddrType = DRV_ADDR_TYPE_WORD,
            .direction   = DRV_DIRECTION_WRITE,
            .settings    = cv2003_4MP_20fps_dol2_setting,
            .settingSize = sizeof(cv2003_4MP_20fps_dol2_setting) / sizeof (cv2003_4MP_20fps_dol2_setting[0]),
        },
        .expParams   = &cv2003_30fps_dol2_exparams,

        .intf = {
          .ln_cfg = {
            .dat_num = 4,
          },
          .link_freq = 1782.0f,
        },
    },
   	 .modeSettings[11] =
    {
        .maxFps = 25,
        .minFps = 25,
        .frameLength = 0xA8C,
        .expLineMargin = 1,
        .dim =
        {
            .width  = 2688,
            .height = 1860,
        },
        .hdrMode   = SENSOR_DOL2_MODE,
        .regConfig =
        {
            .regDataType = DRV_DATA_TYPE_BYTE,
            .regAddrType = DRV_ADDR_TYPE_WORD,
            .direction   = DRV_DIRECTION_WRITE,
            .settings    = cv2003_5MP_25fps_dol2_setting,
            .settingSize = sizeof(cv2003_5MP_25fps_dol2_setting) / sizeof (cv2003_5MP_25fps_dol2_setting[0]),
        },
        .expParams   = &cv2003_5MP_25fps_dol2_exparams,

        .intf = {
          .ln_cfg = {
            .dat_num = 4,
          },
          .link_freq = 1782.0f,
        },
    },
  	 .modeSettings[12] =
    {
        .maxFps = 25,
        .minFps = 25,
        .frameLength = 0x5BC,
        .expLineMargin = 1,
        .dim =
        {
            .width  = 2592,
            .height = 1536,
        },
        .hdrMode   = SENSOR_DOL2_MODE,
        .regConfig =
        {
            .regDataType = DRV_DATA_TYPE_BYTE,
            .regAddrType = DRV_ADDR_TYPE_WORD,
            .direction   = DRV_DIRECTION_WRITE,
            .settings    = cv2003_4MP_30fps_dol2_setting,
            .settingSize = sizeof(cv2003_4MP_30fps_dol2_setting) / sizeof (cv2003_4MP_30fps_dol2_setting[0]),
        },
        .expParams   = &cv2003_30fps_dol2_exparams,

        .intf = {
          .ln_cfg = {
            .dat_num = 4,
          },
          .link_freq = 1782.0f,
        },
    },
  	 .modeSettings[13] =
    {
        .maxFps = 23,
        .minFps = 23,
        .frameLength = 0x87A,
        .expLineMargin = 1,
        .dim =
        {
            .width  = 2688,
            .height = 1860,
        },
        .hdrMode   = SENSOR_DOL2_MODE,
        .regConfig =
        {
            .regDataType = DRV_DATA_TYPE_BYTE,
            .regAddrType = DRV_ADDR_TYPE_WORD,
            .direction   = DRV_DIRECTION_WRITE,
            .settings    = cv2003_5MP_23fps_dol2_setting,
            .settingSize = sizeof(cv2003_5MP_23fps_dol2_setting) / sizeof (cv2003_5MP_23fps_dol2_setting[0]),
        },
        .expParams   = &cv2003_5MP_23fps_dol2_exparams,

        .intf = {
          .ln_cfg = {
            .dat_num = 4,
          },
          .link_freq = 1782.0f,
        },
    },
    .modeSettings[14] =
    {
        .maxFps = 30,
        .minFps = 30,
        .frameLength = 0x8ca,
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
            //.settings    = cv2003_1080p_30fps_setting,
            //.settingSize = sizeof(cv2003_1080p_30fps_setting) / sizeof (cv2003_1080p_30fps_setting[0]),
            .settings    = cv2003_1080p_30fps_setting_2lane,
            .settingSize = sizeof(cv2003_1080p_30fps_setting_2lane) / sizeof (cv2003_1080p_30fps_setting_2lane[0]),
        },
        .expParams   = &cv2003_2MP_30fps_exparams,

        .intf = {
          .ln_cfg = {
            //.dat_num = 4,
            .dat_num = 2,
          },
          //.link_freq = 891.0f,
          .link_freq = 1782.0f,
        },
    },
    .streamOnSetting =
    {
        .regDataType = DRV_DATA_TYPE_BYTE,
        .regAddrType = DRV_ADDR_TYPE_WORD,
        .direction   = DRV_DIRECTION_WRITE,
        .settings    = cv2003_streamOn_setting,
        .settingSize = sizeof(cv2003_streamOn_setting) / sizeof (cv2003_streamOn_setting[0]),
    },

    .streamOffSetting =
    {
        .regDataType = DRV_DATA_TYPE_BYTE,
        .regAddrType = DRV_ADDR_TYPE_WORD,
        .direction   = DRV_DIRECTION_WRITE,
        .settings    = cv2003_streamOff_setting,
        .settingSize = sizeof(cv2003_streamOff_setting) / sizeof (cv2003_streamOff_setting[0]),
    },

    .fill_exposure_reg = CV2003_Fill_Exposure_Reg,
};


sensor_drv_params_t* load_cv2003_sensor_driver(void);

#endif

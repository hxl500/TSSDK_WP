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

#ifndef __IMX415_SENSOR_H__
#define __IMX415_SENSOR_H__

#include "sensor_common.h"
#include "log_utils.h"

isp_ret_t IMX415_Fill_Exposure_Reg(sensor_apply_exp_param_t * pExpParam, drv_regs_array_info_t * pRegInfo);
static drv_sensor_reg_info_t  imx415_1080p_setting[] =
{
	{ .regAddr= 0x3000, .regData= 0x01, .delayUs=0 }, /* enter standby */
	{ .regAddr= 0x3003, .regData= 0x00, .delayUs=0 }, /* master mode */
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
#ifdef IMX415_HARD_SYNC
	{ .regAddr= 0x30C0, .regData= 0x2a, .delayUs=0,}, //vsync/hsync Output
	{ .regAddr= 0x30C1, .regData= 0x00, .delayUs=0 }, // XVS_DRV[1:0] master=0, slave=0xf
	{ .regAddr= 0x30CC, .regData= 0x00, .delayUs=0 }, //XVSLNG
	{ .regAddr= 0x30CD, .regData= 0x00, .delayUs=0 }, //XHSLNG
#endif
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

//all pixel scan mode, 24Mhz, 2lane, lane clock 1440M, AD 10bit, MD 10bit
static drv_sensor_reg_info_t  imx415_4MP_30fps_setting[] =
{
	{ .regAddr= 0x3000, .regData= 0x01, .delayUs=0 }, /* enter standby */
	{ .regAddr= 0x3002, .regData= 0x01, .delayUs=0 }, /* master stop */
	{ .regAddr= 0x3008, .regData= 0x54, .delayUs=0 }, // BCWAIT_TIME[9:0]
	{ .regAddr= 0x3009, .regData= 0x00, .delayUs=0 }, // BCWAIT_TIME[9:0]
	{ .regAddr= 0x300A, .regData= 0x3B, .delayUs=0 }, // CPWAIT_TIME[9:0]
	{ .regAddr= 0x300B, .regData= 0x00, .delayUs=0 }, // CPWAIT_TIME[9:0]
//cropping
	{ .regAddr= 0x301c, .regData= 0x04, .delayUs=0 },  //WINMODE  4h:window croping mode
	{ .regAddr= 0x3040, .regData= 0x00, .delayUs=0 },  //PIX_HST effective pixel Start position(H direction)
	{ .regAddr= 0x3041, .regData= 0x00, .delayUs=0 },
	{ .regAddr= 0x3042, .regData= 0x08, .delayUs=0 },  //PIX_HWIDTH  4h:window croping mode, 2568(multiplies of 24)
	{ .regAddr= 0x3043, .regData= 0x0A, .delayUs=0 },
	{ .regAddr= 0x3044, .regData= 0x00, .delayUs=0 },  //PIX_VST  4h:window croping mode
	{ .regAddr= 0x3045, .regData= 0x00, .delayUs=0 },
	{ .regAddr= 0x3046, .regData= 0x80, .delayUs=0 },   //PIX_VWIDTH  4h:window croping mode, 1440
	{ .regAddr= 0x3047, .regData= 0x0b, .delayUs=0 },

	{ .regAddr= 0x3020, .regData= 0x0, .delayUs=0 },  //horizontal 2 binning
	{ .regAddr= 0x3021, .regData= 0x0, .delayUs=0 },  //veritical 2 binning
	{ .regAddr= 0x3022, .regData= 0x0, .delayUs=0 },  //ADDMODE 2/2-line binning
	// { .regAddr= 0x3024, .regData= 0xCA, .delayUs=0 }, // VMAX[19:0]
	// { .regAddr= 0x3025, .regData= 0x08, .delayUs=0 }, // VMAX[19:0]
	// { .regAddr= 0x3026, .regData= 0x00, .delayUs=0 }, // VMAX[19:0]
	// { .regAddr= 0x3024, .regData= 0xa6, .delayUs=0 }, // VMAX[19:0]
	// { .regAddr= 0x3025, .regData= 0x0e, .delayUs=0 }, // VMAX[19:0]
	{ .regAddr= 0x3024, .regData= 0x94, .delayUs=0 }, // VMAX[19:0]
	{ .regAddr= 0x3025, .regData= 0x11, .delayUs=0 }, // VMAX[19:0]
	{ .regAddr= 0x3026, .regData= 0x00, .delayUs=0 }, // VMAX[19:0]

	{ .regAddr= 0x3028, .regData= 0x2a, .delayUs=0 }, // HMAX[15:0]
	{ .regAddr= 0x3029, .regData= 0x04, .delayUs=0 }, // HMAX[15:0]
  	{ .regAddr= 0x3030, .regData= 0x00, .delayUs=0 },  //normal
	{ .regAddr= 0x3031, .regData= 0x00, .delayUs=0 }, // AD:10 bit
	{ .regAddr= 0x3032, .regData= 0x00, .delayUs=0 }, // MD:10 bit
	{ .regAddr= 0x3033, .regData= 0x08, .delayUs=0 }, // SYS_MODE[3:0]

	{ .regAddr= 0x302c, .regData= 0x00, .delayUs=0 }, // WDMODE[1:0] 0:normal exposure
	{ .regAddr= 0x302d, .regData= 0x00, .delayUs=0 }, // WDSEL[1:0] 0:normal exposure
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

	{ .regAddr= 0x30D9, .regData= 0x06, .delayUs=0 }, // DIG_CLP_VSTAET[4:0]
	{ .regAddr= 0x30DA, .regData= 0x02, .delayUs=0 }, // DIG_VLP_VNUM[1:0]

	{ .regAddr= 0x3115, .regData= 0x00, .delayUs=0 }, // INCKSEL1[7:0]
	{ .regAddr= 0x3116, .regData= 0x23, .delayUs=0 }, // INCKSEL2[7:0]
	{ .regAddr= 0x3118, .regData= 0xB4, .delayUs=0 }, // INCKSEL3[10:0]
	{ .regAddr= 0x3119, .regData= 0x00, .delayUs=0 }, // INCKSEL3[10:0]
	{ .regAddr= 0x311A, .regData= 0xFC, .delayUs=0 }, // INCKSEL4[10:0]
	{ .regAddr= 0x311B, .regData= 0x00, .delayUs=0 }, // INCKSEL4[10:0]
	{ .regAddr= 0x311E, .regData= 0x23, .delayUs=0 }, // INCKSEL5[7:0]
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
	{ .regAddr= 0x4004, .regData= 0x00, .delayUs=0 }, // TXCLKESC_FREQ[15:0]
	{ .regAddr= 0x4005, .regData= 0x06, .delayUs=0 }, //
	{ .regAddr= 0x400C, .regData= 0x01, .delayUs=0 }, // INCKSEL6

	{ .regAddr= 0x4018, .regData= 0x9f, .delayUs=0 }, // TCLKPOST[15:0]
	{ .regAddr= 0x4019, .regData= 0x00, .delayUs=0 }, // TCLKPOST[15:0]
	{ .regAddr= 0x401A, .regData= 0x57, .delayUs=0 }, // TCLKPREPARE[15:0]
	{ .regAddr= 0x401B, .regData= 0x00, .delayUs=0 }, // TCLKPREPARE[15:0]
	{ .regAddr= 0x401C, .regData= 0x57, .delayUs=0 }, // TCLKTRAI[15:0]
	{ .regAddr= 0x401D, .regData= 0x00, .delayUs=0 }, // TCLKTRAI[15:0]
	{ .regAddr= 0x401E, .regData= 0x87, .delayUs=0 }, // TCLKZERO[15:0]
	{ .regAddr= 0x401F, .regData= 0x01, .delayUs=0 }, // TCLKZERO[15:0]
	{ .regAddr= 0x4020, .regData= 0x5F, .delayUs=0 }, // THSPREPARE[15:0]
	{ .regAddr= 0x4021, .regData= 0x00, .delayUs=0 }, // THSPREPARE[15:0]
	{ .regAddr= 0x4022, .regData= 0xA7, .delayUs=0 }, // THSZERO[15:0]
	{ .regAddr= 0x4023, .regData= 0x00, .delayUs=0 }, // THSZERO[15:0]
	{ .regAddr= 0x4024, .regData= 0x5F, .delayUs=0 }, // THSTRAI[15:0]
	{ .regAddr= 0x4025, .regData= 0x00, .delayUs=0 }, // THSTRAI[15:0]
	{ .regAddr= 0x4026, .regData= 0x97, .delayUs=0 }, // THSEXIT[15:0]
	{ .regAddr= 0x4027, .regData= 0x00, .delayUs=0 }, // THSEXIT[15:0]
	{ .regAddr= 0x4028, .regData= 0x4F, .delayUs=0 }, // TLPX[15:0]
	{ .regAddr= 0x4029, .regData= 0x00, .delayUs=0 }, // TLPX[15:0]
	{ .regAddr= 0x4074, .regData= 0x00, .delayUs=0 }, // INCKSEL7 [2:0]
    { .regAddr = 0x3000, .regData = 0x00, .delayUs = 24000}, /* leave standby */
};

//all pixel scan mode, 24Mhz, 2lane, lane clock 1440M, AD 10bit, MD 10bit
static drv_sensor_reg_info_t  imx415_5MP_4_3_30fps_setting[] =
{
	{ .regAddr= 0x3000, .regData= 0x01, .delayUs=0 }, /* enter standby */
	{ .regAddr= 0x3002, .regData= 0x01, .delayUs=0 }, /* master stop */
	{ .regAddr= 0x3008, .regData= 0x54, .delayUs=0 }, // BCWAIT_TIME[9:0]
	{ .regAddr= 0x3009, .regData= 0x00, .delayUs=0 }, // BCWAIT_TIME[9:0]
	{ .regAddr= 0x300A, .regData= 0x3B, .delayUs=0 }, // CPWAIT_TIME[9:0]
	{ .regAddr= 0x300B, .regData= 0x00, .delayUs=0 }, // CPWAIT_TIME[9:0]
//cropping
	{ .regAddr= 0x301c, .regData= 0x04, .delayUs=0 },  //WINMODE  4h:window croping mode
	{ .regAddr= 0x3040, .regData= 0x00, .delayUs=0 },  //PIX_HST effective pixel Start position(H direction)
	{ .regAddr= 0x3041, .regData= 0x00, .delayUs=0 },
	{ .regAddr= 0x3042, .regData= 0x20, .delayUs=0 },  //PIX_HWIDTH  4h:window croping mode, 2592(multiplies of 24)
	{ .regAddr= 0x3043, .regData= 0x0A, .delayUs=0 },
	{ .regAddr= 0x3044, .regData= 0x00, .delayUs=0 },  //PIX_VST  4h:window croping mode
	{ .regAddr= 0x3045, .regData= 0x00, .delayUs=0 },
	{ .regAddr= 0x3046, .regData= 0x38, .delayUs=0 },   //PIX_VWIDTH  4h:window croping mode, 1944->1948for de purple line
	{ .regAddr= 0x3047, .regData= 0x0F, .delayUs=0 },

	{ .regAddr= 0x3020, .regData= 0x0, .delayUs=0 },  //horizontal 2 binning
	{ .regAddr= 0x3021, .regData= 0x0, .delayUs=0 },  //veritical 2 binning
	{ .regAddr= 0x3022, .regData= 0x0, .delayUs=0 },  //ADDMODE 2/2-line binning
	{ .regAddr= 0x3024, .regData= 0xCA, .delayUs=0 }, // VMAX[19:0]
	{ .regAddr= 0x3025, .regData= 0x08, .delayUs=0 }, // VMAX[19:0]
	{ .regAddr= 0x3026, .regData= 0x00, .delayUs=0 }, // VMAX[19:0]

	{ .regAddr= 0x3028, .regData= 0x2a, .delayUs=0 }, // HMAX[15:0]
	{ .regAddr= 0x3029, .regData= 0x04, .delayUs=0 }, // HMAX[15:0]
  	{ .regAddr= 0x3030, .regData= 0x00, .delayUs=0 },  //normal
	{ .regAddr= 0x3031, .regData= 0x00, .delayUs=0 }, // AD:10 bit
	{ .regAddr= 0x3032, .regData= 0x00, .delayUs=0 }, // MD:10 bit
	{ .regAddr= 0x3033, .regData= 0x08, .delayUs=0 }, // SYS_MODE[3:0]

	{ .regAddr= 0x302c, .regData= 0x00, .delayUs=0 }, // WDMODE[1:0] 0:normal exposure
	{ .regAddr= 0x302d, .regData= 0x00, .delayUs=0 }, // WDSEL[1:0] 0:normal exposure
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

	{ .regAddr= 0x30D9, .regData= 0x06, .delayUs=0 }, // DIG_CLP_VSTAET[4:0]
	{ .regAddr= 0x30DA, .regData= 0x02, .delayUs=0 }, // DIG_VLP_VNUM[1:0]

	{ .regAddr= 0x3115, .regData= 0x00, .delayUs=0 }, // INCKSEL1[7:0]
	{ .regAddr= 0x3116, .regData= 0x23, .delayUs=0 }, // INCKSEL2[7:0]
	{ .regAddr= 0x3118, .regData= 0xB4, .delayUs=0 }, // INCKSEL3[10:0]
	{ .regAddr= 0x3119, .regData= 0x00, .delayUs=0 }, // INCKSEL3[10:0]
	{ .regAddr= 0x311A, .regData= 0xFC, .delayUs=0 }, // INCKSEL4[10:0]
	{ .regAddr= 0x311B, .regData= 0x00, .delayUs=0 }, // INCKSEL4[10:0]
	{ .regAddr= 0x311E, .regData= 0x23, .delayUs=0 }, // INCKSEL5[7:0]
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
	{ .regAddr= 0x4004, .regData= 0x00, .delayUs=0 }, // TXCLKESC_FREQ[15:0]
	{ .regAddr= 0x4005, .regData= 0x06, .delayUs=0 }, //
	{ .regAddr= 0x400C, .regData= 0x01, .delayUs=0 }, // INCKSEL6

	{ .regAddr= 0x4018, .regData= 0x9f, .delayUs=0 }, // TCLKPOST[15:0]
	{ .regAddr= 0x4019, .regData= 0x00, .delayUs=0 }, // TCLKPOST[15:0]
	{ .regAddr= 0x401A, .regData= 0x57, .delayUs=0 }, // TCLKPREPARE[15:0]
	{ .regAddr= 0x401B, .regData= 0x00, .delayUs=0 }, // TCLKPREPARE[15:0]
	{ .regAddr= 0x401C, .regData= 0x57, .delayUs=0 }, // TCLKTRAI[15:0]
	{ .regAddr= 0x401D, .regData= 0x00, .delayUs=0 }, // TCLKTRAI[15:0]
	{ .regAddr= 0x401E, .regData= 0x87, .delayUs=0 }, // TCLKZERO[15:0]
	{ .regAddr= 0x401F, .regData= 0x01, .delayUs=0 }, // TCLKZERO[15:0]
	{ .regAddr= 0x4020, .regData= 0x5F, .delayUs=0 }, // THSPREPARE[15:0]
	{ .regAddr= 0x4021, .regData= 0x00, .delayUs=0 }, // THSPREPARE[15:0]
	{ .regAddr= 0x4022, .regData= 0xA7, .delayUs=0 }, // THSZERO[15:0]
	{ .regAddr= 0x4023, .regData= 0x00, .delayUs=0 }, // THSZERO[15:0]
	{ .regAddr= 0x4024, .regData= 0x5F, .delayUs=0 }, // THSTRAI[15:0]
	{ .regAddr= 0x4025, .regData= 0x00, .delayUs=0 }, // THSTRAI[15:0]
	{ .regAddr= 0x4026, .regData= 0x97, .delayUs=0 }, // THSEXIT[15:0]
	{ .regAddr= 0x4027, .regData= 0x00, .delayUs=0 }, // THSEXIT[15:0]
	{ .regAddr= 0x4028, .regData= 0x4F, .delayUs=0 }, // TLPX[15:0]
	{ .regAddr= 0x4029, .regData= 0x00, .delayUs=0 }, // TLPX[15:0]
	{ .regAddr= 0x4074, .regData= 0x00, .delayUs=0 }, // INCKSEL7 [2:0]
    { .regAddr = 0x3000, .regData = 0x00, .delayUs = 24000}, /* leave standby */
};

//IMX415-AAQR All-pixel scan CSI-2_2lane win mode, 24Mhz AD:10bit Output:10bit, lane clk 720Mbps, Master Mode
static drv_sensor_reg_info_t imx415_4mp_15fps_setting[] =
{
	{ .regAddr= 0x3000, .regData= 0x01, .delayUs=0 }, /* enter standby */
	{ .regAddr= 0x3002, .regData= 0x01, .delayUs=0 }, /* master stop */
	{ .regAddr= 0x3008, .regData= 0x54, .delayUs=0 }, // BCWAIT_TIME[9:0]
	{ .regAddr= 0x3009, .regData= 0x00, .delayUs=0 }, // BCWAIT_TIME[9:0]
	{ .regAddr= 0x300A, .regData= 0x3B, .delayUs=0 }, // CPWAIT_TIME[9:0]
	{ .regAddr= 0x300B, .regData= 0x00, .delayUs=0 }, // CPWAIT_TIME[9:0]
	{ .regAddr= 0x3020, .regData= 0x0, .delayUs=0 },  //horizontal 2 binning
	{ .regAddr= 0x3021, .regData= 0x0, .delayUs=0 },  //veritical 2 binning
	{ .regAddr= 0x3022, .regData= 0x0, .delayUs=0 },  //ADDMODE all pixel
	{ .regAddr= 0x3024, .regData= 0xCA, .delayUs=0 }, // VMAX[19:0]
	{ .regAddr= 0x3025, .regData= 0x08, .delayUs=0 }, // VMAX[19:0]
	{ .regAddr= 0x3026, .regData= 0x00, .delayUs=0 }, // VMAX[19:0]

	{ .regAddr= 0x3028, .regData= 0xF0, .delayUs=0 }, // HMAX[15:0]
	{ .regAddr= 0x3029, .regData= 0x07, .delayUs=0 }, // HMAX[15:0]
    { .regAddr= 0x3030, .regData= 0x00, .delayUs=0 },  //normal
	{ .regAddr= 0x3031, .regData= 0x00, .delayUs=0 }, // AD:10 bit
	{ .regAddr= 0x3032, .regData= 0x00, .delayUs=0 }, // MD:10 bit
	{ .regAddr= 0x3033, .regData= 0x09, .delayUs=0 }, // SYS_MODE[3:0]

	{ .regAddr= 0x301c, .regData= 0x4, .delayUs=0 },  //WINMODE
	{ .regAddr= 0x302C,  .regData=  0x00 , .delayUs=0 }, // WDMODE[1:0]
	{ .regAddr= 0x302D,  .regData=  0x00 , .delayUs=0 }, // WDSEL[1:0]
	{ .regAddr= 0x3040, .regData= 0x00, .delayUs=0 },  //PIX_HST effective pixel Start position(H direction)
	{ .regAddr= 0x3041, .regData= 0x00, .delayUs=0 },
	{ .regAddr= 0x3042, .regData= 0x08, .delayUs=0 },  //PIX_HWIDTH  4h:window croping mode, 2568(multiplies of 24)
	{ .regAddr= 0x3043, .regData= 0x0A, .delayUs=0 },
	{ .regAddr= 0x3044, .regData= 0x00, .delayUs=0 },  //PIX_VST  4h:window croping mode
	{ .regAddr= 0x3045, .regData= 0x00, .delayUs=0 },
	{ .regAddr= 0x3046, .regData= 0x80, .delayUs=0 },   //PIX_VWIDTH  4h:window croping mode, 1440
	{ .regAddr= 0x3047, .regData= 0x0b, .delayUs=0 },

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
	{ .regAddr= 0x30CF, .regData= 0x00, .delayUs=0 }, // XVSMSKCNT_INT[1:0]

	{ .regAddr= 0x30D9, .regData= 0x06, .delayUs=0 }, // DIG_CLP_VSTAET[4:0]
	{ .regAddr= 0x30DA, .regData= 0x02, .delayUs=0 }, // DIG_VLP_VNUM[1:0]

	{ .regAddr= 0x3115, .regData= 0x00, .delayUs=0 }, // INCKSEL1[7:0]
	{ .regAddr= 0x3116, .regData= 0x23, .delayUs=0 }, // INCKSEL2[7:0]
	{ .regAddr= 0x3118, .regData= 0xB4, .delayUs=0 }, // INCKSEL3[10:0]
	{ .regAddr= 0x3119, .regData= 0x00, .delayUs=0 }, // INCKSEL3[10:0]
	{ .regAddr= 0x311A, .regData= 0xFC, .delayUs=0 }, // INCKSEL4[10:0]
	{ .regAddr= 0x311B, .regData= 0x00, .delayUs=0 }, // INCKSEL4[10:0]
	{ .regAddr= 0x311E, .regData= 0x23, .delayUs=0 }, // INCKSEL5[7:0]
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
	{ .regAddr= 0x4004, .regData= 0x00, .delayUs=0 }, // TXCLKESC_FREQ[15:0]
	{ .regAddr= 0x4005, .regData= 0x06, .delayUs=0 }, //
	{ .regAddr= 0x400C, .regData= 0x00, .delayUs=0 }, // INCKSEL6

	{ .regAddr= 0x4018, .regData= 0x9f, .delayUs=0 }, // TCLKPOST[15:0]
	{ .regAddr= 0x4019, .regData= 0x00, .delayUs=0 }, // TCLKPOST[15:0]
	{ .regAddr= 0x401A, .regData= 0x57, .delayUs=0 }, // TCLKPREPARE[15:0]
	{ .regAddr= 0x401B, .regData= 0x00, .delayUs=0 }, // TCLKPREPARE[15:0]
	{ .regAddr= 0x401C, .regData= 0x57, .delayUs=0 }, // TCLKTRAI[15:0]
	{ .regAddr= 0x401D, .regData= 0x00, .delayUs=0 }, // TCLKTRAI[15:0]
	{ .regAddr= 0x401E, .regData= 0x87, .delayUs=0 }, // TCLKZERO[15:0]
	{ .regAddr= 0x401F, .regData= 0x01, .delayUs=0 }, // TCLKZERO[15:0]
	{ .regAddr= 0x4020, .regData= 0x5F, .delayUs=0 }, // THSPREPARE[15:0]
	{ .regAddr= 0x4021, .regData= 0x00, .delayUs=0 }, // THSPREPARE[15:0]
	{ .regAddr= 0x4022, .regData= 0xA7, .delayUs=0 }, // THSZERO[15:0]
	{ .regAddr= 0x4023, .regData= 0x00, .delayUs=0 }, // THSZERO[15:0]
	{ .regAddr= 0x4024, .regData= 0x5F, .delayUs=0 }, // THSTRAI[15:0]
	{ .regAddr= 0x4025, .regData= 0x00, .delayUs=0 }, // THSTRAI[15:0]
	{ .regAddr= 0x4026, .regData= 0x97, .delayUs=0 }, // THSEXIT[15:0]
	{ .regAddr= 0x4027, .regData= 0x00, .delayUs=0 }, // THSEXIT[15:0]
	{ .regAddr= 0x4028, .regData= 0x4f, .delayUs=0 }, // TLPX[15:0]
	{ .regAddr= 0x4029, .regData= 0x00, .delayUs=0 }, // TLPX[15:0]
	{ .regAddr= 0x4074, .regData= 0x01, .delayUs=0 }, // INCKSEL7 [2:0]
    { .regAddr = 0x3000, .regData = 0x00, .delayUs = 24000}, /* leave standby */
};

#if 0
//IMX415-AAQR All-pixel scan CSI-2_2lane win mode, 24Mhz AD:10bit Output:12bit, lane clk 1440Mbps, Master Mode
static drv_sensor_reg_info_t imx415_1080p_30fps_setting[] =
{
	{ .regAddr= 0x3000, .regData= 0x01, .delayUs=0 }, /* enter standby */
	{ .regAddr= 0x3002, .regData= 0x01, .delayUs=0 }, /* master stop */
	{ .regAddr= 0x3008, .regData= 0x54, .delayUs=0 }, // BCWAIT_TIME[9:0]
	{ .regAddr= 0x3009, .regData= 0x00, .delayUs=0 }, // BCWAIT_TIME[9:0]
	{ .regAddr= 0x300A, .regData= 0x3B, .delayUs=0 }, // CPWAIT_TIME[9:0]
	{ .regAddr= 0x300B, .regData= 0x00, .delayUs=0 }, // CPWAIT_TIME[9:0]
	{ .regAddr= 0x301c, .regData= 0x4, .delayUs=0 },  //WINMODE
	{ .regAddr= 0x3020, .regData= 0x0, .delayUs=0 },  //horizontal 2 binning
	{ .regAddr= 0x3021, .regData= 0x0, .delayUs=0 },  //veritical 2 binning
	{ .regAddr= 0x3022, .regData= 0x0, .delayUs=0 },  //ADDMODE all pixel
	{ .regAddr= 0x3024, .regData= 0xCA, .delayUs=0 }, // VMAX[19:0]
	{ .regAddr= 0x3025, .regData= 0x08, .delayUs=0 }, // VMAX[19:0]
	{ .regAddr= 0x3026, .regData= 0x00, .delayUs=0 }, // VMAX[19:0]

	{ .regAddr= 0x3028, .regData= 0x2A, .delayUs=0 }, // HMAX[15:0]
	{ .regAddr= 0x3029, .regData= 0x04, .delayUs=0 }, // HMAX[15:0]
    { .regAddr= 0x3030, .regData= 0x00, .delayUs=0 },  //normal
	{ .regAddr= 0x3031, .regData= 0x00, .delayUs=0 }, // AD:10 bit
	{ .regAddr= 0x3032, .regData= 0x00, .delayUs=0 }, // MD:10 bit
	{ .regAddr= 0x3033, .regData= 0x08, .delayUs=0 }, // SYS_MODE[3:0]

	{ .regAddr= 0x302C,  .regData=  0x00 , .delayUs=0 }, // WDMODE[1:0]
	{ .regAddr= 0x302D,  .regData=  0x00 , .delayUs=0 }, // WDSEL[1:0]
	{ .regAddr= 0x3040,  .regData=  0xC0 , .delayUs=0 }, // PIX_HST[12:0]
	{ .regAddr= 0x3041,  .regData=  0x03 , .delayUs=0 }, // 
	{ .regAddr= 0x3042,  .regData=  0x80 , .delayUs=0 }, // PIX_HWIDTH[12:0]
	{ .regAddr= 0x3043,  .regData=  0x07 , .delayUs=0 }, // 
	{ .regAddr= 0x3044,  .regData=  0x1C , .delayUs=0 }, // PIX_VST[12:0]
	{ .regAddr= 0x3045,  .regData=  0x02 , .delayUs=0 }, // 
	{ .regAddr= 0x3046,  .regData=  0x70 , .delayUs=0 }, // PIX_VWIDTH[12:0]
	{ .regAddr= 0x3047,  .regData=  0x08 , .delayUs=0 }, // 

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
	{ .regAddr= 0x30CF, .regData= 0x00, .delayUs=0 }, // XVSMSKCNT_INT[1:0]

	{ .regAddr= 0x30D9, .regData= 0x06, .delayUs=0 }, // DIG_CLP_VSTAET[4:0]
	{ .regAddr= 0x30DA, .regData= 0x02, .delayUs=0 }, // DIG_VLP_VNUM[1:0]

	{ .regAddr= 0x3115, .regData= 0x00, .delayUs=0 }, // INCKSEL1[7:0]
	{ .regAddr= 0x3116, .regData= 0x23, .delayUs=0 }, // INCKSEL2[7:0]
	{ .regAddr= 0x3118, .regData= 0xB4, .delayUs=0 }, // INCKSEL3[10:0]
	{ .regAddr= 0x3119, .regData= 0x00, .delayUs=0 }, // INCKSEL3[10:0]
	{ .regAddr= 0x311A, .regData= 0xFC, .delayUs=0 }, // INCKSEL4[10:0]
	{ .regAddr= 0x311B, .regData= 0x00, .delayUs=0 }, // INCKSEL4[10:0]
	{ .regAddr= 0x311E, .regData= 0x23, .delayUs=0 }, // INCKSEL5[7:0]
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
	{ .regAddr= 0x4004, .regData= 0x00, .delayUs=0 }, // TXCLKESC_FREQ[15:0]
	{ .regAddr= 0x4005, .regData= 0x06, .delayUs=0 }, //
	{ .regAddr= 0x400C, .regData= 0x01, .delayUs=0 }, // INCKSEL6

	{ .regAddr= 0x4018, .regData= 0x9f, .delayUs=0 }, // TCLKPOST[15:0]
	{ .regAddr= 0x4019, .regData= 0x00, .delayUs=0 }, // TCLKPOST[15:0]
	{ .regAddr= 0x401A, .regData= 0x57, .delayUs=0 }, // TCLKPREPARE[15:0]
	{ .regAddr= 0x401B, .regData= 0x00, .delayUs=0 }, // TCLKPREPARE[15:0]
	{ .regAddr= 0x401C, .regData= 0x57, .delayUs=0 }, // TCLKTRAI[15:0]
	{ .regAddr= 0x401D, .regData= 0x00, .delayUs=0 }, // TCLKTRAI[15:0]
	{ .regAddr= 0x401E, .regData= 0x87, .delayUs=0 }, // TCLKZERO[15:0]
	{ .regAddr= 0x401F, .regData= 0x01, .delayUs=0 }, // TCLKZERO[15:0]
	{ .regAddr= 0x4020, .regData= 0x5F, .delayUs=0 }, // THSPREPARE[15:0]
	{ .regAddr= 0x4021, .regData= 0x00, .delayUs=0 }, // THSPREPARE[15:0]
	{ .regAddr= 0x4022, .regData= 0xA7, .delayUs=0 }, // THSZERO[15:0]
	{ .regAddr= 0x4023, .regData= 0x00, .delayUs=0 }, // THSZERO[15:0]
	{ .regAddr= 0x4024, .regData= 0x5F, .delayUs=0 }, // THSTRAI[15:0]
	{ .regAddr= 0x4025, .regData= 0x00, .delayUs=0 }, // THSTRAI[15:0]
	{ .regAddr= 0x4026, .regData= 0x97, .delayUs=0 }, // THSEXIT[15:0]
	{ .regAddr= 0x4027, .regData= 0x00, .delayUs=0 }, // THSEXIT[15:0]
	{ .regAddr= 0x4028, .regData= 0x4f, .delayUs=0 }, // TLPX[15:0]
	{ .regAddr= 0x4029, .regData= 0x00, .delayUs=0 }, // TLPX[15:0]
	{ .regAddr= 0x4074, .regData= 0x00, .delayUs=0 }, // INCKSEL7 [2:0]
    { .regAddr = 0x3000, .regData = 0x00, .delayUs = 24000}, /* leave standby */
};
#else
//IMX415-AAQR Bining mode CSI-2_2lane 37.125Mhz AD:10bit Output:12bit 1782Mbps Master Mode fps Integration Time xx.xxms Gain:6dB
static drv_sensor_reg_info_t  imx415_1080p_30fps_setting[] =
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
	{ .regAddr= 0x302c, .regData= 0x00, .delayUs=0 }, // WDMODE[1:0]
	{ .regAddr= 0x302d, .regData= 0x00, .delayUs=0 }, // WDSEL[1:0]
	{ .regAddr= 0x30CF, .regData= 0x00, .delayUs=0 }, // XVSMSKCNT_INT[1:0]

    { .regAddr= 0x3030, .regData= 0x00, .delayUs = 0x0},
	{ .regAddr= 0x3031, .regData= 0x00, .delayUs=0 }, // AD:10 bit
	{ .regAddr= 0x3032, .regData= 0x00, .delayUs=0 }, // MD:12 bit
	{ .regAddr= 0x3033, .regData= 0x04, .delayUs=0 }, // SYS_MODE[3:0]
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

#ifdef IMX415_HARD_SYNC
    { .regAddr= 0x3003, .regData= 0x00, .delayUs=0 }, /* master mode */
    { .regAddr= 0x30C0, .regData= 0x2a, .delayUs=0,}, //vsync/hsync Output
    { .regAddr= 0x30C1, .regData= 0x00, .delayUs=0 }, // XVS_DRV[1:0] master=0, slave=0xf
    { .regAddr= 0x30CC, .regData= 0x00, .delayUs=0 }, //XVSLNG
    { .regAddr= 0x30CD, .regData= 0x00, .delayUs=0 }, //XHSLNG
#endif
	{ .regAddr = 0x3000, .regData = 0x00, .delayUs = 24000}, /* standby */
};
#endif


//IMX415-AAQR Bining mode CSI-2_2lane 37.125Mhz AD:10bit Output:12bit 1782Mbps Master Mode fps:14(measured fps=10+- when dt56)
static drv_sensor_reg_info_t  imx415_1080p_14fps_setting[] =
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
	{ .regAddr= 0x3024, .regData= 0xca, .delayUs=0 }, // VMAX[19:0]
	{ .regAddr= 0x3025, .regData= 0x08, .delayUs=0 }, // VMAX[19:0]
	{ .regAddr= 0x3026, .regData= 0x00, .delayUs=0 }, // VMAX[19:0]
	{ .regAddr= 0x3028, .regData= 0x4C, .delayUs=0 }, // HMAX[15:0]
	{ .regAddr= 0x3029, .regData= 0x04, .delayUs=0 }, // HMAX[15:0]
	{ .regAddr= 0x302c, .regData= 0x00, .delayUs=0 }, // WDMODE[1:0]
	{ .regAddr= 0x302d, .regData= 0x00, .delayUs=0 }, // WDSEL[1:0]
	{ .regAddr= 0x30CF, .regData= 0x00, .delayUs=0 }, // XVSMSKCNT_INT[1:0]

    { .regAddr= 0x3030, .regData= 0x00, .delayUs = 0x0},
	{ .regAddr= 0x3031, .regData= 0x00, .delayUs=0 }, // AD:10 bit
	{ .regAddr= 0x3032, .regData= 0x00, .delayUs=0 }, // MD:12 bit
	{ .regAddr= 0x3033, .regData= 0x04, .delayUs=0 }, // SYS_MODE[3:0]
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

#ifdef IMX415_HARD_SYNC
    { .regAddr= 0x3003, .regData= 0x00, .delayUs=0 }, /* master mode */
    { .regAddr= 0x30C0, .regData= 0x2a, .delayUs=0,}, //vsync/hsync Output
    { .regAddr= 0x30C1, .regData= 0x00, .delayUs=0 }, // XVS_DRV[1:0] master=0, slave=0xf
    { .regAddr= 0x30CC, .regData= 0x00, .delayUs=0 }, //XVSLNG
    { .regAddr= 0x30CD, .regData= 0x00, .delayUs=0 }, //XHSLNG
#endif
	{ .regAddr = 0x3000, .regData = 0x00, .delayUs = 24000}, /* standby */
};


//IMX415-AAQR All-pixel scan CSI-2_2lane win mode, 24Mhz AD:10bit Output:10bit, lane clk 720Mbps, Master Mode
static drv_sensor_reg_info_t imx415_1080p_15fps_setting[] =
{
	{ .regAddr= 0x3000, .regData= 0x01, .delayUs=0 }, /* enter standby */
	{ .regAddr= 0x3002, .regData= 0x01, .delayUs=0 }, /* master stop */
	{ .regAddr= 0x3008, .regData= 0x54, .delayUs=0 }, // BCWAIT_TIME[9:0]
	{ .regAddr= 0x3009, .regData= 0x00, .delayUs=0 }, // BCWAIT_TIME[9:0]
	{ .regAddr= 0x300A, .regData= 0x3B, .delayUs=0 }, // CPWAIT_TIME[9:0]
	{ .regAddr= 0x300B, .regData= 0x00, .delayUs=0 }, // CPWAIT_TIME[9:0]
	{ .regAddr= 0x3020, .regData= 0x0, .delayUs=0 },  //horizontal 2 binning
	{ .regAddr= 0x3021, .regData= 0x0, .delayUs=0 },  //veritical 2 binning
	{ .regAddr= 0x3022, .regData= 0x0, .delayUs=0 },  //ADDMODE all pixel
	{ .regAddr= 0x3024, .regData= 0xCA, .delayUs=0 }, // VMAX[19:0]
	{ .regAddr= 0x3025, .regData= 0x08, .delayUs=0 }, // VMAX[19:0]
	{ .regAddr= 0x3026, .regData= 0x00, .delayUs=0 }, // VMAX[19:0]

	{ .regAddr= 0x3028, .regData= 0xF0, .delayUs=0 }, // HMAX[15:0]
	{ .regAddr= 0x3029, .regData= 0x07, .delayUs=0 }, // HMAX[15:0]
    { .regAddr= 0x3030, .regData= 0x00, .delayUs=0 },  //normal
	{ .regAddr= 0x3031, .regData= 0x00, .delayUs=0 }, // AD:10 bit
	{ .regAddr= 0x3032, .regData= 0x00, .delayUs=0 }, // MD:10 bit
	{ .regAddr= 0x3033, .regData= 0x09, .delayUs=0 }, // SYS_MODE[3:0]

	{ .regAddr= 0x301c, .regData= 0x4, .delayUs=0 },  //WINMODE
	{ .regAddr= 0x302C,  .regData=  0x00 , .delayUs=0 }, // WDMODE[1:0]
	{ .regAddr= 0x302D,  .regData=  0x00 , .delayUs=0 }, // WDSEL[1:0]
	{ .regAddr= 0x3040,  .regData=  0xC0 , .delayUs=0 }, // PIX_HST[12:0]
	{ .regAddr= 0x3041,  .regData=  0x03 , .delayUs=0 }, // 
	{ .regAddr= 0x3042,  .regData=  0x80 , .delayUs=0 }, // PIX_HWIDTH[12:0]
	{ .regAddr= 0x3043,  .regData=  0x07 , .delayUs=0 }, // 
	{ .regAddr= 0x3044,  .regData=  0x1C , .delayUs=0 }, // PIX_VST[12:0]
	{ .regAddr= 0x3045,  .regData=  0x02 , .delayUs=0 }, // 
	{ .regAddr= 0x3046,  .regData=  0x70 , .delayUs=0 }, // PIX_VWIDTH[12:0]
	{ .regAddr= 0x3047,  .regData=  0x08 , .delayUs=0 }, // 

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
	{ .regAddr= 0x30CF, .regData= 0x00, .delayUs=0 }, // XVSMSKCNT_INT[1:0]

	{ .regAddr= 0x30D9, .regData= 0x06, .delayUs=0 }, // DIG_CLP_VSTAET[4:0]
	{ .regAddr= 0x30DA, .regData= 0x02, .delayUs=0 }, // DIG_VLP_VNUM[1:0]

	{ .regAddr= 0x3115, .regData= 0x00, .delayUs=0 }, // INCKSEL1[7:0]
	{ .regAddr= 0x3116, .regData= 0x23, .delayUs=0 }, // INCKSEL2[7:0]
	{ .regAddr= 0x3118, .regData= 0xB4, .delayUs=0 }, // INCKSEL3[10:0]
	{ .regAddr= 0x3119, .regData= 0x00, .delayUs=0 }, // INCKSEL3[10:0]
	{ .regAddr= 0x311A, .regData= 0xFC, .delayUs=0 }, // INCKSEL4[10:0]
	{ .regAddr= 0x311B, .regData= 0x00, .delayUs=0 }, // INCKSEL4[10:0]
	{ .regAddr= 0x311E, .regData= 0x23, .delayUs=0 }, // INCKSEL5[7:0]
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
	{ .regAddr= 0x4004, .regData= 0x00, .delayUs=0 }, // TXCLKESC_FREQ[15:0]
	{ .regAddr= 0x4005, .regData= 0x06, .delayUs=0 }, //
	{ .regAddr= 0x400C, .regData= 0x00, .delayUs=0 }, // INCKSEL6

	{ .regAddr= 0x4018, .regData= 0x9f, .delayUs=0 }, // TCLKPOST[15:0]
	{ .regAddr= 0x4019, .regData= 0x00, .delayUs=0 }, // TCLKPOST[15:0]
	{ .regAddr= 0x401A, .regData= 0x57, .delayUs=0 }, // TCLKPREPARE[15:0]
	{ .regAddr= 0x401B, .regData= 0x00, .delayUs=0 }, // TCLKPREPARE[15:0]
	{ .regAddr= 0x401C, .regData= 0x57, .delayUs=0 }, // TCLKTRAI[15:0]
	{ .regAddr= 0x401D, .regData= 0x00, .delayUs=0 }, // TCLKTRAI[15:0]
	{ .regAddr= 0x401E, .regData= 0x87, .delayUs=0 }, // TCLKZERO[15:0]
	{ .regAddr= 0x401F, .regData= 0x01, .delayUs=0 }, // TCLKZERO[15:0]
	{ .regAddr= 0x4020, .regData= 0x5F, .delayUs=0 }, // THSPREPARE[15:0]
	{ .regAddr= 0x4021, .regData= 0x00, .delayUs=0 }, // THSPREPARE[15:0]
	{ .regAddr= 0x4022, .regData= 0xA7, .delayUs=0 }, // THSZERO[15:0]
	{ .regAddr= 0x4023, .regData= 0x00, .delayUs=0 }, // THSZERO[15:0]
	{ .regAddr= 0x4024, .regData= 0x5F, .delayUs=0 }, // THSTRAI[15:0]
	{ .regAddr= 0x4025, .regData= 0x00, .delayUs=0 }, // THSTRAI[15:0]
	{ .regAddr= 0x4026, .regData= 0x97, .delayUs=0 }, // THSEXIT[15:0]
	{ .regAddr= 0x4027, .regData= 0x00, .delayUs=0 }, // THSEXIT[15:0]
	{ .regAddr= 0x4028, .regData= 0x4f, .delayUs=0 }, // TLPX[15:0]
	{ .regAddr= 0x4029, .regData= 0x00, .delayUs=0 }, // TLPX[15:0]
	{ .regAddr= 0x4074, .regData= 0x01, .delayUs=0 }, // INCKSEL7 [2:0]
    { .regAddr = 0x3000, .regData = 0x00, .delayUs = 24000}, /* leave standby */
};

//2/2-line binning mode, 24Mhz, 2lane, lane clock 1440M, AD 10bit, MD 12bit
static drv_sensor_reg_info_t  imx415_1080p_30fps_dol2_setting[] =
{
	{ .regAddr= 0x3000, .regData= 0x01, .delayUs=0 }, /* enter standby */
	{ .regAddr= 0x3002, .regData= 0x01, .delayUs=0 }, /* master stop */
	{ .regAddr= 0x3008, .regData= 0x54, .delayUs=0 }, // BCWAIT_TIME[9:0]
	{ .regAddr= 0x3009, .regData= 0x00, .delayUs=0 }, // BCWAIT_TIME[9:0]
	{ .regAddr= 0x300A, .regData= 0x3B, .delayUs=0 }, // CPWAIT_TIME[9:0]
	{ .regAddr= 0x300B, .regData= 0x00, .delayUs=0 }, // CPWAIT_TIME[9:0]
#if 1
	{ .regAddr= 0x301c, .regData= 0x0, .delayUs=0 },  //WINMODE
	{ .regAddr= 0x3020, .regData= 0x1, .delayUs=0 },  //horizontal 2 binning
	{ .regAddr= 0x3021, .regData= 0x1, .delayUs=0 },  //veritical 2 binning
	{ .regAddr= 0x3022, .regData= 0x1, .delayUs=0 },  //ADDMODE 2/2-line binning
	{ .regAddr= 0x3024, .regData= 0xC0, .delayUs=0 }, // VMAX[19:0]
	{ .regAddr= 0x3025, .regData= 0x09, .delayUs=0 }, // VMAX[19:0]
	{ .regAddr= 0x3026, .regData= 0x00, .delayUs=0 }, // VMAX[19:0]

	{ .regAddr= 0x3028, .regData= 0xc0, .delayUs=0 }, // HMAX[15:0]
	{ .regAddr= 0x3029, .regData= 0x03, .delayUs=0 }, // HMAX[15:0]
#else
	{ .regAddr= 0x301c, .regData= 0x4, .delayUs=0 },  //WINMODE
	{ .regAddr= 0x3020, .regData= 0x0, .delayUs=0 },  //horizontal 2 binning
	{ .regAddr= 0x3021, .regData= 0x0, .delayUs=0 },  //veritical 2 binning
	{ .regAddr= 0x3022, .regData= 0x0, .delayUs=0 },  //ADDMODE all pixel
	{ .regAddr= 0x3024, .regData= 0xCA, .delayUs=0 }, // VMAX[19:0]
	{ .regAddr= 0x3025, .regData= 0x08, .delayUs=0 }, // VMAX[19:0]
	{ .regAddr= 0x3026, .regData= 0x00, .delayUs=0 }, // VMAX[19:0]

	{ .regAddr= 0x3028, .regData= 0x2A, .delayUs=0 }, // HMAX[15:0]
	{ .regAddr= 0x3029, .regData= 0x04, .delayUs=0 }, // HMAX[15:0]
#endif
    { .regAddr= 0x3030, .regData= 0x00, .delayUs=0 },  //normal
	{ .regAddr= 0x3031, .regData= 0x00, .delayUs=0 }, // AD:10 bit
	{ .regAddr= 0x3032, .regData= 0x01, .delayUs=0 }, // MD:12 bit
	{ .regAddr= 0x3033, .regData= 0x08, .delayUs=0 }, // SYS_MODE[3:0]

	// { .regAddr= 0x302C,  .regData=  0x00 , .delayUs=0 }, // WDMODE[1:0]
	// { .regAddr= 0x302D,  .regData=  0x00 , .delayUs=0 }, // WDSEL[1:0]
	{ .regAddr= 0x3040,  .regData=  0xC0 , .delayUs=0 }, // PIX_HST[12:0]
	{ .regAddr= 0x3041,  .regData=  0x03 , .delayUs=0 }, // 
	{ .regAddr= 0x3042,  .regData=  0x80 , .delayUs=0 }, // PIX_HWIDTH[12:0]
	{ .regAddr= 0x3043,  .regData=  0x07 , .delayUs=0 }, // 
	{ .regAddr= 0x3044,  .regData=  0x1C , .delayUs=0 }, // PIX_VST[12:0]
	{ .regAddr= 0x3045,  .regData=  0x02 , .delayUs=0 }, // 
	{ .regAddr= 0x3046,  .regData=  0x70 , .delayUs=0 }, // PIX_VWIDTH[12:0]
	{ .regAddr= 0x3047,  .regData=  0x08 , .delayUs=0 }, // 

	{ .regAddr= 0x3023, .regData= 0x0, .delayUs=0 },  //VCMODE
	{ .regAddr= 0x302c, .regData= 0x01, .delayUs=0 }, // WDMODE[1:0]
	{ .regAddr= 0x302d, .regData= 0x01, .delayUs=0 }, // WDSEL[1:0]
	{ .regAddr= 0x3050, .regData= 0x26, .delayUs=0 }, // SHR0[19:0]
	{ .regAddr= 0x3051, .regData= 0x0F, .delayUs=0 }, // SHR0[19:0]
	{ .regAddr= 0x3054, .regData= 0x09, .delayUs=0 }, // SHR1[19:0]
	{ .regAddr= 0x3060, .regData= 0x4d, .delayUs=0 }, // RHS1[19:0]
	//{ .regAddr= 0x3050, .regData= 0x66, .delayUs=0 }, // SHR0[19:0]
	//{ .regAddr= 0x3051, .regData= 0x00, .delayUs=0 }, //
	//{ .regAddr= 0x3052, .regData= 0x00, .delayUs=0 }, //
	//{ .regAddr= 0x3090, .regData= 0x14, .delayUs=0 }, // GAIN_PCG_0[8:0]
	//{ .regAddr= 0x3091, .regData= 0x00, .delayUs=0 }, // GAIN_PCG_0[8:0]
	{ .regAddr= 0x3090, .regData= 0x00, .delayUs=0 }, // GAIN_PCG_0[8:0]
	{ .regAddr= 0x3091, .regData= 0x00, .delayUs=0 }, // GAIN_PCG_0[8:0]
	{ .regAddr= 0x30C1, .regData= 0x00, .delayUs=0 }, // XVS_DRV[1:0]
	{ .regAddr= 0x30CF, .regData= 0x01, .delayUs=0 }, // XVSMSKCNT_INT[1:0]

	{ .regAddr= 0x30D9, .regData= 0x06, .delayUs=0 }, // DIG_CLP_VSTAET[4:0]
	{ .regAddr= 0x30DA, .regData= 0x02, .delayUs=0 }, // DIG_VLP_VNUM[1:0]

	{ .regAddr= 0x3115, .regData= 0x00, .delayUs=0 }, // INCKSEL1[7:0]
	{ .regAddr= 0x3116, .regData= 0x23, .delayUs=0 }, // INCKSEL2[7:0]
	{ .regAddr= 0x3118, .regData= 0xB4, .delayUs=0 }, // INCKSEL3[10:0]
	{ .regAddr= 0x3119, .regData= 0x00, .delayUs=0 }, // INCKSEL3[10:0]
	{ .regAddr= 0x311A, .regData= 0xFC, .delayUs=0 }, // INCKSEL4[10:0]
	{ .regAddr= 0x311B, .regData= 0x00, .delayUs=0 }, // INCKSEL4[10:0]
	{ .regAddr= 0x311E, .regData= 0x23, .delayUs=0 }, // INCKSEL5[7:0]
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
	{ .regAddr= 0x4004, .regData= 0x00, .delayUs=0 }, // TXCLKESC_FREQ[15:0]
	{ .regAddr= 0x4005, .regData= 0x06, .delayUs=0 }, //
	{ .regAddr= 0x400C, .regData= 0x01, .delayUs=0 }, // INCKSEL6

	{ .regAddr= 0x4018, .regData= 0x9f, .delayUs=0 }, // TCLKPOST[15:0]
	{ .regAddr= 0x4019, .regData= 0x00, .delayUs=0 }, // TCLKPOST[15:0]
	{ .regAddr= 0x401A, .regData= 0x57, .delayUs=0 }, // TCLKPREPARE[15:0]
	{ .regAddr= 0x401B, .regData= 0x00, .delayUs=0 }, // TCLKPREPARE[15:0]
	{ .regAddr= 0x401C, .regData= 0x57, .delayUs=0 }, // TCLKTRAI[15:0]
	{ .regAddr= 0x401D, .regData= 0x00, .delayUs=0 }, // TCLKTRAI[15:0]
	{ .regAddr= 0x401E, .regData= 0x87, .delayUs=0 }, // TCLKZERO[15:0]
	{ .regAddr= 0x401F, .regData= 0x01, .delayUs=0 }, // TCLKZERO[15:0]
	{ .regAddr= 0x4020, .regData= 0x5F, .delayUs=0 }, // THSPREPARE[15:0]
	{ .regAddr= 0x4021, .regData= 0x00, .delayUs=0 }, // THSPREPARE[15:0]
	{ .regAddr= 0x4022, .regData= 0xA7, .delayUs=0 }, // THSZERO[15:0]
	{ .regAddr= 0x4023, .regData= 0x00, .delayUs=0 }, // THSZERO[15:0]
	{ .regAddr= 0x4024, .regData= 0x5F, .delayUs=0 }, // THSTRAI[15:0]
	{ .regAddr= 0x4025, .regData= 0x00, .delayUs=0 }, // THSTRAI[15:0]
	{ .regAddr= 0x4026, .regData= 0x97, .delayUs=0 }, // THSEXIT[15:0]
	{ .regAddr= 0x4027, .regData= 0x00, .delayUs=0 }, // THSEXIT[15:0]
	{ .regAddr= 0x4028, .regData= 0x4f, .delayUs=0 }, // TLPX[15:0]
	{ .regAddr= 0x4029, .regData= 0x00, .delayUs=0 }, // TLPX[15:0]
	{ .regAddr= 0x4074, .regData= 0x00, .delayUs=0 }, // INCKSEL7 [2:0]
    { .regAddr = 0x3000, .regData = 0x00, .delayUs = 24000}, /* leave standby */
};

//all pixel scan mode, 24Mhz, 2lane, lane clock 1440M, AD 10bit, MD 12bit
static drv_sensor_reg_info_t  imx415_4MP_30fps_dol2_setting[] =
{
	{ .regAddr= 0x3000, .regData= 0x01, .delayUs=0 }, /* enter standby */
	{ .regAddr= 0x3002, .regData= 0x01, .delayUs=0 }, /* master stop */
	{ .regAddr= 0x3008, .regData= 0x54, .delayUs=0 }, // BCWAIT_TIME[9:0]
	{ .regAddr= 0x3009, .regData= 0x00, .delayUs=0 }, // BCWAIT_TIME[9:0]
	{ .regAddr= 0x300A, .regData= 0x3B, .delayUs=0 }, // CPWAIT_TIME[9:0]
	{ .regAddr= 0x300B, .regData= 0x00, .delayUs=0 }, // CPWAIT_TIME[9:0]
//cropping
	{ .regAddr= 0x301c, .regData= 0x04, .delayUs=0 },  //WINMODE  4h:window croping mode
	{ .regAddr= 0x3040, .regData= 0x00, .delayUs=0 },  //PIX_HST effective pixel Start position(H direction)
	{ .regAddr= 0x3041, .regData= 0x00, .delayUs=0 },
	{ .regAddr= 0x3042, .regData= 0x08, .delayUs=0 },  //PIX_HWIDTH  4h:window croping mode, 2568(multiplies of 24)
	{ .regAddr= 0x3043, .regData= 0x0A, .delayUs=0 },
	{ .regAddr= 0x3044, .regData= 0x00, .delayUs=0 },  //PIX_VST  4h:window croping mode
	{ .regAddr= 0x3045, .regData= 0x00, .delayUs=0 },
	{ .regAddr= 0x3046, .regData= 0x80, .delayUs=0 },   //PIX_VWIDTH  4h:window croping mode, 1440
	{ .regAddr= 0x3047, .regData= 0x0b, .delayUs=0 },

	{ .regAddr= 0x3020, .regData= 0x0, .delayUs=0 },  //horizontal 2 binning
	{ .regAddr= 0x3021, .regData= 0x0, .delayUs=0 },  //veritical 2 binning
	{ .regAddr= 0x3022, .regData= 0x0, .delayUs=0 },  //ADDMODE 2/2-line binning
	{ .regAddr= 0x3024, .regData= 0xCA, .delayUs=0 }, // VMAX[19:0]
	{ .regAddr= 0x3025, .regData= 0x08, .delayUs=0 }, // VMAX[19:0]
	{ .regAddr= 0x3026, .regData= 0x00, .delayUs=0 }, // VMAX[19:0]

	{ .regAddr= 0x3028, .regData= 0x2a, .delayUs=0 }, // HMAX[15:0]
	{ .regAddr= 0x3029, .regData= 0x04, .delayUs=0 }, // HMAX[15:0]
  	{ .regAddr= 0x3030, .regData= 0x00, .delayUs=0 },  //normal
	{ .regAddr= 0x3031, .regData= 0x00, .delayUs=0 }, // AD:10 bit
	{ .regAddr= 0x3032, .regData= 0x01, .delayUs=0 }, // MD:12 bit
	{ .regAddr= 0x3033, .regData= 0x08, .delayUs=0 }, // SYS_MODE[3:0]

	// { .regAddr= 0x302c, .regData= 0x00, .delayUs=0 }, // WDMODE[1:0] 0:normal exposure
	// { .regAddr= 0x302d, .regData= 0x00, .delayUs=0 }, // WDSEL[1:0] 0:normal exposure
	{ .regAddr= 0x30CF, .regData= 0x01, .delayUs=0 }, // XVSMSKCNT_INT[1:0]

	{ .regAddr= 0x3023, .regData= 0x0, .delayUs=0 },  //VCMODE
	{ .regAddr= 0x302c, .regData= 0x01, .delayUs=0 }, // WDMODE[1:0]
	{ .regAddr= 0x302d, .regData= 0x01, .delayUs=0 }, // WDSEL[1:0]
	{ .regAddr= 0x3050, .regData= 0x26, .delayUs=0 }, // SHR0[19:0]
	{ .regAddr= 0x3051, .regData= 0x0F, .delayUs=0 }, // SHR0[19:0]
	{ .regAddr= 0x3054, .regData= 0x09, .delayUs=0 }, // SHR1[19:0]
	{ .regAddr= 0x3060, .regData= 0x4d, .delayUs=0 }, // RHS1[19:0]

	// { .regAddr= 0x3050, .regData= 0xC6, .delayUs=0 }, // SHR0[19:0]
	// { .regAddr= 0x3051, .regData= 0x02, .delayUs=0 }, // SHR0[19:0]
	// { .regAddr= 0x3052, .regData= 0x00, .delayUs=0 }, // SHR0[19:0]
	//{ .regAddr= 0x3050, .regData= 0x66, .delayUs=0 }, // SHR0[19:0]
	//{ .regAddr= 0x3051, .regData= 0x00, .delayUs=0 }, //
	//{ .regAddr= 0x3052, .regData= 0x00, .delayUs=0 }, //
	//{ .regAddr= 0x3090, .regData= 0x14, .delayUs=0 }, // GAIN_PCG_0[8:0]
	//{ .regAddr= 0x3091, .regData= 0x00, .delayUs=0 }, // GAIN_PCG_0[8:0]
	{ .regAddr= 0x3090, .regData= 0x00, .delayUs=0 }, // GAIN_PCG_0[8:0]
	{ .regAddr= 0x3091, .regData= 0x00, .delayUs=0 }, // GAIN_PCG_0[8:0]
	{ .regAddr= 0x30C1, .regData= 0x00, .delayUs=0 }, // XVS_DRV[1:0]

	{ .regAddr= 0x30D9, .regData= 0x06, .delayUs=0 }, // DIG_CLP_VSTAET[4:0]
	{ .regAddr= 0x30DA, .regData= 0x02, .delayUs=0 }, // DIG_VLP_VNUM[1:0]

	{ .regAddr= 0x3115, .regData= 0x00, .delayUs=0 }, // INCKSEL1[7:0]
	{ .regAddr= 0x3116, .regData= 0x23, .delayUs=0 }, // INCKSEL2[7:0]
	{ .regAddr= 0x3118, .regData= 0xB4, .delayUs=0 }, // INCKSEL3[10:0]
	{ .regAddr= 0x3119, .regData= 0x00, .delayUs=0 }, // INCKSEL3[10:0]
	{ .regAddr= 0x311A, .regData= 0xFC, .delayUs=0 }, // INCKSEL4[10:0]
	{ .regAddr= 0x311B, .regData= 0x00, .delayUs=0 }, // INCKSEL4[10:0]
	{ .regAddr= 0x311E, .regData= 0x23, .delayUs=0 }, // INCKSEL5[7:0]
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
	{ .regAddr= 0x4004, .regData= 0x00, .delayUs=0 }, // TXCLKESC_FREQ[15:0]
	{ .regAddr= 0x4005, .regData= 0x06, .delayUs=0 }, //
	{ .regAddr= 0x400C, .regData= 0x01, .delayUs=0 }, // INCKSEL6

	{ .regAddr= 0x4018, .regData= 0x9f, .delayUs=0 }, // TCLKPOST[15:0]
	{ .regAddr= 0x4019, .regData= 0x00, .delayUs=0 }, // TCLKPOST[15:0]
	{ .regAddr= 0x401A, .regData= 0x57, .delayUs=0 }, // TCLKPREPARE[15:0]
	{ .regAddr= 0x401B, .regData= 0x00, .delayUs=0 }, // TCLKPREPARE[15:0]
	{ .regAddr= 0x401C, .regData= 0x57, .delayUs=0 }, // TCLKTRAI[15:0]
	{ .regAddr= 0x401D, .regData= 0x00, .delayUs=0 }, // TCLKTRAI[15:0]
	{ .regAddr= 0x401E, .regData= 0x87, .delayUs=0 }, // TCLKZERO[15:0]
	{ .regAddr= 0x401F, .regData= 0x01, .delayUs=0 }, // TCLKZERO[15:0]
	{ .regAddr= 0x4020, .regData= 0x5F, .delayUs=0 }, // THSPREPARE[15:0]
	{ .regAddr= 0x4021, .regData= 0x00, .delayUs=0 }, // THSPREPARE[15:0]
	{ .regAddr= 0x4022, .regData= 0xA7, .delayUs=0 }, // THSZERO[15:0]
	{ .regAddr= 0x4023, .regData= 0x00, .delayUs=0 }, // THSZERO[15:0]
	{ .regAddr= 0x4024, .regData= 0x5F, .delayUs=0 }, // THSTRAI[15:0]
	{ .regAddr= 0x4025, .regData= 0x00, .delayUs=0 }, // THSTRAI[15:0]
	{ .regAddr= 0x4026, .regData= 0x97, .delayUs=0 }, // THSEXIT[15:0]
	{ .regAddr= 0x4027, .regData= 0x00, .delayUs=0 }, // THSEXIT[15:0]
	{ .regAddr= 0x4028, .regData= 0x4F, .delayUs=0 }, // TLPX[15:0]
	{ .regAddr= 0x4029, .regData= 0x00, .delayUs=0 }, // TLPX[15:0]
	{ .regAddr= 0x4074, .regData= 0x00, .delayUs=0 }, // INCKSEL7 [2:0]
    { .regAddr = 0x3000, .regData = 0x00, .delayUs = 24000}, /* leave standby */
};

//4lane,12bit
static drv_sensor_reg_info_t  imx415_4mp_30fps_4lane_setting[] =
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

	{ .regAddr= 0x3028, .regData= 0x4c, .delayUs=0 }, // HMAX[15:0]
	{ .regAddr= 0x3029, .regData= 0x04, .delayUs=0 }, // HMAX[15:0]
	//{ .regAddr= 0x3028, .regData= 0x40, .delayUs=0 }, // HMAX[15:0] 25fps
	//{ .regAddr= 0x3029, .regData= 0x05, .delayUs=0 }, // HMAX[15:0]
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
	/*width 2560x1440*/
	{ .regAddr= 0x3040, .regData= 0x00, .delayUs=0 },  //PIX_HST effective pixel Start position(H direction)
	{ .regAddr= 0x3041, .regData= 0x00, .delayUs=0 },
	{ .regAddr= 0x3042, .regData= 0x08, .delayUs=0 },  //PIX_HWIDTH  4h:window croping mode, 2568(multiplies of 24)
	{ .regAddr= 0x3043, .regData= 0x0A, .delayUs=0 },
	{ .regAddr= 0x3044, .regData= 0x00, .delayUs=0 },  //PIX_VST  4h:window croping mode
	{ .regAddr= 0x3045, .regData= 0x00, .delayUs=0 },
	{ .regAddr= 0x3046, .regData= 0x80, .delayUs=0 },   //PIX_VWIDTH  4h:window croping mode, 1440
	{ .regAddr= 0x3047, .regData= 0x0b, .delayUs=0 },

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



//all pixel scan mode, 24Mhz, 2lane, lane clock 1440M, AD 10bit, MD 12bit
static drv_sensor_reg_info_t  imx415_5MP_30fps_dol2_setting[] =
{
	{ .regAddr= 0x3000, .regData= 0x01, .delayUs=0 }, /* enter standby */
	{ .regAddr= 0x3002, .regData= 0x01, .delayUs=0 }, /* master stop */
	{ .regAddr= 0x3008, .regData= 0x54, .delayUs=0 }, // BCWAIT_TIME[9:0]
	{ .regAddr= 0x3009, .regData= 0x00, .delayUs=0 }, // BCWAIT_TIME[9:0]
	{ .regAddr= 0x300A, .regData= 0x3B, .delayUs=0 }, // CPWAIT_TIME[9:0]
	{ .regAddr= 0x300B, .regData= 0x00, .delayUs=0 }, // CPWAIT_TIME[9:0]
//cropping
	{ .regAddr= 0x301c, .regData= 0x04, .delayUs=0 },  //WINMODE  4h:window croping mode
	{ .regAddr= 0x3040, .regData= 0x00, .delayUs=0 },  //PIX_HST effective pixel Start position(H direction)
	{ .regAddr= 0x3041, .regData= 0x00, .delayUs=0 },
	{ .regAddr= 0x3042, .regData= 0x80, .delayUs=0 },  //PIX_HWIDTH  4h:window croping mode, 2688(multiplies of 24)
	{ .regAddr= 0x3043, .regData= 0x0A, .delayUs=0 },
	{ .regAddr= 0x3044, .regData= 0x00, .delayUs=0 },  //PIX_VST  4h:window croping mode
	{ .regAddr= 0x3045, .regData= 0x00, .delayUs=0 },
	{ .regAddr= 0x3046, .regData= 0xa8, .delayUs=0 },   //PIX_VWIDTH  4h:window croping mode, 1860
	{ .regAddr= 0x3047, .regData= 0x0e, .delayUs=0 },

	{ .regAddr= 0x3020, .regData= 0x0, .delayUs=0 },  //horizontal 2 binning
	{ .regAddr= 0x3021, .regData= 0x0, .delayUs=0 },  //veritical 2 binning
	{ .regAddr= 0x3022, .regData= 0x0, .delayUs=0 },  //ADDMODE 2/2-line binning
	{ .regAddr= 0x3024, .regData= 0xCA, .delayUs=0 }, // VMAX[19:0]
	{ .regAddr= 0x3025, .regData= 0x08, .delayUs=0 }, // VMAX[19:0]
	{ .regAddr= 0x3026, .regData= 0x00, .delayUs=0 }, // VMAX[19:0]

	{ .regAddr= 0x3028, .regData= 0x2a, .delayUs=0 }, // HMAX[15:0]
	{ .regAddr= 0x3029, .regData= 0x04, .delayUs=0 }, // HMAX[15:0]
  	{ .regAddr= 0x3030, .regData= 0x00, .delayUs=0 },  //normal
	{ .regAddr= 0x3031, .regData= 0x00, .delayUs=0 }, // AD:10 bit
	{ .regAddr= 0x3032, .regData= 0x01, .delayUs=0 }, // MD:12 bit
	{ .regAddr= 0x3033, .regData= 0x08, .delayUs=0 }, // SYS_MODE[3:0]

	// { .regAddr= 0x302c, .regData= 0x00, .delayUs=0 }, // WDMODE[1:0] 0:normal exposure
	// { .regAddr= 0x302d, .regData= 0x00, .delayUs=0 }, // WDSEL[1:0] 0:normal exposure
	{ .regAddr= 0x30CF, .regData= 0x01, .delayUs=0 }, // XVSMSKCNT_INT[1:0]

	{ .regAddr= 0x3023, .regData= 0x0, .delayUs=0 },  //VCMODE
	{ .regAddr= 0x302c, .regData= 0x01, .delayUs=0 }, // WDMODE[1:0]
	{ .regAddr= 0x302d, .regData= 0x01, .delayUs=0 }, // WDSEL[1:0]
	{ .regAddr= 0x3050, .regData= 0x26, .delayUs=0 }, // SHR0[19:0]
	{ .regAddr= 0x3051, .regData= 0x0F, .delayUs=0 }, // SHR0[19:0]
	{ .regAddr= 0x3054, .regData= 0x09, .delayUs=0 }, // SHR1[19:0]
	{ .regAddr= 0x3060, .regData= 0x4d, .delayUs=0 }, // RHS1[19:0]

	// { .regAddr= 0x3050, .regData= 0xC6, .delayUs=0 }, // SHR0[19:0]
	// { .regAddr= 0x3051, .regData= 0x02, .delayUs=0 }, // SHR0[19:0]
	// { .regAddr= 0x3052, .regData= 0x00, .delayUs=0 }, // SHR0[19:0]
	//{ .regAddr= 0x3050, .regData= 0x66, .delayUs=0 }, // SHR0[19:0]
	//{ .regAddr= 0x3051, .regData= 0x00, .delayUs=0 }, //
	//{ .regAddr= 0x3052, .regData= 0x00, .delayUs=0 }, //
	//{ .regAddr= 0x3090, .regData= 0x14, .delayUs=0 }, // GAIN_PCG_0[8:0]
	//{ .regAddr= 0x3091, .regData= 0x00, .delayUs=0 }, // GAIN_PCG_0[8:0]
	{ .regAddr= 0x3090, .regData= 0x00, .delayUs=0 }, // GAIN_PCG_0[8:0]
	{ .regAddr= 0x3091, .regData= 0x00, .delayUs=0 }, // GAIN_PCG_0[8:0]
	{ .regAddr= 0x30C1, .regData= 0x00, .delayUs=0 }, // XVS_DRV[1:0]

	{ .regAddr= 0x30D9, .regData= 0x06, .delayUs=0 }, // DIG_CLP_VSTAET[4:0]
	{ .regAddr= 0x30DA, .regData= 0x02, .delayUs=0 }, // DIG_VLP_VNUM[1:0]

	{ .regAddr= 0x3115, .regData= 0x00, .delayUs=0 }, // INCKSEL1[7:0]
	{ .regAddr= 0x3116, .regData= 0x23, .delayUs=0 }, // INCKSEL2[7:0]
	{ .regAddr= 0x3118, .regData= 0xB4, .delayUs=0 }, // INCKSEL3[10:0]
	{ .regAddr= 0x3119, .regData= 0x00, .delayUs=0 }, // INCKSEL3[10:0]
	{ .regAddr= 0x311A, .regData= 0xFC, .delayUs=0 }, // INCKSEL4[10:0]
	{ .regAddr= 0x311B, .regData= 0x00, .delayUs=0 }, // INCKSEL4[10:0]
	{ .regAddr= 0x311E, .regData= 0x23, .delayUs=0 }, // INCKSEL5[7:0]
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
	{ .regAddr= 0x4004, .regData= 0x00, .delayUs=0 }, // TXCLKESC_FREQ[15:0]
	{ .regAddr= 0x4005, .regData= 0x06, .delayUs=0 }, //
	{ .regAddr= 0x400C, .regData= 0x01, .delayUs=0 }, // INCKSEL6

	{ .regAddr= 0x4018, .regData= 0x9f, .delayUs=0 }, // TCLKPOST[15:0]
	{ .regAddr= 0x4019, .regData= 0x00, .delayUs=0 }, // TCLKPOST[15:0]
	{ .regAddr= 0x401A, .regData= 0x57, .delayUs=0 }, // TCLKPREPARE[15:0]
	{ .regAddr= 0x401B, .regData= 0x00, .delayUs=0 }, // TCLKPREPARE[15:0]
	{ .regAddr= 0x401C, .regData= 0x57, .delayUs=0 }, // TCLKTRAI[15:0]
	{ .regAddr= 0x401D, .regData= 0x00, .delayUs=0 }, // TCLKTRAI[15:0]
	{ .regAddr= 0x401E, .regData= 0x87, .delayUs=0 }, // TCLKZERO[15:0]
	{ .regAddr= 0x401F, .regData= 0x01, .delayUs=0 }, // TCLKZERO[15:0]
	{ .regAddr= 0x4020, .regData= 0x5F, .delayUs=0 }, // THSPREPARE[15:0]
	{ .regAddr= 0x4021, .regData= 0x00, .delayUs=0 }, // THSPREPARE[15:0]
	{ .regAddr= 0x4022, .regData= 0xA7, .delayUs=0 }, // THSZERO[15:0]
	{ .regAddr= 0x4023, .regData= 0x00, .delayUs=0 }, // THSZERO[15:0]
	{ .regAddr= 0x4024, .regData= 0x5F, .delayUs=0 }, // THSTRAI[15:0]
	{ .regAddr= 0x4025, .regData= 0x00, .delayUs=0 }, // THSTRAI[15:0]
	{ .regAddr= 0x4026, .regData= 0x97, .delayUs=0 }, // THSEXIT[15:0]
	{ .regAddr= 0x4027, .regData= 0x00, .delayUs=0 }, // THSEXIT[15:0]
	{ .regAddr= 0x4028, .regData= 0x4F, .delayUs=0 }, // TLPX[15:0]
	{ .regAddr= 0x4029, .regData= 0x00, .delayUs=0 }, // TLPX[15:0]
	{ .regAddr= 0x4074, .regData= 0x00, .delayUs=0 }, // INCKSEL7 [2:0]
    { .regAddr = 0x3000, .regData = 0x00, .delayUs = 24000}, /* leave standby */
};

//all pixel scan mode, 24Mhz, 2lane, lane clock 1440M, AD 10bit, MD 10bit,for 56_5mp
static drv_sensor_reg_info_t  imx415_5MP_30fps_setting[] =
{
	{ .regAddr= 0x3000, .regData= 0x01, .delayUs=0 }, /* enter standby */
	{ .regAddr= 0x3002, .regData= 0x01, .delayUs=0 }, /* master stop */
	{ .regAddr= 0x3008, .regData= 0x54, .delayUs=0 }, // BCWAIT_TIME[9:0]
	{ .regAddr= 0x3009, .regData= 0x00, .delayUs=0 }, // BCWAIT_TIME[9:0]
	{ .regAddr= 0x300A, .regData= 0x3B, .delayUs=0 }, // CPWAIT_TIME[9:0]
	{ .regAddr= 0x300B, .regData= 0x00, .delayUs=0 }, // CPWAIT_TIME[9:0]
//cropping
	{ .regAddr= 0x301c, .regData= 0x04, .delayUs=0 },  //WINMODE  4h:window croping mode
	{ .regAddr= 0x3040, .regData= 0x40, .delayUs=0 },  //PIX_HST effective pixel Start position(H direction)
	{ .regAddr= 0x3041, .regData= 0x02, .delayUs=0 },
	{ .regAddr= 0x3042, .regData= 0x80, .delayUs=0 },  //PIX_HWIDTH  4h:window croping mode, 2688(multiplies of 24)
	{ .regAddr= 0x3043, .regData= 0x0A, .delayUs=0 },
	{ .regAddr= 0x3044, .regData= 0x44, .delayUs=0 },  //PIX_VST  4h:window croping mode
	{ .regAddr= 0x3045, .regData= 0x01, .delayUs=0 },
	{ .regAddr= 0x3046, .regData= 0xE0, .delayUs=0 },   //PIX_VWIDTH  4h:window croping mode, 1520*2
	{ .regAddr= 0x3047, .regData= 0x0B, .delayUs=0 },


	{ .regAddr= 0x3020, .regData= 0x0, .delayUs=0 },  //horizontal 2 binning
	{ .regAddr= 0x3021, .regData= 0x0, .delayUs=0 },  //veritical 2 binning
	{ .regAddr= 0x3022, .regData= 0x0, .delayUs=0 },  //ADDMODE 2/2-line binning
	{ .regAddr= 0x3024, .regData= 0x94, .delayUs=0 }, // VMAX[19:0]
	{ .regAddr= 0x3025, .regData= 0x11, .delayUs=0 }, // VMAX[19:0]
	{ .regAddr= 0x3026, .regData= 0x00, .delayUs=0 }, // VMAX[19:0]

	{ .regAddr= 0x3028, .regData= 0x2a, .delayUs=0 }, // HMAX[15:0]
	{ .regAddr= 0x3029, .regData= 0x04, .delayUs=0 }, // HMAX[15:0]
  	{ .regAddr= 0x3030, .regData= 0x00, .delayUs=0 },  //normal
	{ .regAddr= 0x3031, .regData= 0x00, .delayUs=0 }, // AD:10 bit
	{ .regAddr= 0x3032, .regData= 0x00, .delayUs=0 }, // MD:10 bit
	{ .regAddr= 0x3033, .regData= 0x08, .delayUs=0 }, // SYS_MODE[3:0]

	{ .regAddr= 0x302c, .regData= 0x00, .delayUs=0 }, // WDMODE[1:0] 0:normal exposure
	{ .regAddr= 0x302d, .regData= 0x00, .delayUs=0 }, // WDSEL[1:0] 0:normal exposure
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

	{ .regAddr= 0x30D9, .regData= 0x06, .delayUs=0 }, // DIG_CLP_VSTAET[4:0]
	{ .regAddr= 0x30DA, .regData= 0x02, .delayUs=0 }, // DIG_VLP_VNUM[1:0]

	{ .regAddr= 0x3115, .regData= 0x00, .delayUs=0 }, // INCKSEL1[7:0]
	{ .regAddr= 0x3116, .regData= 0x23, .delayUs=0 }, // INCKSEL2[7:0]
	{ .regAddr= 0x3118, .regData= 0xB4, .delayUs=0 }, // INCKSEL3[10:0]
	{ .regAddr= 0x3119, .regData= 0x00, .delayUs=0 }, // INCKSEL3[10:0]
	{ .regAddr= 0x311A, .regData= 0xFC, .delayUs=0 }, // INCKSEL4[10:0]
	{ .regAddr= 0x311B, .regData= 0x00, .delayUs=0 }, // INCKSEL4[10:0]
	{ .regAddr= 0x311E, .regData= 0x23, .delayUs=0 }, // INCKSEL5[7:0]
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
	{ .regAddr= 0x4004, .regData= 0x00, .delayUs=0 }, // TXCLKESC_FREQ[15:0]
	{ .regAddr= 0x4005, .regData= 0x06, .delayUs=0 }, //
	{ .regAddr= 0x400C, .regData= 0x01, .delayUs=0 }, // INCKSEL6

	{ .regAddr= 0x4018, .regData= 0x9f, .delayUs=0 }, // TCLKPOST[15:0]
	{ .regAddr= 0x4019, .regData= 0x00, .delayUs=0 }, // TCLKPOST[15:0]
	{ .regAddr= 0x401A, .regData= 0x57, .delayUs=0 }, // TCLKPREPARE[15:0]
	{ .regAddr= 0x401B, .regData= 0x00, .delayUs=0 }, // TCLKPREPARE[15:0]
	{ .regAddr= 0x401C, .regData= 0x57, .delayUs=0 }, // TCLKTRAI[15:0]
	{ .regAddr= 0x401D, .regData= 0x00, .delayUs=0 }, // TCLKTRAI[15:0]
	{ .regAddr= 0x401E, .regData= 0x87, .delayUs=0 }, // TCLKZERO[15:0]
	{ .regAddr= 0x401F, .regData= 0x01, .delayUs=0 }, // TCLKZERO[15:0]
	{ .regAddr= 0x4020, .regData= 0x5F, .delayUs=0 }, // THSPREPARE[15:0]
	{ .regAddr= 0x4021, .regData= 0x00, .delayUs=0 }, // THSPREPARE[15:0]
	{ .regAddr= 0x4022, .regData= 0xA7, .delayUs=0 }, // THSZERO[15:0]
	{ .regAddr= 0x4023, .regData= 0x00, .delayUs=0 }, // THSZERO[15:0]
	{ .regAddr= 0x4024, .regData= 0x5F, .delayUs=0 }, // THSTRAI[15:0]
	{ .regAddr= 0x4025, .regData= 0x00, .delayUs=0 }, // THSTRAI[15:0]
	{ .regAddr= 0x4026, .regData= 0x97, .delayUs=0 }, // THSEXIT[15:0]
	{ .regAddr= 0x4027, .regData= 0x00, .delayUs=0 }, // THSEXIT[15:0]
	{ .regAddr= 0x4028, .regData= 0x4F, .delayUs=0 }, // TLPX[15:0]
	{ .regAddr= 0x4029, .regData= 0x00, .delayUs=0 }, // TLPX[15:0]
	{ .regAddr= 0x4074, .regData= 0x00, .delayUs=0 }, // INCKSEL7 [2:0]
    { .regAddr = 0x3000, .regData = 0x00, .delayUs = 24000}, /* leave standby */
};


//IMX415-AAQR Bining mode CSI-2_2lane 37.125Mhz AD:10bit Output:12bit 1782Mbps Master Mode 15fps_for 24MHZ dt57mipi aligned 
static drv_sensor_reg_info_t  imx415_1080p_15fps_37_to_24_setting[] =
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
	{ .regAddr= 0x3024, .regData= 0xB8, .delayUs=0 }, // VMAX[19:0]
	{ .regAddr= 0x3025, .regData= 0x0B, .delayUs=0 }, // VMAX[19:0]
	{ .regAddr= 0x3026, .regData= 0x00, .delayUs=0 }, // VMAX[19:0]
	{ .regAddr= 0x3028, .regData= 0x4C, .delayUs=0 }, // HMAX[15:0]
	{ .regAddr= 0x3029, .regData= 0x04, .delayUs=0 }, // HMAX[15:0]
	{ .regAddr= 0x302c, .regData= 0x00, .delayUs=0 }, // WDMODE[1:0]
	{ .regAddr= 0x302d, .regData= 0x00, .delayUs=0 }, // WDSEL[1:0]
	{ .regAddr= 0x30CF, .regData= 0x00, .delayUs=0 }, // XVSMSKCNT_INT[1:0]

    { .regAddr= 0x3030, .regData= 0x00, .delayUs = 0x0},
	{ .regAddr= 0x3031, .regData= 0x00, .delayUs=0 }, // AD:10 bit
	{ .regAddr= 0x3032, .regData= 0x00, .delayUs=0 }, // MD:12 bit
	{ .regAddr= 0x3033, .regData= 0x04, .delayUs=0 }, // SYS_MODE[3:0]
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

#ifdef IMX415_HARD_SYNC
    { .regAddr= 0x3003, .regData= 0x00, .delayUs=0 }, /* master mode */
    { .regAddr= 0x30C0, .regData= 0x2a, .delayUs=0,}, //vsync/hsync Output
    { .regAddr= 0x30C1, .regData= 0x00, .delayUs=0 }, // XVS_DRV[1:0] master=0, slave=0xf
    { .regAddr= 0x30CC, .regData= 0x00, .delayUs=0 }, //XVSLNG
    { .regAddr= 0x30CD, .regData= 0x00, .delayUs=0 }, //XHSLNG
#endif
	{ .regAddr = 0x3000, .regData = 0x00, .delayUs = 24000}, /* standby */
};

//2/2-line binning mode, 24Mhz, 2lane, lane clock 1440M, AD 10bit, MD 12bit VCMODE
static drv_sensor_reg_info_t  imx415_1080p_30fps_dol2_vcmode_setting[] =
{
	{ .regAddr= 0x3000, .regData= 0x01, .delayUs=0 }, /* enter standby */
	{ .regAddr= 0x3002, .regData= 0x01, .delayUs=0 }, /* master stop */
	{ .regAddr= 0x3008, .regData= 0x54, .delayUs=0 }, // BCWAIT_TIME[9:0]
	{ .regAddr= 0x3009, .regData= 0x00, .delayUs=0 }, // BCWAIT_TIME[9:0]
	{ .regAddr= 0x300A, .regData= 0x3B, .delayUs=0 }, // CPWAIT_TIME[9:0]
	{ .regAddr= 0x300B, .regData= 0x00, .delayUs=0 }, // CPWAIT_TIME[9:0]
#if 1
	{ .regAddr= 0x301c, .regData= 0x0, .delayUs=0 },  //WINMODE
	{ .regAddr= 0x3020, .regData= 0x1, .delayUs=0 },  //horizontal 2 binning
	{ .regAddr= 0x3021, .regData= 0x1, .delayUs=0 },  //veritical 2 binning
	{ .regAddr= 0x3022, .regData= 0x1, .delayUs=0 },  //ADDMODE 2/2-line binning
	{ .regAddr= 0x3024, .regData= 0xC0, .delayUs=0 }, // VMAX[19:0]
	{ .regAddr= 0x3025, .regData= 0x09, .delayUs=0 }, // VMAX[19:0]
	{ .regAddr= 0x3026, .regData= 0x00, .delayUs=0 }, // VMAX[19:0]

	{ .regAddr= 0x3028, .regData= 0xc0, .delayUs=0 }, // HMAX[15:0]
	{ .regAddr= 0x3029, .regData= 0x03, .delayUs=0 }, // HMAX[15:0]
#else
	{ .regAddr= 0x301c, .regData= 0x4, .delayUs=0 },  //WINMODE
	{ .regAddr= 0x3020, .regData= 0x0, .delayUs=0 },  //horizontal 2 binning
	{ .regAddr= 0x3021, .regData= 0x0, .delayUs=0 },  //veritical 2 binning
	{ .regAddr= 0x3022, .regData= 0x0, .delayUs=0 },  //ADDMODE all pixel
	{ .regAddr= 0x3024, .regData= 0xCA, .delayUs=0 }, // VMAX[19:0]
	{ .regAddr= 0x3025, .regData= 0x08, .delayUs=0 }, // VMAX[19:0]
	{ .regAddr= 0x3026, .regData= 0x00, .delayUs=0 }, // VMAX[19:0]

	{ .regAddr= 0x3028, .regData= 0x2A, .delayUs=0 }, // HMAX[15:0]
	{ .regAddr= 0x3029, .regData= 0x04, .delayUs=0 }, // HMAX[15:0]
#endif
    { .regAddr= 0x3030, .regData= 0x00, .delayUs=0 },  //normal
	{ .regAddr= 0x3031, .regData= 0x00, .delayUs=0 }, // AD:10 bit
	{ .regAddr= 0x3032, .regData= 0x01, .delayUs=0 }, // MD:12 bit
	{ .regAddr= 0x3033, .regData= 0x08, .delayUs=0 }, // SYS_MODE[3:0]

	// { .regAddr= 0x302C,  .regData=  0x00 , .delayUs=0 }, // WDMODE[1:0]
	// { .regAddr= 0x302D,  .regData=  0x00 , .delayUs=0 }, // WDSEL[1:0]
	{ .regAddr= 0x3040,  .regData=  0xC0 , .delayUs=0 }, // PIX_HST[12:0]
	{ .regAddr= 0x3041,  .regData=  0x03 , .delayUs=0 }, // 
	{ .regAddr= 0x3042,  .regData=  0x80 , .delayUs=0 }, // PIX_HWIDTH[12:0]
	{ .regAddr= 0x3043,  .regData=  0x07 , .delayUs=0 }, // 
	{ .regAddr= 0x3044,  .regData=  0x1C , .delayUs=0 }, // PIX_VST[12:0]
	{ .regAddr= 0x3045,  .regData=  0x02 , .delayUs=0 }, // 
	{ .regAddr= 0x3046,  .regData=  0x70 , .delayUs=0 }, // PIX_VWIDTH[12:0]
	{ .regAddr= 0x3047,  .regData=  0x08 , .delayUs=0 }, // 

	{ .regAddr= 0x3023, .regData= 0x01, .delayUs=0 },  //VCMODE
	{ .regAddr= 0x302c, .regData= 0x01, .delayUs=0 }, // WDMODE[1:0]
	{ .regAddr= 0x302d, .regData= 0x01, .delayUs=0 }, // WDSEL[1:0]
	{ .regAddr= 0x3050, .regData= 0x26, .delayUs=0 }, // SHR0[19:0]
	{ .regAddr= 0x3051, .regData= 0x0F, .delayUs=0 }, // SHR0[19:0]
	{ .regAddr= 0x3054, .regData= 0x09, .delayUs=0 }, // SHR1[19:0]
	{ .regAddr= 0x3060, .regData= 0x4d, .delayUs=0 }, // RHS1[19:0]
	//{ .regAddr= 0x3050, .regData= 0x66, .delayUs=0 }, // SHR0[19:0]
	//{ .regAddr= 0x3051, .regData= 0x00, .delayUs=0 }, //
	//{ .regAddr= 0x3052, .regData= 0x00, .delayUs=0 }, //
	//{ .regAddr= 0x3090, .regData= 0x14, .delayUs=0 }, // GAIN_PCG_0[8:0]
	//{ .regAddr= 0x3091, .regData= 0x00, .delayUs=0 }, // GAIN_PCG_0[8:0]
	{ .regAddr= 0x3090, .regData= 0x00, .delayUs=0 }, // GAIN_PCG_0[8:0]
	{ .regAddr= 0x3091, .regData= 0x00, .delayUs=0 }, // GAIN_PCG_0[8:0]
	{ .regAddr= 0x30C1, .regData= 0x00, .delayUs=0 }, // XVS_DRV[1:0]
	{ .regAddr= 0x30CF, .regData= 0x01, .delayUs=0 }, // XVSMSKCNT_INT[1:0]

	{ .regAddr= 0x30D9, .regData= 0x06, .delayUs=0 }, // DIG_CLP_VSTAET[4:0]
	{ .regAddr= 0x30DA, .regData= 0x02, .delayUs=0 }, // DIG_VLP_VNUM[1:0]

	{ .regAddr= 0x3115, .regData= 0x00, .delayUs=0 }, // INCKSEL1[7:0]
	{ .regAddr= 0x3116, .regData= 0x23, .delayUs=0 }, // INCKSEL2[7:0]
	{ .regAddr= 0x3118, .regData= 0xB4, .delayUs=0 }, // INCKSEL3[10:0]
	{ .regAddr= 0x3119, .regData= 0x00, .delayUs=0 }, // INCKSEL3[10:0]
	{ .regAddr= 0x311A, .regData= 0xFC, .delayUs=0 }, // INCKSEL4[10:0]
	{ .regAddr= 0x311B, .regData= 0x00, .delayUs=0 }, // INCKSEL4[10:0]
	{ .regAddr= 0x311E, .regData= 0x23, .delayUs=0 }, // INCKSEL5[7:0]
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
	{ .regAddr= 0x4004, .regData= 0x00, .delayUs=0 }, // TXCLKESC_FREQ[15:0]
	{ .regAddr= 0x4005, .regData= 0x06, .delayUs=0 }, //
	{ .regAddr= 0x400C, .regData= 0x01, .delayUs=0 }, // INCKSEL6

	{ .regAddr= 0x4018, .regData= 0x9f, .delayUs=0 }, // TCLKPOST[15:0]
	{ .regAddr= 0x4019, .regData= 0x00, .delayUs=0 }, // TCLKPOST[15:0]
	{ .regAddr= 0x401A, .regData= 0x57, .delayUs=0 }, // TCLKPREPARE[15:0]
	{ .regAddr= 0x401B, .regData= 0x00, .delayUs=0 }, // TCLKPREPARE[15:0]
	{ .regAddr= 0x401C, .regData= 0x57, .delayUs=0 }, // TCLKTRAI[15:0]
	{ .regAddr= 0x401D, .regData= 0x00, .delayUs=0 }, // TCLKTRAI[15:0]
	{ .regAddr= 0x401E, .regData= 0x87, .delayUs=0 }, // TCLKZERO[15:0]
	{ .regAddr= 0x401F, .regData= 0x01, .delayUs=0 }, // TCLKZERO[15:0]
	{ .regAddr= 0x4020, .regData= 0x5F, .delayUs=0 }, // THSPREPARE[15:0]
	{ .regAddr= 0x4021, .regData= 0x00, .delayUs=0 }, // THSPREPARE[15:0]
	{ .regAddr= 0x4022, .regData= 0xA7, .delayUs=0 }, // THSZERO[15:0]
	{ .regAddr= 0x4023, .regData= 0x00, .delayUs=0 }, // THSZERO[15:0]
	{ .regAddr= 0x4024, .regData= 0x5F, .delayUs=0 }, // THSTRAI[15:0]
	{ .regAddr= 0x4025, .regData= 0x00, .delayUs=0 }, // THSTRAI[15:0]
	{ .regAddr= 0x4026, .regData= 0x97, .delayUs=0 }, // THSEXIT[15:0]
	{ .regAddr= 0x4027, .regData= 0x00, .delayUs=0 }, // THSEXIT[15:0]
	{ .regAddr= 0x4028, .regData= 0x4f, .delayUs=0 }, // TLPX[15:0]
	{ .regAddr= 0x4029, .regData= 0x00, .delayUs=0 }, // TLPX[15:0]
	{ .regAddr= 0x4074, .regData= 0x00, .delayUs=0 }, // INCKSEL7 [2:0]
    { .regAddr = 0x3000, .regData = 0x00, .delayUs = 24000}, /* leave standby */
};

//all pixel scan mode, 24Mhz, 2lane, lane clock 1440M, AD 10bit, MD 12bit VCMODE
static drv_sensor_reg_info_t  imx415_4MP_30fps_dol2_vcmode_setting[] =
{
	{ .regAddr= 0x3000, .regData= 0x01, .delayUs=0 }, /* enter standby */
	{ .regAddr= 0x3002, .regData= 0x01, .delayUs=0 }, /* master stop */
	{ .regAddr= 0x3008, .regData= 0x54, .delayUs=0 }, // BCWAIT_TIME[9:0]
	{ .regAddr= 0x3009, .regData= 0x00, .delayUs=0 }, // BCWAIT_TIME[9:0]
	{ .regAddr= 0x300A, .regData= 0x3B, .delayUs=0 }, // CPWAIT_TIME[9:0]
	{ .regAddr= 0x300B, .regData= 0x00, .delayUs=0 }, // CPWAIT_TIME[9:0]
//cropping
	{ .regAddr= 0x301c, .regData= 0x04, .delayUs=0 },  //WINMODE  4h:window croping mode
	{ .regAddr= 0x3040, .regData= 0x00, .delayUs=0 },  //PIX_HST effective pixel Start position(H direction)
	{ .regAddr= 0x3041, .regData= 0x00, .delayUs=0 },
	{ .regAddr= 0x3042, .regData= 0x08, .delayUs=0 },  //PIX_HWIDTH  4h:window croping mode, 2568(multiplies of 24)
	{ .regAddr= 0x3043, .regData= 0x0A, .delayUs=0 },
	{ .regAddr= 0x3044, .regData= 0x00, .delayUs=0 },  //PIX_VST  4h:window croping mode
	{ .regAddr= 0x3045, .regData= 0x00, .delayUs=0 },
	{ .regAddr= 0x3046, .regData= 0x80, .delayUs=0 },   //PIX_VWIDTH  4h:window croping mode, 1440
	{ .regAddr= 0x3047, .regData= 0x0b, .delayUs=0 },

	{ .regAddr= 0x3020, .regData= 0x0, .delayUs=0 },  //horizontal 2 binning
	{ .regAddr= 0x3021, .regData= 0x0, .delayUs=0 },  //veritical 2 binning
	{ .regAddr= 0x3022, .regData= 0x0, .delayUs=0 },  //ADDMODE 2/2-line binning
	{ .regAddr= 0x3024, .regData= 0xCA, .delayUs=0 }, // VMAX[19:0]
	{ .regAddr= 0x3025, .regData= 0x08, .delayUs=0 }, // VMAX[19:0]
	{ .regAddr= 0x3026, .regData= 0x00, .delayUs=0 }, // VMAX[19:0]

	{ .regAddr= 0x3028, .regData= 0x2a, .delayUs=0 }, // HMAX[15:0]
	{ .regAddr= 0x3029, .regData= 0x04, .delayUs=0 }, // HMAX[15:0]
  	{ .regAddr= 0x3030, .regData= 0x00, .delayUs=0 },  //normal
	{ .regAddr= 0x3031, .regData= 0x00, .delayUs=0 }, // AD:10 bit
	{ .regAddr= 0x3032, .regData= 0x01, .delayUs=0 }, // MD:12 bit
	{ .regAddr= 0x3033, .regData= 0x08, .delayUs=0 }, // SYS_MODE[3:0]

	// { .regAddr= 0x302c, .regData= 0x00, .delayUs=0 }, // WDMODE[1:0] 0:normal exposure
	// { .regAddr= 0x302d, .regData= 0x00, .delayUs=0 }, // WDSEL[1:0] 0:normal exposure
	{ .regAddr= 0x30CF, .regData= 0x01, .delayUs=0 }, // XVSMSKCNT_INT[1:0]

	{ .regAddr= 0x3023, .regData= 0x01, .delayUs=0 },  //VCMODE
	{ .regAddr= 0x302c, .regData= 0x01, .delayUs=0 }, // WDMODE[1:0]
	{ .regAddr= 0x302d, .regData= 0x01, .delayUs=0 }, // WDSEL[1:0]
	{ .regAddr= 0x3050, .regData= 0x26, .delayUs=0 }, // SHR0[19:0]
	{ .regAddr= 0x3051, .regData= 0x0F, .delayUs=0 }, // SHR0[19:0]
	{ .regAddr= 0x3054, .regData= 0x09, .delayUs=0 }, // SHR1[19:0]
	{ .regAddr= 0x3060, .regData= 0x4d, .delayUs=0 }, // RHS1[19:0]

	// { .regAddr= 0x3050, .regData= 0xC6, .delayUs=0 }, // SHR0[19:0]
	// { .regAddr= 0x3051, .regData= 0x02, .delayUs=0 }, // SHR0[19:0]
	// { .regAddr= 0x3052, .regData= 0x00, .delayUs=0 }, // SHR0[19:0]
	//{ .regAddr= 0x3050, .regData= 0x66, .delayUs=0 }, // SHR0[19:0]
	//{ .regAddr= 0x3051, .regData= 0x00, .delayUs=0 }, //
	//{ .regAddr= 0x3052, .regData= 0x00, .delayUs=0 }, //
	//{ .regAddr= 0x3090, .regData= 0x14, .delayUs=0 }, // GAIN_PCG_0[8:0]
	//{ .regAddr= 0x3091, .regData= 0x00, .delayUs=0 }, // GAIN_PCG_0[8:0]
	{ .regAddr= 0x3090, .regData= 0x00, .delayUs=0 }, // GAIN_PCG_0[8:0]
	{ .regAddr= 0x3091, .regData= 0x00, .delayUs=0 }, // GAIN_PCG_0[8:0]
	{ .regAddr= 0x30C1, .regData= 0x00, .delayUs=0 }, // XVS_DRV[1:0]

	{ .regAddr= 0x30D9, .regData= 0x06, .delayUs=0 }, // DIG_CLP_VSTAET[4:0]
	{ .regAddr= 0x30DA, .regData= 0x02, .delayUs=0 }, // DIG_VLP_VNUM[1:0]

	{ .regAddr= 0x3115, .regData= 0x00, .delayUs=0 }, // INCKSEL1[7:0]
	{ .regAddr= 0x3116, .regData= 0x23, .delayUs=0 }, // INCKSEL2[7:0]
	{ .regAddr= 0x3118, .regData= 0xB4, .delayUs=0 }, // INCKSEL3[10:0]
	{ .regAddr= 0x3119, .regData= 0x00, .delayUs=0 }, // INCKSEL3[10:0]
	{ .regAddr= 0x311A, .regData= 0xFC, .delayUs=0 }, // INCKSEL4[10:0]
	{ .regAddr= 0x311B, .regData= 0x00, .delayUs=0 }, // INCKSEL4[10:0]
	{ .regAddr= 0x311E, .regData= 0x23, .delayUs=0 }, // INCKSEL5[7:0]
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
	{ .regAddr= 0x4004, .regData= 0x00, .delayUs=0 }, // TXCLKESC_FREQ[15:0]
	{ .regAddr= 0x4005, .regData= 0x06, .delayUs=0 }, //
	{ .regAddr= 0x400C, .regData= 0x01, .delayUs=0 }, // INCKSEL6

	{ .regAddr= 0x4018, .regData= 0x9f, .delayUs=0 }, // TCLKPOST[15:0]
	{ .regAddr= 0x4019, .regData= 0x00, .delayUs=0 }, // TCLKPOST[15:0]
	{ .regAddr= 0x401A, .regData= 0x57, .delayUs=0 }, // TCLKPREPARE[15:0]
	{ .regAddr= 0x401B, .regData= 0x00, .delayUs=0 }, // TCLKPREPARE[15:0]
	{ .regAddr= 0x401C, .regData= 0x57, .delayUs=0 }, // TCLKTRAI[15:0]
	{ .regAddr= 0x401D, .regData= 0x00, .delayUs=0 }, // TCLKTRAI[15:0]
	{ .regAddr= 0x401E, .regData= 0x87, .delayUs=0 }, // TCLKZERO[15:0]
	{ .regAddr= 0x401F, .regData= 0x01, .delayUs=0 }, // TCLKZERO[15:0]
	{ .regAddr= 0x4020, .regData= 0x5F, .delayUs=0 }, // THSPREPARE[15:0]
	{ .regAddr= 0x4021, .regData= 0x00, .delayUs=0 }, // THSPREPARE[15:0]
	{ .regAddr= 0x4022, .regData= 0xA7, .delayUs=0 }, // THSZERO[15:0]
	{ .regAddr= 0x4023, .regData= 0x00, .delayUs=0 }, // THSZERO[15:0]
	{ .regAddr= 0x4024, .regData= 0x5F, .delayUs=0 }, // THSTRAI[15:0]
	{ .regAddr= 0x4025, .regData= 0x00, .delayUs=0 }, // THSTRAI[15:0]
	{ .regAddr= 0x4026, .regData= 0x97, .delayUs=0 }, // THSEXIT[15:0]
	{ .regAddr= 0x4027, .regData= 0x00, .delayUs=0 }, // THSEXIT[15:0]
	{ .regAddr= 0x4028, .regData= 0x4F, .delayUs=0 }, // TLPX[15:0]
	{ .regAddr= 0x4029, .regData= 0x00, .delayUs=0 }, // TLPX[15:0]
	{ .regAddr= 0x4074, .regData= 0x00, .delayUs=0 }, // INCKSEL7 [2:0]
    { .regAddr = 0x3000, .regData = 0x00, .delayUs = 24000}, /* leave standby */
};

//all pixel scan mode, 24Mhz, 2lane, lane clock 1440M, AD 10bit, MD 12bit VCMODE
static drv_sensor_reg_info_t  imx415_5MP_30fps_dol2_vcmode_setting[] =
{
	{ .regAddr= 0x3000, .regData= 0x01, .delayUs=0 }, /* enter standby */
	{ .regAddr= 0x3002, .regData= 0x01, .delayUs=0 }, /* master stop */
	{ .regAddr= 0x3008, .regData= 0x54, .delayUs=0 }, // BCWAIT_TIME[9:0]
	{ .regAddr= 0x3009, .regData= 0x00, .delayUs=0 }, // BCWAIT_TIME[9:0]
	{ .regAddr= 0x300A, .regData= 0x3B, .delayUs=0 }, // CPWAIT_TIME[9:0]
	{ .regAddr= 0x300B, .regData= 0x00, .delayUs=0 }, // CPWAIT_TIME[9:0]
//cropping
	{ .regAddr= 0x301c, .regData= 0x04, .delayUs=0 },  //WINMODE  4h:window croping mode
	{ .regAddr= 0x3040, .regData= 0x00, .delayUs=0 },  //PIX_HST effective pixel Start position(H direction)
	{ .regAddr= 0x3041, .regData= 0x00, .delayUs=0 },
	{ .regAddr= 0x3042, .regData= 0x80, .delayUs=0 },  //PIX_HWIDTH  4h:window croping mode, 2688(multiplies of 24)
	{ .regAddr= 0x3043, .regData= 0x0A, .delayUs=0 },
	{ .regAddr= 0x3044, .regData= 0x00, .delayUs=0 },  //PIX_VST  4h:window croping mode
	{ .regAddr= 0x3045, .regData= 0x00, .delayUs=0 },
	{ .regAddr= 0x3046, .regData= 0xa8, .delayUs=0 },   //PIX_VWIDTH  4h:window croping mode, 1860
	{ .regAddr= 0x3047, .regData= 0x0e, .delayUs=0 },

	{ .regAddr= 0x3020, .regData= 0x01, .delayUs=0 },  //horizontal 2 binning
	{ .regAddr= 0x3021, .regData= 0x0, .delayUs=0 },  //veritical 2 binning
	{ .regAddr= 0x3022, .regData= 0x0, .delayUs=0 },  //ADDMODE 2/2-line binning
	{ .regAddr= 0x3024, .regData= 0xCA, .delayUs=0 }, // VMAX[19:0]
	{ .regAddr= 0x3025, .regData= 0x08, .delayUs=0 }, // VMAX[19:0]
	{ .regAddr= 0x3026, .regData= 0x00, .delayUs=0 }, // VMAX[19:0]

	{ .regAddr= 0x3028, .regData= 0x2a, .delayUs=0 }, // HMAX[15:0]
	{ .regAddr= 0x3029, .regData= 0x04, .delayUs=0 }, // HMAX[15:0]
  	{ .regAddr= 0x3030, .regData= 0x00, .delayUs=0 },  //normal
	{ .regAddr= 0x3031, .regData= 0x00, .delayUs=0 }, // AD:10 bit
	{ .regAddr= 0x3032, .regData= 0x01, .delayUs=0 }, // MD:12 bit
	{ .regAddr= 0x3033, .regData= 0x08, .delayUs=0 }, // SYS_MODE[3:0]

	// { .regAddr= 0x302c, .regData= 0x00, .delayUs=0 }, // WDMODE[1:0] 0:normal exposure
	// { .regAddr= 0x302d, .regData= 0x00, .delayUs=0 }, // WDSEL[1:0] 0:normal exposure
	{ .regAddr= 0x30CF, .regData= 0x01, .delayUs=0 }, // XVSMSKCNT_INT[1:0]

	{ .regAddr= 0x3023, .regData= 0x01, .delayUs=0 },  //VCMODE
	{ .regAddr= 0x302c, .regData= 0x01, .delayUs=0 }, // WDMODE[1:0]
	{ .regAddr= 0x302d, .regData= 0x01, .delayUs=0 }, // WDSEL[1:0]
	{ .regAddr= 0x3050, .regData= 0x26, .delayUs=0 }, // SHR0[19:0]
	{ .regAddr= 0x3051, .regData= 0x0F, .delayUs=0 }, // SHR0[19:0]
	{ .regAddr= 0x3054, .regData= 0x09, .delayUs=0 }, // SHR1[19:0]
	{ .regAddr= 0x3060, .regData= 0x4d, .delayUs=0 }, // RHS1[19:0]

	// { .regAddr= 0x3050, .regData= 0xC6, .delayUs=0 }, // SHR0[19:0]
	// { .regAddr= 0x3051, .regData= 0x02, .delayUs=0 }, // SHR0[19:0]
	// { .regAddr= 0x3052, .regData= 0x00, .delayUs=0 }, // SHR0[19:0]
	//{ .regAddr= 0x3050, .regData= 0x66, .delayUs=0 }, // SHR0[19:0]
	//{ .regAddr= 0x3051, .regData= 0x00, .delayUs=0 }, //
	//{ .regAddr= 0x3052, .regData= 0x00, .delayUs=0 }, //
	//{ .regAddr= 0x3090, .regData= 0x14, .delayUs=0 }, // GAIN_PCG_0[8:0]
	//{ .regAddr= 0x3091, .regData= 0x00, .delayUs=0 }, // GAIN_PCG_0[8:0]
	{ .regAddr= 0x3090, .regData= 0x00, .delayUs=0 }, // GAIN_PCG_0[8:0]
	{ .regAddr= 0x3091, .regData= 0x00, .delayUs=0 }, // GAIN_PCG_0[8:0]
	{ .regAddr= 0x30C1, .regData= 0x00, .delayUs=0 }, // XVS_DRV[1:0]

	{ .regAddr= 0x30D9, .regData= 0x06, .delayUs=0 }, // DIG_CLP_VSTAET[4:0]
	{ .regAddr= 0x30DA, .regData= 0x02, .delayUs=0 }, // DIG_VLP_VNUM[1:0]

	{ .regAddr= 0x3115, .regData= 0x00, .delayUs=0 }, // INCKSEL1[7:0]
	{ .regAddr= 0x3116, .regData= 0x23, .delayUs=0 }, // INCKSEL2[7:0]
	{ .regAddr= 0x3118, .regData= 0xB4, .delayUs=0 }, // INCKSEL3[10:0]
	{ .regAddr= 0x3119, .regData= 0x00, .delayUs=0 }, // INCKSEL3[10:0]
	{ .regAddr= 0x311A, .regData= 0xFC, .delayUs=0 }, // INCKSEL4[10:0]
	{ .regAddr= 0x311B, .regData= 0x00, .delayUs=0 }, // INCKSEL4[10:0]
	{ .regAddr= 0x311E, .regData= 0x23, .delayUs=0 }, // INCKSEL5[7:0]
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
	{ .regAddr= 0x4004, .regData= 0x00, .delayUs=0 }, // TXCLKESC_FREQ[15:0]
	{ .regAddr= 0x4005, .regData= 0x06, .delayUs=0 }, //
	{ .regAddr= 0x400C, .regData= 0x01, .delayUs=0 }, // INCKSEL6

	{ .regAddr= 0x4018, .regData= 0x9f, .delayUs=0 }, // TCLKPOST[15:0]
	{ .regAddr= 0x4019, .regData= 0x00, .delayUs=0 }, // TCLKPOST[15:0]
	{ .regAddr= 0x401A, .regData= 0x57, .delayUs=0 }, // TCLKPREPARE[15:0]
	{ .regAddr= 0x401B, .regData= 0x00, .delayUs=0 }, // TCLKPREPARE[15:0]
	{ .regAddr= 0x401C, .regData= 0x57, .delayUs=0 }, // TCLKTRAI[15:0]
	{ .regAddr= 0x401D, .regData= 0x00, .delayUs=0 }, // TCLKTRAI[15:0]
	{ .regAddr= 0x401E, .regData= 0x87, .delayUs=0 }, // TCLKZERO[15:0]
	{ .regAddr= 0x401F, .regData= 0x01, .delayUs=0 }, // TCLKZERO[15:0]
	{ .regAddr= 0x4020, .regData= 0x5F, .delayUs=0 }, // THSPREPARE[15:0]
	{ .regAddr= 0x4021, .regData= 0x00, .delayUs=0 }, // THSPREPARE[15:0]
	{ .regAddr= 0x4022, .regData= 0xA7, .delayUs=0 }, // THSZERO[15:0]
	{ .regAddr= 0x4023, .regData= 0x00, .delayUs=0 }, // THSZERO[15:0]
	{ .regAddr= 0x4024, .regData= 0x5F, .delayUs=0 }, // THSTRAI[15:0]
	{ .regAddr= 0x4025, .regData= 0x00, .delayUs=0 }, // THSTRAI[15:0]
	{ .regAddr= 0x4026, .regData= 0x97, .delayUs=0 }, // THSEXIT[15:0]
	{ .regAddr= 0x4027, .regData= 0x00, .delayUs=0 }, // THSEXIT[15:0]
	{ .regAddr= 0x4028, .regData= 0x4F, .delayUs=0 }, // TLPX[15:0]
	{ .regAddr= 0x4029, .regData= 0x00, .delayUs=0 }, // TLPX[15:0]
	{ .regAddr= 0x4074, .regData= 0x00, .delayUs=0 }, // INCKSEL7 [2:0]
    { .regAddr = 0x3000, .regData = 0x00, .delayUs = 24000}, /* leave standby */
};

static drv_sensor_reg_info_t  imx415_streamOn_setting[] =
{
	// { .regAddr = 0x3000, .regData = 0x00, .delayUs = 24000}, /* standby */
    { .regAddr = 0x3002, .regData = 0x00, .delayUs = 0x0 },  /* master*/
//    { .regAddr = 0x30c0, .regData = 0x2a, .delayUs = 0x0,},
};

static drv_sensor_reg_info_t  imx415_streamOff_setting[] =
{
	// { .regAddr= 0x3000, .regData= 0x01, .delayUs=0 }, /* enter standby */
    { .regAddr = 0x3002, .regData = 0x01, .delayUs = 0x0,},  /*stop master*/
};

static drv_sensor_exp_param_t imx415_2MP_10fps_exparams =
{
    .maxIntegrationTime = 2242  ,
    .minIntegrationTime = 4     ,
    .maxAnalogGain      = 33532,
    .minAnalogGain      = 1024  ,
    .lineTime           = 182044,
    .limitShortMid      = 0     ,
    .limitLong          = 0     ,
    .expLineMargin      = 1     ,
};
static drv_sensor_exp_param_t imx415_2MP_14fps_exparams =
{
    .maxIntegrationTime = 2242  ,
    .minIntegrationTime = 4     ,
    .maxAnalogGain      = 33532,
    .minAnalogGain      = 1024  ,
    .lineTime           = 60681,
    .limitShortMid      = 0     ,
    .limitLong          = 0     ,
    .expLineMargin      = 1     ,
};
static drv_sensor_exp_param_t imx415_2MP_15fps_exparams =
{
    .maxIntegrationTime = 2242  ,
    .minIntegrationTime = 4     ,
    .maxAnalogGain      = 33532,
    .minAnalogGain      = 1024  ,
    .lineTime           = 121362,
    .limitShortMid      = 0     ,
    .limitLong          = 0     ,
    .expLineMargin      = 1     ,
};
static drv_sensor_exp_param_t imx415_2MP_20fps_exparams =
{
    .maxIntegrationTime = 2242  ,
    .minIntegrationTime = 4     ,
    .maxAnalogGain      = 33532,
    .minAnalogGain      = 1024  ,
    .lineTime           = 91022,
    .limitShortMid      = 0     ,
    .limitLong          = 0     ,
    .expLineMargin      = 1     ,
};
static drv_sensor_exp_param_t imx415_2MP_30fps_exparams =
{
    .maxIntegrationTime = 6317  ,
    .minIntegrationTime = 4     ,
    .maxAnalogGain      = 33532,
    .minAnalogGain      = 1024  ,
    .lineTime           = 60681,
    .limitShortMid      = 0     ,
    .limitLong          = 0     ,
    .expLineMargin      = 1     ,
};
static drv_sensor_exp_param_t imx415_30fps_dol2_exparams =
{
    .maxIntegrationTime = 500  ,//2242
    .minIntegrationTime = 8     ,
    .maxAnalogGain      = 33532,
    .minAnalogGain      = 1024  ,
    .lineTime           = 28139,
    .limitShortMid      = 500     ,
    .limitLong          = 2000     ,
    .expLineMargin      = 1     ,
};
static drv_sensor_exp_param_t imx415_5MP_15fps_exparams =
{
    .maxIntegrationTime = 4492  ,
    .minIntegrationTime = 4     ,
    .maxAnalogGain      = 33532,
    .minAnalogGain      = 1024  ,
    .lineTime           = 60681,
    .limitShortMid      = 0     ,
    .limitLong          = 0     ,
    .expLineMargin      = 1     ,
};

static drv_sensor_exp_param_t imx415_2MP_15fps_37_to_24_exparams =//for dt57_2mp_24mhz
{
    .maxIntegrationTime = 2992  ,
    .minIntegrationTime = 4     ,
    .maxAnalogGain      = 33532,
    .minAnalogGain      = 1024  ,
    .lineTime           = 93383,//91022
    .limitShortMid      = 0     ,
    .limitLong          = 0     ,
    .expLineMargin      = 1     ,
};
static drv_power_setting_t imx415_power_up_setting[] =
{
    {.type = DRV_POWER_REG, .regAddr = 0x83c40004, .bitMask  = 0x01, .bitVal = 0x00, .delayUs = 0}, /// 50ms
    {.type = DRV_POWER_REG, .regAddr = 0x83c40004, .bitMask  = 0x01, .bitVal = 0x01, .delayUs = 0}  /// APB2 SENSOR_CTRL_REG 0x83c40004
};


static drv_power_setting_t imx415_power_down_setting[] =
{
    {.type = DRV_POWER_REG, .regAddr = 0x83c40004, .bitMask  = 0x01, .bitVal = 0x00, .delayUs = 0},
    {.type = DRV_POWER_REG, .regAddr = 0x83c40004, .bitMask  = 0x01, .bitVal = 0x01, .delayUs = 0}
};

static sensor_drv_params_t imx415_sensor_params =
{
    .sensorId     = 0xe,
    .sensorIdAddr = 0x31dc,
    .name         = "imx415",
    .slaveInfo =
    {
        .hwType      = DRV_HW_I2C,
        .slaveAddr   = (0x20 >> 1),
        .channel     = 0,
        .regDataType = DRV_ADDR_TYPE_BYTE,
        .regAddrType = DRV_DATA_TYPE_WORD,
    },
    .powerUpSettings  =
    {
        .powerSetting = imx415_power_up_setting,
        .size = sizeof(imx415_power_up_setting) / sizeof(imx415_power_up_setting[0]),
    },
    .powerDownSettings =
    {
        .powerSetting  = imx415_power_down_setting,
        .size = sizeof(imx415_power_down_setting) / sizeof(imx415_power_down_setting[0]),
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
	.modeSettings[0] =  //setting for FPGA
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
			.settings    = imx415_1080p_setting,
			.settingSize = sizeof(imx415_1080p_setting) / sizeof (imx415_1080p_setting[0]),
		},
		.expParams   = &imx415_2MP_10fps_exparams,

		.intf = {
			.ln_cfg = {
				.dat_num = 2,
			},
			.link_freq = 594.0f,
		},
	},
	.modeSettings[1] = //Crop 1080P
	{
		.maxFps = 15,
        .minFps = 12,
        .frameLength = 0x1194,
        .expLineMargin = 4,
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
            .settings    = imx415_4MP_30fps_setting,
            .settingSize = sizeof(imx415_4MP_30fps_setting) / sizeof (imx415_4MP_30fps_setting[0]),
        },
        .expParams   = &imx415_2MP_30fps_exparams,

        .intf = {
          .ln_cfg = {
            .dat_num = 2,
          },
          .link_freq = 1440.0f,
		},
	},

	.modeSettings[2] =
	{
		.maxFps = 30,
		.minFps = 20,
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
			.settings    = imx415_1080p_30fps_setting,
			.settingSize = sizeof(imx415_1080p_30fps_setting) / sizeof (imx415_1080p_30fps_setting[0]),
		},
		.expParams   = &imx415_2MP_30fps_exparams,

		.intf = {
			.ln_cfg = {
				.dat_num = 2,
			},
			.link_freq = 1440.0f,
		},
	},
	.modeSettings[3] =
	{
		.maxFps = 15,
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
			.settings    = imx415_1080p_15fps_setting,
			.settingSize = sizeof(imx415_1080p_15fps_setting) / sizeof (imx415_1080p_15fps_setting[0]),
		},
		.expParams   = &imx415_2MP_15fps_exparams,

		.intf = {
			.ln_cfg = {
				.dat_num = 2,
			},
			.link_freq = 720.0f,
		},
	},
	.modeSettings[4] =
	{
		.maxFps = 15,
        .minFps = 5,
        .frameLength = 0x1194,
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
            .settings    = imx415_4MP_30fps_setting,
            .settingSize = sizeof(imx415_4MP_30fps_setting) / sizeof (imx415_4MP_30fps_setting[0]),
        },
        .expParams   = &imx415_2MP_30fps_exparams,

        .intf = {
          .ln_cfg = {
            .dat_num = 2,
          },
          .link_freq = 1440.0f,
		},
	},
	.modeSettings[5] =
	{
		.maxFps = 30,
		.minFps = 30,
		.frameLength = 0x8ca,
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
			.settings    = imx415_4mp_15fps_setting,
			.settingSize = sizeof(imx415_4mp_15fps_setting) / sizeof (imx415_4mp_15fps_setting[0]),
		},
		.expParams   = &imx415_2MP_15fps_exparams,

		.intf = {
			.ln_cfg = {
				.dat_num = 2,
			},
			.link_freq = 720.0f,
		},
	},
	.modeSettings[6] =
    {
        .maxFps = 15,
        .minFps = 15,
        .frameLength = 0x9c0,
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
            .settings    = imx415_1080p_30fps_dol2_setting,
            .settingSize = sizeof(imx415_1080p_30fps_dol2_setting) / sizeof (imx415_1080p_30fps_dol2_setting[0]),
        },
        .expParams   = &imx415_30fps_dol2_exparams,

        .intf = {
          .ln_cfg = {
            .dat_num = 2,
          },
          .link_freq = 1440.0f,
        },
    },
	.modeSettings[7] =
	{
		.maxFps = 15,
		.minFps = 15,
		.frameLength = 0x8ca,
		.expLineMargin = 1,
		.dim =
		{
			.width  = 2560,
			.height = 1440,
		},
		.hdrMode   = SENSOR_DOL2_MODE,
		.regConfig =
		{
			.regDataType = DRV_DATA_TYPE_BYTE,
			.regAddrType = DRV_ADDR_TYPE_WORD,
			.direction   = DRV_DIRECTION_WRITE,
			.settings    = imx415_4MP_30fps_dol2_setting,
			.settingSize = sizeof(imx415_4MP_30fps_dol2_setting) / sizeof (imx415_4MP_30fps_dol2_setting[0]),
		},
		.expParams   = &imx415_30fps_dol2_exparams,

		.intf = {
			.ln_cfg = {
				.dat_num = 2,
			},
			.link_freq = 1440.0f,
		},
	},
	.modeSettings[8] =
	{
		.maxFps = 15,
		.minFps = 15,
		.frameLength = 0x8ca,
		.expLineMargin = 1,
		.dim =
		{
			.width  = 2688,
			.height = 1520,
		},
		.hdrMode   = SENSOR_DOL2_MODE,
		.regConfig =
		{
			.regDataType = DRV_DATA_TYPE_BYTE,
			.regAddrType = DRV_ADDR_TYPE_WORD,
			.direction   = DRV_DIRECTION_WRITE,
			.settings    = imx415_5MP_30fps_dol2_setting,
			.settingSize = sizeof(imx415_5MP_30fps_dol2_setting) / sizeof (imx415_5MP_30fps_dol2_setting[0]),
		},
		.expParams   = &imx415_30fps_dol2_exparams,

		.intf = {
			.ln_cfg = {
				.dat_num = 2,
			},
			.link_freq = 1440.0f,
		},
	},
	.modeSettings[9] =
	{
		    .maxFps = 15,
        .minFps = 15,
        .frameLength = 0x1194,//0x8ca,
        .expLineMargin = 1,
        .dim =
        {
            .width  = 2688,
            .height = 1512,
        },
        .hdrMode   = SENSOR_NORMAL_MODE,
        .regConfig =
        {
            .regDataType = DRV_DATA_TYPE_BYTE,
            .regAddrType = DRV_ADDR_TYPE_WORD,
            .direction   = DRV_DIRECTION_WRITE,
            .settings    = imx415_5MP_30fps_setting,//imx415_5MP_30fps_setting is wrong.
            .settingSize = sizeof(imx415_5MP_30fps_setting) / sizeof (imx415_5MP_30fps_setting[0]),
        },
        .expParams   = &imx415_5MP_15fps_exparams,

        .intf = {
          .ln_cfg = {
            .dat_num = 2,
          },
          .link_freq = 1440.0f,
		},
	},
	.modeSettings[10] = //Crop 300w
	{
		.maxFps = 13,
        .minFps = 13,
        .frameLength = 0x1448, // 15fps对应0x1194 0x1194 = 4500, 4500*15 = 67,500, 67500 / 13 = 5,192.30 = 0x1448 , 1,125 + 5,192 = 6317
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
            .settings    = imx415_4MP_30fps_setting,
            .settingSize = sizeof(imx415_4MP_30fps_setting) / sizeof (imx415_4MP_30fps_setting[0]),
        },
        .expParams   = &imx415_2MP_30fps_exparams,

        .intf = {
          .ln_cfg = {
            .dat_num = 2,
          },
          .link_freq = 1440.0f,
		},
	},
	.modeSettings[11] =
	{
		.maxFps = 30,
        .minFps = 15,
        .frameLength = 0x8ca,
        .expLineMargin = 1,
        .dim =
        {
            .width  = 2592,
            .height = 1944,
        },
        .hdrMode   = SENSOR_NORMAL_MODE,
        .regConfig =
        {
            .regDataType = DRV_DATA_TYPE_BYTE,
            .regAddrType = DRV_ADDR_TYPE_WORD,
            .direction   = DRV_DIRECTION_WRITE,
            .settings    = imx415_5MP_4_3_30fps_setting,
            .settingSize = sizeof(imx415_5MP_4_3_30fps_setting) / sizeof (imx415_5MP_4_3_30fps_setting[0]),
        },
        .expParams   = &imx415_2MP_30fps_exparams,

        .intf = {
          .ln_cfg = {
            .dat_num = 2,
          },
          .link_freq = 1440.0f,
		},
	},
    .modeSettings[12] =
    {
        .maxFps = 15,
        .minFps = 15,
        .frameLength = 0x9c0,
        .expLineMargin = 1,
        .dim =
        {
            .width  = 1920,
            .height = 1080,
        },
        .hdrMode   = SENSOR_STAGGER_2_MODE,
        .regConfig =
        {
            .regDataType = DRV_DATA_TYPE_BYTE,
            .regAddrType = DRV_ADDR_TYPE_WORD,
            .direction   = DRV_DIRECTION_WRITE,
            .settings    = imx415_1080p_30fps_dol2_vcmode_setting,
            .settingSize = sizeof(imx415_1080p_30fps_dol2_vcmode_setting) / sizeof (imx415_1080p_30fps_dol2_vcmode_setting[0]),
        },
        .expParams   = &imx415_30fps_dol2_exparams,

        .intf = {
            .ln_cfg = {
            .dat_num = 2,
            },
            .link_freq = 1440.0f,
        },
    },
    .modeSettings[13] =
    {
        .maxFps = 15,
        .minFps = 15,
        .frameLength = 0x8ca,
        .expLineMargin = 1,
        .dim =
        {
            .width  = 2560,
            .height = 1440,
        },
        .hdrMode   = SENSOR_STAGGER_2_MODE,
        .regConfig =
        {
            .regDataType = DRV_DATA_TYPE_BYTE,
            .regAddrType = DRV_ADDR_TYPE_WORD,
            .direction   = DRV_DIRECTION_WRITE,
            .settings    = imx415_4MP_30fps_dol2_vcmode_setting,
            .settingSize = sizeof(imx415_4MP_30fps_dol2_vcmode_setting) / sizeof (imx415_4MP_30fps_dol2_vcmode_setting[0]),
        },
        .expParams   = &imx415_30fps_dol2_exparams,

        .intf = {
            .ln_cfg = {
                .dat_num = 2,
            },
            .link_freq = 1440.0f,
        },
    },
    .modeSettings[14] =
    {
        .maxFps = 15,
        .minFps = 15,
        .frameLength = 0x8ca,
        .expLineMargin = 1,
        .dim =
        {
            .width  = 2688,
            .height = 1520,
        },
        .hdrMode   = SENSOR_STAGGER_2_MODE,
        .regConfig =
        {
            .regDataType = DRV_DATA_TYPE_BYTE,
            .regAddrType = DRV_ADDR_TYPE_WORD,
            .direction   = DRV_DIRECTION_WRITE,
            .settings    = imx415_5MP_30fps_dol2_vcmode_setting,
            .settingSize = sizeof(imx415_5MP_30fps_dol2_vcmode_setting) / sizeof (imx415_5MP_30fps_dol2_vcmode_setting[0]),
        },
        .expParams   = &imx415_30fps_dol2_exparams,

        .intf = {
            .ln_cfg = {
                .dat_num = 2,
            },
            .link_freq = 1440.0f,
        },
    },
    .streamOnSetting =
    {
        .regDataType = DRV_DATA_TYPE_BYTE,
        .regAddrType = DRV_ADDR_TYPE_WORD,
        .direction   = DRV_DIRECTION_WRITE,
        .settings    = imx415_streamOn_setting,
        .settingSize = sizeof(imx415_streamOn_setting) / sizeof (imx415_streamOn_setting[0]),
    },

    .streamOffSetting =
    {
        .regDataType = DRV_DATA_TYPE_BYTE,
        .regAddrType = DRV_ADDR_TYPE_WORD,
        .direction   = DRV_DIRECTION_WRITE,
        .settings    = imx415_streamOff_setting,
        .settingSize = sizeof(imx415_streamOff_setting) / sizeof (imx415_streamOff_setting[0]),
    },

    .fill_exposure_reg = IMX415_Fill_Exposure_Reg,
};


sensor_drv_params_t* load_imx415_sensor_driver(void);

#endif

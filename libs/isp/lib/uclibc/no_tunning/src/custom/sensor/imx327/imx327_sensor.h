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

#ifndef __IMX327_SENSOR_H__
#define __IMX327_SENSOR_H__

#include "sensor_common.h"
#include "log_utils.h"

isp_ret_t IMX327_Fill_Exposure_Reg(sensor_apply_exp_param_t * pExpParam, drv_regs_array_info_t * pRegInfo);

#define IMX327_2LANE
static drv_sensor_reg_info_t  imx327_normal_720P_setting[] =
{
    { .regAddr = 0x3003, .regData = 0x01, .delayUs = 100000,},
    { .regAddr = 0x3003, .regData = 0x00, .delayUs = 100000,},

	{ .regAddr= 0x3000, .regData= 0x01L, .delayUs=0 }, /* enter standby */
	{ .regAddr= 0x3002, .regData= 0x01L, .delayUs=0 }, /* master mode */
	/*ADBIT*/
	{ .regAddr= 0x3005, .regData= 0x01L, .delayUs=0 }, /* ADBIT: */ // 0=10Bit; 0x01(default)=12Bit
	{ .regAddr= 0x3129, .regData= 0x00L, .delayUs=0 }, // 0x00); /*ADBIT1*/  // Noted by wsh: 0x1d=10-bit. 0x00 = 12-bit.
	{ .regAddr= 0x317c, .regData= 0x00L, .delayUs=0 }, // 0x00); /*ADBIT2*/  // Noted by wsh: 0x12=10-bit. 0x00 = 12-bit.
	{ .regAddr= 0x31ec, .regData= 0x0eL, .delayUs=0 }, // 0x0e); /*ADBIT3*/  // Noted by wsh: 0x37=10-bit. 0x0e = 12-bit.

	{ .regAddr= 0x3007, .regData= 0x013L, .delayUs=0 }, /* WINMODE: 1080P */ // Noted by wsh: [6:4]: default 0, 0=Full HD 1080p, 1=HD720P, 4=Window cropping from Full HD 1080P.
	{ .regAddr= 0x3009, .regData= 0x02L, .delayUs=0 }, // Noted by wsh: 2=30 frames/s.
//	{ .regAddr= 0x300a, .regData= 0xf0L, .delayUs=0 }, // Noted by wsh: 0x3c for 10-bit, 0xf0 for 12-bit.

	/*INCK*/
	{ .regAddr= 0x305c, .regData= 0x20L, .delayUs=0 }, /*inck1*/
	{ .regAddr= 0x305d, .regData= 0x00L, .delayUs=0 }, /*inck2*/
	{ .regAddr= 0x305e, .regData= 0x20L, .delayUs=0 }, /*inck3*/
	{ .regAddr= 0x305f, .regData= 0x01L, .delayUs=0 }, /*inck4 */
	{ .regAddr= 0x315e, .regData= 0x1aL, .delayUs=0 }, /*inck5*/
	{ .regAddr= 0x3164, .regData= 0x1aL, .delayUs=0 }, /*inck6*/
	{ .regAddr= 0x3480, .regData= 0x49L, .delayUs=0 }, /*inck7*/

#ifdef IMX327_2LANE
	{ .regAddr= 0x3018, .regData= 0xeeL, .delayUs=0 },  /*VMAX  750 */
	{ .regAddr= 0x3019, .regData= 0x02L, .delayUs=0 },
	{ .regAddr= 0x301c, .regData= 0xc8L, .delayUs=0 },  /*HMAX  6600 */
	{ .regAddr= 0x301d, .regData= 0x19L, .delayUs=0 }, 
#else
	{ .regAddr= 0x3018, .regData= 0xee, .delayUs=0 },  // VMAX
	{ .regAddr= 0x3019, .regData= 0x02, .delayUs=0 },
	{ .regAddr= 0x301c, .regData= 0xc8, .delayUs=0 }, /*Hmax*/
	{ .regAddr= 0x301d, .regData= 0x19, .delayUs=0 },
#endif
//	{ .regAddr= 0x300C, .regData= 0x00L, .delayUs=0 }, /* set normal mode */
//	{ .regAddr= 0x3044, .regData= 0x01L, .delayUs=0 },
	{ .regAddr= 0x3046, .regData= 0x01L, .delayUs=0 },  /*ODBIT*/ //output data bit

	/*fixed*/
	{ .regAddr= 0x3011, .regData= 0x02L, .delayUs=0 },
	{ .regAddr= 0x309e, .regData= 0x4aL, .delayUs=0 },
	{ .regAddr= 0x309f, .regData= 0x4aL, .delayUs=0 },
	{ .regAddr= 0x30d2, .regData= 0x19L, .delayUs=0 },
	{ .regAddr= 0x30d7, .regData= 0x03L, .delayUs=0 },
	{ .regAddr= 0x313b, .regData= 0x61L, .delayUs=0 },

#ifdef IMX327_2LANE
	{ .regAddr= 0x3405, .regData= 0x10L, .delayUs=0 }, // ? Noted by wsh: [5:4]: RECEPTION, 2=222.75MHz, 1=445.5MHz, 0=891MHz.
	{ .regAddr= 0x3407, .regData= 0x01L, .delayUs=0 }, // Noted by wsh: PHYSICAL_LANE_NUM, 3=4ch, 1=2ch.
#else
	{ .regAddr= 0x3405, .regData= 0x20L, .delayUs=0 }, // ? Noted by wsh: [5:4]: RECEPTION, 2=222.75MHz, 1=445.5MHz, 0=891MHz.
	{ .regAddr= 0x3407, .regData= 0x03L, .delayUs=0 }, // Noted by wsh: PHYSICAL_LANE_NUM, 3=4ch, 1=2ch.
#endif
	{ .regAddr= 0x3414, .regData= 0x04L, .delayUs=0 }, // Noted by wsh: [5:0]: OPB_SIZE_V, default 0x0A.
//	{ .regAddr= 0x3415, .regData= 0x01L, .delayUs=0 },

	{ .regAddr= 0x3418, .regData= 0xd9L, .delayUs=0 }, /*Y_OUT_SIZE*/
	{ .regAddr= 0x3419, .regData= 0x02L, .delayUs=0 }, // 729

	{ .regAddr= 0x3441, .regData= 0x0CL, .delayUs=0 }, // Noted by wsh: CSI_DT_FMT, 0x0a0a=RAW10, 0x0c0c=RAW12.
	{ .regAddr= 0x3442, .regData= 0x0CL, .delayUs=0 },
#ifdef IMX327_2LANE
	{ .regAddr= 0x3443, .regData= 0x01L, .delayUs=0 }, // Noted by wsh: CSI_LANE_MODE, 1=2 lane, 3=4 lane.
#else
	{ .regAddr= 0x3443, .regData= 0x03L, .delayUs=0 }, // Noted by wsh: CSI_LANE_MODE, 1=2 lane, 3=4 lane.
#endif
	{ .regAddr= 0x3444, .regData= 0x20L, .delayUs=0 }, // Noted by wsh: EXTCK_FREQ, 2520 for 37.125MHz INCK, 4A40 for 74.25MHz INCK.
	{ .regAddr= 0x3445, .regData= 0x25L, .delayUs=0 },
#ifdef IMX327_2LANE
	{ .regAddr= 0x3446, .regData= 0x4fL, .delayUs=0 },
	{ .regAddr= 0x3447, .regData= 0x00L, .delayUs=0 },

	{ .regAddr= 0x3448, .regData= 0x2fL, .delayUs=0 },
	{ .regAddr= 0x3449, .regData= 0x00L, .delayUs=0 },

	{ .regAddr= 0x344a, .regData= 0x17L, .delayUs=0 },
	{ .regAddr= 0x344b, .regData= 0x00L, .delayUs=0 },

	{ .regAddr= 0x344c, .regData= 0x17L, .delayUs=0 },
	{ .regAddr= 0x344d, .regData= 0x00L, .delayUs=0 },

	{ .regAddr= 0x344e, .regData= 0x17L, .delayUs=0 },
	{ .regAddr= 0x344f, .regData= 0x00L, .delayUs=0 },

	{ .regAddr= 0x3450, .regData= 0x57L, .delayUs=0 },
	{ .regAddr= 0x3451, .regData= 0x00L, .delayUs=0 },

	{ .regAddr= 0x3452, .regData= 0x17L, .delayUs=0 },
	{ .regAddr= 0x3453, .regData= 0x00L, .delayUs=0 },

	{ .regAddr= 0x3454, .regData= 0x17L, .delayUs=0 },
	{ .regAddr= 0x3455, .regData= 0x00L, .delayUs=0 },
#else
	{ .regAddr= 0x3446, .regData= 0x47L, .delayUs=0 },
	{ .regAddr= 0x3447, .regData= 0x00L, .delayUs=0 },

	{ .regAddr= 0x3448, .regData= 0x17L, .delayUs=0 },
	{ .regAddr= 0x3449, .regData= 0x00L, .delayUs=0 },

	{ .regAddr= 0x344a, .regData= 0x0FL, .delayUs=0 },
	{ .regAddr= 0x344b, .regData= 0x00L, .delayUs=0 },

	{ .regAddr= 0x344c, .regData= 0x0FL, .delayUs=0 },
	{ .regAddr= 0x344d, .regData= 0x00L, .delayUs=0 },

	{ .regAddr= 0x344e, .regData= 0x0FL, .delayUs=0 },
	{ .regAddr= 0x344f, .regData= 0x00L, .delayUs=0 },

	{ .regAddr= 0x3450, .regData= 0x2BL, .delayUs=0 },
	{ .regAddr= 0x3451, .regData= 0x00L, .delayUs=0 },

	{ .regAddr= 0x3452, .regData= 0x0BL, .delayUs=0 },
	{ .regAddr= 0x3453, .regData= 0x00L, .delayUs=0 },

	{ .regAddr= 0x3454, .regData= 0x0FL, .delayUs=0 },
	{ .regAddr= 0x3455, .regData= 0x00L, .delayUs=0 },
#endif
	{ .regAddr= 0x3472, .regData= 0x1cL, .delayUs=0 }, //X_OUT_SIZE
	{ .regAddr= 0x3473, .regData= 0x05L, .delayUs=0 }, // 1308

//	{ .regAddr= 0x347b, .regData= 0x23L, .delayUs=0 },
//	{ .regAddr= 0x3045, .regData= 0x01L, .delayUs=0 }, /* IDENTIFI ID */

	{ .regAddr= 0x3000, .regData= 0x00L, .delayUs=0 }, /* standby */
};
// {
//     { .regAddr = 0x3000, .regData = 0x01, .delayUs = 0x0,}, /* STANDBY */
//     { .regAddr = 0x3002, .regData = 0x01, .delayUs = 0x0,}, /* XTMSTA */
//
//     { .regAddr = 0x305c, .regData = 0x40, .delayUs = 0x0,}, // 0x28); // 0x20); // 0x18); // /*inck1*/
//     { .regAddr = 0x305d, .regData = 0x00, .delayUs = 0x0,}, // 0x03); // /*inck2*/
//     { .regAddr = 0x305e, .regData = 0x20, .delayUs = 0x0,}, /*inck3*/
//     { .regAddr = 0x305f, .regData = 0x01, .delayUs = 0x0,}, /*inck4 */ // INCKSEL4, 1080P, CSI-2, 37.125MHz;74.25MHz->0x01*/
//     { .regAddr = 0x315e, .regData = 0x1a, .delayUs = 0x0,}, /*inck5*/
//     { .regAddr = 0x3164, .regData = 0x1a, .delayUs = 0x0,}, /*inck6*/
//     { .regAddr = 0x3480, .regData = 0x49, .delayUs = 0x0,}, /*inck7*/ // 0x92 for 74.25mhz inck, .regData = 0x49 for 37.125MHz.
//
//
//     { .regAddr = 0x3011, .regData = 0x02, .delayUs = 0x0,},
//     { .regAddr = 0x309e, .regData = 0x4a, .delayUs = 0x0,},
//     { .regAddr = 0x309f, .regData = 0x4a, .delayUs = 0x0,},
//     { .regAddr = 0x30d2, .regData = 0x19, .delayUs = 0x0,},
//     { .regAddr = 0x30d7, .regData = 0x03, .delayUs = 0x0,},
//     { .regAddr = 0x313b, .regData = 0x61, .delayUs = 0x0,},
//
//
//     { .regAddr = 0x3005, .regData = 0x01, .delayUs = 0x0,}, /*0: 10Bit; 0x01(default): 12Bit*/
//     { .regAddr = 0x3007, .regData = 0x13, .delayUs = 0x0,}, /* WINMODE: 720P */  // Noted by wsh: [6:4]: default 0, 0=Full HD 1080p, 1=HD720P, 4=Window cropping from Full HD 1080P.
//
//     { .regAddr = 0x3009, .regData = 0x02, .delayUs = 0x0,}, // Noted by wsh: 2=30/25 frames/s, 1=60 f/s.
//     { .regAddr = 0x300a, .regData = 0xf0, .delayUs = 0x0,}, // Noted by wsh: 0x3c for 10-bit, .regData = 0xf0 for 12-bit.
//
//     { .regAddr = 0x3044, .regData = 0x01, .delayUs = 0x0,}, // Noted by wsh: [1:0], ODBIT, 0=10-bit, 1=12=bit. [7:4]: fixed to 0 for csi2.
//     { .regAddr = 0x3046, .regData = 0x01, .delayUs = 0x0,}, /*ODBIT*/  // Noted by wsh: for LVDS output setting. [1:0]: 0x01=12-bit, 00=10-bit.
//
//     { .regAddr = 0x3129, .regData = 0x00, .delayUs = 0x0,}, // 0x00); /*ADBIT1*/  // Noted by wsh: 0x1d=10-bit. 0x00 = 12-bit.
//     { .regAddr = 0x317c, .regData = 0x00, .delayUs = 0x0,}, // 0x00); /*ADBIT2*/  // Noted by wsh: 0x12=10-bit. 0x00 = 12-bit.
//     { .regAddr = 0x31ec, .regData = 0x0e, .delayUs = 0x0,}, // 0x0e); /*ADBIT3*/  // Noted by wsh: 0x37=10-bit. 0x0e = 12-bit.
//
//     { .regAddr = 0x3644, .regData = 0x20, .delayUs = 0x0,}, // Noted by wsh: Not specified in spec.
//     { .regAddr = 0x3645, .regData = 0x25, .delayUs = 0x0,},
//     { .regAddr = 0x3672, .regData = 0x1c, .delayUs = 0x0,}, // Copied from 0x3472.
//     { .regAddr = 0x3673, .regData = 0x05, .delayUs = 0x0,}, // Copied from 0x3473.
//     { .regAddr = 0x367b, .regData = 0x23, .delayUs = 0x0,},
//     { .regAddr = 0x3680, .regData = 0x49, .delayUs = 0x0,},
//
//     { .regAddr = 0x3405, .regData = 0x20, .delayUs = 0x0,}, // ? Noted by wsh: [5:4]: REPETITION, 2=222.75MHz, 1=445.5MHz, 0=891MHz.
//     { .regAddr = 0x3407, .regData = 0x03, .delayUs = 0x0,}, // Noted by wsh: PHYSICAL_LANE_NUM, 3=4ch, 1=2ch.
//     { .regAddr = 0x3414, .regData = 0x04, .delayUs = 0x0,}, // Noted by wsh: [5:0]: OPB_SIZE_V, default 0x0A. Spec set to 0x04.
//     { .regAddr = 0x3415, .regData = 0x00, .delayUs = 0x0,},
//
//     { .regAddr = 0x3441, .regData = 0x0C, .delayUs = 0x0,}, // Noted by wsh: CSI_DT_FMT, .regData = 0x0a0a=RAW10, .regData = 0x0c0c=RAW12.
//     { .regAddr = 0x3442, .regData = 0x0C, .delayUs = 0x0,},
//
//     { .regAddr = 0x3443, .regData = 0x03, .delayUs = 0x0,}, // Noted by wsh: CSI_LANE_MODE, 1=2 lane, 3=4 lane.
//
//     { .regAddr = 0x3444, .regData = 0x20, .delayUs = 0x0,}, // Noted by wsh: EXTCK_FREQ, 2520 for 37.125MHz INCK, 4A40 for 74.25MHz INCK.
//     { .regAddr = 0x3445, .regData = 0x25, .delayUs = 0x0,},
//
//     { .regAddr = 0x3446, .regData = 0x47, .delayUs = 0x0,}, // Noted by wsh: spec set to 0x47 for 4lane30p.
//     { .regAddr = 0x3447, .regData = 0x00, .delayUs = 0x0,},
//
//     { .regAddr = 0x3448, .regData = 0x17, .delayUs = 0x0,}, // Noted by wsh: spec set to 0x17 for 4lane30p.
//     { .regAddr = 0x3449, .regData = 0x00, .delayUs = 0x0,},
//
//     { .regAddr = 0x344a, .regData = 0x0f, .delayUs = 0x0,}, // Noted by wsh: spec set to 0x0f for 4lane30p.
//     { .regAddr = 0x344b, .regData = 0x00, .delayUs = 0x0,},
//
//     { .regAddr = 0x344c, .regData = 0x0f, .delayUs = 0x0,}, // Noted by wsh: spec set to 0x0f for 4lane30p.
//     { .regAddr = 0x344d, .regData = 0x00, .delayUs = 0x0,},
//
//     { .regAddr = 0x344e, .regData = 0x0f, .delayUs = 0x0,}, // Noted by wsh: spec set to 0x0f for 4lane30p.
//     { .regAddr = 0x344f, .regData = 0x00, .delayUs = 0x0,},
//
//     { .regAddr = 0x3450, .regData = 0x2b, .delayUs = 0x0,}, // Noted by wsh: spec set to 0x2b for 4lane30p.
//     { .regAddr = 0x3451, .regData = 0x00, .delayUs = 0x0,},
//
//     { .regAddr = 0x3452, .regData = 0x0b, .delayUs = 0x0,}, // Noted by wsh: spec set to 0x0b for 4lane30p.
//     { .regAddr = 0x3453, .regData = 0x00, .delayUs = 0x0,},
//
//     { .regAddr = 0x3454, .regData = 0x0f, .delayUs = 0x0,}, // Noted by wsh: spec set to 0x0f for 4lane30p.
//     { .regAddr = 0x3455, .regData = 0x00, .delayUs = 0x0,},
//
//     { .regAddr = 0x3018, .regData = 0xee, .delayUs = 0x0,}, /*Vmax*/
//     { .regAddr = 0x3019, .regData = 0x02, .delayUs = 0x0,}, /*Vmax*/
//
//     { .regAddr = 0x301c, .regData = 0xc8, .delayUs = 0x0,}, /*Hmax*/
//     { .regAddr = 0x301d, .regData = 0x19, .delayUs = 0x0,}, /*Hmax*/
//
//     { .regAddr = 0x3418, .regData = 0xd9, .delayUs = 0x0,}, /*Y_OUT_SIZE*/ // Noted by wsh: spec set to 0x2d9.
//     { .regAddr = 0x3419, .regData = 0x02, .delayUs = 0x0,},
//     { .regAddr = 0x3472, .regData = 0x1c, .delayUs = 0x0,}, // Noted by wsh: Horizontal effective pixel width, X_OUT_SIZE, .regData = 0x07A0 = 1952.
//     { .regAddr = 0x3473, .regData = 0x05, .delayUs = 0x0,}, // Default 0x079C(1948).
//
//     { .regAddr = 0x347b, .regData = 0x23, .delayUs = 0x0,},
//
//     { .regAddr = 0x3000, .regData = 0x00, .delayUs = 0x0,}, /* standby */
//
//     { .regAddr = 0x3018, .regData = 0x65, .delayUs = 0x0,}, /*Vmax*/ // 0x465 = 1125
//     { .regAddr = 0x3019, .regData = 0x04, .delayUs = 0x0,}, /*Vmax*/
//     //{ 0x301c, .regData = 0x00, .delayUs = 0x0,}, /*Hmax*/ // 0x0898 = 2200
//     //{ 0x301d, .regData = 0x0a, .delayUs = 0x0,}, /*Hmax*/ // 0x1130 = 4400
//     { .regAddr = 0x301c, .regData = 0x30, .delayUs = 0x0,}, /*Hmax*/
//     { .regAddr = 0x301d, .regData = 0x11, .delayUs = 0x0,}, /*Hmax*/
//     { .regAddr = 0x0000, .regData = 0x00, .delayUs = 0x0,},
// };

static drv_sensor_reg_info_t  imx327_normal_setting[] =
{
  { .regAddr = 0x3003, .regData = 0x01, .delayUs = 100000,},
  { .regAddr = 0x3003, .regData = 0x00, .delayUs = 100000,},

	{ .regAddr= 0x3000, .regData= 0x01L, .delayUs=0 }, /* enter standby */
	{ .regAddr= 0x3002, .regData= 0x01L, .delayUs=0 }, /* master mode */
#ifdef IMX327_2LANE
	{ .regAddr= 0x305c, .regData= 0x18L, .delayUs=0 }, // 0x18); /*inck1*/
	{ .regAddr= 0x305d, .regData= 0x03L, .delayUs=0 }, /*inck2*/
#else
	{ .regAddr= 0x305c, .regData= 0x20L, .delayUs=0 }, // 0x18); /*inck1*/   //1080p 37.125MHz why 0x20 ?
	{ .regAddr= 0x305d, .regData= 0x03L, .delayUs=0 }, /*inck2*/
#endif
	{ .regAddr= 0x305e, .regData= 0x20L, .delayUs=0 }, /*inck3*/
	{ .regAddr= 0x305f, .regData= 0x01L, .delayUs=0 }, /*inck4 */ // INCKSEL4, 1080P, CSI-2, 37.125MHz;74.25MHz->0x01*/
	{ .regAddr= 0x315e, .regData= 0x1aL, .delayUs=0 }, /*inck5*/
	{ .regAddr= 0x3164, .regData= 0x1aL, .delayUs=0 }, /*inck6*/
	{ .regAddr= 0x3480, .regData= 0x49L, .delayUs=0 }, /*inck7*/

	{ .regAddr= 0x3011, .regData= 0x02L, .delayUs=0 },
	{ .regAddr= 0x309e, .regData= 0x4aL, .delayUs=0 },
	{ .regAddr= 0x309f, .regData= 0x4aL, .delayUs=0 },
	{ .regAddr= 0x30d2, .regData= 0x19L, .delayUs=0 },
	{ .regAddr= 0x30d7, .regData= 0x03L, .delayUs=0 },
	{ .regAddr= 0x313b, .regData= 0x61L, .delayUs=0 },

	{ .regAddr= 0x3005, .regData= 0x01L, .delayUs=0 }, /* ADBIT: */ // 0=10Bit; 0x01(default)=12Bit
	{ .regAddr= 0x3129, .regData= 0x00L, .delayUs=0 }, // 0x00); /*ADBIT1*/  // Noted by wsh: 0x1d=10-bit. 0x00 = 12-bit.
	{ .regAddr= 0x317c, .regData= 0x00L, .delayUs=0 }, // 0x00); /*ADBIT2*/  // Noted by wsh: 0x12=10-bit. 0x00 = 12-bit.
	{ .regAddr= 0x31ec, .regData= 0x0eL, .delayUs=0 }, // 0x0e); /*ADBIT3*/  // Noted by wsh: 0x37=10-bit. 0x0e = 12-bit.

	{ .regAddr= 0x3007, .regData= 0x03L, .delayUs=0 }, /* WINMODE: 1080P */ // Noted by wsh: [6:4]: default 0, 0=Full HD 1080p, 1=HD720P, 4=Window cropping from Full HD 1080P.
	{ .regAddr= 0x3009, .regData= 0x02L, .delayUs=0 }, // Noted by wsh: 2=30/25 frames/s.
	{ .regAddr= 0x300a, .regData= 0xf0L, .delayUs=0 }, // Noted by wsh: 0x3c for 10-bit, 0xf0 for 12-bit.
#ifdef IMX327_2LANE
	{ .regAddr= 0x3018, .regData= 0x65L, .delayUs=0 },  // VMAX
	{ .regAddr= 0x3019, .regData= 0x04L, .delayUs=0 },
	{ .regAddr= 0x301c, .regData= 0x60L, .delayUs=0 }, /*Hmax*/ // 0x0898 = 2200
	{ .regAddr= 0x301d, .regData= 0x22L, .delayUs=0 }, /* 4400 * 2 */
#else
	{ .regAddr= 0x3018, .regData= 0x65, .delayUs=0 },  // VMAX
	{ .regAddr= 0x3019, .regData= 0x04, .delayUs=0 },
	{ .regAddr= 0x301c, .regData= 0x30, .delayUs=0 }, /*Hmax*/
	{ .regAddr= 0x301d, .regData= 0x11, .delayUs=0 },
#endif
	{ .regAddr= 0x300C, .regData= 0x00L, .delayUs=0 }, /* set normal mode */

	{ .regAddr= 0x3044, .regData= 0x01L, .delayUs=0 },
	{ .regAddr= 0x3046, .regData= 0x01L, .delayUs=0 },  /*ODBIT*/  // ? Noted by wsh: 0x01=12-bit, 00=10-bit. For LVDS output only.

#ifdef IMX327_2LANE
	{ .regAddr= 0x3405, .regData= 0x10L, .delayUs=0 }, // ? Noted by wsh: [5:4]: RECEPTION, 2=222.75MHz, 1=445.5MHz, 0=891MHz.
	{ .regAddr= 0x3407, .regData= 0x01L, .delayUs=0 }, // Noted by wsh: PHYSICAL_LANE_NUM, 3=4ch, 1=2ch.
#else
	{ .regAddr= 0x3405, .regData= 0x20L, .delayUs=0 }, // ? Noted by wsh: [5:4]: RECEPTION, 2=222.75MHz, 1=445.5MHz, 0=891MHz.
	{ .regAddr= 0x3407, .regData= 0x03L, .delayUs=0 }, // Noted by wsh: PHYSICAL_LANE_NUM, 3=4ch, 1=2ch.
#endif
	{ .regAddr= 0x3414, .regData= 0x0AL, .delayUs=0 }, // Noted by wsh: [5:0]: OPB_SIZE_V, default 0x0A.
	{ .regAddr= 0x3415, .regData= 0x01L, .delayUs=0 },
#ifdef IMX327_2LANE
	{ .regAddr= 0x3418, .regData= 0x38L, .delayUs=0 }, /*Y_OUT_SIZE*/
#else
	{ .regAddr= 0x3418, .regData= 0x49L, .delayUs=0 }, /*Y_OUT_SIZE*/
#endif
	{ .regAddr= 0x3419, .regData= 0x04L, .delayUs=0 }, // Noted by wsh: Default 0x0449/1097. 0x097a=2426, 0x0438=1080.

	{ .regAddr= 0x3441, .regData= 0x0CL, .delayUs=0 }, // Noted by wsh: CSI_DT_FMT, 0x0a0a=RAW10, 0x0c0c=RAW12.
	{ .regAddr= 0x3442, .regData= 0x0CL, .delayUs=0 },
#ifdef IMX327_2LANE
	{ .regAddr= 0x3443, .regData= 0x01L, .delayUs=0 }, // Noted by wsh: CSI_LANE_MODE, 1=2 lane, 3=4 lane.
#else
	{ .regAddr= 0x3443, .regData= 0x03L, .delayUs=0 }, // Noted by wsh: CSI_LANE_MODE, 1=2 lane, 3=4 lane.
#endif
	{ .regAddr= 0x3444, .regData= 0x20L, .delayUs=0 }, // Noted by wsh: EXTCK_FREQ, 2520 for 37.125MHz INCK, 4A40 for 74.25MHz INCK.
	{ .regAddr= 0x3445, .regData= 0x25L, .delayUs=0 },
#ifdef IMX327_2LANE
	{ .regAddr= 0x3446, .regData= 0x57L, .delayUs=0 },
	{ .regAddr= 0x3447, .regData= 0x00L, .delayUs=0 },

	{ .regAddr= 0x3448, .regData= 0x37L, .delayUs=0 },
	{ .regAddr= 0x3449, .regData= 0x00L, .delayUs=0 },

	{ .regAddr= 0x344a, .regData= 0x1fL, .delayUs=0 },
	{ .regAddr= 0x344b, .regData= 0x00L, .delayUs=0 },

	{ .regAddr= 0x344c, .regData= 0x1FL, .delayUs=0 },
	{ .regAddr= 0x344d, .regData= 0x00L, .delayUs=0 },

	{ .regAddr= 0x344e, .regData= 0x1fL, .delayUs=0 },
	{ .regAddr= 0x344f, .regData= 0x00L, .delayUs=0 },

	{ .regAddr= 0x3450, .regData= 0x77L, .delayUs=0 },
	{ .regAddr= 0x3451, .regData= 0x00L, .delayUs=0 },

	{ .regAddr= 0x3452, .regData= 0x1FL, .delayUs=0 },
	{ .regAddr= 0x3453, .regData= 0x00L, .delayUs=0 },

	{ .regAddr= 0x3454, .regData= 0x17L, .delayUs=0 },
	{ .regAddr= 0x3455, .regData= 0x00L, .delayUs=0 },

	{ .regAddr= 0x3472, .regData= 0x80L, .delayUs=0 }, // Noted by wsh: X_OUT_SIZE, 0x07A0 = 1952.

#else
	{ .regAddr= 0x3446, .regData= 0x47L, .delayUs=0 },
	{ .regAddr= 0x3447, .regData= 0x00L, .delayUs=0 },

	{ .regAddr= 0x3448, .regData= 0x1FL, .delayUs=0 },
	{ .regAddr= 0x3449, .regData= 0x00L, .delayUs=0 },

	{ .regAddr= 0x344a, .regData= 0x17L, .delayUs=0 },
	{ .regAddr= 0x344b, .regData= 0x00L, .delayUs=0 },

	{ .regAddr= 0x344c, .regData= 0x0FL, .delayUs=0 },
	{ .regAddr= 0x344d, .regData= 0x00L, .delayUs=0 },

	{ .regAddr= 0x344e, .regData= 0x17L, .delayUs=0 },
	{ .regAddr= 0x344f, .regData= 0x00L, .delayUs=0 },

	{ .regAddr= 0x3450, .regData= 0x47L, .delayUs=0 },
	{ .regAddr= 0x3451, .regData= 0x00L, .delayUs=0 },

	{ .regAddr= 0x3452, .regData= 0x0FL, .delayUs=0 },
	{ .regAddr= 0x3453, .regData= 0x00L, .delayUs=0 },

	{ .regAddr= 0x3454, .regData= 0x0FL, .delayUs=0 },
	{ .regAddr= 0x3455, .regData= 0x00L, .delayUs=0 },

	{ .regAddr= 0x3472, .regData= 0x9cL, .delayUs=0 }, // Noted by wsh: X_OUT_SIZE, 0x07A0 = 1952.
#endif
	{ .regAddr= 0x3473, .regData= 0x07L, .delayUs=0 }, // Default 0x079C(1948). 0x0780(1920).

	{ .regAddr= 0x347b, .regData= 0x23L, .delayUs=0 },
	{ .regAddr= 0x3045, .regData= 0x01L, .delayUs=0 }, /* IDENTIFI ID */

	{ .regAddr= 0x3000, .regData= 0x00L, .delayUs=0 }, /* standby */
};
// {
//     { .regAddr = 0x3000, .regData = 0x01, .delayUs = 0x0,}, /* STANDBY */
//     { .regAddr = 0x3002, .regData = 0x01, .delayUs = 0x0,}, /* XTMSTA */
//     { .regAddr = 0x305c, .regData = 0x20, .delayUs = 0x0,}, // 0x18); /*inck1*/
//     { .regAddr = 0x305d, .regData = 0x03, .delayUs = 0x0,}, /*inck2*/
//     { .regAddr = 0x305e, .regData = 0x20, .delayUs = 0x0,}, /*inck3*/
//     { .regAddr = 0x305f, .regData = 0x01, .delayUs = 0x0,}, /*inck4 */ // INCKSEL4, 1080P, CSI-2, 37.125MHz;74.25MHz->0x01*/
//     { .regAddr = 0x315e, .regData = 0x1a, .delayUs = 0x0,}, /*inck5*/
//     { .regAddr = 0x3164, .regData = 0x1a, .delayUs = 0x0,}, /*inck6*/
//     { .regAddr = 0x3480, .regData = 0x49, .delayUs = 0x0,}, /*inck7*/
//     { .regAddr = 0x3011, .regData = 0x02, .delayUs = 0x0,},
//     { .regAddr = 0x309e, .regData = 0x4a, .delayUs = 0x0,},
//     { .regAddr = 0x309f, .regData = 0x4a, .delayUs = 0x0,},
//     { .regAddr = 0x30d2, .regData = 0x19, .delayUs = 0x0,},
//     { .regAddr = 0x30d7, .regData = 0x03, .delayUs = 0x0,},
//     { .regAddr = 0x313b, .regData = 0x61, .delayUs = 0x0,},
//     { .regAddr = 0x3005, .regData = 0x01, .delayUs = 0x0,}, /* ADBIT: */ // 0=10Bit; 0x01(default)=12Bit
//     { .regAddr = 0x3129, .regData = 0x00, .delayUs = 0x0,}, // 0x00); /*ADBIT1*/  // Noted by wsh: 0x1d=10-bit. 0x00 = 12-bit.
//     { .regAddr = 0x317c, .regData = 0x00, .delayUs = 0x0,}, // 0x00); /*ADBIT2*/  // Noted by wsh: 0x12=10-bit. 0x00 = 12-bit.
//     { .regAddr = 0x31ec, .regData = 0x0e, .delayUs = 0x0,}, // 0x0e); /*ADBIT3*/  // Noted by wsh: 0x37=10-bit. 0x0e = 12-bit.
//     { .regAddr = 0x3007, .regData = 0x03, .delayUs = 0x0,}, /* WINMODE: 1080P */ // Noted by wsh: [6:4]: default 0, 0=Full HD 1080p, 1=HD720P, 4=Window cropping from Full HD 1080P.
//     { .regAddr = 0x3009, .regData = 0x02, .delayUs = 0x0,}, // Noted by wsh: 2=30/25 frames/s.
//     { .regAddr = 0x300a, .regData = 0xf0, .delayUs = 0x0,}, // Noted by wsh: 0x3c for 10-bit, 0xf0 for 12-bit.
//     { .regAddr = 0x3018, .regData = 0x65, .delayUs = 0x0,},  // VMAX
//     { .regAddr = 0x3019, .regData = 0x04, .delayUs = 0x0,},
//     { .regAddr = 0x3018, .regData = 0x2F, .delayUs = 0x0,},  // VMAX
//     { .regAddr = 0x3019, .regData = 0x0D, .delayUs = 0x0,},
//     { .regAddr = 0x301c, .regData = 0x30, .delayUs = 0x0,}, /*Hmax*/ // 0x0898 = 2200
//     { .regAddr = 0x301d, .regData = 0x11, .delayUs = 0x0,}, /*Hmax*/ // 0x1130 = 4400
//     { .regAddr = 0x300C, .regData = 0x00, .delayUs = 0x0,}, /* set normal mode */
//     { .regAddr = 0x3044, .regData = 0x01, .delayUs = 0x0,},
//     { .regAddr = 0x3046, .regData = 0x01, .delayUs = 0x0,},  /*ODBIT*/  // ? Noted by wsh: 0x01=12-bit, 00=10-bit. For LVDS output only.
//     { .regAddr = 0x3405, .regData = 0x20, .delayUs = 0x0,}, // ? Noted by wsh: [5:4]: RECEPTION, 2=222.75MHz, 1=445.5MHz, 0=891MHz.
//     { .regAddr = 0x3407, .regData = 0x03, .delayUs = 0x0,}, // Noted by wsh: PHYSICAL_LANE_NUM, 3=4ch, 1=2ch.
//     { .regAddr = 0x3414, .regData = 0x0A, .delayUs = 0x0,}, // Noted by wsh: [5:0]: OPB_SIZE_V, default 0x0A.
//     { .regAddr = 0x3415, .regData = 0x01, .delayUs = 0x0,},
//     { .regAddr = 0x3418, .regData = 0x49, .delayUs = 0x0,}, /*Y_OUT_SIZE*/
//     { .regAddr = 0x3419, .regData = 0x04, .delayUs = 0x0,}, // Noted by wsh: Default 0x0449/1097. 0x097a=2426, 0x0438=1080.
//     { .regAddr = 0x3441, .regData = 0x0C, .delayUs = 0x0,}, // Noted by wsh: CSI_DT_FMT, 0x0a0a=RAW10, 0x0c0c=RAW12.
//     { .regAddr = 0x3442, .regData = 0x0C, .delayUs = 0x0,},
//     { .regAddr = 0x3443, .regData = 0x03, .delayUs = 0x0,}, // Noted by wsh: CSI_LANE_MODE, 1=2 lane, 3=4 lane.
//     { .regAddr = 0x3444, .regData = 0x20, .delayUs = 0x0,}, // Noted by wsh: EXTCK_FREQ, 2520 for 37.125MHz INCK, 4A40 for 74.25MHz INCK.
//     { .regAddr = 0x3445, .regData = 0x25, .delayUs = 0x0,},
//     { .regAddr = 0x3446, .regData = 0x47, .delayUs = 0x0,},
//     { .regAddr = 0x3447, .regData = 0x00, .delayUs = 0x0,},
//     { .regAddr = 0x3448, .regData = 0x1F, .delayUs = 0x0,},
//     { .regAddr = 0x3449, .regData = 0x00, .delayUs = 0x0,},
//     { .regAddr = 0x344a, .regData = 0x17, .delayUs = 0x0,},
//     { .regAddr = 0x344b, .regData = 0x00, .delayUs = 0x0,},
//     { .regAddr = 0x344c, .regData = 0x0F, .delayUs = 0x0,},
//     { .regAddr = 0x344d, .regData = 0x00, .delayUs = 0x0,},
//     { .regAddr = 0x344e, .regData = 0x17, .delayUs = 0x0,},
//     { .regAddr = 0x344f, .regData = 0x00, .delayUs = 0x0,},
//     { .regAddr = 0x3450, .regData = 0x47, .delayUs = 0x0,},
//     { .regAddr = 0x3451, .regData = 0x00, .delayUs = 0x0,},
//     { .regAddr = 0x3452, .regData = 0x0F, .delayUs = 0x0,},
//     { .regAddr = 0x3453, .regData = 0x00, .delayUs = 0x0,},
//     { .regAddr = 0x3454, .regData = 0x0F, .delayUs = 0x0,},
//     { .regAddr = 0x3455, .regData = 0x00, .delayUs = 0x0,},
//     { .regAddr = 0x3472, .regData = 0x9c, .delayUs = 0x0,}, // Noted by wsh: X_OUT_SIZE, 0x07A0 = 1952.
//     { .regAddr = 0x3473, .regData = 0x07, .delayUs = 0x0,}, // Default 0x079C(1948). 0x0780(1920).
//     { .regAddr = 0x347b, .regData = 0x23, .delayUs = 0x0,},
//     { .regAddr = 0x3045, .regData = 0x01, .delayUs = 0x0,}, /* IDENTIFI ID */
//     { .regAddr = 0x3000, .regData = 0x00, .delayUs = 0x0,}, /* standby */
// };

static drv_sensor_reg_info_t  imx327_dol2_setting[] =
{
    { .regAddr = 0x3002, .regData = 0x01, .delayUs = 0x0},
    { .regAddr = 0x3005, .regData = 0x01, .delayUs = 0x0},
    { .regAddr = 0x3007, .regData = 0x03, .delayUs = 0x0},
    { .regAddr = 0x3009, .regData = 0x02, .delayUs = 0x0},
    { .regAddr = 0x300A, .regData = 0xF0, .delayUs = 0x0},
    { .regAddr = 0x300C, .regData = 0x11, .delayUs = 0x0},
    { .regAddr = 0x3011, .regData = 0x02, .delayUs = 0x0},
    { .regAddr = 0x3018, .regData = 0x65, .delayUs = 0x0},
    { .regAddr = 0x3019, .regData = 0x04, .delayUs = 0x0},
    { .regAddr = 0x301C, .regData = 0x30, .delayUs = 0x0},
    { .regAddr = 0x301D, .regData = 0x11, .delayUs = 0x0},
    { .regAddr = 0x3020, .regData = 0x02, .delayUs = 0x0},
    { .regAddr = 0x3021, .regData = 0x00, .delayUs = 0x0},
    { .regAddr = 0x3024, .regData = 0x49, .delayUs = 0x0},
    { .regAddr = 0x3025, .regData = 0x08, .delayUs = 0x0},
    { .regAddr = 0x3030, .regData = 0xfB, .delayUs = 0x0},
    { .regAddr = 0x3031, .regData = 0x01, .delayUs = 0x0},
    { .regAddr = 0x3045, .regData = 0x05, .delayUs = 0x0},
    { .regAddr = 0x3046, .regData = 0x01, .delayUs = 0x0},
    //{ .regAddr = 0x304B, .regData = 0x0A, .delayUs = 0x0},
    { .regAddr = 0x305C, .regData = 0x20, .delayUs = 0x0},
    { .regAddr = 0x305D, .regData = 0x03, .delayUs = 0x0},
    { .regAddr = 0x305E, .regData = 0x20, .delayUs = 0x0},
    { .regAddr = 0x305F, .regData = 0x01, .delayUs = 0x0},
    { .regAddr = 0x309E, .regData = 0x4A, .delayUs = 0x0},
    { .regAddr = 0x309F, .regData = 0x4A, .delayUs = 0x0},
    { .regAddr = 0x30D2, .regData = 0x19, .delayUs = 0x0},
    { .regAddr = 0x30D7, .regData = 0x03, .delayUs = 0x0},
    { .regAddr = 0x3106, .regData = 0x11, .delayUs = 0x0},
    { .regAddr = 0x3129, .regData = 0x00, .delayUs = 0x0},
    { .regAddr = 0x313B, .regData = 0x61, .delayUs = 0x0},
    { .regAddr = 0x315E, .regData = 0x1A, .delayUs = 0x0},
    { .regAddr = 0x3164, .regData = 0x1A, .delayUs = 0x0},
    { .regAddr = 0x317C, .regData = 0x00, .delayUs = 0x0},
    { .regAddr = 0x31EC, .regData = 0x0E, .delayUs = 0x0},
    { .regAddr = 0x3405, .regData = 0x20, .delayUs = 0x0},
    { .regAddr = 0x3407, .regData = 0x03, .delayUs = 0x0},
    { .regAddr = 0x3414, .regData = 0x0A, .delayUs = 0x0},
    { .regAddr = 0x3415, .regData = 0x00, .delayUs = 0x0},
    { .regAddr = 0x3418, .regData = 0xB4, .delayUs = 0x0},
    { .regAddr = 0x3419, .regData = 0x08, .delayUs = 0x0},
    { .regAddr = 0x3441, .regData = 0x0C, .delayUs = 0x0},
    { .regAddr = 0x3442, .regData = 0x0C, .delayUs = 0x0},
    { .regAddr = 0x3443, .regData = 0x03, .delayUs = 0x0},
    { .regAddr = 0x3444, .regData = 0x20, .delayUs = 0x0},
    { .regAddr = 0x3445, .regData = 0x25, .delayUs = 0x0},
    { .regAddr = 0x3446, .regData = 0x47, .delayUs = 0x0},
    { .regAddr = 0x3447, .regData = 0x00, .delayUs = 0x0},
    { .regAddr = 0x3448, .regData = 0x1F, .delayUs = 0x0},
    { .regAddr = 0x3449, .regData = 0x00, .delayUs = 0x0},
    { .regAddr = 0x344A, .regData = 0x17, .delayUs = 0x0},
    { .regAddr = 0x344B, .regData = 0x00, .delayUs = 0x0},
    { .regAddr = 0x344C, .regData = 0x0F, .delayUs = 0x0},
    { .regAddr = 0x344D, .regData = 0x00, .delayUs = 0x0},
    { .regAddr = 0x344E, .regData = 0x17, .delayUs = 0x0},
    { .regAddr = 0x344F, .regData = 0x00, .delayUs = 0x0},
    { .regAddr = 0x3450, .regData = 0x47, .delayUs = 0x0},
    { .regAddr = 0x3451, .regData = 0x00, .delayUs = 0x0},
    { .regAddr = 0x3452, .regData = 0x0F, .delayUs = 0x0},
    { .regAddr = 0x3453, .regData = 0x00, .delayUs = 0x0},
    { .regAddr = 0x3454, .regData = 0x0F, .delayUs = 0x0},
    { .regAddr = 0x3455, .regData = 0x00, .delayUs = 0x0},
    { .regAddr = 0x3472, .regData = 0xA0, .delayUs = 0x0},
    { .regAddr = 0x3473, .regData = 0x07, .delayUs = 0x0},
    { .regAddr = 0x347B, .regData = 0x23, .delayUs = 0x0},
    { .regAddr = 0x3480, .regData = 0x49, .delayUs = 0x0},
    { .regAddr = 0x3418, .regData = 0xa8, .delayUs = 0x0}, /*Y_OUT_SIZE*/
    { .regAddr = 0x3419, .regData = 0x0c, .delayUs = 0x0},
    { .regAddr = 0x301c, .regData = 0xc0, .delayUs = 0x0}, /*H MAX*/
    { .regAddr = 0x301d, .regData = 0x0d, .delayUs = 0x0},
    { .regAddr = 0x3018, .regData = 0x7e, .delayUs = 0x0}, /*V MAX*/
    { .regAddr = 0x3019, .regData = 0x05, .delayUs = 0x0},
    { .regAddr = 0x3000, .regData = 0x00, .delayUs = 0x0}, /* standby */
    { .regAddr = 0x0000, .regData = 0x00, .delayUs = 0x0}

};

static drv_sensor_reg_info_t  imx327_dol2_720_setting[] =
{
    { .regAddr = 0x3000, .regData = 0x01, .delayUs = 0x0,}, /* STANDBY */
    { .regAddr = 0x3002, .regData = 0x01, .delayUs = 0x0,}, /* XTMSTA */
    { .regAddr = 0x305c, .regData = 0x20, .delayUs = 0x0,}, // 0x18); /*inck1*/
    { .regAddr = 0x305d, .regData = 0x03, .delayUs = 0x0,}, /*inck2*/
    { .regAddr = 0x305e, .regData = 0x20, .delayUs = 0x0,}, /*inck3*/
    { .regAddr = 0x305f, .regData = 0x01, .delayUs = 0x0,}, /*inck4 */ // INCKSEL4, 1080P, CSI-2, 37.125MHz;74.25MHz->0x01*/
    { .regAddr = 0x315e, .regData = 0x1a, .delayUs = 0x0,}, /*inck5*/
    { .regAddr = 0x3164, .regData = 0x1a, .delayUs = 0x0,}, /*inck6*/
    { .regAddr = 0x3480, .regData = 0x49, .delayUs = 0x0,}, /*inck7*/

    { .regAddr = 0x3011, .regData = 0x02, .delayUs = 0x0,},
    { .regAddr = 0x309e, .regData = 0x4a, .delayUs = 0x0,},
    { .regAddr = 0x309f, .regData = 0x4a, .delayUs = 0x0,},
    { .regAddr = 0x30d2, .regData = 0x19, .delayUs = 0x0,},
    { .regAddr = 0x30d7, .regData = 0x03, .delayUs = 0x0,},
    { .regAddr = 0x313b, .regData = 0x61, .delayUs = 0x0,},

    { .regAddr = 0x3005, .regData = 0x01, .delayUs = 0x0,}, /* ADBIT: */ // 0=10Bit; 0x01(default)=12Bit
    { .regAddr = 0x3129, .regData = 0x00, .delayUs = 0x0,}, // 0x00); /*ADBIT1*/  // Noted by wsh: 0x1d=10-bit. 0x00 = 12-bit.
    { .regAddr = 0x317c, .regData = 0x00, .delayUs = 0x0,}, // 0x00); /*ADBIT2*/  // Noted by wsh: 0x12=10-bit. 0x00 = 12-bit.
    { .regAddr = 0x31ec, .regData = 0x0e, .delayUs = 0x0,}, // 0x0e); /*ADBIT3*/  // Noted by wsh: 0x37=10-bit. 0x0e = 12-bit.

    { .regAddr = 0x3007, .regData = 0x03, .delayUs = 0x0,}, /* WINMODE: 1080P */ // Noted by wsh: [6:4]: default 0, 0=Full HD 1080p, 1=HD720P, 4=Window cropping from Full HD 1080P.
    { .regAddr = 0x3009, .regData = 0x02, .delayUs = 0x0,}, // Noted by wsh: 2=30/25 frames/s.
    { .regAddr = 0x300a, .regData = 0xf0, .delayUs = 0x0,}, // Noted by wsh: 0x3c for 10-bit, 0xf0 for 12-bit.

    { .regAddr = 0x3018, .regData = 0x65, .delayUs = 0x0,},  // VMAX
    { .regAddr = 0x3019, .regData = 0x04, .delayUs = 0x0,},

    { .regAddr = 0x301c, .regData = 0x30, .delayUs = 0x0,}, /*Hmax*/ // 0x0898 = 2200
    { .regAddr = 0x301d, .regData = 0x11, .delayUs = 0x0,}, /*Hmax*/ // 0x1130 = 4400
    { .regAddr = 0x300C, .regData = 0x00, .delayUs = 0x0,}, /* set normal mode */
    { .regAddr = 0x3044, .regData = 0x01, .delayUs = 0x0,},
    { .regAddr = 0x3046, .regData = 0x01, .delayUs = 0x0,},  /*ODBIT*/  // ? Noted by wsh: 0x01=12-bit, 00=10-bit. For LVDS output only.
    { .regAddr = 0x3405, .regData = 0x20, .delayUs = 0x0,}, // ? Noted by wsh: [5:4]: RECEPTION, 2=222.75MHz, 1=445.5MHz, 0=891MHz.
    { .regAddr = 0x3407, .regData = 0x03, .delayUs = 0x0,}, // Noted by wsh: PHYSICAL_LANE_NUM, 3=4ch, 1=2ch.
    { .regAddr = 0x3414, .regData = 0x0A, .delayUs = 0x0,}, // Noted by wsh: [5:0]: OPB_SIZE_V, default 0x0A.
    { .regAddr = 0x3415, .regData = 0x01, .delayUs = 0x0,},
    { .regAddr = 0x3418, .regData = 0x49, .delayUs = 0x0,}, /*Y_OUT_SIZE*/
    { .regAddr = 0x3419, .regData = 0x04, .delayUs = 0x0,}, // Noted by wsh: Default 0x0449/1097. 0x097a=2426, 0x0438=1080.
    { .regAddr = 0x3441, .regData = 0x0C, .delayUs = 0x0,}, // Noted by wsh: CSI_DT_FMT, 0x0a0a=RAW10, 0x0c0c=RAW12.
    { .regAddr = 0x3442, .regData = 0x0C, .delayUs = 0x0,},
    { .regAddr = 0x3443, .regData = 0x03, .delayUs = 0x0,}, // Noted by wsh: CSI_LANE_MODE, 1=2 lane, 3=4 lane.
    { .regAddr = 0x3444, .regData = 0x20, .delayUs = 0x0,}, // Noted by wsh: EXTCK_FREQ, 2520 for 37.125MHz INCK, 4A40 for 74.25MHz INCK.
    { .regAddr = 0x3445, .regData = 0x25, .delayUs = 0x0,},
    { .regAddr = 0x3446, .regData = 0x47, .delayUs = 0x0,},
    { .regAddr = 0x3447, .regData = 0x00, .delayUs = 0x0,},
    { .regAddr = 0x3448, .regData = 0x1F, .delayUs = 0x0,},
    { .regAddr = 0x3449, .regData = 0x00, .delayUs = 0x0,},
    { .regAddr = 0x344a, .regData = 0x17, .delayUs = 0x0,},
    { .regAddr = 0x344b, .regData = 0x00, .delayUs = 0x0,},
    { .regAddr = 0x344c, .regData = 0x0F, .delayUs = 0x0,},
    { .regAddr = 0x344d, .regData = 0x00, .delayUs = 0x0,},
    { .regAddr = 0x344e, .regData = 0x17, .delayUs = 0x0,},
    { .regAddr = 0x344f, .regData = 0x00, .delayUs = 0x0,},
    { .regAddr = 0x3450, .regData = 0x47, .delayUs = 0x0,},
    { .regAddr = 0x3451, .regData = 0x00, .delayUs = 0x0,},
    { .regAddr = 0x3452, .regData = 0x0F, .delayUs = 0x0,},
    { .regAddr = 0x3453, .regData = 0x00, .delayUs = 0x0,},
    { .regAddr = 0x3454, .regData = 0x0F, .delayUs = 0x0,},
    { .regAddr = 0x3455, .regData = 0x00, .delayUs = 0x0,},
    { .regAddr = 0x3472, .regData = 0x9c, .delayUs = 0x0,}, // Noted by wsh: X_OUT_SIZE, 0x07A0 = 1952.
    { .regAddr = 0x3473, .regData = 0x07, .delayUs = 0x0,}, // Default 0x079C(1948). 0x0780(1920).
    { .regAddr = 0x347b, .regData = 0x23, .delayUs = 0x0,},
    { .regAddr = 0x3045, .regData = 0x01, .delayUs = 0x0,}, /* IDENTIFI ID */
    { .regAddr = 0x3000, .regData = 0x00, .delayUs = 0x0,}, /* standby */

    { .regAddr = 0x3002, .regData = 0x00, .delayUs = 0x0}, /* master mode start */

    { .regAddr = 0x304b, .regData = 0x0a, .delayUs = 0x0}, /* XVSOUTSEL XHSOUTSEL */
    { .regAddr = 0x300C, .regData = 0x11, .delayUs = 0x0}, /* DOL3 */
    { .regAddr = 0x3106, .regData = 0x15, .delayUs = 0x0}, //33
    { .regAddr = 0x3415, .regData = 0x0C, .delayUs = 0x0},

    { .regAddr = 0x3472, .regData = 0x20, .delayUs = 0x0},
    { .regAddr = 0x3473, .regData = 0x05, .delayUs = 0x0},
    { .regAddr = 0x301C, .regData = 0xE4, .delayUs = 0x0},
    { .regAddr = 0x301D, .regData = 0x0C, .delayUs = 0x0},
    { .regAddr = 0x3045, .regData = 0x05, .delayUs = 0x0}, /* IDENTIFI ID */
    ///////////////without following, short exp very short///////////////////////////////////////
    { .regAddr = 0x3018, .regData = 0xEE, .delayUs = 0x0},
    { .regAddr = 0x3019, .regData = 0x04, .delayUs = 0x0},
    { .regAddr = 0x3418, .regData = 0x70, .delayUs = 0x0}, /*Y_OUT_SIZE*/
    { .regAddr = 0x3419, .regData = 0x08, .delayUs = 0x0},

};

static drv_sensor_reg_info_t  imx327_dol3_setting[] =
{
    { .regAddr = 0x3002, .regData = 0x00, .delayUs = 0x0},
    { .regAddr = 0x3005, .regData = 0x01, .delayUs = 0x0},
    { .regAddr = 0x3007, .regData = 0x03, .delayUs = 0x0},
    { .regAddr = 0x3009, .regData = 0x02, .delayUs = 0x0},
    { .regAddr = 0x300A, .regData = 0xF0, .delayUs = 0x0},
    { .regAddr = 0x300C, .regData = 0x21, .delayUs = 0x0},
    { .regAddr = 0x3011, .regData = 0x02, .delayUs = 0x0},
    { .regAddr = 0x3018, .regData = 0x65, .delayUs = 0x0},
    { .regAddr = 0x3019, .regData = 0x04, .delayUs = 0x0},
    { .regAddr = 0x301C, .regData = 0x30, .delayUs = 0x0},
    { .regAddr = 0x301D, .regData = 0x11, .delayUs = 0x0},
    { .regAddr = 0x3020, .regData = 0x05, .delayUs = 0x0},
    { .regAddr = 0x3021, .regData = 0x00, .delayUs = 0x0},
    { .regAddr = 0x3024, .regData = 0x0B, .delayUs = 0x0},
    { .regAddr = 0x3025, .regData = 0x01, .delayUs = 0x0},
    { .regAddr = 0x3028, .regData = 0x93, .delayUs = 0x0},
    { .regAddr = 0x3029, .regData = 0x01, .delayUs = 0x0},
    { .regAddr = 0x3030, .regData = 0x06, .delayUs = 0x0},
    { .regAddr = 0x3031, .regData = 0x01, .delayUs = 0x0},
    { .regAddr = 0x3034, .regData = 0x1C, .delayUs = 0x0},
    { .regAddr = 0x3035, .regData = 0x01, .delayUs = 0x0},
    { .regAddr = 0x3045, .regData = 0x05, .delayUs = 0x0},
    { .regAddr = 0x3046, .regData = 0x01, .delayUs = 0x0},
    { .regAddr = 0x304B, .regData = 0x0A, .delayUs = 0x0},
    { .regAddr = 0x305C, .regData = 0x20, .delayUs = 0x0},
    { .regAddr = 0x305D, .regData = 0x03, .delayUs = 0x0},
    { .regAddr = 0x305E, .regData = 0x20, .delayUs = 0x0},
    { .regAddr = 0x305F, .regData = 0x01, .delayUs = 0x0},
    { .regAddr = 0x309E, .regData = 0x4A, .delayUs = 0x0},
    { .regAddr = 0x309F, .regData = 0x4A, .delayUs = 0x0},
    { .regAddr = 0x30D2, .regData = 0x19, .delayUs = 0x0},
    { .regAddr = 0x30D7, .regData = 0x03, .delayUs = 0x0},
    { .regAddr = 0x3106, .regData = 0x33, .delayUs = 0x0},
    { .regAddr = 0x3129, .regData = 0x00, .delayUs = 0x0},
    { .regAddr = 0x313B, .regData = 0x61, .delayUs = 0x0},
    { .regAddr = 0x315E, .regData = 0x1A, .delayUs = 0x0},
    { .regAddr = 0x3164, .regData = 0x1A, .delayUs = 0x0},
    { .regAddr = 0x317C, .regData = 0x00, .delayUs = 0x0},
    { .regAddr = 0x31EC, .regData = 0x0E, .delayUs = 0x0},
    { .regAddr = 0x3405, .regData = 0x20, .delayUs = 0x0},
    { .regAddr = 0x3407, .regData = 0x03, .delayUs = 0x0},
    { .regAddr = 0x3414, .regData = 0x0A, .delayUs = 0x0},
    { .regAddr = 0x3415, .regData = 0x00, .delayUs = 0x0},
    { .regAddr = 0x3418, .regData = 0x19, .delayUs = 0x0},
    { .regAddr = 0x3419, .regData = 0x0E, .delayUs = 0x0},
    { .regAddr = 0x3441, .regData = 0x0C, .delayUs = 0x0},
    { .regAddr = 0x3442, .regData = 0x0C, .delayUs = 0x0},
    { .regAddr = 0x3443, .regData = 0x03, .delayUs = 0x0},
    { .regAddr = 0x3444, .regData = 0x20, .delayUs = 0x0},
    { .regAddr = 0x3445, .regData = 0x25, .delayUs = 0x0},
    { .regAddr = 0x3446, .regData = 0x47, .delayUs = 0x0},
    { .regAddr = 0x3447, .regData = 0x00, .delayUs = 0x0},
    { .regAddr = 0x3448, .regData = 0x1F, .delayUs = 0x0},
    { .regAddr = 0x3449, .regData = 0x00, .delayUs = 0x0},
    { .regAddr = 0x344A, .regData = 0x17, .delayUs = 0x0},
    { .regAddr = 0x344B, .regData = 0x00, .delayUs = 0x0},
    { .regAddr = 0x344C, .regData = 0x0F, .delayUs = 0x0},
    { .regAddr = 0x344D, .regData = 0x00, .delayUs = 0x0},
    { .regAddr = 0x344E, .regData = 0x17, .delayUs = 0x0},
    { .regAddr = 0x344F, .regData = 0x00, .delayUs = 0x0},
    { .regAddr = 0x3450, .regData = 0x47, .delayUs = 0x0},
    { .regAddr = 0x3451, .regData = 0x00, .delayUs = 0x0},
    { .regAddr = 0x3452, .regData = 0x0F, .delayUs = 0x0},
    { .regAddr = 0x3453, .regData = 0x00, .delayUs = 0x0},
    { .regAddr = 0x3454, .regData = 0x0F, .delayUs = 0x0},
    { .regAddr = 0x3455, .regData = 0x00, .delayUs = 0x0},
    { .regAddr = 0x3472, .regData = 0xA0, .delayUs = 0x0},
    { .regAddr = 0x3473, .regData = 0x07, .delayUs = 0x0},
    { .regAddr = 0x347B, .regData = 0x23, .delayUs = 0x0},
    { .regAddr = 0x3480, .regData = 0x49, .delayUs = 0x0},
    { .regAddr = 0x3418, .regData = 0x55, .delayUs = 0x0},
    { .regAddr = 0x3419, .regData = 0x11, .delayUs = 0x0},
    { .regAddr = 0x3000, .regData = 0x00, .delayUs = 20000},
};

/*
{ 0x3002, 0x00L, 0x0, 1 },
{ 0x304b, 0x0aL, 0x0, 1 },
//stop sequence - address is 0x0000
{ 0x0000, 0x0000, 0x0000, 0x0000 }

*/
static drv_sensor_reg_info_t  imx327_streamOn_setting[] =
{
    { .regAddr = 0x3002, .regData = 0x00, .delayUs = 0x0,},
    { .regAddr = 0x304b, .regData = 0x0a, .delayUs = 0x0,},
    { .regAddr = 0x0000, .regData = 0x00, .delayUs = 20000,},
};

static drv_sensor_reg_info_t  imx327_streamOff_setting[] =
{
    { .regAddr = 0x3002, .regData = 0x01, .delayUs = 0x0,},
    { .regAddr = 0x3003, .regData = 0x01, .delayUs = 100000,},
};

static drv_sensor_exp_param_t imx327_normal_720_exparams =
{
    .maxIntegrationTime = 750   ,
    .minIntegrationTime = 1     ,
    .maxAnalogGain      = 33532 ,
    .minAnalogGain      = 1024  ,
    .lineTime           = 182044,
    .limitShortMid      = 0     ,
    .limitLong          = 0     ,
    .expLineMargin      = 1     ,
};

static drv_sensor_exp_param_t imx327_normal_exparams =
{
    .maxIntegrationTime = 2248  ,
    .minIntegrationTime = 1     ,
    .maxAnalogGain      = 33532,
    .minAnalogGain      = 1024  ,
    .lineTime           = 121360,
    .limitShortMid      = 0     ,
    .limitLong          = 0     ,
    .expLineMargin      = 1     ,
};

static drv_sensor_exp_param_t imx327_dol2_exparams =
{
    .maxIntegrationTime = 500   ,
    .minIntegrationTime = 1     ,
    .maxAnalogGain      = 33532,
    .minAnalogGain      = 1024  ,
    .lineTime           = 97108 ,
    .limitShortMid      = 1000  ,
    .limitLong          = 3696  ,
    .expLineMargin      = 20    ,
};

static drv_sensor_exp_param_t imx327_dol3_exparams =
{
    .maxIntegrationTime = 300   ,
    .minIntegrationTime = 1     ,
    .maxAnalogGain      = 33532,
    .minAnalogGain      = 1024  ,
    .lineTime           = 97108 ,
    .limitShortMid      = 1100  ,
    .limitLong          = 3000  ,
    .expLineMargin      = 1    ,
};


static drv_sensor_exp_param_t imx327_normal_frame_stitch3_exparams =
{
    .maxIntegrationTime = 200   ,
    .minIntegrationTime = 1     ,
    .maxAnalogGain      = 33532,
    .minAnalogGain      = 1024  ,
    .lineTime           = 121360,
    .limitShortMid      = 1000  ,
    .limitLong          = 1349  ,
    .expLineMargin      = 1    ,
};

static drv_sensor_exp_param_t imx327_normal_frame_stitch2_exparams =
{
    .maxIntegrationTime = 500   ,
    .minIntegrationTime = 1     ,
    .maxAnalogGain      = 33532,
    .minAnalogGain      = 1024  ,
    .lineTime           = 121360,
    .limitShortMid      = 2249  ,
    .limitLong          = 2249  ,
    .expLineMargin      = 1    ,
};

static drv_power_setting_t imx327_power_up_setting[] =
{
    {.type = DRV_POWER_REG, .regAddr = 0x83c40004, .bitMask  = 0x01, .bitVal = 0x00, .delayUs = 100000}, /// 100ms
    {.type = DRV_POWER_REG, .regAddr = 0x83c40004, .bitMask  = 0x01, .bitVal = 0x01, .delayUs = 100000}  /// APB2 SENSOR_CTRL_REG 0x83c40004
};


static drv_power_setting_t imx327_power_down_setting[] =
{
    {.type = DRV_POWER_REG, .regAddr = 0x83c40004, .bitMask  = 0x01, .bitVal = 0x00, .delayUs = 100000},
    {.type = DRV_POWER_REG, .regAddr = 0x83c40004, .bitMask  = 0x01, .bitVal = 0x01, .delayUs = 100000}
};

static sensor_drv_params_t imx327_sensor_params =
{
    .sensorId     = 0xe,
    .sensorIdAddr = 0x31dc,
    .name         = "imx327",
    .slaveInfo =
    {
        .hwType      = DRV_HW_I2C,
        .slaveAddr   = (0x34 >> 1),
        .channel     = 0,
        .regDataType = DRV_ADDR_TYPE_BYTE,
        .regAddrType = DRV_DATA_TYPE_WORD,
    },
    .powerUpSettings  =
    {
        .powerSetting = imx327_power_up_setting,
        .size = sizeof(imx327_power_up_setting) / sizeof(imx327_power_up_setting[0]),
    },
    .powerDownSettings =
    {
        .powerSetting  = imx327_power_down_setting,
        .size = sizeof(imx327_power_down_setting) / sizeof(imx327_power_down_setting[0]),
    },
    .activesize =
    {
        .width  = 1920,
        .height = 1080
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
        .minFps = 15,
        .frameLength = 0x465,
        .expLineMargin = 1,
        .dim =
        {
            .width  = 1920,
            .height = 1080
        },
        .hdrMode   = SENSOR_NORMAL_MODE,
        .regConfig =
        {
            .regDataType = DRV_DATA_TYPE_BYTE,
            .regAddrType = DRV_ADDR_TYPE_WORD,
            .direction   = DRV_DIRECTION_WRITE,
            .settings    = imx327_normal_setting,
            .settingSize = sizeof(imx327_normal_setting) / sizeof (imx327_normal_setting[0]),

        },
        .expParams   = &imx327_normal_exparams,

        .intf = {
          .ln_cfg = {
            .dat_num = 2,
          },
          .link_freq = 446.0f,
        },
    },
    .modeSettings[1] =
    {
        .maxFps = 15,
        .minFps = 10,
        .frameLength = 0x57e,
        .expLineMargin = 20,
        .dim =
        {
            .width  = 1920,
            .height = 1080
        },
        .hdrMode   = SENSOR_DOL2_MODE,
        .regConfig =
        {
            .regDataType = DRV_DATA_TYPE_BYTE,
            .regAddrType = DRV_ADDR_TYPE_WORD,
            .direction   = DRV_DIRECTION_WRITE,
            .settings    = imx327_dol2_setting,
            .settingSize = sizeof(imx327_dol2_setting) / sizeof (imx327_dol2_setting[0]),
        },
        .expParams   = &imx327_dol2_exparams,

        .intf = {
          .ln_cfg = {
            .dat_num = 2,
          },
          .link_freq = 446.0f,
        },
    },
    .modeSettings[2] =
    {
        .maxFps = 15,
        .minFps = 15,
        .frameLength = 0x465,
        .expLineMargin = 1,
        .dim =
        {
            .width  = 1920,
            .height = 1080
        },
        .hdrMode   = SENSOR_DOL3_MODE,
        .regConfig =
        {
            .regDataType = DRV_DATA_TYPE_BYTE,
            .regAddrType = DRV_ADDR_TYPE_WORD,
            .direction   = DRV_DIRECTION_WRITE,
            .settings    = imx327_dol3_setting,
            .settingSize = sizeof(imx327_dol3_setting) / sizeof (imx327_dol3_setting[0]),
        },
        .expParams   = &imx327_dol3_exparams,

        .intf = {
          .ln_cfg = {
            .dat_num = 2,
          },
          .link_freq = 446.0f,
        },
    },
    .modeSettings[3] =
    {
        .maxFps = 30,
        .minFps = 30,
        .frameLength = 0x465,
        .expLineMargin = 1,
        .dim =
        {
            .width  = 1920,
            .height = 1080
        },
        .hdrMode   = SENSOR_STITCH_3_MODE,
        .regConfig =
        {
            .regDataType = DRV_DATA_TYPE_BYTE,
            .regAddrType = DRV_ADDR_TYPE_WORD,
            .direction   = DRV_DIRECTION_WRITE,
            .settings    = imx327_normal_setting,
            .settingSize = sizeof(imx327_normal_setting) / sizeof (imx327_normal_setting[0]),
        },
        .expParams   = &imx327_normal_frame_stitch3_exparams,

        .intf = {
          .ln_cfg = {
            .dat_num = 2,
          },
          .link_freq = 446.0f,
        },
    },
    .modeSettings[4] =
    {
        .maxFps = 15,
        .minFps = 15,
        .frameLength = 0x8ca,
        .expLineMargin = 1,
        .dim =
        {
            .width  = 1920,
            .height = 1080
        },
        .hdrMode   = SENSOR_STITCH_2_MODE,
        .regConfig =
        {
            .regDataType = DRV_DATA_TYPE_BYTE,
            .regAddrType = DRV_ADDR_TYPE_WORD,
            .direction   = DRV_DIRECTION_WRITE,
            .settings    = imx327_normal_setting,
            .settingSize = sizeof(imx327_normal_setting) / sizeof (imx327_normal_setting[0]),
        },
        .expParams   = &imx327_normal_frame_stitch2_exparams,

        .intf = {
          .ln_cfg = {
            .dat_num = 2,
          },
          .link_freq = 446.0f,
        },
    },

    .modeSettings[5] =
    {
        .maxFps = 30,
        .minFps = 30,
        .frameLength = 0x2ee, //0x465,
        .expLineMargin = 1,
        .dim =
        {
            .width  = 1280,
            .height = 720
        },
        .hdrMode   = SENSOR_NORMAL_MODE,
        .regConfig =
        {
            .regDataType = DRV_DATA_TYPE_BYTE,
            .regAddrType = DRV_ADDR_TYPE_WORD,
            .direction   = DRV_DIRECTION_WRITE,
            .settings    = imx327_normal_720P_setting,
            .settingSize = sizeof(imx327_normal_720P_setting) / sizeof (imx327_normal_720P_setting[0]),
        },
        .expParams   = &imx327_normal_720_exparams, //&imx327_normal_exparams,

        .intf = {
          .ln_cfg = {
            .dat_num = 2,
          },
          .link_freq = 446.0f,
        },
    },

    .modeSettings[6] =
    {
        .maxFps = 15,
        .minFps = 15,
        .frameLength = 0x4EE,
        .expLineMargin = 1,
        .dim =
        {
            .width  = 1280,
            .height = 720
        },
        .hdrMode   = SENSOR_DOL2_MODE,
        .regConfig =
        {
            .regDataType = DRV_DATA_TYPE_BYTE,
            .regAddrType = DRV_ADDR_TYPE_WORD,
            .direction   = DRV_DIRECTION_WRITE,
            .settings    = imx327_dol2_720_setting,
            .settingSize = sizeof(imx327_dol2_720_setting) / sizeof (imx327_dol2_720_setting[0]),
        },
        .expParams   = &imx327_dol2_exparams,

        .intf = {
          .ln_cfg = {
            .dat_num = 2,
          },
          .link_freq = 446.0f,
        },
    },

    .streamOnSetting =
    {
        .regDataType = DRV_DATA_TYPE_BYTE,
        .regAddrType = DRV_ADDR_TYPE_WORD,
        .direction   = DRV_DIRECTION_WRITE,
        .settings    = imx327_streamOn_setting,
        .settingSize = sizeof(imx327_streamOn_setting) / sizeof (imx327_streamOn_setting[0]),
    },

    .streamOffSetting =
    {
        .regDataType = DRV_DATA_TYPE_BYTE,
        .regAddrType = DRV_ADDR_TYPE_WORD,
        .direction   = DRV_DIRECTION_WRITE,
        .settings    = imx327_streamOff_setting,
        .settingSize = sizeof(imx327_streamOff_setting) / sizeof (imx327_streamOff_setting[0]),
    },

    .fill_exposure_reg = IMX327_Fill_Exposure_Reg,
};


sensor_drv_params_t* Load_Imx327_Sensor_Driver(void);

#endif

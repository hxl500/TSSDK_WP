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

#ifndef __CV2003_AUX1_SENSOR_H__
#define __CV2003_AUX1_SENSOR_H__

#include "sensor_common.h"
#include "log_utils.h"

isp_ret_t CV2003_Fill_Exposure_Reg(sensor_apply_exp_param_t * pExpParam, drv_regs_array_info_t * pRegInfo);

//CV2003
//window size:1920x1080
//mclk=24mhz,
/*LANE 1:lock, LANE2:lock, LANE3:unlock, LANE4:unlockClock Lane Non-continuous:0
Ck lane: feg:236718khz,rate:473436kbps=462.3Mbps
Restarted:0
CRC errors:0, Ec errors:0, Eec corrected.0
Packets recv:3432264
Packets per fame: 1082
Long packet info: data ID:28,size:2400
virtual channel o used
Lp00 Min time(set:0
Lp00 Min time(actual).0
Lane1 sync time:0
Lane2 sync time:0
Lane3 sync time:0
Lane4 sync time:0
DataID:00 found
*/

static drv_sensor_reg_info_t  cv2003_2mp_setting[] =
{
	{ .regAddr=0x3000, .regData= 0x01, .delayUs=0 },
	{ .regAddr=0x306D, .regData= 0x00, .delayUs=0 },
	{ .regAddr=0x3070, .regData= 0x00, .delayUs=0 },
	{ .regAddr=0x3071, .regData= 0x00, .delayUs=0 },
	{ .regAddr=0x3074, .regData= 0x00, .delayUs=0 },
	{ .regAddr=0x3075, .regData= 0x00, .delayUs=0 },
	{ .regAddr=0x3078, .regData= 0x00, .delayUs=0 },
	{ .regAddr=0x3079, .regData= 0x00, .delayUs=0 },
	{ .regAddr=0x307A, .regData= 0x02, .delayUs=0 },
	{ .regAddr=0x307B, .regData= 0x00, .delayUs=0 },
	{ .regAddr=0x324C, .regData= 0x00, .delayUs=0 },
	{ .regAddr=0x324D, .regData= 0x04, .delayUs=0 },
	{ .regAddr=0x324E, .regData= 0x00, .delayUs=0 },
	{ .regAddr=0x324F, .regData= 0x04, .delayUs=0 },
	{ .regAddr=0x3250, .regData= 0x00, .delayUs=0 },
	{ .regAddr=0x3251, .regData= 0x04, .delayUs=0 },
	{ .regAddr=0x3252, .regData= 0x00, .delayUs=0 },
	{ .regAddr=0x3253, .regData= 0x04, .delayUs=0 },
	{ .regAddr=0x342F, .regData= 0x03, .delayUs=0 },
	{ .regAddr=0x3430, .regData= 0x00, .delayUs=0 },
	{ .regAddr=0x3431, .regData= 0x01, .delayUs=0 },
	{ .regAddr=0x3432, .regData= 0x04, .delayUs=0 },
	{ .regAddr=0x3433, .regData= 0x05, .delayUs=0 },
	{ .regAddr=0x3440, .regData= 0x01, .delayUs=0 }, //HMAX 0x44c=29.62963us
	{ .regAddr=0x3441, .regData= 0x00, .delayUs=0 },
	{ .regAddr=0x3442, .regData= 0x00, .delayUs=0 },
	{ .regAddr=0x3443, .regData= 0x00, .delayUs=0 },
	{ .regAddr=0x3300, .regData= 0x01, .delayUs=0 },
	{ .regAddr=0x3401, .regData= 0x01, .delayUs=0 },
	{ .regAddr=0x3440, .regData= 0x03, .delayUs=0 },
	{ .regAddr=0x3442, .regData= 0x00, .delayUs=0 },
	{ .regAddr=0x3806, .regData= 0x01, .delayUs=0 },
	{ .regAddr=0x3158, .regData= 0x01, .delayUs=0 },
	{ .regAddr=0x3159, .regData= 0x01, .delayUs=0 },
	{ .regAddr=0x315A, .regData= 0x01, .delayUs=0 },
	{ .regAddr=0x315B, .regData= 0x01, .delayUs=0 },
	{ .regAddr=0x35B3, .regData= 0x15, .delayUs=0 },
	{ .regAddr=0x3148, .regData= 0x64, .delayUs=0 },
	{ .regAddr=0x3031, .regData= 0x00, .delayUs=0 },
	{ .regAddr=0x3118, .regData= 0x01, .delayUs=0 },
	{ .regAddr=0x3119, .regData= 0x06, .delayUs=0 },
	{ .regAddr=0x3670, .regData= 0x00, .delayUs=0 },
	{ .regAddr=0x3679, .regData= 0x02, .delayUs=0 },
	{ .regAddr=0x3330, .regData= 0x00, .delayUs=0 },
	{ .regAddr=0x320e, .regData= 0x02, .delayUs=0 },
	{ .regAddr=0x3804, .regData= 0x10, .delayUs=0 },
	{ .regAddr=0x35a1, .regData= 0x06, .delayUs=0 },
	{ .regAddr=0x35a8, .regData= 0x06, .delayUs=0 },
	{ .regAddr=0x35a9, .regData= 0x06, .delayUs=0 },
	{ .regAddr=0x35aa, .regData= 0x06, .delayUs=0 },
	{ .regAddr=0x35ab, .regData= 0x06, .delayUs=0 },
	{ .regAddr=0x35ac, .regData= 0x06, .delayUs=0 },
	{ .regAddr=0x35ad, .regData= 0x06, .delayUs=0 },
	{ .regAddr=0x35ae, .regData= 0x07, .delayUs=0 },
	{ .regAddr=0x35af, .regData= 0x07, .delayUs=0 },
	{ .regAddr=0x333B, .regData= 0x01, .delayUs=0 },
	{ .regAddr=0x3338, .regData= 0x1E, .delayUs=0 },
	{ .regAddr=0x3339, .regData= 0x00, .delayUs=0 },
	{ .regAddr=0x3141, .regData= 0x01, .delayUs=0 },
	{ .regAddr=0x3030, .regData= 0x01, .delayUs=0 },
	{ .regAddr=0x3020, .regData= 0xC4, .delayUs=0 },   //C4
	{ .regAddr=0x3021, .regData= 0x09, .delayUs=0 },//09
	{ .regAddr=0x3024, .regData= 0xF0, .delayUs=0 },
	{ .regAddr=0x3025, .regData= 0x04, .delayUs=0 }, //X add?
	{ .regAddr=0x3038, .regData= 0x00, .delayUs=0 }, //X add?
	{ .regAddr=0x3039, .regData= 0x00, .delayUs=0 }, //X add?
	{ .regAddr=0x303A, .regData= 0x80, .delayUs=0 },    //89
	{ .regAddr=0x303B, .regData= 0x07, .delayUs=0 },
	{ .regAddr=0x3034, .regData= 0x04, .delayUs=0 },
	{ .regAddr=0x3035, .regData= 0x00, .delayUs=0 },
	{ .regAddr=0x3036, .regData= 0x38, .delayUs=0 },
	{ .regAddr=0x3037, .regData= 0x04, .delayUs=0 },
	{ .regAddr=0x3908, .regData= 0x4f, .delayUs=0 },
	{ .regAddr=0x390A, .regData= 0x04, .delayUs=0 },
	//{ .regAddr=0x3000, .regData= 0x00, .delayUs=0 },

};

//CV2003
//window size:1920x1080
//mclk=24mhz,
/*LANE 1:lock, LANE2:lock, LANE3:unlock, LANE4:unlockClock Lane Non-continuous:0
Ck lane: feg:378124khz,rate:756248kbps=738.52Mbps
Restarted:0
CRC errors:0, Ec errors:0, Eec corrected.0
Packets recv:4157981
Packets per fame: 1082
Long packet info: data ID:28,size:2400
virtual channel o used
Lp00 Min time(set:0
Lp00 Min time(actual).0
Lane1 sync time:0
Lane2 sync time:0
Lane3 sync time:0
Lane4 sync time:0
DataID:00 found
*/
static drv_sensor_reg_info_t  cv2003_2mp_15fps_setting[] =
{
	{ .regAddr=0x3000, .regData=  0x01, .delayUs=0 },
	{ .regAddr=0x3020, .regData=  0x60, .delayUs=0 },
	{ .regAddr=0x3021, .regData=  0x09, .delayUs=0 },
	{ .regAddr=0x3022, .regData=  0x00, .delayUs=0 },
	{ .regAddr=0x3024, .regData=  0x70, .delayUs=0 },
	{ .regAddr=0x3025, .regData=  0x02, .delayUs=0 },
	{ .regAddr=0x3029, .regData=  0x00, .delayUs=0 },
	{ .regAddr=0x302a, .regData=  0x00, .delayUs=0 },
	{ .regAddr=0x3300, .regData=  0x01, .delayUs=0 },
	{ .regAddr=0x3400, .regData=  0x11, .delayUs=0 },
	{ .regAddr=0x3401, .regData=  0x01, .delayUs=0 },
	{ .regAddr=0x3422, .regData=  0xBF, .delayUs=0 },
	{ .regAddr=0x3460, .regData=  0x03, .delayUs=0 },
	{ .regAddr=0x3440, .regData=  0x03, .delayUs=0 },
	{ .regAddr=0x3442, .regData=  0x00, .delayUs=0 },
	{ .regAddr=0x3806, .regData=  0x01, .delayUs=0 },
	{ .regAddr=0x3908, .regData=  0x4b, .delayUs=0 },
	{ .regAddr=0x3909, .regData=  0x00, .delayUs=0 },
	{ .regAddr=0x3158, .regData=  0x01, .delayUs=0 },
	{ .regAddr=0x3159, .regData=  0x01, .delayUs=0 },
	{ .regAddr=0x315a, .regData=  0x01, .delayUs=0 },
	{ .regAddr=0x315b, .regData=  0x01, .delayUs=0 },
	{ .regAddr=0x3148, .regData=  0x64, .delayUs=0 },
	{ .regAddr=0x3670, .regData=  0x00, .delayUs=0 },
	{ .regAddr=0x3679, .regData=  0x02, .delayUs=0 },
	{ .regAddr=0x35b3, .regData=  0x15, .delayUs=0 },
	{ .regAddr=0x320e, .regData=  0x02, .delayUs=0 },
	{ .regAddr=0x3804, .regData=  0x15, .delayUs=0 },
	{ .regAddr=0x35a1, .regData=  0x06, .delayUs=0 },
	{ .regAddr=0x35a8, .regData=  0x06, .delayUs=0 },
	{ .regAddr=0x35a9, .regData=  0x06, .delayUs=0 },
	{ .regAddr=0x35aa, .regData=  0x06, .delayUs=0 },
	{ .regAddr=0x35ab, .regData=  0x06, .delayUs=0 },
	{ .regAddr=0x35ac, .regData=  0x06, .delayUs=0 },
	{ .regAddr=0x35ad, .regData=  0x06, .delayUs=0 },
	{ .regAddr=0x35ae, .regData=  0x07, .delayUs=0 },
	{ .regAddr=0x35af, .regData=  0x07, .delayUs=0 },
	{ .regAddr=0x333b, .regData=  0x01, .delayUs=0 },
	{ .regAddr=0x3339, .regData=  0x00, .delayUs=0 },
	{ .regAddr=0x3031, .regData=  0x00, .delayUs=0 },
	{ .regAddr=0x3118, .regData=  0x01, .delayUs=0 },
	{ .regAddr=0x3119, .regData=  0x06, .delayUs=0 },
	{ .regAddr=0x3330, .regData=  0x00, .delayUs=0 },
	{ .regAddr=0x3030, .regData=  0x01, .delayUs=0 },
//	{ .regAddr=0x3020, .regData=  0x2C, .delayUs=0 },
//	{ .regAddr=0x3021, .regData=  0x17, .delayUs=0 },
//  { .regAddr=0x3020, .regData=  0x09, .delayUs=0 },
//  { .regAddr=0x3021, .regData=  0x1B, .delayUs=0 },
	{ .regAddr=0x3020, .regData=  0xD6, .delayUs=0 },
	{ .regAddr=0x3021, .regData=  0x1E, .delayUs=0 },
	{ .regAddr=0x3024, .regData=  0xD1, .delayUs=0 },
	{ .regAddr=0x3025, .regData=  0x03, .delayUs=0 },
	/*{ .regAddr=0x3038, .regData=  0x04, .delayUs=0 },
	{ .regAddr=0x3039, .regData=  0x00, .delayUs=0 },
	{ .regAddr=0x303a, .regData=  0x80, .delayUs=0 },
	{ .regAddr=0x303b, .regData=  0x07, .delayUs=0 },
	{ .regAddr=0x3034, .regData=  0x04, .delayUs=0 },
	{ .regAddr=0x3035, .regData=  0x00, .delayUs=0 },
	{ .regAddr=0x3036, .regData=  0x38, .delayUs=0 },
	{ .regAddr=0x3037, .regData=  0x04, .delayUs=0 },*/
	{ .regAddr=0x301C, .regData= 0x00, .delayUs=0 },
	{ .regAddr=0x3038, .regData= 0x00, .delayUs=0 }, //X add?
	{ .regAddr=0x3039, .regData= 0x00, .delayUs=0 }, //X add?
	{ .regAddr=0x303A, .regData= 0x88, .delayUs=0 },	//89
	{ .regAddr=0x303B, .regData= 0x07, .delayUs=0 },
	{ .regAddr=0x3034, .regData= 0x00, .delayUs=0 },
	{ .regAddr=0x3035, .regData= 0x00, .delayUs=0 },
	{ .regAddr=0x3036, .regData= 0x40, .delayUs=0 },
	{ .regAddr=0x3037, .regData= 0x04, .delayUs=0 },
	{ .regAddr=0x3908, .regData= 0x5A, .delayUs=0 },
	{ .regAddr=0x390a, .regData= 0x02, .delayUs=0 },
	// { .regAddr=0x3000, .regData=  0x00, .delayUs=0 },
};

static drv_sensor_reg_info_t  cv2003_streamOn_setting[] =
{
	{ .regAddr=0x3000, .regData= 0x00, .delayUs=0 },
};

static drv_sensor_reg_info_t  cv2003_streamOff_setting[] =
{
	{ .regAddr=0x3000, .regData= 0x01, .delayUs=50000 }, //50ms
};

static drv_sensor_exp_param_t cv2003_30fps_exparams =
{
    .maxIntegrationTime = 5536 - 6,
    .minIntegrationTime = 2,
    .maxAnalogGain      = 65536,
    .minAnalogGain      = 1024,
    .lineTime           = 54613,
    .limitShortMid      = 0,
    .limitLong          = 0,
    .expLineMargin      = 6,
};
static drv_sensor_exp_param_t cv2003_15fps_exparams =
{
    .maxIntegrationTime = 7888,//5536,//framth=2n，5000=2500*2-12
    .minIntegrationTime = 2, //Lation Modify
    .maxAnalogGain      = 133444, //266300, //65536,//133300,
    .minAnalogGain      = 1024,
    .lineTime           = 37062,
    .limitShortMid      = 0,
    .limitLong          = 0,
    .expLineMargin      = 6,
};
static drv_sensor_exp_param_t cv2003_12fps_exparams =
{
    .maxIntegrationTime = 7888,//12
    .minIntegrationTime = 2,
    .maxAnalogGain      = 133444, //266300, //65536,//133300,
    .minAnalogGain      = 1024,
    .lineTime           = 37062,
    .limitShortMid      = 0,
    .limitLong          = 0,
    .expLineMargin      = 6,
};
static drv_sensor_exp_param_t cv2003_5fps_exparams =
{
    .maxIntegrationTime = 16608 - 6,//framth=2n，5000=2500*2-12
    .minIntegrationTime = 2, //Lation Modify
    .maxAnalogGain      = 133444, //266300, //65536,//133300,
    .minAnalogGain      = 1024,
    .lineTime           = 49325,
    .limitShortMid      = 0,
    .limitLong          = 0,
    .expLineMargin      = 6,
};
static drv_power_setting_t cv2003_power_up_setting[] =
{
    {.type = DRV_POWER_REG, .regAddr = 0x83c40004, .bitMask  = 0x01, .bitVal = 0x00, .delayUs = 0}, /// 100ms
    {.type = DRV_POWER_REG, .regAddr = 0x83c40004, .bitMask  = 0x01, .bitVal = 0x01, .delayUs = 0}  /// APB2 SENSOR_CTRL_REG 0x83c40004
};

static drv_power_setting_t cv2003_power_down_setting[] =
{
    {.type = DRV_POWER_REG, .regAddr = 0x83c40004, .bitMask  = 0x01, .bitVal = 0x00, .delayUs = 0},
    {.type = DRV_POWER_REG, .regAddr = 0x83c40004, .bitMask  = 0x01, .bitVal = 0x01, .delayUs = 0}
};

static sensor_drv_params_t cv2003_aux1_sensor_params =
{
    .sensorId     = 0x03,
    .sensorIdAddr = 0x3138,
    .name         = "cv2003",
    .slaveInfo =
    {
        .hwType      = DRV_HW_I2C,
        .slaveAddr   = (0x6A>>1), //0x6C >> 1
        .channel     = 1,
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
        .width  = 1928,
        .height = 1088
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
		.maxFps = 14, //12,
		.minFps = 14, //12,
		.frameLength = 0x1ED6, //0x1B09, //0x172C,//   15fps:0x15a0   5536   14fps:5932
		.expLineMargin = 6,
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
			.settings    = cv2003_2mp_15fps_setting,
			.settingSize = sizeof(cv2003_2mp_15fps_setting) / sizeof (cv2003_2mp_15fps_setting[0]),
		},
		.expParams   = &cv2003_15fps_exparams, //&cv2003_12fps_exparams,

		.intf = {
			.ln_cfg = {
				.dat_num = 2,
			},
			.link_freq = 738.52f,//462.3f,//594.0f
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

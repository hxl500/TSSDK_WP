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

#ifndef __SC2337P_SENSOR_H__
#define __SC2337P_SENSOR_H__

#include "sensor_common.h"
#include "log_utils.h"

isp_ret_t SC2337P_Fill_Exposure_Reg(sensor_apply_exp_param_t * pExpParam, drv_regs_array_info_t * pRegInfo);

//SC2337P All-pixel scan CSI-2_2lane 24Mhz AD:10bit Output:10bit 396Mbps Master Mode 15.00fps Integration Time: Xx.xxms Gain:XdB vts = 1125 row time=22.22us
static drv_sensor_reg_info_t  sc2337p_1080p_setting[] =
{
   // softid { .regAddr=0x0103, .regData=0x01, .delayUs=0 },
   // softid { .regAddr=0x0103, .regData=0x00, .delayUs=0 },
    { .regAddr=0x0100, .regData=0x00, .delayUs=0 },
    { .regAddr=0x36e9, .regData=0x80, .delayUs=0 },
    { .regAddr=0x37f9, .regData=0x80, .delayUs=0 },
    { .regAddr=0x301f, .regData=0x09, .delayUs=0 },
    { .regAddr=0x3106, .regData=0x05, .delayUs=0 },
    { .regAddr=0x320e, .regData=0x07, .delayUs=0 },
    { .regAddr=0x320f, .regData=0x08, .delayUs=0 },
	{ .regAddr=0x3211, .regData=0x03, .delayUs=0 }, //not in the init seq
	{ .regAddr=0x3213, .regData=0x03, .delayUs=0 }, //not in the init seq
	{ .regAddr=0x3221, .regData=0xff, .delayUs=0 }, //not in the init seq
    { .regAddr=0x3248, .regData=0x04, .delayUs=0 },
    { .regAddr=0x3249, .regData=0x0b, .delayUs=0 },
    { .regAddr=0x3253, .regData=0x08, .delayUs=0 },
    { .regAddr=0x3301, .regData=0x09, .delayUs=0 },
    { .regAddr=0x3302, .regData=0xff, .delayUs=0 },
    { .regAddr=0x3303, .regData=0x10, .delayUs=0 },
    { .regAddr=0x3306, .regData=0x80, .delayUs=0 },
    { .regAddr=0x3307, .regData=0x02, .delayUs=0 },
    { .regAddr=0x3309, .regData=0xc8, .delayUs=0 },
    { .regAddr=0x330a, .regData=0x01, .delayUs=0 },
    { .regAddr=0x330b, .regData=0x30, .delayUs=0 },
    { .regAddr=0x330c, .regData=0x16, .delayUs=0 },
    { .regAddr=0x330d, .regData=0xff, .delayUs=0 },
    { .regAddr=0x3318, .regData=0x02, .delayUs=0 },
    { .regAddr=0x331f, .regData=0xb9, .delayUs=0 },
    { .regAddr=0x3321, .regData=0x0a, .delayUs=0 },
    { .regAddr=0x3327, .regData=0x0e, .delayUs=0 },
    { .regAddr=0x332b, .regData=0x12, .delayUs=0 },
    { .regAddr=0x3333, .regData=0x10, .delayUs=0 },
    { .regAddr=0x3334, .regData=0x40, .delayUs=0 },
    { .regAddr=0x335e, .regData=0x06, .delayUs=0 },
    { .regAddr=0x335f, .regData=0x0a, .delayUs=0 },
    { .regAddr=0x3364, .regData=0x1f, .delayUs=0 },
    { .regAddr=0x337c, .regData=0x02, .delayUs=0 },
    { .regAddr=0x337d, .regData=0x0e, .delayUs=0 },
    { .regAddr=0x3390, .regData=0x09, .delayUs=0 },
    { .regAddr=0x3391, .regData=0x0f, .delayUs=0 },
    { .regAddr=0x3392, .regData=0x1f, .delayUs=0 },
    { .regAddr=0x3393, .regData=0x20, .delayUs=0 },
    { .regAddr=0x3394, .regData=0x20, .delayUs=0 },
    { .regAddr=0x3395, .regData=0xe0, .delayUs=0 },
    { .regAddr=0x33a2, .regData=0x04, .delayUs=0 },
    { .regAddr=0x33b1, .regData=0x80, .delayUs=0 },
    { .regAddr=0x33b2, .regData=0x68, .delayUs=0 },
    { .regAddr=0x33b3, .regData=0x42, .delayUs=0 },
    { .regAddr=0x33f9, .regData=0x90, .delayUs=0 },
    { .regAddr=0x33fb, .regData=0xd0, .delayUs=0 },
    { .regAddr=0x33fc, .regData=0x0f, .delayUs=0 },
    { .regAddr=0x33fd, .regData=0x1f, .delayUs=0 },
    { .regAddr=0x349f, .regData=0x03, .delayUs=0 },
    { .regAddr=0x34a6, .regData=0x0f, .delayUs=0 },
    { .regAddr=0x34a7, .regData=0x1f, .delayUs=0 },
    { .regAddr=0x34a8, .regData=0x42, .delayUs=0 },
    { .regAddr=0x34a9, .regData=0x18, .delayUs=0 },
    { .regAddr=0x34aa, .regData=0x01, .delayUs=0 },
    { .regAddr=0x34ab, .regData=0x43, .delayUs=0 },
    { .regAddr=0x34ac, .regData=0x01, .delayUs=0 },
    { .regAddr=0x34ad, .regData=0x80, .delayUs=0 },
    { .regAddr=0x3630, .regData=0xf4, .delayUs=0 },
    { .regAddr=0x3632, .regData=0x44, .delayUs=0 },
    { .regAddr=0x3633, .regData=0x22, .delayUs=0 },
    { .regAddr=0x3639, .regData=0xf4, .delayUs=0 },
    { .regAddr=0x363c, .regData=0x47, .delayUs=0 },
    { .regAddr=0x3650, .regData=0x33, .delayUs=0 }, //not in the init seq
    { .regAddr=0x3651, .regData=0x7f, .delayUs=0 }, //not in the init seq
	{ .regAddr=0x3021, .regData=0x86, .delayUs=0 },
    { .regAddr=0x3670, .regData=0x09, .delayUs=0 },
    { .regAddr=0x3674, .regData=0xf4, .delayUs=0 },
    { .regAddr=0x3675, .regData=0xfb, .delayUs=0 },
    { .regAddr=0x3676, .regData=0xed, .delayUs=0 },
    { .regAddr=0x367c, .regData=0x09, .delayUs=0 },
    { .regAddr=0x367d, .regData=0x0f, .delayUs=0 },
    { .regAddr=0x3690, .regData=0x22, .delayUs=0 },
    { .regAddr=0x3691, .regData=0x22, .delayUs=0 },
    { .regAddr=0x3692, .regData=0x22, .delayUs=0 },
    { .regAddr=0x3698, .regData=0x89, .delayUs=0 },
    { .regAddr=0x3699, .regData=0x96, .delayUs=0 },
    { .regAddr=0x369a, .regData=0xd0, .delayUs=0 },
    { .regAddr=0x369b, .regData=0xd0, .delayUs=0 },
    { .regAddr=0x369c, .regData=0x09, .delayUs=0 },
    { .regAddr=0x369d, .regData=0x0f, .delayUs=0 },
    { .regAddr=0x36a2, .regData=0x09, .delayUs=0 },
    { .regAddr=0x36a3, .regData=0x0f, .delayUs=0 },
    { .regAddr=0x36a4, .regData=0x1f, .delayUs=0 },
    { .regAddr=0x36d0, .regData=0x01, .delayUs=0 },
    { .regAddr=0x36ea, .regData=0x0b, .delayUs=0 },
    { .regAddr=0x36eb, .regData=0x0c, .delayUs=0 },
    { .regAddr=0x36ec, .regData=0x1c, .delayUs=0 },
    { .regAddr=0x36ed, .regData=0x18, .delayUs=0 },
    { .regAddr=0x3722, .regData=0xc1, .delayUs=0 },
    { .regAddr=0x3724, .regData=0x41, .delayUs=0 },
    { .regAddr=0x3725, .regData=0xc1, .delayUs=0 },
    { .regAddr=0x3728, .regData=0x20, .delayUs=0 },
    { .regAddr=0x37fa, .regData=0xcb, .delayUs=0 },
    { .regAddr=0x37fb, .regData=0x32, .delayUs=0 },
    { .regAddr=0x37fc, .regData=0x11, .delayUs=0 },
    { .regAddr=0x37fd, .regData=0x07, .delayUs=0 },
    { .regAddr=0x3900, .regData=0x0d, .delayUs=0 },
    { .regAddr=0x3905, .regData=0x98, .delayUs=0 },
    { .regAddr=0x3919, .regData=0x04, .delayUs=0 },
    { .regAddr=0x391b, .regData=0x81, .delayUs=0 },
    { .regAddr=0x391c, .regData=0x10, .delayUs=0 },
    { .regAddr=0x3933, .regData=0x81, .delayUs=0 },
    { .regAddr=0x3934, .regData=0xd0, .delayUs=0 },
    { .regAddr=0x3940, .regData=0x75, .delayUs=0 },
    { .regAddr=0x3941, .regData=0x00, .delayUs=0 },
    { .regAddr=0x3942, .regData=0x01, .delayUs=0 },
    { .regAddr=0x3943, .regData=0xd1, .delayUs=0 },
    { .regAddr=0x3952, .regData=0x02, .delayUs=0 },
    { .regAddr=0x3953, .regData=0x0f, .delayUs=0 },
    { .regAddr=0x3e01, .regData=0x4a, .delayUs=0 },
    { .regAddr=0x3e02, .regData=0xa0, .delayUs=0 },
    { .regAddr=0x3e08, .regData=0x1f, .delayUs=0 },
    { .regAddr=0x3e1b, .regData=0x14, .delayUs=0 },
    // { .regAddr=0x440e, .regData=0x02, .delayUs=0 }, //not in the init seq
    { .regAddr=0x4509, .regData=0x38, .delayUs=0 },
    { .regAddr=0x4819, .regData=0x06, .delayUs=0 },
    { .regAddr=0x481b, .regData=0x03, .delayUs=0 },
    { .regAddr=0x481d, .regData=0x0b, .delayUs=0 },
    { .regAddr=0x481f, .regData=0x02, .delayUs=0 },
    { .regAddr=0x4821, .regData=0x08, .delayUs=0 },
    { .regAddr=0x4823, .regData=0x03, .delayUs=0 },
    { .regAddr=0x4825, .regData=0x02, .delayUs=0 },
    { .regAddr=0x4827, .regData=0x03, .delayUs=0 },
    { .regAddr=0x4829, .regData=0x04, .delayUs=0 },
    { .regAddr=0x5799, .regData=0x06, .delayUs=0 },
    { .regAddr=0x5ae0, .regData=0xfe, .delayUs=0 },
    { .regAddr=0x5ae1, .regData=0x40, .delayUs=0 },
    { .regAddr=0x5ae2, .regData=0x30, .delayUs=0 },
    { .regAddr=0x5ae3, .regData=0x28, .delayUs=0 },
    { .regAddr=0x5ae4, .regData=0x20, .delayUs=0 },
    { .regAddr=0x5ae5, .regData=0x30, .delayUs=0 },
    { .regAddr=0x5ae6, .regData=0x28, .delayUs=0 },
    { .regAddr=0x5ae7, .regData=0x20, .delayUs=0 },
    { .regAddr=0x5ae8, .regData=0x3c, .delayUs=0 },
    { .regAddr=0x5ae9, .regData=0x30, .delayUs=0 },
    { .regAddr=0x5aea, .regData=0x28, .delayUs=0 },
    { .regAddr=0x5aeb, .regData=0x3c, .delayUs=0 },
    { .regAddr=0x5aec, .regData=0x30, .delayUs=0 },
    { .regAddr=0x5aed, .regData=0x28, .delayUs=0 },
    { .regAddr=0x5aee, .regData=0xfe, .delayUs=0 },
    { .regAddr=0x5aef, .regData=0x40, .delayUs=0 },
    { .regAddr=0x5af4, .regData=0x30, .delayUs=0 },
    { .regAddr=0x5af5, .regData=0x28, .delayUs=0 },
    { .regAddr=0x5af6, .regData=0x20, .delayUs=0 },
    { .regAddr=0x5af7, .regData=0x30, .delayUs=0 },
    { .regAddr=0x5af8, .regData=0x28, .delayUs=0 },
    { .regAddr=0x5af9, .regData=0x20, .delayUs=0 },
    { .regAddr=0x5afa, .regData=0x3c, .delayUs=0 },
    { .regAddr=0x5afb, .regData=0x30, .delayUs=0 },
    { .regAddr=0x5afc, .regData=0x28, .delayUs=0 },
    { .regAddr=0x5afd, .regData=0x3c, .delayUs=0 },
    { .regAddr=0x5afe, .regData=0x30, .delayUs=0 },
    { .regAddr=0x5aff, .regData=0x28, .delayUs=0 },
    { .regAddr=0x36e9, .regData=0x53, .delayUs=0 },
    { .regAddr=0x37f9, .regData=0x33, .delayUs=0 },
//    { .regAddr=0x0x0100, .regData=0x0x01, .delayUs=0 },

};
static drv_sensor_reg_info_t  sc2337p_streamOn_setting[] =
{
	{ .regAddr=0x0100, .regData= 0x01, .delayUs=0 },
};

static drv_sensor_reg_info_t  sc2337p_streamOff_setting[] =
{
	{ .regAddr=0x0100, .regData= 0x00, .delayUs=0 }, //500ms
};

static drv_sensor_exp_param_t sc2337p_25fps_exparams =
{
    .maxIntegrationTime = 1432,//1440-8
    .minIntegrationTime = 1,
    .maxAnalogGain      = 128*1024,
    .minAnalogGain      = 1024,
    .lineTime           = 114413,//113778,
    .limitShortMid      = 0,
    .limitLong          = 0,
    .expLineMargin      = 7,
};

static drv_sensor_exp_param_t sc2337p_15fps_exparams =
{
    .maxIntegrationTime = 4480,//1434,2622-8
    .minIntegrationTime = 1,
    .maxAnalogGain      = 128*1024,
    .minAnalogGain      = 1024,
    .lineTime           = 113778,//113778,
    .limitShortMid      = 0,
    .limitLong          = 0,
    .expLineMargin      = 7,
};
static drv_power_setting_t sc2337p_power_up_setting[] =
{
    {.type = DRV_POWER_REG, .regAddr = 0x83c40004, .bitMask  = 0x01, .bitVal = 0x00, .delayUs = 50000}, /// 100ms
    {.type = DRV_POWER_REG, .regAddr = 0x83c40004, .bitMask  = 0x01, .bitVal = 0x01, .delayUs = 50000}  /// APB2 SENSOR_CTRL_REG 0x83c40004
};

static drv_power_setting_t sc2337p_power_down_setting[] =
{
    {.type = DRV_POWER_REG, .regAddr = 0x83c40004, .bitMask  = 0x01, .bitVal = 0x00, .delayUs = 0},
    {.type = DRV_POWER_REG, .regAddr = 0x83c40004, .bitMask  = 0x01, .bitVal = 0x01, .delayUs = 0}
};

static sensor_drv_params_t sc2337p_sensor_params =
{
    .sensorId     = 0x3a,//0xcb,
    .sensorIdAddr = 0x3108,//0x3107,
    .name         = "sc2337p",
    .slaveInfo =
    {
        .hwType      = DRV_HW_I2C,
        .slaveAddr   = (0x32),
        .channel     = 0,
        .regDataType = DRV_ADDR_TYPE_BYTE,
        .regAddrType = DRV_DATA_TYPE_WORD,
    },
    .powerUpSettings  =
    {
        .powerSetting = sc2337p_power_up_setting,
        .size = sizeof(sc2337p_power_up_setting) / sizeof(sc2337p_power_up_setting[0]),
    },
    .powerDownSettings =
    {
        .powerSetting  = sc2337p_power_down_setting,
        .size = sizeof(sc2337p_power_down_setting) / sizeof(sc2337p_power_down_setting[0]),
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
		.maxFps = 20,//25
		.minFps = 10,//8
		.frameLength = 0x708,//0x5A0,
		.expLineMargin = 7,
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
			.settings    = sc2337p_1080p_setting,
			.settingSize = sizeof(sc2337p_1080p_setting) / sizeof (sc2337p_1080p_setting[0]),
		},
		.expParams   = &sc2337p_15fps_exparams,

		.intf = {
			.ln_cfg = {
				.dat_num = 2,
			},
			.link_freq = 396.00f,
		},
	},
    .streamOnSetting =
    {
        .regDataType = DRV_DATA_TYPE_BYTE,
        .regAddrType = DRV_ADDR_TYPE_WORD,
        .direction   = DRV_DIRECTION_WRITE,
        .settings    = sc2337p_streamOn_setting,
        .settingSize = sizeof(sc2337p_streamOn_setting) / sizeof (sc2337p_streamOn_setting[0]),
    },

    .streamOffSetting =
    {
        .regDataType = DRV_DATA_TYPE_BYTE,
        .regAddrType = DRV_ADDR_TYPE_WORD,
        .direction   = DRV_DIRECTION_WRITE,
        .settings    = sc2337p_streamOff_setting,
        .settingSize = sizeof(sc2337p_streamOff_setting) / sizeof (sc2337p_streamOff_setting[0]),
    },

    .fill_exposure_reg = SC2337P_Fill_Exposure_Reg,
};


sensor_drv_params_t* load_sc2337p_sensor_driver(void);

#endif

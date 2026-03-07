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

#ifndef __sc501ai_SENSOR_H__
#define __sc501ai_SENSOR_H__

#include "sensor_common.h"
#include "log_utils.h"

isp_ret_t sc501ai_Fill_Exposure_Reg(sensor_apply_exp_param_t * pExpParam, drv_regs_array_info_t * pRegInfo);
//sc501ai All-pixel scan CSI-2_2lane 24Mhz AD:10bit Output:10bit 660Mbps Master Mode 25.00fps Integration Time: Xx.xxms Gain:XdB vts = 1650 row time=xx.xxus
static drv_sensor_reg_info_t  sc501ai_4mp_setting[] =
{	
  { .regAddr=0x0103, .regData=0x01, .delayUs=0 },
  { .regAddr=0x0100, .regData=0x00, .delayUs=0 },
  { .regAddr=0x36e9, .regData=0x80, .delayUs=0 },
  { .regAddr=0x36f9, .regData=0x80, .delayUs=0 },
  { .regAddr=0x3018, .regData=0x32, .delayUs=0 },
  { .regAddr=0x3019, .regData=0x0c, .delayUs=0 },
  { .regAddr=0x301f, .regData=0x53, .delayUs=0 },
  { .regAddr=0x3253, .regData=0x0a, .delayUs=0 },
  { .regAddr=0x3301, .regData=0x0a, .delayUs=0 },
  { .regAddr=0x3302, .regData=0x18, .delayUs=0 },
  { .regAddr=0x3303, .regData=0x10, .delayUs=0 },
  { .regAddr=0x3304, .regData=0x60, .delayUs=0 },
  { .regAddr=0x3306, .regData=0x60, .delayUs=0 },
  { .regAddr=0x3308, .regData=0x10, .delayUs=0 },
  { .regAddr=0x3309, .regData=0x70, .delayUs=0 },
  { .regAddr=0x330a, .regData=0x00, .delayUs=0 },
  { .regAddr=0x330b, .regData=0xf0, .delayUs=0 },
  { .regAddr=0x330d, .regData=0x18, .delayUs=0 },
  { .regAddr=0x330e, .regData=0x20, .delayUs=0 },
  { .regAddr=0x330f, .regData=0x02, .delayUs=0 },
  { .regAddr=0x3310, .regData=0x02, .delayUs=0 },
  { .regAddr=0x331c, .regData=0x04, .delayUs=0 },
  { .regAddr=0x331e, .regData=0x51, .delayUs=0 },
  { .regAddr=0x331f, .regData=0x61, .delayUs=0 },
  { .regAddr=0x3320, .regData=0x09, .delayUs=0 },
  { .regAddr=0x3333, .regData=0x10, .delayUs=0 },
  { .regAddr=0x334c, .regData=0x08, .delayUs=0 },
  { .regAddr=0x3356, .regData=0x09, .delayUs=0 },
  { .regAddr=0x3364, .regData=0x17, .delayUs=0 },
  { .regAddr=0x336d, .regData=0x03, .delayUs=0 },
  { .regAddr=0x3390, .regData=0x08, .delayUs=0 },
  { .regAddr=0x3391, .regData=0x18, .delayUs=0 },
  { .regAddr=0x3392, .regData=0x38, .delayUs=0 },
  { .regAddr=0x3393, .regData=0x0a, .delayUs=0 },
  { .regAddr=0x3394, .regData=0x20, .delayUs=0 },
  { .regAddr=0x3395, .regData=0x20, .delayUs=0 },
  { .regAddr=0x3396, .regData=0x08, .delayUs=0 },
  { .regAddr=0x3397, .regData=0x18, .delayUs=0 },
  { .regAddr=0x3398, .regData=0x38, .delayUs=0 },
  { .regAddr=0x3399, .regData=0x0a, .delayUs=0 },
  { .regAddr=0x339a, .regData=0x20, .delayUs=0 },
  { .regAddr=0x339b, .regData=0x20, .delayUs=0 },
  { .regAddr=0x339c, .regData=0x20, .delayUs=0 },
  { .regAddr=0x33ac, .regData=0x10, .delayUs=0 },
  { .regAddr=0x33ae, .regData=0x10, .delayUs=0 },
  { .regAddr=0x33af, .regData=0x19, .delayUs=0 },
  { .regAddr=0x360f, .regData=0x01, .delayUs=0 },
  { .regAddr=0x3622, .regData=0x03, .delayUs=0 },
  { .regAddr=0x363a, .regData=0x1f, .delayUs=0 },
  { .regAddr=0x363c, .regData=0x40, .delayUs=0 },
  { .regAddr=0x3651, .regData=0x7d, .delayUs=0 },
  { .regAddr=0x3670, .regData=0x0a, .delayUs=0 },
  { .regAddr=0x3671, .regData=0x07, .delayUs=0 },
  { .regAddr=0x3672, .regData=0x17, .delayUs=0 },
  { .regAddr=0x3673, .regData=0x1e, .delayUs=0 },
  { .regAddr=0x3674, .regData=0x82, .delayUs=0 },
  { .regAddr=0x3675, .regData=0x64, .delayUs=0 },
  { .regAddr=0x3676, .regData=0x66, .delayUs=0 },
  { .regAddr=0x367a, .regData=0x48, .delayUs=0 },
  { .regAddr=0x367b, .regData=0x78, .delayUs=0 },
  { .regAddr=0x367c, .regData=0x58, .delayUs=0 },
  { .regAddr=0x367d, .regData=0x78, .delayUs=0 },
  { .regAddr=0x3690, .regData=0x34, .delayUs=0 },
  { .regAddr=0x3691, .regData=0x34, .delayUs=0 },
  { .regAddr=0x3692, .regData=0x54, .delayUs=0 },
  { .regAddr=0x369c, .regData=0x48, .delayUs=0 },
  { .regAddr=0x369d, .regData=0x78, .delayUs=0 },
  { .regAddr=0x36ea, .regData=0x35, .delayUs=0 },
  { .regAddr=0x36eb, .regData=0x0c, .delayUs=0 },
  { .regAddr=0x36ec, .regData=0x0a, .delayUs=0 },
  { .regAddr=0x36ed, .regData=0x24, .delayUs=0 },
  { .regAddr=0x36fa, .regData=0x35, .delayUs=0 },
  { .regAddr=0x36fb, .regData=0x35, .delayUs=0 },
  { .regAddr=0x36fc, .regData=0x10, .delayUs=0 },
  { .regAddr=0x36fd, .regData=0x24, .delayUs=0 },
  { .regAddr=0x3904, .regData=0x04, .delayUs=0 },
  { .regAddr=0x3908, .regData=0x41, .delayUs=0 },
  { .regAddr=0x391d, .regData=0x04, .delayUs=0 },
  { .regAddr=0x39c2, .regData=0x30, .delayUs=0 },
  { .regAddr=0x3e01, .regData=0xcd, .delayUs=0 },
  { .regAddr=0x3e02, .regData=0xc0, .delayUs=0 },
  { .regAddr=0x3e16, .regData=0x00, .delayUs=0 },
  { .regAddr=0x3e17, .regData=0x80, .delayUs=0 },
  { .regAddr=0x4500, .regData=0x88, .delayUs=0 },
  { .regAddr=0x4509, .regData=0x20, .delayUs=0 },
  { .regAddr=0x4837, .regData=0x18, .delayUs=0 },
  { .regAddr=0x5799, .regData=0x00, .delayUs=0 },
  { .regAddr=0x59e0, .regData=0x60, .delayUs=0 },
  { .regAddr=0x59e1, .regData=0x08, .delayUs=0 },
  { .regAddr=0x59e2, .regData=0x3f, .delayUs=0 },
  { .regAddr=0x59e3, .regData=0x18, .delayUs=0 },
  { .regAddr=0x59e4, .regData=0x18, .delayUs=0 },
  { .regAddr=0x59e5, .regData=0x3f, .delayUs=0 },
  { .regAddr=0x59e7, .regData=0x02, .delayUs=0 },
  { .regAddr=0x59e8, .regData=0x38, .delayUs=0 },
  { .regAddr=0x59e9, .regData=0x20, .delayUs=0 },
  { .regAddr=0x59ea, .regData=0x0c, .delayUs=0 },
  { .regAddr=0x59ec, .regData=0x08, .delayUs=0 },
  { .regAddr=0x59ed, .regData=0x02, .delayUs=0 },
  { .regAddr=0x59ee, .regData=0xa0, .delayUs=0 },
  { .regAddr=0x59ef, .regData=0x08, .delayUs=0 },
  { .regAddr=0x59f4, .regData=0x18, .delayUs=0 },
  { .regAddr=0x59f5, .regData=0x10, .delayUs=0 },
  { .regAddr=0x59f6, .regData=0x0c, .delayUs=0 },
  { .regAddr=0x59f9, .regData=0x02, .delayUs=0 },
  { .regAddr=0x59fa, .regData=0x18, .delayUs=0 },
  { .regAddr=0x59fb, .regData=0x10, .delayUs=0 },
  { .regAddr=0x59fc, .regData=0x0c, .delayUs=0 },
  { .regAddr=0x59ff, .regData=0x02, .delayUs=0 },
  { .regAddr=0x36e9, .regData=0x20, .delayUs=0 },
  { .regAddr=0x36f9, .regData=0x53, .delayUs=0 },
  { .regAddr=0x0100, .regData=0x01, .delayUs=0 },

};


static drv_sensor_reg_info_t  sc501ai_streamOn_setting[] =
{
	{ .regAddr=0x0100, .regData= 0x09, .delayUs=0 },
};

static drv_sensor_reg_info_t  sc501ai_streamOff_setting[] =
{
	{ .regAddr=0x0100, .regData= 0x00, .delayUs=500000 }, //500ms
};

static drv_sensor_exp_param_t sc501ai_30fps_exparams =
{
    .maxIntegrationTime = 1432,
    .minIntegrationTime = 8,
    .maxAnalogGain      = 33532,
    .minAnalogGain      = 1024,
    .lineTime           = 91022,
    .limitShortMid      = 0,
    .limitLong          = 0,
    .expLineMargin      = 1,
};
static drv_sensor_exp_param_t sc501ai_10fps_exparams =
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
static drv_sensor_exp_param_t sc501ai_20fps_exparams =
{
    .maxIntegrationTime = 2242,
    .minIntegrationTime = 8,
    .maxAnalogGain      = 65536,
    .minAnalogGain      = 1024,
    .lineTime           = 90112,
    .limitShortMid      = 0,
    .limitLong          = 0,
    .expLineMargin      = 1,
};
static drv_sensor_exp_param_t sc501ai_15fps_exparams =
{
    .maxIntegrationTime = 1525,//2980,
    .minIntegrationTime = 0,
    .maxAnalogGain      = 65536, //266300, //65536,//133300,
    .minAnalogGain      = 1024,
    .lineTime           = 90112,
    .limitShortMid      = 0,
    .limitLong          = 0,
    .expLineMargin      = 1,
};
static drv_sensor_exp_param_t sc501ai_5fps_exparams =
{
    .maxIntegrationTime = 1432,
    .minIntegrationTime = 8,
    .maxAnalogGain      = 33532,
    .minAnalogGain      = 1024,
    .lineTime           = 546079,
    .limitShortMid      = 0,
    .limitLong          = 0,
    .expLineMargin      = 1,
};

static drv_power_setting_t sc501ai_power_up_setting[] =
{
    {.type = DRV_POWER_REG, .regAddr = 0x83c40004, .bitMask  = 0x01, .bitVal = 0x00, .delayUs = 0}, /// 100ms
    {.type = DRV_POWER_REG, .regAddr = 0x83c40004, .bitMask  = 0x01, .bitVal = 0x01, .delayUs = 0}  /// APB2 SENSOR_CTRL_REG 0x83c40004
};

static drv_power_setting_t sc501ai_power_down_setting[] =
{
    {.type = DRV_POWER_REG, .regAddr = 0x83c40004, .bitMask  = 0x01, .bitVal = 0x00, .delayUs = 0},
    {.type = DRV_POWER_REG, .regAddr = 0x83c40004, .bitMask  = 0x01, .bitVal = 0x01, .delayUs = 0}
};

static sensor_drv_params_t sc501ai_sensor_params =
{
    .sensorId     = 0x1f,
    .sensorIdAddr = 0x3108,
    .name         = "sc501ai",
    .slaveInfo =
    {
        .hwType      = DRV_HW_I2C,
        .slaveAddr   =0x30,
        .channel     = 0,
        .regDataType = DRV_ADDR_TYPE_BYTE,
        .regAddrType = DRV_DATA_TYPE_WORD,
    },
    .powerUpSettings  =
    {
        .powerSetting = sc501ai_power_up_setting,
        .size = sizeof(sc501ai_power_up_setting) / sizeof(sc501ai_power_up_setting[0]),
    },
    .powerDownSettings =
    {
        .powerSetting  = sc501ai_power_down_setting,
        .size = sizeof(sc501ai_power_down_setting) / sizeof(sc501ai_power_down_setting[0]),
    },
    .activesize =
    {
        .width  = 2880,
        .height = 1620
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
		.maxFps = 25,
		.minFps = 15,
		.frameLength = 0x672,
		.expLineMargin = 1,
		.dim =
		{
			.width  = 2880,
			.height = 1620,
		},
		.hdrMode   = SENSOR_NORMAL_MODE,
		.regConfig =
		{
			.regDataType = DRV_DATA_TYPE_BYTE,
			.regAddrType = DRV_ADDR_TYPE_WORD,
			.direction   = DRV_DIRECTION_WRITE,
			.settings    = sc501ai_4mp_setting,
			.settingSize = sizeof(sc501ai_4mp_setting) / sizeof (sc501ai_4mp_setting[0]),
		},
		.expParams   = &sc501ai_15fps_exparams,

		.intf = {
			.ln_cfg = {
				.dat_num = 2,
			},
			.link_freq = 660.0f,
		},
	},
    .streamOnSetting =
    {
        .regDataType = DRV_DATA_TYPE_BYTE,
        .regAddrType = DRV_ADDR_TYPE_WORD,
        .direction   = DRV_DIRECTION_WRITE,
        .settings    = sc501ai_streamOn_setting,
        .settingSize = sizeof(sc501ai_streamOn_setting) / sizeof (sc501ai_streamOn_setting[0]),
    },

    .streamOffSetting =
    {
        .regDataType = DRV_DATA_TYPE_BYTE,
        .regAddrType = DRV_ADDR_TYPE_WORD,
        .direction   = DRV_DIRECTION_WRITE,
        .settings    = sc501ai_streamOff_setting,
        .settingSize = sizeof(sc501ai_streamOff_setting) / sizeof (sc501ai_streamOff_setting[0]),
    },

    .fill_exposure_reg = sc501ai_Fill_Exposure_Reg,
};


sensor_drv_params_t* load_sc501ai_sensor_driver(void);

#endif

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

#ifndef __SC200ai_SENSOR_H__
#define __SC200ai_SENSOR_H__

#include "sensor_common.h"
#include "log_utils.h"

isp_ret_t SC200ai_Fill_Exposure_Reg(sensor_apply_exp_param_t * pExpParam, drv_regs_array_info_t * pRegInfo);



//SC200ai All-pixel scan CSI-2_2lane 27Mhz AD:10bit Output:10bit 704Mbps Master Mode 30.00fps Integration Time: Xx.xxms Gain:XdB vts = 1500 row time=22.22us
static drv_sensor_reg_info_t  sc200ai_1080p_setting[] =
{
 	{ .regAddr=0x0103,.regData= 0x01,.delayUs=0 },
	{ .regAddr=0x0100,.regData= 0x00,.delayUs=0 },
	{ .regAddr=0x36e9,.regData= 0x80,.delayUs=0 },
	{ .regAddr=0x36f9,.regData= 0x80,.delayUs=0 },
	{ .regAddr=0x301f,.regData= 0x90,.delayUs=0 },//
 	{ .regAddr=0x320c,.regData= 0x04,.delayUs=0 },//
  { .regAddr=0x320d,.regData= 0x18,.delayUs=0 },//
  { .regAddr=0x320e,.regData= 0x06,.delayUs=0 },//
  { .regAddr=0x320f,.regData= 0x5f,.delayUs=0 },//
 
	{ .regAddr=0x3243,.regData= 0x01,.delayUs=0 },
	{ .regAddr=0x3248,.regData= 0x02,.delayUs=0 },
	{ .regAddr=0x3249,.regData= 0x09,.delayUs=0 },
	{ .regAddr=0x3253,.regData= 0x08,.delayUs=0 },
	{ .regAddr=0x3271,.regData= 0x0a,.delayUs=0 },
	{ .regAddr=0x3301,.regData= 0x20,.delayUs=0 },
	{ .regAddr=0x3304,.regData= 0x40,.delayUs=0 },
	{ .regAddr=0x3306,.regData= 0x32,.delayUs=0 },
	{ .regAddr=0x330b,.regData= 0x88,.delayUs=0 },
	{ .regAddr=0x330f,.regData= 0x02,.delayUs=0 },
	{ .regAddr=0x331e,.regData= 0x39,.delayUs=0 },
	{ .regAddr=0x3333,.regData= 0x10,.delayUs=0 },
	{ .regAddr=0x3621,.regData= 0xe8,.delayUs=0 },
	{ .regAddr=0x3622,.regData= 0x16,.delayUs=0 },
	{ .regAddr=0x3637,.regData= 0x1b,.delayUs=0 },
	{ .regAddr=0x363a,.regData= 0x1f,.delayUs=0 },
	{ .regAddr=0x363b,.regData= 0xc6,.delayUs=0 },
	{ .regAddr=0x363c,.regData= 0x0e,.delayUs=0 },
	{ .regAddr=0x3670,.regData= 0x0a,.delayUs=0 },
	{ .regAddr=0x3674,.regData= 0x82,.delayUs=0 },
	{ .regAddr=0x3675,.regData= 0x76,.delayUs=0 },
	{ .regAddr=0x3676,.regData= 0x78,.delayUs=0 },
	{ .regAddr=0x367c,.regData= 0x48,.delayUs=0 },
	{ .regAddr=0x367d,.regData= 0x58,.delayUs=0 },
	{ .regAddr=0x3690,.regData= 0x34,.delayUs=0 },
	{ .regAddr=0x3691,.regData= 0x33,.delayUs=0 },
	{ .regAddr=0x3692,.regData= 0x44,.delayUs=0 },
	{ .regAddr=0x369c,.regData= 0x40,.delayUs=0 },
	{ .regAddr=0x369d,.regData= 0x48,.delayUs=0 },
	{ .regAddr=0x36ea,.regData= 0x35,.delayUs=0 },
	{ .regAddr=0x36eb,.regData= 0x0d,.delayUs=0 },
	{ .regAddr=0x36ec,.regData= 0x1c,.delayUs=0 },
	{ .regAddr=0x36ed,.regData= 0x14,.delayUs=0 },
	{ .regAddr=0x36fa,.regData= 0x35,.delayUs=0 },
	{ .regAddr=0x36fb,.regData= 0x00,.delayUs=0 },
	{ .regAddr=0x36fc,.regData= 0x10,.delayUs=0 },
	{ .regAddr=0x36fd,.regData= 0x14,.delayUs=0 },
	{ .regAddr=0x3901,.regData= 0x02,.delayUs=0 },
	{ .regAddr=0x3904,.regData= 0x04,.delayUs=0 },
	{ .regAddr=0x3908,.regData= 0x41,.delayUs=0 },
	{ .regAddr=0x391d,.regData= 0x14,.delayUs=0 },
	{ .regAddr=0x391f,.regData= 0x18,.delayUs=0 },
  { .regAddr=0x3e00,.regData= 0x00,.delayUs=0 },
	{ .regAddr=0x3e01,.regData= 0x8b,.delayUs=0 },
	{ .regAddr=0x3e02,.regData= 0x60,.delayUs=0 },
	{ .regAddr=0x3e16,.regData= 0x00,.delayUs=0 },
	{ .regAddr=0x3e17,.regData= 0x80,.delayUs=0 },
	{ .regAddr=0x3f09,.regData= 0x48,.delayUs=0 },
	{ .regAddr=0x5787,.regData= 0x10,.delayUs=0 },
	{ .regAddr=0x5788,.regData= 0x06,.delayUs=0 },
	{ .regAddr=0x578a,.regData= 0x10,.delayUs=0 },
	{ .regAddr=0x578b,.regData= 0x06,.delayUs=0 },
	{ .regAddr=0x5790,.regData= 0x10,.delayUs=0 },
	{ .regAddr=0x5791,.regData= 0x10,.delayUs=0 },
	{ .regAddr=0x5792,.regData= 0x00,.delayUs=0 },
	{ .regAddr=0x5793,.regData= 0x10,.delayUs=0 },
	{ .regAddr=0x5794,.regData= 0x10,.delayUs=0 },
	{ .regAddr=0x5795,.regData= 0x00,.delayUs=0 },
	{ .regAddr=0x5799,.regData= 0x00,.delayUs=0 },
	{ .regAddr=0x57c7,.regData= 0x10,.delayUs=0 },
	{ .regAddr=0x57c8,.regData= 0x06,.delayUs=0 },
	{ .regAddr=0x57ca,.regData= 0x10,.delayUs=0 },
	{ .regAddr=0x57cb,.regData= 0x06,.delayUs=0 },
	{ .regAddr=0x57d1,.regData= 0x10,.delayUs=0 },
	{ .regAddr=0x57d4,.regData= 0x10,.delayUs=0 },
	{ .regAddr=0x57d9,.regData= 0x00,.delayUs=0 },
	{ .regAddr=0x59e0,.regData= 0x60,.delayUs=0 },
	{ .regAddr=0x59e1,.regData= 0x08,.delayUs=0 },
	{ .regAddr=0x59e2,.regData= 0x3f,.delayUs=0 },
	{ .regAddr=0x59e3,.regData= 0x18,.delayUs=0 },
	{ .regAddr=0x59e4,.regData= 0x18,.delayUs=0 },
	{ .regAddr=0x59e5,.regData= 0x3f,.delayUs=0 },
	{ .regAddr=0x59e6,.regData= 0x06,.delayUs=0 },
	{ .regAddr=0x59e7,.regData= 0x02,.delayUs=0 },
	{ .regAddr=0x59e8,.regData= 0x38,.delayUs=0 },
	{ .regAddr=0x59e9,.regData= 0x10,.delayUs=0 },
	{ .regAddr=0x59ea,.regData= 0x0c,.delayUs=0 },
	{ .regAddr=0x59eb,.regData= 0x10,.delayUs=0 },
	{ .regAddr=0x59ec,.regData= 0x04,.delayUs=0 },
	{ .regAddr=0x59ed,.regData= 0x02,.delayUs=0 },
	{ .regAddr=0x59ee,.regData= 0xa0,.delayUs=0 },
	{ .regAddr=0x59ef,.regData= 0x08,.delayUs=0 },
	{ .regAddr=0x59f4,.regData= 0x18,.delayUs=0 },
	{ .regAddr=0x59f5,.regData= 0x10,.delayUs=0 },
	{ .regAddr=0x59f6,.regData= 0x0c,.delayUs=0 },
	{ .regAddr=0x59f7,.regData= 0x10,.delayUs=0 },
	{ .regAddr=0x59f8,.regData= 0x06,.delayUs=0 },
	{ .regAddr=0x59f9,.regData= 0x02,.delayUs=0 },
	{ .regAddr=0x59fa,.regData= 0x18,.delayUs=0 },
	{ .regAddr=0x59fb,.regData= 0x10,.delayUs=0 },
	{ .regAddr=0x59fc,.regData= 0x0c,.delayUs=0 },
	{ .regAddr=0x59fd,.regData= 0x10,.delayUs=0 },
	{ .regAddr=0x59fe,.regData= 0x04,.delayUs=0 },
	{ .regAddr=0x59ff,.regData= 0x02,.delayUs=0 },
	{ .regAddr=0x36e9,.regData= 0x44,.delayUs=0 },
	{ .regAddr=0x36f9,.regData= 0x34,.delayUs=0 },
	{ .regAddr=0x0100,.regData= 0x01,.delayUs=0 }
};

static drv_sensor_reg_info_t  sc200ai_streamOn_setting[] =
{
	{ .regAddr=0x0100, .regData= 0x01, .delayUs=0 },
};

static drv_sensor_reg_info_t  sc200ai_streamOff_setting[] =
{
	{ .regAddr=0x0100, .regData= 0x00, .delayUs=500000 }, //500ms
};

static drv_sensor_exp_param_t sc200ai_30fps_exparams =
{
    .maxIntegrationTime = 1120,
    .minIntegrationTime = 8,
    .maxAnalogGain      = 33532,
    .minAnalogGain      = 1024,
    .lineTime           = 91022,
    .limitShortMid      = 0,
    .limitLong          = 0,
    .expLineMargin      = 1,
};
static drv_sensor_exp_param_t sc200ai_20fps_exparams =
{
    .maxIntegrationTime = 1120,
    .minIntegrationTime = 8,
    .maxAnalogGain      = 128*1024,
    .minAnalogGain      = 1024,
    .lineTime           = 90112,
    .limitShortMid      = 0,
    .limitLong          = 0,
    .expLineMargin      = 1,
};
static drv_sensor_exp_param_t sc200ai_15fps_exparams =
{
    .maxIntegrationTime = 2980,
    .minIntegrationTime = 0,
    .maxAnalogGain      = 128*1024,
    .minAnalogGain      = 1024,
    .lineTime           = 90112,
    .limitShortMid      = 0,
    .limitLong          = 0,
    .expLineMargin      = 1,
};

static drv_power_setting_t sc200ai_power_up_setting[] =
{
    {.type = DRV_POWER_REG, .regAddr = 0x83c40004, .bitMask  = 0x01, .bitVal = 0x00, .delayUs = 100000}, /// 100ms
    {.type = DRV_POWER_REG, .regAddr = 0x83c40004, .bitMask  = 0x01, .bitVal = 0x01, .delayUs = 100000}  /// APB2 SENSOR_CTRL_REG 0x83c40004
};

static drv_power_setting_t sc200ai_power_down_setting[] =
{
    {.type = DRV_POWER_REG, .regAddr = 0x83c40004, .bitMask  = 0x01, .bitVal = 0x00, .delayUs = 100000},
    {.type = DRV_POWER_REG, .regAddr = 0x83c40004, .bitMask  = 0x01, .bitVal = 0x01, .delayUs = 100000}
};

static sensor_drv_params_t sc200ai_aux1_sensor_params =
{
    .sensorId     = 0xcb,
    .sensorIdAddr = 0x3107,
    .name         = "sc200ai",
    .slaveInfo =
    {
        .hwType      = DRV_HW_I2C,
        .slaveAddr   = (0x30),
        .channel     = 1,
        .regDataType = DRV_ADDR_TYPE_BYTE,
        .regAddrType = DRV_DATA_TYPE_WORD,
    },
    .powerUpSettings  =
    {
        .powerSetting = sc200ai_power_up_setting,
        .size = sizeof(sc200ai_power_up_setting) / sizeof(sc200ai_power_up_setting[0]),
    },
    .powerDownSettings =
    {
        .powerSetting  = sc200ai_power_down_setting,
        .size = sizeof(sc200ai_power_down_setting) / sizeof(sc200ai_power_down_setting[0]),
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
		.frameLength = 0x65f,
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
			.settings    = sc200ai_1080p_setting,
			.settingSize = sizeof(sc200ai_1080p_setting) / sizeof (sc200ai_1080p_setting[0]),
		},
		.expParams   = &sc200ai_30fps_exparams,

		.intf = {
			.ln_cfg = {
				.dat_num = 2,
			},
			.link_freq = 352.00f,
		},
	},
	


    .streamOnSetting =
    {
        .regDataType = DRV_DATA_TYPE_BYTE,
        .regAddrType = DRV_ADDR_TYPE_WORD,
        .direction   = DRV_DIRECTION_WRITE,
        .settings    = sc200ai_streamOn_setting,
        .settingSize = sizeof(sc200ai_streamOn_setting) / sizeof (sc200ai_streamOn_setting[0]),
    },

    .streamOffSetting =
    {
        .regDataType = DRV_DATA_TYPE_BYTE,
        .regAddrType = DRV_ADDR_TYPE_WORD,
        .direction   = DRV_DIRECTION_WRITE,
        .settings    = sc200ai_streamOff_setting,
        .settingSize = sizeof(sc200ai_streamOff_setting) / sizeof (sc200ai_streamOff_setting[0]),
    },

    .fill_exposure_reg = SC200ai_Fill_Exposure_Reg,
};


sensor_drv_params_t* load_sc200ai_sensor_driver(void);

#endif

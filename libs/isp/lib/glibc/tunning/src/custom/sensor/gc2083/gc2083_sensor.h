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

#ifndef __GC0283_SENSOR_H__
#define __GC2083_SENSOR_H__

#include "sensor_common.h"
#include "log_utils.h"

isp_ret_t GC2083_Fill_Exposure_Reg(sensor_apply_exp_param_t * pExpParam, drv_regs_array_info_t * pRegInfo);

//GC2083
//window size:1920x1080
//mclk=24mhz,mipi_rate=594Mbps/lane   PCLK=74.25M
//rowtime=29.62963us
//frame rate:30fps,
//hts=1100*2,vts=1125,
//bayer mode:RGGB
static drv_sensor_reg_info_t  gc2083_2mp_setting[] =
{	
	{ .regAddr=0x03fe, .regData= 0xf0, .delayUs=0 },
	{ .regAddr=0x03fe, .regData= 0xf0, .delayUs=0 },
	{ .regAddr=0x03fe, .regData= 0xf0, .delayUs=0 },
	{ .regAddr=0x03fe, .regData= 0x00, .delayUs=0 },
	{ .regAddr=0x03f2, .regData= 0x00, .delayUs=0 },
	{ .regAddr=0x03f3, .regData= 0x00, .delayUs=0 },
	{ .regAddr=0x03f4, .regData= 0x36, .delayUs=0 },
	{ .regAddr=0x03f5, .regData= 0xc0, .delayUs=0 },
	{ .regAddr=0x03f6, .regData= 0x44, .delayUs=0 },
	{ .regAddr=0x03f7, .regData= 0x01, .delayUs=0 },
	{ .regAddr=0x03f8, .regData= 0x63, .delayUs=0 },
	{ .regAddr=0x03f9, .regData= 0x41, .delayUs=0 },
	{ .regAddr=0x03fc, .regData= 0x8e, .delayUs=0 },
	{ .regAddr=0x0381, .regData= 0x07, .delayUs=0 },
	{ .regAddr=0x00d7, .regData= 0x29, .delayUs=0 },
	{ .regAddr=0x0d6d, .regData= 0x18, .delayUs=0 },
	{ .regAddr=0x00d5, .regData= 0x03, .delayUs=0 },
	{ .regAddr=0x0082, .regData= 0x01, .delayUs=0 },
	{ .regAddr=0x0db3, .regData= 0xd4, .delayUs=0 },
	{ .regAddr=0x0db0, .regData= 0x0d, .delayUs=0 },
	{ .regAddr=0x0db5, .regData= 0x96, .delayUs=0 }, 
	{ .regAddr=0x0d03, .regData= 0x02, .delayUs=0 },
	{ .regAddr=0x0d04, .regData= 0x02, .delayUs=0 },
	{ .regAddr=0x0d05, .regData= 0x04, .delayUs=0 }, //HMAX 0x44c=29.62963us
	{ .regAddr=0x0d06, .regData= 0x20, .delayUs=0 }, 
	{ .regAddr=0x0d07, .regData= 0x00, .delayUs=0 },
	{ .regAddr=0x0d08, .regData= 0x11, .delayUs=0 },
	{ .regAddr=0x0d09, .regData= 0x00, .delayUs=0 }, 
	{ .regAddr=0x0d0a, .regData= 0x02, .delayUs=0 },
	{ .regAddr=0x000b, .regData= 0x00, .delayUs=0 },
	{ .regAddr=0x000c, .regData= 0x02, .delayUs=0 },
	{ .regAddr=0x0d0d, .regData= 0x04, .delayUs=0 },
	{ .regAddr=0x0d0e, .regData= 0x40, .delayUs=0 },
	{ .regAddr=0x000f, .regData= 0x07, .delayUs=0 },
	{ .regAddr=0x0010, .regData= 0x90, .delayUs=0 },
	{ .regAddr=0x0017, .regData= 0x0c, .delayUs=0 },
	{ .regAddr=0x0d73, .regData= 0x92, .delayUs=0 },
	{ .regAddr=0x0076, .regData= 0x00, .delayUs=0 },
	{ .regAddr=0x0d76, .regData= 0x00, .delayUs=0 },
	// { .regAddr=0x0d41, .regData= 0x04, .delayUs=0 },
	// { .regAddr=0x0d42, .regData= 0x65, .delayUs=0 },
	{ .regAddr=0x0d41, .regData= 0x09, .delayUs=0 },
	{ .regAddr=0x0d42, .regData= 0xe4, .delayUs=0 },
	// { .regAddr=0x0d7a, .regData= 0x10, .delayUs=0 },
	{ .regAddr=0x0d19, .regData= 0x31, .delayUs=0 },
	{ .regAddr=0x0d25, .regData= 0xcb, .delayUs=0 },
	{ .regAddr=0x0d20, .regData= 0x60, .delayUs=0 },
	{ .regAddr=0x0d27, .regData= 0x03, .delayUs=0 },
	{ .regAddr=0x0d29, .regData= 0x60, .delayUs=0 },
	{ .regAddr=0x0d43, .regData= 0x10, .delayUs=0 },
	{ .regAddr=0x0d49, .regData= 0x10, .delayUs=0 },
	{ .regAddr=0x0d55, .regData= 0x18, .delayUs=0 },
	{ .regAddr=0x0dc2, .regData= 0x44, .delayUs=0 },
	{ .regAddr=0x0058, .regData= 0x3c, .delayUs=0 },
	{ .regAddr=0x00d8, .regData= 0x68, .delayUs=0 },
	{ .regAddr=0x00d9, .regData= 0x14, .delayUs=0 },
	{ .regAddr=0x00da, .regData= 0xc1, .delayUs=0 },
	{ .regAddr=0x0050, .regData= 0x18, .delayUs=0 },
	{ .regAddr=0x0db6, .regData= 0x3d, .delayUs=0 },
	{ .regAddr=0x00d2, .regData= 0xbc, .delayUs=0 },
	{ .regAddr=0x0d66, .regData= 0x42, .delayUs=0 },
	{ .regAddr=0x008c, .regData= 0x05, .delayUs=0 },
	{ .regAddr=0x008d, .regData= 0xff, .delayUs=0 },
	{ .regAddr=0x007a, .regData= 0x50, .delayUs=0 },   //global gain
	{ .regAddr=0x00d0, .regData= 0x00, .delayUs=0 },
	{ .regAddr=0x00d1, .regData= 0x00, .delayUs=0 },
	{ .regAddr=0x00f8, .regData= 0x63, .delayUs=0 }, //X add?
	{ .regAddr=0x00f9, .regData= 0x40, .delayUs=0 }, //X add?
	{ .regAddr=0x0dc1, .regData= 0x00, .delayUs=0 }, //X add?
	{ .regAddr=0x0102, .regData= 0xa9, .delayUs=0 },    //89
	{ .regAddr=0x0158, .regData= 0x00, .delayUs=0 },
	{ .regAddr=0x0107, .regData= 0xa6, .delayUs=0 },
	{ .regAddr=0x0108, .regData= 0xa9, .delayUs=0 },
	{ .regAddr=0x0109, .regData= 0xa8, .delayUs=0 },
	{ .regAddr=0x010a, .regData= 0xa7, .delayUs=0 },
	{ .regAddr=0x010b, .regData= 0xff, .delayUs=0 },
	{ .regAddr=0x010c, .regData= 0xff, .delayUs=0 },
	{ .regAddr=0x0428, .regData= 0x86, .delayUs=0 },
	{ .regAddr=0x0429, .regData= 0x86, .delayUs=0 },
	{ .regAddr=0x042a, .regData= 0x86, .delayUs=0 },
	{ .regAddr=0x042b, .regData= 0x68, .delayUs=0 },
	{ .regAddr=0x042c, .regData= 0x68, .delayUs=0 },
	{ .regAddr=0x042d, .regData= 0x68, .delayUs=0 },
	{ .regAddr=0x042e, .regData= 0x68, .delayUs=0 },
	{ .regAddr=0x042f, .regData= 0x68, .delayUs=0 },
	{ .regAddr=0x0430, .regData= 0x4f, .delayUs=0 },
	{ .regAddr=0x0431, .regData= 0x68, .delayUs=0 },
	{ .regAddr=0x0432, .regData= 0x67, .delayUs=0 },
	{ .regAddr=0x0433, .regData= 0x66, .delayUs=0 },
	{ .regAddr=0x0434, .regData= 0x66, .delayUs=0 },
	{ .regAddr=0x0435, .regData= 0x66, .delayUs=0 },
	{ .regAddr=0x0436, .regData= 0x66, .delayUs=0 },
	{ .regAddr=0x0437, .regData= 0x66, .delayUs=0 },
	{ .regAddr=0x0438, .regData= 0x62, .delayUs=0 },
	{ .regAddr=0x0439, .regData= 0x62, .delayUs=0 },
	{ .regAddr=0x043a, .regData= 0x62, .delayUs=0 },
	{ .regAddr=0x043b, .regData= 0x62, .delayUs=0 },
	{ .regAddr=0x043c, .regData= 0x62, .delayUs=0 },
	{ .regAddr=0x043d, .regData= 0x62, .delayUs=0 },
	{ .regAddr=0x043e, .regData= 0x62, .delayUs=0 },
	{ .regAddr=0x043f, .regData= 0x62, .delayUs=0 },
	{ .regAddr=0x0077, .regData= 0x01, .delayUs=0 },
	{ .regAddr=0x0078, .regData= 0x65, .delayUs=0 },
	{ .regAddr=0x0079, .regData= 0x04, .delayUs=0 },
	{ .regAddr=0x0067, .regData= 0xa0, .delayUs=0 },
	{ .regAddr=0x0054, .regData= 0xff, .delayUs=0 },
	{ .regAddr=0x0055, .regData= 0x02, .delayUs=0 },
	{ .regAddr=0x0056, .regData= 0x00, .delayUs=0 },
	{ .regAddr=0x0057, .regData= 0x04, .delayUs=0 },
	{ .regAddr=0x005a, .regData= 0xff, .delayUs=0 },
	{ .regAddr=0x005b, .regData= 0x07, .delayUs=0 },
	{ .regAddr=0x0026, .regData= 0x01, .delayUs=0 },
	{ .regAddr=0x0152, .regData= 0x02, .delayUs=0 },
	{ .regAddr=0x0153, .regData= 0x50, .delayUs=0 },
	{ .regAddr=0x0155, .regData= 0x93, .delayUs=0 },
	{ .regAddr=0x0410, .regData= 0x16, .delayUs=0 },
	{ .regAddr=0x0411, .regData= 0x16, .delayUs=0 },
	{ .regAddr=0x0412, .regData= 0x16, .delayUs=0 },
	{ .regAddr=0x0413, .regData= 0x16, .delayUs=0 },
	{ .regAddr=0x0414, .regData= 0x6f, .delayUs=0 },
	{ .regAddr=0x0415, .regData= 0x6f, .delayUs=0 },
	{ .regAddr=0x0416, .regData= 0x6f, .delayUs=0 },
	{ .regAddr=0x0417, .regData= 0x6f, .delayUs=0 },
	{ .regAddr=0x04e0, .regData= 0x18, .delayUs=0 },
	{ .regAddr=0x0192, .regData= 0x04, .delayUs=0 },
	{ .regAddr=0x0194, .regData= 0x04, .delayUs=0 },
	{ .regAddr=0x0195, .regData= 0x04, .delayUs=0 },
	{ .regAddr=0x0196, .regData= 0x38, .delayUs=0 },
	{ .regAddr=0x0197, .regData= 0x07, .delayUs=0 },
	{ .regAddr=0x0198, .regData= 0x80, .delayUs=0 },
	{ .regAddr=0x0201, .regData= 0x27, .delayUs=0 },
	{ .regAddr=0x0202, .regData= 0x53, .delayUs=0 },  //0x50
	{ .regAddr=0x0203, .regData= 0xce, .delayUs=0 },  //0xb6//0x8e
	{ .regAddr=0x0204, .regData= 0x40, .delayUs=0 },
	{ .regAddr=0x0212, .regData= 0x07, .delayUs=0 },
	{ .regAddr=0x0213, .regData= 0x80, .delayUs=0 },
	{ .regAddr=0x0215, .regData= 0x12, .delayUs=0 },
	{ .regAddr=0x0229, .regData= 0x05, .delayUs=0 },
	{ .regAddr=0x0237, .regData= 0x03, .delayUs=0 },
	{ .regAddr=0x023e, .regData= 0x81, .delayUs=0 },
};

static drv_sensor_reg_info_t  gc2083_streamOn_setting[] =
{

	{ .regAddr=0x03fe, .regData= 0x10, .delayUs=0 },
	{ .regAddr=0x03fe, .regData= 0x00, .delayUs=0 },
	{ .regAddr=0x023e, .regData= 0x99, .delayUs=0 },
};

static drv_sensor_reg_info_t  gc2083_streamOff_setting[] =
{
	//{ .regAddr=0x0100, .regData= 0x00, .delayUs=500000 }, //500ms
};

static drv_sensor_exp_param_t gc2083_30fps_exparams =
{
    .maxIntegrationTime = 1072,
    .minIntegrationTime = 8,
    .maxAnalogGain      = 65536,
    .minAnalogGain      = 1024,
    .lineTime           = 91022,
    .limitShortMid      = 0,
    .limitLong          = 0,
    .expLineMargin      = 1,
};
static drv_sensor_exp_param_t gc2083_15fps_exparams =
{
    .maxIntegrationTime = 2000,
    .minIntegrationTime = 8,
    .maxAnalogGain      = 65536,
    .minAnalogGain      = 1024,
    .lineTime           = 112000,//136533,
    .limitShortMid      = 0,
    .limitLong          = 0,
    .expLineMargin      = 1,
};
static drv_power_setting_t gc2083_power_up_setting[] =
{
    {.type = DRV_POWER_REG, .regAddr = 0x83c40004, .bitMask  = 0x01, .bitVal = 0x00, .delayUs = 0}, /// 100ms
    {.type = DRV_POWER_REG, .regAddr = 0x83c40004, .bitMask  = 0x01, .bitVal = 0x01, .delayUs = 0}  /// APB2 SENSOR_CTRL_REG 0x83c40004
};

static drv_power_setting_t gc2083_power_down_setting[] =
{
    {.type = DRV_POWER_REG, .regAddr = 0x83c40004, .bitMask  = 0x01, .bitVal = 0x00, .delayUs = 0},
    {.type = DRV_POWER_REG, .regAddr = 0x83c40004, .bitMask  = 0x01, .bitVal = 0x01, .delayUs = 0}
};

static sensor_drv_params_t gc2083_sensor_params =
{
    .sensorId     = 0x83,
    .sensorIdAddr = 0x3f1,
    .name         = "gc2083",
    .slaveInfo =
    {
        .hwType      = DRV_HW_I2C,
        .slaveAddr   = (0x7e >> 1),
        .channel     = 0,
        .regDataType = DRV_ADDR_TYPE_BYTE,
        .regAddrType = DRV_DATA_TYPE_WORD,
    },
    .powerUpSettings  =
    {
        .powerSetting = gc2083_power_up_setting,
        .size = sizeof(gc2083_power_up_setting) / sizeof(gc2083_power_up_setting[0]),
    },
    .powerDownSettings =
    {
        .powerSetting  = gc2083_power_down_setting,
        .size = sizeof(gc2083_power_down_setting) / sizeof(gc2083_power_down_setting[0]),
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
		.maxFps = 15,
		.minFps = 5,
		.frameLength = 0x7FD,
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
			.settings    = gc2083_2mp_setting,
			.settingSize = sizeof(gc2083_2mp_setting) / sizeof (gc2083_2mp_setting[0]),
		},
		.expParams   = &gc2083_15fps_exparams,

		.intf = {
			.ln_cfg = {
				.dat_num = 2,
			},
			.link_freq = 594.0f,
		},
	},

    .streamOnSetting =
    {
        .regDataType = DRV_DATA_TYPE_BYTE,
        .regAddrType = DRV_ADDR_TYPE_WORD,
        .direction   = DRV_DIRECTION_WRITE,
        .settings    = gc2083_streamOn_setting,
        .settingSize = sizeof(gc2083_streamOn_setting) / sizeof (gc2083_streamOn_setting[0]),
    },

    .streamOffSetting =
    {
        .regDataType = DRV_DATA_TYPE_BYTE,
        .regAddrType = DRV_ADDR_TYPE_WORD,
        .direction   = DRV_DIRECTION_WRITE,
        .settings    = gc2083_streamOff_setting,
        .settingSize = sizeof(gc2083_streamOff_setting) / sizeof (gc2083_streamOff_setting[0]),
    },

    .fill_exposure_reg = GC2083_Fill_Exposure_Reg,
};


sensor_drv_params_t* load_gc2083_sensor_driver(void);

#endif

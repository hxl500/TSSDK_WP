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

#ifndef __OV5647_SENSOR_H__
#define __OV5647_SENSOR_H__

#include "sensor_common.h"
#include "log_utils.h"

isp_ret_t OV5647_Fill_Exposure_Reg(sensor_apply_exp_param_t * pExpParam, drv_regs_array_info_t * pRegInfo);

static drv_sensor_reg_info_t  ov5647_normal_setting[] =
{
    { .regAddr = 0x3035, .regData = 0x11, .delayUs = 0x0,},  // Noted by wsh: Debug mode.
    { .regAddr = 0x303c, .regData = 0x11, .delayUs = 0x0,},  // Noted by wsh: PLL setting.
    { .regAddr = 0x370c, .regData = 0x03, .delayUs = 0x0,},
    { .regAddr = 0x5000, .regData = 0x06, .delayUs = 0x0,},
    { .regAddr = 0x5003, .regData = 0x08, .delayUs = 0x0,},
    { .regAddr = 0x5a00, .regData = 0x08, .delayUs = 0x0,},
    { .regAddr = 0x3000, .regData = 0xff, .delayUs = 0x0,},
    { .regAddr = 0x3001, .regData = 0xff, .delayUs = 0x0,},
    { .regAddr = 0x3002, .regData = 0xff, .delayUs = 0x0,},
    { .regAddr = 0x301d, .regData = 0xf0, .delayUs = 0x0,},
    { .regAddr = 0x3a18, .regData = 0x00, .delayUs = 0x0,},
    { .regAddr = 0x3a19, .regData = 0xf8, .delayUs = 0x0,},
    { .regAddr = 0x3c01, .regData = 0x80, .delayUs = 0x0,},
    { .regAddr = 0x3b07, .regData = 0x0c, .delayUs = 0x0,},
    { .regAddr = 0x3708, .regData = 0x64, .delayUs = 0x0,},
    { .regAddr = 0x3630, .regData = 0x2e, .delayUs = 0x0,},
    { .regAddr = 0x3632, .regData = 0xe2, .delayUs = 0x0,},
    { .regAddr = 0x3633, .regData = 0x23, .delayUs = 0x0,},
    { .regAddr = 0x3634, .regData = 0x44, .delayUs = 0x0,},
    { .regAddr = 0x3620, .regData = 0x64, .delayUs = 0x0,},
    { .regAddr = 0x3621, .regData = 0xe0, .delayUs = 0x0,},
    { .regAddr = 0x3600, .regData = 0x37, .delayUs = 0x0,},
    { .regAddr = 0x3704, .regData = 0xa0, .delayUs = 0x0,},
    { .regAddr = 0x3703, .regData = 0x5a, .delayUs = 0x0,},
    { .regAddr = 0x3715, .regData = 0x78, .delayUs = 0x0,},
    { .regAddr = 0x3717, .regData = 0x01, .delayUs = 0x0,},
    { .regAddr = 0x3731, .regData = 0x02, .delayUs = 0x0,},
    { .regAddr = 0x370b, .regData = 0x60, .delayUs = 0x0,},
    { .regAddr = 0x3705, .regData = 0x1a, .delayUs = 0x0,},
    { .regAddr = 0x3f05, .regData = 0x02, .delayUs = 0x0,},
    { .regAddr = 0x3f06, .regData = 0x10, .delayUs = 0x0,},
    { .regAddr = 0x3f01, .regData = 0x0a, .delayUs = 0x0,},
    { .regAddr = 0x3a08, .regData = 0x01, .delayUs = 0x0,},
    { .regAddr = 0x3a0f, .regData = 0x58, .delayUs = 0x0,},
    { .regAddr = 0x3a10, .regData = 0x50, .delayUs = 0x0,},
    { .regAddr = 0x3a1b, .regData = 0x58, .delayUs = 0x0,},
    { .regAddr = 0x3a1e, .regData = 0x50, .delayUs = 0x0,},
    { .regAddr = 0x3a11, .regData = 0x60, .delayUs = 0x0,},
    { .regAddr = 0x3a1f, .regData = 0x28, .delayUs = 0x0,},
    { .regAddr = 0x4001, .regData = 0x02, .delayUs = 0x0,},
    { .regAddr = 0x4000, .regData = 0x09, .delayUs = 0x0,},
    { .regAddr = 0x3000, .regData = 0x00, .delayUs = 0x0,},
    { .regAddr = 0x3001, .regData = 0x00, .delayUs = 0x0,},
    { .regAddr = 0x3002, .regData = 0x00, .delayUs = 0x0,},
    { .regAddr = 0x3017, .regData = 0xe0, .delayUs = 0x0,},
    { .regAddr = 0x301c, .regData = 0xfc, .delayUs = 0x0,},
    { .regAddr = 0x3636, .regData = 0x06, .delayUs = 0x0,},
    { .regAddr = 0x3016, .regData = 0x08, .delayUs = 0x0,},
    { .regAddr = 0x3827, .regData = 0xec, .delayUs = 0x0,},
    { .regAddr = 0x3018, .regData = 0x44, .delayUs = 0x0,},
    { .regAddr = 0x3035, .regData = 0x21, .delayUs = 0x0,},
    { .regAddr = 0x3106, .regData = 0xf5, .delayUs = 0x0,},
    { .regAddr = 0x3034, .regData = 0x18, .delayUs = 0x0,},
    { .regAddr = 0x301c, .regData = 0xf8, .delayUs = 0x0,},
    { .regAddr = 0x5000, .regData = 0x86, .delayUs = 0x0,},////////////////////*lens setting*/
    { .regAddr = 0x5800, .regData = 0x11, .delayUs = 0x0,},
    { .regAddr = 0x5801, .regData = 0x0c, .delayUs = 0x0,},
    { .regAddr = 0x5802, .regData = 0x0a, .delayUs = 0x0,},
    { .regAddr = 0x5803, .regData = 0x0b, .delayUs = 0x0,},
    { .regAddr = 0x5804, .regData = 0x0d, .delayUs = 0x0,},
    { .regAddr = 0x5805, .regData = 0x13, .delayUs = 0x0,},
    { .regAddr = 0x5806, .regData = 0x09, .delayUs = 0x0,},
    { .regAddr = 0x5807, .regData = 0x05, .delayUs = 0x0,},
    { .regAddr = 0x5808, .regData = 0x03, .delayUs = 0x0,},
    { .regAddr = 0x5809, .regData = 0x03, .delayUs = 0x0,},
    { .regAddr = 0x580a, .regData = 0x06, .delayUs = 0x0,},
    { .regAddr = 0x580b, .regData = 0x08, .delayUs = 0x0,},
    { .regAddr = 0x580c, .regData = 0x05, .delayUs = 0x0,},
    { .regAddr = 0x580d, .regData = 0x01, .delayUs = 0x0,},
    { .regAddr = 0x580e, .regData = 0x00, .delayUs = 0x0,},
    { .regAddr = 0x580f, .regData = 0x00, .delayUs = 0x0,},
    { .regAddr = 0x5810, .regData = 0x02, .delayUs = 0x0,},
    { .regAddr = 0x5811, .regData = 0x06, .delayUs = 0x0,},
    { .regAddr = 0x5812, .regData = 0x05, .delayUs = 0x0,},
    { .regAddr = 0x5813, .regData = 0x01, .delayUs = 0x0,},
    { .regAddr = 0x5814, .regData = 0x00, .delayUs = 0x0,},
    { .regAddr = 0x5815, .regData = 0x00, .delayUs = 0x0,},
    { .regAddr = 0x5816, .regData = 0x02, .delayUs = 0x0,},
    { .regAddr = 0x5817, .regData = 0x06, .delayUs = 0x0,},
    { .regAddr = 0x5818, .regData = 0x09, .delayUs = 0x0,},
    { .regAddr = 0x5819, .regData = 0x05, .delayUs = 0x0,},
    { .regAddr = 0x581a, .regData = 0x04, .delayUs = 0x0,},
    { .regAddr = 0x581b, .regData = 0x04, .delayUs = 0x0,},
    { .regAddr = 0x581c, .regData = 0x06, .delayUs = 0x0,},
    { .regAddr = 0x581d, .regData = 0x09, .delayUs = 0x0,},
    { .regAddr = 0x581e, .regData = 0x11, .delayUs = 0x0,},
    { .regAddr = 0x581f, .regData = 0x0c, .delayUs = 0x0,},
    { .regAddr = 0x5820, .regData = 0x0b, .delayUs = 0x0,},
    { .regAddr = 0x5821, .regData = 0x0b, .delayUs = 0x0,},
    { .regAddr = 0x5822, .regData = 0x0d, .delayUs = 0x0,},
    { .regAddr = 0x5823, .regData = 0x13, .delayUs = 0x0,},
    { .regAddr = 0x5824, .regData = 0x22, .delayUs = 0x0,},
    { .regAddr = 0x5825, .regData = 0x26, .delayUs = 0x0,},
    { .regAddr = 0x5826, .regData = 0x26, .delayUs = 0x0,},
    { .regAddr = 0x5827, .regData = 0x24, .delayUs = 0x0,},
    { .regAddr = 0x5828, .regData = 0x24, .delayUs = 0x0,},
    { .regAddr = 0x5829, .regData = 0x24, .delayUs = 0x0,},
    { .regAddr = 0x582a, .regData = 0x22, .delayUs = 0x0,},
    { .regAddr = 0x582b, .regData = 0x20, .delayUs = 0x0,},
    { .regAddr = 0x582c, .regData = 0x22, .delayUs = 0x0,},
    { .regAddr = 0x582d, .regData = 0x26, .delayUs = 0x0,},
    { .regAddr = 0x582e, .regData = 0x22, .delayUs = 0x0,},
    { .regAddr = 0x582f, .regData = 0x22, .delayUs = 0x0,},
    { .regAddr = 0x5830, .regData = 0x42, .delayUs = 0x0,},
    { .regAddr = 0x5831, .regData = 0x22, .delayUs = 0x0,},
    { .regAddr = 0x5832, .regData = 0x02, .delayUs = 0x0,},
    { .regAddr = 0x5833, .regData = 0x24, .delayUs = 0x0,},
    { .regAddr = 0x5834, .regData = 0x22, .delayUs = 0x0,},
    { .regAddr = 0x5835, .regData = 0x22, .delayUs = 0x0,},
    { .regAddr = 0x5836, .regData = 0x22, .delayUs = 0x0,},
    { .regAddr = 0x5837, .regData = 0x26, .delayUs = 0x0,},
    { .regAddr = 0x5838, .regData = 0x42, .delayUs = 0x0,},
    { .regAddr = 0x5839, .regData = 0x26, .delayUs = 0x0,},
    { .regAddr = 0x583a, .regData = 0x06, .delayUs = 0x0,},
    { .regAddr = 0x583b, .regData = 0x26, .delayUs = 0x0,},
    { .regAddr = 0x583c, .regData = 0x24, .delayUs = 0x0,},
    { .regAddr = 0x583d, .regData = 0xce, .delayUs = 0x0,},/////////////////////* manual AWB, manual AE, close Lenc, open WBC*/
    { .regAddr = 0x3503, .regData = 0x03, .delayUs = 0x0,}, ///////////////////////*manual AE*/
    { .regAddr = 0x3501, .regData = 0x10, .delayUs = 0x0,},
    { .regAddr = 0x3502, .regData = 0x80, .delayUs = 0x0,},
    { .regAddr = 0x350a, .regData = 0x00, .delayUs = 0x0,},
    { .regAddr = 0x350b, .regData = 0x7f, .delayUs = 0x0,},
    { .regAddr = 0x5001, .regData = 0x01, .delayUs = 0x0,}, ///////////////////////*manual AWB*/
    { .regAddr = 0x5180, .regData = 0x08, .delayUs = 0x0,},
    { .regAddr = 0x5186, .regData = 0x04, .delayUs = 0x0,},
    { .regAddr = 0x5187, .regData = 0x00, .delayUs = 0x0,},
    { .regAddr = 0x5188, .regData = 0x04, .delayUs = 0x0,},
    { .regAddr = 0x5189, .regData = 0x00, .delayUs = 0x0,},
    { .regAddr = 0x518a, .regData = 0x04, .delayUs = 0x0,},
    { .regAddr = 0x518b, .regData = 0x00, .delayUs = 0x0,},
    { .regAddr = 0x5000, .regData = 0x06, .delayUs = 0x20000,}, ///////////////////////*No lenc, WBC on  delay 20 ms*/
    /*1280*960 Reference Setting 24M MCLK 2lane 280Mbps/lane (47.9fps)30fps for back to preview*//*h time = 20.864us*/

    //////////////////////// Modified by wsh for running on Arena.
    //////////////////////   {0x3035, .regData = 0x21}, // Noted by wsh: [7:4]: system_clk_div.
                          //               [3:0]: scale divide mipi. MIPI PCLK/SERCLK can be slowed down when image scaling down.
    { .regAddr = 0x3035, .regData = 0x21, .delayUs = 0x0,},

    ////////////////////// Modified by wsh for running on Arena.
    //////////////////////{0x3036, .regData = 0x37}, // Noted by wsh: PLL multiplier. Any int in 4~127 or even int in 128~252.
    { .regAddr = 0x3036, .regData = 0x37, .delayUs = 0x0,},
    { .regAddr = 0x3821, .regData = 0x07, .delayUs = 0x0,}, // Noted by wsh: [2]: r_mirror_isp, [1]: r_mirror_snr, [0]: r_hbin.
    { .regAddr = 0x3820, .regData = 0x41, .delayUs = 0x0,}, // Noted by wsh: [2]: r_vflip_isp, [1]: r_vflip_snr, [0]: r_vbin.
    { .regAddr = 0x3612, .regData = 0x09, .delayUs = 0x0,},
    { .regAddr = 0x3618, .regData = 0x00, .delayUs = 0x0,},
    { .regAddr = 0x380c, .regData = 0x07, .delayUs = 0x0,}, // Noted by wsh: [4:0]: Hsize[12:8]. 0x0768=1896.
    { .regAddr = 0x380d, .regData = 0x68, .delayUs = 0x0,}, // Noted by wsh: [7:0]: Hsize[7:0]. Total horizontal size.
    { .regAddr = 0x380e, .regData = 0x03, .delayUs = 0x0,}, // Noted by wsh: [1:0]: Vsize[9:8]. 0x3d8=984.
    { .regAddr = 0x380f, .regData = 0xd8, .delayUs = 0x0,}, // Noted by wsh: [7:0]: Vsize[7:0]. Total vertical size.
    { .regAddr = 0x3814, .regData = 0x31, .delayUs = 0x0,},
    { .regAddr = 0x3815, .regData = 0x31, .delayUs = 0x0,},
    { .regAddr = 0x3709, .regData = 0x52, .delayUs = 0x0,},

    { .regAddr = 0x3808, .regData = 0x05, .delayUs = 0x0,},  // Noted by wsh: [3:0]: DVP output Hsize[11:8]. 0x0500=1280.
    { .regAddr = 0x3809, .regData = 0x00, .delayUs = 0x0,},  // Noted by wsh: [7:0]: DVP output Hsize[7:0]
    { .regAddr = 0x380a, .regData = 0x03, .delayUs = 0x0,},  // Noted by wsh: [3:0]: DVP output Vsize[11:8]. 0x03c0=960.
    { .regAddr = 0x380b, .regData = 0xc0, .delayUs = 0x0,},  // Noted by wsh: [7:0]: DVP output Vsize[7:0]

    { .regAddr = 0x3800, .regData = 0x00, .delayUs = 0x0,},  // Noted by wsh: [3:0]: x_addr_start[11:8].
    { .regAddr = 0x3801, .regData = 0x18, .delayUs = 0x0,},  // Noted by wsh: [7:0]: x_addr_start[7:0].
    { .regAddr = 0x3802, .regData = 0x00, .delayUs = 0x0,},  // Noted by wsh: [3:0]: y_addr_start[11:8].
    { .regAddr = 0x3803, .regData = 0x0e, .delayUs = 0x0,},  // Noted by wsh: [7:0]: y_addr_start[7:0].
    { .regAddr = 0x3804, .regData = 0x0a, .delayUs = 0x0,},  // Noted by wsh: [3:0]: x_addr_end[11:8]. 0x0a27=2599
    { .regAddr = 0x3805, .regData = 0x27, .delayUs = 0x0,},  // Noted by wsh: [7:0]: x_addr_end[7:0].
    { .regAddr = 0x3806, .regData = 0x07, .delayUs = 0x0,},  // Noted by wsh: [3:0]: y_addr_end[11:8]. 0x0795=1941
    { .regAddr = 0x3807, .regData = 0x95, .delayUs = 0x0,},  // Noted by wsh: [7:0]: y_addr_end[7:0].
    { .regAddr = 0x4004, .regData = 0x02, .delayUs = 0x0,},  // Noted by wsh: [7:0]: BLC line number.

    { .regAddr = 0x3500, .regData = 0x00, .delayUs = 0x0,},  // exp h

    { .regAddr = 0x380e, .regData = 0x07, .delayUs = 0x0,},  // 30fps
    { .regAddr = 0x380f, .regData = 0xdd, .delayUs = 0x0,},  //
};


static drv_sensor_reg_info_t  ov5647_streamOn_setting[] =
{
    { .regAddr = 0x0100, .regData = 0x01, .delayUs = 20000,},
    { .regAddr = 0x4800, .regData = 0x04, .delayUs = 20000,},
    { .regAddr = 0x4202, .regData = 0x00, .delayUs = 20000,},
};

static drv_sensor_reg_info_t  ov5647_streamOff_setting[] =
{
    { .regAddr = 0x4800, .regData = 0x25, .delayUs = 20000,},
};

static drv_sensor_exp_param_t ov5647_normal_exparams =
{
    .maxIntegrationTime = 0x3900  ,
    .minIntegrationTime = 1       ,
    .maxAnalogGain      = 0xffe8  ,
    .minAnalogGain      = 0x100   ,
    .lineTime           = 135790  ,
    .limitShortMid      = 0       ,
    .limitLong          = 0       ,
    .expLineMargin      = 1       ,
};

static drv_power_setting_t ov5647_sensor0_power_up_setting[] = {
    {.type = DRV_POWER_REG, .regAddr = 0x83c40004, .bitMask  = 0x01, .bitVal = 0x00, .delayUs = 100000}, /// 100ms
    {.type = DRV_POWER_REG, .regAddr = 0x83c40004, .bitMask  = 0x01, .bitVal = 0x01, .delayUs = 100000}
};


static drv_power_setting_t ov5647_sensor0_power_down_setting[] = {
    {.type = DRV_POWER_REG, .regAddr = 0x83c40004, .bitMask  = 0x01, .bitVal = 0x00, .delayUs = 100000},
    {.type = DRV_POWER_REG, .regAddr = 0x83c40004, .bitMask  = 0x01, .bitVal = 0x01, .delayUs = 100000}
};

static drv_power_setting_t ov5647_sensor1_power_up_setting[] = {
    {.type = DRV_POWER_REG, .regAddr = 0x83c40044, .bitMask  = 0x01, .bitVal = 0x00, .delayUs = 100000}, /// 100ms
    {.type = DRV_POWER_REG, .regAddr = 0x83c40044, .bitMask  = 0x01, .bitVal = 0x01, .delayUs = 100000}
};


static drv_power_setting_t ov5647_sensor1_power_down_setting[] = {
    {.type = DRV_POWER_REG, .regAddr = 0x83c40044, .bitMask  = 0x01, .bitVal = 0x00, .delayUs = 100000},
    {.type = DRV_POWER_REG, .regAddr = 0x83c40044, .bitMask  = 0x01, .bitVal = 0x01, .delayUs = 100000}
};

static drv_power_setting_t ov5647_sensor2_power_up_setting[] = {
    {.type = DRV_POWER_REG, .regAddr = 0x83c400b4, .bitMask  = 0x01, .bitVal = 0x00, .delayUs = 100000}, /// 100ms
    {.type = DRV_POWER_REG, .regAddr = 0x83c400b4, .bitMask  = 0x01, .bitVal = 0x01, .delayUs = 100000}
};


static drv_power_setting_t ov5647_sensor2_power_down_setting[] = {
    {.type = DRV_POWER_REG, .regAddr = 0x83c400b4, .bitMask  = 0x01, .bitVal = 0x00, .delayUs = 100000},
    {.type = DRV_POWER_REG, .regAddr = 0x83c400b4, .bitMask  = 0x01, .bitVal = 0x01, .delayUs = 100000}
};


static drv_power_setting_t ov5647_sensor3_power_up_setting[] = {
    {.type = DRV_POWER_REG, .regAddr = 0x83c400c4, .bitMask  = 0x01, .bitVal = 0x00, .delayUs = 100000}, /// 100ms
    {.type = DRV_POWER_REG, .regAddr = 0x83c400c4, .bitMask  = 0x01, .bitVal = 0x01, .delayUs = 100000}
};


static drv_power_setting_t ov5647_sensor3_power_down_setting[] = {
    {.type = DRV_POWER_REG, .regAddr = 0x83c400c4, .bitMask  = 0x01, .bitVal = 0x00, .delayUs = 100000},
    {.type = DRV_POWER_REG, .regAddr = 0x83c400c4, .bitMask  = 0x01, .bitVal = 0x01, .delayUs = 100000}
};

static sensor_drv_params_t ov5647_sensor_params =
{
    .sensorId     = 0x56,
    .sensorIdAddr = 0x300a,
    .name         = "ov5647",
    .slaveInfo =
    {
        .hwType      = DRV_HW_I2C,
        .slaveAddr   = (0x6C >> 1),
        .channel     = 0,
        .regDataType = DRV_ADDR_TYPE_BYTE,
        .regAddrType = DRV_DATA_TYPE_WORD,
    },
    .powerUpSettings  =
    {
        .powerSetting = ov5647_sensor0_power_up_setting,
        .size = sizeof(ov5647_sensor0_power_up_setting) / sizeof(ov5647_sensor0_power_up_setting[0]),
    },
    .powerDownSettings =
    {
        .powerSetting  = ov5647_sensor0_power_down_setting,
        .size = sizeof(ov5647_sensor0_power_down_setting) / sizeof(ov5647_sensor0_power_down_setting[0]),
    },
    .activesize =
    {
        .width  = 1280,
        .height = 960
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
        .minFps = 30,
        .frameLength = 0x7dd, /// not used
        .expLineMargin = 1,
        .dim =
        {
            .width  = 1280,
            .height = 960
        },
        .hdrMode   = SENSOR_NORMAL_MODE,
        .regConfig =
        {
            .regDataType = DRV_DATA_TYPE_BYTE,
            .regAddrType = DRV_ADDR_TYPE_WORD,
            .direction   = DRV_DIRECTION_WRITE,
            .settings    = ov5647_normal_setting,
            .settingSize = sizeof(ov5647_normal_setting) / sizeof (ov5647_normal_setting[0]),

        },
        .expParams   = &ov5647_normal_exparams,
    },

    .streamOnSetting =
    {
        .regDataType = DRV_DATA_TYPE_BYTE,
        .regAddrType = DRV_ADDR_TYPE_WORD,
        .direction   = DRV_DIRECTION_WRITE,
        .settings    = ov5647_streamOn_setting,
        .settingSize = sizeof(ov5647_streamOn_setting) / sizeof (ov5647_streamOn_setting[0]),
    },

    .streamOffSetting =
    {
        .regDataType = DRV_DATA_TYPE_BYTE,
        .regAddrType = DRV_ADDR_TYPE_WORD,
        .direction   = DRV_DIRECTION_WRITE,
        .settings    = ov5647_streamOff_setting,
        .settingSize = sizeof(ov5647_streamOff_setting) / sizeof (ov5647_streamOff_setting[0]),
    },

    .fill_exposure_reg = OV5647_Fill_Exposure_Reg,
};

sensor_drv_params_t* Load_Ov5647_Sensor0_Driver(void);
sensor_drv_params_t* Load_Ov5647_Sensor1_Driver(void);
sensor_drv_params_t* Load_Ov5647_Sensor2_Driver(void);
sensor_drv_params_t* Load_Ov5647_Sensor3_Driver(void);

#endif

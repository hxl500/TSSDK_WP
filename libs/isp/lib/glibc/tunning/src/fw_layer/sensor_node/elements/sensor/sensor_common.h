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

#ifndef __SENSOR_COMMON_H__
#define __SENSOR_COMMON_H__

#include "types_utils.h"
#include "global_def.h"
#include "log_utils.h"
#include "fw_param_types.h"
#include "isp_hw_if.h"
#include "hal_types.h"

struct expUpdateFlag
{
    u8 isFpsUpdate        :1;
    u8 isLongExpUpdate    :1;
    u8 isMidExpUpdate     :1;
    u8 isShortExpUpdate   :1;
    u8 isSensorGainUpdate :1;
    u8 reserved           :3;
};

/// Define sensor exposure parameters required by driver every frame
typedef struct
{
    u32                     defaultFrameLength;     /// Defualt frame length lines
    u32                     frameLengthLines;       /// Lines of every frame length
    u32                     analogGain;             /// analog gain of sensor, DOL farmeID 0 sensor gain
    u32                     analogGain1;            /// DOL farmeID 1 sensor gain
    u32                     analogGain2;            /// DOL farmeID 2 sensor gain
    u32                     analogGain3;            /// DOL farmeID 3 sensor gain
    u32                     gainIndividualEn;       /// DOL sensor gain individual enable 0: all frame id use analogGain, 1: frame id 0 use analogGain and frame id1/2/3 use analogGain1/2/3
    u32                     expLineLong;            /// Long expouse line of sensor
    u32                     expLineMid;             /// Mid expouse line of sensor
    u32                     expLineShort;           /// Short expouse line of sensor
    u32                     expLineVShort;          /// Vshort expouse line of sensor
    f32                     maxFps;                 /// maximun of fps
    f32                     minFps;                 /// minimun of fps
    u32                     hdrMode;                /// Current hdr mode
    struct expUpdateFlag    flag;                   /// flag of expouse parameters update(to be used)
    u32                     isFpsUpdate;            /// minimun of fps
    u32                     currFps;                /// current fps

	u32						minAgain;
	u32						maxAgain;
	u32						minDgain;
	u32						maxDgain;
	u32						minIspgain;
	u32						maxIspgain;
	u32						minSysgain;
	u32						maxSysgain;

	u32						lastAgain;
	u32						lastDgain;
	u32						lastIspgain;
	u32						lastSysgain;
	u32						lastExpLongUS;
    u32                     last_exptime_min;
    u32                     last_exptime_max;
} sensor_apply_exp_param_t;


/// Define sensor driver exposure parameter required by turning
typedef struct
{
    u32  maxIntegrationTime;    /// Maximun of integration time(lines)
    u32  minIntegrationTime;    /// Minimun of integration time(lines)
    u32  maxCurIntegrationTime; /// Maximun of cur fps integration time(lines)
    u32  maxAnalogGain;         /// Q10 1x: 0x400
    u32  minAnalogGain;         /// 1x
    u32  maxTotalGain;         /// Q10 1x: 0x400
    u32  minTotalGain;         /// 1x
    u32  lineTime;              /// 1us: 4096
    u32  limitShortMid;         /// S + M limitation for DOL3
    u32  limitLong;             /// Long limitation
    u32  expLineMargin;         /// expouse lines margin, need update to ae
} drv_sensor_exp_param_t;


/// define sensor csi2 interface information
#define SENSOR_LANES_NUM_MAX (4)

struct dphy_lane {
	u8 pos;
	u8 pol;
};

struct csi2_lanes_cfg {
	struct dphy_lane clk;
	struct dphy_lane dat[SENSOR_LANES_NUM_MAX];
	u8 dat_num;
};

struct pix_interface {
	struct csi2_lanes_cfg ln_cfg;
	
	f32 link_freq;
	f32 pixel_rate;
};
typedef struct pix_interface pix_interface_t;

/// Define sensor mode setting information
typedef struct
{
    f32                     minFps;              /// Minimun of fps
    f32                     maxFps;              /// Maximun of fps
    u32                     hdrMode;             /// Hdr mode
    dim_t                   dim;                 /// width/height of image
    u32                     frameLength;         /// frameLength =  expouse lines + expLineMargin
    u32                     expLineMargin;       /// expouse lines margin
    u32                     lengthPixcel;        /// to be used
    drv_sensor_exp_param_t *expParams;           /// Exposure parameter required by turning
    drv_regs_array_info_t   regConfig;           /// Register configuration of mode setting

    pix_interface_t intf;
} drv_sensor_modesetting_t;


/// Define slave device information
typedef struct
{
    hw_type_t      hwType;            /// Hardware type(I2C/SPI/..)
    u32            slaveAddr;         /// Slave device address(Only used for I2c )
    u8             channel;           /// Device channel
    drv_data_type  regDataType;       /// Register data type
    drv_addr_type  regAddrType;       /// Register address type
} drv_slave_info_t;

typedef isp_ret_t (*fill_exposure_reg_func)(sensor_apply_exp_param_t * , drv_regs_array_info_t * );

typedef struct
{
    drv_slave_info_t            slaveInfo;                            /// Slave device information
    char8*                      name;                                 /// Sensor name
    u32                         sensorId;                             /// Sensor ID
    u32                         sensorIdAddr;                         /// Sensor ID address
    drv_power_settings_t        powerUpSettings;                      /// Power up setting
    drv_power_settings_t        powerDownSettings;                    /// Power down setting
    drv_regs_array_info_t       initSettings;                         /// Initial register setting of sensor
    drv_sensor_modesetting_t    modeSettings[DRV_SENSOR_SETTING_MAX]; /// Sensor mode setting information(register/fps/...)
    dim_t                       activesize;                           /// Sensor active size (pixel)
    drv_regs_array_info_t       streamOnSetting;                      /// Stream on register configuation
    drv_regs_array_info_t       streamOffSetting;                     /// Stream off register configuation
    fill_exposure_reg_func      fill_exposure_reg ;                   /// Driver callback Function of fill exposure register

} sensor_drv_params_t;

typedef sensor_drv_params_t* (*load_sensor_drv_func)();


#endif

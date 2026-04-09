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

#ifndef __ACTUATOR_COMMON_H__
#define __ACTUATOR_COMMON_H__

#include "types_utils.h"
#include "sensor_common.h"

#define ACTUAOTR_RESET_OFFSET 6

typedef enum
{
    MOTOR_STEPPER_UNINITIALIZED   = 0,
    MOTOR_STEPPER_RESET           = 1,
    MOTOR_STEPPER_READY           = 2
} motor_state_type;

typedef enum
{
    DRV_ACTUATOR_INVALID              = 0,
    DRV_ACTUATOR_VCM_FOCUS            = 1,
    DRV_ACTUATOR_VCM_FOCUS_ZOOM       = 2,
    DRV_ACTUATOR_STEPPER_FOCUS        = 3,
    DRV_ACTUATOR_STEPPER_FOCUS_ZOOM   = 4,
    DRV_ACTUATOR_IR_CUT               = 5,
} drv_actuator_type;

typedef struct
{
    u32 initPosition;
    u32 minPosition;
    u32 maxPosition;
} drv_focus_params_t;

typedef struct
{
    u32 initPosition;
    u32 minPosition;
    u32 maxPosition;
} drv_zoom_params_t;

typedef struct
{
    u32 initPosition;
    u32 initZoom;
} actuator_init_params_t;

typedef struct
{
    u32 nextFocusPos;
    u32 currFocusPos;
    u32 nextZoomPos;
    u32 currZoomPos;
    u32 irCutEnable;
} actuator_config_params_t;

typedef struct
{
    u32                     period;    /// period of pulse
    drv_power_settings_t    powerCfg;  /// pulse power configure
} actuator_plus_cfg_t;

typedef u32 (*actuator_init_Func)(actuator_init_params_t* , drv_regs_array_info_t *);
typedef u32 (*actuator_config_Func)(actuator_config_params_t* , drv_regs_array_info_t *);

typedef struct
{
    drv_slave_info_t            slaveInfo;            /// Slave information
    char8                      *name;                 /// Name of actuator
    drv_actuator_type           actuatorType;         /// Type of actuator (VCM / STEPPER / ...)
    drv_power_settings_t        powerUpSettings;      /// Power up setting
    drv_power_settings_t        powerDownSettings;    /// Power down setting
    drv_regs_array_info_t       initSettings;         /// Initialize setting
    actuator_plus_cfg_t        *pulseCfg;             /// pulse configure (only for stepper)
    actuator_init_Func          initFunc;             /// Actuator callback function for initialization
    actuator_config_Func        configFunc;           /// Actuator callback function for configuration
    drv_focus_params_t          focusParams;          /// Register configuration of focus(VCM) or one step of focus(STEPPER)
    drv_zoom_params_t           zoomParams;           /// Register configuration of zoom (VCM) or one step of zoom (STEPPER)
    actuator_config_Func        irCfgFunc;            /// irq cut callback function for configuration
} actuator_drv_params_t;

typedef actuator_drv_params_t* (*load_actuator_drv_func)();

#endif

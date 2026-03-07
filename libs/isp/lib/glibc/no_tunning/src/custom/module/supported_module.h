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

#ifndef __SUPPORTED_MODULE_H__
#define __SUPPORTED_MODULE_H__


#include "params_printer.h"
#include "params_to_json.h"
#include "camera_module.h"

typedef struct {
  isp_ret_t (* bind)(isp_pipe_mode_t, u32);
  isp_ret_t (* set)(isp_pipe_mode_t, camera_session_mode_t);
  camera_session_mode_t (* get)(isp_pipe_mode_t);
} supported_camera_module_ops_t;

typedef struct
{
    u32                     camId;
    load_sensor_drv_func    load_sensor;
    load_actuator_drv_func  load_actuator;
    // load_csi_drv_func       load_csi;
    load_csi2d_drv_func     load_csi2d;

    sensor_tuning_t         tuning[MAX_SENSOR_MODE];
    sensor_calibration_t    calibration;
} sensor_module_load_t;

typedef struct
{
    u32                    numOfModules;
    sensor_module_load_t  *pSensorModuleData;

    supported_camera_module_ops_t *ops;
} supported_camera_modules_t;

/*
* Get_Supported_Camera_Modules
*
* @brief get supported camera modules
*
* @params void

* @return pointer ot supported camera modules
*
*/
supported_camera_modules_t* Get_Supported_Camera_Modules(void);

#endif

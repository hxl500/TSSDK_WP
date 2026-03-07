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

#ifndef __FW_PARAM_TYPES_H__
#define __FW_PARAM_TYPES_H__

#include "stream_types.h"

typedef enum __awb_mode_t
{
    AWB_MODE_OFF = 0,
    AWB_MODE_AUTO,
    AWB_MODE_MWB,
} awb_mode_t;

typedef enum __aec_mode_t
{
    AEC_MODE_OFF = 0,
    AEC_MODE_AUTO,
} aec_mode_t;

typedef enum __af_mode_t
{
    AF_MODE_OFF = 0,
    AF_MODE_CONTINUE,
    AF_MODE_ONETIME,
    AF_MODE_EXTERN,
} af_mode_t;

typedef enum __motor_type_t
{
    motor_type_invalid = 0,
    motor_vcm_focus,
    motor_vcm_focus_zoom,
    motor_stepper_focus,
    motor_stepper_focus_zoom,
} motor_type_t;

/// Sensor Mode
typedef enum
{
    /// Never change the enum order!!!
    SENSOR_NORMAL_MODE    = 0,
    SENSOR_DOL2_MODE      = 1,
    SENSOR_DOL3_MODE      = 2,
    SENSOR_STITCH_3_MODE  = 3,
    SENSOR_STITCH_2_MODE  = 4,
    SENSOR_STAGGER_2_MODE  = 5,
    SENSOR_STAGGER_3_MODE  = 6,
    SENSOR_MAX_MODE,
} hdr_mode_t;

/// Multi camera mode
typedef enum
{
    SINGLE_CAM = 0,
    SINGLE_CAM_SPLIT,
    DUAL_CAM_LINEAR_LINEAR,
    DUAL_CAM_HDR2_LINEAR,
    TRIP_CAM_LINEAR_LINEAR,
    QUAD_CAM_LINEAR_LINEAR,
} multiply_cam_mode_t;

/// Multi camera display mode
typedef enum
{
    DISPLAY_SINGLE_CAM = 0,
    DISPLAY_DUAL_CAM   = 1,
} multiply_cam_display_mode_t;

/// Define antibanding mode
typedef enum
{
    ANTIBANDING_DISABLEE   = 0,
    ANTIBANDING_AUTO,
    ANTIBANDING_50HZ,
    ANTIBANDING_60HZ,
} antibanding_mode_t;

/// define dump group
typedef enum
{
    DUMP_GROUP_NONE      = 0,  /// Disable dump
    DUMP_GROUP_APP       = 1,  /// APP Layer Group
    DUMP_GROUP_HAL       = 2,  /// HAL Layer Group
    DUMP_GROUP_FW        = 3,  /// FW Layer Group
    DUMP_GROUP_SENSOR    = 4,  /// Sensor Group
    DUMP_GROUP_ISP       = 5,  /// ISP Group
    DUMP_GROUP_3A        = 6,  /// 3A Group
} dump_group_t;


typedef struct
{
    stream_fmt_t            streamFmt;
    u32                     width;        /// Width
    u32                     height;       /// Height
    u32                     hdrMode;
} hw_env_stream_cfg_t;


#define ISP_ALG_LIB_NAME_SIZE_MAX (32)
#define ISP_ALG_LIB_NUM_MAX (16)

typedef enum __isp_alg_type {
  ISP_ALG_TYPE_INVAL = 0,
  ISP_ALG_TYPE_3A_AEC = 1,
  ISP_ALG_TYPE_3A_AWB = 2,
  ISP_ALG_TYPE_3A_AFC = 3,
  ISP_ALG_TYPE_BOTM = ISP_ALG_LIB_NUM_MAX,
} isp_alg_lib_type_e;

typedef struct __isp_alg_lib_attr {
  char8 name[ISP_ALG_LIB_NAME_SIZE_MAX];
} isp_alg_lib_attr_t;

typedef struct __isp_alg_lib_bind {
  BOOL enable;
  isp_alg_lib_attr_t attr;
} isp_alg_lib_bind_t;

typedef struct __isp_alg_lib_reg {
  BOOL enable; //eret
  isp_alg_lib_attr_t attr;
  isp_alg_lib_type_e type;
} isp_alg_reg_t;

typedef struct __isp_alg_bind_attr {
  isp_alg_lib_bind_t bind[ISP_ALG_TYPE_BOTM];
} isp_alg_bind_t;

typedef enum {
  SW3A_COMB_TYPE_IVL = 0,
  SW3A_COMB_TYPE_STITCH = 1,
  SW3A_COMB_TYPE_HYBIRD = 2,
  SW3A_COMB_TYPE_BTM,
} sw3a_comb_type_t;

#define ISP_VIN_PIPE_NUM_MAX (4)
#define ISP_VIN_SPLIT_NUM_MAX (3)
#define ISP_VIN_DATA_RESERVED_NUM (16)

typedef struct {
  u32 ob1; u32 ob2;
  u32 l1; u32 l2;
  u32 m1; u32 m2;
  u32 s1; u32 s2;
} isp_cis_identity_code_t;

typedef struct {
  u32 fbc_en;
  u32 outstand;
  u32 try_online;
  isp_cis_identity_code_t idcode;
} isp_vin_rtpipe_t;

typedef struct {
  u32 sync_htotal_margin;
  u32 sync_vtotal_margin;
} isp_vin_retiming_t;

typedef struct {
  u32 ich_matrix;
  u32 long_first;
  u32 burst_len;
  u32 frame_mask_num;
  u32 frame_stitch_output_mode;
  u32 pclk;
} isp_vin_ctrl_t;

typedef struct {
  // u32 num;
  u32 width;
  struct {
    u32 padf;
    u32 pade;
  } sub[ISP_VIN_SPLIT_NUM_MAX];
} isp_vin_split_t;

typedef struct {
  u32 m_ctrl :1;
  u32 m_retiming :1;
  u32 m_rtpipe :1;
} isp_vin_field_t;

typedef struct {
  isp_vin_ctrl_t ctrl;
  isp_vin_retiming_t retiming;
  isp_vin_rtpipe_t rtpipe[ISP_VIN_PIPE_NUM_MAX];
  isp_vin_split_t split;
  u32 reserved[ISP_VIN_DATA_RESERVED_NUM];
  isp_vin_field_t field;
} isp_vin_data_t;

typedef enum {
  ISP_USR_PREF_SCV_BRIGHTNESS = 0,
  ISP_USR_PREF_SCV_CONTRAST = 1,
  ISP_USR_PREF_SCV_SATURATION = 2,
  ISP_USR_PREF_SCV_SHARPNESS = 3,
  ISP_USR_PREF_SCV_CHROMINANCE = 4,
  ISP_USR_PREF_SCV_RAWDENOISE = 5,
  ISP_USR_PREF_SCV_YUVDENOISE = 6,
  ISP_USR_PREF_SCV_NUM,
} isp_user_preference_type_t;

typedef struct {
  struct {
    u32 strength;
  } scv_rows[ISP_USR_PREF_SCV_NUM];
} isp_user_preference_t;

typedef enum {
  FW_EVENT_INVAL = 0x0000,
  FW_EVENT_BASE = 0x1000,
  FW_ISP_EVENT_BASE = 0x2000,
  FW_ISP_EVENT_VIN_DUMP_SCENE = (FW_ISP_EVENT_BASE + 0x00),
  FW_ISP_EVENT_VIN_DUMP_RAW = (FW_ISP_EVENT_BASE + 0x01),
  FW_AAA_EVENT_BASE = 0x3000,
  FW_AAA_EVENT_FRAME_DATA_UPDATE = (FW_AAA_EVENT_BASE + 0x00),
} fw_event_t;

typedef isp_ret_t (* fw_event_func_t)(fw_event_t, void *, void *);
typedef struct {
  fw_event_func_t func;
  void *ctx;
} fw_event_cb_t;
#endif

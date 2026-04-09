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

#ifndef __METADATA_H__
#define __METADATA_H__

#include "types_utils.h"
#include "log_utils.h"
#include "mem_osutils.h"
#include "obj_queue_utils.h"
#include "stream_types.h"

/// Define antibanding mode
typedef enum __meta_antibanding_mode
{
    META_ANTIBANDING_DISABLEE   = 0,
    META_ANTIBANDING_AUTO,
    META_ANTIBANDING_50HZ,
    META_ANTIBANDING_60HZ,
} meta_antibanding_mode_t;

/// Define HDR mode
typedef enum __meta_hdr_mode
{
    META_HDR_MODE_DISABLE   = 0,
    META_HDR_DOL2_MODE      = 1,
    META_HDR_DOL3_MODE      = 2,
    META_HDR_STITCH_3_MODE  = 3,
    META_HDR_STITCH_2_MODE  = 4,
    META_HDR_STAGGER_2_MODE  = 5,
    META_HDR_STAGGER_3_MODE  = 6,
    META_HDR_MAX_MODE,
} meta_hdr_mode_t;

/// Define aec mode
typedef enum __meta_aec_mode_t
{
    META_AEC_MODE_OFF = 0,
    META_AEC_MODE_AUTO,
} meta_aec_mode_t;

/// Define awb mode
typedef enum __meta_awb_mode_t
{
    META_AWB_MODE_OFF = 0,
    META_AWB_MODE_AUTO,
    META_AWB_MODE_MWB,
} meta_awb_mode_t;

/// Define af mode
typedef enum __meta_af_mode_t
{
    META_AF_MODE_OFF = 0,
    META_AF_MODE_CONTINUE,
    META_AF_MODE_ONETIME,
    META_AF_MODE_EXTERN,
} meta_af_mode_t;

/// Define multiply camera mode
typedef enum __meta_multiply_cam_mode_t
{
    META_MULTICAM_MODE_SINGLE_CAM = 0,
    META_MULTICAM_MODE_SINGLE_CAM_SPLIT,
    META_MULTICAM_MODE_DUAL_CAM_LINEAR_LINEAR,
    META_MULTICAM_MODE_DUAL_CAM_HDR2_LINEAR,
    META_MULTICAM_MODE_TRIP_CAM_LINEAR_LINEAR,
    META_MULTICAM_MODE_QUAD_CAM_LINEAR_LINEAR,
} meta_multiply_cam_mode_t;

/// define stream config struct
typedef struct __metadata_stream_config
{
    f32                           fps; /// todo: range
    u32                           inputStreamNum;
    stream_info_t                 inputStreamInfo[BUF_STREAM_IDX_MAX];
    u32                           outputStreamNum;
    stream_info_t                 outputStreamInfo[BUF_STREAM_IDX_MAX];
} metadata_stream_config_t;

/// Define metadata capability params
typedef struct __metadata_capability
{
    u32                     camId;
    range_t                 supportedFps;
    range_t                 supportedWidth;
    range_t                 supportedHeight;
    s8                      supportedFmt[128];
    range_t                 supportedZoom;
} metadata_capability_t;

/// Define metadata event status
typedef enum __metadata_evt_status
{
    META_EVT_STATUS_INVALID = 0,
    META_EVT_STATUS_PROCESSING,
    META_EVT_STATUS_PROCESSED,
    META_EVT_STATUS_MAX,
} metadata_evt_status_t;

/// Define metadata event status
typedef enum __metadata_evt_dirt
{
    META_EVT_DIRT_WRITE = 0,
    META_EVT_DIRT_READ,
    META_EVT_DIRT_MAX,
} metadata_evt_dirt_t;

/// Define metadata event struct
typedef struct __metadata_event
{
    metadata_evt_status_t       status;
    u32                         camId;
    //u32                       nodeId;
    //u32                       elementId;
    u32                         type;
    u32                         evtId;
    metadata_evt_dirt_t         direction;
    u32                         data;
    void *buffer;
    BOOL                        needRestart;
} metadata_event_t;


#define ALG_LIB_NAME_SIZE_MAX (32)
#define ALG_LIB_NUM_MAX (16)

typedef enum {
    META_CAM_SESSION_VISIBLE = 0,
    META_CAM_SESSION_IR = 1,
    META_CAM_SESSION_WHITELIGHT = 2,
    META_CAM_SESSION_DUMMY0,
    META_CAM_SESSION_DUMMY1,
    META_CAM_SESSION_DUMMY2,
    META_CAM_SESSION_DUMMY3,
    META_CAM_SESSION_DUMMY4,
    META_CAM_SESSION_DUMMY5,
    META_CAM_SESSION_DUMMY6,
    META_CAM_SESSION_NUM_MAX,
    META_CAM_SESSION_NUM_INV = META_CAM_SESSION_NUM_MAX,
} meta_camera_session_t;

typedef enum __hal_alg_lib_type {
  ALG_LIB_INV = 0,
  ALG_LIB_AEC = 1,
  ALG_LIB_AWB = 2,
  ALG_LIB_AFC = 3,
  ALG_LIB_BTM = ALG_LIB_NUM_MAX,
} alg_lib_type_e;

typedef struct __hal_alg_lib_attr {
  char8 name[ALG_LIB_NAME_SIZE_MAX];
} alg_lib_attr_t;

typedef struct __hal_alg_lib_bind {
  BOOL enable;
  alg_lib_attr_t attr;
} alg_lib_bind_t;

typedef enum {
  META_AAA_COMB_TYPE_IVL = 0,
  META_AAA_COMB_TYPE_STITCH = 1,
  META_AAA_COMB_TYPE_HYBIRD = 2,
  META_AAA_COMB_TYPE_BTM,
} meta_aaa_comb_type_t;

typedef struct __hal_alg_lib_reg {
  BOOL enable;
  alg_lib_attr_t attr;
  alg_lib_type_e type;
} meta_alg_reg_t;

typedef struct __alg_lib_bind_attr {
  alg_lib_bind_t bind[ALG_LIB_BTM];
} meta_alg_bind_t;

typedef enum {
  META_USR_PREF_SCV_BRIGHTNESS = 0,
  META_USR_PREF_SCV_CONTRAST = 1,
  META_USR_PREF_SCV_SATURATION = 2,
  META_USR_PREF_SCV_SHARPNESS = 3,
  META_USR_PREF_SCV_CHROMINANCE = 4,
  META_USR_PREF_SCV_RAWDENOISE = 5,
  META_USR_PREF_SCV_YUVDENOISE = 6,
  META_USR_PREF_SCV_NUM,
} meta_user_preference_type_t;

typedef struct {
  struct {
    u32 strength;
  } scv_rows[META_USR_PREF_SCV_NUM];
} meta_user_preference_t;

typedef	struct __usr_exposure_attr{
	range_t	again_range;	/// multiply with 0x400
	range_t	dgain_range;	/// multiply with 0x1000
	range_t ispgain_range;	/// multiply with 0x400
	range_t sysgain_range;	/// multiply with 0x400
	range_t exptime_range;	/// based util is us
}usr_exposure_attr_t;
/// Define metadata user settings params
typedef struct __metadata_user
{
    metadata_stream_config_t    streamCfg;        /// stream params to store steaming information
    meta_hdr_mode_t             hdrMode;          /// HDR mode
    meta_antibanding_mode_t     antibandingMode;  /// Antibanding mode
    BOOL                        asdEnable;        /// Auto-scene detection enable
    meta_aec_mode_t             aecMode;          /// aec mode
    meta_awb_mode_t             awbMode;          /// awb mode
    meta_af_mode_t              afMode;           /// af mode
    u32                         senTestMode;      /// sensor test mode
    u32                         maxSenGain;       /// max sensor gain

    meta_alg_reg_t              algreg;
    meta_alg_bind_t             algbind;
    meta_camera_session_t       camSession;
    meta_user_preference_t      preference;
    BOOL                        dual_cam_online_en;
    BOOL                        dual_cam_dynamic_fps;
	usr_exposure_attr_t			usr_exp_attr;
	range_t						fps_range;	/// multiply with 100
} metadata_user_t;

#define META_IFE_PIPE_NUM_MAX (4)
#define META_IFE_SPLIT_NUM_MAX (3)
#define META_IFE_DATA_RESERVED_NUM (16)

typedef struct {
  u32 ob1; u32 ob2;
  u32 l1; u32 l2;
  u32 m1; u32 m2;
  u32 s1; u32 s2;
} meta_cis_identity_code_t;

typedef struct {
  u32 fbc_en;
  u32 outstand;
  u32 try_online;
  meta_cis_identity_code_t idcode;
} meta_ife_rtpipe_t;

typedef struct {
  u32 sync_htotal_margin;
  u32 sync_vtotal_margin;
} meta_ife_retiming_t;

typedef struct {
  u32 ich_matrix;
  u32 long_first;
  u32 burst_len;
  u32 frame_mask_num;
  u32 frame_stitch_output_mode;
  u32 pclk;
} meta_ife_ctrl_t;

typedef struct {
  // u32 num;
  u32 width;
  struct {
    u32 padf;
    u32 pade;
  } sub[META_IFE_SPLIT_NUM_MAX];
} meta_ife_split_t;

typedef struct {
  u32 m_ctrl :1;
  u32 m_retiming :1;
  u32 m_rtpipe :1;
} meta_ife_field_t;

typedef struct {
  meta_ife_ctrl_t ctrl;
  meta_ife_retiming_t retiming;
  meta_ife_rtpipe_t rtpipe[META_IFE_PIPE_NUM_MAX];
  meta_ife_split_t split;
  u32 reserved[META_IFE_DATA_RESERVED_NUM];
  meta_ife_field_t field;
} meta_ife_data_t;

typedef struct __metadata_global
{
  u8                          activeCamNum;
  meta_multiply_cam_mode_t    multiCamMode;
  meta_aaa_comb_type_t        aaaCombine;
  meta_ife_data_t             ifeTune;
} metadata_global_t;

/// Define metadata result params
typedef struct __metadata_result
{
    u32 hwFrameNum;
} metadata_result_t;

/// Define metadata struct
typedef struct __metadata
{
    metadata_capability_t        capability;
    metadata_user_t              userSettings;
    obj_queue_t*                 eventQ;   /// event queue
    metadata_global_t            global;
    metadata_result_t            result;
} metadata_t;

/// Macro function to calculate member offset in struct
#define META_OFFSET(TYPE, MEMBER) ((INTPTR)(&((TYPE *)0)->MEMBER))

/// Define metadata tag ID
typedef enum __metadata_tag_offset
{
    /// Capability Metadata
    METADATA_CAP_REGION                 = META_OFFSET(metadata_t, capability),
    METADATA_CAP_CAMERA_ID              = METADATA_CAP_REGION + META_OFFSET(metadata_capability_t, camId),

    /// User settings Metadata
    METADATA_USER_REGION                = META_OFFSET(metadata_t, userSettings),
    METADATA_USER_STREAM_CFG            = METADATA_USER_REGION + META_OFFSET(metadata_user_t, streamCfg),
    METADATA_USER_HDR_MODE              = METADATA_USER_REGION + META_OFFSET(metadata_user_t, hdrMode),
    METADATA_USER_ANTIBANDING_MODE      = METADATA_USER_REGION + META_OFFSET(metadata_user_t, antibandingMode),
    METADATA_USER_ASD_ENABLE            = METADATA_USER_REGION + META_OFFSET(metadata_user_t, asdEnable),
    METADATA_USER_AEC_MODE              = METADATA_USER_REGION + META_OFFSET(metadata_user_t, aecMode),
    METADATA_USER_AWB_MODE              = METADATA_USER_REGION + META_OFFSET(metadata_user_t, awbMode),
    METADATA_USER_AF_MODE               = METADATA_USER_REGION + META_OFFSET(metadata_user_t, afMode),
    METADATA_USER_SEN_TEST_MODE         = METADATA_USER_REGION + META_OFFSET(metadata_user_t, senTestMode),
    METADATA_USER_MAX_SEN_GAIN          = METADATA_USER_REGION + META_OFFSET(metadata_user_t, maxSenGain),
    METADATA_USER_ALG_REG               = METADATA_USER_REGION + META_OFFSET(metadata_user_t, algreg),
    METADATA_USER_ALG_BIND              = METADATA_USER_REGION + META_OFFSET(metadata_user_t, algbind),
    METADATA_USER_CAM_SESSION           = METADATA_USER_REGION + META_OFFSET(metadata_user_t, camSession),
    METADATA_USER_PREFERENCE            = METADATA_USER_REGION + META_OFFSET(metadata_user_t, preference),
    METADATA_USER_DUALCAM_ONLINE        = METADATA_USER_REGION + META_OFFSET(metadata_user_t, dual_cam_online_en),
    METADATA_USER_DUALCAM_DYFPS           = METADATA_USER_REGION + META_OFFSET(metadata_user_t, dual_cam_dynamic_fps),

    METADATA_USER_ISP_USR_EXP           = METADATA_USER_REGION + META_OFFSET(metadata_user_t, usr_exp_attr),
    METADATA_USER_ISP_USR_FPS           = METADATA_USER_REGION + META_OFFSET(metadata_user_t, fps_range),

    /// Event Queue
    METADATA_EVENTQ                     = META_OFFSET(metadata_t, eventQ),

    /// Global region settings are same and shared for all camera
    METADATA_GLOBAL_REGION              = META_OFFSET(metadata_t, global),
    METADATA_GLOBAL_ACTIVE_CAM_NUM      = METADATA_GLOBAL_REGION + META_OFFSET(metadata_global_t, activeCamNum),
    METADATA_GLOBAL_MULTICAM_MODE       = METADATA_GLOBAL_REGION + META_OFFSET(metadata_global_t, multiCamMode),
    METADATA_GLOBAL_AAA_COMBINE         = METADATA_GLOBAL_REGION + META_OFFSET(metadata_global_t, aaaCombine),
    METADATA_GLOBAL_IFE_TUNE            = METADATA_GLOBAL_REGION + META_OFFSET(metadata_global_t, ifeTune),

    /// Result Metadata
    METADATA_RESULT_REGION              = META_OFFSET(metadata_t, result),
    METADATA_RESULT_HW_FRAME_NUM        = METADATA_RESULT_REGION + META_OFFSET(metadata_result_t, hwFrameNum),

    METADATA_MAX,
} metadata_tag_offset_t;

/*
* Metadata_Transfer_Event
*
* @brief Transfer metadata events
*
* @param pDstMeta: pointer to metadata_t, dst meta buffer
*
* @param pSrcMeta: pointer to metadata_t, src meta buffer
*
* @return void
*/
void Metadata_Transfer_Event(metadata_t* __restrict__ pDstMeta, metadata_t* __restrict__ pSrcMeta);


/*
* Metadata_Push_Event
*
* @brief Push event to metadata
*
* @param pMeta: pointer to metadata_t
*
* @param pEvt: pointer to event
*
* @return ISP_RET_SUCCESS if success, others if failed
*/
isp_ret_t Metadata_Push_Event(metadata_t* pMeta, metadata_event_t* pEvt);

/*
* Metadata_Pop_Event
*
* @brief Pop event from metadata
*
* @param pMeta: pointer to metadata_t
*
* @param pEvt: pointer to event
*
* @return ISP_RET_SUCCESS if success, others if failed
*/
isp_ret_t Metadata_Pop_Event(metadata_t* pMeta, metadata_event_t* pEvt);

/*
* Metadata_Set
*
* @brief Set data to metadata
*
* @param pMeta: pointer to metadata_t
*
* @param tag: metadata tag ID
*
* @param pData: pointer to void, data used to set
*
* @param size: data size
*
* @return ISP_RET_SUCCESS if success, others if failed
*/
isp_ret_t Metadata_Set(metadata_t* pMeta,
                           metadata_tag_offset_t tag,
                           void* pData,
                           size_t size);

/*
* Metadata_Get
*
* @brief Get data from metadata
*
* @param pMeta: pointer to metadata_t
*
* @param tag: metadata tag ID
*
* @return pointer to void, data acquired from metadata
*/
void* Metadata_Get(metadata_t* pMeta,
                      metadata_tag_offset_t tag);

/*
* Metadata_Copy
*
* @brief Copy metadata
*
* @param pDstMeta: pointer to metadata_t, dst meta buffer
*
* @param pSrcMeta: pointer to metadata_t, src meta buffer
*
* @return void
*/
void Metadata_Copy(metadata_t* __restrict__ pDstMeta, metadata_t* __restrict__ pSrcMeta);


/*
* Metadata_Create
*
* @brief Create metadata
*
* @return pointer to metadata_t
*/
metadata_t* Metadata_Create();

/*
* Metadata_Destroy
*
* @brief Destroy metadata
*
* @param ppMeta: dimensional pointer to metadata_t
*
* @return void
*/
void Metadata_Destroy(metadata_t** ppMeta);

/// Macro function to get pointer from metadata
#define META_GET_ADDR(META, TAG, TYPE) ((TYPE*)Metadata_Get(META, TAG))

/// Macro function to get value from metadata
#define META_GET_VALUE(META, TAG, TYPE) (*(META_GET_ADDR(META, TAG, TYPE)))

/// Macro function to set value to metadata
#define META_SET(META, TAG, VAR) (Metadata_Set(META, TAG, (void*)(&(VAR)), sizeof(VAR)))

#endif

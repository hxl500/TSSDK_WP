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
 * Copyright (C) 2020 - 2021 Arm Technology (China) Co. Ltd. All rights reserved.
 *
 ***********************************************************************************************/

#ifndef __FW_SPECIAL_API_I5_H__
#define __FW_SPECIAL_API_I5_H__

//Direction
#define SPECIAL_API_SET_VAL                             0x00000000
#define SPECIAL_API_GET_VAL                             0x00000001

//Parsing_Start
//Sensor 0x00-03
#define SPECIAL_API_SENSOR_START                         0x00000000
#define SPECIAL_API_SENSOR_HDR_MODE                      (SPECIAL_API_SENSOR_START + 0x00) // [0, 4], 1
//#define SPECIAL_API_SENSOR_RESOLUTION_MODE               (SPECIAL_API_SENSOR_START + 0x01) // [0, 1], 0
//#define SPECIAL_API_SENSOR_LOG_ENABLE                    (SPECIAL_API_SENSOR_START + 0x02) // [0, 2], 0
#define SPECIAL_API_SENSOR_EXPO_TIME_L_LINE              (SPECIAL_API_SENSOR_START + 0x03) // [0, 8191], 1
#define SPECIAL_API_SENSOR_EXPO_TIME_M_LINE              (SPECIAL_API_SENSOR_START + 0x04) // [0, 8191], 1
#define SPECIAL_API_SENSOR_EXPO_TIME_S_LINE              (SPECIAL_API_SENSOR_START + 0x05) // [0, 8191], 1
#define SPECIAL_API_SENSOR_EXPO_TIME_L_US                (SPECIAL_API_SENSOR_START + 0x06) // [0, 100000], 1
#define SPECIAL_API_SENSOR_EXPO_TIME_M_US                (SPECIAL_API_SENSOR_START + 0x07) // [0, 100000], 1
#define SPECIAL_API_SENSOR_EXPO_TIME_S_US                (SPECIAL_API_SENSOR_START + 0x08) // [0, 100000], 1
#define SPECIAL_API_SENSOR_GAIN                          (SPECIAL_API_SENSOR_START + 0x09) // [1024, 133444], 1
#define SPECIAL_API_SENSOR_GAIN_X                        (SPECIAL_API_SENSOR_START + 0x0A) // [100, 12800], 1
#define SPECIAL_API_SENSOR_MAX_FPS                       (SPECIAL_API_SENSOR_START + 0x0B) // [1, 60], 1
#define SPECIAL_API_SENSOR_MIN_FPS                       (SPECIAL_API_SENSOR_START + 0x0C) // [1, 60], 1

#define SPECIAL_API_SENSOR_END                          SPECIAL_API_SENSOR_MIN_FPS

//Digital Gain 0x04
#define SPECIAL_API_ISP_GAIN                             (SPECIAL_API_SENSOR_END + 0x01) // [4096, 1048575], 1
#define SPECIAL_API_ISP_GAIN_X                           (SPECIAL_API_SENSOR_END + 0x02) // [100, 3200], 1

//AE common
#define SPECIAL_API_AE_START                            (SPECIAL_API_ISP_GAIN_X + 0x01)
#define SPECIAL_API_AE_ENABLE                           (SPECIAL_API_AE_START + 0x00) // [0, 1], 1
#define SPECIAL_API_AE_LOG_ENABLE                       (SPECIAL_API_AE_START + 0x01) // [0, 2], 1
#define SPECIAL_API_AE_CURRENT_LUX_INDEX                (SPECIAL_API_AE_START + 0x03)  // [0, 1000], 1
/// AE converge
#define SPECIAL_API_AE_EXPOSURE_TABLE_ENABLE            (SPECIAL_API_AE_START + 0x04)  // [0, 1], 0
#define SPECIAL_API_AE_CONVERGE_STEP                    (SPECIAL_API_AE_START + 0x05)  // [1, 32], 1
#define SPECIAL_API_AE_MAX_CONVERGE_SPEED               (SPECIAL_API_AE_START + 0x06) // [30, 200],1
#define SPECIAL_API_AE_STABLE_IN_RANGE                  (SPECIAL_API_AE_START + 0x07)  // [2, 255], 1
#define SPECIAL_API_AE_STABLE_OUT_RANGE                 (SPECIAL_API_AE_START + 0x08)  // [5, 512],1
#define SPECIAL_API_AE_SATURATION_THR                   (SPECIAL_API_AE_START + 0x09) // [512, 1024],1
#define SPECIAL_API_AE_STATS_Y_SELECT_IDX               (SPECIAL_API_AE_START + 0x0A) // [0, 3],1
#define SPECIAL_API_AE_STABLE_TIMEOUT_FRAME             (SPECIAL_API_AE_START + 0x0B) // [0, 4096],1

/// AE target param interpolation by lux index
#define SPECIAL_API_AE_MANUAL_SET_PARAM_ENABLE          (SPECIAL_API_AE_START + 0x10) // [0, 1], 1
#define SPECIAL_API_AE_TARGET                           (SPECIAL_API_AE_START + 0x11)  // [0, 1023], 1
#define SPECIAL_API_AE_HIGH_CONTR_TARGET                (SPECIAL_API_AE_START + 0x12)  // [0, 100], 1
#define SPECIAL_API_AE_LOW_LEVEL                        (SPECIAL_API_AE_START + 0x13)  // [0, 100000], 1
#define SPECIAL_API_AE_HIGH_LEVEL                       (SPECIAL_API_AE_START + 0x14)  // [1, 100000], 1
#define SPECIAL_API_AE_LOW_CONTRAST                     (SPECIAL_API_AE_START + 0x15)  // [0, 1023], 1
#define SPECIAL_API_AE_HIGH_CONTRAST                    (SPECIAL_API_AE_START + 0x16)  // [0, 1023], 1
#define SPECIAL_API_AE_HIGH_CUT_MIN                     (SPECIAL_API_AE_START + 0x17)  // [1, 1023],0
#define SPECIAL_API_AE_HIGH_CUT_TARGET                  (SPECIAL_API_AE_START + 0x18)  // [1, 1023],0
#define SPECIAL_API_AE_LONG_TARGET                      (SPECIAL_API_AE_START + 0x19)  // [0, 100], 1
#define SPECIAL_API_AE_LONG_RATIO_MAX                   (SPECIAL_API_AE_START + 0x1A)  // [1, 1024], 1
#define SPECIAL_API_AE_MID_TARGET                       (SPECIAL_API_AE_START + 0x1B)  // [0, 100], 1
#define SPECIAL_API_AE_MID_RATIO_MAX                    (SPECIAL_API_AE_START + 0x1C)  // [1, 512], 1
#define SPECIAL_API_AE_HDR_SAT_MAX_PCENT                (SPECIAL_API_AE_START + 0x1D)  // [0, 102400], 1
#define SPECIAL_API_AE_ADJ_RATIO                        (SPECIAL_API_AE_START + 0x1E)  // [1, 100], 0

#define SPECIAL_API_AE_MANUAL_ROI_ENABLE                (SPECIAL_API_AE_START + 0x20)  // [0, 1], 1
#define SPECIAL_API_AE_MANUAL_ROI_X                     (SPECIAL_API_AE_START + 0x21)  // [0, 14], 1
#define SPECIAL_API_AE_MANUAL_ROI_Y                     (SPECIAL_API_AE_START + 0x22)  // [0, 14], 1
#define SPECIAL_API_AE_MANUAL_ROI_W                     (SPECIAL_API_AE_START + 0x23)  // [1, 15], 1
#define SPECIAL_API_AE_MANUAL_ROI_H                     (SPECIAL_API_AE_START + 0x24)  // [1, 15], 1
#define SPECIAL_API_AE_STATS_SEL                        (SPECIAL_API_AE_START + 0x25)  // [0, 3], 1
#define SPECIAL_API_AE_WEIGHT_TBL_SEL                   (SPECIAL_API_AE_START + 0x26)  // [0, 5], 1
#define SPECIAL_API_AE_HDR_EXP_BASE_SEL                 (SPECIAL_API_AE_START + 0x27)  // [0, 1], 0
#define SPECIAL_API_AE_HL_SUP                           (SPECIAL_API_AE_START + 0x28)  // [0, 1], 1
#define SPECIAL_API_AE_STITCH_SKIP_FRAME                (SPECIAL_API_AE_START + 0x29)  // [0, 10], 0
#define SPECIAL_API_AE_AAO_DUMP                         (SPECIAL_API_AE_START + 0x2A)  // [0, 65536], 0
#define SPECIAL_API_AE_DOL2_LUX_USE_L_FRAME_EN          (SPECIAL_API_AE_START + 0x2B)  // [0, 1], 0
#define SPECIAL_API_AE_DOL2_LUX_ISP_GAIN_THR            (SPECIAL_API_AE_START + 0x2C)  // [32768, 1048576], 0
#define SPECIAL_API_AE_DOL_SENSOR_GAIN_INDIVIDUAL       (SPECIAL_API_AE_START + 0x2d)  // [0, 1], 0
#define SPECIAL_API_AE_EXP_MODE                            (SPECIAL_API_AE_START + 0x2e)  // [0, 1], 0

#define SPECIAL_API_AE_END                              (SPECIAL_API_AE_START + 0x30)

//AntiFlicker 0x1A-1B
#define SPECIAL_API_ANTI_FLICKER_ENABLE                 (SPECIAL_API_AE_END + 0x00) // [0, 1], 1
#define SPECIAL_API_ANTI_FLICKER_FREQUENCY              (SPECIAL_API_AE_END + 0x01) // [40, 70], 1
#define SPECIAL_API_ANTI_FLICKER_FORCE                  (SPECIAL_API_AE_END + 0x02) // [0, 1], 1

/// @TODO resev to ae use

//ChannleGain 0x1C-1E
#define SPECIAL_API_ISP_WB_GAIN_R                       (SPECIAL_API_ANTI_FLICKER_FREQUENCY + 0x10) // [256, 1024], 1
#define SPECIAL_API_ISP_WB_GAIN_G                       (SPECIAL_API_ANTI_FLICKER_FREQUENCY + 0x11) // [256, 1024], 1
#define SPECIAL_API_ISP_WB_GAIN_B                       (SPECIAL_API_ANTI_FLICKER_FREQUENCY + 0x12) // [256, 2048], 1

//AWB 0x1F-25
#define SPECIAL_API_AWB_START                           (SPECIAL_API_ISP_WB_GAIN_B + 0x01)
#define SPECIAL_API_AWB_ENABLE                          (SPECIAL_API_AWB_START + 0x00) // [0, 1], 1
#define SPECIAL_API_AWB_LOG_ENABLE                      (SPECIAL_API_AWB_START + 0x01) // [0, 2], 1
#define SPECIAL_API_AWB_CONVERGE_STEP                   (SPECIAL_API_AWB_START + 0x02) // [1, 20], 1
#define SPECIAL_API_AWB_STABLE_RANGE                    (SPECIAL_API_AWB_START + 0x03) // [1, 100], 1
#define SPECIAL_API_AWB_MANUAL_ROI_ENABLE               (SPECIAL_API_AWB_START + 0x04)  // [0, 1], 1
#define SPECIAL_API_AWB_MANUAL_ROI_X                    (SPECIAL_API_AWB_START + 0x05)  // [0, 14], 1
#define SPECIAL_API_AWB_MANUAL_ROI_Y                    (SPECIAL_API_AWB_START + 0x06)  // [0, 14], 1
#define SPECIAL_API_AWB_MANUAL_ROI_W                    (SPECIAL_API_AWB_START + 0x07)  // [1, 15], 1
#define SPECIAL_API_AWB_MANUAL_ROI_H                    (SPECIAL_API_AWB_START + 0x08)  // [1, 15], 1
#define SPECIAL_API_AWB_PREF_GAIN_R                     (SPECIAL_API_AWB_START + 0x09)  // [1, 1024], 1
#define SPECIAL_API_AWB_PREF_GAIN_B                     (SPECIAL_API_AWB_START + 0x0A)  // [1, 1024], 1
#define SPECIAL_API_AWB_LOWLIGHT_LUX_THR                (SPECIAL_API_AWB_START + 0x0B)  // [1, 1024], 1
#define SPECIAL_API_AWB_LOWCT_RGPOS_THR                 (SPECIAL_API_AWB_START + 0x0C)  // [1, 8192], 1

#define SPECIAL_API_AWB_END                             (SPECIAL_API_AWB_START + 0x0C)

//CC 0x26
#define SPECIAL_API_CCM_START                           (SPECIAL_API_AWB_END + 0x01)
//#define SPECIAL_API_CCM_LOG_ENABLE                      (SPECIAL_API_CCM_START + 0x00) // [0, 2], 0
#define SPECIAL_API_CCM_SATURATION                      (SPECIAL_API_CCM_START + 0x01) // [0, 512], 1
#define SPECIAL_API_CCM_MANUAL_INDEX_ENABLE             (SPECIAL_API_CCM_START + 0x02) // [0, 1], 1
#define SPECIAL_API_CCM_INDEX                           (SPECIAL_API_CCM_START + 0x03) // [0, 2], 1
#define SPECIAL_API_CCM_INTER_PARAM                      (SPECIAL_API_CCM_START + 0x04) // [0, 256], 1
#define SPECIAL_API_CCM_END                             (SPECIAL_API_CCM_START + 0x04)

//GGC 0x27-2E
#define SPECIAL_API_GGC_START                           (SPECIAL_API_CCM_END + 0x01)
//#define SPECIAL_API_GGC_LOG_LEVEL                       (SPECIAL_API_GGC_START + 0x00)  // [0, 3], 0
// GGC Basic lut interpolation params
//#define SPECIAL_API_GGC_DYNAMIC_SW_ENABLE               (SPECIAL_API_GGC_START + 0x01)  // [0, 1], 0
#define SPECIAL_API_GGC_LOWISO_THR                      (SPECIAL_API_GGC_START + 0x02)  // [100, 51200], 1
#define SPECIAL_API_GGC_HIGHISO_THR                     (SPECIAL_API_GGC_START + 0x03)  // [100, 51200], 1
#define SPECIAL_API_GGC_CONVERGE_SPEED                  (SPECIAL_API_GGC_START + 0x04)  // [10, 100], 1
#define SPECIAL_API_GGC_BASIC_SELECTION                 (SPECIAL_API_GGC_START + 0x05)  // [0, 17], 1
// GGC Tuning params
#define SPECIAL_API_GGC_CONTRAST_ENHANCE_ENABLE         (SPECIAL_API_GGC_START + 0x06)  // [0, 2], 1
#define SPECIAL_API_GGC_CONTRAST_AUTO_MODE              (SPECIAL_API_GGC_START + 0x07)  // [0, 1], 1
#define SPECIAL_API_GGC_CONTRAST_AUTO_STR               (SPECIAL_API_GGC_START + 0x08)  // [0, 2000], 1
#define SPECIAL_API_GGC_CONTRAST_MANUAL_STR             (SPECIAL_API_GGC_START + 0x09)  // [0, 100], 1
#define SPECIAL_API_GGC_DEHAZE_STRENGTH_LIGHT           (SPECIAL_API_GGC_START + 0x0A)  // [0, 8192], 1
#define SPECIAL_API_GGC_DEHAZE_STRENGTH_DARK            (SPECIAL_API_GGC_START + 0x0B)  // [0, 8192], 1
#define SPECIAL_API_GGC_GLOBAL_LUMINATION_ADJ           (SPECIAL_API_GGC_START + 0x0C)  // [0, 256], 1
#define SPECIAL_API_GGC_LOCAL_LUM_ADJ_NODE_0            (SPECIAL_API_GGC_START + 0x0D)  // [0, 100], 1
#define SPECIAL_API_GGC_LOCAL_LUM_ADJ_NODE_1            (SPECIAL_API_GGC_START + 0x0E)  // [0, 100], 1
#define SPECIAL_API_GGC_LOCAL_LUM_ADJ_NODE_2            (SPECIAL_API_GGC_START + 0x0F)  // [0, 100], 1
#define SPECIAL_API_GGC_LOCAL_LUM_ADJ_NODE_3            (SPECIAL_API_GGC_START + 0x10)  // [0, 100], 1
#define SPECIAL_API_GGC_LOCAL_LUM_ADJ_NODE_4            (SPECIAL_API_GGC_START + 0x11)  // [0, 100], 1
#define SPECIAL_API_GGC_LOCAL_LUM_ADJ_NODE_5            (SPECIAL_API_GGC_START + 0x12)  // [0, 100], 1
#define SPECIAL_API_GGC_LOCAL_LUM_ADJ_NODE_6            (SPECIAL_API_GGC_START + 0x13)  // [0, 100], 1
#define SPECIAL_API_GGC_LOCAL_ADAPTIVE_NODE_0           (SPECIAL_API_GGC_START + 0x14)  // [0, 100], 1
#define SPECIAL_API_GGC_LOCAL_ADAPTIVE_NODE_1           (SPECIAL_API_GGC_START + 0x15)  // [0, 100], 1
#define SPECIAL_API_GGC_LOCAL_ADAPTIVE_NODE_2           (SPECIAL_API_GGC_START + 0x16)  // [0, 100], 1
#define SPECIAL_API_GGC_LOCAL_ADAPTIVE_NODE_3           (SPECIAL_API_GGC_START + 0x17)  // [0, 100], 1
#define SPECIAL_API_GGC_LOCAL_ADAPTIVE_NODE_4           (SPECIAL_API_GGC_START + 0x18)  // [0, 100], 1
#define SPECIAL_API_GGC_LOCAL_ADAPTIVE_NODE_5           (SPECIAL_API_GGC_START + 0x19)  // [0, 100], 1
#define SPECIAL_API_GGC_LOCAL_ADAPTIVE_NODE_6           (SPECIAL_API_GGC_START + 0x1A)  // [0, 100], 1
#define SPECIAL_API_GGC_CONTRAST_SMOOTH_RATE            (SPECIAL_API_GGC_START + 0x1B)  // [0, 256], 1
#define SPECIAL_API_GGC_END                             SPECIAL_API_GGC_CONTRAST_SMOOTH_RATE


// AF 0x2F-0x4F
#define SPECIAL_API_AF_START                            (SPECIAL_API_GGC_END + 0x01)
#define SPECIAL_API_AF_MODE                             (SPECIAL_API_AF_START + 0x00) // [0, 1], 1
// AF enable log
#define SPECIAL_API_AF_LOG_ENABLE                       (SPECIAL_API_AF_START + 0x01)  // [0, 2], 1
// AF use yuv stats
#define SPECIAL_API_AF_SOURCE_SEL_YUV                   (SPECIAL_API_AF_START + 0x02)  // [0, 1], 1
#define SPECIAL_API_AF_FULLSCAN_MODE                    (SPECIAL_API_AF_START + 0x03)  // [0, 3], 1
#define SPECIAL_API_AF_BACKJUMP_COMPSTEP                (SPECIAL_API_AF_START + 0x04)  // [0, 7], 1
// AF Monitor with luma
#define SPECIAL_API_AF_LUMA_CHANGE_THR                  (SPECIAL_API_AF_START + 0x05)  // [0, 50], 1
#define SPECIAL_API_AF_LUMA_CHANGE_CNT_THR              (SPECIAL_API_AF_START + 0x06)  // [0, 50], 1
#define SPECIAL_API_AF_LUMA_STABLE_THR                  (SPECIAL_API_AF_START + 0x07)  // [0, 50], 1
#define SPECIAL_API_AF_LUMA_STABLE_CNT_THR              (SPECIAL_API_AF_START + 0x08)  // [0, 50], 1
#define SPECIAL_API_AF_EXPOSURE_CHG_THR                 (SPECIAL_API_AF_START + 0x09)  // [0, 100], 1
// AF Monitor with focus value
#define SPECIAL_API_AF_FV_CHANGE_ENABLE                 (SPECIAL_API_AF_START + 0x0A)  // [0, 1], 1
#define SPECIAL_API_AF_FV_CHANGE_THR                    (SPECIAL_API_AF_START + 0x0B)  // [0, 50], 1
#define SPECIAL_API_AF_FV_CHANGE_CNT_THR                (SPECIAL_API_AF_START + 0x0C)  // [0, 50], 1
#define SPECIAL_API_AF_FV_STABLE_THR                    (SPECIAL_API_AF_START + 0x0D)  // [0, 50], 1
#define SPECIAL_API_AF_FV_STABLE_CNT_THR                (SPECIAL_API_AF_START + 0x0E)  // [0, 50], 1
// AF ROI
#define SPECIAL_API_AF_ROI_X                            (SPECIAL_API_AF_START + 0x0F)  // [0, 14], 1
#define SPECIAL_API_AF_ROI_Y                            (SPECIAL_API_AF_START + 0x10)  // [0, 14], 1
#define SPECIAL_API_AF_ROI_W                            (SPECIAL_API_AF_START + 0x11)  // [1, 15], 1
#define SPECIAL_API_AF_ROI_H                            (SPECIAL_API_AF_START + 0x12)  // [1, 15], 1
// AF contrast search
#define SPECIAL_API_AF_NEAR_RANGE                       (SPECIAL_API_AF_START + 0x13)  // [0, 100], 0
#define SPECIAL_API_AF_FAR_RANGE                        (SPECIAL_API_AF_START + 0x14)  // [0, 100], 0
#define SPECIAL_API_AF_FV_INC_RATIO                     (SPECIAL_API_AF_START + 0x15)  // [1000, 2000], 0
#define SPECIAL_API_AF_FV_DEC_RATIO                     (SPECIAL_API_AF_START + 0x16)  // [800, 1000], 0
#define SPECIAL_API_AF_INC_DEC_RATIO                    (SPECIAL_API_AF_START + 0x17)  // [50, 100], 1
#define SPECIAL_API_AF_DEC_DEC_RATIO                    (SPECIAL_API_AF_START + 0x18)  // [50, 100], 1
#define SPECIAL_API_AF_MOVE_FRAME_SKIP_NUM              (SPECIAL_API_AF_START + 0x19)  // [0, 10], 1
#define SPECIAL_API_AF_DEC_DEC_RATIO_LOWLIGHT           (SPECIAL_API_AF_START + 0x1A)  // [50, 100], 0
// AF point light scene
#define SPECIAL_API_AF_BLOOMINGPOINT_THR                (SPECIAL_API_AF_START + 0x1B)  // [3600, 4096], 0
#define SPECIAL_API_AF_BLOOMINGPOINT_NUM_THR            (SPECIAL_API_AF_START + 0x1C)  // [10, 100], 0
#define SPECIAL_API_AF_BLOOMINGPOINT_GAIN_THR           (SPECIAL_API_AF_START + 0x1D)  // [0, 542720], 0
// AF low light param
#define SPECIAL_API_AF_LOWLIGHT_GAIN_THR                (SPECIAL_API_AF_START + 0x1E)  // [0, 542720], 1
#define SPECIAL_API_AF_MACRO_SEARCH_END_THR             (SPECIAL_API_AF_START + 0x1F)  // [0, 100], 1
#define SPECIAL_API_AF_MANUAL_SET_PARAM_ENABLE          (SPECIAL_API_AF_START + 0x20)  // [0, 1], 1
#define SPECIAL_API_AF_END                              SPECIAL_API_AF_MANUAL_SET_PARAM_ENABLE

#define SPECIAL_API_LTM_START                           (SPECIAL_API_AF_END + 0x01)
//#define SPECIAL_API_LTM_MANUAL_SET_PARAM_ENABLE         (SPECIAL_API_LTM_START + 0x00) // [0, 1], 0
#define SPECIAL_API_LTM_LOG_LEVEL                       (SPECIAL_API_LTM_START + 0x01) // [0, 2], 1
#define SPECIAL_API_LTM_GAIN                            (SPECIAL_API_LTM_START + 0x02) // [1, 1024], 1
#define SPECIAL_API_LTM_DARK_PRC                        (SPECIAL_API_LTM_START + 0x03) // [0, 1000], 1
#define SPECIAL_API_LTM_BRIGHT_PRC                      (SPECIAL_API_LTM_START + 0x04) // [0, 1000], 1
#define SPECIAL_API_LTM_MIN_LIGHTEN                     (SPECIAL_API_LTM_START + 0x05) // [0, 255], 1
#define SPECIAL_API_LTM_MAX_LIGHTEN                     (SPECIAL_API_LTM_START + 0x06) // [0, 255], 1
#define SPECIAL_API_LTM_CONTRAST_MIN                    (SPECIAL_API_LTM_START + 0x07) // [0, 1023], 1
#define SPECIAL_API_LTM_CONTRAST_MAX                    (SPECIAL_API_LTM_START + 0x08) // [0, 1023], 1
#define SPECIAL_API_LTM_GDG_ENABLE                      (SPECIAL_API_LTM_START + 0x09) // [0, 1], 1
#define SPECIAL_API_LTM_TARGET_LDR                      (SPECIAL_API_LTM_START + 0x0A) // [1, 1023], 1
#define SPECIAL_API_LTM_HI_TARGET_PRC                   (SPECIAL_API_LTM_START + 0x0B) // [0, 100], 1
#define SPECIAL_API_LTM_DARK_LEVEL                      (SPECIAL_API_LTM_START + 0x0C) // [0, 1023], 1
#define SPECIAL_API_LTM_DARK_MIN_PRC                    (SPECIAL_API_LTM_START + 0x0D) // [0, 100], 1
#define SPECIAL_API_LTM_DARK_MAX_PRC                    (SPECIAL_API_LTM_START + 0x0E) // [0, 100], 1
#define SPECIAL_API_LTM_DARK_GAIN_RATIO                 (SPECIAL_API_LTM_START + 0x0F) // [100, 1000], 1
#define SPECIAL_API_LTM_AMPLIFYALPHA                    (SPECIAL_API_LTM_START + 0x10) // [0, 4095], 1
#define SPECIAL_API_LTM_AMPLIFYALPHA_MAX                (SPECIAL_API_LTM_START + 0x11) // [0, 4095], 1
#define SPECIAL_API_LTM_END                             (SPECIAL_API_LTM_START + 0x11)

#define SPECIAL_API_SDE_START                         (SPECIAL_API_LTM_END + 0x01)
#define SPECIAL_API_AWB_SDE_CT_COOL                     (SPECIAL_API_SDE_START + 0x00)  // [0, 1], 1
#define SPECIAL_API_AWB_SDE_CT_WARM                     (SPECIAL_API_SDE_START + 0x01)  // [0, 1], 1
#define SPECIAL_API_LSH_SDE_SHARPEN                     (SPECIAL_API_SDE_START + 0x02)  // [0, 1], 1
#define SPECIAL_API_SDE_END                           (SPECIAL_API_LSH_SDE_SHARPEN)

#define SPECIAL_API_CALI_LNC_ENABLE                   (SPECIAL_API_SDE_END + 0x01)      // [0, 1], 0

#define SPECIAL_API_MANUAL_MODE_START                 (SPECIAL_API_CALI_LNC_ENABLE + 0x01)
#define SPECIAL_API_MANUAL_MODE_ALL_MODULE             (SPECIAL_API_MANUAL_MODE_START + 0x00)  // [0, 1], 1
#define SPECIAL_API_MANUAL_MODE_CMB                    (SPECIAL_API_MANUAL_MODE_START + 0x01)  // [0, 1], 1
#define SPECIAL_API_MANUAL_MODE_DPC                    (SPECIAL_API_MANUAL_MODE_START + 0x02)  // [0, 1], 1
#define SPECIAL_API_MANUAL_MODE_RNR                    (SPECIAL_API_MANUAL_MODE_START + 0x03)  // [0, 1], 1
#define SPECIAL_API_MANUAL_MODE_BLC                    (SPECIAL_API_MANUAL_MODE_START + 0x04)  // [0, 1], 1
#define SPECIAL_API_MANUAL_MODE_LSC                    (SPECIAL_API_MANUAL_MODE_START + 0x05)  // [0, 1], 1
#define SPECIAL_API_MANUAL_MODE_LTM                    (SPECIAL_API_MANUAL_MODE_START + 0x06)  // [0, 1], 1
#define SPECIAL_API_MANUAL_MODE_DMS                    (SPECIAL_API_MANUAL_MODE_START + 0x07)  // [0, 1], 1
#define SPECIAL_API_MANUAL_MODE_DPF                    (SPECIAL_API_MANUAL_MODE_START + 0x08)  // [0, 1], 1
#define SPECIAL_API_MANUAL_MODE_CCM                    (SPECIAL_API_MANUAL_MODE_START + 0x09)  // [0, 1], 1
#define SPECIAL_API_MANUAL_MODE_GGC                    (SPECIAL_API_MANUAL_MODE_START + 0x0A)  // [0, 1], 1
#define SPECIAL_API_MANUAL_MODE_CSP                    (SPECIAL_API_MANUAL_MODE_START + 0x0B)  // [0, 1], 0
#define SPECIAL_API_MANUAL_MODE_MVD                    (SPECIAL_API_MANUAL_MODE_START + 0x0C)  // [0, 1], 1
#define SPECIAL_API_MANUAL_MODE_TNR                    (SPECIAL_API_MANUAL_MODE_START + 0x0D)  // [0, 1], 1
#define SPECIAL_API_MANUAL_MODE_CNR                    (SPECIAL_API_MANUAL_MODE_START + 0x0E)  // [0, 1], 1
#define SPECIAL_API_MANUAL_MODE_LSH                    (SPECIAL_API_MANUAL_MODE_START + 0x0F)  // [0, 1], 1
#define SPECIAL_API_MANUAL_MODE_SDE                    (SPECIAL_API_MANUAL_MODE_START + 0x10)  // [0, 1], 1
#define SPECIAL_API_MANUAL_MODE_END                   (SPECIAL_API_MANUAL_MODE_START + 0x10)

#define SPECIAL_API_FACE_INFO_START                   (SPECIAL_API_MANUAL_MODE_END + 0x01)
#define SPECIAL_API_FACE_DETECT_IMG_W                  (SPECIAL_API_FACE_INFO_START + 0x00)  // [128, 3840], 1
#define SPECIAL_API_FACE_DETECT_IMG_H                  (SPECIAL_API_FACE_INFO_START + 0x01)  // [128, 3840], 1
#define SPECIAL_API_FACE_NUM                           (SPECIAL_API_FACE_INFO_START + 0x02)  // [0, 4],      1
#define SPECIAL_API_FACE_INDEX                         (SPECIAL_API_FACE_INFO_START + 0x03)  // [0, 3],      1
#define SPECIAL_API_FACE_CONFDENCE                     (SPECIAL_API_FACE_INFO_START + 0x04)  // [0, 100],    1
#define SPECIAL_API_FACE_ROT_ANGLE                     (SPECIAL_API_FACE_INFO_START + 0x05)  // [0, 180],    1
#define SPECIAL_API_FACE_X                             (SPECIAL_API_FACE_INFO_START + 0x06)  // [0, 3840],   1
#define SPECIAL_API_FACE_Y                             (SPECIAL_API_FACE_INFO_START + 0x07)  // [0, 3840],   1
#define SPECIAL_API_FACE_W                             (SPECIAL_API_FACE_INFO_START + 0x08)  // [0, 3840],   1
#define SPECIAL_API_FACE_H                             (SPECIAL_API_FACE_INFO_START + 0x09)  // [0, 3840],   1
#define SPECIAL_API_FACE_LEFT_EYE_L_X                  (SPECIAL_API_FACE_INFO_START + 0x0A)  // [0, 3840],   1
#define SPECIAL_API_FACE_LEFT_EYE_L_Y                  (SPECIAL_API_FACE_INFO_START + 0x0B)  // [0, 3840],   1
#define SPECIAL_API_FACE_LEFT_EYE_R_X                  (SPECIAL_API_FACE_INFO_START + 0x0C)  // [0, 3840],   1
#define SPECIAL_API_FACE_LEFT_EYE_R_Y                  (SPECIAL_API_FACE_INFO_START + 0x0D)  // [0, 3840],   1
#define SPECIAL_API_FACE_RIGHT_EYE_L_X                 (SPECIAL_API_FACE_INFO_START + 0x0E)  // [0, 3840],   1
#define SPECIAL_API_FACE_RIGHT_EYE_L_Y                 (SPECIAL_API_FACE_INFO_START + 0x0F)  // [0, 3840],   1
#define SPECIAL_API_FACE_RIGHT_EYE_R_X                 (SPECIAL_API_FACE_INFO_START + 0x10)  // [0, 3840],   1
#define SPECIAL_API_FACE_RIGHT_EYE_R_Y                 (SPECIAL_API_FACE_INFO_START + 0x11)  // [0, 3840],   1
#define SPECIAL_API_FACE_MOUSE_L_X                     (SPECIAL_API_FACE_INFO_START + 0x12)  // [0, 3840],   1
#define SPECIAL_API_FACE_MOUSE_L_Y                     (SPECIAL_API_FACE_INFO_START + 0x13)  // [0, 3840],   1
#define SPECIAL_API_FACE_MOUSE_R_X                     (SPECIAL_API_FACE_INFO_START + 0x14)  // [0, 3840],   1
#define SPECIAL_API_FACE_MOUSE_R_Y                     (SPECIAL_API_FACE_INFO_START + 0x15)  // [0, 3840],   1
#define SPECIAL_API_FACE_NOSE_X                        (SPECIAL_API_FACE_INFO_START + 0x16)  // [0, 3840],   1
#define SPECIAL_API_FACE_NOSE_Y                        (SPECIAL_API_FACE_INFO_START + 0x17)  // [0, 3840],   1

#define SPECIAL_API_FACE_INFO_END                     (SPECIAL_API_FACE_INFO_START + 0x17)

#define SPECIAL_API_FACE_PARAM_START                  (SPECIAL_API_FACE_INFO_END + 0x01)
#define SPECIAL_API_FACE_AE_ENABLE                     (SPECIAL_API_FACE_PARAM_START + 0x00)  // [0, 1],    1
#define SPECIAL_API_FACE_STATS_SEL                     (SPECIAL_API_FACE_PARAM_START + 0x01)  // [0, 1],    1
#define SPECIAL_API_FACE_AE_CONVERGE_STEP              (SPECIAL_API_FACE_PARAM_START + 0x02)  // [1, 32],   1
#define SPECIAL_API_FACE_AE_STABLE_RANGE               (SPECIAL_API_FACE_PARAM_START + 0x03)  // [1, 255],  1
#define SPECIAL_API_FACE_RAW_TARGET                    (SPECIAL_API_FACE_PARAM_START + 0x04)  // [1, 1023], 1
#define SPECIAL_API_FACE_YUV_TARGET                    (SPECIAL_API_FACE_PARAM_START + 0x05)  // [1, 1023], 1
#define SPECIAL_API_FACE_SCALE                         (SPECIAL_API_FACE_PARAM_START + 0x06)  // [10,1000], 1
#define SPECIAL_API_FACE_CROP_TOP                      (SPECIAL_API_FACE_PARAM_START + 0x07)  // [0, 50],   1
#define SPECIAL_API_FACE_CROP_BOT                      (SPECIAL_API_FACE_PARAM_START + 0x08)  // [0, 50],   1
#define SPECIAL_API_FACE_CROP_LEF                      (SPECIAL_API_FACE_PARAM_START + 0x09)  // [0, 50],   1
#define SPECIAL_API_FACE_CROP_RIG                      (SPECIAL_API_FACE_PARAM_START + 0x0A)  // [0, 50],   1
#define SPECIAL_API_FACE_IN_CNT_THR                    (SPECIAL_API_FACE_PARAM_START + 0x0B)  // [1, 50],   1
#define SPECIAL_API_FACE_OUT_CNT_THR                   (SPECIAL_API_FACE_PARAM_START + 0x0C)  // [1, 50],   1
#define SPECIAL_API_FACE_SMALL_SIZE_THR                (SPECIAL_API_FACE_PARAM_START + 0x0D)  // [0, 10000], 1
#define SPECIAL_API_FACE_PARAM_END                    (SPECIAL_API_FACE_PARAM_START + 0x0D)

/// new AWB tuning params
#define SPECIAL_API_AWB_MANUAL_SET_PARAM_ENABLE         (SPECIAL_API_FACE_PARAM_END + 0x01)              // [0, 1]    1
#define SPECIAL_API_AWB_DETECT_ZONE_START             (SPECIAL_API_AWB_MANUAL_SET_PARAM_ENABLE + 0x01)
#define SPECIAL_API_AWB_DETECT_ZONE_INDEX               (SPECIAL_API_AWB_DETECT_ZONE_START + 0x00)  // [0, 9],    1
#define SPECIAL_API_AWB_DETECT_ZONE_ENABLE              (SPECIAL_API_AWB_DETECT_ZONE_START + 0x01)  // [0, 1],    1
#define SPECIAL_API_AWB_DETECT_ZONE_LUX_START           (SPECIAL_API_AWB_DETECT_ZONE_START + 0x02)  // [1, 1000],    1
#define SPECIAL_API_AWB_DETECT_ZONE_LUX_END             (SPECIAL_API_AWB_DETECT_ZONE_START + 0x03)  // [1, 1000],    1
#define SPECIAL_API_AWB_DETECT_ZONE_LUX_BUF             (SPECIAL_API_AWB_DETECT_ZONE_START + 0x04)  // [1, 256],    1
#define SPECIAL_API_AWB_DETECT_ZONE_LEFT                (SPECIAL_API_AWB_DETECT_ZONE_START + 0x05)  // [0, 2000],    1
#define SPECIAL_API_AWB_DETECT_ZONE_RIGHT               (SPECIAL_API_AWB_DETECT_ZONE_START + 0x06)  // [0, 2000],    1
#define SPECIAL_API_AWB_DETECT_ZONE_TOP                 (SPECIAL_API_AWB_DETECT_ZONE_START + 0x07)  // [0, 2000],    1
#define SPECIAL_API_AWB_DETECT_ZONE_BOT                 (SPECIAL_API_AWB_DETECT_ZONE_START + 0x08)  // [0, 2000],    1
#define SPECIAL_API_AWB_DETECT_ZONE_END               (SPECIAL_API_AWB_DETECT_ZONE_START + 0x08)

#define SPECIAL_API_AWB_MLCZ_START                    (SPECIAL_API_AWB_DETECT_ZONE_END + 0x01)
#define SPECIAL_API_AWB_MLCZ_INDEX                      (SPECIAL_API_AWB_MLCZ_START + 0x00)  // [0, 9],    1
#define SPECIAL_API_AWB_MLCZ_ENABLE                     (SPECIAL_API_AWB_MLCZ_START + 0x01)  // [0, 1],    1
#define SPECIAL_API_AWB_MLCZ_DETECT_ZONE_IDX            (SPECIAL_API_AWB_MLCZ_START + 0x02)  // [0, 16],    1
#define SPECIAL_API_AWB_MLCZ_LUX_START                  (SPECIAL_API_AWB_MLCZ_START + 0x03)  // [1, 1000],    1
#define SPECIAL_API_AWB_MLCZ_LUX_END                    (SPECIAL_API_AWB_MLCZ_START + 0x04)  // [1, 1000],    1
#define SPECIAL_API_AWB_MLCZ_LUX_BUF                    (SPECIAL_API_AWB_MLCZ_START + 0x05)  // [1, 256],    1
#define SPECIAL_API_AWB_MLCZ_HIT_PRC_THR                (SPECIAL_API_AWB_MLCZ_START + 0x06)  // [0, 1000],    1
#define SPECIAL_API_AWB_MLCZ_HIT_PRC_BUF                (SPECIAL_API_AWB_MLCZ_START + 0x07)  // [0, 512],    1
#define SPECIAL_API_AWB_MLCZ_LEFT                       (SPECIAL_API_AWB_MLCZ_START + 0x08)  // [0, 2000],    1
#define SPECIAL_API_AWB_MLCZ_RIGHT                      (SPECIAL_API_AWB_MLCZ_START + 0x09)  // [0, 2000],    1
#define SPECIAL_API_AWB_MLCZ_TOP                        (SPECIAL_API_AWB_MLCZ_START + 0x0A)  // [0, 2000],    1
#define SPECIAL_API_AWB_MLCZ_BOT                        (SPECIAL_API_AWB_MLCZ_START + 0x0B)  // [0, 2000],    1
#define SPECIAL_API_AWB_MLCZ_END                    (SPECIAL_API_AWB_MLCZ_START + 0x0B)

#define SPECIAL_API_AWB_PREF_COLOR_START                    (SPECIAL_API_AWB_MLCZ_END + 0x01)
#define SPECIAL_API_AWB_PREF_COLOR_INDEX                      (SPECIAL_API_AWB_PREF_COLOR_START + 0x00)  // [0, 9],    1
#define SPECIAL_API_AWB_PREF_COLOR_ENABLE                     (SPECIAL_API_AWB_PREF_COLOR_START + 0x01)  // [0, 1],    1
#define SPECIAL_API_AWB_PREF_COLOR_REF_LIGHT_IDX              (SPECIAL_API_AWB_PREF_COLOR_START + 0x02)  // [0, 6],    1
#define SPECIAL_API_AWB_PREF_COLOR_LUX_START                  (SPECIAL_API_AWB_PREF_COLOR_START + 0x03)  // [1, 1000],    1
#define SPECIAL_API_AWB_PREF_COLOR_LUX_END                    (SPECIAL_API_AWB_PREF_COLOR_START + 0x04)  // [1, 1000],    1
#define SPECIAL_API_AWB_PREF_COLOR_LUX_BUF                    (SPECIAL_API_AWB_PREF_COLOR_START + 0x05)  // [1, 256],    1
#define SPECIAL_API_AWB_PREF_COLOR_OFFSET_THR_X               (SPECIAL_API_AWB_PREF_COLOR_START + 0x06)  // [0, 256],    1
#define SPECIAL_API_AWB_PREF_COLOR_OFFSET_THR_Y               (SPECIAL_API_AWB_PREF_COLOR_START + 0x07)  // [0, 256],    1
#define SPECIAL_API_AWB_PREF_COLOR_CLIP_RATIO_X               (SPECIAL_API_AWB_PREF_COLOR_START + 0x08)  // [0, 256],    1
#define SPECIAL_API_AWB_PREF_COLOR_CLIP_RATIO_Y               (SPECIAL_API_AWB_PREF_COLOR_START + 0x09)  // [0, 256],    1
#define SPECIAL_API_AWB_PREF_COLOR_SHIFT_GAIN_R               (SPECIAL_API_AWB_PREF_COLOR_START + 0x0A)  // [128, 512],    1
#define SPECIAL_API_AWB_PREF_COLOR_SHIFT_GAIN_B               (SPECIAL_API_AWB_PREF_COLOR_START + 0x0B)  // [128, 512],    1
#define SPECIAL_API_AWB_PREF_COLOR_END                     (SPECIAL_API_AWB_PREF_COLOR_START + 0x0B)

#define SPECIAL_API_AWB_LIMIT_Y_START                    (SPECIAL_API_AWB_PREF_COLOR_END + 0x01)
#define SPECIAL_API_AWB_LIMIT_Y_INDEX                      (SPECIAL_API_AWB_LIMIT_Y_START + 0x00)  // [0, 9],    1
#define SPECIAL_API_AWB_LIMIT_Y_ENABLE                     (SPECIAL_API_AWB_LIMIT_Y_START + 0x01)  // [0, 1],    1
#define SPECIAL_API_AWB_LIMIT_Y_REF_LIGHT_IDX              (SPECIAL_API_AWB_LIMIT_Y_START + 0x02)  // [0, 6],    1
#define SPECIAL_API_AWB_LIMIT_Y_LUX_START                  (SPECIAL_API_AWB_LIMIT_Y_START + 0x03)  // [1, 1000],    1
#define SPECIAL_API_AWB_LIMIT_Y_LUX_END                    (SPECIAL_API_AWB_LIMIT_Y_START + 0x04)  // [1, 1000],    1
#define SPECIAL_API_AWB_LIMIT_Y_LUX_BUF                    (SPECIAL_API_AWB_LIMIT_Y_START + 0x05)  // [1, 256],    1
#define SPECIAL_API_AWB_LIMIT_Y_PRC                        (SPECIAL_API_AWB_LIMIT_Y_START + 0x06)  // [0, 256],    1
#define SPECIAL_API_AWB_LIMIT_Y_WEIGHT_REDUCE              (SPECIAL_API_AWB_LIMIT_Y_START + 0x07)  // [0, 256],    1
#define SPECIAL_API_AWB_LIMIT_Y_END                     (SPECIAL_API_AWB_LIMIT_Y_START + 0x07)

#define SPECIAL_API_AWB_LIGHT_SOURCE_START                    (SPECIAL_API_AWB_LIMIT_Y_END + 0x01)
#define SPECIAL_API_AWB_LIGHT_SOURCE_INDEX                      (SPECIAL_API_AWB_LIGHT_SOURCE_START + 0x00)  // [0, 9],    1
#define SPECIAL_API_AWB_LIGHT_SOURCE_ENABLE                     (SPECIAL_API_AWB_LIGHT_SOURCE_START + 0x01)  // [0, 1],    1
#define SPECIAL_API_AWB_LIGHT_SOURCE_REF_LIGHT_IDX              (SPECIAL_API_AWB_LIGHT_SOURCE_START + 0x02)  // [0, 6],    1
#define SPECIAL_API_AWB_LIGHT_SOURCE_LUX_START                  (SPECIAL_API_AWB_LIGHT_SOURCE_START + 0x03)  // [1, 1000],    1
#define SPECIAL_API_AWB_LIGHT_SOURCE_LUX_END                    (SPECIAL_API_AWB_LIGHT_SOURCE_START + 0x04)  // [1, 1000],    1
#define SPECIAL_API_AWB_LIGHT_SOURCE_LUX_BUF                    (SPECIAL_API_AWB_LIGHT_SOURCE_START + 0x05)  // [1, 256],    1
#define SPECIAL_API_AWB_LIGHT_SOURCE_SCALE_LEFT                 (SPECIAL_API_AWB_LIGHT_SOURCE_START + 0x06)  // [0, 1024],    1
#define SPECIAL_API_AWB_LIGHT_SOURCE_SCALE_RIGHT                (SPECIAL_API_AWB_LIGHT_SOURCE_START + 0x07)  // [0, 1024],    1
#define SPECIAL_API_AWB_LIGHT_SOURCE_SCALE_TOP                  (SPECIAL_API_AWB_LIGHT_SOURCE_START + 0x08)  // [0, 1024],    1
#define SPECIAL_API_AWB_LIGHT_SOURCE_SCALE_BOT                  (SPECIAL_API_AWB_LIGHT_SOURCE_START + 0x09)  // [0, 1024],    1
#define SPECIAL_API_AWB_LIGHT_SOURCE_END                     (SPECIAL_API_AWB_LIGHT_SOURCE_START + 0x09)

#define SPECIAL_API_AWB_LIGHT_SOURCE_PROB_START               (SPECIAL_API_AWB_LIGHT_SOURCE_END + 0x01)
#define SPECIAL_API_AWB_LIGHT_SOURCE_PROB_H                     (SPECIAL_API_AWB_LIGHT_SOURCE_PROB_START + 0x00)  // [0, 256],    1
#define SPECIAL_API_AWB_LIGHT_SOURCE_PROB_A                     (SPECIAL_API_AWB_LIGHT_SOURCE_PROB_START + 0x01)  // [0, 256],    1
#define SPECIAL_API_AWB_LIGHT_SOURCE_PROB_TL84                  (SPECIAL_API_AWB_LIGHT_SOURCE_PROB_START + 0x02)  // [0, 256],    1
#define SPECIAL_API_AWB_LIGHT_SOURCE_PROB_CWF                   (SPECIAL_API_AWB_LIGHT_SOURCE_PROB_START + 0x03)  // [0, 256],    1
#define SPECIAL_API_AWB_LIGHT_SOURCE_PROB_D65                   (SPECIAL_API_AWB_LIGHT_SOURCE_PROB_START + 0x04)  // [0, 256],    1
#define SPECIAL_API_AWB_LIGHT_SOURCE_PROB_DF                    (SPECIAL_API_AWB_LIGHT_SOURCE_PROB_START + 0x05)  // [0, 256],    1
#define SPECIAL_API_AWB_LIGHT_SOURCE_PROB_SHADE                 (SPECIAL_API_AWB_LIGHT_SOURCE_PROB_START + 0x06)  // [0, 256],    1
#define SPECIAL_API_AWB_LIGHT_SOURCE_PROB_END                 (SPECIAL_API_AWB_LIGHT_SOURCE_PROB_START + 0x06)

#define SPECIAL_API_FACE_AWB_START                            (SPECIAL_API_AWB_LIGHT_SOURCE_PROB_END + 0x01)
#define SPECIAL_API_FACE_AWB_ENABLE                             (SPECIAL_API_FACE_AWB_START + 0x00)  // [0, 1],    1
#define SPECIAL_API_FACE_AWB_ENABLE_NR                          (SPECIAL_API_FACE_AWB_START + 0x01)  // [0, 1],    1
#define SPECIAL_API_FACE_AWB_WEIGHT                             (SPECIAL_API_FACE_AWB_START + 0x02)  // [0, 100],    1
#define SPECIAL_API_FACE_AWB_REF_TARGET_TOL                     (SPECIAL_API_FACE_AWB_START + 0x03)  // [0, 32],    1
#define SPECIAL_API_FACE_AWB_FACE_SCALE                         (SPECIAL_API_FACE_AWB_START + 0x04)  // [10, 1000],    1
#define SPECIAL_API_FACE_AWB_FACE_SIZE_RATIO_L                  (SPECIAL_API_FACE_AWB_START + 0x05)  // [0, 1024],    1
#define SPECIAL_API_FACE_AWB_FACE_SIZE_RATIO_H                  (SPECIAL_API_FACE_AWB_START + 0x06)  // [0, 1024],    1
#define SPECIAL_API_FACE_AWB_DIST_THR_L                         (SPECIAL_API_FACE_AWB_START + 0x07)  // [0, 256],    1
#define SPECIAL_API_FACE_AWB_DIST_THR_H                         (SPECIAL_API_FACE_AWB_START + 0x08)  // [0, 256],    1
#define SPECIAL_API_FACE_AWB_PREF_GAIN_R                        (SPECIAL_API_FACE_AWB_START + 0x09)  // [64, 1024],    1
#define SPECIAL_API_FACE_AWB_PREF_GAIN_B                        (SPECIAL_API_FACE_AWB_START + 0x0A)  // [64, 1024],    1
#define SPECIAL_API_FACE_AWB_END                              (SPECIAL_API_FACE_AWB_START + 0x0A)


#define SPECIAL_API_AWB_EXTRA_COLOR_START                    (SPECIAL_API_FACE_AWB_END + 0x01)
#define SPECIAL_API_AWB_EXTRA_COLOR_INDEX                      (SPECIAL_API_AWB_EXTRA_COLOR_START + 0x00)  // [0, 9],    1
#define SPECIAL_API_AWB_EXTRA_COLOR_ENABLE                     (SPECIAL_API_AWB_EXTRA_COLOR_START + 0x01)  // [0, 1],    1
#define SPECIAL_API_AWB_EXTRA_COLOR_LUX_START                  (SPECIAL_API_AWB_EXTRA_COLOR_START + 0x02)  // [1, 1000],    1
#define SPECIAL_API_AWB_EXTRA_COLOR_LUX_END                    (SPECIAL_API_AWB_EXTRA_COLOR_START + 0x03)  // [1, 1000],    1
#define SPECIAL_API_AWB_EXTRA_COLOR_LUX_BUF                    (SPECIAL_API_AWB_EXTRA_COLOR_START + 0x04)  // [1, 256],    1
#define SPECIAL_API_AWB_EXTRA_COLOR_DETECT_PRC_THR             (SPECIAL_API_AWB_EXTRA_COLOR_START + 0x05)  // [0, 1024],    1
#define SPECIAL_API_AWB_EXTRA_COLOR_DETECT_PRC_BUF             (SPECIAL_API_AWB_EXTRA_COLOR_START + 0x06)  // [0, 512],    1
#define SPECIAL_API_AWB_EXTRA_COLOR_LEFT                       (SPECIAL_API_AWB_EXTRA_COLOR_START + 0x07)  // [0, 2000],    1
#define SPECIAL_API_AWB_EXTRA_COLOR_RIGHT                      (SPECIAL_API_AWB_EXTRA_COLOR_START + 0x08)  // [0, 2000],    1
#define SPECIAL_API_AWB_EXTRA_COLOR_TOP                        (SPECIAL_API_AWB_EXTRA_COLOR_START + 0x09)  // [0, 2000],    1
#define SPECIAL_API_AWB_EXTRA_COLOR_BOT                        (SPECIAL_API_AWB_EXTRA_COLOR_START + 0x0A)  // [0, 2000],    1
#define SPECIAL_API_AWB_EXTRA_COLOR_LIGHT_IDX                  (SPECIAL_API_AWB_EXTRA_COLOR_START + 0x0B)  // [0, 7],    1
#define SPECIAL_API_AWB_EXTRA_COLOR_WEIGHT                     (SPECIAL_API_AWB_EXTRA_COLOR_START + 0x0C)  // [0, 256],    1
#define SPECIAL_API_AWB_EXTRA_COLOR_PREF_GAIN_R                (SPECIAL_API_AWB_EXTRA_COLOR_START + 0x0D)  // [128, 512],    1
#define SPECIAL_API_AWB_EXTRA_COLOR_PREF_GAIN_B                (SPECIAL_API_AWB_EXTRA_COLOR_START + 0x0E)  // [128, 512],    1
#define SPECIAL_API_AWB_EXTRA_COLOR_END                      (SPECIAL_API_AWB_EXTRA_COLOR_START + 0x0E)

#define SPECIAL_API_AWB_SPECIAL_GAIN_START               (SPECIAL_API_AWB_EXTRA_COLOR_END + 0x01)
#define SPECIAL_API_AWB_SPECIAL_GAIN_H_R                     (SPECIAL_API_AWB_SPECIAL_GAIN_START + 0x00)  // [256, 1024],    1
#define SPECIAL_API_AWB_SPECIAL_GAIN_H_B                     (SPECIAL_API_AWB_SPECIAL_GAIN_START + 0x01)  // [256, 1024],    1
#define SPECIAL_API_AWB_SPECIAL_GAIN_A_R                     (SPECIAL_API_AWB_SPECIAL_GAIN_START + 0x02)  // [256, 1024],    1
#define SPECIAL_API_AWB_SPECIAL_GAIN_A_B                     (SPECIAL_API_AWB_SPECIAL_GAIN_START + 0x03)  // [256, 1024],    1
#define SPECIAL_API_AWB_SPECIAL_GAIN_TL84_R                  (SPECIAL_API_AWB_SPECIAL_GAIN_START + 0x04)  // [256, 1024],    1
#define SPECIAL_API_AWB_SPECIAL_GAIN_TL84_B                  (SPECIAL_API_AWB_SPECIAL_GAIN_START + 0x05)  // [256, 1024],    1
#define SPECIAL_API_AWB_SPECIAL_GAIN_CWF_R                   (SPECIAL_API_AWB_SPECIAL_GAIN_START + 0x06)  // [256, 1024],    1
#define SPECIAL_API_AWB_SPECIAL_GAIN_CWF_B                   (SPECIAL_API_AWB_SPECIAL_GAIN_START + 0x07)  // [256, 1024],    1
#define SPECIAL_API_AWB_SPECIAL_GAIN_D65_R                   (SPECIAL_API_AWB_SPECIAL_GAIN_START + 0x08)  // [256, 1024],    1
#define SPECIAL_API_AWB_SPECIAL_GAIN_D65_B                   (SPECIAL_API_AWB_SPECIAL_GAIN_START + 0x09)  // [256, 1024],    1
#define SPECIAL_API_AWB_SPECIAL_GAIN_DF_R                    (SPECIAL_API_AWB_SPECIAL_GAIN_START + 0x0A)  // [256, 1024],    1
#define SPECIAL_API_AWB_SPECIAL_GAIN_DF_B                    (SPECIAL_API_AWB_SPECIAL_GAIN_START + 0x0B)  // [256, 1024],    1
#define SPECIAL_API_AWB_SPECIAL_GAIN_SHADE_R                 (SPECIAL_API_AWB_SPECIAL_GAIN_START + 0x0C)  // [256, 1024],    1
#define SPECIAL_API_AWB_SPECIAL_GAIN_SHADE_B                 (SPECIAL_API_AWB_SPECIAL_GAIN_START + 0x0D)  // [256, 1024],    1
#define SPECIAL_API_AWB_SPECIAL_GAIN_END                 (SPECIAL_API_AWB_SPECIAL_GAIN_START + 0x0D)

#define SPECIAL_API_AWB_SPECIAL_GAIN_WEIGHT_START               (SPECIAL_API_AWB_SPECIAL_GAIN_END + 0x01)
#define SPECIAL_API_AWB_SPECIAL_GAIN_WEIGHT_H                     (SPECIAL_API_AWB_SPECIAL_GAIN_WEIGHT_START + 0x00)  // [0, 256],    1
#define SPECIAL_API_AWB_SPECIAL_GAIN_WEIGHT_A                     (SPECIAL_API_AWB_SPECIAL_GAIN_WEIGHT_START + 0x01)  // [0, 256],    1
#define SPECIAL_API_AWB_SPECIAL_GAIN_WEIGHT_TL84                  (SPECIAL_API_AWB_SPECIAL_GAIN_WEIGHT_START + 0x02)  // [0, 256],    1
#define SPECIAL_API_AWB_SPECIAL_GAIN_WEIGHT_CWF                   (SPECIAL_API_AWB_SPECIAL_GAIN_WEIGHT_START + 0x03)  // [0, 256],    1
#define SPECIAL_API_AWB_SPECIAL_GAIN_WEIGHT_D65                   (SPECIAL_API_AWB_SPECIAL_GAIN_WEIGHT_START + 0x04)  // [0, 256],    1
#define SPECIAL_API_AWB_SPECIAL_GAIN_WEIGHT_DF                    (SPECIAL_API_AWB_SPECIAL_GAIN_WEIGHT_START + 0x05)  // [0, 256],    1
#define SPECIAL_API_AWB_SPECIAL_GAIN_WEIGHT_SHADE                 (SPECIAL_API_AWB_SPECIAL_GAIN_WEIGHT_START + 0x06)  // [0, 256],    1
#define SPECIAL_API_AWB_SPECIAL_GAIN_WEIGHT_END                 (SPECIAL_API_AWB_SPECIAL_GAIN_WEIGHT_START + 0x06)

#define SPECIAL_API_AWB_OTP_GAIN_START                    (SPECIAL_API_AWB_SPECIAL_GAIN_WEIGHT_END + 0x01)
#define SPECIAL_API_AWB_OTP_HIGH_CT_GAIN_R                (SPECIAL_API_AWB_OTP_GAIN_START + 0x00)  // [128, 512],    0
#define SPECIAL_API_AWB_OTP_HIGH_CT_GAIN_B                (SPECIAL_API_AWB_OTP_GAIN_START + 0x01)  // [128, 512],    0
#define SPECIAL_API_AWB_OTP_MID_CT_GAIN_R                 (SPECIAL_API_AWB_OTP_GAIN_START + 0x02)  // [128, 512],    0
#define SPECIAL_API_AWB_OTP_MID_CT_GAIN_B                 (SPECIAL_API_AWB_OTP_GAIN_START + 0x03)  // [128, 512],    0
#define SPECIAL_API_AWB_OTP_LOW_CT_GAIN_R                 (SPECIAL_API_AWB_OTP_GAIN_START + 0x04)  // [128, 512],    0
#define SPECIAL_API_AWB_OTP_LOW_CT_GAIN_B                 (SPECIAL_API_AWB_OTP_GAIN_START + 0x05)  // [128, 512],    0
#define SPECIAL_API_AWB_OTP_GAIN_END                     (SPECIAL_API_AWB_OTP_GAIN_START + 0x05)

#define SPECIAL_API_SENSOR_TEST_EXP_STEP                    (SPECIAL_API_AWB_OTP_GAIN_END + 0x01) //[1, 100], 0
#define SPECIAL_API_SENSOR_TEST_GAIN_STEP                   (SPECIAL_API_AWB_OTP_GAIN_END + 0x02) //[4, 1024], 0
#define SPECIAL_API_SENSOR_TEST_MODE                        (SPECIAL_API_AWB_OTP_GAIN_END + 0x03) //[0, 7], 0


#define SPECIAL_API_DOL2_NP_START                           (SPECIAL_API_SENSOR_TEST_MODE + 0x00)
#define SPECIAL_API_DOL2_NP_LGAIN                           (SPECIAL_API_DOL2_NP_START + 0x01)     // [1, 4096], 1
#define SPECIAL_API_DOL2_NP_MGAIN                           (SPECIAL_API_DOL2_NP_START + 0x02)     // [1, 4096], 1
#define SPECIAL_API_DOL2_NP_TRANS_GAIN                      (SPECIAL_API_DOL2_NP_START + 0x03)     // [0, 1024], 1
#define SPECIAL_API_DOL2_NP_FIT_ORDER                       (SPECIAL_API_DOL2_NP_START + 0x04)     // [1, 5], 1

/// AWB NEW
#define SPECIAL_API_AWB_NEW_START                         (SPECIAL_API_DOL2_NP_FIT_ORDER + 0x01)
#define SPECIAL_API_AWB_DARK_PIXEL_THR                      (SPECIAL_API_AWB_NEW_START + 0x00)  // [0, 2048],    0
#define SPECIAL_API_AWB_SAT_PIXEL_THR                       (SPECIAL_API_AWB_NEW_START + 0x01)  // [2049, 4095],    0
#define SPECIAL_API_AWB_DARK_PRC_THR                        (SPECIAL_API_AWB_NEW_START + 0x02)  // [0, 255],    0
#define SPECIAL_API_AWB_SAT_PRC_THR                         (SPECIAL_API_AWB_NEW_START + 0x03)  // [0, 255],    0
#define SPECIAL_API_AWB_LUMA_WEIGHT_ENABLE                  (SPECIAL_API_AWB_NEW_START + 0x04)  // [0, 1],    0

#define SPECIAL_API_AWB_NEW_END                            (SPECIAL_API_AWB_NEW_START + 0x04)


#define SPECIAL_API_SENSOR_GAIN_START                     (SPECIAL_API_AWB_NEW_END + 0x10)
#define SPECIAL_API_SENSOR_GAIN1                            (SPECIAL_API_SENSOR_GAIN_START + 0x01) // [1024, 133444], 0
#define SPECIAL_API_SENSOR_GAIN1_X                          (SPECIAL_API_SENSOR_GAIN_START + 0x02) // [100, 12800], 1
#define SPECIAL_API_SENSOR_GAIN2                            (SPECIAL_API_SENSOR_GAIN_START + 0x03) // [1024, 133444], 0
#define SPECIAL_API_SENSOR_GAIN2_X                          (SPECIAL_API_SENSOR_GAIN_START + 0x04) // [100, 12800], 1
#define SPECIAL_API_SENSOR_GAIN3                            (SPECIAL_API_SENSOR_GAIN_START + 0x05) // [1024, 133444], 0
#define SPECIAL_API_SENSOR_GAIN3_X                          (SPECIAL_API_SENSOR_GAIN_START + 0x06) // [100, 12800], 0
#define SPECIAL_API_SENSOR_GAIN_END                      (SPECIAL_API_SENSOR_GAIN_START + 0x06)

//Parsing_End
#if !defined (HW_BUILD_FOR_CUST0)
#define SPECIAL_API_TOTAL_SIZE                                (SPECIAL_API_SENSOR_GAIN_END + 0x01)
#else
#include "fw_special_api_i5_ttnr.h"
#include "fw_special_api_i5_sw3a.h"
#define SPECIAL_API_TOTAL_SIZE                              (SPECIAL_API_EXT_SW3A_END + 0x01)
#endif

typedef struct
{
    u32 specialApiValTab[SPECIAL_API_TOTAL_SIZE];
} specital_api_t;

#endif

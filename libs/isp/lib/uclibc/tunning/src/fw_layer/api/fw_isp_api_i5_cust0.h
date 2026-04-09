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

#ifndef __FW_ISP_API_I5_CUST0_H__
#define __FW_ISP_API_I5_CUST0_H__

//Direction
#define ISP_API_SET_VAL             0x00000000
#define ISP_API_GET_VAL             0x00000001


//Parsing_Start  // [min, max], isRelease

/*Top*/
#define _TOP_START                      0x00000000
//#define ISP_API_INPUT_MODE                  (_TOP_START + 0x00)  // [1, 6], 0
#define ISP_API_OUTPUT_MODE                 (_TOP_START + 0x01)  // null
#define ISP_API_GGC_ENABLE                  (_TOP_START + 0x02)  // [0, 1], 1
#define ISP_API_COLOR_MATRIX_ENABLE         (_TOP_START + 0x03)  // [0, 1], 1
#define ISP_API_LIN_BYPASS                  (_TOP_START + 0x04)  // [0, 1], 0
#define ISP_API_DPC_BYPASS                  (_TOP_START + 0x05)  // [0, 1], 1
#define ISP_API_RNR_BYPASS                  (_TOP_START + 0x06)  // [0, 1], 1
#define ISP_API_DMS_BYPASS                  (_TOP_START + 0x07)  // [0, 1], 1
#define ISP_API_LSH_BYPASS                  (_TOP_START + 0x08)  // [0, 1], 1
#define ISP_API_CNR_BYPASS                  (_TOP_START + 0x09)  // [0, 1], 1
#define ISP_API_MVD_BYPASS                  (_TOP_START + 0x0A)  // [0, 1], 1
#define ISP_API_TNR_BYPASS                  (_TOP_START + 0x0B)  // [0, 1], 1
#define ISP_API_CAC_BYPASS                  (_TOP_START + 0x0C)  // [0, 1], 1
#define ISP_API_LTM_BYPASS                  (_TOP_START + 0x0D)  // [0, 1], 1
#define ISP_API_CMB_BYPASS                  (_TOP_START + 0x0E)  // [0, 1], 1
#define ISP_API_DPF_BYPASS                  (_TOP_START + 0x0F)  // [0, 1], 1
#define ISP_API_DEBUG_OUT_MODE              (_TOP_START + 0x10)  // [0, 25], 1
#define ISP_API_SKIP_FRAME                  (_TOP_START + 0x11)  // [0, 2], 1
#define ISP_API_OUT_INDEX                   (_TOP_START + 0x12)  // [0, 1], 1
#define ISP_API_DISP_TYPE                   (_TOP_START + 0x13)  // [0, 1], 1
#define _TOP_END                        (_TOP_START + 0x13)

/*Module*/
#define _RAW_DOMAIN_START               (_TOP_END + 0x01) //0x14
#define ISP_API_CMB_OUT_MODE                (_RAW_DOMAIN_START + 0x00)  // [0, 3], 1
#define ISP_API_CMB_NP_MULT_LM              (_RAW_DOMAIN_START + 0x01)  // [0, 65535], 1
#define ISP_API_CMB_NP_MULT_MS              (_RAW_DOMAIN_START + 0x02)  // [0, 65535], 1
#define ISP_API_CMB_MOV_LM_MULT             (_RAW_DOMAIN_START + 0x03)  // [0, 65535], 1
#define ISP_API_CMB_MOV_MS_MULT             (_RAW_DOMAIN_START + 0x04)  // [0, 65535], 1
#define ISP_API_CMB_THRESH1_LM              (_RAW_DOMAIN_START + 0x05)  // [0, 4095], 1
#define ISP_API_CMB_THRESH2_LM              (_RAW_DOMAIN_START + 0x06)  // [0, 4095], 1
#define ISP_API_CMB_THRESH1_MS              (_RAW_DOMAIN_START + 0x07)  // [0, 4095], 1
#define ISP_API_CMB_THRESH2_MS              (_RAW_DOMAIN_START + 0x08)  // [0, 4095], 1
#define ISP_API_CMB_OUTPUT_ALPHA_LM         (_RAW_DOMAIN_START + 0x09)  // [0, 1], 1
#define ISP_API_CMB_OUTPUT_ALPHA_MS         (_RAW_DOMAIN_START + 0x0A)  // [0, 1], 1
#define ISP_API_CMB_OUTPUT_LM               (_RAW_DOMAIN_START + 0x0B)  // [0, 1], 1
#define ISP_API_CMB_OUTPUT_MS               (_RAW_DOMAIN_START + 0x0C)  // [0, 1], 1

#define ISP_API_CMB_BL_LONG_R               (_RAW_DOMAIN_START + 0x0D)  // [0, 4095], 1
#define ISP_API_CMB_BL_LONG_GR              (_RAW_DOMAIN_START + 0x0E)  // [0, 4095], 1
#define ISP_API_CMB_BL_LONG_GB              (_RAW_DOMAIN_START + 0x0F)  // [0, 4095], 1
#define ISP_API_CMB_BL_LONG_B               (_RAW_DOMAIN_START + 0x10)  // [0, 4095], 1
#define ISP_API_CMB_BL_MEDIUM_R             (_RAW_DOMAIN_START + 0x11)  // [0, 4095], 1
#define ISP_API_CMB_BL_MEDIUM_GR            (_RAW_DOMAIN_START + 0x12)  // [0, 4095], 1
#define ISP_API_CMB_BL_MEDIUM_GB            (_RAW_DOMAIN_START + 0x13)  // [0, 4095], 1
#define ISP_API_CMB_BL_MEDIUM_B             (_RAW_DOMAIN_START + 0x14)  // [0, 4095], 1
#define ISP_API_CMB_BL_SHORT_R              (_RAW_DOMAIN_START + 0x15)  // [0, 4095], 1
#define ISP_API_CMB_BL_SHORT_GR             (_RAW_DOMAIN_START + 0x16)  // [0, 4095], 1
#define ISP_API_CMB_BL_SHORT_GB             (_RAW_DOMAIN_START + 0x17)  // [0, 4095], 1
#define ISP_API_CMB_BL_SHORT_B              (_RAW_DOMAIN_START + 0x18)  // [0, 4095], 1

#define ISP_API_DPC_DEBUG                   (_RAW_DOMAIN_START + 0x19)  // [0, 1], 1
#define ISP_API_DPC_GE_EN                   (_RAW_DOMAIN_START + 0x1A)  // [0, 1], 1
#define ISP_API_DPC_INTPL_ALPHA             (_RAW_DOMAIN_START + 0x1B)  // [0, 256], 1
#define ISP_API_DPC_DIV_SLP                 (_RAW_DOMAIN_START + 0x1C)  // [0, 4095], 1
#define ISP_API_DPC_DIV_THR                 (_RAW_DOMAIN_START + 0x1D)  // [0, 4095], 1
#define ISP_API_DPC_STDEV_MASK_THR          (_RAW_DOMAIN_START + 0x1E)  // [0, 4095], 1
#define ISP_API_DPC_DP_DIS_THR              (_RAW_DOMAIN_START + 0x1F)  // [0, 4095], 1
#define ISP_API_DPC_CUR_MED_THR             (_RAW_DOMAIN_START + 0x20)  // [0, 4095], 1
#define ISP_API_DPC_GE_STR                  (_RAW_DOMAIN_START + 0x21)  // [0, 4095], 1
#define ISP_API_DPC_GE_THR                  (_RAW_DOMAIN_START + 0x22)  // [0, 255], 1
#define ISP_API_DPC_GE_SENS                 (_RAW_DOMAIN_START + 0x23)  // [0, 4095], 1
#define ISP_API_DPC_GE_SLOPE                (_RAW_DOMAIN_START + 0x24)  // [0, 4095], 1
#define ISP_API_DPC_NP_GAIN                 (_RAW_DOMAIN_START + 0x25)  // [0, 4095], 1
#define ISP_API_DPC_NP_OFFSET               (_RAW_DOMAIN_START + 0x26)  // [0, 4095], 1

#define ISP_API_RNR_BLT_LF_NP_GAIN          (_RAW_DOMAIN_START + 0x27)  // [0, 4095], 1
#define ISP_API_RNR_BLT_LF_NP_STR           (_RAW_DOMAIN_START + 0x28)  // [0, 256], 1
//#define ISP_API_RNR_NLM_WEIGHT_TABLE        (_RAW_DOMAIN_START + 0x29)  // [0, 100], 0
#define ISP_API_RNR_SAD_MULT2               (_RAW_DOMAIN_START + 0x2A)  // [0, 15], 0
#define ISP_API_RNR_MF_ENHANCE              (_RAW_DOMAIN_START + 0x2B)  // [0, 128], 0

#define ISP_API_BLACK_LEVEL_ENABLE          (_RAW_DOMAIN_START + 0x2C)  // [0, 1], 1
#define ISP_API_BLACK_LEVEL_R               (_RAW_DOMAIN_START + 0x2D)  // [0, 65535], 1
#define ISP_API_BLACK_LEVEL_GR              (_RAW_DOMAIN_START + 0x2E)  // [0, 65535], 1
#define ISP_API_BLACK_LEVEL_GB              (_RAW_DOMAIN_START + 0x2F)  // [0, 65535], 1
#define ISP_API_BLACK_LEVEL_B               (_RAW_DOMAIN_START + 0x30)  // [0, 65535], 1

#define ISP_API_DIGITAL_GAIN                (_RAW_DOMAIN_START + 0x31)  // [4096, 1048575], 0

#define ISP_API_LSC_ENABLE                  (_RAW_DOMAIN_START + 0x32)  // [0, 1], 1
#define ISP_API_LSC_MESH_RADIAL_SWITCH      (_RAW_DOMAIN_START + 0x33)  // [0, 1], 1
#define ISP_API_LSC_STRENGTH                (_RAW_DOMAIN_START + 0x34)  // [0, 2047], 1
#define ISP_API_LSC_MESH_FRAC               (_RAW_DOMAIN_START + 0x35)  // [0, 7], 1
#define ISP_API_LSC_RADIAL_FRAC             (_RAW_DOMAIN_START + 0x36)  // [0, 6], 1

#define ISP_API_WB_GAIN_ENABLE              (_RAW_DOMAIN_START + 0x37)  // [0, 1], 1
#define ISP_API_CHANNEL_GAIN_R              (_RAW_DOMAIN_START + 0x38)  // [256, 1024], 1
#define ISP_API_CHANNEL_GAIN_G              (_RAW_DOMAIN_START + 0x39)  // [256, 1024], 1
#define ISP_API_CHANNEL_GAIN_B              (_RAW_DOMAIN_START + 0x3A)  // [256, 1024], 1

#define ISP_API_LTM_LOCAL_CONTRAST          (_RAW_DOMAIN_START + 0x3B)  // [0, 512], 0
#define ISP_API_LTM_MAX_GAIN                (_RAW_DOMAIN_START + 0x3C)  // [0, 1024], 0
#define ISP_API_LTM_MAX_LIGHTEN             (_RAW_DOMAIN_START + 0x3D)  // [0, 255], 0
#define ISP_API_LTM_BGGEN                   (_RAW_DOMAIN_START + 0x3E)  // [0, 1], 0
#define ISP_API_LTM_SPATIAL_FILTER_TIME     (_RAW_DOMAIN_START + 0x3F)  // [0, 31], 1
#define ISP_API_LTM_AMPLIFYALPHA            (_RAW_DOMAIN_START + 0x40)  // [0, 4095], 0
#define ISP_API_LTM_CONVERGE_SPEED          (_RAW_DOMAIN_START + 0x41)  // [0, 4095], 1
#define ISP_API_LTM_MAX_APPLY               (_RAW_DOMAIN_START + 0x42)  // [0, 1048575], 0
#define _RAW_DOMAIN_END                 (_RAW_DOMAIN_START + 0x42) //0x56

#define _RGB_DOMAIN_START               (_RAW_DOMAIN_END + 0x01) //0x57
#define ISP_API_DMS_HF_DETAIL_ENH_EN        (_RGB_DOMAIN_START + 0x00)  // [0, 1], 1
#define ISP_API_DMS_HF_DETAIL_ENH_STR       (_RGB_DOMAIN_START + 0x01)  // [0, 255], 1
#define ISP_API_DMS_SHARPEN_EN              (_RGB_DOMAIN_START + 0x02)  // [0, 1], 1
#define ISP_API_DMS_SHARPEN_STR             (_RGB_DOMAIN_START + 0x03)  // [0, 16384], 1
#define ISP_API_DMS_NP_OFF_ABS              (_RGB_DOMAIN_START + 0x04)  // [0, 1], 1
#define ISP_API_DMS_NP_OFF                  (_RGB_DOMAIN_START + 0x05)  // [0, 255], 1
#define ISP_API_DMS_GREY_SMALIAS_SLP        (_RGB_DOMAIN_START + 0x06)  // [0, 8191], 1
#define ISP_API_DMS_GREEN_USERB_THR         (_RGB_DOMAIN_START + 0x07)  // [0, 4095], 1
#define ISP_API_DMS_GREEN_USERB_SLP         (_RGB_DOMAIN_START + 0x08)  // [0, 8191], 1
#define ISP_API_DMS_DX_THR                  (_RGB_DOMAIN_START + 0x09)  // [0, 4095], 1
#define ISP_API_DMS_DX_SLP                  (_RGB_DOMAIN_START + 0x0A)  // [0, 255], 1
#define ISP_API_DMS_DP_THR                  (_RGB_DOMAIN_START + 0x0B)  // [0, 4095], 1
#define ISP_API_DMS_DP_SLP                  (_RGB_DOMAIN_START + 0x0C)  // [0, 255], 1
#define ISP_API_DMS_DU_SLP                  (_RGB_DOMAIN_START + 0x0D)  // [0, 255], 0
#define ISP_API_DMS_DUSH_OFFSET             (_RGB_DOMAIN_START + 0x0E)  // [0, 256], 1
#define ISP_API_DMS_DUSH_THR                (_RGB_DOMAIN_START + 0x0F)  // [0, 256], 1
#define ISP_API_DMS_DUSH_SLP                (_RGB_DOMAIN_START + 0x10)  // [0, 255], 1
#define ISP_API_DMS_DPX_THR                 (_RGB_DOMAIN_START + 0x11)  // [0, 4095], 1
#define ISP_API_DMS_DPX_SLP                 (_RGB_DOMAIN_START + 0x12)  // [0, 255], 1
#define ISP_API_DMS_FCRATE_STR              (_RGB_DOMAIN_START + 0x13)  // [0, 3], 1
#define ISP_API_DMS_FC_SLP                  (_RGB_DOMAIN_START + 0x14)  // [0, 512], 1
#define ISP_API_DMS_ALIAS_THR               (_RGB_DOMAIN_START + 0x15)  // [0, 511], 1
#define ISP_API_DMS_ALIAS_SLP               (_RGB_DOMAIN_START + 0x16)  // [0, 255], 1
#define ISP_API_DMS_BLDALIAS_THR            (_RGB_DOMAIN_START + 0x17)  // [0, 1024], 1
#define ISP_API_DMS_BLDALIAS_SLP            (_RGB_DOMAIN_START + 0x18)  // [0, 255], 1
#define ISP_API_DMS_SAT_THR                 (_RGB_DOMAIN_START + 0x19)  // [0, 4095], 1
#define ISP_API_DMS_SAT_SLP                 (_RGB_DOMAIN_START + 0x1A)  // [0, 255], 1
#define ISP_API_DMS_BD_OPTION               (_RGB_DOMAIN_START + 0x1B)  // [0, 1], 1
#define ISP_API_DMS_DEBUG_MODE              (_RGB_DOMAIN_START + 0x1C)  // [0, 31], 1

#define ISP_API_COLOR_MATRIX_RR             (_RGB_DOMAIN_START + 0x1D)  // [0, 16383], 1
#define ISP_API_COLOR_MATRIX_RG             (_RGB_DOMAIN_START + 0x1E)  // [0, 16383], 1
#define ISP_API_COLOR_MATRIX_RB             (_RGB_DOMAIN_START + 0x1F)  // [0, 16383], 1
#define ISP_API_COLOR_MATRIX_GR             (_RGB_DOMAIN_START + 0x20)  // [0, 16383], 1
#define ISP_API_COLOR_MATRIX_GG             (_RGB_DOMAIN_START + 0x21)  // [0, 16383], 1
#define ISP_API_COLOR_MATRIX_GB             (_RGB_DOMAIN_START + 0x22)  // [0, 16383], 1
#define ISP_API_COLOR_MATRIX_BR             (_RGB_DOMAIN_START + 0x23)  // [0, 16383], 1
#define ISP_API_COLOR_MATRIX_BG             (_RGB_DOMAIN_START + 0x24)  // [0, 16383], 1
#define ISP_API_COLOR_MATRIX_BB             (_RGB_DOMAIN_START + 0x25)  // [0, 16383], 1

#define ISP_API_DPF_CENTER                  (_RGB_DOMAIN_START + 0x26)  // [0, 100], 0
#define ISP_API_DPF_SAD_SLP                 (_RGB_DOMAIN_START + 0x27)  // [0, 4095], 1
#define ISP_API_DPF_SAD_THR                 (_RGB_DOMAIN_START + 0x28)  // [0, 4095], 1
#define ISP_API_DPF_HUE1_SLOPE              (_RGB_DOMAIN_START + 0x29)  // [0, 4095], 1
#define ISP_API_DPF_HUE1_TH                 (_RGB_DOMAIN_START + 0x2A)  // [0, 4095], 1
#define ISP_API_DPF_HUE2_SLOPE              (_RGB_DOMAIN_START + 0x2B)  // [0, 4095], 1
#define ISP_API_DPF_HUE2_TH                 (_RGB_DOMAIN_START + 0x2C)  // [0, 4095], 1
#define ISP_API_DPF_HUE_STR                 (_RGB_DOMAIN_START + 0x2D)  // [0, 4095], 1
#define ISP_API_DPF_SAT1_SLOPE              (_RGB_DOMAIN_START + 0x2E)  // [0, 4095], 1
#define ISP_API_DPF_SAT1_TH                 (_RGB_DOMAIN_START + 0x2F)  // [0, 4095], 1
#define ISP_API_DPF_SAT_STR                 (_RGB_DOMAIN_START + 0x30)  // [0, 4095], 1
#define ISP_API_DPF_LUMA11_SLOPE            (_RGB_DOMAIN_START + 0x31)  // [0, 4095], 1
#define ISP_API_DPF_LUMA11_TH               (_RGB_DOMAIN_START + 0x32)  // [0, 4095], 1
#define ISP_API_DPF_LUMA12_SLOPE            (_RGB_DOMAIN_START + 0x33)  // [0, 4095], 1
#define ISP_API_DPF_LUMA12_TH               (_RGB_DOMAIN_START + 0x34)  // [0, 4095], 1
#define ISP_API_DPF_LUMA21_SLOPE            (_RGB_DOMAIN_START + 0x35)  // [0, 4095], 1
#define ISP_API_DPF_LUMA21_TH               (_RGB_DOMAIN_START + 0x36)  // [0, 4095], 1
#define ISP_API_DPF_LUMA22_SLOPE            (_RGB_DOMAIN_START + 0x37)  // [0, 4095], 1
#define ISP_API_DPF_LUMA22_TH               (_RGB_DOMAIN_START + 0x38)  // [0, 4095], 1
#define ISP_API_DPF_LUMA_STR                (_RGB_DOMAIN_START + 0x39)  // [0, 4095], 1
#define ISP_API_DPF_HSL_SLOPE               (_RGB_DOMAIN_START + 0x3A)  // [0, 4095], 1
#define ISP_API_DPF_HSL_TH                  (_RGB_DOMAIN_START + 0x3B)  // [0, 4095], 1
#define ISP_API_DPF_PURPLE_STR              (_RGB_DOMAIN_START + 0x3C)  // [0, 4095], 1
#define ISP_API_DPF_SATUR_STR               (_RGB_DOMAIN_START + 0x3D)  // [0, 255], 1
#define ISP_API_DPF_DEBUG_MODE              (_RGB_DOMAIN_START + 0x3E)  // [0, 7], 1
#define _RGB_DOMAIN_END                 (_RGB_DOMAIN_START + 0x3E) //0x95

#define _3DNR_START                     (_RGB_DOMAIN_END + 0x01) //0x96
#define ISP_API_MVD_DEBUG                   (_3DNR_START + 0x00)  // [0, 1], 1
#define ISP_API_MVD_NP_GAIN                 (_3DNR_START + 0x01)  // [0, 1023], 1
#define ISP_API_MVD_NP_OFFSET               (_3DNR_START + 0x02)  // [0, 4095], 1
#define ISP_API_MVD_PIPEA_THR               (_3DNR_START + 0x03)  // [0, 25], 1
#define ISP_API_MVD_SAD_BITDEPTH            (_3DNR_START + 0x04)  // [8, 12], 0
#define ISP_API_MVD_LOSSY_COMP_MODE         (_3DNR_START + 0x05)  // [0, 3], 0
#define ISP_API_MVD_PIPEB_ERODE1            (_3DNR_START + 0x06)  // [0, 5], 1
#define ISP_API_MVD_PIPEB_DILATE            (_3DNR_START + 0x07)  // [0, 7], 1
#define ISP_API_MVD_PIPEB_ERODE2            (_3DNR_START + 0x08)  // [0, 7], 1
#define ISP_API_MVD_PIPEB_MAP_REFINE_THR    (_3DNR_START + 0x09)  // [0, 8], 1
#define ISP_API_MVD_PIPEAB_TRI_DIR_STR      (_3DNR_START + 0x0A)  // [0, 256], 1
#define ISP_API_MVD_BLACK_LEVEL             (_3DNR_START + 0x0B)  // [0, 4095], 0
#define ISP_API_MVD_BLACK_LEVEL_R           (_3DNR_START + 0x0C)  // [0, 4095], 1
#define ISP_API_MVD_BLACK_LEVEL_GR          (_3DNR_START + 0x0D)  // [0, 4095], 1
#define ISP_API_MVD_BLACK_LEVEL_GB          (_3DNR_START + 0x0E)  // [0, 4095], 1
#define ISP_API_MVD_BLACK_LEVEL_B           (_3DNR_START + 0x0F)  // [0, 4095], 1

#define ISP_API_TNR_MOTION_2P_MODE          (_3DNR_START + 0x10)  // [0, 2], 1
#define ISP_API_TNR_NR2D_PYRAMID_WT         (_3DNR_START + 0x11)  // [0, 2], 1
#define ISP_API_TNR_GLOBAL_NOISE_THR        (_3DNR_START + 0x12)  // [0, 1023], 1
#define ISP_API_TNR_NR2D_GRAD_SLOPE         (_3DNR_START + 0x13)  // [16, 255], 1
#define ISP_API_TNR_NR2D_YNOISE_THR         (_3DNR_START + 0x14)  // [0, 1023], 1
#define ISP_API_TNR_NR2D_CNOISE_THR         (_3DNR_START + 0x15)  // [0, 1023], 1
#define ISP_API_TNR_NR2D_NOISE_BACK_RATIO   (_3DNR_START + 0x16)  // [0, 64], 1
#define ISP_API_TNR_MOTION_THR              (_3DNR_START + 0x17)  // [0, 13], 1
#define ISP_API_TNR_MOTION_CURVE_OFFSET     (_3DNR_START + 0x18)  // [0, 1024], 1
#define ISP_API_TNR_MOTION_CURVE_MID        (_3DNR_START + 0x19)  // [0, 11], 1
#define ISP_API_TNR_MOTION_CURVE_END        (_3DNR_START + 0x1A)  // [0, 11], 1
#define ISP_API_TNR_MOTION_CURVE_Y1         (_3DNR_START + 0x1B)  // [0, 64], 1
#define ISP_API_TNR_MOTION_CURVE_Y2         (_3DNR_START + 0x1C)  // [0, 64], 1
#define ISP_API_TNR_MOTION_CURVE_Y3         (_3DNR_START + 0x1D)  // [0, 64], 1
#define ISP_API_TNR_NR2D_CURVE_OFFSET       (_3DNR_START + 0x1E)  // [0, 256], 1
#define ISP_API_TNR_NR2D_CURVE_MID          (_3DNR_START + 0x1F)  // [0, 9], 1
#define ISP_API_TNR_NR2D_CURVE_END          (_3DNR_START + 0x20)  // [0, 9], 1
#define ISP_API_TNR_NR2D_CURVE_Y1           (_3DNR_START + 0x21)  // [0, 64], 1
#define ISP_API_TNR_NR2D_CURVE_Y2           (_3DNR_START + 0x22)  // [0, 64], 1
#define ISP_API_TNR_NR2D_CURVE_Y3           (_3DNR_START + 0x23)  // [0, 64], 1
#define ISP_API_TNR_NR2D_CURVE_C1           (_3DNR_START + 0x24)  // [0, 64], 1
#define ISP_API_TNR_NR2D_CURVE_C2           (_3DNR_START + 0x25)  // [0, 64], 1
#define ISP_API_TNR_NR2D_CURVE_C3           (_3DNR_START + 0x26)  // [0, 64], 1
#define ISP_API_TNR_NR3D_CURVE_OFFSET       (_3DNR_START + 0x27)  // [0, 256], 1
#define ISP_API_TNR_NR3D_CURVE_MID          (_3DNR_START + 0x28)  // [0, 9], 1
#define ISP_API_TNR_NR3D_CURVE_END          (_3DNR_START + 0x29)  // [0, 9], 1
#define ISP_API_TNR_NR3D_CURVE_Y1           (_3DNR_START + 0x2A)  // [0, 64], 1
#define ISP_API_TNR_NR3D_CURVE_Y2           (_3DNR_START + 0x2B)  // [0, 64], 1
#define ISP_API_TNR_NR3D_CURVE_Y3           (_3DNR_START + 0x2C)  // [0, 64], 1
#define ISP_API_TNR_NR3D_CURVE_C1           (_3DNR_START + 0x2D)  // [0, 64], 1
#define ISP_API_TNR_NR3D_CURVE_C2           (_3DNR_START + 0x2E)  // [0, 64], 1
#define ISP_API_TNR_NR3D_CURVE_C3           (_3DNR_START + 0x2F)  // [0, 64], 1
#define _3DNR_END                       (_3DNR_START + 0x2F) //0xc5

#define _CNR_START                      (_3DNR_END + 0x01) //0xc6
#define ISP_API_CNR_MOTION_NOISE_GAIN       (_CNR_START + 0x00)  // [0, 16383], 1
#define ISP_API_CNR_LUMA_NOISE_OFFSET       (_CNR_START + 0x01)  // [0, 1024], 1
#define ISP_API_CNR_LUMA_NOISE_MID          (_CNR_START + 0x02)  // [0, 11], 1
#define ISP_API_CNR_LUMA_NOISE_END          (_CNR_START + 0x03)  // [0, 11], 1
#define ISP_API_CNR_LUMA_NOISE_Y1           (_CNR_START + 0x04)  // [0, 1023], 1
#define ISP_API_CNR_LUMA_NOISE_Y2           (_CNR_START + 0x05)  // [0, 1023], 1
#define ISP_API_CNR_LUMA_NOISE_Y3           (_CNR_START + 0x06)  // [0, 1023], 1
#define ISP_API_CNR_RFILT_SAT_OFFSET        (_CNR_START + 0x07)  // [0, 512], 1
#define ISP_API_CNR_RFILT_SAT_MID           (_CNR_START + 0x08)  // [0, 10], 1
#define ISP_API_CNR_RFILT_SAT_END           (_CNR_START + 0x09)  // [0, 10], 1
#define ISP_API_CNR_RFILT_SAT_Y1            (_CNR_START + 0x0A)  // [0, 256], 1
#define ISP_API_CNR_RFILT_SAT_Y2            (_CNR_START + 0x0B)  // [0, 256], 1
#define ISP_API_CNR_RFILT_SAT_Y3            (_CNR_START + 0x0C)  // [0, 256], 1
#define ISP_API_CNR_RFILT_DIFF_OFFSET       (_CNR_START + 0x0D)  // [0, 1024], 1
#define ISP_API_CNR_RFILT_DIFF_MID          (_CNR_START + 0x0E)  // [0, 11], 1
#define ISP_API_CNR_RFILT_DIFF_END          (_CNR_START + 0x0F)  // [0, 11], 1
#define ISP_API_CNR_RFILT_DIFF_Y1           (_CNR_START + 0x10)  // [0, 256], 1
#define ISP_API_CNR_RFILT_DIFF_Y2           (_CNR_START + 0x11)  // [0, 256], 1
#define ISP_API_CNR_RFILT_DIFF_Y3           (_CNR_START + 0x12)  // [0, 256], 1
#define ISP_API_CNR_GAUSS_NOISE_OFFSET      (_CNR_START + 0x13)  // [0, 128], 1
#define ISP_API_CNR_GAUSS_NOISE_GAIN        (_CNR_START + 0x14)  // [0, 16383], 1
#define ISP_API_CNR_UVDIFF_SFT              (_CNR_START + 0x15)  // [0, 15], 1
#define ISP_API_CNR_UV_BLEND_OFFSET         (_CNR_START + 0x16)  // [0, 1024], 1
#define ISP_API_CNR_UV_BLEND_MID            (_CNR_START + 0x17)  // [0, 11], 1
#define ISP_API_CNR_UV_BLEND_END            (_CNR_START + 0x18)  // [0, 11], 1
#define ISP_API_CNR_UV_BLEND_Y1             (_CNR_START + 0x19)  // [0, 256], 1
#define ISP_API_CNR_UV_BLEND_Y2             (_CNR_START + 0x1A)  // [0, 256], 1
#define ISP_API_CNR_UV_BLEND_Y3             (_CNR_START + 0x1B)  // [0, 256], 1
#define ISP_API_CNR_FC_SUP_EN               (_CNR_START + 0x1C)  // [0, 1], 1
#define ISP_API_CNR_FC_SUP_OFFSET           (_CNR_START + 0x1D)  // [0, 1024], 1
#define ISP_API_CNR_FC_SUP_MID              (_CNR_START + 0x1E)  // [0, 11], 1
#define ISP_API_CNR_FC_SUP_END              (_CNR_START + 0x1F)  // [0, 11], 1
#define ISP_API_CNR_FC_SUP_Y1               (_CNR_START + 0x20)  // [0, 256], 1
#define ISP_API_CNR_FC_SUP_Y2               (_CNR_START + 0x21)  // [0, 256], 1
#define ISP_API_CNR_FC_SUP_Y3               (_CNR_START + 0x22)  // [0, 256], 1
#define ISP_API_CNR_RADIAL_TABLE            (_CNR_START + 0x23)  // [0, 103], 0
#define _CNR_END                        (_CNR_START + 0x23) //0xe9

#define _LSH_START                      (_CNR_END + 0x01) //0xea
#define ISP_API_LSH_MOTION_R                (_LSH_START + 0x00)  // [0, 256], 1
#define ISP_API_LSH_LUM_OFFSET              (_LSH_START + 0x01)  // [0, 1024], 1
#define ISP_API_LSH_LUM_MID                 (_LSH_START + 0x02)  // [0, 11], 1
#define ISP_API_LSH_LUM_END                 (_LSH_START + 0x03)  // [0, 11], 1
#define ISP_API_LSH_LUM_Y1                  (_LSH_START + 0x04)  // [0, 1023], 1
#define ISP_API_LSH_LUM_Y2                  (_LSH_START + 0x05)  // [0, 1023], 1
#define ISP_API_LSH_LUM_Y3                  (_LSH_START + 0x06)  // [0, 1023], 1
#define ISP_API_LSH_EDGE_NLM_SLP            (_LSH_START + 0x07)  // [0, 4095], 1
#define ISP_API_LSH_EDGE_N2W_NORM_R         (_LSH_START + 0x08)  // [0, 4095], 1
#define ISP_API_LSH_EDGE_THIN_OFFSET        (_LSH_START + 0x09)  // [0, 1024], 1
#define ISP_API_LSH_EDGE_THIN_MID           (_LSH_START + 0x0A)  // [0, 11], 1
#define ISP_API_LSH_EDGE_THIN_END           (_LSH_START + 0x0B)  // [0, 11], 1
#define ISP_API_LSH_EDGE_THIN_Y1            (_LSH_START + 0x0C)  // [0, 256], 1
#define ISP_API_LSH_EDGE_THIN_Y2            (_LSH_START + 0x0D)  // [0, 256], 1
#define ISP_API_LSH_EDGE_THIN_Y3            (_LSH_START + 0x0E)  // [0, 256], 1
#define ISP_API_LSH_NR_FREQ_OFFSET          (_LSH_START + 0x0F)  // [0, 1024], 1
#define ISP_API_LSH_NR_FREQ_MID             (_LSH_START + 0x10)  // [0, 11], 1
#define ISP_API_LSH_NR_FREQ_END             (_LSH_START + 0x11)  // [0, 11], 1
#define ISP_API_LSH_NR_FREQ_Y1              (_LSH_START + 0x12)  // [0, 256], 1
#define ISP_API_LSH_NR_FREQ_Y2              (_LSH_START + 0x13)  // [0, 256], 1
#define ISP_API_LSH_NR_FREQ_Y3              (_LSH_START + 0x14)  // [0, 256], 1
#define ISP_API_LSH_NEG_STR1                (_LSH_START + 0x15)  // [0, 16383], 1
#define ISP_API_LSH_POS_STR1                (_LSH_START + 0x16)  // [0, 16383], 1
#define ISP_API_LSH_NEG_MIN_OFFSET1         (_LSH_START + 0x17)  // [0, 1023], 1
#define ISP_API_LSH_POS_MAX_OFFSET1         (_LSH_START + 0x18)  // [0, 1023], 1
#define ISP_API_LSH_NEG_CLIP_RATIO1         (_LSH_START + 0x19)  // [0, 1023], 1
#define ISP_API_LSH_POS_CLIP_RATIO1         (_LSH_START + 0x1A)  // [0, 1023], 1
#define ISP_API_LSH_DETAIL_NW_ALPHA         (_LSH_START + 0x1B)  // [0, 256], 1
#define ISP_API_LSH_DETAIL_NR_R             (_LSH_START + 0x1C)  // [0, 256], 1
#define ISP_API_LSH_DETAIL_TYPE             (_LSH_START + 0x1D)  // [0, 1], 1
#define ISP_API_LSH_DETAIL_FREQ_OFFSET      (_LSH_START + 0x1E)  // [0, 1024], 1
#define ISP_API_LSH_DETAIL_FREQ_MID         (_LSH_START + 0x1F)  // [0, 11], 1
#define ISP_API_LSH_DETAIL_FREQ_END         (_LSH_START + 0x20)  // [0, 11, 1
#define ISP_API_LSH_DETAIL_FREQ_Y1          (_LSH_START + 0x21)  // [0, 4095], 1
#define ISP_API_LSH_DETAIL_FREQ_Y2          (_LSH_START + 0x22)  // [0, 4095], 1
#define ISP_API_LSH_DETAIL_FREQ_Y3          (_LSH_START + 0x23)  // [0, 4095], 1
#define ISP_API_LSH_DETAIL_LUM_OFFSET       (_LSH_START + 0x24)  // [0, 1024], 1
#define ISP_API_LSH_DETAIL_LUM_MID          (_LSH_START + 0x25)  // [0, 11], 1
#define ISP_API_LSH_DETAIL_LUM_END          (_LSH_START + 0x26)  // [0, 11], 1
#define ISP_API_LSH_DETAIL_LUM_Y1           (_LSH_START + 0x27)  // [0, 4095], 1
#define ISP_API_LSH_DETAIL_LUM_Y2           (_LSH_START + 0x28)  // [0, 4095], 1
#define ISP_API_LSH_DETAIL_LUM_Y3           (_LSH_START + 0x29)  // [0, 4095], 1
#define ISP_API_LSH_NEG_STR2                (_LSH_START + 0x2A)  // [0, 16383], 1
#define ISP_API_LSH_POS_STR2                (_LSH_START + 0x2B)  // [0, 16383], 1
#define ISP_API_LSH_NEG_MIN_OFFSET2         (_LSH_START + 0x2C)  // [0, 1023], 1
#define ISP_API_LSH_POS_MAX_OFFSET2         (_LSH_START + 0x2D)  // [0, 1023], 1
#define ISP_API_LSH_NEG_CLIP_RATIO2         (_LSH_START + 0x2E)  // [0, 1023], 1
#define ISP_API_LSH_POS_CLIP_RATIO2         (_LSH_START + 0x2F)  // [0, 1023], 1
#define ISP_API_LSH_EDGE_NLM_WEIGHT_TABLE   (_LSH_START + 0x30)  // [0, 100], 1
#define _LSH_END                        (_LSH_START + 0x30) //0x11a

#define _YUV_SDE_START                  (_LSH_END + 0x01)  //0x11b
#define ISP_API_SDE_ENABLE                  (_YUV_SDE_START + 0x00)  // [0, 1], 1
#define ISP_API_SDE_LIGHTEN                 (_YUV_SDE_START + 0x01)  // [0, 1], 1
#define ISP_API_SDE_GAILY                   (_YUV_SDE_START + 0x02)  // [0, 1], 1
#define ISP_API_SDE_MONO                    (_YUV_SDE_START + 0x03)  // [0, 1], 1
#define ISP_API_SDE_CONTRAST_HIGH           (_YUV_SDE_START + 0x04)  // [0, 1], 1
#define ISP_API_SDE_BRIGHTNESS              (_YUV_SDE_START + 0x05)  // [0, 4096], 1
#define ISP_API_SDE_SATURATION              (_YUV_SDE_START + 0x06)  // [0, 4096], 1
#define ISP_API_SDE_HUE                     (_YUV_SDE_START + 0x07)  // [0, 59], 1
#define ISP_API_SDE_NEG_EN                  (_YUV_SDE_START + 0x08)  // [0, 1], 1
#define ISP_API_SDE_CLIP_Y_LOW              (_YUV_SDE_START + 0x09)  // [0, 1023], 1
#define ISP_API_SDE_CLIP_Y_HIGH             (_YUV_SDE_START + 0x0A)  // [0, 1023], 1
#define ISP_API_SDE_CONTRAST_ALL            (_YUV_SDE_START + 0x0B)  // [0, 4096], 1
#define ISP_API_SDE_CONTRAST_Y              (_YUV_SDE_START + 0x0C)  // [0, 4096], 1
#define ISP_API_SDE_CONTRAST_U              (_YUV_SDE_START + 0x0D)  // [0, 4096], 1
#define ISP_API_SDE_CONTRAST_V              (_YUV_SDE_START + 0x0E)  // [0, 4096], 1
#define _YUV_SDE_END                    (_YUV_SDE_START + 0x0E) //0x129

/*Misc*/
#define _MISC_START                     (_YUV_SDE_END + 0x01) //0x12a
#define ISP_API_CAC_ENABLE                  (_MISC_START + 0x00)  // [0, 1], 0
#define ISP_API_CAC_BYPASS_H                (_MISC_START + 0x01)  // [0, 1], 0
#define ISP_API_CAC_BYPASS_V                (_MISC_START + 0x02)  // [0, 1], 0
#define ISP_API_CSC1_LIMITE_EN              (_MISC_START + 0x03)  // [0, 1], 0
#define ISP_API_CSC2_LIMITE_EN              (_MISC_START + 0x04)  // [0, 1], 0
#define ISP_API_DIGITAL_GAIN_ENABLE         (_MISC_START + 0x05)  // [0, 1], 0
#define ISP_API_VIN_SAVE_RAW                (_MISC_START + 0x06)  // [0, 511], 0
#define ISP_API_VOUT_0_SAVE_DATA            (_MISC_START + 0x07)  // [0, 511], 0
#define ISP_API_DUMP_VIN_VOUT_DATA          (_MISC_START + 0x08)  // [0, 1], 0

#define ISP_API_PSC_EN                      (_MISC_START + 0x09)  // [0, 1], 0
#define ISP_API_PSC_ROI                     (_MISC_START + 0x0A)  // [0, 8], 1
#define ISP_API_SCA_EN                      (_MISC_START + 0x0B)  // [0, 1], 0
#define ISP_API_SCA_ROI                     (_MISC_START + 0x0C)  // [0, 8], 1
#define ISP_API_SCA_DEWARP                  (_MISC_START + 0x0D)  // [0, 19], 1
#define ISP_API_VIN_ZSL_STORE               (_MISC_START + 0x0E)  // [0, 3], 1
#define ISP_API_VIN_ZSL_LOAD                (_MISC_START + 0x0F)  // [0, 2], 1
#define _MISC_END                       (_MISC_START + 0x0F) //0x139

/*LUT*/
#define _LUT_START                      (_MISC_END + 0x01)  //0x13a
#define ISP_API_AE_STATS_WEIGHT_LUT         (_LUT_START + 0x00)
#define ISP_API_LSC_MESH_LUT_R              (_LUT_START + 0x01)
#define ISP_API_LSC_MESH_LUT_G              (_LUT_START + 0x02)
#define ISP_API_LSC_MESH_LUT_B              (_LUT_START + 0x03)
#define ISP_API_TONE_CURVE_LUT              (_LUT_START + 0x04)
#define ISP_API_COLOR_MATRIX_LUT            (_LUT_START + 0x05)
#define ISP_API_NP_LUT                      (_LUT_START + 0x06)
#define ISP_API_NP_TABLE_STR                (_LUT_START + 0x07)  // [0, 7], 0
#define ISP_API_RNR_RADIAL_LUT              (_LUT_START + 0x08)
#define ISP_API_RNR_WEIGHT_LUT              (_LUT_START + 0x09)
#define ISP_API_CNR_RADIAL_LUT              (_LUT_START + 0x0A)
#define ISP_API_LSH_WEIGHT_LUT              (_LUT_START + 0x0B)
#define _LUT_END                        (_LUT_START + 0x0B) //0x145

/*Feature*/
#define _FEATURE_START                  (_LUT_END + 0x01)  //0x146
#define ISP_API_OSD_MODE                    (_FEATURE_START + 0x00)  // [0, 3], 1
#define ISP_API_TPG_MODE                    (_FEATURE_START + 0x01)  // [0, 7], 1
#define ISP_API_TPG_SFT_SPEED               (_FEATURE_START + 0x02)  // [0, 255], 1
#define ISP_API_IR_PROCESS                  (_FEATURE_START + 0x03)  // [0, 1], 1
#define ISP_API_SCENE_MODE                  (_FEATURE_START + 0x04)  // [0, 14], 0
#define _FEATURE_END                    (_FEATURE_START + 0x04)  //0x14a

/*Debug*/
#define _DEBUG_START                    (_FEATURE_END + 0x01) //0x14b  331
#define ISP_API_ENABLE_AE_IN_RAW_DUMP       (_DEBUG_START + 0x00)  // [0, 1], 0
#define ISP_API_VIN_DUMP_RAW                (_DEBUG_START + 0x01)  // [1, 510], 1
#define ISP_API_VOUT_0_DUMP_YUV420_8BIT     (_DEBUG_START + 0x02)  // [1, 511], 1
#define ISP_API_VOUT_0_DUMP_YUV422_8BIT     (_DEBUG_START + 0x03)  // [1, 511], 1
#define ISP_API_VOUT_0_DUMP_RGB24           (_DEBUG_START + 0x04)  // [1, 511], 1
#define ISP_API_VOUT_0_DUMP_RGB888          (_DEBUG_START + 0x05)  // [1, 511], 1
#define ISP_API_VOUT_0_DUMP_RGB10           (_DEBUG_START + 0x06)  // [1, 511], 1
#define ISP_API_VOUT_0_DUMP_RGB565          (_DEBUG_START + 0x07)  // [1, 511], 1
#define ISP_API_VIN_DDR_PROCESS_ENABLE      (_DEBUG_START + 0x08)  // [0, 1], 1
#define ISP_API_VIN_DDR_PROCESS_NUM         (_DEBUG_START + 0x09)  // [0, 511], 1
#define ISP_API_VIN_DDR_PROCESS_FPS         (_DEBUG_START + 0x0A)  // [0, 30], 1
#define ISP_API_VIN_DDR_LOAD_FRAME          (_DEBUG_START + 0x0B)  // [0, 1], 0

#define ISP_API_VOUT_2_DUMP_RGB24           (_DEBUG_START + 0x0C)  // [1, 511], 1
#define ISP_API_VOUT_3_DUMP_TNR_YUV420_8BIT (_DEBUG_START + 0x0D)  // [1, 511], 1

#define ISP_API_VOUT_0_CUT_16_9             (_DEBUG_START + 0x0E)  // [0, 30], 1
#define ISP_API_VOUT_1_CUT_16_9             (_DEBUG_START + 0x0F)  // [0, 30], 1
#define ISP_API_VOUT_2_CUT_16_9             (_DEBUG_START + 0x10)  // [0, 30], 1
#define ISP_API_VOUT_3_CUT_16_9             (_DEBUG_START + 0x11)  // [0, 30], 1

#define ISP_API_VIN_DUMP_MULTI_CAM_INDEX    (_DEBUG_START + 0x12)  // [0, 4], 1
#define ISP_API_VIN_DUMP_RAW_MODE           (_DEBUG_START + 0x13)  // [0, 7], 1
#define ISP_API_VIN_WIN_SV_DUMP0            (_DEBUG_START + 0x14)  // [0, 4095], 1
#define ISP_API_VIN_WIN_EV_DUMP0            (_DEBUG_START + 0x15)  // [0, 4095], 1
#define ISP_API_VIN_WIN_SH_DUMP0            (_DEBUG_START + 0x16)  // [0, 4095], 1
#define ISP_API_VIN_WIN_EH_DUMP0            (_DEBUG_START + 0x17)  // [0, 4095], 1
#define ISP_API_VIN_WIN_SV_DUMP1            (_DEBUG_START + 0x18)  // [0, 4095], 1
#define ISP_API_VIN_WIN_EV_DUMP1            (_DEBUG_START + 0x19)  // [0, 4095], 1
#define ISP_API_VIN_WIN_SH_DUMP1            (_DEBUG_START + 0x1A)  // [0, 4095], 1
#define ISP_API_VIN_WIN_EH_DUMP1            (_DEBUG_START + 0x1B)  // [0, 4095], 1

#define ISP_API_VIN_LOAD_4CH_RAW            (_DEBUG_START + 0x1C)  // [0, 500], 1
//#define ISP_API_VIN_DUMP_BITDEPTH           (_DEBUG_START + 0x1D)  // [0, 1], 1
#define ISP_API_SCA_ROI_FULL_FUNC           (_DEBUG_START + 0x1E)  // [0, 1], 0
#define ISP_API_SCB_ROI_FULL_FUNC           (_DEBUG_START + 0x1F)  // [0, 1], 0
#define ISP_API_SCD_ROI_FULL_FUNC           (_DEBUG_START + 0x20)  // [0, 1], 0
#define ISP_API_SCE_ROI_FULL_FUNC           (_DEBUG_START + 0x21)  // [0, 1], 0

#define ISP_API_SCB_EN                      (_DEBUG_START + 0x22)  // [0, 1], 0
#define ISP_API_SCB_ROI                     (_DEBUG_START + 0x23)  // [0, 8], 1
#define ISP_API_SCB_DEWARP                  (_DEBUG_START + 0x24)  // [0, 19], 1
#define ISP_API_SCD_EN                      (_DEBUG_START + 0x25)  // [0, 1], 0
#define ISP_API_SCD_ROI                     (_DEBUG_START + 0x26)  // [0, 8], 1
#define ISP_API_SCE_EN                      (_DEBUG_START + 0x27)  // [0, 1], 0
#define ISP_API_SCE_ROI                     (_DEBUG_START + 0x28)  // [0, 8], 1

#define ISP_API_VOUT_1_DUMP_YUV420_8_BIT        (_DEBUG_START + 0x29)  // [1, 511], 1
#define ISP_API_VOUT_2_DUMP_YUV420_8_BIT        (_DEBUG_START + 0x2A)  // [1, 511], 1
#define ISP_API_VOUT_3_DUMP_SCE_YUV420_8_BIT    (_DEBUG_START + 0x2B)  // [1, 511], 1
#define ISP_API_VOUT_SAVE_AT_THE_SAME_TIME      (_DEBUG_START + 0x2C)  // [0, 1], 1

#define ISP_API_VOUT_DUMP_DMS_SELECT_R0         (_DEBUG_START + 0x2D)  // [0, 1], 1
#define ISP_API_VOUT_DUMP_DMS_SELECT_R1         (_DEBUG_START + 0x2E)  // [0, 1], 1
#define ISP_API_VOUT_DUMP_TNR_SELECT_R0         (_DEBUG_START + 0x2F)  // [0, 1], 1
#define ISP_API_VOUT_DUMP_TNR_SELECT_R1         (_DEBUG_START + 0x30)  // [0, 1], 1
#define ISP_API_VOUT_DUMP_CSC2_SELECT_R0        (_DEBUG_START + 0x31)  // [0, 1], 1
#define ISP_API_VOUT_DUMP_CSC2_SELECT_R1        (_DEBUG_START + 0x32)  // [0, 1], 1
#define ISP_API_VOUT_SIMULTANEOUS_DUMP_FRAME    (_DEBUG_START + 0x33)  // [0, 8], 1

#define ISP_API_LSH_DLTI_EN                     (_DEBUG_START + 0x34)  // [0, 1], 1
#define ISP_API_LSH_DLTI_POS_OFFSET             (_DEBUG_START + 0x35)  // [0, 1024], 1
#define ISP_API_LSH_DLTI_POS_MID                (_DEBUG_START + 0x36)  // [0, 11], 1
#define ISP_API_LSH_DLTI_POS_END                (_DEBUG_START + 0x37)  // [0, 11], 1
#define ISP_API_LSH_DLTI_POS_Y1                 (_DEBUG_START + 0x38)  // [0, 256], 1
#define ISP_API_LSH_DLTI_POS_Y2                 (_DEBUG_START + 0x39)  // [0, 256], 1
#define ISP_API_LSH_DLTI_POS_Y3                 (_DEBUG_START + 0x3A)  // [0, 256], 1
#define ISP_API_LSH_DLTI_NEG_OFFSET             (_DEBUG_START + 0x3B)  // [0, 1024], 1
#define ISP_API_LSH_DLTI_NEG_MID                (_DEBUG_START + 0x3C)  // [0, 11], 1
#define ISP_API_LSH_DLTI_NEG_END                (_DEBUG_START + 0x3D)  // [0, 11], 1
#define ISP_API_LSH_DLTI_NEG_Y1                 (_DEBUG_START + 0x3E)  // [0, 256], 1
#define ISP_API_LSH_DLTI_NEG_Y2                 (_DEBUG_START + 0x3F)  // [0, 256], 1
#define ISP_API_LSH_DLTI_NEG_Y3                 (_DEBUG_START + 0x40)  // [0, 256], 1

/// read only API for check tune version
#define ISP_API_CALI_VERSION                    (_DEBUG_START + 0x41)  // [0, 4294967295], 1
#define ISP_API_SW3A_TUNE_VERSION               (_DEBUG_START + 0x42)  // [0, 4294967295], 1
#define ISP_API_ISP_TUNE_VERSION                (_DEBUG_START + 0x43)  // [0, 4294967295], 1


#define _DEBUG_END                              (_DEBUG_START + 0x43)

//Parsing_End
#if !defined (HW_BUILD_FOR_CUST0)
#define ISP_API_TOTAL_SIZE                  (_DEBUG_END + 0x01)
#else
#include "fw_isp_api_i5_cust0_ttnr.h"

#define _DBG_VOUT_START                         (_TTNR_END       + 0x01)
#define ISP_API_VOUT_DUMP_TIMEOUT               (_DBG_VOUT_START + 0x01)
#define ISP_API_VOUT_DUMP_WRDONE                (_DBG_VOUT_START + 0x02)
#define _DBG_VOUT_END                           (_DBG_VOUT_START + 0x10)

#define ISP_API_TOTAL_SIZE                      (_DBG_VOUT_END + 0x01)
#endif

typedef struct
{
    u32 ispApiValTab[ISP_API_TOTAL_SIZE];
} isp_api_t;

#endif

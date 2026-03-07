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

#ifndef __CALIBRATION_COMMON_H__
#define __CALIBRATION_COMMON_H__

#include "tune_utils.h"

#include "isp_ttnr_tuning_cal_536.h"
/***************************************************
*                       BLC                         *
 ***************************************************/
typedef struct
{
    u32 calNormalBlcR;      /// default 0, [0:65535]
    u32 calNormalBlcGr;     /// default 0, [0:65535]
    u32 calNormalBlcGb;     /// default 0, [0:65535]
    u32 calNormalBlcB;      /// default 0, [0:65535]
    u32 calLncBlcL;         /// default 0, [0:4095]
    u32 calLncBlcM;         /// default 0, [0:4095]
    u32 calLncBlcS;         /// default 0, [0:4095]
    u32 calLncBlcVs;        /// default 0, [0:4095]
    u32 calDolLBlcR;        /// default 0, [0:4095]
    u32 calDolLBlcGr;       /// default 0, [0:4095]
    u32 calDolLBlcGb;       /// default 0, [0:4095]
    u32 calDolLBlcB;        /// default 0, [0:4095]
    u32 calDolMBlcR;        /// default 0, [0:4095]
    u32 calDolMBlcGr;       /// default 0, [0:4095]
    u32 calDolMBlcGb;       /// default 0, [0:4095]
    u32 calDolMBlcB;        /// default 0, [0:4095]
    u32 calDolSBlcR;        /// default 0, [0:4095]
    u32 calDolSBlcGr;       /// default 0, [0:4095]
    u32 calDolSBlcGb;       /// default 0, [0:4095]
    u32 calDolSBlcB;        /// default 0, [0:4095]
} cal_blc_param_by_gain_t;

typedef struct
{
    DYNAMIC_TABLE(cal_blc_param_by_gain_t, calBlcGainTab);
} cal_blc_param_t;

/***************************************************
*                       NOISE                       *
 ***************************************************/
typedef struct
{
    u32 calNoiseProfile[128];
} cal_noise_param_t;

/***************************************************
*                       LSC                         *
 ***************************************************/
typedef struct
{
    u32 calLscMeshScale;
    u8  calLscMeshFrac;                 /// default 0,[0:7] Manual, LSC gain accuracy for mesh mode   (to be used)
    u8  calLscRadialFrac;               /// default 6,[0:7] Manual, LSC gain accuracy for radial mode (to be used)
    u32 calLscCenRow;
    u32 calLscCenCol;
    u32 calLscMeshLutRHigh[1024];
    u32 calLscMeshLutGHigh[1024];
    u32 calLscMeshLutBHigh[1024];
    u32 calLscMeshLutRMid[1024];
    u32 calLscMeshLutGMid[1024];
    u32 calLscMeshLutBMid[1024];
    u32 calLscMeshLutRLow[1024];
    u32 calLscMeshLutGLow[1024];
    u32 calLscMeshLutBLow[1024];
    u32 calLscGainLutR[33];
    u32 calLscGainLutGr[33];
    u32 calLscGainLutGb[33];
    u32 calLscGainLutB[33];
    u32 calLscLocLut[32];
    u32 calLscLog2Lut[32];
    u32 calLscMeshRGPos[3];
    u32 calLscMeshBGPos[3];
} cal_lsc_param_t;

/***************************************************
*                       WB                          *
 ***************************************************/
#define AWB_EXTRA_LIGHT_MAX 1

typedef struct
{
    u32 rgPos[15];
    u32 bgPos[15];
    u32 rgbgWeightMesh[225];
    u32 colorTempMesh[225];
    u32 extraLightNum;
    u32 extraLight[2 * AWB_EXTRA_LIGHT_MAX];
    u32 extraLightMesh[225];
}cal_awb_mesh_t;

typedef struct
{
    u32 calWbStaticWb[5];  /// Actually just use 1st ~ 4th value, because calWbStaticWb[4] as awbConvergeStep is used by awb tuning.
    u32 calWBLuxThr[2];    /// use to interpolation low and high lux calibration data
    cal_awb_mesh_t awbMeshLow;  /// low lux index awb calibration data
    cal_awb_mesh_t awbMeshHigh; /// high lux index awb calibration data
    DYNAMIC_TABLE(rgb_t, awbLightSource); /// calibration light source average r/g/b value of gray chart
    DYNAMIC_TABLE(rgb_t, darkSkin); /// calibration light source average r/g/b value of 24 colorchart dark skin color
    DYNAMIC_TABLE(rgb_t, lightSkin); /// calibration light source average r/g/b value of 24 colorchart light skin color
    DYNAMIC_TABLE(awb_gain_t, awbOtpGain); /// otp gain for unit module
} cal_wb_param_t;

/***************************************************
*                       CCM                         *
 ***************************************************/
typedef struct
{
    u32   validRowSize;
    u32   calColorMatrix[10][9];
    u32   calColorMatrixRGPos[10];
    u32   calColorMatrixBGPos[10];
} cal_color_tbl_t;

typedef struct
{
    cal_color_tbl_t calColorMatrixTab;
} cal_ccm_param_t;

/***************************************************
*                       GGC                         *
 ***************************************************/
typedef struct
{
    u32   validRowSize;
    u32   calToneCurve[20][129];
} cal_tone_curve_tbl_t;

typedef struct
{
    cal_tone_curve_tbl_t calToneCurveTab;
} cal_ggc_param_t;

/***************************************************
*                       LNC                         *
 ***************************************************/
typedef struct
{
    u32 calLncCh1[(353+1)/2];     /// R
    u32 calLncCh2[(353+1)/2];     /// G
    u32 calLncCh3[(353+1)/2];     /// B
} cal_lnc_param_t;

/***************************************************
*                      DEWARP                       *
 ***************************************************/
typedef struct
{
    u32 calDewarpTab3072[3072/2];
    u32 calDewarpTabUP[64/2];
    u32 calDewarpTabDW[64/2];
    u32 calDewarpTabLF[48/2];
    u32 calDewarpTabRG[48/2];
    u32 calDewarpTabCOR[4/2];
    u32 calDewarpTabIni[48];
    u32 calDewarpTabIniPad2[2];
} cal_dewarp_param_t;

/***************************************************
*                       CAC                         *
 ***************************************************/
typedef struct
{
    u32 calCac[64*64];
} cal_cac_param_t;

/***************************************************
*                       IR                         *
 ***************************************************/
typedef struct
{
    u32 calNormalBlcR;       /// default 0, [0:4095]
    u32 calNormalBlcG;       /// default 0, [0:4095]
    u32 calNormalBlcB;       /// default 0, [0:4095]
    u32 calNormalBlcIR;      /// default 0, [0:4095]
} cal_ir_blc_param_by_gain_t;

typedef struct
{
    u32   validRowSize;
    u32   calColorMatrix[10][12];
    u32   calColorMatrixRGPos[10];
    u32   calColorMatrixBGPos[10];
} cal_ir_color_tbl_t;

typedef struct
{

    BOOL isValid;
    cal_ir_color_tbl_t calColorMatrixTab;
    DYNAMIC_TABLE(cal_ir_blc_param_by_gain_t, calBlcGainTab);
} cal_ir_param_t;

/***************************************************
*                    Calibration                    *
 ***************************************************/
typedef struct
{
    u32 reserved[10];
} calibration_reserved_params_t;

typedef struct
{
    tune_version_t                  calVersion;
    cal_blc_param_t                 blc;
    cal_noise_param_t               noise;
    cal_lsc_param_t                 lsc;
    cal_wb_param_t                  wb;
    cal_ccm_param_t                 ccm;
    cal_ggc_param_t                 ggc;
    cal_lnc_param_t                 lnc;
    cal_dewarp_param_t              dewarp;
    cal_cac_param_t                 cac;
    cal_ir_param_t                  ir;
    
    cal_ttnr_param_t ttnr;

    calibration_reserved_params_t   resv;
} calibration_params_t;

typedef calibration_params_t* (*load_calibration_func)(const char *filename);

#endif

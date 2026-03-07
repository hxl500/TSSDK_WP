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

#ifndef __TUNE_ISP_GGC_V0_H__
#define __TUNE_ISP_GGC_V0_H__

#if (defined(HW_BUILD_FOR_I5)       ||\
     defined(HW_BUILD_FOR_I5_R1P0)  ||\
     defined(HW_BUILD_FOR_I3)       ||\
     defined(HW_BUILD_FOR_CUST0))

#include "tune_utils.h"

/***************************************************
*                      GGC                          *
 ***************************************************/
typedef struct
{
    u32 ggcContrastAutoStr;         /// default 250, [0:2000], Target of global contrast in auto mode
    u32 ggcLocalLumAdjNode0;        /// default 50, [0:100], Lum adjust in node 0
    u32 ggcLocalLumAdjNode1;        /// default 50, [0:100], Lum adjust in node 1
    u32 ggcLocalLumAdjNode5;        /// default 50, [0:100], Lum adjust in node 5
    u32 ggcLocalLumAdjNode6;        /// default 50, [0:100], Lum adjust in node 6
} ggc_core_param_by_lux_t;

typedef struct
{
    /// static parameters
    BOOL ggcTuneEnable;             /// default 1, [0:1], GGC tuning enable/disable
    u16  ggcLowisoThr;              /// default 100, [100:51200], GGC curve switch lower ISO threshold
    u16  ggcHighisoThr;             /// default 100, [100:51200], GGC curve switch upper ISO threshold
    u8   ggcConvergeSpeed;          /// default 0, [10:100], Auto GGC curve switch convergence speed (100 as fastest)
    u8   ggcBasicSelection;         /// default 0, [0:19], Several preset GGC cuve selection, refer to calibration param struct CalibrationToneCurve, if exceed max index will enable cuve interpolation
    u8   ggcContrastEnhanceEnable;  /// default 2, [0:2], [0]Basic, [1]GCC, [2]GCE
    u8   ggcContrastManualStr;      /// default 20, [0:100], Global contrast strength(not used in auto mode)
    u16  ggcDehazeStrengthLight;    /// default 0, [0:8192], Hist cut in light area
    u16  ggcDehazeStrengthDark;     /// default 0, [0:8192], Hist cut in dark area
    u8   ggcLocalLumAdjNode0;       /// default 50, [0:100], Lum adjust in node 0
    u8   ggcLocalLumAdjNode1;       /// default 50, [0:100], Lum adjust in node 1
    u8   ggcLocalLumAdjNode2;       /// default 50, [0:100], Lum adjust in node 2
    u8   ggcLocalLumAdjNode3;       /// default 50, [0:100], Lum adjust in node 3
    u8   ggcLocalLumAdjNode4;       /// default 50, [0:100], Lum adjust in node 4
    u8   ggcLocalLumAdjNode5;       /// default 50, [0:100], Lum adjust in node 5
    u8   ggcLocalLumAdjNode6;       /// default 50, [0:100], Lum adjust in node 6
    u8   ggcLocalAdaptiveNode0;     /// default 0, [0:100], Local adaptive in node 0
    u8   ggcLocalAdaptiveNode1;     /// default 0, [0:100], Local adaptive in node 1
    u8   ggcLocalAdaptiveNode2;     /// default 0, [0:100], Local adaptive in node 2
    u8   ggcLocalAdaptiveNode3;     /// default 0, [0:100], Local adaptive in node 3
    u8   ggcLocalAdaptiveNode4;     /// default 0, [0:100], Local adaptive in node 4
    u8   ggcLocalAdaptiveNode5;     /// default 0, [0:100], Local adaptive in node 5
    u8   ggcLocalAdaptiveNode6;     /// default 0, [0:100], Local adaptive in node 6
    u16  ggcGlobalLuminationAdj;    /// default 128, [0:256], Global lumination adjust
    BOOL ggcContrastAutoMode;       /// default 1, [0:1], Enable of global contrast auto mode
    u16  ggcContrastAutoStr;        /// default 250, [0:2000], Target of global contrast in auto mode
    u16  ggcContrastSmoothRate;     /// default 220, [0:256], Smooth rate when scene changes

    /// dynamic parameters
    DYNAMIC_TABLE(ggc_core_param_by_lux_t, ggcLuxInterpTab);
} tune_gamma_param_t;

#endif

#endif

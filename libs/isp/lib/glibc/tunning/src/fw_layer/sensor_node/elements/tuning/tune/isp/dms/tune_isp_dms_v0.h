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

#ifndef __TUNE_ISP_DMS_V0_H__
#define __TUNE_ISP_DMS_V0_H__

#if (defined(HW_BUILD_FOR_I5)       ||\
     defined(HW_BUILD_FOR_I5_R1P0)  ||\
     defined(HW_BUILD_FOR_I3)       ||\
     defined(HW_BUILD_FOR_CUST0))

#include "tune_utils.h"

/***************************************************
*                      DMS                          *
 ***************************************************/
typedef struct
{
    u32 dmsNpOffAbs;            /// default 1, [0:1], Noise profile offset mode switch (This will be Manual in future version)
    u32 dmsSharpenEn;           /// default 1, [0:1], DMS sharpen tuning enable/disable
    u32 dmsHfDetailEnhEn;       /// default 1, [0:1], DMS high frequency detail information enhance tuning enable/disable
} dms_switch_param_by_gain_t;

typedef struct
{
    u32  dmsNpOff;              /// default 40, [0:255], Noise profile offset value
    u32  dmsGreySmalliasSlp;    /// default 5000, [0:8191], Grey small alias slope value
    u32  dmsGreenUseRbThr;      /// default 253, [0:4095], Green small alias threshold value (This will be Manual in future version)
    u32  dmsGreenUseRbSlp;      /// default 1350, [0:8191], Green small alias slope value (This will be Manual in future version)
    u32  dmsDxSlp;              /// default 235, [0:255], Diagonal direction interpolation strength slope value
    u32  dmsDxThr;              /// default 100, [0:4095], Diagonal direction interpolation strength threshold value
    u32  dmsDpSlp;              /// default 255, [0:255], Polaris direction interpolation strength slope value
    u32  dmsDpThr;              /// default 200, [0:4095], Polaris direction interpolation strength threshold value
    u32  dmsDuSlp;              /// default 64, [0:255], Directional/Undirectional blending strength slope value
    u32  dmsDuThr;              /// default 50, [0:256], Directional/Undirectional blending strength threshold value
    u32  dmsDuShSlp;            /// default 100, [0:65535]
    u32  dmsDuShThresh;         /// default 100, [0:65535]
    u32  dmsDpxSlp;             /// default 160, [0:255], Direction interpolation blending slope value
    u32  dmsDpxThr;             /// default 50, [0:4095], Direction interpolation blending strength threshold value
    u32  dmsFcSlp;              /// default 255, [0:512], False Color Removal strength
    u32  dmsAliasThresh;        /// default 90, [0:511]
    u32  dmsAliasSlope;         /// default 150, [0:65535]
    u32  dmsBldaliasThr;        /// default 435, [0:1024], Alias blending strength threshold value
    u32  dmsBldaliasSlp;        /// default 201, [0:255], Alias blending strength slope value
    u32  dmsSatThr;             /// default 369, [0:4095], sat threshold value (This will be Manual in future version)
    u32  dmsSatSlp;             /// default 178, [0:255], sat slope value (This will be Manual in future version)
    u32  dmsSharpenStr;         /// default 4096, [0:16384], DMS sharpen strength
    u32  dmsHfDetailEnhStr;     /// default 96, [0:255], DMS high frequency detail information enhance strength
    u32  dmsFcrateStr;          /// default 0, [0:3], False Color Rate Strength selection
} dms_core_param_by_gain_t;

/// DMS NP region params auto by Gain
typedef struct
{
    u32 dmsNpLutTab[128];
} dms_np_lut_by_gain_t;

typedef struct
{
    /// static parameters
    BOOL dmsTuneEnable;            /// default 1,  [0:1], DMS tuning enable/disable
    u32  dmsSmallaliasDetThresh; /// default 0,  [0:65535]
    BOOL dmsBdOption;            /// default 1,  [0:1]
    u32  dmsDuShOffset;          /// default 75, [0:65535]

    /// dynamic parameters
    DYNAMIC_TABLE(dms_switch_param_by_gain_t, dmsGainSwitchTab);
    DYNAMIC_TABLE(dms_core_param_by_gain_t, dmsGainInterpTab);
    DYNAMIC_TABLE(dms_np_lut_by_gain_t, dmsNpLutTab);
} tune_dms_param_t;

#endif

#endif

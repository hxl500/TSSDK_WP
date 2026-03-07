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
 * Copyright (C) 2020-2021 Arm Technology (China) Co. Ltd. All rights reserved.
 *
 ***********************************************************************************************/

#ifndef __TUNE_UTILS_H__
#define __TUNE_UTILS_H__

#include "types_utils.h"

#define CONVERAGE_TAB_MAX_SIZE (15)

/// @TODO: need define a new dynamic table without region
#define DYNAMIC_TABLE(PARAM_T, TAB_NAME)            \
struct                                              \
{                                                   \
    u32  validSize;                                 \
    struct                                          \
    {                                               \
        u32     region;                             \
        /* Require type of u32 for each param */    \
        PARAM_T params;                             \
    } tab[CONVERAGE_TAB_MAX_SIZE];                  \
} TAB_NAME

#define DYNAMIC_TABLE_PARAM_SIZE(a) (sizeof(a.tab[0].params) / sizeof(u32))

typedef struct
{
    u32   validSize;

    /// to solve alignment issue on 64bit system, use s32 array instead of void*
    s32   data[1];
} tune_trigger_tab_t;

#define TUNE_ALG_NAME_SIZE_MAX (32)
#define TUNE_ALG_SW3A_NUM_MAX (3)
typedef enum {
  TUNE_ALG_TYPE_INVAL = 0,
  TUNE_ALG_TYPE_3A = 1,
  TUNE_ALG_TYPE_ISP = 2,
} tune_alg_type_e;

typedef struct {
  char8 name[TUNE_ALG_NAME_SIZE_MAX];
} tune_alg_attr_t;

typedef enum {
  TUNE_LUT_TYPE_NOR = 0,
  TUNE_LUT_TYPE_CAL = 1,
  TUNE_LUT_TYPE_NUM,
} tune_lut_type_t;

/// tune version default 0 1 0 0
typedef struct
{
    u32  major; /// major version number
    u32  minor; /// minor version number
    u32  debug; /// debug used
    u32  check; /// checksum field
} tune_version_t;

/// define cct trigger region
typedef struct
{
    u32 cctStart; /// trigger region start
    u32 cctEnd;   /// trigger region end
}cct_trigger_t;

/// define gain trigger region
typedef struct
{
    u32 gainStart; /// gain region start
    u32 gainEnd;   /// gain resion end
}gain_trigger_t;

/// define lux index trigger region
typedef struct
{
    u32 luxStart; /// lux index region start
    u32 luxEnd;   /// lux index region end
}luxidx_trigger_t;

/// define ratio trigger region
typedef struct
{
    u32 ratioStart; /// ratio region start
    u32 ratioEnd;   /// ratio region end
}ratio_trigger_t;

/// Trigger type
typedef enum
{
    CCT     = 0x01, /// Interpolation or trigger by Correlated color temperature
    GAIN    = 0x02, /// Interpolation or trigger by Gain
    LUXIDX  = 0x04,  /// Interpolation or trigger by Lux Index
    RATIO   = 0x08  /// Interpolation or trigger by Ratio Index
}trigger_type_t;

/// Define all trigger region
typedef union
{
    cct_trigger_t     cct;      /// cct trigger region
    gain_trigger_t    gain;     /// gain trigger region
    luxidx_trigger_t  luxIdx;   /// lux index trigger region
    ratio_trigger_t   ratio;    /// ratio trigger region
}trigger_region_t;

/// Define all trigger region
typedef union
{
    cct_trigger_t     cct;      /// cct trigger region
    gain_trigger_t    gain;     /// gain trigger region
    luxidx_trigger_t  luxIdx;   /// lux index trigger region
    ratio_trigger_t   ratio;    /// ratio trigger region
}trigger_t;

/// This structure define for Region of interest
typedef struct
{
    u32 left;   /// ROI left axis
    u32 top;    /// ROI top axis
    u32 width;  /// ROI width
    u32 height; /// ROI height
} roi_t;

/// This structure define for crop region
typedef struct
{
    u32 xStart; /// crop region x axis start point
    u32 yStart; /// crop region y axis start point
    u32 xEnd;   /// crop region x axis end point
    u32 yEnd;   /// crop region y axis end point
} crop_t;

typedef struct
{
    u32 r;
    u32 g;
    u32 b;
} rgb_t;

typedef struct __awb_gain_t
{
    u32 rGain;
    u32 gGain;
    u32 bGain;
} awb_gain_t;

#endif

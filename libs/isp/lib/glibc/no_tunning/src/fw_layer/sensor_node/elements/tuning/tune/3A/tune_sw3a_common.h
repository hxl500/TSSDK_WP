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

#ifndef __TUNE_SW3A_COMMON_H__
#define __TUNE_SW3A_COMMON_H__

#include "types_utils.h"
////////////////////////  AEC  ///////////////////////////
#define AE_UPDATE_BUFFER_LEN (4)   /// must >= 4

typedef enum
{
    EXP_METERRING_MODE_CENTER           = 0,
    EXP_METERRING_MODE_AVERAGE,
    EXP_METERRING_MODE_UP_TO_DOWN,
    EXP_METERRING_MODE_DOWN_TO_UP,
    EXP_METERRING_MODE_LEFT_TO_RIGHT,
    EXP_METERRING_MODE_RIGHT_TO_LEFT,
    EXP_METERRING_MODE_MAX,
} exp_meterring_mode_t;

/// AEC converge params no need trigger by lux
typedef struct
{
    u32  aeConvergeStep;         /// default 6,[3:32] AE convergence speed
    u32  aeMaxConvSpeed;         /// default 60,[30 : 200] 100 base percent
    u32  aeStableInRange;        /// default 4,[2:255] AE convergence stability range in
    u32  aeStableOutRange;       /// default 32,[5:512] AE convergence stability range out
    u32  aeSaturationThr;        /// default 960,[0:1023] AE Saturation Threshold, use to calculation linear Y
    u32  aeStatsYIdx;            /// default 3,[0,3] which stats Y use to judge ae stable out
    u32  aeChgFrameCnt;          /// default 15,[0, 4096] AE change time out frames
}ae_converge_param_t;

/// AEC target  parameters, trigger by Lux
typedef struct
{
    u32  aeTarget;               /// default 250,[0:1023] base brightness target
    u32  aeHighContrTarget;      /// default 60,[0:100] high dynamic range target
    u32  aeLowLevel;             /// default 15000,[0:100000] dark area percent 100000 is 100%
    u32  aeHighLevel;            /// default 95000,[0:100000] bright area percent 100000 is 100%
    u32  aeLowContrast;          /// default 15,[0:255] dynamic range lower bound
    u32  aeHighContrast;         /// default 25,[0:255] dynamic range upper bound
    u32  aeLongTarget;           /// default 40,[0:100] HDR Long Target for DOL2/3/4
    u32  aeLmRatioMax;           /// default 32,[1:64] HDR Max L/M ratio
    u32  aeMidTarget;            /// default 40,[0:100] HDR Mid Target for DOL3/4
    u32  aeMsRatioMax;           /// default 32,[1:64] HDR Max M/S ratio
    u32  aeShortTarget;          /// default 40,[0:100] HDR Short Target for DOL4
    u32  aeSvsRatioMax;          /// default 32,[1:64] HDR Max S/VS ratio
    u32  aeSaturationMaxPercent; /// default 10240, [0:102400]AE Saturation Max Pcent use for Long base DOL, 1024 is %1
} ae_target_param_t;

/// AEC ROI information
typedef struct
{
    BOOL    aeManualRoiEnable;     /// default 0,[0:1] Manual, Enable manual set AE process ROI
    roi_t   roi;                   /// default left/top 1,[0:14] w/h 13,[1:15]Manual, ROI 0-14 block number
} aec_roi_t;

/// AEC Weight table
typedef struct
{
    u8      aeWeightTabSel;        /// default 0,[0:5] Manual, AE weight table selection, refer to tuning param
    u32     tab[EXP_METERRING_MODE_MAX][225];
} aec_weight_tab_t;

typedef struct _knee_point_t
{
    u32 frameRate;
    u32 totalGain;
    u32 incrementPriority; /// 0:exp  1:gain
}aec_exp_knee_point_t;


/// Face AEC Converge parameters, trigger by Lux
typedef struct
{
    u32     faceAeStatsSel;         /// default 1, [0]RAW, [1]YUV
    u32     faceAeConvergeStep;     /// default 6,[1:32] AE convergence speed
    u32     faceAeStableRange;      /// default 2,[1:255] AE convergence stability range
    u32     faceAeRawTarget;        /// default 250,[0:1023] Auto exposure target - Base  brightness target
    u32     faceAeYuvTarget;        /// default 250,[0:1023] Auto exposure target - Base  brightness target
    u32     faceAeScale;            /// default 100,[10:1000]
    u32     faceAeCropTop;          /// default 1,[0:50]
    u32     faceAeCropBot;          /// default 1,[0:50]
    u32     faceAeCropLef;          /// default 1,[0:50]
    u32     faceAeCropRig;          /// default 1,[0:50]
    u32     faceAeInCntThr;         /// (face in count(1-100) greater than this value, judge as had face enter)
    u32     faceAeOutCntThr;        /// (face out count(1-100) greater than this value, judge as face disappear)
    u32     faceAeSmallSizeThr;     /// (face size (face_width * face_height / img_width * img_height)
                                    ///  less than this value, judge as small face.permillage 10:1-1000)
} face_aec_converge_params_t;

/// AEC tuning paramters structure
typedef struct
{
    BOOL                      aeEnable;               /// default 1, [0]disable, [1]enable
    BOOL                      aeHdrExpBaseSel;        /// default 1,[0:1] Manual, [0]short base exposure, [1]long base exposure
    u8                        aeStatsSel;             /// default 2,[0:3] Manual, AE statistics information position selection, [0]AE0, [1]AE1, [2]AE2, [3]AE3
    BOOL                      antiFlickerEnable;      /// default 0,[0:1] Manual, [0]Anti flicker disable,  [1]Anti flicker enable, 81:
    u8                        antiFlickerFrequency;   /// default 50,[40:70] Manual, Anti flicker frequency setting  (ex: 50/60), 82:
    u16                       sensitivityRatio;       /// default 4096,[410:40960] manual, lux index compensation to reference module, value is 4096*refY/testY
    BOOL                      exposureTableEnable;    /// default 1, enable use exposure table do ae converge
    aec_roi_t                 aeRoi;                  /// default left/top 1,[0:14] w/h 13,[1:15]Manual, ROI 0-14 block number
    aec_weight_tab_t          aeWeightTab;
    ae_converge_param_t       aeConvParam;

    DYNAMIC_TABLE(ae_target_param_t, aecLuxConvTabSBase);
    DYNAMIC_TABLE(ae_target_param_t, aecLuxConvTabLBase);
    DYNAMIC_TABLE(ae_target_param_t, aecLuxConvTabSBaseHlSup);
    DYNAMIC_TABLE(face_aec_converge_params_t, faceAecLuxConvTab);
    DYNAMIC_TABLE(aec_exp_knee_point_t, aecExpKneePoint);
} tune_aec_param_t;

////////////////////////  AWB  ///////////////////////////
#define AWB_MAX_DETECT_ZONE        10
#define AWB_MAX_MISLEAD_COLOR_ZONE 10
#define AWB_MAX_EXTRA_COLOR_ZONE   10

typedef struct
{
    BOOL    awbManualRoiEnable;     /// default 0,[0:1] Manual, Enable manual set AWB process ROI
    roi_t   roi;                    /// default left/top 1,[0:14] w/h 13,[1:15]Manual, ROI 0-14 block number
} awb_roi_t;

typedef struct _awb_detect_zone_t
{
    u32     enable;       /// default 0, [0]disable, [1]enable
    u32     luxStart;     /// lux index region start [1:1000]
    u32     luxEnd;       /// lux index region end [1:1000]
    u32     luxBuf;       /// lux index left and right buffer [1:256]
    s32     left;         /// zone left [-1000:1000]
    s32     right;        /// zone right [-1000:1000]
    s32     top;          /// zone top [-1000:1000]
    s32     bottom;       /// zone bottom [-1000:1000]
}awb_detect_zone_t;

typedef struct _awb_mislead_color_zone_t
{
    u32     enable;         /// default 0, [0]disable, [1]enable
    u32     zoneIdx;        /// detect color zone index [0:16][0:6] is H/A/TL84/CWF/D65/DF/SHADE
    u32     luxStart;       /// lux index region start [1:1000]
    u32     luxEnd;         /// lux index region end [1:1000]
    u32     luxBuf;         /// lux index left and right buffer [1:256]
    u32     detectPrcThr;   /// detect point percent threshold [0:1024]
    u32     detectPrcBuf;   /// detect point percent buffer [0:512]
    s32     left;           /// zone left [-1000:1000]
    s32     right;          /// zone right [-1000:1000]
    s32     top;            /// zone top [-1000:1000]
    s32     bottom;         /// zone bottom [-1000:1000]
}awb_mislead_color_zone_t;

typedef struct _awb_prefcolor_zone_t
{
    u32     enable;       /// default 0, [0]disable, [1]enable
    u32     lightIdx;     /// default 0 [0: 6] light source index [0:6] H/A/TL84/CWF/D65/DF/SHADE
    u32     luxStart;     /// lux index region start [1:1000]
    u32     luxEnd;       /// lux index region end [1:1000]
    u32     luxBuf;       /// lux index left and right buffer [1:256]
    u32     xOffsetThr;   /// default 0[0: 256] percent of light source area width, x large than Thr then remapping x
    u32     yOffsetThr;   /// default 0[0: 256] percent of light source area heihgt, y large than Thr then remapping y
    u32     xClipRatio;   /// default 0 [0: 256] daylight locus offset clip ratio
    u32     yClipRatio;   /// default 0 [0: 256] green and magenta offset clip ratio
    u32     shiftRGain;   /// default 256 [128: 512]
    u32     shiftBGain;   /// default 256 [128: 512]
}awb_prefcolor_zone_t;

typedef struct _awb_extra_color_zone_t
{
    u32     enable;         /// default 0, [0]disable, [1]enable
    u32     luxStart;       /// lux index region start [1:1000]
    u32     luxEnd;         /// lux index region end [1:1000]
    u32     luxBuf;         /// lux index left and right buffer [1:256]
    u32     detectPrcThr;   /// detect point percent threshold [0:1024]
    u32     detectPrcBuf;   /// detect point percent buffer [0:512]
    s32     left;           /// zone left [-1000:1000]
    s32     right;          /// zone right [-1000:1000]
    s32     top;            /// zone top [-1000:1000]
    s32     bottom;         /// zone bottom [-1000:1000]
    u32     lightIdx;       /// light source index [0:7] H/A/TL84/CWF/D65/DF/SHADE/NULL
    u32     weight;         /// extra color weight[0:256]
    u32     shiftRGain;     /// default 256 [128: 512]
    u32     shiftBGain;     /// default 256 [128: 512]
}awb_extra_color_zone_t;

typedef struct _awb_statistic_shift_t
{
    u32     enable;           /// default 0, [0]disable, [1]enable
    u32     lightIdx;         /// light source index [0:6] H/A/TL84/CWF/D65/DF/SHADE
    u32     luxStart;         /// lux index region start [1:1000]
    u32     luxEnd;           /// lux index region end [1:1000]
    u32     luxBuf;           /// lux index left and right buffer [1:256]
    u32     limitYPrc;        /// default 0 [0: 256] projection indide shrink 256 is 100%
    u32     weightReducePrc;  /// default 0[0: 256] weight reduce percent 256 is 100%
}awb_statistics_shift_t;

typedef struct _awb_light_source_area_param_t
{
    u32     enable;        /// default 0, [0]disable, [1]enable
    u32     lightIdx;      /// default 0 [0: 6] reference light source index
    u32     luxStart;      /// lux index region start [1:1000]
    u32     luxEnd;        /// lux index region end [1:1000]
    u32     luxBuf;        /// lux index left and right buffer [1:256]
    u32     scaleLeft;     /// default 256 [0: 512] light source area left scale 256 is 1x, 512 is up scale 2x, 0 is down scale 2x
    u32     scaleRight;    /// default 256 [0: 512] light source area right scale 256 is 1x, 512 is up scale 2x, 0 is down scale 2x
    u32     scaleTop;      /// default 256 [0: 512] light source area top scale 256 is 1x, 512 is up scale 2x, 0 is down scale 2x
    u32     scaleBottom;   /// default 256 [0: 512] light source area bottom scale 256 is 1x, 512 is up scale 2x, 0 is down scale 2x
}awb_light_source_area_param_t;

typedef struct _awb_light_source_prob_t
{
    u32     probH;         /// default 256 [0: 256]
    u32     probA;         /// default 256 [0: 256]
    u32     probTL84;      /// default 256 [0: 256]
    u32     probCWF;       /// default 256 [0: 256]
    u32     probD65;       /// default 256 [0: 256]
    u32     probDF;        /// default 256 [0: 256]
    u32     probShade;     /// default 256 [0: 256]
}awb_light_source_prob_t;

typedef struct _awb_special_gain_t
{
    u32     specialHGainR;       /// default 256 [256: 1024] use for H
    u32     specialHGainB;       /// default 256 [256: 1024] use for H
    u32     specialAGainR;       /// default 256 [256: 1024] use for A
    u32     specialAGainB;       /// default 256 [256: 1024] use for A
    u32     specialTL84GainR;    /// default 256 [256: 1024] use for TL84
    u32     specialTL84GainB;    /// default 256 [256: 1024] use for TL84
    u32     specialCWFGainR;     /// default 256 [256: 1024] use for CWF
    u32     specialCWFGainB;     /// default 256 [256: 1024] use for CWF
    u32     specialD65GainR;     /// default 256 [256: 1024] use for D65
    u32     specialD65GainB;     /// default 256 [256: 1024] use for D65
    u32     specialDFGainR;      /// default 256 [256: 1024] use for DF
    u32     specialDFGainB;      /// default 256 [256: 1024] use for DF
    u32     specialShadeGainR;   /// default 256 [256: 1024] use for Shade
    u32     specialShadeGainB;   /// default 256 [256: 1024] use for Shade
}awb_special_gain_t;

typedef struct _awb_special_gain_weight_t
{
    u32     weightH;         /// default 0 [0: 256]
    u32     weightA;         /// default 0 [0: 256]
    u32     weightTL84;      /// default 0 [0: 256]
    u32     weightCWF;       /// default 0 [0: 256]
    u32     weightD65;       /// default 0 [0: 256]
    u32     weightDF;        /// default 0 [0: 256]
    u32     weightShade;     /// default 0 [0: 256]
}awb_special_gain_weight_t;

typedef struct _awb_non_neutral_predict_t
{
    u32 neutralPointPrcThrL;     /// default 10,  [0:256] neutral point percent less than neutralPointPrcThrL then use non-neutral awb gain as final awb gain
    u32 neutralPointPrcThrH;     /// default 20,  [0:256] neutral point percent large than neutralPointPrcThrH then use neutral awb gain as final awb gain
    u32 defaultGainWeight;       /// default 128, [0:256] use this weight blend default gain and temporal predict gain as non-neutral awb gain
    u32 defaultGainR;            /// default 512, [256:2048] default R gain
    u32 defaultGainG;            /// default 256, [256:2048] default G gain
    u32 defaultGainB;            /// default 512, [256:2048] default B gain
    u32 enqueNeutralPointThr;    /// default 128, [0:256] H/A/TL84/D65/SHADE white point large than this threshold, then meet Condition 1
    u32 enqueCWFDFThr;           /// default 128, [0:256] CWF and DF white point less than this threshold, then meet Condition 2
} awb_non_neutral_predict_t;

typedef struct _face_awb_param_t
{
    u32      enable;              /// default 0,[0:1] enable face awb
    u32      enableNR;            /// default 0,[0:1] enable exclude face roi white point
    u32      weight;              /// default 40,[0:100] face awb max weight
    u32      refTargetTolerance;  /// default 8,[0:32] face reference target tolerance range
    u32      faceScale;           /// default 100,[10,1000] face roi scale ratio, 100 base
    u32      faceSizeRatioLow;    /// default 102,[0:1024] face size ratio threshold low, 1024 base
    u32      faceSizeRatioHi;     /// default 204,[0:1024] face size ratio threshold high, 1024 base
    u32      distThrLow;          /// default 30,[0:255] distance low threshold between face XY and face target
    u32      distThrHi;           /// default 150,[0:255] distance high threshold between face XY and face target
    u32      facePrefGainR;       /// default 256,[1:1024] Manual, preference R Gain
    u32      facePrefGainB;       /// default 256,[1:1024] Manual, preference B Gain
} face_awb_param_t;

typedef struct _luma_weight_lut
{
    u32 weightLut[16];
} luma_weight_lut_t;

typedef struct _awb_shrink_param_t
{
    u32      enable;        /// default 0, [0]disable, [1]enable
    u32      lightIdx;      /// default 0, [0: 6] reference light source index
    u32      ratio;         /// default 10, [0:128] shink ratio
    u32      enLeft;        /// default 0, enable/disable left shrink
    u32      enRight;       /// default 0, enable/disable left shrink
    u32      enTop;         /// default 0, enable/disable top shrink
    u32      enBot;         /// default 0, enable/disable bottom shrink
    u32      leftObj;       /// left shrink object(left adjacent color zone)
    u32      rightObj;      /// right shrink object(right adjacent color zone)
    u32      topObj;        /// top shrink object(top adjacent color zone)
    u32      botObj;        /// bottom shrink object(bottom adjacent color zone)
}awb_shrink_param_t;

/// AWB tuning parameters
typedef struct
{
    BOOL              awbEnable;          /// default 1, [0]disable, [1]enable
    u8                awbConvergeStep;    /// default 10,[1:20] Manual, AWB convergence speed, 92:
    u8                awbStableRange;     /// default 1,[1:100] Manual, AWB convergence stability range, 93:
    BOOL              awbLumaWeightEnable;/// default 1,[0:1][0]disable, [1]enable
    u16               awbPrefGainR;       /// default 256,[1:1024] Manual, shift R Gain, 99:
    u16               awbPrefGainB;       /// default 256,[1:1024] Manual, shift B Gain, 100:
    u16               awbLowlightLuxThr;  /// default 420,[1:1024] Manual, Lowlight lux index threshold, 101:
    u16               awbLowctRgPosThr;   /// default 3800,[1:8192] Manual, RGPos(RG ratio = R/G) lowCT threshold, 102:
    u16               awbDarkPixelThr;    /// default 4,[0:2048] aac_min_th
    u16               awbSatPixelThr;     /// default 4090,[2049:4095] aac_max_th
    u8                awbDarkPrcThr;      /// default 50, [0:255]dark pixel percent large than this threshold, drop this white point
    u8                awbSatPrcThr;       /// default 25, [0:255] saturation pixel percent large than this threshold, drop this white point
    awb_roi_t         awbRoi;             /// default left/top 1,[0:14] w/h 13,[1:15]Manual, ROI 0-14 block number

    DYNAMIC_TABLE(awb_light_source_area_param_t, awbLightSourceAreaScale); /// awb standard detect zone params

    DYNAMIC_TABLE(awb_detect_zone_t, awbDetectZone); ///awb detect zone

    DYNAMIC_TABLE(awb_mislead_color_zone_t, awbMisleadColorZone); /// awb mislead zone

    DYNAMIC_TABLE(awb_prefcolor_zone_t, awbPrefColorZone); /// awb perference color zone

    DYNAMIC_TABLE(awb_light_source_prob_t, awbLightsourceProb); /// awb light source prob

    DYNAMIC_TABLE(awb_statistics_shift_t, awbStatisticsShift); /// awb statistics shift for each light source

    DYNAMIC_TABLE(awb_extra_color_zone_t, awbExtraColorZone); /// awb extra color for mix extra color zone awb gain

    DYNAMIC_TABLE(awb_special_gain_t, awbSpecialGain); /// awb special gain for all light source area

    DYNAMIC_TABLE(awb_special_gain_weight_t, awbSpecialGainWeight); /// blend special gain weight

    DYNAMIC_TABLE(luma_weight_lut_t, awbLumaWeightLut); ///luma lut table

    DYNAMIC_TABLE(awb_non_neutral_predict_t, awbNonNeutralPredict); /// blend non neutral predict gain

    DYNAMIC_TABLE(awb_shrink_param_t, awbShrinkParam); /// awb shrink param for each light source

    face_awb_param_t faceAwbParam;
} tune_awb_param_t;

////////////////////////  AF  ///////////////////////////
/// AF table index
typedef enum
{
    AF_NEAR_LIMIT_IDX = 0,
    AF_7CM_IDX        = 1,
    AF_10CM_IDX       = 2,
    AF_14CM_IDX       = 3,
    AF_20CM_IDX       = 4,
    AF_30CM_IDX       = 5,
    AF_40CM_IDX       = 6,
    AF_50CM_IDX       = 7,
    AF_60CM_IDX       = 8,
    AF_120CM_IDX      = 9,
    AF_HYP_F_IDX      = 10,
    AF_INF_LIMIT_IDX  = 11,
    AF_MAX_IDX        = 12,
} af_table_index_t;

/// AF Converge parameters, trigger by afZoomVal
typedef struct
{
    u32    afNearEndPos;
    u32    afFarEndPos;
    u32    afNearMarginPercent;
    u32    afFarMarginPercent;
    u32    afMoveStep;
} af_zoom_focus_mapping_t;

/// AF ROI information
typedef struct
{
    BOOL    afManualRoiEnable;     /// default 0,[0:1] Manual, Enable manual set AF process ROI
    roi_t   roi;                   /// default left/top 1,[0:14] w/h 13,[1:15]Manual, ROI 0-14 block number
} af_roi_t;

/// af monitor and contrast search params
typedef struct
{
    u32     afLumaChangeThr;         /// default 15,[0:50] Manual, If current average luma is larger than Thr*Ref_Value,  change cnt + 1, 118:
    u32     afLumaChangeCntThr;      /// default 6,[0:50] Manual, change cnt > ChangeCntThr, judge as scene change, 119:
    u32     afLumaStableThr;         /// default 3,[0:50] Manual, If current (average luma - previous frame average luma) is smaller then StableThr, stable cnt +1, 120:
    u32     afLumaStableCntThr;      /// default 6,[0:50] Manual, stable cnt > StableCntThr, judge as scene stable, 121:
    u32     afExposureChgThr;        /// default 5,[0:100] Manual, exposure thange > Thr then judge as change and trigger AF, 122:
    u32     afFvChangeEnable;        /// default 1,[0:1] Manual, 1 - scene change reference FV change, 123:
    u32     afFvChangeThr;           /// default 20,[0:50] Manual, curr FV > Thr*Ref_Value then change cnt + 1, 124:
    u32     afFvChangeCntThr;        /// default 2,[0:50] Manual, change cnt > ChangeCntThr, judge as scene change, 125:
    u32     afFvStableThr;           /// default 10,[0:50] Manual, curr fv - previous frame fv < StableThr then stable cnt +1, 126:
    u32     afFvStableCntThr;        /// default 5,[0:50] Manual, stable cnt > StableCntThr, judge as scene stable, 127:
    u32     afFvIncRatio;            /// default 1060,[1000:1500] Manual, fv increase ratio > xxx/1000 judge as fv inc
    u32     afFvDecRatio;            /// default 980,[500:1000] Manual, fv decrease ratio < xxx/1000 judge as fv dec
    u32     afIncDecPeakRatio;       /// default 86,[50:100] Manual, FVIncDecPeakRatio for find the peak, 100 base, 128:
    u32     afDecDecPeakRatio;       /// default 86,[50:100] Manual, FVDecDecPeakRatio for find the peak, 100 base, 129:
    u32     roiScale;                /// default 100,[10:1000]Manual, lowlight scene could set large ROI to do contrast af, 100 base
} af_param_by_lux_t;

/// Face roi scale params
typedef struct
{
    u32     roiScale;               /// default 100,[10:1000] whole roi scale ratio
    u32     topRatio;               /// default 100,[10:1000] roi left scale ratio
    u32     botRatio;               /// default 100,[10:1000] roi bottom scale ratio
    u32     leftRatio;              /// default 100,[10:1000] roi left scale ratio
    u32     rightRatio;             /// default 100,[10:1000] roi right scale ratio
    u32     faceInCntThr;           /// default 4, [1:100](face in count greater than this value, judge as had face enter)
    u32     faceOutCntThr;          /// default 8, [1:100](face out count greater than this value, judge as face disappear)
    u32     faceSmallSizeThr;       /// default 10, [1:1000] (face size (face_width * face_height / img_width * img_height)
                                    ///  less than this value, judge as small face.permillage 10:1-1000)
} face_af_roi_param_t;

/// AF tuning paramters structure
typedef struct
{
    BOOL                      afEnable;                /// default 1, [0]disable, [1]enable
    BOOL                      afSourceSelYuv;          /// default 0,[0:1] Manual, [0]raw stats  [1]yuv stats, 115:
    u8                        afBackjumpCompStep;      /// default 4,[0:7] Manual, BackJumpCompStep when  direct change, 117:
    u8                        afMoveSkipFrame;         /// default 1,[0:3]Manual set skip frame num after motor move finish
    af_roi_t                  afRoi;                   /// default 5,[0:14] Manual, ROI start x, 0-14 block number, 135:
    u8                        afRoiBlockNumX;          /// default 5,[1:9] Manual, ROI sub block number x
    u8                        afRoiBlockNumY;          /// default 5,[1:9] Manual, ROI sub block number y
    u8                        afNearRange;             /// default 20,[0:100] Manual, pos < search range * xxx% set as near range
    u8                        afFarRange;              /// default 70,[0:100] Manual, pos > search range * xxx% set as far range
    u16                       afBloomingThr;           /// default 3800,[0:4095] Manual, pixel value > Thr, judge as blooming pixel
    u32                       afBloomingCntThr;        /// default 80,[0:100] Manual, plooming pixel count thr, > thr judge as poing light af scene
    u32                       afBloomingGainThr;       /// default 1023,[0:maxGain] Manual, ae total gain > thr then start judge poinng light af scene
    u32                       afLowlightGainThr;       /// default 65536,[0:542720] Manual, gain < Thr, judge as Low light scene, 132:
    u8                        afMacroSearchEndThr;     /// default 50,[0:100] Manual, set search end pos = search max - search Range * 50%, 133:
    u32                       afWeight[25];
    u32                       afGammaLut[33];
//    u32                       afTable[AF_MAX_IDX];
    u32                       afMotorZoomInit;
    DYNAMIC_TABLE(af_zoom_focus_mapping_t, afZoomFocusMappingTab);

    DYNAMIC_TABLE(af_param_by_lux_t, afParamByLux);

    DYNAMIC_TABLE(face_af_roi_param_t, faceAfRoiParam);
}tune_af_param_t;

/// SW 3A tuning parameters
typedef struct
{
    tune_version_t   sw3aAlgoVersion;
    tune_version_t   sw3aTuneVersion;
    tune_alg_attr_t  sw3aAlgoAttr;
    tune_aec_param_t ae;
    tune_awb_param_t awb;
    tune_af_param_t  af;
} tune_sw3a_params_t;

#endif

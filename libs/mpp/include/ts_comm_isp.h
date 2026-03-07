/*
 * Copyright © 2019 TSING MICRO.com.cn All Rights Reserved
 * Description: ts_comm_isp.h
 * Author:TsingMicro multimedia software group
 * Create: 2022/06/02
 */

#ifndef __TS_COMM_ISP_H__
#define __TS_COMM_ISP_H__

#include "ts_type.h"
#include "ts_errno.h"
#include "ts_common.h"
#include "ts_comm_video.h"

// i5

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

/****************************************************************************
 * MACRO DEFINITION                                                         *
 ****************************************************************************/
#define AWB_ZONE_ORIG_ROW               (32)
#define AWB_ZONE_ORIG_COLUMN            (32)
#define AE_ZONE_ROW                     (15)
#define AE_ZONE_COLUMN                  (17)

#define AWB_ZONE_BIN                              (1)

#define ISP_MAX_STITCH_NUM (1)

#define AWB_ZONE_NUM                    (AWB_ZONE_ORIG_ROW * AWB_ZONE_ORIG_COLUMN * AWB_ZONE_BIN)
#define AWB_ZONE_STITCH_MAX             (AWB_ZONE_NUM * ISP_MAX_STITCH_NUM)

#define GAMMA_NODE_NUM                  (1025)
#define PREGAMMA_NODE_NUM               (257)
#define PREGAMMA_SEG_NUM                (8)
#define ISP_BAYER_CHN_NUM               (4)

/****************************************************************************
 * GENERAL STRUCTURES                                                       *
 ****************************************************************************/
/*
ISP Error Code
0x40 = ISP_NOT_INIT
0x41 = ISP_MEM_NOT_INIT
0x42 = ISP_ATTR_NOT_CFG
0x43 = ISP_SNS_UNREGISTER
0x44 = ISP_INVALID_ADDR
0x45 = ISP_NOMEM
0x46 = ISP_NO_INT
*/
typedef enum tsISP_ERR_CODE_E
{
    ERR_ISP_NOT_INIT                = 0x40,// ISP not init
    ERR_ISP_MEM_NOT_INIT            = 0x41,//ISP memory not init
    ERR_ISP_ATTR_NOT_CFG            = 0x42,//ISP attribute not cfg
    ERR_ISP_SNS_UNREGISTER          = 0x43,//ISP sensor unregister
    ERR_ISP_INVALID_ADDR            = 0x44,//ISP invalid address
    ERR_ISP_NOMEM                   = 0x45,//ISP nomem
    ERR_ISP_NO_INT                  = 0x46,//ISP
} ISP_ERR_CODE_E;

#define TS_ERR_ISP_NULL_PTR         TS_DEF_ERR(TS_ID_ISP, EN_ERR_LEVEL_ERROR, EN_ERR_NULL_PTR)
#define TS_ERR_ISP_ILLEGAL_PARAM    TS_DEF_ERR(TS_ID_ISP, EN_ERR_LEVEL_ERROR, EN_ERR_ILLEGAL_PARAM)
#define TS_ERR_ISP_NOT_SUPPORT      TS_DEF_ERR(TS_ID_ISP, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_SUPPORT)

#define TS_ERR_ISP_NOT_INIT         TS_DEF_ERR(TS_ID_ISP, EN_ERR_LEVEL_ERROR, ERR_ISP_NOT_INIT)
#define TS_ERR_ISP_MEM_NOT_INIT     TS_DEF_ERR(TS_ID_ISP, EN_ERR_LEVEL_ERROR, ERR_ISP_MEM_NOT_INIT)
#define TS_ERR_ISP_ATTR_NOT_CFG     TS_DEF_ERR(TS_ID_ISP, EN_ERR_LEVEL_ERROR, ERR_ISP_ATTR_NOT_CFG)
#define TS_ERR_ISP_SNS_UNREGISTER   TS_DEF_ERR(TS_ID_ISP, EN_ERR_LEVEL_ERROR, ERR_ISP_SNS_UNREGISTER)
#define TS_ERR_ISP_INVALID_ADDR     TS_DEF_ERR(TS_ID_ISP, EN_ERR_LEVEL_ERROR, ERR_ISP_INVALID_ADDR)
#define TS_ERR_ISP_NOMEM            TS_DEF_ERR(TS_ID_ISP, EN_ERR_LEVEL_ERROR, ERR_ISP_NOMEM)
#define TS_ERR_ISP_NO_INT           TS_DEF_ERR(TS_ID_ISP, EN_ERR_LEVEL_ERROR, ERR_ISP_NO_INT)

/*
Defines the working mode of ISP
0 = automatic mode
1 = manual mode
*/
typedef enum tsISP_OP_TYPE_E {
    OP_TYPE_AUTO    = 0,
    OP_TYPE_MANUAL  = 1,
    OP_TYPE_BUTT
} ISP_OP_TYPE_E;

/*Defines the format of the input Bayer image*/
typedef enum tsISP_BAYER_FORMAT_E
{
    BAYER_RGGB    = 0,
    BAYER_GRBG    = 1,
    BAYER_GBRG    = 2,
    BAYER_BGGR    = 3,
    BAYER_BUTT
} ISP_BAYER_FORMAT_E;

/*Defines the bitwidth of the input Bayer image, used for lsc online calibration*/
typedef enum tsISP_BAYER_RAWBIT_E
{
    BAYER_RAWBIT_8BIT    = 8,
    BAYER_RAWBIT_10BIT   = 10,
    BAYER_RAWBIT_12BIT   = 12,
    BAYER_RAWBIT_14BIT   = 14,
    BAYER_RAWBIT_16BIT   = 16,
    BAYER_RAWBIT_BUTT

} ISP_BAYER_RAWBIT_E;

typedef struct{
	TS_BOOL	bOnline;
	TS_U32	u32VinoutHtotalMargin;
	TS_U32  u32VinoutVtotalMargin;
}ISP_ONLINE_PARAM_S;

/* ISP public attribute, contains the public image attribute */
typedef struct tsISP_PUB_ATTR_S
{
    RECT_S          stWndRect;      /* RW; Start position of the cropping window, image width, and image height */
    SIZE_S          stSnsSize;      /* RW; Width and height of the image output from the sensor*/
    TS_FLOAT        f32FrameRate;   /* RW; Range: [0, 0xFFFF]; For frame rate */
    ISP_BAYER_FORMAT_E  enBayer;    /* RW; Range:[0,3] ;Format:2.0;the format of the input Bayer image*/
    WDR_MODE_E      enWDRMode;      /* RW; WDR mode select*/
	//TS_BOOL			bOnline;
	ISP_ONLINE_PARAM_S	stOnlineParam;
    TS_U8           u8SnsMode;      /* RW; Range: [0, 0xFF]; Format: 8.0; For special sensor mode switches */
	TS_BOOL			bDynFpsSync;
} ISP_PUB_ATTR_S;

/*Defines the WDR mode of the ISP*/
typedef struct tsISP_WDR_MODE_S
{
    WDR_MODE_E  enWDRMode;
} ISP_WDR_MODE_S;

#if 0
typedef struct tsISP_GAMMA_ATTR_S
{
    TS_BOOL   bEnable;                     /*RW; Range:[0, 1]; Format:1.0;Enable/Disable Gamma Function*/
    TS_U16    u16Table[GAMMA_NODE_NUM];    /*RW; Range:[0, 4095]; Format:12.0;Gamma LUT nodes value*/

    ISP_GAMMA_CURVE_TYPE_E enCurveType;    /*RW; Range:[0, 3]; Format:2.0;Gamma curve type*/
} ISP_GAMMA_ATTR_S;
#else

typedef enum tsISP_GAMMA_BASE_SELECTION_E {
	ISP_GAMMA_BASIC_SELECTION_LINEAR_OUTDOOR = 0,
	ISP_GAMMA_BASIC_SELECTION_LINEAR_LOWLIGHT,
	ISP_GAMMA_BASIC_SELECTION_DOL_OUTDOOR,
	ISP_GAMMA_BASIC_SELECTION_DOL_LOWLIGHT,
	ISP_GAMMA_BASIC_SELECTION_SRGB,
	ISP_GAMMA_BASIC_SELECTION_REC709,
	ISP_GAMMA_BASIC_SELECTION_GMA1_8,
	ISP_GAMMA_BASIC_SELECTION_GMA2_0,
	ISP_GAMMA_BASIC_SELECTION_GMA2_2,
	ISP_GAMMA_BASIC_SELECTION_GMA2_4,
	ISP_GAMMA_BASIC_SELECTION_GMA2_6,
	ISP_GAMMA_BASIC_SELECTION_CUSTOM0,
	ISP_GAMMA_BASIC_SELECTION_CUSTOM1,
	ISP_GAMMA_BASIC_SELECTION_CUSTOM2,
	ISP_GAMMA_BASIC_SELECTION_CUSTOM3,
	ISP_GAMMA_BASIC_SELECTION_CUSTOM4,
	ISP_GAMMA_BASIC_SELECTION_CUSTOM5,
	ISP_GAMMA_BASIC_SELECTION_BUTT
}ISP_GAMMA_BASE_SELECTION_E;

typedef enum tsISP_GAMMA_CONTRAST_ENHANCE_ENABLE_E{
	ISP_GAMMA_CONTRAST_ENHANCE_ENABLE_BASIC = 0,
	ISP_GAMMA_CONTRAST_ENHANCE_ENABLE_GCC,
	ISP_GAMMA_CONTRAST_ENHANCE_ENABLE_GCE,
	ISP_GAMMA_CONTRAST_ENHANCE_ENABLE_BUTT
} ISP_GAMMA_CONTRAST_ENHANCE_ENABLE_E;

typedef struct tsISP_GAMMA_ATTR_S {
	TS_BOOL b_manual_mode_ggc;
	TS_BOOL b_ggc_enable;
	TS_U16 ggc_lowiso_thr;
	TS_U16 ggc_highiso_thr;
	TS_U16 ggc_converge_speed;
	ISP_GAMMA_BASE_SELECTION_E e_ggc_basic_selection;
	ISP_GAMMA_CONTRAST_ENHANCE_ENABLE_E e_ggc_contrast_enhance_enable;
	TS_BOOL b_ggc_contrast_auto_mode;
	TS_U16 ggc_contrast_auto_str;
	TS_U16 ggc_contrast_manual_str;
	TS_U16 ggc_dehaze_strength_light;
	TS_U16 ggc_dehaze_strength_dark;
	TS_U16 ggc_global_lumination_adj;
	TS_U8 ggc_local_lum_adj_node_0;
	TS_U8 ggc_local_lum_adj_node_1;
	TS_U8 ggc_local_lum_adj_node_2;
	TS_U8 ggc_local_lum_adj_node_3;
	TS_U8 ggc_local_lum_adj_node_4;
	TS_U8 ggc_local_lum_adj_node_5;
	TS_U8 ggc_lcoal_lum_adj_node_6;
	TS_U8 ggc_local_adaptive_node_0;
	TS_U8 ggc_local_adaptive_node_1;
	TS_U8 ggc_local_adaptive_node_2;
	TS_U8 ggc_local_adaptive_node_3;
	TS_U8 ggc_local_adaptive_node_4;
	TS_U8 ggc_local_adaptive_node_5;
	TS_U8 ggc_local_adaptive_node_6;
	TS_U16 ggc_contrast_smooth_rate;
} ISP_GAMMA_ATTR_S;
#endif

typedef struct tsISP_DEHAZE_ATTR_S {
	TS_U32 u32DeHazeStrength; //[0-8192]
} ISP_DEHAZE_ATTR_S;

typedef enum tsISP_LTM_LOG_LEVEL_E {
	ISP_LTM_LOG_LEVEL_PFF = 0,
	ISP_LTM_LOG_LEVEL_FILE,
	ISP_LTM_LOG_LEVEL_UART,
	ISP_LTM_LOG_LEVEL_BUTT
} ISP_LTM_LOG_LEVEL_E;

typedef struct tsISP_LTM_ATTR_S {
	TS_BOOL bManualModeLtm;
	ISP_LTM_LOG_LEVEL_E eLtmLogLevel;
	TS_U16 u16LtmConvergeSpeed;
	TS_U16 u16LtmSpatialFilterTime;
	TS_U16 u16LtmMinLighten;
	TS_U16 u16LtmMaxLighten;
	TS_U16 u16LtmGain;
	TS_U16 u16LtmDarkPrc;
	TS_U16 u16LtmBrightPrc;
	TS_U16 u16LtmContrastMax;
	TS_U16 u16LtmContrastMin;
	TS_BOOL bLtmGdgEnable;
	TS_U16 u16LtmTargetLdr;
	TS_U8 u8LtmHiTargetPrc;
	TS_U16 u16LtmDarkLevel;
	TS_U8 u8LtmDarkMinPrc;
	TS_U8 u8LtmDarkMaxParc;
	TS_U16 u16LtmDarkGainRatio;
	TS_U16 u16LtmAmplifyalpha;
	TS_U16 u16LtmAmplifyalphaMax;
} ISP_LTM_ATTR_S;

typedef enum tsISP_CMB_OUT_MODE_E {
	ISP_CMB_OUT_MODE_NORMAL_OUTPUT = 0,
	ISP_CMB_OUT_MODE_LONG_EXPO_FRAME,
	ISP_CMB_OUT_MODE_MEDIUM_EXPO_FRAME,
	ISP_CMB_OUT_MDOE_SHORT_EXPO_FRAME,
	ISP_CMB_OUT_MDOE_BUTT
} ISP_CMB_OUT_MODE_E;

typedef struct tsISP_CMB_ATTR_S {
	TS_BOOL bManualModeCmb;
	ISP_CMB_OUT_MODE_E eCmbOutMode;
	TS_U16 u16CmbNpMultLm;
	TS_U16 u16CmbNpMultMs;
	TS_U16 u16CmbMovLmMult;
	TS_U16 u16CmbMovMsMult;
	TS_U16 u16CmbThresh1Ms;
	TS_U16 u16CmbThresh2Ms;
	TS_U16 u16CmbThresh1Lm;
	TS_U16 u16CmbThresh2Lm;
	TS_U8 u8CmbOutputAlphaMs;
	TS_U8 u8CmbOutputAlphaLm;
	TS_U8 u8CmbOutputMs;
	TS_U8 u8CmbOutputLm;
	TS_U16 u16CmbBlLongR;
	TS_U16 u16CmbBlLongGr;
	TS_U16 u16CmbBlLongGb;
	TS_U16 u16CmbBlLongB;
	TS_U16 u16CmbBlMediumR;
	TS_U16 u16CmbBlMediumGr;
	TS_U16 u16CmbBlMediumGb;
	TS_U16 u16CmbBlMediumB;
	TS_U16 u16CmbBlShortR;
	TS_U16 u16CmbBlShortGr;
	TS_U16 u16CmbBlShortGb;
	TS_U16 u16CmbBlShortB;
} ISP_CMB_ATTR_S;


/* statistics structs */
#define HIST_NUM        (1024)
#define BAYER_PATTERN_NUM   (4)
#define WDR_CHN_MAX         (4)
#define ISP_CHN_MAX_NUM     (4)

typedef struct tsISP_AE_STATISTICS_S
{
	TS_U32 ae0Hist[HIST_NUM];
    TS_U32 au32FEHist1024Value[ISP_CHN_MAX_NUM][HIST_NUM];                                 /*R; Range: [0x0, 0xFFFFFFFF]; Format:32.0; Global 1024 bins histogram of FE*/
    TS_U16 au16FEGlobalAvg[ISP_CHN_MAX_NUM][BAYER_PATTERN_NUM];                            /*R; Range: [0x0, 0xFFFF]; Format:16.0; Global average value of FE*/
    TS_U16 au16FEZoneAvg[ISP_CHN_MAX_NUM][AE_ZONE_ROW][AE_ZONE_COLUMN][BAYER_PATTERN_NUM]; /*R; Range: [0x0, 0xFFFF]; Format:16.0; Zone average value of FE.*/
    TS_U32 au32BEHist1024Value[HIST_NUM];                                                  /*R; Range: [0x0, 0xFFFFFFFF]; Format:32.0; Global 1024 bins histogram of BE*/
    TS_U16 au16BEGlobalAvg[BAYER_PATTERN_NUM];                                             /*R; Range: [0x0, 0xFFFF]; Format:16.0; Global average value of BE*/
    TS_U16 au16BEZoneAvg[AE_ZONE_ROW][AE_ZONE_COLUMN][BAYER_PATTERN_NUM];                  /*R; Range: [0x0, 0xFFFF]; Format:16.0; Zone average value of BE*/
} ISP_AE_STATISTICS_S;


/************ AWB*************/
#define ISP_AAC_BLOCK_NUMBER				 (ISP_AAC_BLOCK_ROW * ISP_AAC_BLOCK_COLUMN)
typedef struct tsISP_WB_STATISTICS_S
{
    TS_U16 u16GlobalR;          /*R; Range: [0x0, 0xFFFF];Global WB output Average R */
    TS_U16 u16GlobalG;          /*R; Range: [0x0, 0xFFFF];Global WB output Average G */
    TS_U16 u16GlobalB;          /*R; Range: [0x0, 0xFFFF];Global WB output Average B */
    TS_U16 u16CountAll;         /*R; Range: [0x0, 0xFFFF];normalized number of Gray points */

	TS_U16 usAWBZonePixelsCounted[ISP_AAC_BLOCK_NUMBER];
	TS_U64 ullAWBZoneRGRatioSum[ISP_AAC_BLOCK_NUMBER];
	TS_U64 ullAWBZoneBGRatioSum[ISP_AAC_BLOCK_NUMBER];

    TS_U16 au16ZoneAvgR[AWB_ZONE_NUM];            /*R; Range: [0x0, 0xFFFF];Zone Average R */
    TS_U16 au16ZoneAvgG[AWB_ZONE_NUM];            /*R; Range: [0x0, 0xFFFF];Zone Average G */
    TS_U16 au16ZoneAvgB[AWB_ZONE_NUM];            /*R; Range: [0x0, 0xFFFF];Zone Average B */
    TS_U16 au16ZoneCountAll[AWB_ZONE_NUM];        /*R; Range: [0x0, 0xFFFF];normalized number of Gray points */
} ISP_WB_STATISTICS_S;

#define ISP_AF_ZONE_ROW			 9
#define ISP_AF_ZONE_COLUMN		 9

typedef struct tsISP_AF_STATISTICS_S
{
	ts_u64 stAfStat[ISP_AF_ZONE_ROW][ISP_AF_ZONE_COLUMN];
} ISP_AF_STATISTICS_S;


typedef enum tsISP_VD_TYPE_E
{
    ISP_VD_FE_START   = 0,
    ISP_VD_FE_END,
    ISP_VD_BE_END,
	ISP_VD_FIRST_FRAME,

    ISP_VD_BUTT
} ISP_VD_TYPE_E;

typedef enum tsISP_MODULE_OSD_MODE_E {
	ISP_MODULE_OSD_MODE_DISABLE = 0,
	ISP_MODULE_OSD_MODE_HISTOGRAM,
	ISP_MODULE_OSD_MESH_GRID,
	ISP_MODULE_OSD_MODE_IMAHE_PSNR,
	ISP_MODULE_OSD_MODE_BUTT
} ISP_MODULE_OSD_MODE_E;

typedef enum tsISP_TPG_MODE_E {
	ISP_MODULE_TPG_MODE_NOT = 0,
	ISP_MODULE_TPG_COLOR_BAR,
	ISP_MODULE_TPG_COLOR_BAR_MONO,
	ISP_MODULE_TPG_MONO_PATCH,
	ISP_MODULE_TPG_SIMPLE_COLOR,
	ISP_MODULE_TPG_SHIFTING_COLOR_BAR,
	ISP_MODULE_TPG_SHIFTING_COLOR_BAR_MONO,
	ISP_MODULE_TPG_BUTT
} ISP_TPG_MODE_E;

typedef enum tsISP_DEBUG_OUT_MODE_E{
	ISP_DEBUG_OUT_MODE_INPUT_CHN_0 = 0,
	ISP_DEBUG_OUT_MODE_VIN0,
	ISP_DEBUG_OUT_MODE_TPG0,
	ISP_DEBUG_OUT_MODE_LIN0,
	ISP_DEBUG_OUT_MODE_DPC,
	ISP_DEBUG_OUT_MODE_RNR,
	ISP_DEBUG_OUT_MODE_CAC,/**Not support in dt56**/
	ISP_DEBUG_OUT_MODE_BLC,
	ISP_DEBUG_OUT_MODE_DGN,
	ISP_DEBUG_OUT_MODE_LSC,
	ISP_DEBUG_OUT_MODE_WBG,
	ISP_DEBUG_OUT_MODE_LTM,
	ISP_DEBUG_OUT_MODE_DMS,
	ISP_DEBUG_OUT_MODE_CCM,
	ISP_DEBUG_OUT_MODE_DPF,
	ISP_DEBUG_OUT_MODE_GGC,
	ISP_DEBUG_OUT_MODE_DTH,
	ISP_DEBUG_OUT_MODE_CSC1,
	ISP_DEBUG_OUT_MODE_CSC2A,
	ISP_DEBUG_OUT_MODE_MVD,
	ISP_DEBUG_OUT_MODE_CNR_LSH,
	ISP_DEBUG_OUT_MODE_INPUT_CHN_1,
	ISP_DEBUG_OUT_MODE_VIN1,
	ISP_DEBUG_OUT_MODE_INPUT_CHN_2,
	ISP_DEBUG_OUT_MODE_VIN2,
	ISP_DEBUG_OUT_MODE_TNR,
	ISP_DEBUG_OUT_MODE_BUTT
} ISP_DEBUG_OUT_MODE_E;

typedef struct tsISP_MODULE_BYPASS_S {
	ts_u32 bypass_dpc : 1;
	ts_u32 bypass_rnr : 1;
	ts_u32 bypass_dms : 1;
	ts_u32 bypass_lsh : 1;
	ts_u32 bypass_cnr : 1;
	ts_u32 bypass_mvd : 1;
	ts_u32 bypass_tnr : 1;
	ts_u32 bypass_cac : 1;
	ts_u32 bypass_ltm : 1;
	ts_u32 bypass_cmb : 1;
	ts_u32 bypass_dpf : 1;
	ts_u32 rsv_21 : 21;
} ISP_MODULE_BYPASS_S;

typedef struct tsISP_GENERAL_TOP_S
{
	ISP_MODULE_OSD_MODE_E e_osd_mode;
	ISP_TPG_MODE_E e_tpg_mode;
	TS_U8 tpg_shift_speed;
	TS_BOOL b_manual_mode_all_module;
	ISP_MODULE_BYPASS_S st_bypass;
	ISP_DEBUG_OUT_MODE_E e_debug_out_mode;
	TS_U8 skip_frame;
	TS_U8 out_index;
	TS_U8 disp_type;
} ISP_GENERAL_TOP_S;

typedef enum tsISP_LSH_DETAIL_TYPE_E
{
	ISP_LSH_DETAIL_TYPE_SMOOTH = 0,
	ISP_LSH_DETAIL_TYPE_DETAIL,
	ISP_LSH_DETAIL_TYPE_BUTT
}ISP_LSH_DETAIL_TYPE_E;

typedef struct tsISP_LSH_S
{
	TS_BOOL	bManualModeLsh;
	TS_U16 	u16LSHMotionR;
	TS_U16 	u16LSHLumOffset;
	TS_U8		u8LSHLumMid;
	TS_U8		u8LSHLumEnd;
	TS_U16 	u16LSHLumY1;
	TS_U16 	u16LSHLumY2;
	TS_U16 	u16LSHLumY3;
	TS_U16 	u16LSHEdgeNlmSlp;
	TS_U16 	u16LSHEdgeN2WNormR;
	TS_U16 	u16LSHEdgeThinOffset;
	TS_U8		u8LSHEdgeThinMid;
	TS_U8		u8LSHEdgeThinEnd;
	TS_U16 	u16LSHEdgeThinY1;
	TS_U16 	u16LSHEdgeThinY2;
	TS_U16 	u16LSHEdgeThinY3;
	TS_U16 	u16LSHNrFreqOffset;
	TS_U8		u8LSHNrFreqMid;
	TS_U8		u8LSHNrFreqEnd;
	TS_U16 	u16LSHNrFreqY1;
	TS_U16 	u16LSHNrFreqY2;
	TS_U16 	u16LSHNrFreqY3;
	TS_U16 	u16LSHNegStr1;
	TS_U16 	u16LSHPosStr1;
	TS_U16 	u16LSHNegMinOffset1;
	TS_U16 	u16LSHPosMaxOffset1;
	TS_U16 	u16LSHNegClipRatio1;
	TS_U16 	u16LSHPosClipRatio1;
	TS_U16 	u16LSHDetailNwAlpha;
	TS_U16 	u16LSHDetailNrR;
	ISP_LSH_DETAIL_TYPE_E	enLSHDetailType;
	TS_U16 	u16LSHDetailFreqOffset;
	TS_U8		u8LSHDetailFreqMid;
	TS_U8		u8LSHDetailFreqEnd;
	TS_U16 	u16LSHDetailFreqY1;
	TS_U16 	u16LSHDetailFreqY2;
	TS_U16 	u16LSHDetailFreqY3;
	TS_U16 	u16LSHDetailLumOffset;
	TS_U8		u8LSHDetailLumMid;
	TS_U8		u8LSHDetailLumEnd;
	TS_U16 	u16LSHDetailLumY1;
	TS_U16 	u16LSHDetailLumY2;
	TS_U16 	u16LSHDetailLumY3;
	TS_U16 	u16LSHNegStr2;
	TS_U16 	u16LSHPOSStr2;
	TS_U16 	u16LSHNegMinOffset2;
	TS_U16 	u16LSHPosMaxOffset2;
	TS_U16 	u16LSHNegClipRatio2;
	TS_U16 	u16LSHPosClipRatio2;
	TS_U8		u8LSHEdgeNlmWeightTable;
}ISP_LSH_S;

typedef struct tsISP_LSH_DLTI_S
{
	TS_BOOL	bLSHDLTIEn; /* [0, 1], LSH DLTI switch */
	TS_U16 	u16LSHDLTIPosOffset;
	TS_U8		u8LSHDLTIPosMid;
	TS_U8		u8LSHDLTIPosEnd;
	TS_U16 	u16LSHDLTIPosY1;
	TS_U16 	u16LSHDLTIPosY2;
	TS_U16 	u16LSHDLTIPosY3;
	TS_U16 	u16LSHDLTINegOffset;
	TS_U8		u8LSHDLTINegMid;
	TS_U8		u8LSHDLTINegEnd;
	TS_U16 	u16LSHDLTINegY1;
	TS_U16 	u16LSHDLTINegY2;
	TS_U16 	u16LSHDLTINegY3;
}ISP_LSH_DLTI_S;

typedef struct tsISP_SDE_S
{
	TS_BOOL	bManulaModeSDE;
	TS_BOOL	bSDEEnable;
	TS_BOOL	bSDELightEn;
	TS_BOOL	bSDEGaily;
	TS_BOOL	bSDEMono;
	TS_BOOL	bSDEContrastHigh;
	TS_BOOL	bAWBSDECtCool;
	TS_BOOL	bAWBSDECtWarm;
	TS_BOOL	bAWBSDECtSharpen;
	TS_U16 	u16SDEBrightness;
	TS_U16 	u16SDESaturation;
	TS_U8		u8SDEHue;
	TS_BOOL	bSDENegEn;
	TS_U16 	u16SDEClipYLow;
	TS_U16 	u16SDEClipYHigh;
	TS_U16 	u16SDEContrastY;
	TS_U16 	u16SDEContrastU;
	TS_U16 	u16SDEContrastV;
	TS_U16 	u16SDEContrastAll;
}ISP_SDE_S;

typedef enum tsISP_TNR_MOTION_MODE_E{
	ISP_TNR_MOTION_MODE_BYPASS_ALL = 0,
	ISP_TNR_MOTION_MODE_RIGHT_3DNR,
	ISP_TNR_MOTION_MODE_LEFT_3DNR,
	ISP_TNR_MOTION_MODE_BUTT
} ISP_TNR_MOTION_MODE_E;

typedef struct tsISP_TNR_S {
	TS_BOOL bManual;
	ISP_TNR_MOTION_MODE_E enTNRMotionMode;
	TS_U8 u32TNRNr2dPyramidWt;
	
	TS_U16 u16TNRGlobalNoiseThr;
	TS_U16 u16TNRNr2dGradSlope;
	
	TS_U16 u16TNRNr2dYnoiseThr;
	TS_U16 u16TNRNr2dCnoiseThr;
	
	TS_U8 u8TNRNr2dNoiseBackRatio;
	TS_U8 u8TNRMotionThr;
	TS_U16 u16TNRMotionCurveOffset;
	
	TS_U8 u8TNRMotionCurveMid;
	TS_U8 u8TNRMotionCurveEnd;
	TS_U8 u8TNRMotionCurveY1;
	TS_U8 u8TNRMotionCurveY2;
	
	TS_U8 u8TNRMotionCurveY3;
	TS_U8 u8TNRNr2dCurveOffset;
	TS_U8 u8TNRNr2dCurveMid;
	TS_U8 u8TNRNr2dCurveEnd;
	
	TS_U8 u8TNRNr2dCurveY1;
	TS_U8 u8TNRNr2dCurveY2;
	TS_U8 u8TNRNr2dCurveY3;
	TS_U8 u8TNRNr2dCurveC1;
	
	TS_U8 u8TNRNr2dCurveC2;
	TS_U8 u8TNRNr2dCurveC3;
	TS_U16 u16TNRNr3dCurveOffset;
	
	TS_U8 u8TNRNr3dCurveMid;
	TS_U8 u8TNRNr3dCurveEnd;
	TS_U8 u8TNRNr3dCurveY1;
	TS_U8 u8TNRNr3dCurveY2;
	
	TS_U8 u8TNRNr3dCurveY3;
	TS_U8 u8TNRNr3dCurveC1;
	TS_U8 u8TNRNr3dCurveC2;
	TS_U8 u8TNRNr3dCurveC3;
} ISP_TNR_S;

typedef struct tsISP_MVD_S {
	ts_bool bManualModeMvd;
	ts_bool bMvdDebug;
	ts_u16 u16MvdNpGain;
	ts_u16 u16MvdNpOffset;
	ts_u8 u8MvdPipeaThr;
	ts_u8 u8MvdPipebErode1;
	ts_u8 u8MvdPipebDilate;
	ts_u8 u8MvdPipebErode2;
	ts_u8 u8MvdPipebMapRefineThr;
	ts_u16 u16MvdPipeabTriDirStr;
	ts_u16 u16MvdBlackLevelR;
	ts_u16 u16MvdBlackLevelGr;
	ts_u16 u16MvdBlackLevelGb;
	ts_u16 u16MvdBlackLevelB;
	ts_u32 u16MvdDigitalGain;
} ISP_MVD_S;

typedef enum tsISP_DMS_DEBUG_MODE_E {
	ISP_DMS_DEBUG_MODE_FINAL_OUTPUT = 0,
	ISP_DMS_DEBUG_MODE_DIRECTIONAL_OUTPUT = 2,
	ISP_DMS_DEBUG_MODE_NON_DIRECTIONAL_OUTPUR = 3,
	ISP_DMS_DEBUG_MODE_POLARIZED_BLENDING_MASK = 17,
	ISP_DMS_DEBUG_MODE_DIAGONAL_BLENDING_MASK = 18,
	ISP_DMS_DEBUG_MODE_POLARIZED_DIAGONAL_BLENDING_MASK = 19,
	ISP_DMS_DEBUG_MODE_DIR_NONDIR_BLENDING_MASK = 20,
	ISP_DMS_DEBUG_MODE_BUTT
} ISP_DMS_DEBUG_MODE_E;

typedef struct tsISP_DMS_S {
	ts_bool bManualModeDms;
	ts_bool bDmsHfDetailEnhEn;
	ts_u16  u16DmsHfDetailEnhStr;
	ts_bool bDmsSharpenEn;
	ts_u16  u16DmsSharpenStr;
	ts_bool bDmsNpOffAbs;
	ts_u8  u8DmsNpOff;
	ts_u16 u16DmsGreySmaliasSlp;
	ts_u16 u16DmsGreenUserbThr;
	ts_u16 u16DmsGreenUserbSlp;
	ts_u16 u16DmsDxThr;
	ts_u16 u16DmsDxSlp;
	ts_u16 u16DmsDpThr;
	ts_u16 u16DmsDpSlp;
	ts_u16 u16DmsDushOffset;
	ts_u16 u16DmsDushThr;
	ts_u16 u16DmsDushSlp;
	ts_u16 u16DmsDpxThr;
	ts_u16 u16DmsDpxSlp;
	ts_u8  u8DmsFcrateStr;
	ts_u16 u16DmsFcSlp;
	ts_u16 u16DmsAliasThr;
	ts_u16 u16DmsAliasSlp;
	ts_u16 u16DmsBldaliasRhr;
	ts_u16 u16DmsBldaliasSlp;
	ts_u16 u16DmsSatThr;
	ts_u16 u16DmsSatSlp;
	ts_u16 u16DmsBdOption;
	ISP_DMS_DEBUG_MODE_E enDmsDebugMode;
} ISP_DMS_S;

typedef struct tsISP_USR_PREFERENCE_S{
	TS_U32	brightness;
	TS_U32	contrast;
	TS_U32	saturation;
	TS_U32	sharpness;
	TS_U32	chrominance;
	TS_U32	raw_denoise;	/* 2D */
	TS_U32	yuv_denoise;	/* 3D */
}ISP_USR_PREFERENCE_S;

typedef struct tsISP_DOL2_NP_ATTR_S {
    TS_U16 dol2_np_lgain;
    TS_U16 dol2_np_mgain;
    TS_U16 dol2_np_trans_gain;
    TS_U16 dol2_np_fit_order;
} ISP_DOL2_NP_ATTR_S;

typedef struct tsISP_RNR_ATTR_S {
    TS_BOOL b_manual_mode_rnr;
    TS_U16 rnr_blt_lf_np_gain;
    TS_U16 rnr_blt_lf_np_str;
} ISP_RNR_ATTR_S;

typedef struct tsISP_DMS_ATTR_S {
    TS_BOOL b_manual_mode_dms;
    TS_BOOL b_dms_hf_detail_enh_en;
    TS_U16 dms_hf_detail_enh_str;
    TS_BOOL b_dms_sharpen_en;
    TS_U16 dms_sharpen_str;
    TS_BOOL b_dms_np_off_abs;
    TS_U8 dms_np_off;
    TS_U16 dms_grey_smalias_slp;
    TS_U16 dms_green_userb_thr;
    TS_U16 dms_green_userb_slp;
    TS_U16 dms_dx_thr;
    TS_U16 dms_dx_slp;
    TS_U16 dms_dp_thr;
    TS_U16 dms_dp_slp;
    TS_U16 dms_dush_offset;
    TS_U16 dms_dush_thr;
    TS_U16 dms_dush_slp;
    TS_U16 dms_dpx_thr;
    TS_U16 dms_dpx_slp;
    TS_U8 dms_fcrate_str;
    TS_U16 dms_fc_slp;
    TS_U16 dms_alias_thr;
    TS_U16 dms_alias_slp;
    TS_U16 dms_bldalias_thr;
    TS_U16 dms_bldalias_slp;
    TS_U16 dms_sat_thr;
    TS_U16 dms_sat_slp;
    TS_U16 dms_bd_option;
    ISP_DMS_DEBUG_MODE_E e_dms_debug_mode;
} ISP_DMS_ATTR_S;

typedef struct tsISP_LSH_DLTI_ATTR_S {
    TS_BOOL b_lsh_dlti_en;
    TS_U16 lsh_dlti_pos_offset;
    TS_U8 lsh_dlti_pos_mid;
    TS_U8 lsh_dlti_pos_end;
    TS_U16 lsh_dlti_pos_y1;
    TS_U16 lsh_dlti_pos_y2;
    TS_U16 lsh_dlti_pos_y3;
    TS_U16 lsh_dlti_neg_offset;
    TS_U8 lsh_dlti_neg_mid;
    TS_U8 lsh_dlti_neg_end;
    TS_U16 lsh_dlti_neg_y1;
    TS_U16 lsh_dlti_neg_y2;
    TS_U16 lsh_dlti_neg_y3;
} ISP_LSH_DLTI_ATTR_S;

typedef struct tsISP_SCALE_ATTR_S {
    TS_U8 psc_roi;
    TS_U8 sca_roi;
    TS_U8 sca_dewarp;
    TS_U8 scb_roi;
    TS_U8 scb_dewarp;
    TS_U8 scd_roi;
    TS_U8 sce_roi;
} ISP_SCALE_ATTR_S;

typedef struct tsISP_CCM_ATTR_S {
	TS_BOOL b_manual_mode_ccm;
	TS_BOOL b_color_matrix_enable;
	TS_U16 ccm_color_matrix_rr;
	TS_U16 ccm_color_matrix_rg;
	TS_U16 ccm_color_matrix_rb;
	TS_U16 ccm_color_matrix_gr;
	TS_U16 ccm_color_matrix_gg;
	TS_U16 ccm_color_matrix_gb;
	TS_U16 ccm_color_matrix_br;
	TS_U16 ccm_color_matrix_bg;
	TS_U16 ccm_color_matrix_bb;
	TS_U16 ccm_saturation;
	TS_BOOL b_ccm_manual_index_enable;
	TS_U16 ccm_index;
	TS_U16 ccm_inter_param;

} ISP_CCM_ATTR_S;

typedef enum tsISP_DUMP_VIN_ZSL_STROBE_E {
	ISP_DUMP_VIN_ZSL_STROBE_NORMAL = 0,
	ISP_DUMP_VIN_ZSL_STROBE_720p,
	ISP_DUMP_VIN_ZSL_STROBE_1080p,
	ISP_DUMP_VIN_ZSL_STROBE_640x480,
	ISP_DUMP_VIN_ZSL_STROBE_BUTT
} ISP_DUMP_VIN_ZSL_STROBE_E;

typedef enum tsISP_DUMP_VIN_ZSL_LOAD_E {
	ISP_DUMP_VIN_ZSL_LOAD_1080p = 0,
	ISP_DUMP_VIN_ZSL_LOAD_720p,
	ISP_DUMP_VIN_ZSL_LOAD_640x480,
	ISP_DUMP_VIN_ZSL_LOAD_BUTT
} ISP_DUMP_VIN_ZSL_LOAD_E;
	
typedef enum tsISP_VIN_DUMP_RAW_MODE_E {
	ISP_VIN_DUMP_RAW_MODE_SENSOR_RAW16_OR_RAW12 = 0,
	ISP_VIN_DUMP_RAW_MODE_FBC_RAW8,
	ISP_VIN_DUMP_RAW_MODE_CMB_RAW20,
	ISP_VIN_DUMP_RAW_MODE_CMB_RAW16,
	ISP_VIN_DUMP_RAW_MODE_LTM_RAW12,
	ISP_VIN_DUMP_RAW_MODE_L2Q_RAW12,
	ISP_VIN_DUMP_RAW_MODE_LTM_AMD_L2Q_RAW12,
	ISP_VIN_DUMP_RAW_MODE_BUTT
} ISP_VIN_DUMP_RAW_MODE_E;

typedef struct tsISP_GENERALDUMP_ATTR_S {
	ISP_DUMP_VIN_ZSL_STROBE_E e_vin_zsl_strobe;
	ISP_DUMP_VIN_ZSL_LOAD_E e_vin_zsl_load;
	TS_U8 vin_dump_multi_cam_index;
	ISP_VIN_DUMP_RAW_MODE_E e_vin_dump_raw_mode;
	TS_U16 vin_dump_raw;
	TS_U16 vin_win_sv_dump0;
	TS_U16 vin_win_ev_dump0;
	TS_U16 vin_win_sh_dump0;
	TS_U16 vin_win_eh_dump0;
	TS_U16 vin_win_sv_dump1;
	TS_U16 vin_win_ev_dump1;
	TS_U16 vin_win_sh_dump1;
	TS_U16 vin_win_eh_dump1;
	TS_U16 vout0_dump_yuv420_8bit;
	TS_U16 vout0_dump_yuv422_8bit;
	TS_U16 vout0_dump_rgb24;
	TS_U16 vout0_dump_rgb888;
	TS_U16 vout0_dump_rgb10;
	TS_U16 vout0_dump_rgb565;
	TS_U16 vout2_dump_rgb24;
	TS_U16 vout3_dump_tnr_yuv420_8bit;
	TS_U16 vout1_dump_yuv420_8bit;
	TS_U16 vout2_dump_yuv420_8bit;
	TS_U16 vout3_dump_sce_yuv420_8bit;
	TS_BOOL b_vout_save_at_the_same_time;
	TS_U8 vout_dump_dm_select_r0;
	TS_U8 vout_dump_dm_select_r1;
	TS_U8 vout_dump_tnr_select_r0;
	TS_U8 vout_dump_tnr_select_r1;
	TS_U8 vout_dump_csc2_select_r0;
	TS_U8 vout_dump_csc2_select_r1;
	TS_U8 vout_simultaneous_dump_frame;
	TS_U8 vout0_cut16_9;
	TS_U8 vout1_cut16_9;
	TS_U8 vout2_cut16_9;
	TS_U8 vout3_cut16_9;
	TS_U16 vin_load4_ch_raw;
	TS_BOOL b_vin_ddr_process_enable;
	TS_U16 vin_ddr_process_num;
	TS_U8 vin_ddr_process_fps;
} ISP_GENERALDUMP_ATTR_S;

typedef struct tsISP_DPC_ATTR_S {
	TS_BOOL b_manual_mode_dpc;
	TS_BOOL b_dpc_debug;
	TS_BOOL b_dpc_ge_en;
	TS_U16 dpc_intpl_alpha;
	TS_U16 dpc_div_slp;
	TS_U16 dpc_div_thr;
	TS_U16 dpc_stdev_mask_thr;
	TS_U16 dpc_dp_dis_thr;
	TS_U16 dpc_cur_med_thr;
	TS_U16 dpc_ge_str;
	TS_U16 dpc_ge_thr;
	TS_U16 dpc_ge_sens;
	TS_U16 dpc_ge_slope;
	TS_U16 dpc_np_gain;
	TS_U16 dpc_np_offset;
} ISP_DPC_ATTR_S;

typedef struct tsISP_BLACKLEVEL_ATTR_S {
	TS_BOOL b_manual_mode_blc;
	TS_BOOL b_black_level_enable;
	TS_U16 black_level_r;
	TS_U16 black_level_gr;
	TS_U16 black_level_gb;
	TS_U16 black_level_b;
} ISP_BLACKLEVEL_ATTR_S;


typedef enum tsISP_LSC_MESH_RADIAL_SWITCH_E {
	ISP_LSC_MESH_RADIAL_SWITCH_MESH = 0,
	ISP_LSC_MESH_RADIAL_SWITCH_RADIAL,
	ISP_LSC_MESH_RADIAL_SWITCH_BUTT
} ISP_LSC_MESH_RADIAL_SWITCH_E;
	

typedef struct tsISP_LSC_ATTR_S {
	TS_BOOL b_manual_mode_lsc;
	TS_BOOL b_lsc_enable;
	ISP_LSC_MESH_RADIAL_SWITCH_E e_lsc_mesh_radial_switch;
	TS_U8 lsc_mesh_frac;
	TS_U8 lsc_radial_frac;
	TS_U16 lsc_strength;
} ISP_LSC_ATTR_S;

typedef enum tsISP_DPF_DEBUG_MODE_E {
	ISP_DPF_DEBUG_MODE_NORMAL_OUTPUT = 0,
	ISP_DPF_DEBUG_MODE_HUE_MASK_MAP,
	ISP_DPF_DEBUG_MODE_SATURATION_MASK_MAP,
	ISP_DPF_DEBUG_MODE_SAD_MASK_MAP,
	ISP_DPF_DEBUG_MODE_CONTRAST_MASK_MAP,
	ISP_DPF_DEBUG_MODE_DPF_AFFECTIVE_AREA_MASK_MAP,
	ISP_DPF_DEBUG_MODE_BUTT
} ISP_DPF_DEBUG_MODE_E;

typedef struct tsISP_DPF_ATTR_S {
	TS_BOOL b_manual_mode_dpf;
	TS_U16 dpf_sad_slp;
	TS_U16 dpf_sad_thr;
	TS_U16 dpf_hue1_slope;
	TS_U16 dpf_hue1_th;
	TS_U16 dpf_hue2_slope;
	TS_U16 dpf_hue2_th;
	TS_U16 dpf_hue_str;
	TS_U16 dpf_sat1_slope;
	TS_U16 dpf_sat1_th;
	TS_U16 dpf_sat_str;
	TS_U16 dpf_luma11_slope;
	TS_U16 dpf_luma11_th;
	TS_U16 dpf_luma12_slope;
	TS_U16 dpf_luma12_th;
	TS_U16 dpf_luma21_slope;
	TS_U16 dpf_luma21_th;
	TS_U16 dpf_luma22_slope;
	TS_U16 dpf_luma22_th;
	TS_U16 dpf_luma_str;
	TS_U16 dpf_hsl_slope;
	TS_U16 dpf_hsl_th;
	TS_U16 dpf_purple_str;
	TS_U16 dpf_satur_str;
	ISP_DPF_DEBUG_MODE_E e_dpf_debug_mode;
} ISP_DPF_ATTR_S;

typedef struct tsISP_CNR_ATTR_S {
	TS_BOOL b_manual_mode_cnr;
	TS_U16 cnr_motion_noise_gain;
	TS_U16 cnr_luma_noise_offset;
	TS_U8 cnr_luma_noise_mid;
	TS_U8 cnr_luma_noise_end;
	TS_U16 cnr_luma_noise_y1;
	TS_U16 cnr_luma_noise_y2;
	TS_U16 cnr_luma_noise_y3;
	TS_U16 cnr_rfilt_sat_offset;
	TS_U8 cnr_rfilt_sat_mid;
	TS_U8 cnr_rfilt_sat_end;
	TS_U16 cnr_rfilt_sat_y1;
	TS_U16 cnr_rfilt_sat_y2;
	TS_U16 cnr_rfilt_sat_y3;
	TS_U16 cnr_rfilt_diff_offset;
	TS_U8 cnr_rfilt_diff_mid;
	TS_U8 cnr_rfilt_diff_end;
	TS_U16 cnr_rfilt_diff_y1;
	TS_U16 cnr_rfilt_diff_y2;
	TS_U16 cnr_rfilt_diff_y3;
	TS_U8 cnr_gauss_noise_offset;
	TS_U16 cnr_gauss_noise_gain;
	TS_U8 cnr_uvdiff_sft;
	TS_U16 cnr_uv_blend_offset;
	TS_U8 cnr_uv_blend_mid;
	TS_U8 cnr_uv_blend_end;
	TS_U16 cnr_uv_blend_y1;
	TS_U16 cnr_uv_blend_y2;
	TS_U16 cnr_uv_blend_y3;
	TS_BOOL b_cnr_fc_sup_en;
	TS_U16 cnr_fc_sup_offset;
	TS_U8 cnr_fc_sup_mid;
	TS_U8 cnr_fc_sup_end;
	TS_U16 cnr_fc_sup_y1;
	TS_U16 cnr_fc_sup_y2;
	TS_U16 cnr_fc_sup_y3;
} ISP_CNR_ATTR_S;

/***************************************************
*				ir auto attr					   *  
***************************************************/
	
typedef enum {
    ISP_IR_SWITCH_TO_NONE,
	ISP_IR_SWITCH_TO_DAY,
	ISP_IR_SWITCH_TO_NIGHT,
	ISP_IR_SWITCH_STATUS_BUTT,
}ISP_IR_SWITCH_STATUS_E;
	
typedef struct{
	TS_U32                 Day2NightThresh;  //  defaultValue="2004604572" numberType="uint" min="0" max="4096" edit="true"
	TS_U32                 IrNight2DayThresh;
    TS_U32                 WlNight2DayThresh;
    TS_U32                 RgMax;
    TS_U32                 RgMin;
    TS_U32                 BgMax;
    TS_U32                 BgMin;
	ISP_IR_SWITCH_STATUS_E IR_switch;
}ISP_IR_AUTO_ATTR_S;


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* __TS_COMM_ISP_H__ */

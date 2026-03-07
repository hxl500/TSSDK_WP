/*
 * Copyright © 2019 TSING MICRO.com.cn All Rights Reserved
 * Description: ts_comm_3a.h
 * Author:TsingMicro multimedia software group
 * Create: 2022/06/02
 */

#ifndef __TS_COMM_3A_H__
#define __TS_COMM_3A_H__

#include "ts_common.h"
#include "ts_comm_isp.h"
//#include "ts_comm_sns.h"

// i5

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#define IS_LINEAR_MODE(mode)      (WDR_MODE_NONE == (mode))
#define IS_WDR_MODE(mode)         ((!IS_LINEAR_MODE(mode)) && ((mode) < WDR_MODE_BUTT))
#define IS_HDR_MODE(mode)         (((mode) > DYNAMIC_RANGE_SDR10) && ((mode) < DYNAMIC_RANGE_BUTT))
#define IS_BUILT_IN_WDR_MODE(mode) (WDR_MODE_BUILT_IN == (mode))
#define IS_FS_WDR_MODE(mode)      ((!IS_LINEAR_MODE(mode))&& (!IS_BUILT_IN_WDR_MODE(mode)) && ((mode) < WDR_MODE_BUTT))
#define IS_2to1_WDR_MODE(mode)    ((WDR_MODE_2To1_FRAME == (mode)) || (WDR_MODE_2To1_FRAME_FULL_RATE == (mode)) || (WDR_MODE_2To1_LINE == (mode))  || (WDR_MODE_QUDRA == (mode)))
#define IS_3to1_WDR_MODE(mode)    ((WDR_MODE_3To1_FRAME == (mode)) || (WDR_MODE_3To1_FRAME_FULL_RATE == (mode)) || (WDR_MODE_3To1_LINE == (mode)))
#define IS_4to1_WDR_MODE(mode)    ((WDR_MODE_4To1_FRAME == (mode)) || (WDR_MODE_4To1_FRAME_FULL_RATE == (mode)) || (WDR_MODE_4To1_LINE == (mode)))
#define IS_FULL_WDR_MODE(mode)    ((WDR_MODE_2To1_FRAME_FULL_RATE == (mode)) || (WDR_MODE_3To1_FRAME_FULL_RATE == (mode)) || (WDR_MODE_4To1_FRAME_FULL_RATE == (mode)))
#define IS_HALF_WDR_MODE(mode)    ((WDR_MODE_2To1_FRAME == (mode)) || (WDR_MODE_3To1_FRAME == (mode)) || (WDR_MODE_4To1_FRAME == (mode)))
#define IS_LINE_WDR_MODE(mode)    ((WDR_MODE_2To1_LINE == (mode)) || (WDR_MODE_3To1_LINE == (mode)) || (WDR_MODE_4To1_LINE == (mode)) || (WDR_MODE_QUDRA == (mode)))

#define IS_STITCH_MAIN_PIPE(ViPipe,MainPipe) ((ViPipe) == (MainPipe))
#define IS_OFFLINE_MODE(mode)    (ISP_MODE_RUNNING_OFFLINE    == (mode))
#define IS_ONLINE_MODE(mode)     (ISP_MODE_RUNNING_ONLINE     == (mode))
#define IS_SIDEBYSIDE_MODE(mode) (ISP_MODE_RUNNING_SIDEBYSIDE == (mode))
#define IS_STRIPING_MODE(mode)   (ISP_MODE_RUNNING_STRIPING   == (mode))

#define MAX_REGISTER_ALG_LIB_NUM 2

typedef enum tsISP_ALG_MOD_E
{
    ISP_ALG_AE = 0,
    ISP_ALG_AF,
    ISP_ALG_AWB,
    ISP_ALG_BLC,
    ISP_ALG_FELOGLUT,
    ISP_ALG_LOGLUT,
    ISP_ALG_DP,
    ISP_ALG_PREGAMMA,
    ISP_ALG_DRC,
    ISP_ALG_DEMOSAIC,
    ISP_ALG_ANTIFALSECOLOR,
    ISP_ALG_GAMMA,
    ISP_ALG_GE,
    ISP_ALG_SHARPEN,
    ISP_ALG_EDGEAMRK,
    ISP_ALG_FrameWDR,
    ISP_ALG_FPN,
    ISP_ALG_DEHAZE,
    ISP_ALG_ACM,
    ISP_ALG_LCAC,
    ISP_ALG_GCAC,
    ISP_ALG_CSC,
    ISP_ALG_EXPANDER,
    ISP_ALG_MCDS,
    ISP_ALG_LSC,
    ISP_ALG_RLSC,
    ISP_ALG_FeLSC,
    ISP_ALG_RC,
    ISP_ALG_RGBIR,
    ISP_ALG_HRS,
    ISP_ALG_DG,
    ISP_ALG_BAYERNR,
    ISP_ALG_FLICKER,
    ISP_ALG_SPLIT,
    ISP_ALG_LDCI,
    ISP_ALG_CA,
    ISP_ALG_CLUT,
    ISP_ALG_DETAIL,
    ISP_ALG_BUTT,
} ISP_ALG_MOD_E;

typedef enum tsISP_CTRL_CMD_E
{
    ISP_WDR_MODE_SET = 8000,
    ISP_PROC_WRITE,

    ISP_AE_FPS_BASE_SET,
    ISP_AE_BLC_SET,
    ISP_AE_RC_SET,
    ISP_AE_BAYER_FORMAT_SET,

    ISP_AWB_ISO_SET,  /* set iso, change saturation when iso change */
    ISP_CHANGE_IMAGE_MODE_SET,
    ISP_UPDATE_INFO_GET,
    ISP_FRAMEINFO_GET,
    ISP_ATTACHINFO_GET,
    ISP_COLORGAMUTINFO_GET,
    ISP_AWB_INTTIME_SET,
    ISP_BAS_MODE_SET,
    ISP_PROTRIGGER_SET,
    ISP_AWB_PIRIS_SET,
    ISP_AWB_SNAP_MODE_SET,
    ISP_AWB_ZONE_ROW_SET,
    ISP_AWB_ZONE_COL_SET,
    ISP_AWB_ZONE_BIN_SET,
    ISP_CTRL_CMD_BUTT,
} ISP_CTRL_CMD_E;

typedef struct tsISP_CTRL_PROC_WRITE_S
{
    TS_CHAR *pcProcBuff;
    TS_U32   u32BuffLen;
    TS_U32   u32WriteLen;   /* The len count should contain '\0'. */
} ISP_CTRL_PROC_WRITE_S;

typedef enum tsISP_AE_STRATEGY_E {
    AE_EXP_NORMALLIGHT_PRIOR = 0,
    AE_EXP_HIGHLIGHT_PRIOR = 1,
    AE_EXP_LOWLIGHT_PRIOR  = 2,
    AE_STRATEGY_MODE_BUTT
} ISP_AE_STRATEGY_E;

typedef struct tsISP_STITCH_ATTR_S
{
    TS_BOOL bStitchEnable;
    TS_BOOL bMainPipe;
    TS_U8   u8StitchPipeNum;
    TS_S8   as8StitchBindId[VI_MAX_PIPE_NUM];
} ISP_STITCH_ATTR_S;

/* Defines the maximum exposure time or gain and minimum exposure time or gain */
typedef struct tsISP_AE_RANGE_S {
    TS_U32 u32Max;  /* RW;Range:[0,0xFFFFFFFF];Format:32.0;Maximum value */
    TS_U32 u32Min;  /* RW;Range:[0,0xFFFFFFFF];Format:32.0;limited Range:[0,u32Max],Minimum value */
} ISP_AE_RANGE_S;

/*
Defines the AE mode
0 = Automatic frame rate reduction mode (slow shutter mode)
1 = Constant frame rate mode
*/
typedef enum tsISP_AE_MODE_E {
    AE_MODE_SLOW_SHUTTER = 0,
    AE_MODE_FIX_FRAME_RATE  = 1,
    AE_MODE_BUTT
} ISP_AE_MODE_E;

/*
Defines the anti-flicker mode.
0 = The epxosure time is fixed to be the multiplies of 1/(2*frequency) sec,it may lead to over exposure in the high-luminance environments.
1 = The anti flicker may be closed to avoid over exposure in the high-luminance environments.
*/
typedef enum tsISP_ANTIFLICKER_MODE_E {
    ISP_ANTIFLICKER_NORMAL_MODE = 0x0,
    ISP_ANTIFLICKER_AUTO_MODE   = 0x1,
    ISP_ANTIFLICKER_MODE_BUTT
} ISP_ANTIFLICKER_MODE_E;

/* Defines the anti-flicker attribute */
typedef struct tsISP_ANTIFLICKER_S {
    TS_BOOL bEnable;              /* RW; Range:[0, 1]; Format:1.0; */
    TS_U8   u8Frequency;          /* RW; Range:[0, 255]; Format:8.0; usually this value is 50 or 60  which is the frequency of the AC power supply */
    ISP_ANTIFLICKER_MODE_E  enMode;
} ISP_ANTIFLICKER_S;

typedef struct tsISP_AE_ATTR_S {
    ISP_AE_RANGE_S stExpTimeRange;   /* RW; Range:[0x0, 0xFFFFFFFF]; Format:32.0; sensor exposure time (unit: us ), it's related to the specific sensor */
    ISP_AE_RANGE_S stAGainRange;     /* RW; Range:[0x400, 0xFFFFFFFF]; Format:22.10; sensor analog gain (unit: times, 10bit precision), it's related to the specific sensor */
    ISP_AE_RANGE_S stDGainRange;     /* RW; Range:[0x400, 0xFFFFFFFF]; Format:22.10;  sensor digital gain (unit: times, 10bit precision), it's related to the specific sensor */
    ISP_AE_RANGE_S stISPDGainRange;  /* RW; Range:[0x400, 0x40000]; Format:22.10;  ISP digital gain (unit: times, 10bit precision), it's related to the ISP digital gain range */
    ISP_AE_RANGE_S stSysGainRange;  /* RW; Range:[0x400, 0x40000]; Format:22.10;  ISP digital gain (unit: times, 10bit precision), it's related to the ISP digital gain range */
    ISP_AE_STRATEGY_E enAEStrategyMode;  /* RW; Range:[0, 1]; Format:1.0; Support Highlight prior or Lowlight prior */

    ISP_AE_MODE_E  enAEMode;         /* RW; Range:[0, 1]; Format:1.0; AE mode(slow shutter/fix framerate)(onvif) */
    ISP_ANTIFLICKER_S stAntiflicker;
} ISP_AE_ATTR_S;

typedef struct hiISP_ME_ATTR_S {
    TS_U32 u32ExpTime;        /* RW; Range:[0x0, 0xFFFFFFFF]; Format:32.0; sensor exposure time (unit: us ), it's related to the specific sensor */
    TS_U32 u32SensorGain;     /* RW; Range:[0x400, 0xFFFFFFFF]; Format:22.10; sensor analog gain (unit: times, 10bit precision), it's related to the specific sensor */
    TS_U32 u32ISPDGain;       /* RW; Range:[0x1000, 0xFFFFFFFF]; Format:22.10; ISP digital gain (unit: times, 12bit precision), it's related to the ISP digital gain range */
} ISP_ME_ATTR_S;

/******************************************************/
/********************* AE structs ************************/
typedef struct tsISP_EXP_INFO_S
{
    TS_U32 u32ExpTime;                 /*R; Range:[0x0, 0xFFFFFFFF]; Format:32.0;*/
    TS_U32 u32ShortExpTime;            /*R; Range:[0x0, 0xFFFFFFFF]; Format:32.0;*/
    TS_U32 u32MedianExpTime;           /*R; Range:[0x0, 0xFFFFFFFF]; Format:32.0;*/
    TS_U32 u32LongExpTime;             /*R; Range:[0x0, 0xFFFFFFFF]; Format:32.0;*/
    TS_U32 u32AGain;                   /*R; Range:[0x400, 0xFFFFFFFF]; Format:22.10;*/
    TS_U32 u32DGain;                   /*R; Range:[0x400, 0xFFFFFFFF]; Format:22.10;*/
    TS_U32 u32ISPDGain;                /*R; Range:[0x400, 0xFFFFFFFF]; Format:22.10;*/
    TS_U32 u32TotalGain;               /*R; Range:[0x400, 0xFFFFFFFF]; Format:22.10;*/
    TS_U32 u32Exposure;                /*R; Range:[0x40, 0xFFFFFFFF]; Format:26.6;*/
    TS_BOOL bExposureIsMAX;            /*R; Range:[0, 1]; Format:1.0;*/
    TS_S16 s16HistError;               /*R; Range:[-32768, 32767]; Format:16.0;*/
    TS_U32 au32AE_Hist1024Value[HIST_NUM]; /*R; Range:[0x0, 0xFFFFFFFF]; Format:32.0; 1024 bins histogram for channel 1 */

    TS_U32 u32AveLum;                   /*R; Range:[0x0, 0xFFFFFFFF]; Format:32.0;*/
    TS_U32 u32LinesPer500ms;           /*R; Range:[0x0, 0xFFFFFFFF]; Format:32.0; exposure lines per 500ms */
    TS_U32 u32Fps;                     /*R; Range:[0x0, 0xFFFFFFFF]; Format:32.0; actual fps */
} ISP_EXP_INFO_S;

typedef struct tsISP_EXPOSURE_ATTR_S {
    ISP_OP_TYPE_E   enOpType;

    ISP_AE_ATTR_S   stAuto;
    ISP_ME_ATTR_S   stManual;
} ISP_EXPOSURE_ATTR_S;

typedef struct tsISP_AE_PARAM_ATTR_S {
	TS_U8 u8AeConvergeStep; /* RW; Range:[1, 32]; AE convergence speed. 步长 */
	TS_U8 u8AeMaxConvergeSpeed; /* RW; Range:[30, 200]; AE convergence speed.*/
	TS_U8 u8AeStableInRange; /* RW; Range:[2, 255]; AE convergence stability range. */
	TS_U16 u16AeStableOutRange; /* RW; Range:[5, 512]; AE convergence stability range. */
	TS_U16 u16AeSaturationThr; /* RW; Range:[512, 1024]; */
	TS_U8 u8AeStatsYSelectidx; /* RW; Range:[0, 3]; */
	TS_U16 u16AeStabletimeoutFrame; /* RW; Range:[0, 4096]; */
	TS_BOOL bAeManualSetParamEnable;
	TS_U16 u16AeTarget; /* RW; Range:[0, 1023]; Auto exposure target - base brightness target. */
	TS_U8 u8AeHighlevel; /* RW; Range:[0, 100]; Auto exposure target parameter - bright area ratio configuration. */
	TS_U8 u8AeLowlevel; /* RW; Range:[0, 100]; Auto exposure target parameter - dark area ratio configuration. */
	TS_U8 u8AeHighcontrTarget; /* RW; Range:[0, 100]; Auto exposure target parameter - high dynamic range scene brightness target. */
	TS_U8 u8AeHighcontrast; /* RW; Range:[0, 255]; Auto exposure target parameter - dynamic range upper bound */
	TS_U8 u8AeLowcontrast; /* RW; Range:[0, 255]; Auto exposure target parameter - dynamic range lower bound */
	TS_U8 u8AeLongtarget; /* RW; Range:[1, 255]; AE convergence stability range. */
	TS_U8 u8AeLongratiomax; /* RW; Range:[1, 255]; AE convergence stability range. */
	TS_U8 u8AeMidtarget; /* RW; Range:[1, 255]; AE convergence stability range. */
	TS_U8 u8AeMidratiomax; /* RW; Range:[1, 255]; AE convergence stability range. */
	TS_U32 u32AeHdrsatmaxpcent;
	TS_U32 u32AeMinintegration;/* RW; Range:[0, -]; AE shutter min value. */
	TS_U32 u32AeMaxintegration;/* RW; Range:[0, -]; AE shutter max value. */
	TS_U32 u32AeMinAgain;/* RW; Range:[0, -]; again min value. */
	TS_U32 u32AeMaxAgain;/* RW; Range:[0, -]; again max value. */
	TS_U32 u32AeMinDgain;/* RW; Range:[0, -]; dgain min value. */
	TS_U32 u32AeMaxDgain;/* RW; Range:[0, -]; dgain max value. */
	TS_U32 u32AeMinIspgain;/* RW; Range:[0, -]; ispgain min value. */
	TS_U32 u32AeMaxIspgain;/* RW; Range:[0, -]; ispgain max value. */
	TS_U32 u32AeMinSysgain;/* RW; Range:[0, -]; sysgain min value. */
	TS_U32 u32AeMaxSysgain;/* RW; Range:[0, -]; sysgain max value. */
	TS_U32 u32AeFixedfps;/* RW; Range:[0, 60]; AE fixed fps range. */
} ISP_AE_PARAM_ATTR_S;

typedef struct tsISP_AE_FACE_PARAM_S {
	TS_BOOL bEnable;
	TS_U32 faceStatsSel;
	TS_U32 convergeStep;
	TS_U32 stableRange;
	TS_U32 faceRawTarget;
	TS_U32 uFaceYuvTarget;
	TS_U32 faceScale;
	TS_U32 faceCropTop;
	TS_U32 faceCropBot;
	TS_U32 faceCropLef;
	TS_U32 faceCropRig;
	TS_U32 faceInCntThr;
	TS_U32 faceOutCntThr;
	TS_U32 smallFaceThr;
} ISP_AE_FACE_PARAM_S;

typedef struct tsISP_FACE_ROI_S
{
	TS_U32 frameIdx;
	TS_U32 x;
	TS_U32 y;
	TS_U32 w;
	TS_U32 h;
} ISP_FACE_ROI_S;

typedef enum tsISP_FACE_LANDMARK_E
{
	ISP_FACE_LANDMARK_LEFTEYPPOSL = 0,
	ISP_FACE_LANDMARK_LEFTEYPPOSR,
	ISP_FACE_LANDMARK_RIGHTEYEPOSL,
	ISP_FACE_LANDMARK_RIGHTEYEPOSR,
	ISP_FACE_LANDMARK_MOUSEPOSL,
	ISP_FACE_LANDMARK_MOUSEPOSR,
	ISP_FACE_LANDMARK_NOSEPOSE,
	ISP_FACE_LANDMARK_BUTT
} ISP_FACE_LANDMARK_E;


typedef struct tsISP_FACE_LANDMARK_S
{
	SHORT_POINT_S landmark[ISP_FACE_LANDMARK_BUTT];
} ISP_FACE_LANDMARK_S;


typedef struct tsISP_AE_FACE_INFO_S {
	TS_U8              faceNum;
	TS_U16             imageWidth;
	TS_U16             imageHeight;
	TS_U16			   currIndex;//[0-3]
	ISP_FACE_ROI_S      sFaceRoi;
	TS_U8              faceConfidence;
	TS_U8              faceRotAngle;
	ISP_FACE_LANDMARK_E sFaceLandmark;
	ISP_FACE_ROI_S      sFaceLandmarkRoi;
	TS_U8              useLandmarkRoi;
} ISP_AE_FACE_INFO_S;
/********************* AWB structs ************************/
typedef struct tsISP_WB_INFO_S
{
	TS_U32 awbRGain;
	TS_U32 awbGGain;
	TS_U32 awbBGain;
} ISP_WB_INFO_S;

typedef struct tsISP_AWB_ENABLE_ATTR_S {
	TS_BOOL bAwbEnable;
	TS_BOOL bAwbManualSetParamEnable;
} ISP_AWB_ENABLE_ATTR_S;

typedef struct tsISP_AWB_CONFIG_ATTR_S {
	TS_BOOL bWbGainEnable;
	TS_U16 u16WbGainR;
	TS_U16 u16WbGainG;
	TS_U16 u16WbGainB;
} ISP_AWB_CONFIG_ATTR_S;

typedef struct tsISP_AWB_PARAM_S {
	TS_BOOL b_awb_log_enable;
	TS_U8 awb_converge_step;
	TS_U8 awb_stable_range;
	TS_BOOL b_awb_manual_roi_enable;
	TS_U8 awb_manual_roiX;
	TS_U8 awb_manual_roiY;
	TS_U8 awb_manual_roiW;
	TS_U8 awb_manual_roiH;
	TS_U16 awb_pref_gainR;
	TS_U16 awb_pref_gainB;
	TS_U16 awb_lowlight_lux_thr;
	TS_U16 awb_lowct_rgpos_thr;
	TS_U16 awb_channel_gainR;
	TS_U16 awb_channel_gainG;
	TS_U16 awb_channel_gainB;
} ISP_AWB_PARAM_S;

typedef struct tsISP_AWB_DETECT_ZONE_S {
	TS_U8 awb_detect_zone_index;
	TS_BOOL b_awb_detect_zone_enable;
	TS_U16 awb_detect_zone_lux_start;
	TS_U16 awb_detect_zone_lux_end;
	TS_U16 awb_detect_zone_lux_buf;
	TS_U16 awb_detect_zone_left;
	TS_U16 awb_detect_zone_right;
	TS_U16 awb_detect_zone_top;
	TS_U16 awb_detect_zone_bot;
} ISP_AWB_DETECT_ZONE_S;

/********************************  AE  *************************************/
/* the init param of ae alg */
typedef struct tsISP_AE_PARAM_S
{
    SENSOR_ID SensorId;
    TS_U8  u8WDRMode;
    TS_U8  u8HDRMode;
    TS_U16 u16BlackLevel;
    TS_FLOAT f32Fps;
    ISP_BAYER_FORMAT_E enBayer;
    ISP_STITCH_ATTR_S stStitchAttr;

    TS_S32 s32Rsv;
} ISP_AE_PARAM_S;

/* the statistics of ae alg */
typedef struct tsISP_FE_AE_STAT_1_S
{
    TS_U32  u32PixelCount[ISP_CHN_MAX_NUM];
    TS_U32  u32PixelWeight[ISP_CHN_MAX_NUM];
    TS_U32  au32HistogramMemArray[ISP_CHN_MAX_NUM][1024];
} ISP_FE_AE_STAT_1_S;

typedef struct tsISP_FE_AE_STAT_2_S
{
    TS_U16  u16GlobalAvgR[ISP_CHN_MAX_NUM];
    TS_U16  u16GlobalAvgGr[ISP_CHN_MAX_NUM];
    TS_U16  u16GlobalAvgGb[ISP_CHN_MAX_NUM];
    TS_U16  u16GlobalAvgB[ISP_CHN_MAX_NUM];
} ISP_FE_AE_STAT_2_S;

typedef struct tsISP_FE_AE_STAT_3_S
{
    TS_U16  au16ZoneAvg[ISP_CHN_MAX_NUM][AE_ZONE_ROW][AE_ZONE_COLUMN][4];
} ISP_FE_AE_STAT_3_S;

typedef struct tsISP_FE_AE_STITCH_STAT_3_S
{
    TS_U16  au16ZoneAvg[VI_MAX_PIPE_NUM][ISP_CHN_MAX_NUM][AE_ZONE_ROW][AE_ZONE_COLUMN][4];
} ISP_FE_AE_STITCH_STAT_3_S;

typedef struct tsISP_BE_AE_STAT_1_S
{
    TS_U32  u32PixelCount;
    TS_U32  u32PixelWeight;
    TS_U32  au32HistogramMemArray[1024];
} ISP_BE_AE_STAT_1_S;

typedef struct tsISP_BE_AE_STAT_2_S
{
    TS_U16  u16GlobalAvgR;
    TS_U16  u16GlobalAvgGr;
    TS_U16  u16GlobalAvgGb;
    TS_U16  u16GlobalAvgB;
} ISP_BE_AE_STAT_2_S;

typedef struct tsISP_BE_AE_STAT_3_S
{
    TS_U16  au16ZoneAvg[AE_ZONE_ROW][AE_ZONE_COLUMN][4];
} ISP_BE_AE_STAT_3_S;

typedef struct tsISP_BE_AE_STITCH_STAT_3_S
{
    TS_U16  au16ZoneAvg[VI_MAX_PIPE_NUM][AE_ZONE_ROW][AE_ZONE_COLUMN][4];
} ISP_BE_AE_STITCH_STAT_3_S;

typedef struct tsISP_AE_INFO_S
{
    TS_U32  u32FrameCnt;    /* the counting of frame */

    ISP_FE_AE_STAT_1_S *pstFEAeStat1;
    ISP_FE_AE_STAT_2_S *pstFEAeStat2;
    ISP_FE_AE_STAT_3_S *pstFEAeStat3;
    ISP_FE_AE_STITCH_STAT_3_S *pstFEAeStiStat;
    ISP_BE_AE_STAT_1_S *pstBEAeStat1;
    ISP_BE_AE_STAT_2_S *pstBEAeStat2;
    ISP_BE_AE_STAT_3_S *pstBEAeStat3;
    ISP_BE_AE_STITCH_STAT_3_S *pstBEAeStiStat;
} ISP_AE_INFO_S;


typedef struct tsISP_AE_STAT_ATTR_S
{
    TS_BOOL bChange;

    TS_BOOL bHistAdjust;
    TS_U8 u8AEBESel;
    TS_U8 u8FourPlaneMode;
    TS_U8 u8HistOffsetX;
    TS_U8 u8HistOffsetY;
    TS_U8 u8HistSkipX;
    TS_U8 u8HistSkipY;

    TS_BOOL bModeUpdate;
    TS_U8 u8HistMode;
    TS_U8 u8AverMode;
    TS_U8 u8MaxGainMode;

    TS_BOOL bWightTableUpdate;
    TS_U8 au8WeightTable[VI_MAX_PIPE_NUM][AE_ZONE_ROW][AE_ZONE_COLUMN];
} ISP_AE_STAT_ATTR_S;

typedef enum tsISP_FSWDR_MODE_E
{
    ISP_FSWDR_NORMAL_MODE = 0x0,
    ISP_FSWDR_LONG_FRAME_MODE = 0x1,
    ISP_FSWDR_AUTO_LONG_FRAME_MODE = 0x2,
    ISP_FSWDR_MODE_BUTT
}ISP_FSWDR_MODE_E;

/* the final calculate of ae alg */
#define AE_INT_TIME_NUM     (4)
typedef struct tsISP_AE_RESULT_S
{
    TS_U32  u32IntTime[AE_INT_TIME_NUM];
    TS_U32  u32IspDgain;
    TS_U32  u32Again;
    TS_U32  u32Dgain;
    TS_U32  u32Iso;
    TS_U8   u8AERunInterval;

    TS_BOOL bPirisValid;
    TS_S32  s32PirisPos;
    TS_U32  u32PirisGain;

    ISP_FSWDR_MODE_E enFSWDRMode;
    TS_U32  au32WDRGain[4];
    TS_U32  u32HmaxTimes; /* unit is ns */

    ISP_AE_STAT_ATTR_S stStatAttr;
    ISP_DCF_UPDATE_INFO_S stUpdateInfo;
} ISP_AE_RESULT_S;

typedef struct tsISP_AE_EXP_FUNC_S
{
    TS_S32 (*pfn_ae_init)(TS_S32 s32Handle, const ISP_AE_PARAM_S *pstAeParam);
    TS_S32 (*pfn_ae_run)(TS_S32 s32Handle,
                         const ISP_AE_INFO_S *pstAeInfo,
                         ISP_AE_RESULT_S *pstAeResult,
                         TS_S32 s32Rsv
                        );
    TS_S32 (*pfn_ae_ctrl)(TS_S32 s32Handle, TS_U32 u32Cmd, TS_VOID *pValue);
    TS_S32 (*pfn_ae_exit)(TS_S32 s32Handle);
} ISP_AE_EXP_FUNC_S;

typedef struct tsISP_AE_REGISTER_S
{
    ISP_AE_EXP_FUNC_S stAeExpFunc;
} ISP_AE_REGISTER_S;

/********************************  AWB  *************************************/
typedef enum tsAWB_CTRL_CMD_E
{
    AWB_CCM_CONFIG_SET,
    AWB_CCM_CONFIG_GET,

    AWB_DEBUG_ATTR_SET,
    AWB_DEBUG_ATTR_GET,

    AWB_CTRL_BUTT,
} AWB_CTRL_CMD_E;

typedef struct tsAWB_CCM_CONFIG_S
{
    TS_BOOL bAWBBypassEn;
    TS_BOOL bManualSatEn;
    TS_BOOL bManualTempEn;

    TS_U32  u32ManualSatValue;
    TS_U32  u32ManualTempValue;
    TS_U16  u16CCMSpeed;

    TS_U16  au16HighCCM[9];
    TS_U16  au16LowCCM[9];
    TS_U16  u16HighColorTemp;
    TS_U16  u16LowColorTemp;
} AWB_CCM_CONFIG_S;

/* the init param of awb alg */
//typedef struct tsISP_AWB_PARAM_S
//{
//    SENSOR_ID SensorId;
//    TS_U8 u8WDRMode;
//    TS_U8 u8AWBZoneRow;
//    TS_U8 u8AWBZoneCol;
//    TS_U8 u8AWBZoneBin;
//    ISP_STITCH_ATTR_S stStitchAttr;
//    TS_U16 u16AWBWidth;
//    TS_U16 u16AWBHeight;
//    TS_S8 s8Rsv;
//} ISP_AWB_PARAM_S;

/* the statistics of awb alg */


typedef struct tsISP_AWB_STAT_1_S
{
    TS_U16  u16MeteringAwbAvgR;
    TS_U16  u16MeteringAwbAvgG;
    TS_U16  u16MeteringAwbAvgB;
    TS_U16  u16MeteringAwbCountAll;

} ISP_AWB_STAT_1_S;

typedef struct tsISP_AWB_STAT_2_S
{
    TS_U16  au16MeteringMemArrayAvgR[AWB_ZONE_NUM];
    TS_U16  au16MeteringMemArrayAvgG[AWB_ZONE_NUM];
    TS_U16  au16MeteringMemArrayAvgB[AWB_ZONE_NUM];
    TS_U16  au16MeteringMemArrayCountAll[AWB_ZONE_NUM];

} ISP_AWB_STAT_2_S;

typedef struct tsISP_AWB_STAT_STITCH_S
{
    TS_U16  u16ZoneCol;
    TS_U16  u16ZoneRow;
    TS_U16  u16ZoneBin;
    TS_U16  au16MeteringMemArrayAvgR[AWB_ZONE_STITCH_MAX];
    TS_U16  au16MeteringMemArrayAvgG[AWB_ZONE_STITCH_MAX];
    TS_U16  au16MeteringMemArrayAvgB[AWB_ZONE_STITCH_MAX];
    TS_U16  au16MeteringMemArrayCountAll[AWB_ZONE_STITCH_MAX];

} ISP_AWB_STAT_STITCH_S;

typedef struct tsISP_AWB_STAT_RESULT_S
{
    TS_U16 *pau16ZoneAvgR;
    TS_U16 *pau16ZoneAvgG;
    TS_U16 *pau16ZoneAvgB;
    TS_U16 *pau16ZoneCount;
} ISP_AWB_STAT_RESULT_S;

typedef struct tsISP_AWB_INFO_S
{
    TS_U32  u32FrameCnt;

    ISP_AWB_STAT_1_S *pstAwbStat1;
    ISP_AWB_STAT_RESULT_S stAwbStat2;
    TS_U8 u8AwbGainSwitch;
    TS_U32 au32WDRWBGain[ISP_BAYER_CHN_NUM];
} ISP_AWB_INFO_S;

/* the statistics's attr of awb alg */


typedef struct tsISP_AWB_RAW_STAT_ATTR_S
{
    TS_BOOL bStatCfgUpdate;

    TS_U16  u16MeteringWhiteLevelAwb;
    TS_U16  u16MeteringBlackLevelAwb;
    TS_U16  u16MeteringCrRefMaxAwb;
    TS_U16  u16MeteringCbRefMaxAwb;
    TS_U16  u16MeteringCrRefMinAwb;
    TS_U16  u16MeteringCbRefMinAwb;
} ISP_AWB_RAW_STAT_ATTR_S;

/* the final calculate of awb alg */
typedef struct tsISP_AWB_RESULT_S
{
    TS_U32  au32WhiteBalanceGain[ISP_BAYER_CHN_NUM];
    TS_U16  au16ColorMatrix[CCM_MATRIX_SIZE];
    TS_U32  u32ColorTemp;
    TS_U8   u8Saturation;
    ISP_AWB_RAW_STAT_ATTR_S stRawStatAttr;
} ISP_AWB_RESULT_S;

typedef struct tsISP_AWB_EXP_FUNC_S
{
    TS_S32 (*pfn_awb_init)(TS_S32 s32Handle, const ISP_AWB_PARAM_S *pstAwbParam);
    TS_S32 (*pfn_awb_run)(TS_S32 s32Handle,
                          const ISP_AWB_INFO_S *pstAwbInfo,
                          ISP_AWB_RESULT_S *pstAwbResult,
                          TS_S32 s32Rsv
                         );
    TS_S32 (*pfn_awb_ctrl)(TS_S32 s32Handle, TS_U32 u32Cmd, TS_VOID *pValue);
    TS_S32 (*pfn_awb_exit)(TS_S32 s32Handle);
} ISP_AWB_EXP_FUNC_S;

typedef struct tsISP_AWB_REGISTER_S
{
    ISP_AWB_EXP_FUNC_S stAwbExpFunc;
} ISP_AWB_REGISTER_S;

/********************************  AF  *************************************/

/* the statistics of af alg */
typedef struct tsISP_AF_ZONE_S
{
    TS_U16  u16v1;
    TS_U16  u16h1;
    TS_U16  u16v2;
    TS_U16  u16h2;
    TS_U16  u16y;
    TS_U16  u16HlCnt;
} ISP_AF_ZONE_S;

#define AF_ZONE_ROW (15)
#define AF_ZONE_COLUMN (17)


typedef struct tsISP_FE_AF_STAT_S
{
    ISP_AF_ZONE_S stZoneMetrics[WDR_CHN_MAX][AF_ZONE_ROW][AF_ZONE_COLUMN]; /*R; The zoned measure of contrast*/
} ISP_FE_AF_STAT_S;

typedef struct tsISP_BE_AF_STAT_S
{
    ISP_AF_ZONE_S stZoneMetrics[AF_ZONE_ROW][AF_ZONE_COLUMN]; /*R; The zoned measure of contrast*/
} ISP_BE_AF_STAT_S;

#define _ALG_LIB_NAME_SIZE_MAX   (20)
typedef struct tsALG_LIB_S
{
    TS_S32  s32Id;
    TS_CHAR acLibName[_ALG_LIB_NAME_SIZE_MAX];
} ALG_LIB_S;

typedef struct tsISP_SNS_ATTR_INFO_S
{
    SENSOR_ID eSensorId;
} ISP_SNS_ATTR_INFO_S;

typedef enum tsISP_IRIS_F_NO_E
{
    ISP_IRIS_F_NO_32_0 = 0,
    ISP_IRIS_F_NO_22_0,
    ISP_IRIS_F_NO_16_0,
    ISP_IRIS_F_NO_11_0,
    ISP_IRIS_F_NO_8_0,
    ISP_IRIS_F_NO_5_6,
    ISP_IRIS_F_NO_4_0,
    ISP_IRIS_F_NO_2_8,
    ISP_IRIS_F_NO_2_0,
    ISP_IRIS_F_NO_1_4,
    ISP_IRIS_F_NO_1_0, 
    ISP_IRIS_F_NO_BUTT,
} ISP_IRIS_F_NO_E;

#define HIST_THRESH_NUM (4)
#define EXP_RATIO_NUM (3)

typedef struct tsAE_SENSOR_DEFAULT_S
{
    TS_U8 au8HistThresh[HIST_THRESH_NUM];
    TS_U8 u8AeCompensation;
    TS_U32 u32LinesPer500ms;
    TS_U32 u32FlickerFreq;
    TS_FLOAT f32Fps;
    TS_U32 u32HmaxTimes;
    TS_U32 u32InitExposure;
    TS_U32 u32InitAESpeed;
    TS_U32 u32InitAETolerance;
    TS_U32 u32FullLinesStd;
    TS_U32 u32FullLinesMax;
    TS_U32 u32FullLines; 
    TS_U32 u32MaxIntTime; 
    TS_U32 u32MinIntTime;
    TS_U32 u32MaxIntTimeTarget;
    TS_U32 u32MinIntTimeTarget;
    TS_U32 u32MaxAgain;
    TS_U32 u32MinAgain;
    TS_U32 u32MaxAgainTarget;
    TS_U32 u32MinAgainTarget;
    TS_U32 u32MaxDgain;
    TS_U32 u32MinDgain;
    TS_U32 u32MaxDgainTarget;
    TS_U32 u32MinDgainTarget;
    TS_U32 u32MaxISPDgainTarget;
    TS_U32 u32MinISPDgainTarget;
    TS_U32 u32ISPDgainShift;
    TS_U32 u32MaxIntTimeStep;
    TS_U32 u32LFMaxShortTime;
    TS_U32 u32LFMinExposure;
    TS_BOOL bAERouteExValid;
    TS_U16 u16ManRatioEnable;
    TS_U32 au32Ratio[EXP_RATIO_NUM];
    ISP_IRIS_F_NO_E enMaxIrisFNO;
    ISP_IRIS_F_NO_E enMinIrisFNO;
    ISP_AE_STRATEGY_E enAeExpMode;
    TS_U16 u16ISOCalCoef;
    TS_U8 u8AERunInterval;
    TS_U32 u32ExpRatioMax;
    TS_U32 u32ExpRatioMin;
} AE_SENSOR_DEFAULT_S;


typedef struct hiAE_SENSOR_EXP_FUNC_S
{
    TS_S32(*pfn_cmos_get_ae_default)(VI_PIPE ViPipe, AE_SENSOR_DEFAULT_S*pstAeSnsDft);
    TS_VOID(*pfn_cmos_fps_set)(VI_PIPE ViPipe, TS_FLOAT f32Fps,AE_SENSOR_DEFAULT_S *pstAeSnsDft);
    TS_VOID(*pfn_cmos_slow_framerate_set)(VI_PIPE ViPipe, TS_U32 u32FullLines, AE_SENSOR_DEFAULT_S *pstAeSnsDft);
    TS_VOID(*pfn_cmos_inttime_update)(VI_PIPE ViPipe, TS_U32 u32IntTime);
    TS_VOID(*pfn_cmos_gains_update)(VI_PIPE ViPipe, TS_U32 u32Again, TS_U32 u32Dgain);
    TS_VOID (*pfn_cmos_again_calc_table)(VI_PIPE ViPipe, TS_U32 *pu32AgainLin, TS_U32 *pu32AgainDb);
    TS_VOID (*pfn_cmos_dgain_calc_table)(VI_PIPE ViPipe, TS_U32 *pu32DgainLin, TS_U32 *pu32DgainDb);
    TS_VOID (*pfn_cmos_get_inttime_max)(VI_PIPE ViPipe, TS_U16 u16ManRatioEnable, TS_U32 *au32Ratio, 
        TS_U32 *au32IntTimeMax, TS_U32 *au32IntTimeMin, TS_U32 *pu32LFMaxIntTime); 
} AE_SENSOR_EXP_FUNC_S;


typedef struct tsAE_SENSOR_REGISTER_S
{
 AE_SENSOR_EXP_FUNC_S stSnsExp;
} AE_SENSOR_REGISTER_S;


typedef struct tsISP_CMOS_SENSOR_IMAGE_MODE_S
{
    TS_U16 u16Width;
    TS_U16 u16Height;
    TS_FLOAT f32Fps;
    TS_U8 u8SnsMode;
}ISP_CMOS_SENSOR_IMAGE_MODE_S;

typedef struct tsISP_CMOS_BLACK_LEVEL_S
{
    TS_BOOL bUpdate; 
    TS_U16 au16BlackLevel[ISP_BAYER_CHN_NUM];
} ISP_CMOS_BLACK_LEVEL_S;


typedef struct hiISP_SENSOR_EXP_FUNC_S
{
    TS_VOID(*pfn_cmos_sensor_init)(VI_PIPE ViPipe) ;
    TS_VOID(*pfn_cmos_sensor_global_init)(VI_PIPE ViPipe);
    TS_VOID(*pfn_cmos_sensor_exit)(VI_PIPE ViPipe);
    TS_S32(*pfn_cmos_set_image_mode)(VI_PIPE ViPipe, ISP_CMOS_SENSOR_IMAGE_MODE_S *pstSensorImageMode);
    TS_S32(*pfn_cmos_set_wdr_mode)(VI_PIPE ViPipe,TS_U8 u8Mode);
    TS_U32(*pfn_cmos_get_isp_default)(VI_PIPE ViPipe,TS_VOID *pstDef);
    TS_U32(*pfn_cmos_get_isp_black_level)(VI_PIPE ViPipe, ISP_CMOS_BLACK_LEVEL_S *pstBlackLevel);
    TS_VOID(*pfn_cmos_set_pixel_detect)(VI_PIPE ViPipe,TS_BOOL bEnable);
} ISP_SENSOR_EXP_FUNC_S;

typedef struct tsISP_SENSOR_REGISTER_S
{
 ISP_SENSOR_EXP_FUNC_S stSnsExp;
} ISP_SENSOR_REGISTER_S;

#define ISP_AUTO_ISO_STRENGTH_NUM (16)

typedef struct tsAWB_AGC_TABLE_S
{
    TS_BOOL bValid;
    TS_U8 au8Saturation[ISP_AUTO_ISO_STRENGTH_NUM]; 
} AWB_AGC_TABLE_S;

typedef struct tsAWB_CCM_TAB_S
{
    TS_U16 u16ColorTemp;
    TS_U16 au16CCM[CCM_MATRIX_SIZE];
} AWB_CCM_TAB_S;


typedef struct tsAWB_CCM_S
{ 
    TS_U16 u16CCMTabNum;
    AWB_CCM_TAB_S astCCMTab[CCM_MATRIX_NUM];
}AWB_CCM_S;


typedef struct tsAWB_SENSOR_DEFAULT_S
{
    TS_U16 u16WbRefTemp; 
    TS_U16 au16GainOffset[4]; 
    TS_S32 as32WbPara[6]; 
    TS_U16 u16GoldenRgain; 
    TS_U16 u16GoldenBgain; 
    TS_U16 u16SampleRgain; 
    TS_U16 u16SampleBgain; 
    AWB_AGC_TABLE_S stAgcTbl;
    AWB_CCM_S stCcm;
    TS_U16 u16InitRgain; 
    TS_U16 u16InitGgain;
    TS_U16 u16InitBgain;
    TS_U8 u8AWBRunInterval;
} AWB_SENSOR_DEFAULT_S;

typedef struct tsISP_SPECKCWB_S
{
    TS_S32 s32RGain;
    TS_S32 s32BGain;
} ISP_SPECKCWB_S;

typedef struct tsISP_SPECAWB_FACTTBL_ELEMENT_S {
    TS_S16 s16Bv;
    TS_U8 u8FactTbl[64][64];
} ISP_SPECAWB_FACTTBL_ELEMENT_S;

typedef struct tsISP_SPECAWB_KELVIN_DBB_MAP_S{
    TS_S16 s16Kelvin;
    TS_S16 s16DBB;
} ISP_SPECAWB_KELVIN_DBB_MAP_S;

typedef struct tsISP_SPECKCWBS16_S
{
    TS_S16 s16RGain;
    TS_S16 s16BGain;
} ISP_SPECKCWBS16_S;

typedef struct tsISP_SPECAWB_BBL_TBL_S{
    TS_S16 s16Kelvin;
    TS_S16 s16Wbr;
    TS_S16 s16Wbb;
} ISP_SPECAWB_BBL_TBL_S;


typedef struct hiISP_SPECAWB_ATTR_S 
{
    ISP_SPECKCWB_S stWBCenter;
    ISP_SPECKCWB_S stWBMin; 
    ISP_SPECKCWB_S stWBMax; 
    ISP_SPECKCWB_S stLogFact; 
    TS_S32 s32LimitFactMin;
    TS_S32 s32LimitFactMax;
    TS_S32 s32LimitFactLimit;
    TS_S32 s32FactOffset;
    TS_S32 s32BFact;
    TS_S32 s32ACntMin;
    ISP_SPECKCWB_S stWBNeutral;
    TS_U8 bLastIsFlash;
    TS_S16 s16BvFlashELimit;
    TS_S32 s32APercent; 
    ISP_SPECKCWBS16_S stWBCnvTbl[64][64];
    TS_U8 u8ElementNum; 
    ISP_SPECAWB_FACTTBL_ELEMENT_S stFactElement[7]; 
    ISP_SPECAWB_KELVIN_DBB_MAP_S stKelvinDBBTbl[64][64]; 
    ISP_SPECAWB_BBL_TBL_S stBlackBodyTbl[40];
    TS_U16 u16Fno;
}ISP_SPECAWB_ATTR_S;

typedef struct tsISP_SPECAWB_CAA_CONVERSION_S {
    int s32SrcKelvin;
    int s32DstKelvin;
} ISP_SPECAWB_CAA_CONVERSION_S;

#define SPECAWB_KEVIN_CONVER_MAX_NUM 8
typedef struct hiISP_SPECAWB_CAA_TBL_S
{
TS_U8 u8Enable; 
TS_S16 s16Bv; 
 ISP_SPECAWB_CAA_CONVERSION_S stKelvinCon[SPECAWB_KEVIN_CONVER_MAX_NUM];
 ISP_SPECKCWBS16_S stWBCnvTbl[64][64];
} ISP_SPECAWB_CAA_TBL_S;


typedef struct tsISP_SPECAWB_CAA_CONTROl_S {
    ISP_SPECAWB_CAA_TBL_S stControl[3];
} ISP_SPECAWB_CAA_CONTROl_S;


typedef struct tsAWB_SPEC_SENSOR_DEFAULT_S
{
    ISP_SPECAWB_ATTR_S stSpecAwbAttrs;
    ISP_SPECAWB_CAA_CONTROl_S stCaaControl;
} AWB_SPEC_SENSOR_DEFAULT_S;


typedef struct tsAWB_SENSOR_EXP_FUNC_S
{
    TS_S32(*pfn_cmos_get_awb_default)(VI_PIPE ViPipe, AWB_SENSOR_DEFAULT_S *pstAwbSnsDft);
    TS_S32(*pfn_cmos_get_awb_spec_default)(VI_PIPE ViPipe, AWB_SPEC_SENSOR_DEFAULT_S *pstAwbSpecSnsDft);
} AWB_SENSOR_EXP_FUNC_S;


typedef struct tsAWB_SENSOR_REGISTER_S
{
    AWB_SENSOR_EXP_FUNC_S stSnsExp;
} AWB_SENSOR_REGISTER_S;



typedef struct tsISP_BIND_ATTR_S
{
    SENSOR_ID   SensorId;
    ALG_LIB_S   stAeLib;
    ALG_LIB_S   stAfLib;
    ALG_LIB_S   stAwbLib;
} ISP_BIND_ATTR_S;


typedef struct tsISP_AWB_MLCZ_S
{
	TS_U8 awb_mlcz_index;
	TS_BOOL b_awb_mlcz_enable;
	TS_U8 awb_mlcz_detect_zone_index;
	TS_U16 awb_mlcz_lux_start;
	TS_U16 awb_mlcz_lux_end;
	TS_U16 awb_mlcz_lux_buf;
	TS_U16 awb_mlcz_hit_prc_thr;
	TS_U16 awb_mlcz_hit_prc_buf;
	TS_U16 awb_mlcz_left;
	TS_U16 awb_mlcz_right;
	TS_U16 awb_mlcz_top;
	TS_U16 awb_mlcz_bot;

} ISP_AWB_MLCZ_S;

typedef struct tsISP_AWB_PREFCOLOR_S
{
	TS_U8 awb_pre_color_index;
	TS_BOOL b_awb_pre_color_enable;
	TS_U8 awb_pre_color_ref_light_idx;
	TS_U16 awb_pre_color_lux_start;
	TS_U16 awb_pre_color_lux_end;
	TS_U16 awb_pre_color_lux_buf;
	TS_U16 awb_pre_color_offset_thrX;
	TS_U16 awb_pre_color_offset_thrY;
	TS_U16 awb_pre_color_clip_ratioX;
	TS_U16 awb_pre_color_clip_ratioY;
	TS_U16 awb_pre_color_shift_gainR;
	TS_U16 awb_pre_color_shift_gainB;

} ISP_AWB_PREFCOLOR_S;

typedef struct tsISP_AWB_EXTRACOLOR_S
{
	TS_U8 awb_extra_color_index;
	TS_BOOL b_awb_extra_color_enable;
	TS_U16 awb_extra_color_lux_start;
	TS_U16 awb_extra_color_lux_end;
	TS_U16 awb_extra_color_lux_buf;
	TS_U16 awb_extra_color_detect_prc_thr;
	TS_U16 awb_extra_color_detect_prc_buf;
	TS_U16 awb_extra_color_left;
	TS_U16 awb_extra_color_right;
	TS_U16 awb_extra_color_top;
	TS_U16 awb_extra_color_bot;
	TS_U16 awb_extra_color_light_idx;
	TS_U16 awb_extra_color_weight;
	TS_U16 awb_extra_color_pref_gainR;
	TS_U16 awb_extra_color_pref_gainB;

} ISP_AWB_EXTRACOLOR_S;

typedef struct tsISP_AWB_SPECIALGAIN_S
{
	TS_U16 awb_special_gainHR;
	TS_U16 awb_special_gainHB;
	TS_U16 awb_special_gainAR;
	TS_U16 awb_special_gainAB;
	TS_U16 awb_special_gain_tl84R;
	TS_U16 awb_special_gain_tl84B;
	TS_U16 awb_special_gain_cwfR;
	TS_U16 awb_special_gain_cwfB;
	TS_U16 awb_special_gain_d65R;
	TS_U16 awb_special_gain_d65B;
	TS_U16 awb_special_gain_dfR;
	TS_U16 awb_special_gain_dfB;
	TS_U16 awb_special_gain_shadeR;
	TS_U16 awb_special_gain_shadeB;

} ISP_AWB_SPECIALGAIN_S;

typedef struct tsISP_AWB_SPECIALGAINWEIGHT_S
{
	TS_U16 awb_special_gain_weightH;
	TS_U16 awb_special_gain_weightA;
	TS_U16 awb_special_gain_weight_tl84;
	TS_U16 awb_special_gain_weight_cwf;
	TS_U16 awb_special_gain_weight_d65;
	TS_U16 awb_special_gain_weight_df;
	TS_U16 awb_special_gain_weight_shade;

} ISP_AWB_SPECIALGAINWEIGHT_S;

typedef struct tsISP_AWB_NEW_S
{
	TS_U16 awb_dark_pixel_thr;
	TS_U16 awb_sat_pixel_thr;
	TS_U8 awb_dark_prc_thr;
	TS_U8 awb_sat_prc_thr;
	TS_BOOL b_awb_luma_weight_enable;
} ISP_AWB_NEW_S;

typedef enum tsISP_AE_STATS_SEL_E {
	ISP_AE_STATS_SEL_AE0 = 0,
	ISP_AE_STATS_SEL_AE1,
	ISP_AE_STATS_SEL_AE2,
	ISP_AE_STATS_SEL_AE3,
	ISP_AE_STATS_SEL_BUTT
} ISP_AE_STATS_SEL_E;

typedef struct tsISP_AE_FUNCTION_ATTR_S
{
	TS_BOOL b_ae_manualroi_enable;
	TS_U8 ae_manualroiX;
	TS_U8 ae_manualroiY;
	TS_U8 ae_manualroiW;
	TS_U8 ae_manualroiH;
	ISP_AE_STATS_SEL_E e_ae_statsel; /* RW; AE statistics information position selection. */
	TS_U8 ae_weighttblsel; /* RW; AE weight table selection. */
	TS_U8 ae_hlsup; /* RW; AE weight table selection. */
	TS_U8 ae_expmode; /* RW; AE framerate mode, 0:auto framerate, 1 constant framerate. */
} ISP_AE_FUNCTION_ATTR_S;

typedef struct tsISP_AWB_LIMITY_S
{
	TS_U8 awb_limit_Y_index;
	TS_BOOL b_awb_limit_Y_enable;
	TS_U8 awb_limit_Y_ref_light_idx;
	TS_U16 awb_limit_Y_lux_start;
	TS_U16 awb_limit_Y_lux_end;
	TS_U16 awb_limit_Y_lux_buf;
	TS_U16 awb_limit_Y_prc;
	TS_U16 awb_limit_Y_weight_reduce;
} ISP_AWB_LIMITY_S;

typedef struct tsISP_AWB_LIGHTSOURCE_S
{
	TS_U8 awb_light_source_index;
	TS_BOOL b_awb_light_source_enable;
	TS_U8 awb_light_source_ref_light_idx;
	TS_U16 awb_light_source_lux_start;
	TS_U16 awb_light_source_lux_end;
	TS_U16 awb_light_source_lux_buf;
	TS_U16 awb_light_source_scale_left;
	TS_U16 awb_light_source_scale_right;
	TS_U16 awb_light_source_scale_top;
	TS_U16 awb_light_source_scale_bot;
	TS_U16 awb_light_source_probH;
	TS_U16 awb_light_source_probA;
	TS_U16 awb_light_source_prob_tl84;
	TS_U16 awb_light_source_prob_cwf;
	TS_U16 awb_light_source_prob_d65;
	TS_U16 awb_light_source_prob_df;
	TS_U16 awb_light_source_prob_shade;
}ISP_AWB_LIGHTSOURCE_S;

typedef struct tsISP_AWB_FACE_S
{
	TS_BOOL b_face_awb_enable;
	TS_BOOL b_face_awb_enable_nr;
	TS_U8 face_awb_weight;
	TS_U8 face_awb_ref_target_tol;
	TS_U16 face_awb_face_scale;
	TS_U16 face_awb_face_size_ratioL;
	TS_U16 face_awb_face_size_ratioH;
	TS_U16 face_awb_dist_thrL;
	TS_U16 face_awb_dist_thrH;
	TS_U16 face_awb_pref_gainR;
	TS_U16 face_awb_pref_gainB;
} ISP_AWB_FACE_S;


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /*__TS_COMM_SNS_H__ */

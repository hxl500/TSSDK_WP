/*
 * Copyright © 2019 TSING MICRO.com.cn All Rights Reserved
 * Description: ts_comm_video.h
 * Author:TsingMicro multimedia software group
 * Create: 2022/06/02
 */

#ifndef __TS_COMM_VIDEO_H__
#define __TS_COMM_VIDEO_H__

#include "ts_type.h"
#include "ts_common.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define SRC_LENS_COEF_SEG       2
#define DST_LENS_COEF_SEG       3
#define SRC_LENS_COEF_NUM       4
#define DST_LENS_COEF_NUM       4
#define DST_LENS_COEF_SEG_POINT (DST_LENS_COEF_SEG - 1)

#define ISP_BAYER_CHN           (4)

typedef enum tsOPERATION_MODE_E {
    OPERATION_MODE_AUTO   = 0,
    OPERATION_MODE_MANUAL = 1,
    OPERATION_MODE_BUTT
} OPERATION_MODE_E;

/* Angle of rotation */
typedef enum tsROTATION_E {
    ROTATION_0   = 0,
    ROTATION_90  = 1,
    ROTATION_180 = 2,
    ROTATION_270 = 3,
    ROTATION_BUTT
} ROTATION_E;

typedef enum tsVB_SOURCE_E {
    VB_SOURCE_COMMON  = 0,
    VB_SOURCE_MODULE  = 1,
    VB_SOURCE_PRIVATE = 2,
    VB_SOURCE_USER    = 3,
    VB_SOURCE_BUTT
} VB_SOURCE_E;

typedef enum tsDATA_RATE_E {
    DATA_RATE_X1 = 0,         /* RW; output 1 pixel per clock */
    DATA_RATE_X2 = 1,         /* RW; output 2 pixel per clock */

    DATA_RATE_BUTT
} DATA_RATE_E;

typedef struct tsBORDER_S {
    TS_U32 u32TopWidth;
    TS_U32 u32BottomWidth;
    TS_U32 u32LeftWidth;
    TS_U32 u32RightWidth;
    TS_U32 u32Color;
} BORDER_S;

typedef struct tsPOINT_S {
    TS_S32 s32X;
    TS_S32 s32Y;
} POINT_S;

typedef struct tsSHORT_POINT_S {
    TS_S16 s16X;
    TS_S16 s16Y;
} SHORT_POINT_S;


typedef struct tsSIZE_S {
    TS_U32 u32Width;
    TS_U32 u32Height;
} SIZE_S;

typedef struct tsRECT_S {
    TS_S32 s32X;
    TS_S32 s32Y;
    TS_U32 u32Width;
    TS_U32 u32Height;
} RECT_S;

typedef struct tsVIDEO_REGION_INFO_S {
    TS_U32           u32RegionNum;       /* W; count of the region */
    RECT_S ATTRIBUTE* pstRegion;         /* W; region attribute */
} VIDEO_REGION_INFO_S;

typedef struct tsCROP_INFO_S {
    TS_BOOL bEnable;
    RECT_S  stRect;
} CROP_INFO_S;

typedef struct tsSCALE_INFO_S {
    TS_BOOL bEnable;
    TS_U32  u32Reserve;
    RECT_S  stRect;
} SCALE_INFO_S;

typedef struct tsFRAME_RATE_CTRL_S {
    TS_S32  s32SrcFrameRate;        /* RW; source frame rate */
    TS_S32  s32DstFrameRate;        /* RW; dest frame rate */
} FRAME_RATE_CTRL_S;

typedef enum tsASPECT_RATIO_E {
    ASPECT_RATIO_NONE   = 0,        /* full screen */
    ASPECT_RATIO_AUTO   = 1,        /* ratio no change, 1:1*/
    ASPECT_RATIO_MANUAL = 2,        /* ratio manual set */
    ASPECT_RATIO_BUTT

} ASPECT_RATIO_E;

typedef struct tsASPECT_RATIO_S {
    ASPECT_RATIO_E enMode;          /* aspect ratio mode: none/auto/manual */
    TS_U32         u32BgColor;      /* background color, RGB 888 */
    RECT_S         stVideoRect;     /* valid in ASPECT_RATIO_MANUAL mode */

} ASPECT_RATIO_S;

/* we ONLY define picture format used, all unused will be deleted! */
typedef enum tsPIXEL_FORMAT_E {
    PIXEL_FORMAT_RGB_444 = 0,
    PIXEL_FORMAT_RGB_555,
    PIXEL_FORMAT_RGB_565,
    PIXEL_FORMAT_RGB_888,

    PIXEL_FORMAT_BGR_444,
    PIXEL_FORMAT_BGR_555,
    PIXEL_FORMAT_BGR_565,
    PIXEL_FORMAT_BGR_888,

    PIXEL_FORMAT_ARGB_1555,
    PIXEL_FORMAT_ARGB_4444,
    PIXEL_FORMAT_ARGB_8565,
    PIXEL_FORMAT_ARGB_8888,
    PIXEL_FORMAT_ARGB_2BPP,
    PIXEL_FORMAT_ARGB_1BIT,
    PIXEL_FORMAT_ARGB_2BIT,

    PIXEL_FORMAT_ABGR_1555,
    PIXEL_FORMAT_ABGR_4444,
    PIXEL_FORMAT_ABGR_8565,
    PIXEL_FORMAT_ABGR_8888,

    PIXEL_FORMAT_BGRA_1555,
    PIXEL_FORMAT_BGRA_4444,//20
    PIXEL_FORMAT_BGRA_8565,
    PIXEL_FORMAT_BGRA_8888,
    PIXEL_FORMAT_RGBA_8888,

    PIXEL_FORMAT_RGB_BAYER_8BPP,
    PIXEL_FORMAT_RGB_BAYER_10BPP,
    PIXEL_FORMAT_RGB_BAYER_12BPP,
    PIXEL_FORMAT_RGB_BAYER_14BPP,
    PIXEL_FORMAT_RGB_BAYER_16BPP,

    PIXEL_FORMAT_YVU_PLANAR_422,
    PIXEL_FORMAT_YVU_PLANAR_420,//YV12 if not IMC
    PIXEL_FORMAT_YVU_PLANAR_444,

    PIXEL_FORMAT_YUV_PLANAR_422,
    PIXEL_FORMAT_YUV_PLANAR_420,//I420 if not IMC
    PIXEL_FORMAT_YUV_PLANAR_444,

    PIXEL_FORMAT_YVU_SEMIPLANAR_422,
    PIXEL_FORMAT_YVU_SEMIPLANAR_420,
    PIXEL_FORMAT_YVU_SEMIPLANAR_444,

    PIXEL_FORMAT_YUV_SEMIPLANAR_422,
    PIXEL_FORMAT_YUV_SEMIPLANAR_420,
    PIXEL_FORMAT_YUV_SEMIPLANAR_444,

    PIXEL_FORMAT_YUYV_PACKAGE_422,
    PIXEL_FORMAT_YVYU_PACKAGE_422,
    PIXEL_FORMAT_UYVY_PACKAGE_422,
    PIXEL_FORMAT_VYUY_PACKAGE_422,
    PIXEL_FORMAT_YYUV_PACKAGE_422,
    PIXEL_FORMAT_YYVU_PACKAGE_422,
    PIXEL_FORMAT_UVYY_PACKAGE_422,
    PIXEL_FORMAT_VUYY_PACKAGE_422,
    PIXEL_FORMAT_VY1UY0_PACKAGE_422,

    PIXEL_FORMAT_YUV_400,
    PIXEL_FORMAT_UV_420,

    PIXEL_FORMAT_NV_12,//equal to PIXEL_FORMAT_YUV_SEMIPLANAR_420
    PIXEL_FORMAT_NV_21,//equal to PIXEL_FORMAT_YVU_SEMIPLANAR_420
    PIXEL_FORMAT_RGB_24,//equal to PIXEL_FORMAT_RGB_888
    PIXEL_FORMAT_RAW_10,//?
    PIXEL_FORMAT_RAW_12,//?
    PIXEL_FORMAT_RAW_16,//?

    /* SVP data format */
    PIXEL_FORMAT_BGR_888_PLANAR,
    PIXEL_FORMAT_HSV_888_PACKAGE,
    PIXEL_FORMAT_HSV_888_PLANAR,
    PIXEL_FORMAT_LAB_888_PACKAGE,
    PIXEL_FORMAT_LAB_888_PLANAR,
    PIXEL_FORMAT_S8C1,
    PIXEL_FORMAT_S8C2_PACKAGE,
    PIXEL_FORMAT_S8C2_PLANAR,
    PIXEL_FORMAT_S8C3_PLANAR,
    PIXEL_FORMAT_S16C1,
    PIXEL_FORMAT_U8C1,
    PIXEL_FORMAT_U16C1,
    PIXEL_FORMAT_S32C1,
    PIXEL_FORMAT_U32C1,
    PIXEL_FORMAT_U64C1,
    PIXEL_FORMAT_S64C1,

    PIXEL_FORMAT_BUTT
} PIXEL_FORMAT_E;

typedef enum tsVIDEO_FIELD_E {
    VIDEO_FIELD_TOP         = 0x1,    /* even field */
    VIDEO_FIELD_BOTTOM      = 0x2,    /* odd field */
    VIDEO_FIELD_INTERLACED  = 0x3,    /* two interlaced fields */
    VIDEO_FIELD_FRAME       = 0x4,    /* frame */

    VIDEO_FIELD_BUTT
} VIDEO_FIELD_E;

typedef enum tsVIDEO_FORMAT_E {
    VIDEO_FORMAT_LINEAR = 0,       /* nature video line */
    VIDEO_FORMAT_TILE_64x16,       /* tile cell: 64pixel x 16line */
    VIDEO_FORMAT_TILE_16x8,        /* tile cell: 16pixel x 8line */
    VIDEO_FORMAT_LINEAR_DISCRETE,  /* The data bits are aligned in bytes */
    VIDEO_FORMAT_BUTT
} VIDEO_FORMAT_E;

typedef enum tsCOMPRESS_MODE_E {
    COMPRESS_MODE_NONE = 0,   /* no compress */
    COMPRESS_MODE_SEG,        /* compress unit is 256x1 bytes as a segment.*/
    COMPRESS_MODE_TILE,       /* compress unit is a tile.*/
    COMPRESS_MODE_LINE,       /* compress unit is the whole line.  raw for VI  */
    COMPRESS_MODE_FRAME,      /* compress unit is the whole frame. YUV for VI(3DNR), RGB for TDE(write)/VO(read) */

    COMPRESS_MODE_BUTT
} COMPRESS_MODE_E;

typedef enum tsVIDEO_DISPLAY_MODE_E {
    VIDEO_DISPLAY_MODE_PREVIEW  = 0x0,
    VIDEO_DISPLAY_MODE_PLAYBACK = 0x1,

    VIDEO_DISPLAY_MODE_BUTT
} VIDEO_DISPLAY_MODE_E;

typedef struct tsLUMA_INFO_S {
    TS_U64 u64LumaPixSum;      /* Luma sum of current frame */
    TS_U32 u32LumaPixAverage;  /* Luma average of current frame */
    TS_U64 u64PTS;             /* PTS of current frame  */
} LUMA_INFO_S;


typedef enum tsGPU_PROCESS_MODE_E {
    GPU_PROCESS_MODE_STREAM_STYLE  = 0x0,
    GPU_PROCESS_MODE_TASK_STYLE = 0x1,
    GPU_PROCESS_MODE_STYLE_BUTT
} GPU_PROCESS_MODE_E;

#define LDCI_STAT_WND_X         24
#define LDCI_STAT_WND_Y         16

#define DCF_DRSCRIPTION_LENGTH  32
#define DCF_CAPTURE_TIME_LENGTH 20

typedef struct tsISP_DCF_CONST_INFO_S {
    TS_U8       au8ImageDescription[DCF_DRSCRIPTION_LENGTH];        /*Describes image*/
    TS_U8       au8Make[DCF_DRSCRIPTION_LENGTH];                    /*Shows manufacturer of digital cameras*/
    TS_U8       au8Model[DCF_DRSCRIPTION_LENGTH];                   /*Shows model number of digital cameras*/
    TS_U8       au8Software[DCF_DRSCRIPTION_LENGTH];                /*Shows firmware (internal software of digital cameras) version number*/

    TS_U8       u8LightSource;                                      /*Light source, actually this means white balance setting. '0' means unknown, '1' daylight, '2'
                                                                               fluorescent, '3' tungsten, '10' flash, '17' standard light A, '18' standard light B, '19' standard light
                                                                               C, '20' D55, '21' D65, '22' D75, '255' other*/
    TS_U32      u32FocalLength;                                     /*Focal length of lens used to take image. Unit is millimeter*/
    TS_U8       u8SceneType;                                        /*Indicates the type of scene. Value '0x01' means that the image was directly photographed.*/
    TS_U8       u8CustomRendered;                                   /*Indicates the use of special processing on image data, such as rendering geared to output.
                                                                               0 = Normal process  1 = Custom process   */
    TS_U8       u8FocalLengthIn35mmFilm;                            /*Indicates the equivalent focal length assuming a 35mm film camera, in mm*/
    TS_U8       u8SceneCaptureType;                                 /*Indicates the type of scene that was shot. 0 = Standard,1 = Landscape,2 = Portrait,3 = Night scene. */
    TS_U8       u8GainControl;                                      /*Indicates the degree of overall image gain adjustment. 0 = None,1 = Low gain up,2 = High gain up,3 = Low gain down,4 = High gain down. */
    TS_U8       u8Contrast;                                         /*Indicates the direction of contrast processing applied by the camera when the image was shot.
                                                                               0 = Normal,1 = Soft,2 = Hard */
    TS_U8       u8Saturation;                                       /*Indicates the direction of saturation processing applied by the camera when the image was shot.
                                                                              0 = Normal,1 = Low saturation,2 = High saturation*/
    TS_U8       u8Sharpness;                                        /*Indicates the direction of sharpness processing applied by the camera when the image was shot.
                                                                              0 = Normal,1 = Soft,2 = Hard .*/
    TS_U8       u8MeteringMode;                                     /*Exposure metering method. '0' means unknown, '1' average, '2' center weighted average, '3'
                                                                              spot, '4' multi-spot, '5' multi-segment, '6' partial, '255' other*/
} ISP_DCF_CONST_INFO_S;

typedef struct tsISP_DCF_UPDATE_INFO_S {
    TS_U32      u32ISOSpeedRatings;                                 /*CCD sensitivity equivalent to Ag-Hr film speedrate*/
    TS_U32      u32ExposureTime;                                    /*Exposure time (reciprocal of shutter speed).*/
    TS_U32      u32ExposureBiasValue;                               /*Exposure bias (compensation) value of taking picture*/
    TS_U8       u8ExposureProgram;                                  /*Exposure program that the camera used when image was taken. '1' means manual control, '2'
                                                                              program normal, '3' aperture priority, '4' shutter priority, '5' program creative (slow program),
                                                                              '6' program action(high-speed program), '7' portrait mode, '8' landscape mode*/
    TS_U32      u32FNumber;                                         /*The actual F-number (F-stop) of lens when the image was taken*/
    TS_U32      u32MaxApertureValue;                                /*Maximum aperture value of lens.*/
    TS_U8       u8ExposureMode;                                     /*Indicates the exposure mode set when the image was shot.
                                                                              0 = Auto exposure,1 = Manual exposure, 2 = Auto bracket*/
    TS_U8       u8WhiteBalance;                                     /* Indicates the white balance mode set when the image was shot.
                                                                                0 = Auto white balance ,1 = Manual white balance */
} ISP_DCF_UPDATE_INFO_S;

typedef struct tsISP_DCF_INFO_S {
    ISP_DCF_CONST_INFO_S  stIspDCFConstInfo;
    ISP_DCF_UPDATE_INFO_S stIspDCFUpdateInfo;
} ISP_DCF_INFO_S;

typedef struct tsJPEG_DCF_S {
    TS_U8           au8CaptureTime[DCF_CAPTURE_TIME_LENGTH];            /*The date and time when the picture data was generated*/
    TS_BOOL         bFlash;                                             /*whether the picture is captured when a flash lamp is on*/
    TS_U32          u32DigitalZoomRatio;                                /*Indicates the digital zoom ratio when the image was shot.
                                                                                   If the numerator of the recorded value is 0, this indicates that digital zoom was not used.*/
    ISP_DCF_INFO_S  stIspDCFInfo;

} JPEG_DCF_S;

typedef struct tsISP_FRAME_INFO_S {
    TS_U32      u32ISO;                    /* ISP internal ISO : Again*Dgain*ISPgain */
    TS_U32      u32ExposureTime;           /* Exposure time (reciprocal of shutter speed),unit is us */
    TS_U32      u32IspDgain;
    TS_U32      u32Again;
    TS_U32      u32Dgain;
    TS_U32      au32Ratio[3];
    TS_U32      u32IspNrStrength;
    TS_U32      u32FNumber;                /* The actual F-number (F-stop) of lens when the image was taken */
    TS_U32      u32SensorID;               /* which sensor is used */
    TS_U32      u32SensorMode;
    TS_U32      u32HmaxTimes;              /* Sensor HmaxTimes,unit is ns */
    TS_U32      u32Vmax;                   /* Sensor Vmax,unit is line */
    TS_U32      u32VcNum;                  /* when dump wdr frame, which is long or short  exposure frame. */
} ISP_FRAME_INFO_S;

#define CFACOLORPLANE     (3)
#define DNG_NP_SIZE       (6)

typedef struct tsDNG_RATIONAL_S {
    TS_U32 u32Numerator;/*represents the numerator of a fraction,*/
    TS_U32 u32Denominator;/* the denominator. */
} DNG_RATIONAL_S;

/*
Defines the structure of DNG image dynamic infomation
*/
typedef struct tsDNG_IMAGE_DYNAMIC_INFO_S {
    TS_U32 au32BlackLevel[ISP_BAYER_CHN];           /* RO;Range: [0x0, 0xFFFF]; black level*/
    DNG_RATIONAL_S astAsShotNeutral[CFACOLORPLANE]; /* specifies the selected white balance at time of capture, encoded as the coordinates of a perfectly neutral color in linear reference space values.*/
    TS_DOUBLE adNoiseProfile[DNG_NP_SIZE];          /* RO;describes the amount of noise in a raw image*/
} DNG_IMAGE_DYNAMIC_INFO_S;

typedef struct tsISP_HDR_INFO_S {
    TS_U32 u32ColorTemp;
    TS_U16 au16CCM[9];
    TS_U8  u8Saturation;
} ISP_HDR_INFO_S;

typedef struct tsISP_ATTACH_INFO_S {
    ISP_HDR_INFO_S     stIspHdr;
    TS_U32 u32ISO;
    TS_U8 u8SnsWDRMode;
} ISP_ATTACH_INFO_S;

typedef enum tsFRAME_FLAG_E {
    FRAME_FLAG_SNAP_FLASH  = 0x1 << 0,
    FRAME_FLAG_SNAP_CUR      = 0x1 << 1,
    FRAME_FLAG_SNAP_REF       = 0x1 << 2,
    FRAME_FLAG_SNAP_END      = 0x1 << 31,
    FRAME_FLAG_BUTT
} FRAME_FLAG_E;

/* RGGB=4 */
#define ISP_WB_GAIN_NUM         4
/* 3*3=9 matrix */
#define ISP_CAP_CCM_NUM         9
typedef struct tsISP_CONFIG_INFO_S {
    TS_U32 u32ISO;
    TS_U32 u32IspDgain;
    TS_U32 u32ExposureTime;
    TS_U32 au32WhiteBalanceGain[ISP_WB_GAIN_NUM];
    TS_U32 u32ColorTemperature;
    TS_U16 au16CapCCM[ISP_CAP_CCM_NUM];
} ISP_CONFIG_INFO_S;

typedef struct tsVIDEO_SUPPLEMENT_S {
    TS_U64   u64JpegDCFPhyAddr;
    TS_U64   u64IspInfoPhyAddr;
    TS_U64   u64LowDelayPhyAddr;
    TS_U64   u64MotionDataPhyAddr;
    TS_U64   u64FrameDNGPhyAddr;

    TS_VOID* ATTRIBUTE pJpegDCFVirAddr;        /* JPEG_DCF_S, used in JPEG DCF */
    TS_VOID* ATTRIBUTE pIspInfoVirAddr;        /* ISP_FRAME_INFO_S, used in ISP debug, when get raw and send raw */
    TS_VOID* ATTRIBUTE pLowDelayVirAddr;       /* used in low delay */
    TS_VOID* ATTRIBUTE pMotionDataVirAddr;     /* vpss 3dnr use: gme motion data, Filter motion data, gyro data.*/
    TS_VOID* ATTRIBUTE pFrameDNGVirAddr;
} VIDEO_SUPPLEMENT_S;

typedef enum tsCOLOR_GAMUT_E {
    COLOR_GAMUT_BT601 = 0,
    COLOR_GAMUT_BT709,
    COLOR_GAMUT_BT2020,
    COLOR_GAMUT_USER,
    COLOR_GAMUT_BUTT
} COLOR_GAMUT_E;

typedef struct tsISP_COLORGAMMUT_INFO_S {
        COLOR_GAMUT_E enColorGamut;
} ISP_COLORGAMMUT_INFO_S;

typedef enum tsDYNAMIC_RANGE_E {
    DYNAMIC_RANGE_SDR8 = 0,
	#if 1
    DYNAMIC_RANGE_SDR10,
    DYNAMIC_RANGE_HDR10,
    DYNAMIC_RANGE_HLG,
    DYNAMIC_RANGE_SLF,
    DYNAMIC_RANGE_XDR,
    #endif
    DYNAMIC_RANGE_BUTT
} DYNAMIC_RANGE_E;

typedef enum tsDATA_BITWIDTH_E {
    DATA_BITWIDTH_8 = 0,
    DATA_BITWIDTH_10,
    DATA_BITWIDTH_12,
    DATA_BITWIDTH_14,
    DATA_BITWIDTH_16,
    DATA_BITWIDTH_BUTT
} DATA_BITWIDTH_E;

typedef struct tsVIDEO_FRAME_S {
    TS_U32              u32Width;
    TS_U32              u32Height;
    VIDEO_FIELD_E       enField;
    PIXEL_FORMAT_E      enPixelFormat;
    VIDEO_FORMAT_E      enVideoFormat;
    COMPRESS_MODE_E     enCompressMode;
    DYNAMIC_RANGE_E     enDynamicRange;
    COLOR_GAMUT_E       enColorGamut;
    TS_U32              u32HeaderStride[3];
    TS_U32              u32Stride[3];
    TS_U32              u32ExtStride[3];

    TS_U64              u64HeaderPhyAddr[3];
    TS_U64              u64HeaderVirAddr[3];
    TS_U64              u64PhyAddr[3];
    TS_U64              u64VirAddr[3];
    TS_U64              u64ExtPhyAddr[3];
    TS_U64              u64ExtVirAddr[3];

    TS_S16              s16OffsetTop;        /* top offset of show area */
    TS_S16              s16OffsetBottom;    /* bottom offset of show area */
    TS_S16              s16OffsetLeft;        /* left offset of show area */
    TS_S16              s16OffsetRight;        /* right offset of show area */

    TS_U32              u32MaxLuminance;
    TS_U32              u32MinLuminance;

    TS_U32              u32TimeRef;
    TS_U64              u64PTS;

    TS_U64              u64PrivateData;
    TS_U32              u32FrameFlag;     /* FRAME_FLAG_E, can be OR operation. */
    VIDEO_SUPPLEMENT_S  stSupplement;
	int32_t           fd;
	ts_s32            size;
	TS_UINTPTR_T flow_frame_addr; // for vpu loop
#ifdef ENABLE_MPP_TRACE
	uint64_t sys_run_time;
	TS_U8 node_info[6];
#endif
} VIDEO_FRAME_S;

typedef struct tsVIDEO_FRAME_INFO_S {
    VIDEO_FRAME_S stVFrame;
    TS_U32        u32PoolId;
    MOD_ID_E      enModId;
} VIDEO_FRAME_INFO_S;

typedef struct tsVB_CAL_CONFIG_S {
    TS_U32 u32VBSize;

    TS_U32 u32HeadStride;
    TS_U32 u32HeadSize;
    TS_U32 u32HeadYSize;

    TS_U32 u32MainStride;
    TS_U32 u32MainSize;
    TS_U32 u32MainYSize;

    TS_U32 u32ExtStride;
    TS_U32 u32ExtYSize;
} VB_CAL_CONFIG_S;

typedef struct tsBITMAP_S {
    PIXEL_FORMAT_E enPixelFormat;  /* Bitmap's pixel format */
    TS_U32 u32Width;               /* Bitmap's width */
    TS_U32 u32Height;              /* Bitmap's height */
    TS_VOID* ATTRIBUTE pData;      /* Address of Bitmap's data */
} BITMAP_S;

typedef enum tsROTATION_VIEW_TYPE_E {
    ROTATION_VIEW_TYPE_ALL      = 0, /* View all source Image,no lose*/
    ROTATION_VIEW_TYPE_TYPICAL  = 1, /* View from rotation Image with source size,same lose*/
    ROTATION_VIEW_TYPE_INSIDE   = 2, /* View with no black section,all  in dest Image*/
    ROTATION_VIEW_TYPE_BUTT,
} ROTATION_VIEW_TYPE_E;

typedef struct tsROTATION_EX_S {
    ROTATION_VIEW_TYPE_E enViewType;       /*RW;Range: [0, 2];Rotation mode*/
    TS_U32               u32Angle;         /*RW;Range: [0,360];Rotation Angle:[0,360]*/
    TS_S32               s32CenterXOffset; /*RW;Range: [-511, 511];Horizontal offset of the image distortion center relative to image center.*/
    TS_S32               s32CenterYOffset; /*RW;Range: [-511, 511];Vertical offset of the image distortion center relative to image center.*/
    SIZE_S               stDestSize;       /*RW;Dest size of any angle rotation*/
} ROTATION_EX_S;

typedef enum tsWDR_MODE_E {
    WDR_MODE_NONE = 0,
    WDR_MODE_BUILT_IN,
    WDR_MODE_QUDRA,

    WDR_MODE_2To1_LINE,
    WDR_MODE_2To1_FRAME,
    WDR_MODE_2To1_FRAME_FULL_RATE,

    WDR_MODE_3To1_LINE,
    WDR_MODE_3To1_FRAME,
    WDR_MODE_3To1_FRAME_FULL_RATE,

    WDR_MODE_4To1_LINE,
    WDR_MODE_4To1_FRAME,
    WDR_MODE_4To1_FRAME_FULL_RATE,

    WDR_MODE_BUTT,
} WDR_MODE_E;

typedef enum tsFRAME_INTERRUPT_TYPE_E {
    FRAME_INTERRUPT_START,
    FRAME_INTERRUPT_EARLY,
    FRAME_INTERRUPT_EARLY_END,
    FRAME_INTERRUPT_BUTT,
} FRAME_INTERRUPT_TYPE_E;

typedef struct tsFRAME_INTERRUPT_ATTR_S {
    FRAME_INTERRUPT_TYPE_E enIntType;
    TS_U32 u32EarlyLine;
} FRAME_INTERRUPT_ATTR_S;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* _TS_COMM_VIDEO_H_ */


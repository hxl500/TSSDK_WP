/*
 * Copyright © 2019 TSING MICRO.com.cn All Rights Reserved
 * Description: ts_comm_vpss.h
 * Author:TsingMicro multimedia software group
 * Create: 2022/06/02
 */

#ifndef __TS_COMM_VPSS_H__
#define __TS_COMM_VPSS_H__

#include "ts_type.h"
#include "ts_common.h"
#include "ts_errno.h"
#include "ts_comm_video.h"

#define VPSS_SHARPEN_GAIN_NUM        (32)
#define VPSS_AUTO_ISO_STRENGTH_NUM   (16)
#define VPSS_YUV_SHPLUMA_NUM         (32)
#define VPSS_3DNR_MAX_AUTO_PARAM_NUM (16)

#define TS_ERR_VPSS_INVALID_PARA     TS_DEF_ERR(TS_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_ILLEGAL_PARAM)
#define TS_ERR_VPSS_NULL_PTR         TS_DEF_ERR(TS_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_NULL_PTR)
#define TS_ERR_VPSS_NOTREADY         TS_DEF_ERR(TS_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_SYS_NOTREADY)
#define TS_ERR_VPSS_INVALID_WAYID    TS_DEF_ERR(TS_ID_VPSS, EN_ERR_LEVEL_ERROR, ERR_VI_INVALID_WAYID)
#define TS_ERR_VPSS_INVALID_DEVID    TS_DEF_ERR(TS_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_DEVID)
#define TS_ERR_VPSS_INVALID_CHNID    TS_DEF_ERR(TS_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_CHNID)
#define TS_ERR_VPSS_EXIST            TS_DEF_ERR(TS_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_EXIST)
#define TS_ERR_VPSS_UNEXIST          TS_DEF_ERR(TS_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_UNEXIST)
#define TS_ERR_VPSS_NOT_SUPPORT      TS_DEF_ERR(TS_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_SUPPORT)
#define TS_ERR_VPSS_NOT_PERM         TS_DEF_ERR(TS_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_PERM)
#define TS_ERR_VPSS_NOMEM            TS_DEF_ERR(TS_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_NOMEM)
#define TS_ERR_VPSS_NOBUF            TS_DEF_ERR(TS_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_NOBUF)
#define TS_ERR_VPSS_SIZE_NOT_ENOUGH  TS_DEF_ERR(TS_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_SIZE_NOT_ENOUGH)
#define TS_ERR_VPSS_ILLEGAL_PARAM    TS_DEF_ERR(TS_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_ILLEGAL_PARAM)
#define TS_ERR_VPSS_BUSY             TS_DEF_ERR(TS_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_BUSY)
#define TS_ERR_VPSS_BUF_EMPTY        TS_DEF_ERR(TS_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_EMPTY)
#define TS_ERR_VPSS_CHNOTDISABLE	  TS_DEF_ERR(TS_ID_VPSS, EN_ERR_LEVEL_ERROR, ERR_VI_FAILED_CHNOTDISABLE)

#define TS_ERR_VPSS_COMP_INIT        TS_DEF_ERR(TS_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_COMP_INIT)
#define TS_ERR_VPSS_COMP_GET         TS_DEF_ERR(TS_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_COMP_GET)
#define TS_ERR_VPSS_COMP_INTERNAL    TS_DEF_ERR(TS_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_COMP_INTERNAL)

#define VPSS_INVALID_FRMRATE         -1
#define VPSS_CHN0                    0
#define VPSS_CHN1                    1
#define VPSS_CHN2                    2
#define VPSS_CHN3                    3
#define VPSS_INVALID_CHN             -1

typedef enum tsVPSS_GRP_MODE_E {
    VPSS_GRP_MODE_STREAM  = 0,       /* stream mode. */
    VPSS_GRP_MODE_FRAME  = 1        /* frame mode. */

} VPSS_GRP_MODE_E;

typedef enum tsVPSS_CHN_MODE_E {
	VPSS_CHN_MODE_USER	= 0,	   /* User mode. */
	VPSS_CHN_MODE_AUTO	= 1 	   /* Auto mode. */

} VPSS_CHN_MODE_E;

typedef struct tsVPSS_GRP_ATTR_S {
	VPSS_GRP_MODE_E			   enGrpMode;
    /* RW; Range: ; Width of source image. */
    TS_U32                     u32MaxW;
    /* RW; Range: ; Height of source image. */
    TS_U32                     u32MaxH;
    PIXEL_FORMAT_E             enPixelFormat;     /* RW; Pixel format of source image. */
    DYNAMIC_RANGE_E            enDynamicRange;    /* RW; DynamicRange of source image. */
    FRAME_RATE_CTRL_S          stFrameRate;       /* Grp frame rate contrl. */
} VPSS_GRP_ATTR_S;

typedef struct{
	TS_U32 u32SensorId; 	// sensorId: 0, 1
	TS_U32 u32IsMain; 	// 是否是主码流，主码流使用loop。
	TS_U32 u32InstNext;	// 本实例编码完成后，下一个编码实例索引
}LOOP_ENC_CFG_S;

typedef enum tsVPSS_FAST_FLOW_MODE_E {
	FAST_FLOW_MODE_NORMAL = 0,
	FAST_FLOW_MODE_LOOP,
	FAST_FLOW_MODE_STITCH,
	FAST_FLOW_MODE_LOOP_STITCH,
    FAST_FLOW_MODE_LOOP1,
} VPSS_FAST_FLOW_MODE_E;

typedef struct tsFAST_FLOW_ATTR_S {
	VPSS_FAST_FLOW_MODE_E enFastFlow;
    TS_U32                u32Depth;
	TS_S8                 s8Index;
	TS_S8                 s8SyncChannel;
	TS_U32                u32MinTiming;
	TS_U32                u32MaxTiming;
    LOOP_ENC_CFG_S        stLoopEncCfg;
} FAST_FLOW_ATTR_S;

typedef struct tsVPSS_CHN_ATTR_S {
    VPSS_CHN_MODE_E     enChnMode;          /* RW; Vpss channel's work mode. */
	TS_U32				u32MaxW;
	TS_U32				u32MaxH;
	//TS_BOOL				bSmallMem;			/* RW; pingpong buffer enable */
	TS_U32				u32MemCount;			/* RW; assign buffer count */
/* RW; Range: ; Width of target image. */
    TS_U32              u32Width;
    /* RW; Range: ; Height of target image. */
    TS_U32              u32Height;
    VIDEO_FORMAT_E      enVideoFormat;      /* RW; Video format of target image. */
    PIXEL_FORMAT_E      enPixelFormat;      /* RW; Pixel format of target image. */
    DYNAMIC_RANGE_E     enDynamicRange;     /* RW; DynamicRange of target image. */
    COMPRESS_MODE_E     enCompressMode;     /* RW; Compression mode of the output. */
    FRAME_RATE_CTRL_S   stFrameRate;        /* Frame rate control info */
    TS_BOOL             bMirror;            /* RW; Mirror enable. */
    TS_BOOL             bFlip;              /* RW; Flip enable. */
    TS_U32              u32Depth;           /* RW; Range: [0, 8]; User get list depth. */
    ASPECT_RATIO_S      stAspectRatio;      /* Aspect Ratio info. */
    TS_BOOL             bVbFromOutside; /* alloc vb by user themselves*/

	/*add loop mode*/
	FAST_FLOW_ATTR_S    stFastFlow;

	TS_BOOL				bVpuLoop;

	TS_U32              u32Align;

} VPSS_CHN_ATTR_S;

typedef enum tsVPSS_CROP_COORDINATE_E {
    VPSS_CROP_RATIO_COOR = 0,   /* Ratio coordinate. */
    VPSS_CROP_ABS_COOR          /* Absolute coordinate. */
} VPSS_CROP_COORDINATE_E;

typedef struct tsVPSS_CROP_INFO_S {
    TS_BOOL                 bEnable;            /* RW; Range: [0, 1];  CROP enable. */
    VPSS_CROP_COORDINATE_E  enCropCoordinate;   /* RW;  Range: [0, 1]; Coordinate mode of the crop start point. */
    RECT_S                  stCropRect;         /* CROP rectangular. */
} VPSS_CROP_INFO_S;

typedef        struct{
        TS_U8        alpha;
        TS_U8        value_R;
        TS_U8        value_G;
        TS_U8        value_B;
}VPSS_OVERLAY_PIXEL_S;

typedef        struct{
        VPSS_OVERLAY_PIXEL_S        pixel_2bit[4];
		VPSS_OVERLAY_PIXEL_S        pixel_1bit[2];
}VPSS_OVERLAY_LUT_S;


#endif /* __TS_COMM_VPSS_H__ */



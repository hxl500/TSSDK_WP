/*
 * Copyright © 2019 TSING MICRO.com.cn All Rights Reserved
 * Description: ts_comm_vi.h
 * Author:TsingMicro multimedia software group
 * Create: 2022/06/02
 */

#ifndef __TS_COMM_VI_H__
#define __TS_COMM_VI_H__

#include "ts_common.h"
#include "ts_errno.h"
#include "ts_comm_video.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define VI_MAX_ADCHN_NUM              (4UL)

#define VI_PMFCOEF_NUM                (9UL)
#define VI_COMPMASK_NUM               (3UL)
#define VI_PRO_MAX_FRAME_NUM          (8UL)

#define VI_INVALID_FRMRATE            (-1)
#define VI_CHN0                       0
#define VI_CHN1                       1
#define VI_CHN2                       2
#define VI_CHN3                       3
#define VI_INVALID_CHN                (-1)

#define VI_MAX_VC_NUM                 4

typedef enum tsEN_VI_ERR_CODE_E {
    ERR_VI_FAILED_NOTENABLE = 64,       /* device or channel not enable */
    ERR_VI_FAILED_NOTDISABLE,           /* device not disable */
    ERR_VI_FAILED_CHNOTDISABLE,         /* channel not disable */
    ERR_VI_CFG_TIMEOUT,                 /* config timeout */
    ERR_VI_NORM_UNMATCH,                /* video norm of ADC and VIU is unmatch */
    ERR_VI_INVALID_WAYID,               /* invlalid way ID */
    ERR_VI_INVALID_PHYCHNID,            /* invalid phychn id */
    ERR_VI_FAILED_NOTBIND,              /* device or channel not bind */
    ERR_VI_FAILED_BINDED,               /* device or channel not unbind */
    ERR_VI_DIS_PROCESS_FAIL             /* dis process failed */
} EN_VI_ERR_CODE_E;

#define TS_ERR_VI_INVALID_PARA        TS_DEF_ERR(TS_ID_VI, EN_ERR_LEVEL_ERROR, EN_ERR_ILLEGAL_PARAM)
#define TS_ERR_VI_INVALID_DEVID       TS_DEF_ERR(TS_ID_VI, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_DEVID)
#define TS_ERR_VI_INVALID_PIPEID      TS_DEF_ERR(TS_ID_VI, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_PIPEID)
#define TS_ERR_VI_INVALID_STITCHGRPID TS_DEF_ERR(TS_ID_VI, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_STITCHGRPID)
#define TS_ERR_VI_INVALID_CHNID       TS_DEF_ERR(TS_ID_VI, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_CHNID)
#define TS_ERR_VI_INVALID_NULL_PTR    TS_DEF_ERR(TS_ID_VI, EN_ERR_LEVEL_ERROR, EN_ERR_NULL_PTR)
#define TS_ERR_VI_FAILED_NOTCONFIG    TS_DEF_ERR(TS_ID_VI, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_CONFIG)
#define TS_ERR_VI_SYS_NOTREADY        TS_DEF_ERR(TS_ID_VI, EN_ERR_LEVEL_ERROR, EN_ERR_SYS_NOTREADY)
#define TS_ERR_VI_BUF_EMPTY           TS_DEF_ERR(TS_ID_VI, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_EMPTY)
#define TS_ERR_VI_BUF_FULL            TS_DEF_ERR(TS_ID_VI, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_FULL)
#define TS_ERR_VI_NOMEM               TS_DEF_ERR(TS_ID_VI, EN_ERR_LEVEL_ERROR, EN_ERR_NOMEM)
#define TS_ERR_VI_NOT_SUPPORT         TS_DEF_ERR(TS_ID_VI, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_SUPPORT)
#define TS_ERR_VI_BUSY                TS_DEF_ERR(TS_ID_VI, EN_ERR_LEVEL_ERROR, EN_ERR_BUSY)
#define TS_ERR_VI_NOT_PERM            TS_DEF_ERR(TS_ID_VI, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_PERM)

#define TS_ERR_VI_FAILED_NOTENABLE    TS_DEF_ERR(TS_ID_VI, EN_ERR_LEVEL_ERROR, ERR_VI_FAILED_NOTENABLE)
#define TS_ERR_VI_FAILED_NOTDISABLE   TS_DEF_ERR(TS_ID_VI, EN_ERR_LEVEL_ERROR, ERR_VI_FAILED_NOTDISABLE)
#define TS_ERR_VI_FAILED_CHNOTDISABLE TS_DEF_ERR(TS_ID_VI, EN_ERR_LEVEL_ERROR, ERR_VI_FAILED_CHNOTDISABLE)
#define TS_ERR_VI_CFG_TIMEOUT         TS_DEF_ERR(TS_ID_VI, EN_ERR_LEVEL_ERROR, ERR_VI_CFG_TIMEOUT)
#define TS_ERR_VI_NORM_UNMATCH        TS_DEF_ERR(TS_ID_VI, EN_ERR_LEVEL_ERROR, ERR_VI_NORM_UNMATCH)
#define TS_ERR_VI_INVALID_WAYID       TS_DEF_ERR(TS_ID_VI, EN_ERR_LEVEL_ERROR, ERR_VI_INVALID_WAYID)
#define TS_ERR_VI_INVALID_PHYCHNID    TS_DEF_ERR(TS_ID_VI, EN_ERR_LEVEL_ERROR, ERR_VI_INVALID_PHYCHNID)
#define TS_ERR_VI_FAILED_NOTBIND      TS_DEF_ERR(TS_ID_VI, EN_ERR_LEVEL_ERROR, ERR_VI_FAILED_NOTBIND)
#define TS_ERR_VI_FAILED_BINDED       TS_DEF_ERR(TS_ID_VI, EN_ERR_LEVEL_ERROR, ERR_VI_FAILED_BINDED)

#define TS_ERR_VI_PIPE_EXIST          TS_DEF_ERR(TS_ID_VI, EN_ERR_LEVEL_ERROR, EN_ERR_EXIST)
#define TS_ERR_VI_PIPE_UNEXIST        TS_DEF_ERR(TS_ID_VI, EN_ERR_LEVEL_ERROR, EN_ERR_UNEXIST)

#define TS_ERR_VI_COMP_INIT            TS_DEF_ERR(TS_ID_VI, EN_ERR_LEVEL_ERROR, EN_ERR_COMP_INIT)
#define TS_ERR_VI_COMP_GET            TS_DEF_ERR(TS_ID_VI, EN_ERR_LEVEL_ERROR, EN_ERR_COMP_GET)
#define TS_ERR_VI_COMP_INTERNAL       TS_DEF_ERR(TS_ID_VI, EN_ERR_LEVEL_ERROR, EN_ERR_COMP_INTERNAL)


typedef enum tsVI_PIPE_BYPASS_MODE_E {
    VI_PIPE_BYPASS_NONE,
    VI_PIPE_BYPASS_FE,
    VI_PIPE_BYPASS_BE,

    VI_PIPE_BYPASS_BUTT
} VI_PIPE_BYPASS_MODE_E;

/* The attributes of pipe */
typedef struct tsVI_PIPE_ATTR_S {
    VI_PIPE_BYPASS_MODE_E enPipeBypassMode;
    TS_BOOL               bYuvSkip;               /* RW;YUV skip enable */
    TS_BOOL               bIspBypass;             /* RW;Range:[0, 1];ISP bypass enable */
    TS_U32                u32MaxW;                /* RW;Range[VI_PIPE_MIN_WIDTH, VI_PIPE_MAX_WIDTH];Maximum width */
    TS_U32                u32MaxH;                /* RW;Range[VI_PIPE_MIN_HEIGHT, VI_PIPE_MAX_HEIGHT];Maximum height */
    PIXEL_FORMAT_E        enPixFmt;               /* RW;Pixel format */
    COMPRESS_MODE_E       enCompressMode;         /* RW;Range:[0, 4];Compress mode. */
    DATA_BITWIDTH_E       enBitWidth;             /* RW;Range:[0, 4];Bit width */
    FRAME_RATE_CTRL_S     stFrameRate;            /* RW;Frame rate */
    TS_BOOL               bDiscardProPic;         /* RW;Range:[0, 1];when professional mode snap, whether to discard
                                                	long exposure picture in the video pipe. */
} VI_PIPE_ATTR_S;

/* The attributes of channel */
typedef struct tsVI_CHN_ATTR_S {
    SIZE_S              stSize;             /* RW;Channel out put size */
    PIXEL_FORMAT_E      enPixelFormat;      /* RW;Pixel format */
    DYNAMIC_RANGE_E     enDynamicRange;     /* RW;Dynamic Range */
    VIDEO_FORMAT_E      enVideoFormat;      /* RW;Video format */
    COMPRESS_MODE_E     enCompressMode;     /* RW;256B Segment compress or no compress. */
    TS_BOOL             bMirror;            /* RW;Mirror enable */
    TS_BOOL             bFlip;              /* RW;Flip enable */
    TS_U32              u32Depth;           /* RW;Range [0,8];Depth */
    FRAME_RATE_CTRL_S   stFrameRate;        /* RW;Frame rate */
} VI_CHN_ATTR_S;

typedef struct tsVI_AAA_INVALID_INFO_S {
    TS_U64 u64Timestamp;
    TS_U32 u32FrameNum;
    TS_U32 u32LastNum;
    TS_U32 u32Reserve[2];
} VI_AAA_INVALID_INFO_S;

/* The status of pipe */
typedef struct tsVI_PIPE_STATUS_S {
    TS_BOOL bEnable;                        /* RO;Whether this pipe is enabled */
    TS_U32  u32IntCnt;                      /* RO;The video frame interrupt count */
    TS_FLOAT  fFrameRate;                   /* RO;Current frame rate */
    TS_U32  u32LostFrame;                   /* RO;Lost frame count */
    TS_U32  u32VbFail;                      /* RO;Video buffer malloc failure */
    SIZE_S  stSize;                         /* RO;Current pipe output size */
    TS_U64  u64AaaErrCnt;
	VI_AAA_INVALID_INFO_S  stAaaInvalidInfo;
} VI_PIPE_STATUS_S;

/* The status of chn */
typedef struct tsVI_CHN_STATUS_S {
    TS_BOOL bEnable;                    /* RO;Whether this channel is enabled */
    TS_U32  u32FrameRate;               /* RO;current frame rate */
    TS_U32  u32LostFrame;               /* RO;Lost frame count */
    TS_U32  u32VbFail;                  /* RO;Video buffer malloc failure */
    SIZE_S  stSize;                     /* RO;chn output size */

} VI_CHN_STATUS_S;

typedef enum tsVI_DUMP_TYPE_E {
    VI_DUMP_TYPE_RAW = 0,
    VI_DUMP_TYPE_YUV = 1,
    //VI_DUMP_TYPE_IR = 2,
    VI_DUMP_TYPE_BUTT
} VI_DUMP_TYPE_E;

/*Defines the bitwidth of the input Bayer image, used for lsc online calibration*/
typedef enum tsVI_BAYER_RAWBIT_E
{
    VI_DUMP_MODE_SENSOR_RAW16    			= 0,
    VI_DUMP_MODE_SENSOR_RAW12    			= 1,
    VI_DUMP_MODE_COMPRESSED_RAW8    		= 2,
    VI_DUMP_MODE_CMB_RAW20    				= 3,
    VI_DUMP_MODE_CMB_COPMRESSED_RAW16    	= 4,
    VI_DUMP_MODE_LTM_RAW12    				= 5,
    VI_DUMP_MODE_L2Q_RAW12    				= 6,
    VI_DUMP_MODE_LTM_L2Q_RAW12    			= 7,
} VI_BAYER_MODE_E;

typedef	enum tsVI_BAYER_CHNL_E{
	VI_BAYER_CHNL_NONE = 0,
	VI_BAYER_CHNL_0,	/* long frame for linear and DOL */
	VI_BAYER_CHNL_1,	/* middle frame for DOL */
	VI_BAYER_CHNL_BOTH,	/* long frame and middle frame */
	VI_BAYER_CHNL_BUTT
}VI_BAYER_CHNL_E;

#define	VI_DUMP_PATH_MAX_LENGTH	22
typedef struct tsVI_DUMP_ATTR_S {
	VI_BAYER_CHNL_E		enBayerChnl[VI_MAX_PIPE_NUM];
	TS_U32          	u32DumpNum;
	VI_BAYER_MODE_E		enMode;
	VI_DUMP_TYPE_E  	enDumpType;
	RECT_S				stCropRect[VI_MAX_PIPE_NUM];
	TS_CHAR				s8PathName[VI_DUMP_PATH_MAX_LENGTH];
} VI_DUMP_ATTR_S;

typedef enum tsVI_USERPIC_MODE_E
{
	VI_USERPIC_MODE_PIC = 0,
	VI_USERPIC_MODE_BGC,
	VI_USERPIC_MODE_BUTT,
} VI_USERPIC_MODE_E;

typedef struct tsVI_USERPIC_BGC_S
{
	/**填充数据，与颜色的 RGB 值对应。取值范围：[0, 0xFFFFFF],输出图像的背景颜色，
		RGB  [23:16]:R     [15:8]:G     [7:0]:B**/
	TS_U32 u32BgColor;
} VI_USERPIC_BGC_S;

typedef struct tsVI_USERPIC_ATTR_S
{
	VI_USERPIC_MODE_E enUsrPicMode;
	union
	{
		VIDEO_FRAME_INFO_S stUsrPicFrm;
		VI_USERPIC_BGC_S stUsrPicBg;
	}unUsrPic; 
} VI_USERPIC_ATTR_S;

typedef enum tsVI_PIPE_FRAME_SOURCE_E
{
	VI_PIPE_FRAME_SOURCE_USER_FE = 0,
	VI_PIPE_FRAME_SOURCE_BUTT
} VI_PIPE_FRAME_SOURCE_E;

#define  VI_AIISP_MODEL_MAX_NUM 20
#define  MAX_AIISP_FILE_PATH_LEN 256
typedef	enum tsVI_AIISP_MODEL_TYPE_E{
	VI_AIISP_MODEL_TYPE_NFR = 1,
	VI_AIISP_MODEL_TYPE_RFR,
	VI_AIISP_MODEL_TYPE_BUTT
}VI_AIISP_MODEL_TYPE_E;

typedef struct tsVI_AIISP_MODEL_INFO_S {
	VI_AIISP_MODEL_TYPE_E eModeType;
	TS_U8 u8ModId;/**不同的eModeType可以modid重复，同一个类型必须modid唯一**/
	TS_CHAR chModePath[MAX_AIISP_FILE_PATH_LEN];
}VI_AIISP_MODEL_INFO_S;

typedef struct tsVI_AIISP_MODEL_GROUP_S {
	TS_U8 modelNum;// max VI_AIISP_MODEL_MAX_NUM
	VI_AIISP_MODEL_INFO_S stAiispModeInfo[VI_AIISP_MODEL_MAX_NUM];
}VI_AIISP_MODEL_GROUP_S;

typedef struct tsVI_AIISP_ATTR_S
{
	TS_BOOL bModelAutoSwitch;
}VI_AIISP_ATTR_S;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif



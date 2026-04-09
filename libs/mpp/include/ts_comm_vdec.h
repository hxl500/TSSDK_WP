/*
 * Copyright © 2019 TSING MICRO.com.cn All Rights Reserved
 * Description: ts_comm_vdec.h
 * Author:TsingMicro multimedia software group
 * Create: 2022/06/02
 */

#ifndef __TS_COMM_VDEC_H__
#define __TS_COMM_VDEC_H__
#include "ts_type.h"
#include "ts_common.h"
#include "ts_errno.h"
#include "ts_comm_video.h"
#include "ts_comm_vb.h"
#include "ts_defines.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#define TS_IO_BLOCK               TS_TRUE
#define TS_IO_NOBLOCK             TS_FALSE

typedef enum tsVIDEO_MODE_E {
    VIDEO_MODE_STREAM = 0, /* send by stream */
    VIDEO_MODE_FRAME, /* send by frame  */
    VIDEO_MODE_COMPAT, /* One frame supports multiple packets sending. */
    /* The current frame is considered to end when bEndOfFrame is equal to TS_TRUE */
    VIDEO_MODE_BUTT
} VIDEO_MODE_E;

typedef struct tsVDEC_ATTR_VIDEO_S {
    TS_U32 u32RefFrameNum; /* RW, Range: [0, 16]; reference frame num. */
    TS_BOOL bTemporalMvpEnable; /* RW; */
    /* specifies whether temporal motion vector predictors can be used for inter prediction */
    TS_U32 u32TmvBufSize; /* RW; tmv buffer size(Byte) */
} VDEC_ATTR_VIDEO_S;

typedef struct tsVDEC_CHN_ATTR_S {
    PAYLOAD_TYPE_E enType; /* RW; video type to be decoded   */
    VIDEO_MODE_E enMode; /* RW; send by stream or by frame */
    TS_U32 u32PicWidth; /* RW; max pic width */
    TS_U32 u32PicHeight; /* RW; max pic height */
    TS_U32 u32StreamBufSize; /* RW; stream buffer size(Byte) */
    TS_U32 u32FrameBufSize; /* RW; frame buffer size(Byte) */
    TS_U32 u32FrameBufCnt;
    union {
        VDEC_ATTR_VIDEO_S stVdecVideoAttr; /* structure with video ( h264/h265) */
    };
} VDEC_CHN_ATTR_S;

typedef struct tsVDEC_STREAM_S {
    TS_U32 u32Len; /* W; stream len */
    TS_U64 u64PTS; /* W; time stamp */
    TS_BOOL bEndOfFrame; /* W; is the end of a frame */
    TS_BOOL bEndOfStream; /* W; is the end of all stream */
    TS_BOOL bDisplay; /* W; is the current frame displayed. only valid by VIDEO_MODE_FRAME */
    TS_U8 *ATTRIBUTE pu8Addr; /* W; stream address */
	TS_U64 u64PrivData;
#ifdef ENABLE_MPP_TRACE
	uint64_t sys_run_time;
	TS_U8 node_info[6];
#endif
} VDEC_STREAM_S;

typedef struct tsVDEC_USERDATA_S {
    TS_U64 u64PhyAddr; /* R; userdata data phy address */
    TS_U32 u32Len; /* R; userdata data len */
    TS_BOOL bValid; /* R; is valid? */
    TS_U8 *ATTRIBUTE pu8Addr; /* R; userdata data vir address */
} VDEC_USERDATA_S;

typedef struct tsVDEC_DECODE_ERROR_S {
    TS_S32 s32FormatErr; /* R; format error. eg: do not support filed */
    TS_S32 s32PicSizeErrSet; /* R; picture width or height is larger than chnnel width or height */
    TS_S32 s32StreamUnsprt; /* R; unsupport the stream specification */
    TS_S32 s32PackErr; /* R; stream package error */
    TS_S32 s32PrtclNumErrSet; /* R; protocol num is not enough. eg: slice, pps, sps */
    TS_S32 s32RefErrSet; /* R; refrence num is not enough */
    TS_S32 s32PicBufSizeErrSet; /* R; the buffer size of picture is not enough */
    TS_S32 s32StreamSizeOver; /* R; the stream size is too big and force discard stream */
    TS_S32 s32VdecStreamNotRelease; /* R; the stream not released for too long time */
} VDEC_DECODE_ERROR_S;

typedef struct tsVDEC_CHN_STATUS_S {
    PAYLOAD_TYPE_E enType; /* R; video type to be decoded */
    TS_U32 u32LeftStreamBytes; /* R; left stream bytes waiting for decode */
    TS_U32 u32LeftStreamFrames; /* R; left frames waiting for decode,only valid for VIDEO_MODE_FRAME */
    TS_U32 u32LeftPics; /* R; pics waiting for output */
    TS_BOOL bStartRecvStream; /* R; had started recv stream? */
    TS_U32 u32RecvStreamFrames; /* R; how many frames of stream has been received. valid when send by frame. */
    TS_U32 u32DecodeStreamFrames; /* R; how many frames of stream has been decoded. valid when send by frame. */
    VDEC_DECODE_ERROR_S stVdecDecErr; /* R; information about decode error */
    TS_U32 u32Width; /* R; the width of the currently decoded stream */
    TS_U32 u32Height; /* R; the height of the currently decoded stream */
} VDEC_CHN_STATUS_S;

typedef enum tsVIDEO_DEC_MODE_E {
    VIDEO_DEC_MODE_IPB = 0,
    VIDEO_DEC_MODE_IP,
    VIDEO_DEC_MODE_I,
    VIDEO_DEC_MODE_BUTT
} VIDEO_DEC_MODE_E;

typedef enum tsVIDEO_OUTPUT_ORDER_E {
    VIDEO_OUTPUT_ORDER_DISP = 0,
    VIDEO_OUTPUT_ORDER_DEC,
    VIDEO_OUTPUT_ORDER_BUTT
} VIDEO_OUTPUT_ORDER_E;

typedef struct tsVDEC_PARAM_VIDEO_S {
    TS_S32 s32ErrThreshold; /* RW, Range: [0, 100]; */
    /* threshold for stream error process, 0: discard with any error, 100 : keep data with any error */
    VIDEO_DEC_MODE_E enDecMode; /* RW; */
    /* decode mode , 0: deocde IPB frames, 1: only decode I frame & P frame , 2: only decode I frame */
    VIDEO_OUTPUT_ORDER_E enOutputOrder; /* RW; */
    /* frames output order ,0: the same with display order , 1: the same width decoder order */
    COMPRESS_MODE_E enCompressMode; /* RW; compress mode */
    VIDEO_FORMAT_E enVideoFormat; /* RW; video format */
} VDEC_PARAM_VIDEO_S;

typedef struct tsVDEC_PARAM_PICTURE_S {
    PIXEL_FORMAT_E enPixelFormat; /* RW; out put pixel format */
    TS_U32 u32Alpha; /* RW, Range: [0, 255]; value 0 is transparent. */
    /* [0 ,127]   is deemed to transparent when enPixelFormat is ARGB1555 or ABGR1555
     * [128 ,256] is deemed to non-transparent when enPixelFormat is ARGB1555 or ABGR1555
     */
} VDEC_PARAM_PICTURE_S;

typedef struct tsVDEC_CHN_PARAM_S {
    PAYLOAD_TYPE_E enType; /* RW; video type to be decoded   */
    TS_U32 u32DisplayFrameNum; /* RW, Range: [0, 16]; display frame num */
    union {
        VDEC_PARAM_VIDEO_S stVdecVideoParam; /* structure with video ( h265/h264) */
        VDEC_PARAM_PICTURE_S stVdecPictureParam; /* structure with picture (jpeg/mjpeg ) */
    };
} VDEC_CHN_PARAM_S;

typedef struct tsH264_PRTCL_PARAM_S {
    TS_S32 s32MaxSliceNum; /* RW; max slice num support */
    TS_S32 s32MaxSpsNum; /* RW; max sps num support */
    TS_S32 s32MaxPpsNum; /* RW; max pps num support */
} H264_PRTCL_PARAM_S;

typedef struct tsH265_PRTCL_PARAM_S {
    TS_S32 s32MaxSliceSegmentNum; /* RW; max slice segmnet num support */
    TS_S32 s32MaxVpsNum; /* RW; max vps num support */
    TS_S32 s32MaxSpsNum; /* RW; max sps num support */
    TS_S32 s32MaxPpsNum; /* RW; max pps num support */
} H265_PRTCL_PARAM_S;

typedef struct tsVDEC_PRTCL_PARAM_S {
    PAYLOAD_TYPE_E enType; /* RW; video type to be decoded, only h264 and h265 supported */
    union {
        H264_PRTCL_PARAM_S stH264PrtclParam; /* protocol param structure for h264 */
        H265_PRTCL_PARAM_S stH265PrtclParam; /* protocol param structure for h265 */
    };
} VDEC_PRTCL_PARAM_S;

typedef struct tsVDEC_CHN_POOL_S {
    VB_POOL hPicVbPool; /* RW;  vb pool id for pic buffer */
    VB_POOL hTmvVbPool; /* RW;  vb pool id for tmv buffer */
} VDEC_CHN_POOL_S;

typedef enum tsVDEC_EVNT_E {
    VDEC_EVNT_STREAM_ERR = 1,
    VDEC_EVNT_UNSUPPORT,
    VDEC_EVNT_OVER_REFTHR,
    VDEC_EVNT_REF_NUM_OVER,
    VDEC_EVNT_SLICE_NUM_OVER,
    VDEC_EVNT_SPS_NUM_OVER,
    VDEC_EVNT_PPS_NUM_OVER,
    VDEC_EVNT_PICBUF_SIZE_ERR,
    VDEC_EVNT_SIZE_OVER,
    VDEC_EVNT_IMG_SIZE_CHANGE,
    VDEC_EVNT_VPS_NUM_OVER,
    VDEC_EVNT_BUTT
} VDEC_EVNT_E;

typedef enum tsVDEC_CAPACITY_STRATEGY_E {
    VDEC_CAPACITY_STRATEGY_BY_MOD = 0,
    VDEC_CAPACITY_STRATEGY_BY_CHN = 1,
    VDEC_CAPACITY_STRATEGY_BUTT
} VDEC_CAPACITY_STRATEGY_E;

typedef struct tsVDEC_VIDEO_MOD_PARAM_S {
    TS_U32 u32MaxPicWidth;
    TS_U32 u32MaxPicHeight;
    TS_U32 u32MaxSliceNum;
    TS_U32 u32VdhMsgNum;
    TS_U32 u32VdhBinSize;
    TS_U32 u32VdhExtMemLevel;
} VDEC_VIDEO_MOD_PARAM_S;

typedef struct tsVDEC_PICTURE_MOD_PARAM_S {
    TS_U32 u32MaxPicWidth;
    TS_U32 u32MaxPicHeight;
    TS_BOOL bSupportProgressive;
    TS_BOOL bDynamicAllocate;
    VDEC_CAPACITY_STRATEGY_E enCapStrategy;
} VDEC_PICTURE_MOD_PARAM_S;

typedef struct tsVDEC_MOD_PARAM_S {
    VB_SOURCE_E enVdecVBSource; /* RW, Range: [1, 3];  frame buffer mode  */
    TS_U32 u32MiniBufMode; /* RW, Range: [0, 1];  stream buffer mode */
    TS_U32 u32ParallelMode; /* RW, Range: [0, 1];  VDH working mode   */
    VDEC_VIDEO_MOD_PARAM_S stVideoModParam;
    VDEC_PICTURE_MOD_PARAM_S stPictureModParam;
} VDEC_MOD_PARAM_S;

typedef struct tsVDEC_USER_DATA_ATTR_S {
    TS_BOOL bEnable;
    TS_U32  u32MaxUserDataLen;
} VDEC_USER_DATA_ATTR_S;

/*********************************************************************************************/
/* invlalid channel ID */
#define TS_ERR_VDEC_INVALID_CHNID TS_DEF_ERR(TS_ID_VDEC, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_CHNID)
/* at lease one parameter is illagal ,eg, an illegal enumeration value  */
#define TS_ERR_VDEC_ILLEGAL_PARAM TS_DEF_ERR(TS_ID_VDEC, EN_ERR_LEVEL_ERROR, EN_ERR_ILLEGAL_PARAM)
/* channel exists */
#define TS_ERR_VDEC_EXIST         TS_DEF_ERR(TS_ID_VDEC, EN_ERR_LEVEL_ERROR, EN_ERR_EXIST)
/* using a NULL point */
#define TS_ERR_VDEC_NULL_PTR      TS_DEF_ERR(TS_ID_VDEC, EN_ERR_LEVEL_ERROR, EN_ERR_NULL_PTR)
/* try to enable or initialize system,device or channel, before configing attribute */
#define TS_ERR_VDEC_NOT_CONFIG    TS_DEF_ERR(TS_ID_VDEC, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_CONFIG)
/* operation is not supported by NOW */
#define TS_ERR_VDEC_NOT_SUPPORT   TS_DEF_ERR(TS_ID_VDEC, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_SUPPORT)
/* operation is not permitted ,eg, try to change stati attribute */
#define TS_ERR_VDEC_NOT_PERM      TS_DEF_ERR(TS_ID_VDEC, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_PERM)
/* the channle is not existed  */
#define TS_ERR_VDEC_UNEXIST       TS_DEF_ERR(TS_ID_VDEC, EN_ERR_LEVEL_ERROR, EN_ERR_UNEXIST)
/* failure caused by malloc memory */
#define TS_ERR_VDEC_NOMEM         TS_DEF_ERR(TS_ID_VDEC, EN_ERR_LEVEL_ERROR, EN_ERR_NOMEM)
/* failure caused by malloc buffer */
#define TS_ERR_VDEC_NOBUF         TS_DEF_ERR(TS_ID_VDEC, EN_ERR_LEVEL_ERROR, EN_ERR_NOBUF)
/* no data in buffer */
#define TS_ERR_VDEC_BUF_EMPTY     TS_DEF_ERR(TS_ID_VDEC, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_EMPTY)
/* no buffer for new data */
#define TS_ERR_VDEC_BUF_FULL      TS_DEF_ERR(TS_ID_VDEC, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_FULL)
/* system is not ready,had not initialed or loaded */
#define TS_ERR_VDEC_SYS_NOTREADY  TS_DEF_ERR(TS_ID_VDEC, EN_ERR_LEVEL_ERROR, EN_ERR_SYS_NOTREADY)
/* system busy */
#define TS_ERR_VDEC_BUSY          TS_DEF_ERR(TS_ID_VDEC, EN_ERR_LEVEL_ERROR, EN_ERR_BUSY)

/* bad address,  eg. used for copy_from_user & copy_to_user   */
#define TS_ERR_VDEC_BADADDR       TS_DEF_ERR(TS_ID_VDEC, EN_ERR_LEVEL_ERROR, EN_ERR_BADADDR)

typedef void (* pf_empty_done_callback)(void *hdl);
typedef void (* pf_fill_done_callback)(void *hdl);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef  __TS_COMM_VDEC_H__ */



/*
 * Copyright © 2019 TSING MICRO.com.cn All Rights Reserved
 * Description: ts_comm_venc.h
 * Author:TsingMicro multimedia software group
 * Create: 2022/06/02
 */

#ifndef __TS_COMM_VENC_H__
#define __TS_COMM_VENC_H__

#include "ts_type.h"
#include "ts_common.h"
#include "ts_errno.h"
#include "ts_comm_video.h"
#include "ts_comm_rc.h"
#include "ts_comm_vb.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define MAX_MOTION_DETECT_NUM     100		// max detect number

/* invlalid channel ID */
#define TS_ERR_VENC_INVALID_CHNID TS_DEF_ERR(TS_ID_VENC, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_CHNID)
/* at lease one parameter is illagal ,eg, an illegal enumeration value  */
#define TS_ERR_VENC_ILLEGAL_PARAM TS_DEF_ERR(TS_ID_VENC, EN_ERR_LEVEL_ERROR, EN_ERR_ILLEGAL_PARAM)
/* channel exists */
#define TS_ERR_VENC_EXIST         TS_DEF_ERR(TS_ID_VENC, EN_ERR_LEVEL_ERROR, EN_ERR_EXIST)
/* channel exists */
#define TS_ERR_VENC_UNEXIST       TS_DEF_ERR(TS_ID_VENC, EN_ERR_LEVEL_ERROR, EN_ERR_UNEXIST)
/* using a NULL point */
#define TS_ERR_VENC_NULL_PTR      TS_DEF_ERR(TS_ID_VENC, EN_ERR_LEVEL_ERROR, EN_ERR_NULL_PTR)
/* try to enable or initialize system,device or channel, before configing attribute */
#define TS_ERR_VENC_NOT_CONFIG    TS_DEF_ERR(TS_ID_VENC, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_CONFIG)
/* operation is not supported by NOW */
#define TS_ERR_VENC_NOT_SUPPORT   TS_DEF_ERR(TS_ID_VENC, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_SUPPORT)
/* operation is not permitted ,eg, try to change stati attribute */
#define TS_ERR_VENC_NOT_PERM      TS_DEF_ERR(TS_ID_VENC, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_PERM)
/* failure caused by malloc memory */
#define TS_ERR_VENC_NOMEM         TS_DEF_ERR(TS_ID_VENC, EN_ERR_LEVEL_ERROR, EN_ERR_NOMEM)
/* failure caused by malloc buffer */
#define TS_ERR_VENC_NOBUF         TS_DEF_ERR(TS_ID_VENC, EN_ERR_LEVEL_ERROR, EN_ERR_NOBUF)
/* no data in buffer */
#define TS_ERR_VENC_BUF_EMPTY     TS_DEF_ERR(TS_ID_VENC, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_EMPTY)
/* no buffer for new data */
#define TS_ERR_VENC_BUF_FULL      TS_DEF_ERR(TS_ID_VENC, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_FULL)
/* system is not ready,had not initialed or loaded */
#define TS_ERR_VENC_SYS_NOTREADY  TS_DEF_ERR(TS_ID_VENC, EN_ERR_LEVEL_ERROR, EN_ERR_SYS_NOTREADY)
/* system is busy */
#define TS_ERR_VENC_BUSY          TS_DEF_ERR(TS_ID_VENC, EN_ERR_LEVEL_ERROR, EN_ERR_BUSY)

/* the nalu type of H264E */
typedef enum tsH264E_NALU_TYPE_E {
    H264E_NALU_BSLICE = 0,                         /* B SLICE types */
    H264E_NALU_PSLICE = 1,                         /* P SLICE types */
    H264E_NALU_ISLICE = 2,                         /* I SLICE types */
    H264E_NALU_IDRSLICE = 5,                       /* IDR SLICE types */
    H264E_NALU_SEI    = 6,                         /* SEI types */
    H264E_NALU_SPS    = 7,                         /* SPS types */
    H264E_NALU_PPS    = 8,                         /* PPS types */
    H264E_NALU_BUTT
} H264E_NALU_TYPE_E;

/* the nalu type of H265E */
typedef enum tsH265E_NALU_TYPE_E {
    H265E_NALU_BSLICE = 0,                          /* B SLICE types */
    H265E_NALU_PSLICE = 1,                          /* P SLICE types */
    H265E_NALU_ISLICE = 2,                          /* I SLICE types */
    H265E_NALU_IDRSLICE = 19,                       /* IDR SLICE types */
    H265E_NALU_VPS    = 32,                         /* VPS types */
    H265E_NALU_SPS    = 33,                         /* SPS types */
    H265E_NALU_PPS    = 34,                         /* PPS types */
    H265E_NALU_SEI    = 39,                         /* SEI types */

    H265E_NALU_BUTT
} H265E_NALU_TYPE_E;

/* the reference type of H264E slice */
typedef enum tsH264E_REFSLICE_TYPE_E {
    H264E_REFSLICE_FOR_1X = 1,                     /* Reference slice for H264E_REF_MODE_1X */
    H264E_REFSLICE_FOR_2X = 2,                     /* Reference slice for H264E_REF_MODE_2X */
    H264E_REFSLICE_FOR_4X = 5,                     /* Reference slice for H264E_REF_MODE_4X */
    H264E_REFSLICE_FOR_BUTT                        /* slice not for reference */
} H264E_REFSLICE_TYPE_E;

/* the pack type of JPEGE */
typedef enum tsJPEGE_PACK_TYPE_E {
    JPEGE_PACK_ECS = 5,                            /* ECS types */
    JPEGE_PACK_APP = 6,                            /* APP types */
    JPEGE_PACK_VDO = 7,                            /* VDO types */
    JPEGE_PACK_PIC = 8,                            /* PIC types */
    JPEGE_PACK_DCF = 9,                            /* DCF types */
    JPEGE_PACK_DCF_PIC = 10,                       /* DCF PIC types */
    JPEGE_PACK_BUTT
} JPEGE_PACK_TYPE_E;

/* the pack type of PRORES */
typedef enum tsPRORES_PACK_TYPE_E {
    PRORES_PACK_PIC = 1,                            /* PIC types */
    PRORES_PACK_BUTT
} PRORES_PACK_TYPE_E;

/* the data type of VENC */
typedef union tsVENC_DATA_TYPE_U {
    H264E_NALU_TYPE_E    enH264EType;               /* R; H264E NALU types */
    JPEGE_PACK_TYPE_E    enJPEGEType;               /* R; JPEGE pack types */
    H265E_NALU_TYPE_E    enH265EType;               /* R; H264E NALU types */
    PRORES_PACK_TYPE_E   enPRORESType;
} VENC_DATA_TYPE_U;

/* the pack info of VENC */
typedef struct tsVENC_PACK_INFO_S {
    VENC_DATA_TYPE_U  u32PackType;                  /* R; the pack type */
    TS_U32 u32PackOffset;
    TS_U32 u32PackLength;
} VENC_PACK_INFO_S;

/* Defines a stream packet */
typedef struct tsVENC_PACK_S {
    TS_U64               u64PhyAddr;                 /* R; the physics address of stream */
    TS_U8                ATTRIBUTE* pu8Addr;         /* R; the virtual address of stream */
    TS_U32               ATTRIBUTE u32Len;           /* R; the length of stream */

    TS_U64               u64PTS;                     /* R; PTS */
    TS_BOOL              bFrameEnd;                  /* R; frame end */

    VENC_DATA_TYPE_U     DataType;                   /* R; the type of stream */
    TS_U32               u32Offset;                  /* R; the offset between the Valid data and the start address */
    TS_U32               u32DataNum;                 /* R; the  stream packets num */
    VENC_PACK_INFO_S     stPackInfo[8];              /* R; the stream packet Information */
} VENC_PACK_S;

/* Defines the frame type and reference attributes of the H.264 frame skipping reference streams */
typedef enum tsH264E_REF_TYPE_E {
    BASE_IDRSLICE = 0,                              /* the Idr frame at Base layer */
    BASE_PSLICE_REFTOIDR,                           /* the P frame at Base layer, referenced by other frames
                                                        at Base layer and reference to Idr frame */
    BASE_PSLICE_REFBYBASE,                          /* the P frame at Base layer, referenced by other frames
                                                        at Base layer */
    BASE_PSLICE_REFBYENHANCE,                       /* the P frame at Base layer, referenced by other frames
                                                        at Enhance layer */
    ENHANCE_PSLICE_REFBYENHANCE,                    /* the P frame at Enhance layer, referenced by other frames
                                                        at Enhance layer */
    ENHANCE_PSLICE_NOTFORREF,                       /* the P frame at Enhance layer ,not referenced */
    ENHANCE_PSLICE_BUTT
} H264E_REF_TYPE_E;

typedef enum tsH264E_REF_TYPE_E H265E_REF_TYPE_E;

/* Defines the features of an H.264 stream */
typedef struct tsVENC_STREAM_INFO_H264_S {
    TS_U32                 u32PicBytesNum;         /* R; the coded picture stream byte number */
    TS_U32                 u32Inter16x16MbNum;     /* R; the inter16x16 macroblock num */
    TS_U32                 u32Inter8x8MbNum;       /* R; the inter8x8 macroblock num */
    TS_U32                 u32Intra16MbNum;        /* R; the intra16x16 macroblock num */
    TS_U32                 u32Intra8MbNum;         /* R; the intra8x8 macroblock num */
    TS_U32                 u32Intra4MbNum;         /* R; the inter4x4 macroblock num */

    H264E_REF_TYPE_E       enRefType;              /* R; Type of encoded frames in
                                                       advanced frame skipping reference mode */
    TS_U32                 u32UpdateAttrCnt;       /* R; Number of times that channel attributes or parameters
                                                       (including RC parameters) are set */
    TS_U32                 u32StartQp;             /* R; the start Qp of encoded frames */
    TS_U32                 u32MeanQp;              /* R; the mean Qp of encoded frames */
    TS_BOOL                bPSkip;
} VENC_STREAM_INFO_H264_S;

/* Defines the features of an H.265 stream */
typedef struct tsVENC_STREAM_INFO_H265_S {
    TS_U32                 u32PicBytesNum;        /* R; the coded picture stream byte number */
    TS_U32                 u32Inter64x64CuNum;    /* R; the inter64x64 cu num  */
    TS_U32                 u32Inter32x32CuNum;    /* R; the inter32x32 cu num  */
    TS_U32                 u32Inter16x16CuNum;    /* R; the inter16x16 cu num  */
    TS_U32                 u32Inter8x8CuNum;      /* R; the inter8x8   cu num  */
    TS_U32                 u32Intra32x32CuNum;    /* R; the Intra32x32 cu num  */
    TS_U32                 u32Intra16x16CuNum;    /* R; the Intra16x16 cu num  */
    TS_U32                 u32Intra8x8CuNum;      /* R; the Intra8x8   cu num  */
    TS_U32                 u32Intra4x4CuNum;      /* R; the Intra4x4   cu num  */

    H265E_REF_TYPE_E       enRefType;            /* R; Type of encoded frames
                                                     in advanced frame skipping reference mode */
    TS_U32                 u32UpdateAttrCnt;     /* R; Number of times that channel attributes or
                                                     parameters (including RC parameters) are set */
    TS_U32                 u32StartQp;           /* R; the start Qp of encoded frames */
    TS_U32                 u32MeanQp;            /* R; the mean Qp of encoded frames */
    TS_BOOL                bPSkip;
} VENC_STREAM_INFO_H265_S;

/* the sse info */
typedef struct tsVENC_SSE_INFO_S {
    TS_BOOL bSSEEn;        /* RW; Range:[0,1]; Region SSE enable */
    TS_U32  u32SSEVal;     /* R; Region SSE value */
} VENC_SSE_INFO_S;

/* the advance information of the h264e */
typedef struct tsVENC_STREAM_ADVANCE_INFO_H264_S {
    TS_U32             u32ResidualBitNum;                    /* R; the residual num */
    TS_U32             u32HeadBitNum;                        /* R; the head bit num */
    TS_U32             u32MadiVal;                           /* R; the madi value */
    TS_U32             u32MadpVal;                           /* R; the madp value */
    TS_DOUBLE          dPSNRVal;                             /* R; the PSNR value */
    TS_U32             u32MseLcuCnt;                         /* R; the lcu cnt of the mse */
    TS_U32             u32MseSum;                            /* R; the sum of the mse */
    VENC_SSE_INFO_S    stSSEInfo[8];                         /* R; the information of the sse */
    TS_U32             u32QpHstgrm[VENC_QP_HISGRM_NUM];      /* R; the Qp tsstogram value */
    TS_U32             u32MoveScene16x16Num;                 /* R; the 16x16 cu num of the move scene */
    TS_U32             u32MoveSceneBits;                     /* R; the stream bit num of the move scene */
} VENC_STREAM_ADVANCE_INFO_H264_S;

/* the advance information of the Jpege */
typedef struct tsVENC_STREAM_ADVANCE_INFO_JPEG_S {
    // TS_U32 u32Reserved;
} VENC_STREAM_ADVANCE_INFO_JPEG_S;

/* the advance information of the Prores */
typedef struct tsVENC_STREAM_ADVANCE_INFO_PRORES_S {
    // TS_U32 u32Reserved;
} VENC_STREAM_ADVANCE_INFO_PRORES_S;

/* the advance information of the h265e */
typedef struct tsVENC_STREAM_ADVANCE_INFO_H265_S {
    TS_U32             u32ResidualBitNum;               /* R; the residual num */
    TS_U32             u32HeadBitNum;                   /* R; the head bit num */
    TS_U32             u32MadiVal;                      /* R; the madi value */
    TS_U32             u32MadpVal;                      /* R; the madp value */
    TS_DOUBLE          dPSNRVal;                        /* R; the PSNR value */
    TS_U32             u32MseLcuCnt;                    /* R; the lcu cnt of the mse */
    TS_U32             u32MseSum;                       /* R; the sum of the mse */
    VENC_SSE_INFO_S    stSSEInfo[8];                    /* R; the information of the sse */
    TS_U32             u32QpHstgrm[VENC_QP_HISGRM_NUM]; /* R; the Qp histogram value */
    TS_U32             u32MoveScene32x32Num;            /* R; the 32x32 cu num of the move scene */
    TS_U32             u32MoveSceneBits;                /* R; the stream bit num of the move scene */
} VENC_STREAM_ADVANCE_INFO_H265_S;

/* Defines the features of an jpege stream */
typedef struct tsVENC_STREAM_INFO_PRORES_S {
    TS_U32 u32PicBytesNum;
    TS_U32 u32UpdateAttrCnt;
} VENC_STREAM_INFO_PRORES_S;

/* Defines the features of an jpege stream */
typedef struct tsVENC_STREAM_INFO_JPEG_S {
    TS_U32 u32PicBytesNum;                      /* R; the coded picture stream byte number */
    TS_U32 u32UpdateAttrCnt;                    /* R; Number of times that channel attributes or parameters
                                                    (including RC parameters) are set */
    TS_U32 u32Qfactor;                          /* R; image quality */
} VENC_STREAM_INFO_JPEG_S;

/* Defines the features of an stream */
typedef struct tsVENC_STREAM_S {
    VENC_PACK_S ATTRIBUTE* pstPack;            /* R; stream pack attribute */
    TS_U32      ATTRIBUTE u32PackCount;        /* R; the pack number of one frame stream */
    TS_U32      u32Seq;                        /* R; the list number of stream */
	TS_U32		u32Width;
	TS_U32		u32Height;
	TS_S32 s32Fd;

    union {
        VENC_STREAM_INFO_H264_S   stH264Info;                        /* R; the stream info of h264 */
        VENC_STREAM_INFO_JPEG_S   stJpegInfo;                        /* R; the stream info of jpeg */
        VENC_STREAM_INFO_H265_S   stH265Info;                        /* R; the stream info of h265 */
        VENC_STREAM_INFO_PRORES_S stProresInfo;                      /* R; the stream info of prores */
    };

    union {
        VENC_STREAM_ADVANCE_INFO_H264_S   stAdvanceH264Info;         /* R; the stream info of h264 */
        VENC_STREAM_ADVANCE_INFO_JPEG_S   stAdvanceJpegInfo;         /* R; the stream info of jpeg */
        VENC_STREAM_ADVANCE_INFO_H265_S   stAdvanceH265Info;         /* R; the stream info of h265 */
        VENC_STREAM_ADVANCE_INFO_PRORES_S stAdvanceProresInfo;       /* R; the stream info of prores */
    };
} VENC_STREAM_S;

typedef struct tsVENC_STREAM_INFO_S {
    H265E_REF_TYPE_E enRefType;  /* R;Type of encoded frames in advanced frame skipping reference mode */

    TS_U32  u32PicBytesNum;      /* R;the coded picture stream byte number */
    TS_U32  u32PicCnt;           /* R;When channel attributes 'bByFrame == 1', it means count of frames.
                                      When channel attributes 'bByFrame == 0', it means count of packets */
    TS_U32  u32StartQp;          /* R;the start Qp of encoded frames */
    TS_U32  u32MeanQp;           /* R;the mean Qp of encoded frames */
    TS_BOOL bPSkip;

    TS_U32  u32ResidualBitNum;   /* R;residual */
    TS_U32  u32HeadBitNum;       /* R;head information */
    TS_U32  u32MadiVal;          /* R;madi */
    TS_U32  u32MadpVal;          /* R;madp */
    TS_U32  u32MseSum;           /* R;Sum of MSE value */
    TS_U32  u32MseLcuCnt;        /* R;Sum of LCU number */
    TS_DOUBLE dPSNRVal;          /* R;PSNR */
} VENC_STREAM_INFO_S;

/* the size of array is 2,that is the maximum */
typedef struct tsVENC_MPF_CFG_S {
    TS_U8   u8LargeThumbNailNum;      /* RW; Range:[0, 2]; the large thumbnail pic num of the MPF */
    SIZE_S  astLargeThumbNailSize[2]; /* RW; The resolution of large ThumbNail */
} VENC_MPF_CFG_S;

typedef enum tsVENC_PIC_RECEIVE_MODE_E {
    VENC_PIC_RECEIVE_SINGLE = 0,
    VENC_PIC_RECEIVE_MULTI,

    VENC_PIC_RECEIVE_BUTT
} VENC_PIC_RECEIVE_MODE_E;

/* the attribute of jpege */
typedef struct tsVENC_ATTR_JPEG_S {
    TS_BOOL                     bSupportDCF;    /*RW; Range:[0, 1]; support dcf */
    VENC_MPF_CFG_S              stMPFCfg;       /*RW; Range:[0, 1]; config of Mpf */
    VENC_PIC_RECEIVE_MODE_E  	enReceiveMode;  /*RW; Config the receive mode */
} VENC_ATTR_JPEG_S;

/* the attribute of mjpege */
typedef struct tsVENC_ATTR_MJPEG_S {
    // reserved
} VENC_ATTR_MJPEG_S;

/* the attribute of h264e */
typedef struct tsVENC_ATTR_H264_S {
    TS_BOOL bRcnRefShareBuf;                 /* RW; Range:[0, 1]; Whether to enable the Share Buf of Rcn and Ref */
} VENC_ATTR_H264_S;

/* the attribute of h265e */
typedef struct tsVENC_ATTR_H265_S {
    TS_BOOL bRcnRefShareBuf;                 /* RW; Range:[0, 1]; Whether to enable the Share Buf of Rcn and Ref */
} VENC_ATTR_H265_S;

/* the frame rate of PRORES */
typedef enum tsPRORES_FRAMERATE {
    PRORES_FR_UNKNOWN = 0,
    PRORES_FR_23_976,
    PRORES_FR_24,
    PRORES_FR_25,
    PRORES_FR_29_97,
    PRORES_FR_30,
    PRORES_FR_50,
    PRORES_FR_59_94,
    PRORES_FR_60,
    PRORES_FR_100,
    PRORES_FR_119_88,
    PRORES_FR_120,
    PRORES_FR_BUTT
} PRORES_FRAMERATE;

/* the aspect ratio of PRORES */
typedef enum tsPRORES_ASPECT_RATIO {
    PRORES_ASPECT_RATIO_UNKNOWN = 0,
    PRORES_ASPECT_RATIO_SQUARE,
    PRORES_ASPECT_RATIO_4_3,
    PRORES_ASPECT_RATIO_16_9,
    PRORES_ASPECT_RATIO_BUTT
} PRORES_ASPECT_RATIO;

/* the attribute of PRORES */
typedef struct tsVENC_ATTR_PRORES_S {
    TS_CHAR             cIdentifier[4];
    PRORES_FRAMERATE    enFrameRateCode;
    PRORES_ASPECT_RATIO enAspectRatio;
} VENC_ATTR_PRORES_S;


typedef struct tsVENC_ATTR_PRI_S {
    TS_BOOL bAllocBufOnce;              /* RW; Range:[0, 1]; Whether to allocate buffer once */
}tsVENC_ATTR_PRI_S;

/* the attribute of the Venc */
typedef struct tsVENC_ATTR_S {
    PAYLOAD_TYPE_E  enType;              /* RW; the type of payload */

    TS_U32  u32MaxPicWidth;              /* RW; Range:[0, 16384];maximum width of a picture to be encoded, in pixel */
    TS_U32  u32MaxPicHeight;             /* RW; Range:[0, 16384];maximum height of a picture to be encoded, in pixel */

    TS_U32  u32BufSize;                  /* RW; stream buffer size */
    TS_U32  u32Profile;                  /* RW; Range:[0, 3];
                                            H.264:   0: baseline; 1:MP; 2:HP; 3: SVC-T [0, 3];
                                            H.265:   0:MP; 1:Main 10  [0 1];
                                            Jpege/MJpege:   0:Baseline
                                            prores: 0:ProRes Proxy; 1:ProRes 422(LT); 2:ProRes 422; 3:ProRes 422(HQ) */
    TS_BOOL bByFrame;                    /* RW; Range:[0, 1]; get stream mode is slice mode or frame mode */

	//u32PicWidth and u32PicHeight are dyn-attr, other elements in VENC_ATTR_S are static-attr
    TS_U32  u32PicWidth;                 /* RW; Range:[0, 16384];width of a picture to be encoded, in pixel */
    TS_U32  u32PicHeight;                /* RW; Range:[0, 16384];height of a picture to be encoded, in pixel */

    union {
        VENC_ATTR_H264_S stAttrH264e;    /* attributes of H264e */
        VENC_ATTR_H265_S stAttrH265e;    /* attributes of H265e */
        VENC_ATTR_MJPEG_S stAttrMjpege;  /* attributes of Mjpeg */
        VENC_ATTR_JPEG_S  stAttrJpege;   /* attributes of jpeg  */
        VENC_ATTR_PRORES_S stAttrProres; /* attributes of prores  */
    };

    tsVENC_ATTR_PRI_S stAttrPrivate;         /* attributes of private */

} VENC_ATTR_S;

/* the gop mode */
typedef enum tsVENC_GOP_MODE_E {
    VENC_GOPMODE_NORMALP    = 0,     /* NORMALP */
    VENC_GOPMODE_DUALP      = 1,     /* DUALP */
    VENC_GOPMODE_SMARTP     = 2,     /* SMARTP */
    VENC_GOPMODE_ADVSMARTP  = 3,     /* ADVSMARTP */
    VENC_GOPMODE_BIPREDB    = 4,     /* BIPREDB */
    VENC_GOPMODE_LOWDELAYB  = 5,     /* LOWDELAYB; Not support */

    VENC_GOPMODE_BUTT,
} VENC_GOP_MODE_E;

/* the attribute of the normalp */
typedef struct tsVENC_GOP_NORMALP_S {
    TS_S32   s32IPQpDelta;           /* RW; Range:[-10, 30]; QP variance between P frame and I frame */
} VENC_GOP_NORMALP_S;

/* the attribute of the dualp */
typedef struct tsVENC_GOP_DUALP_S {
    TS_U32 u32SPInterval;            /* RW; Range:[0, 65536]; Interval of the special P frames,
                                        1 is not supported and should be less than Gop */
    TS_S32 s32SPQpDelta;             /* RW; Range:[-10, 30]; QP variance between P frame and special P frame */
    TS_S32 s32IPQpDelta;             /* RW; Range:[-10, 30]; QP variance between P frame and I frame */
} VENC_GOP_DUALP_S;

/* the attribute of the smartp */
typedef struct tsVENC_GOP_SMARTP_S {
    TS_U32  u32BgInterval;           /* RW; Interval of the long-term reference frame, can not be less than gop */
    TS_S32  s32BgQpDelta;            /* RW; Range:[-10, 30]; QP variance between P frame and Bg frame */
    TS_S32  s32ViQpDelta;            /* RW; Range:[-10, 30]; QP variance between P frame and virtual I  frame */
} VENC_GOP_SMARTP_S;

/* the attribute of the advsmartp */
typedef struct tsVENC_GOP_ADVSMARTP_S {
    TS_U32  u32BgInterval;           /* RW; Interval of the long-term reference frame, can not be less than gop */
    TS_S32  s32BgQpDelta;            /* RW; Range:[-10, 30]; QP variance between P frame and Bg frame */
    TS_S32  s32ViQpDelta;            /* RW; Range:[-10, 30]; QP variance between P frame and virtual I  frame */
} VENC_GOP_ADVSMARTP_S;

/* the attribute of the bipredb */
typedef struct tsVENC_GOP_BIPREDB_S {
    TS_U32 u32BFrmNum;              /* RW; Range:[1, 3]; Number of B frames */
    TS_S32 s32BQpDelta;             /* RW; Range:[-10, 30]; QP variance between P frame and B frame */
    TS_S32 s32IPQpDelta;            /* RW; Range:[-10, 30]; QP variance between P frame and I frame */
} VENC_GOP_BIPREDB_S;

/* the attribute of the gop */
typedef struct tsVENC_GOP_ATTR_S {
    VENC_GOP_MODE_E enGopMode;                   /* RW; Encoding GOP type */
    union {
        VENC_GOP_NORMALP_S   stNormalP;          /* attributes of normal P */
        VENC_GOP_DUALP_S     stDualP;            /* attributes of dual   P */
        VENC_GOP_SMARTP_S    stSmartP;           /* attributes of Smart P */
        VENC_GOP_ADVSMARTP_S stAdvSmartP;        /* attributes of AdvSmart P */
        VENC_GOP_BIPREDB_S   stBipredB;          /* attributes of b */
    };

} VENC_GOP_ATTR_S;

/* the attribute of the venc chnl */
typedef struct tsVENC_CHN_ATTR_S {
    VENC_ATTR_S     stVencAttr;                   /* the attribute of video encoder */
    VENC_RC_ATTR_S  stRcAttr;                     /* the attribute of rate  ctrl */
    VENC_GOP_ATTR_S stGopAttr;                    /* the attribute of gop */
} VENC_CHN_ATTR_S;

/* the param of receive picture */
typedef struct tsVENC_RECV_PIC_PARAM_S {
    TS_S32 s32RecvPicNum;                         /* RW; Range:[-1, 2147483647]; Number of frames received and
                                                    encoded by the encoding channel,0 is not supported */
} VENC_RECV_PIC_PARAM_S;

/* the status of the venc chnl */
typedef struct tsVENC_CHN_STATUS_S {
    TS_U32 u32LeftPics;                           /* R; left picture number */
    TS_U32 u32LeftStreamBytes;                    /* R; left stream bytes */
    TS_U32 u32LeftStreamFrames;                   /* R; left stream frames */
    TS_U32 u32CurPacks;                           /* R; pack number of current frame */
    TS_U32 u32LeftRecvPics;                       /* R; Number of frames to be received. This member is valid
                                                    after TS_MPI_VENC_StartRecvPicEx is called. */
    TS_U32 u32LeftEncPics;                        /* R; Number of frames to be encoded. This member is valid
                                                    after TS_MPI_VENC_StartRecvPicEx is called. */
    TS_BOOL bJpegSnapEnd;                         /* R; the end of Snap.*/
    VENC_STREAM_INFO_S stVencStrmInfo;
} VENC_CHN_STATUS_S;

/* the param of the h264e slice split */
typedef struct tsVENC_H264_SLICE_SPLIT_S {
    TS_BOOL bSplitEnable;                         /* RW; Range:[0, 1]; slice split enable, TS_TRUE:enable,
                                                    TS_FALSE:diable, default value:TS_FALSE */
    TS_U32  u32MbLineNum;                         /* RW; the max number is (Picture height + 15)/16;
                                                    this value presents the mb line number of one slice */
} VENC_H264_SLICE_SPLIT_S;

/* the param of the h264e intra pred */
typedef struct tsVENC_H264_INTRA_PRED_S {
    TS_U32     constrained_intra_pred_flag;       /* RW; Range:[0, 1];default: TS_FALSE,
                                                    see the H.264 protocol for the meaning */
} VENC_H264_INTRA_PRED_S;

/* the param of the h264e trans */
typedef struct tsVENC_H264_TRANS_S {
    TS_U32     u32IntraTransMode;               /* RW; Range:[0, 2]; Conversion mode for intra-prediction,
                                                    0: trans4x4, trans8x8; 1: trans4x4, 2: trans8x8 */
    TS_U32     u32InterTransMode;               /* RW; Range:[0, 2]; Conversion mode for inter-prediction,
                                                    0: trans4x4, trans8x8; 1: trans4x4, 2: trans8x8 */

    TS_BOOL    bScalingListValid;               /* RW; Range:[0, 1]; enable Scaling,default: TS_FALSE  */
    TS_U8      InterScalingList8X8[64];         /* RW; Range:[1, 255]; A quantization table for 8x8 inter-prediction */
    TS_U8      IntraScalingList8X8[64];         /* RW; Range:[1, 255]; A quantization table for 8x8 intra-prediction */

    TS_S32     chroma_qp_index_offset;          /* RW; Range:[-12, 12];default value: 0,
                                                    see the H.264 protocol for the meaning */
} VENC_H264_TRANS_S;

/* the param of the h264e entropy */
typedef struct tsVENC_H264_ENTROPY_S {
    TS_U32 u32EntropyEncModeI;                  /* RW; Range:[0, 1]; Entropy encoding mode for the I frame,
                                                    0:cavlc, 1:cabac */
    TS_U32 u32EntropyEncModeP;                  /* RW; Range:[0, 1]; Entropy encoding mode for the P frame,
                                                    0:cavlc, 1:cabac */
    TS_U32 u32EntropyEncModeB;                  /* RW; Range:[0, 1]; Entropy encoding mode for the B frame,
                                                    0:cavlc, 1:cabac */
    TS_U32 cabac_init_idc;                      /* RW; Range:[0, 2]; see the H.264 protocol for the meaning */
} VENC_H264_ENTROPY_S;

/* the config of the h264e poc */
typedef struct tsVENC_H264_POC_S {
    TS_U32 pic_order_cnt_type;                  /* RW; Range:[0, 2]; see the H.264 protocol for the meaning */

} VENC_H264_POC_S;

/* the param of the h264e deblocking */
typedef struct tsVENC_H264_DBLK_S {
    TS_U32 disable_deblocking_filter_idc;       /*  RW; Range:[0, 2]; see the H.264 protocol for the meaning */
    TS_S32 slice_alpha_c0_offset_div2;          /*  RW; Range:[-6, +6]; see the H.264 protocol for the meaning */
    TS_S32 slice_beta_offset_div2;              /*  RW; Range:[-6, +6]; see the H.264 protocol for the meaning */
} VENC_H264_DBLK_S;

/* the param of the h264e vui timing info */
typedef struct tsVENC_H264_VUI_TIME_INFO_S {
    TS_U8  timing_info_present_flag;          /* RW; Range:[0, 1];
                                                If 1, timing info belows will be encoded into vui.*/
    TS_U8  fixed_frame_rate_flag;             /* RW; Range:[0, 1];
                                                see the H.264 protocol for the meaning. */
    TS_U32 num_units_in_tick;                 /* RW; Range:(0, 4294967295];
                                                see the H.264 protocol for the meaning */
    TS_U32 time_scale;                        /* RW; Range:(0, 4294967295];
                                                see the H.264 protocol for the meaning */
} VENC_VUI_H264_TIME_INFO_S;

/* the param of the vui aspct ratio */
typedef struct tsVENC_VUI_ASPECT_RATIO_S {
    TS_U8  aspect_ratio_info_present_flag;    /* RW; Range:[0, 1]; If 1, aspectratio info belows
                                                will be encoded into vui */
    TS_U8  aspect_ratio_idc;                  /* RW; Range:[0, 255]; 17~254 is reserved,
                                                see the protocol for the meaning. */
    TS_U8  overscan_info_present_flag;        /* RW; Range:[0, 1]; If 1,
                                                oversacan info belows will be encoded into vui. */
    TS_U8  overscan_appropriate_flag;         /* RW; Range:[0, 1]; see the protocol for the meaning. */
    TS_U16 sar_width;                         /* RW; Range:(0, 65535]; see the protocol for the meaning. */
    TS_U16 sar_height ;                       /* RW; Range:(0, 65535]; see the protocol for the meaning.
                                                notes: sar_width  and  sar_height  shall  be  relatively  prime. */
} VENC_VUI_ASPECT_RATIO_S;

/* the param of the vui video signal */
typedef struct tsVENC_VUI_VIDEO_SIGNAL_S {
    TS_U8  video_signal_type_present_flag ;        /* RW; Range:[0, 1];
                                                    If 1, video singnal info will be encoded into vui */
    TS_U8  video_format ;                          /* RW; H.264e Range:[0, 7], H.265e Range:[0,5];
                                                    see the protocol for the meaning. */
    TS_U8  video_full_range_flag;                  /* RW; Range: [0, 1]; see the protocol for the meaning */
    TS_U8  colour_description_present_flag ;       /* RO; Range: [0, 1]; see the protocol for the meaning */
    TS_U8  colour_primaries ;                      /* RO; Range: [0, 255]; see the protocol for the meaning */
    TS_U8  transfer_characteristics;               /* RO; Range: [0, 255]; see the protocol for the meaning */
    TS_U8  matrix_coefficients;                    /* RO; Range:[0, 255]; see the protocol for the meaning */
} VENC_VUI_VIDEO_SIGNAL_S;

/* the param of the vui video signal */
typedef struct tsVENC_VUI_BITSTREAM_RESTRIC_S {
    TS_U8  bitstream_restriction_flag ;            /* RW; Range: [0, 1]; see the protocol for the meaning */
} VENC_VUI_BITSTREAM_RESTRIC_S;

/* the param of the h264e vui */
typedef struct tsVENC_H264_VUI_S {
    VENC_VUI_ASPECT_RATIO_S           stVuiAspectRatio;
    VENC_VUI_H264_TIME_INFO_S          stVuiTimeInfo;
    VENC_VUI_VIDEO_SIGNAL_S           stVuiVideoSignal;
    VENC_VUI_BITSTREAM_RESTRIC_S      stVuiBitstreamRestric;
} VENC_H264_VUI_S;

/* the param of the h265e vui timing info */
typedef struct tsVENC_VUI_H265_TIME_INFO_S {
    TS_U32 timing_info_present_flag;      /* RW; Range:[0, 1]; If 1, timing info belows will be encoded into vui. */
    TS_U32 num_units_in_tick;             /* RW; Range:[0, 4294967295]; see the H.265 protocol for the meaning. */
    TS_U32 time_scale;                    /* RW; Range:(0, 4294967295]; see the H.265 protocol for the meaning */
    TS_U32 num_ticks_poc_diff_one_minus1; /* RW; Range:(0, 4294967294]; see the H.265 protocol for the meaning */
} VENC_VUI_H265_TIME_INFO_S;

/* the param of the h265e vui */
typedef struct tsVENC_H265_VUI_S {
    VENC_VUI_ASPECT_RATIO_S        stVuiAspectRatio;
    VENC_VUI_H265_TIME_INFO_S     stVuiTimeInfo;
    VENC_VUI_VIDEO_SIGNAL_S       stVuiVideoSignal;
    VENC_VUI_BITSTREAM_RESTRIC_S  stVuiBitstreamRestric;
} VENC_H265_VUI_S;

/* the param of the jpege */
typedef struct tsVENC_JPEG_PARAM_S {
    TS_U32 u32Qfactor;                /* RW; Range:[1, 99]; Qfactor value  */
    TS_U8  u8YQt[64];                 /* RW; Range:[1, 255]; Y quantization table */
    TS_U8  u8CbQt[64];                /* RW; Range:[1, 255]; Cb quantization table */
    TS_U8  u8CrQt[64];                /* RW; Range:[1, 255]; Cr quantization table */
    TS_U32 u32MCUPerECS;              /* RW; the max MCU number is (picwidth + 15) >> 4 x (picheight + 15) >> 4 x 2];
                                        MCU number of one ECS*/
} VENC_JPEG_PARAM_S;

/* the param of the mjpege */
typedef struct tsVENC_MJPEG_PARAM_S {
    TS_U8 u8YQt[64];                  /* RW; Range:[1, 255]; Y quantization table */
    TS_U8 u8CbQt[64];                 /* RW; Range:[1, 255]; Cb quantization table */
    TS_U8 u8CrQt[64];                 /* RW; Range:[1, 255]; Cr quantization table */
    TS_U32 u32MCUPerECS;              /* RW; the max MCU number is (picwidth + 15) >> 4 x (picheight + 15) >> 4 x 2];
                                        MCU number of one ECS */
} VENC_MJPEG_PARAM_S;

/* the param of the ProRes */
typedef struct tsVENC_PRORES_PARAM_S {
    TS_U8 u8LumaQt[64];               /* RW; Range:[1, 255]; Luma quantization table */
    TS_U8 u8ChromaQt[64];             /* RW; Range:[1, 255]; Chroma quantization table */
    TS_CHAR encoder_identifier[4];    /* RW: identifies the encoder vendor or product that
                                        generated the compressed frame */
} VENC_PRORES_PARAM_S;

/* the attribute of the roi */
typedef struct tsVENC_ROI_ATTR_S {
    TS_U32  u32Index;                     /* RW; Range:[0, 7]; Index of an ROI.
                                            The system supports indexes ranging from 0 to 7 */
    TS_BOOL bEnable;                      /* RW; Range:[0, 1]; Whether to enable this ROI */
    TS_BOOL bAbsQp;                       /* RW; Range:[0, 1]; QP mode of an ROI.
                                            TS_FALSE: relative QP.TS_TURE: absolute QP */
    TS_S32  s32Qp;                        /* RW; Range:[-51, 51];
                                            QP value,only relative mode can QP value less than 0. */
    RECT_S  stRect;                       /* RW; Region of an ROI */
} VENC_ROI_ATTR_S;

/* ROI struct */
typedef struct tsVENC_ROI_ATTR_EX_S {
    TS_U32  u32Index;                     /* RW; Range:[0, 7]; Index of an ROI.
                                            The system supports indexes ranging from 0 to 7 */
    TS_BOOL bEnable[3];                   /* RW; Range:[0, 1]; Subscript of array
                                            0: I Frame; 1: P/B Frame; 2: VI Frame; other params are the same */
    TS_BOOL bAbsQp[3];                    /* RW; Range:[0, 1]; QP mode of an ROI.
                                            TS_FALSE: relative QP.TS_TURE: absolute QP */
    TS_S32  s32Qp[3];                     /* RW; Range:[-51, 51]; QP value,
                                            only relative mode can QP value less than 0 */
    RECT_S  stRect[3];                    /* RW;Region of an ROI */
}VENC_ROI_ATTR_EX_S;

/* the param of the roibg frame rate */
typedef struct tsVENC_ROIBG_FRAME_RATE_S {
    TS_S32 s32SrcFrmRate;                 /* RW; Range:[-1, 2147483647];Source frame rate of a non-ROI,
                                            can not be configured 0 */
    TS_S32 s32DstFrmRate;                 /* RW; Range:[-1, 2147483647];Target frame rate of a non-ROI,
                                            can not be larger than s32SrcFrmRate */
} VENC_ROIBG_FRAME_RATE_S;

/* the param of the roibg frame rate */
typedef struct tsVENC_REF_PARAM_S {
    TS_U32       u32Base;               /* RW; Range:[0, 4294967295]; Base layer period */
    TS_U32       u32Enhance;            /* RW; Range:[0, 255]; Enhance layer period */
    TS_BOOL      bEnablePred;           /* RW; Range:[0, 1]; Whether some frames at the base layer are referenced
                                            by other frames at the base layer. When bEnablePred is TS_FALSE,
                                            all frames at the base layer reference IDR frames */
} VENC_REF_PARAM_S;

/* Jpeg snap mode */
typedef enum tsVENC_JPEG_ENCODE_MODE_E {
    JPEG_ENCODE_ALL   = 0,                        /* Jpeg channel snap all the pictures when started. */
    JPEG_ENCODE_SNAP  = 1,                        /* Jpeg channel snap the flashed pictures when started. */
    JPEG_ENCODE_BUTT,
} VENC_JPEG_ENCODE_MODE_E;


/* the information of the stream */
typedef struct tsVENC_STREAM_BUF_INFO_S {
    TS_U64   u64PhyAddr[MAX_TILE_NUM];             /* R; Start physical address for a stream buffer */
    TS_VOID ATTRIBUTE* pUserAddr[MAX_TILE_NUM];    /* R; Start virtual address for a stream buffer */
    TS_U64  ATTRIBUTE u64BufSize[MAX_TILE_NUM];    /* R; Stream buffer size */
} VENC_STREAM_BUF_INFO_S;

/* the param of the h265e slice split */
typedef struct tsVENC_H265_SLICE_SPLIT_S {
    TS_BOOL bSplitEnable;                          /* RW; Range:[0, 1]; slice split enable,
                                                    TS_TRUE:enable, TS_FALSE:diable, default value:TS_FALSE */
    TS_U32  u32LcuLineNum;                         /* RW; Range:(Picture height + lcu size minus one)/lcu size;
                                                    this value presents lcu line number */
} VENC_H265_SLICE_SPLIT_S;

/* the param of the h265e pu */
typedef struct tsVENC_H265_PU_S {
    TS_U32    constrained_intra_pred_flag;         /* RW; Range:[0, 1]; see the H.265 protocol for the meaning. */
    TS_U32    strong_intra_smoothing_enabled_flag; /* RW; Range:[0, 1]; see the H.265 protocol for the meaning. */
} VENC_H265_PU_S;

/* the param of the h265e trans */
typedef struct tsVENC_H265_TRANS_S {
    TS_S32  cb_qp_offset;                     /* RW; Range:[-12, 12]; see the H.265 protocol for the meaning. */
    TS_S32  cr_qp_offset;                     /* RW; Range:[-12, 12]; see the H.265 protocol for the meaning. */

    TS_BOOL bScalingListEnabled;              /* RW; Range:[0, 1]; If 1, specifies that a scaling list is used. */

    TS_BOOL bScalingListTu4Valid;             /* RW; Range:[0, 1]; If 1, ScalingList4X4 belows will be encoded. */
    TS_U8   InterScalingList4X4[2][16];       /* RW; Range:[1, 255]; Scaling List for inter 4X4 block. */
    TS_U8   IntraScalingList4X4[2][16];       /* RW; Range:[1, 255]; Scaling List for intra 4X4 block. */

    TS_BOOL bScalingListTu8Valid;             /* RW; Range:[0, 1]; If 1, ScalingList8X8 belows will be encoded. */
    TS_U8   InterScalingList8X8[2][64];       /* RW; Range:[1, 255]; Scaling List for inter 8X8 block. */
    TS_U8   IntraScalingList8X8[2][64];       /* RW; Range:[1, 255]; Scaling List for intra 8X8 block. */

    TS_BOOL bScalingListTu16Valid;            /* RW; Range:[0, 1]; If 1, ScalingList16X16 belows will be encoded. */
    TS_U8   InterScalingList16X16[2][64];     /* RW; Range:[1, 255]; Scaling List for inter 16X16 block. */
    TS_U8   IntraScalingList16X16[2][64];     /* RW; Range:[1, 255]; Scaling List for inter 16X16 block. */

    TS_BOOL bScalingListTu32Valid;            /* RW; Range:[0, 1]; If 1, ScalingList32X32 belows will be encoded. */
    TS_U8   InterScalingList32X32[64];        /* RW; Range:[1, 255]; Scaling List for inter 32X32 block. */
    TS_U8   IntraScalingList32X32[64];        /* RW; Range:[1 ,255]; Scaling List for inter 32X32 block. */

} VENC_H265_TRANS_S;

/* the param of the h265e entroy */
typedef struct tsVENC_H265_ENTROPY_S {
    TS_U32 cabac_init_flag;                    /* RW; Range:[0, 1]; see the H.265 protocol for the meaning. */
} VENC_H265_ENTROPY_S;

/* the param of the h265e deblocking */
typedef struct tsVENC_H265_DBLK_S {
    TS_U32 slice_deblocking_filter_disabled_flag;   /* RW; Range:[0, 1]; see the H.265 protocol for the meaning. */
    TS_S32 slice_beta_offset_div2;                  /* RW; Range:[-6, 6]; see the H.265 protocol for the meaning. */
    TS_S32 slice_tc_offset_div2;                    /* RW; Range:[-6, 6]; see the H.265 protocol for the meaning. */
} VENC_H265_DBLK_S;

/* the param of the h265e sao */
typedef struct tsVENC_H265_SAO_S {
    TS_U32  slice_sao_luma_flag;      /* RW; Range:[0, 1]; Indicates whether SAO filtering is performed on the
                                        luminance component of the current slice. */
    TS_U32  slice_sao_chroma_flag;    /* RW; Range:[0, 1]; Indicates whether SAO filtering is performed on
                                        the chrominance component of the current slice */
} VENC_H265_SAO_S;

/* venc mode type */
typedef enum tsVENC_INTRA_REFRESH_MODE_E {
    INTRA_REFRESH_ROW = 0,                      /* Line mode */
    INTRA_REFRESH_COLUMN,                       /* Column mode */
    INTRA_REFRESH_BUTT
} VENC_INTRA_REFRESH_MODE_E;


/* the param of the intra refresh */
typedef struct tsVENC_INTRA_REFRESH_S {
    TS_BOOL                     bRefreshEnable;     /* RW; Range:[0, 1]; intra refresh enable,
                                                        TS_TRUE:enable, TS_FALSE:diable, default value:TS_FALSE */
    VENC_INTRA_REFRESH_MODE_E   enIntraRefreshMode; /* RW; The mode of intra refresh */
    TS_U32                      u32RefreshNum;      /* RW; Number of rows/column to be refreshed
                                                        during each I macroblock refresh */
    TS_U32                      u32ReqIQp;          /* RW; Range:[0, 51]; QP value of the I frame */
} VENC_INTRA_REFRESH_S;


/* default rate: [3mp] 170/85/70, [4mp] 230/110/85, [5mp] 290/160/120, [8mp] 500/300/170 */
typedef struct {
    TS_U32 u32Rate4Complex;      /* RW; Rate for static complex scene, may switch to u32Rate4Medium if pre qp of frame i is low enough */
    TS_U32 u32Rate4Medium;       /* RW; Rate for static medium scene, may switch to u32Rate4Simple if pre qp of frame i is low enough */
    TS_U32 u32Rate4Simple;       /* RW; Rate for static simple scene, may switch to u32Rate4Complex if pre qp of frame i is high enough */
} VENC_STATIC_RATE_CFG_S;

/* default qp threshold: 44/33/33 */
typedef struct {
    TS_U32 u32QpThr4Complex;    /* RW; For u32Rate4Medium & u32Rate4Simple, pre qp of frame i >= u32QpThr4Complex, switch to u32Rate4Complex */
    TS_U32 u32QpThr4Medium;     /* RW; For u32Rate4Simple, pre qp of frame i <= u32QpThr4Medium, switch to u32Rate4Medium */
    TS_U32 u32QpThr4Simple;     /* RW; For u32Rate4Medium, pre qp of frame i <= u32QpThr4Simple, switch to u32Rate4Simple */
} VENC_STATIC_QP_THR_CFG_S;

typedef struct {
    TS_U32 u32Valid;          /* RW; Range:[0, 1]; 1 - valid, 0 - invalid */
    TS_U32 u32SmartGopSize;      /* RW; GOP size when smart encode enabled */
    VENC_STATIC_RATE_CFG_S stRateCfg;    /* RW; Static scene rate config */
    VENC_STATIC_QP_THR_CFG_S stQpThrCfg; /* RW; Static rate switch threshold qp config */
} VENC_SMART_ENC_PARA_S;


/* venc mode type */
typedef enum tsVENC_MODTYPE_E {
    MODTYPE_VENC = 1,
    MODTYPE_H264E,
    MODTYPE_H265E,
    MODTYPE_JPEGE,
    MODTYPE_RC,
    MODTYPE_BUTT
} VENC_MODTYPE_E;

/* the param of the h264e mod */
typedef struct tsVENC_MOD_H264E_S {
    TS_U32          u32OneStreamBuffer;     /* RW; Range:[0, 1]; one stream buffer */
    TS_U32          u32H264eMiniBufMode;    /* RW; Range:[0, 1]; H264e MiniBufMode */
    TS_U32          u32H264ePowerSaveEn;    /* RW; Range:[0, 1]; H264e PowerSaveEn */
    VB_SOURCE_E     enH264eVBSource;        /* RW; H264e VBSource */
    TS_BOOL         bQpHstgrmEn;            /* RW; Range:[0, 1] */
    TS_U32          u32UserDataMaxLen;      /* RW; Range:[0, 65536]; one user data buffer len */
} VENC_MOD_H264E_S;

/* the param of the h265e mod */
typedef struct tsVENC_MOD_H265E_S {
    TS_U32          u32OneStreamBuffer;      /* RW; Range:[0, 1]; one stream buffer */
    TS_U32          u32H265eMiniBufMode;     /* RW; Range:[0, 1]; H265e MiniBufMode */
    TS_U32          u32H265ePowerSaveEn;     /* RW; Range:[0, 2]; H265e PowerSaveEn */
    VB_SOURCE_E     enH265eVBSource;         /* RW; H265e VBSource */
    TS_BOOL         bQpHstgrmEn;             /* RW; Range:[0, 1] */
    TS_U32          u32UserDataMaxLen;       /* RW; Range:[0, 65536]; one user data buffer len */
} VENC_MOD_H265E_S;

/* the param of the jpege mod */
typedef struct tsVENC_MOD_JPEGE_S {
    TS_U32  u32OneStreamBuffer;         /* RW; Range:[0, 1]; one stream buffer */
    TS_U32  u32JpegeMiniBufMode;        /* RW; Range:[0, 1]; Jpege MiniBufMode */
    TS_U32  u32JpegClearStreamBuf;      /* RW; Range:[0, 1]; JpegClearStreamBuf */
    TS_U32  u32JpegeDeringMode;         /* RW; Range:[0, 1]; Jpege Dering Mode */
} VENC_MOD_JPEGE_S;

typedef struct tsVENC_MOD_RC_S {
    TS_U32  u32ClrStatAfterSetBr;
} VENC_MOD_RC_S;
/* the param of the venc mod */
typedef struct tsVENC_MOD_VENC_S {
    TS_U32 u32VencBufferCache;  /* RW; Range:[0, 1]; VencBufferCache */
    TS_U32 u32FrameBufRecycle;  /* RW; Range:[0, 1]; FrameBufRecycle */
} VENC_MOD_VENC_S;

/* the param of the mod */
typedef struct tsVENC_MODPARAM_S {
    VENC_MODTYPE_E enVencModType;        /* RW; VencModType */
    union {
        VENC_MOD_VENC_S  stVencModParam;
        VENC_MOD_H264E_S stH264eModParam;
        VENC_MOD_H265E_S stH265eModParam;
        VENC_MOD_JPEGE_S stJpegeModParam;
        VENC_MOD_RC_S    stRcModParam;
    };
} VENC_PARAM_MOD_S;

typedef enum tsVENC_FRAME_TYPE_E {
    VENC_FRAME_TYPE_NONE = 1,
    VENC_FRAME_TYPE_IDR,
    VENC_FRAME_TYPE_PSKIP,
    VENC_FRAME_TYPE_BUTT
} VENC_FRAME_TYPE_E;

/* the information of the user rc */
typedef struct tsUSER_RC_INFO_S {
    TS_BOOL bQpMapValid;           /* RW; Range:[0, 1];
                                    Indicates whether the QpMap mode is valid for the current frame */
    TS_BOOL bSkipWeightValid;      /* RW; Range:[0, 1];
                                    Indicates whether the SkipWeight mode is valid for the current frame */
    TS_U32  u32BlkStartQp;         /* RW; Range:[0, 51]; QP value of the first 16 x 16 block in QpMap mode */
    TS_U64  u64QpMapPhyAddr;       /* RW; Physical address of the QP table in QpMap mode */
    TS_U64  u64SkipWeightPhyAddr;  /* RW; Physical address of the SkipWeight table in QpMap mode */
    VENC_FRAME_TYPE_E enFrameType; /* RW; Encoding frame type of the current frame */
} USER_RC_INFO_S;


/* the information of the user frame */
typedef struct tsUSER_FRAME_INFO_S {
    VIDEO_FRAME_INFO_S stUserFrame;
    USER_RC_INFO_S     stUserRcInfo;
} USER_FRAME_INFO_S;

/* the config of the sse */
typedef struct tsVENC_SSE_CFG_S {
    TS_U32  u32Index;       /* RW; Range:[0, 7]; Index of an SSE. The system supports indexes ranging from 0 to 7 */
    TS_BOOL bEnable;        /* RW; Range:[0, 1]; Whether to enable SSE */
    RECT_S  stRect;         /* RW; */
} VENC_SSE_CFG_S;


/* the param of the crop */
typedef struct tsVENC_CROP_INFO_S {
    TS_BOOL bEnable;                       /* RW; Range:[0, 1]; Crop region enable */
    RECT_S  stRect;                        /* RW; Crop region, note: s32X must be multi of 16 */
} VENC_CROP_INFO_S;

/* the param of the venc frame rate */
typedef struct tsVENC_FRAME_RATE_S {
    TS_S32 s32SrcFrmRate;                  /* RW; Range:[0, 240]; Input frame rate of a  channel */
    TS_S32 s32DstFrmRate;                  /* RW; Range:[0, 240]; Output frame rate of a channel */
} VENC_FRAME_RATE_S;

/* the param of the venc encode chnl */
typedef struct tsVENC_CHN_PARAM_S {
    TS_BOOL bColor2Grey;            /* RW; Range:[0, 1]; Whether to enable Color2Grey. */
    TS_U32  u32Priority;            /* RW; Range:[0, 1]; The priority of the coding chnl. */
    TS_U32  u32MaxStrmCnt;          /* RW: Range:[0, 4294967295]; Maximum number of frames in a stream buffer */
    TS_U32  u32PollWakeUpFrmCnt;    /* RW: Range:(0, 4294967295]; the frame num needed to wake up  obtaining streams */
    VENC_CROP_INFO_S    stCropCfg;
    VENC_FRAME_RATE_S   stFrameRate;
} VENC_CHN_PARAM_S;

/*the ground protect of FOREGROUND */
typedef struct tsVENC_FOREGROUND_PROTECT_S {
    TS_BOOL bForegroundCuRcEn;
    TS_U32  u32ForegroundDirectionThresh;              /* RW; Range:[0, 16];
                                                        The direction for controlling the macroblock-level bit rate */
    TS_U32  u32ForegroundThreshGain;                   /* RW; Range:[0, 15]; The gain of the thresh */
    TS_U32  u32ForegroundThreshOffset;                 /* RW; Range:[0, 255]; The offset of the thresh */
    TS_U32  u32ForegroundThreshP[RC_TEXTURE_THR_SIZE]; /* RW; Range:[0, 255]; Mad threshold for controlling
                                                        the foreground macroblock-level bit rate of P frames */
    TS_U32  u32ForegroundThreshB[RC_TEXTURE_THR_SIZE]; /* RW; Range:[0, 255]; Mad threshold for controlling
                                                        the foreground macroblock-level bit rate of B frames */
} VENC_FOREGROUND_PROTECT_S;

/* the scene mode of the venc encode chnl */
typedef enum tsVENC_SCENE_MODE_E {
    SCENE_0  = 0,              /* RW; A scene in which the camera does not move or periodically moves continuously */
    SCENE_1  = 1,              /* RW; Motion scene at high bit rate */
    SCENE_2  = 2,              /* RW; It has regular continuous motion at medium bit rate and
                                the encoding pressure is relatively large */
    SCENE_BUTT
} VENC_SCENE_MODE_E;


typedef struct tsVENC_DEBREATHEFFECT_S {
    TS_BOOL   bEnable;                 /* RW; Range:[0, 1];  default: 0, DeBreathEffect enable */
    TS_S32    s32Strength0;            /* RW; Range:[0, 35]; The Strength0 of DeBreathEffect. */
    TS_S32    s32Strength1;            /* RW; Range:[0, 35]; The Strength1 of DeBreathEffect. */
} VENC_DEBREATHEFFECT_S;

typedef struct tsVENC_CU_PREDICTION_S {
    OPERATION_MODE_E enPredMode;     /* RW; CU tendency configuration mode  */

    TS_U32 u32Intra32Cost;           /* RW; Range:[0, 15]; Tendency adjustment in Intra32 mode */
    TS_U32 u32Intra16Cost;           /* RW; Range:[0, 15]; Tendency adjustment in Intra16 mode */
    TS_U32 u32Intra8Cost;            /* RW; Range:[0, 15]; Tendency adjustment in Intra8 mode */
    TS_U32 u32Intra4Cost;            /* RW; Range:[0, 15]; Tendency adjustment in Intra4 mode */

    TS_U32 u32Inter64Cost;           /* RW; Range:[0, 15]; Tendency adjustment in Intra64 mode */
    TS_U32 u32Inter32Cost;           /* RW; Range:[0, 15]; Tendency adjustment in Inter32 mode */
    TS_U32 u32Inter16Cost;           /* RW; Range:[0, 15]; Tendency adjustment in Inter16 mode */
    TS_U32 u32Inter8Cost;            /* RW; Range:[0, 15]; Tendency adjustment in Inter8 mode */
} VENC_CU_PREDICTION_S;

typedef struct tsVENC_SKIP_BIAS_S {
    TS_BOOL bSkipBiasEn;             /* RW; Range:[0, 1];
                                        Flag indicating whether the skip tendency function is enabled */
    TS_U32  u32SkipThreshGain;       /* RW; Range:[0, 15];
                                        used to calculate the SAD threshold for foreground detection */
    TS_U32  u32SkipThreshOffset;     /* RW; Range:[0, 255];
                                        used to calculate the SAD threshold for foreground detection */
    TS_U32  u32SkipBackgroundCost;   /* RW; Range:[0, 15];
                                        Skip tendency adjustment in the background */
    TS_U32  u32SkipForegroundCost;   /* RW; Range:[0, 15];
                                        Skip tendency adjustment in the foreground */
} VENC_SKIP_BIAS_S;

typedef struct tsVENC_HIERARCtsCAL_QP_S {
    TS_BOOL     bHierarchicalQpEn;              /* RW; Range:[0, 1];    tserarctscal QP enable */
    TS_S32      s32HierarchicalQpDelta[4];      /* RW; Range:[-10, 10]; QP delta of the frames at each layer
                                                relative to the P-frame at layer 0 */
    TS_S32      s32HierarchicalFrameNum[4];     /* RW; Range:[0, 5];    Number of frames at each layer */
} VENC_HIERARCHICAL_QP_S;

typedef struct tsVENC_CHN_POOL_S {
    VB_POOL hPicVbPool;     /* RW;  vb pool id for pic buffer */
    VB_POOL hPicInfoVbPool; /* RW;  vb pool id for pic info buffer */
} VENC_CHN_POOL_S;

typedef struct tsVENC_RC_ADVPARAM_S {
    TS_U32 u32ClearStatAfterSetAttr; /* RW; Range:[0, 1]; Clear Stat After SetAttr enable */
} VENC_RC_ADVPARAM_S;

typedef struct {
    TS_BOOL bSplitEnable;
    TS_U32  u32SplitMode;
    TS_U32  u32SplitSize;
} VENC_SLICE_SPLIT_S;

typedef struct tsVENC_MOTION_DETECT_BOX_S {
	// the range of x and y is 0.0f~1.0f
	TS_FLOAT f32Xmin;
	TS_FLOAT f32Ymin;
	TS_FLOAT f32Xmax;
	TS_FLOAT f32Ymax;
	TS_FLOAT f32Score;
	TS_FLOAT f32Reserve;
} VENC_MOTION_DETECT_BOX_S;

typedef struct tsVENC_MOTION_DETECT_RESULT_S {
	TS_U32 u32ObjNum;
	VENC_MOTION_DETECT_BOX_S stBox[MAX_MOTION_DETECT_NUM];
} VENC_MOTION_DETECT_RESULT_S;

typedef struct tsVENC_MMS_INFO_S {
	TS_U32 len;
	TS_S16 *buf;
	TS_U32 bs_type;
} VENC_MMS_INFO_S;

/* callback define
 * toggle moment: one encoded frame be produced out
 * offer: comp's hdl
 * request: NewBufferHeader
 */
typedef void (* pf_filldone_callback)(void *hdl, int idx, int *pause);
typedef void (* pf_fillthisbuf_callback)(void *hdl, void *tmp);

typedef void* (* pf_alloc_callback)(void *hdl, int size);
typedef void (* pf_free_callback)(void *vAddr);

typedef void (* pf_emptydone_callback)(void *hdl, int idx);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __TS_COMM_VENC_H__ */

/*
 * Copyright © 2019 TSING MICRO.com.cn All Rights Reserved
 * Description: ts_comm_vgs.h
 * Author:TsingMicro multimedia software group
 * Create: 2022/06/02
 */

#ifndef __TS_COMM_VGS_H__
#define __TS_COMM_VGS_H__

#include "ts_type.h"
#include "ts_common.h"
#include "ts_errno.h"
#include "ts_comm_video.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/* failure caused by malloc buffer */
#define TS_ERR_VGS_NOBUF         TS_DEF_ERR(TS_ID_VGS, EN_ERR_LEVEL_ERROR, EN_ERR_NOBUF)
#define TS_ERR_VGS_BUF_EMPTY     TS_DEF_ERR(TS_ID_VGS, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_EMPTY)
#define TS_ERR_VGS_NULL_PTR      TS_DEF_ERR(TS_ID_VGS, EN_ERR_LEVEL_ERROR, EN_ERR_NULL_PTR)
#define TS_ERR_VGS_ILLEGAL_PARAM TS_DEF_ERR(TS_ID_VGS, EN_ERR_LEVEL_ERROR, EN_ERR_ILLEGAL_PARAM)
#define TS_ERR_VGS_BUF_FULL      TS_DEF_ERR(TS_ID_VGS, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_FULL)
#define TS_ERR_VGS_SYS_NOTREADY  TS_DEF_ERR(TS_ID_VGS, EN_ERR_LEVEL_ERROR, EN_ERR_SYS_NOTREADY)
#define TS_ERR_VGS_NOT_SUPPORT   TS_DEF_ERR(TS_ID_VGS, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_SUPPORT)
#define TS_ERR_VGS_NOT_PERMITTED TS_DEF_ERR(TS_ID_VGS, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_PERM)

#define VGS_PRIVATE_DATA_LEN 6

typedef TS_S32 VGS_HANDLE;

typedef enum tsVGS_COLOR_REVERT_MODE_E {
    VGS_COLOR_REVERT_NONE = 0, /* Not revert */
    VGS_COLOR_REVERT_RGB,      /* Revert RGB */
    VGS_COLOR_REVERT_ALPHA,    /* Revert alpha */
    VGS_COLOR_REVERT_BOTH,     /* Revert RGB and alpha */
    VGS_COLOR_REVERT_BUTT
} VGS_COLOR_REVERT_MODE_E;

typedef struct tsVGS_OSD_REVERT_S {
    RECT_S stSrcRect;                          /* OSD color revert area */
    VGS_COLOR_REVERT_MODE_E enColorRevertMode; /* OSD color revert mode */
} VGS_OSD_REVERT_S;

typedef struct tsVGS_TASK_ATTR_S {
    VIDEO_FRAME_INFO_S stImgIn;  /* Input picture */
    VIDEO_FRAME_INFO_S stImgOut; /* Output picture */
    TS_U64 au64PrivateData[4];   /* RW; Private data of task */
    TS_U32 reserved;             /* RW; Debug information,state of current picture */
} VGS_TASK_ATTR_S;

typedef struct tsVGS_DRAW_LINE_S {
    POINT_S stStartPoint; /* Line start point */
    POINT_S stEndPoint;   /* Line end point */
    TS_U32 u32Thick;      /* RW; Width of line */
    TS_U32 u32Color;      /* RW; Range: [0,0xFFFFFF]; Color of line */
} VGS_DRAW_LINE_S;

typedef enum tsVGS_COVER_TYPE_E {
    COVER_RECT = 0,    /* Retangle cover */
    COVER_QUAD_RANGLE, /* Quadrangle cover */
    COVER_BUTT
} VGS_COVER_TYPE_E;

typedef struct tsVGS_QUADRANGLE_COVER_S {
    TS_BOOL bSolid;     /* Solid or hollow cover */
    TS_U32 u32Thick;    /* RW; Range: [2,8]; Thick of the hollow quadrangle */
    POINT_S stPoint[4]; /* Four points of the quadrangle */
} VGS_QUADRANGLE_COVER_S;

typedef struct tsVGS_ADD_COVER_S {
    VGS_COVER_TYPE_E enCoverType; /* Cover type */
    union {
        RECT_S stDstRect;                    /* The rectangle area */
        VGS_QUADRANGLE_COVER_S stQuadRangle; /* The quadrangle area */
    };

    TS_U32 u32Color; /* RW; Range: [0,0xFFFFFF]; Color of cover */
} VGS_ADD_COVER_S;

typedef struct tsVGS_ADD_OSD_S {
    RECT_S stRect;                /* Osd area */
    TS_U32 u32BgColor;            /* RW; Background color of osd, depends on pixel format of osd,
                                    ARGB8888:[0,0xFFFFFFFF], ARGB4444:[0,0xFFFF], ARGB1555:[0,0x1FFF] */
    PIXEL_FORMAT_E enPixelFmt;    /* Pixel format of osd */
    TS_U64 u64PhyAddr;            /* RW; Physical address of osd */
    TS_U64 u64VirtAddr;            /* RW; Virt address of osd, special for Tsing */
    TS_U32 u32Stride;             /* RW; Range: [32,8192]; Stride of osd */
    TS_U32 u32BgAlpha;            /* RW; Range: [0,255]; Background alpha of osd */
    TS_U32 u32FgAlpha;            /* RW; Range: [0,255]; Foreground alpha of osd */
    TS_BOOL bOsdRevert;           /* RW; Enable osd color revert */
    VGS_OSD_REVERT_S stOsdRevert; /* Osd color revert information */
    TS_U16 u16ColorLUT[2];
} VGS_ADD_OSD_S;

typedef enum tsVGS_SCLCOEF_MODE_E {
    VGS_SCLCOEF_NORMAL = 0, /* normal scale coefficient */
    VGS_SCLCOEF_BUTT
} VGS_SCLCOEF_MODE_E;

#define HC_CV_VGS 1
#ifdef HC_CV_VGS
typedef struct tsVGS_SCALE_S {
    TS_U32 u32BgColor;      // set dst backgournd color
    RECT_S stRect;          // set rect pos
} VGS_SCALE_S;
typedef struct tsVGS_CROP_S {
    RECT_S stRect;          // set rect pos
    TS_U32 dstX;
    TS_U32 dstY;
} VGS_CROP_S;
#endif // HC_CV_VGS

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __TS_COMM_VGS_H__ */

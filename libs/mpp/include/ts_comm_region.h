/*
 * Copyright © 2019 TSING MICRO.com.cn All Rights Reserved
 * Description: ts_comm_region.h
 * Author:TsingMicro multimedia software group
 * Create: 2022/06/02
 */
#ifndef __TS_COMM_REGION_H__
#define __TS_COMM_REGION_H__

#include "ts_type.h"
#include "ts_defines.h"
#include "ts_common.h"
#include "ts_comm_video.h"
#include "ts_errno.h"
#include <pthread.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#if 0
#define COVER_MAX_NUM_VPSS  8
#define MOSAIC_MAX_NUM_VPSS  4
#define OVERLAYEX_MAX_NUM_ONE_CHN  8
#define COVEREX_MAX_NUM_ONE_CHN  8
#define MOSAICEX_MAX_NUM_ONE_CHN  8
#endif

typedef TS_U32 RGN_HANDLE;
typedef TS_U32 RGN_HANDLEGROUP;

/* type of video regions */
typedef enum tsRGN_TYPE_E {
	OVERLAY_RGN = 0, /* video overlay region */
	COVER_RGN,
	COVEREX_RGN,
	OVERLAYEX_RGN,
	MOSAIC_RGN,
	MOSAICEX_RGN,
	RGN_BUTT
} RGN_TYPE_E;

typedef struct tsMPP_GROUP_S {
	MOD_ID_E    enModId;
	TS_S32      s32GroupId;
} MPP_GROUP_S;

typedef enum tsINVERT_COLOR_MODE_E {
	LESSTHAN_LUM_THRESH = 0, /* the lum of the video is less than the lum threshold which is set by u32LumThresh  */
	MORETHAN_LUM_THRESH,     /* the lum of the video is more than the lum threshold which is set by u32LumThresh  */
	INVERT_COLOR_BUTT
} INVERT_COLOR_MODE_E;

typedef struct tsOVERLAY_QP_INFO_S {
	TS_BOOL bAbsQp;
	TS_S32 s32Qp;
	TS_BOOL bQpDisable;
} OVERLAY_QP_INFO_S;

typedef struct tsOVERLAY_INVERT_COLOR_S {
	SIZE_S stInvColArea;  // It must be multipe of 16 but not more than 64.
	TS_U32 u32LumThresh;  // The threshold to decide whether invert the OSD's color or not.
	INVERT_COLOR_MODE_E enChgMod;
	TS_BOOL bInvColEn;  // The switch of inverting color.
} OVERLAY_INVERT_COLOR_S;

typedef enum tsATTACH_DEST_E {
	ATTACH_JPEG_MAIN = 0,
	ATTACH_JPEG_MPF0,
	ATTACH_JPEG_MPF1,
	ATTACH_JPEG_BUTT
} ATTACH_DEST_E;

#define RGN_COLOR_LUT_NUM      2
#define RGN_MAX_BMP_UPDATE_NUM 16
//#define RGN_BATCHHANDLE_MAX    24

typedef struct tsOVERLAY_ATTR_S {
	/* bitmap pixel format,now only support ARGB1555 or ARGB4444 */
	PIXEL_FORMAT_E enPixelFmt;

	/* background color, pixel format depends on "enPixelFmt" */
	TS_U32 u32BgColor;

	/* region size,W:[2,RGN_OVERLAY_MAX_WIDTH],align:2,H:[2,RGN_OVERLAY_MAX_HEIGHT],align:2 */
	SIZE_S stSize;
	TS_U32 u32CanvasNum;
	} OVERLAY_ATTR_S;

	typedef struct tsOVERLAY_CHN_ATTR_S {
	/* X:[0,OVERLAY_MAX_X_VENC],align:2,Y:[0,OVERLAY_MAX_Y_VENC],align:2 */
	POINT_S stPoint;

	/* background an foreground transparence when pixel format is ARGB1555
	 * the pixel format is ARGB1555,when the alpha bit is 1 this alpha is value!
	 * range:[0,128]
	 */
	TS_U32 u32FgAlpha;

	/* background an foreground transparence when pixel format is ARGB1555
	 * the pixel format is ARGB1555,when the alpha bit is 0 this alpha is value!
	 * range:[0,128]
	 */
	TS_U32 u32BgAlpha;

	TS_U32 u32Layer; /* OVERLAY region layer range:[0,7] */

	OVERLAY_QP_INFO_S stQpInfo;

	OVERLAY_INVERT_COLOR_S stInvertColor;

	ATTACH_DEST_E enAttachDest;

	TS_U16 u16ColorLUT[RGN_COLOR_LUT_NUM];
} OVERLAY_CHN_ATTR_S;

typedef enum tsRGN_AREA_TYPE_E {
	AREA_RECT = 0,
	AREA_QUAD_RANGLE,
	AREA_BUTT
} RGN_AREA_TYPE_E;

typedef enum tsRGN_COORDINATE_E {
	RGN_ABS_COOR = 0, /* Absolute coordinate */
	RGN_RATIO_COOR    /* Ratio coordinate */
} RGN_COORDINATE_E;

typedef struct tsRGN_QUADRANGLE_S {
	TS_BOOL bSolid;     /* whether solid or dashed quadrangle */
	TS_U32 u32Thick;    /* Line Width of quadrangle, valid when dashed quadrangle */
	POINT_S stPoint[4]; /* points of quadrilateral */
} RGN_QUADRANGLE_S;

typedef struct tsCOVER_CHN_ATTR_S {
	RGN_AREA_TYPE_E enCoverType; /* rect or arbitary quadrilateral COVER */
	union {
	    RECT_S stRect;                 /* config of rect */
	    RGN_QUADRANGLE_S stQuadRangle; /* config of arbitary quadrilateral COVER */
	};
	TS_U32 u32Color;
	TS_U32 u32Layer;               /* COVER region layer */
	RGN_COORDINATE_E enCoordinate; /* ratio coordiante or abs coordinate */
} COVER_CHN_ATTR_S;

typedef struct tsCOVEREX_CHN_ATTR_S {
	RGN_AREA_TYPE_E enCoverType; /* rect or arbitary quadrilateral COVER */
	union {
	    RECT_S stRect;                 /* config of rect */
	    RGN_QUADRANGLE_S stQuadRangle; /* config of arbitary quadrilateral COVER */
	};
	TS_U32 u32Color;
	TS_U32 u32Layer; /* COVEREX region layer range */
} COVEREX_CHN_ATTR_S;

typedef enum tsMOSAIC_BLK_SIZE_E {
	MOSAIC_BLK_SIZE_8 = 0, /* block size 8*8 of MOSAIC */
	MOSAIC_BLK_SIZE_16,    /* block size 16*16 of MOSAIC */
	MOSAIC_BLK_SIZE_32,    /* block size 32*32 of MOSAIC */
	MOSAIC_BLK_SIZE_64,    /* block size 64*64 of MOSAIC */
	MOSAIC_BLK_SIZE_BUTT
} MOSAIC_BLK_SIZE_E;

typedef struct tsMOSAIC_CHN_ATTR_S {
	RECT_S stRect;               /* location of MOSAIC */
	MOSAIC_BLK_SIZE_E enBlkSize; /* block size of MOSAIC */
	TS_U32 u32Layer;             /* MOSAIC region layer range:[0,3] */
} MOSAIC_CHN_ATTR_S;

typedef struct tsMOSAICEX_CHN_ATTR_S {
	RECT_S stRect;               /* location of MOSAIC */
	MOSAIC_BLK_SIZE_E enBlkSize; /* block size of MOSAIC */
	TS_U32 u32Layer;             /* MOSAIC region layer range:[0,3] */
} MOSAICEX_CHN_ATTR_S;

typedef struct tsOVERLAYEX_COMM_ATTR_S {
	PIXEL_FORMAT_E enPixelFmt;

	/* background color, pixel format depends on "enPixelFmt" */
	TS_U32 u32BgColor;

	/* region size,W:[2,RGN_OVERLAY_MAX_WIDTH],align:2,H:[2,RGN_OVERLAY_MAX_HEIGHT],align:2 */
	SIZE_S stSize;
	TS_U32 u32CanvasNum;
} OVERLAYEX_ATTR_S;

typedef struct tsOVERLAYEX_CHN_ATTR_S {
	/* X:[0,RGN_OVERLAY_MAX_X],align:2,Y:[0,RGN_OVERLAY_MAX_Y],align:2 */
	POINT_S stPoint;

	/* background an foreground transparence when pixel format is ARGB1555
	 * the pixel format is ARGB1555,when the alpha bit is 1 this alpha is value!
	 * range:[0,255]
	 */
	TS_U32 u32FgAlpha;

	/* background an foreground transparence when pixel format is ARGB1555
	 * the pixel format is ARGB1555,when the alpha bit is 0 this alpha is value!
	 * range:[0,255]
	 */
	TS_U32 u32BgAlpha;

	TS_U32 u32Layer; /* OVERLAYEX region layer range:[0,15] */

	TS_U16 u16ColorLUT[RGN_COLOR_LUT_NUM];
} OVERLAYEX_CHN_ATTR_S;

typedef union tsRGN_ATTR_U {
	OVERLAY_ATTR_S stOverlay;     /* attribute of overlay region */
	OVERLAYEX_ATTR_S stOverlayEx; /* attribute of overlayex region */
} RGN_ATTR_U;

typedef union tsRGN_CHN_ATTR_U {
	OVERLAY_CHN_ATTR_S stOverlayChn;     /* attribute of overlay region */
	COVER_CHN_ATTR_S stCoverChn;         /* attribute of cover region */
	COVEREX_CHN_ATTR_S stCoverExChn;     /* attribute of coverex region */
	OVERLAYEX_CHN_ATTR_S stOverlayExChn; /* attribute of overlayex region */
	MOSAIC_CHN_ATTR_S stMosaicChn;       /* attribute of mosic region */
	MOSAICEX_CHN_ATTR_S stMosaicExChn;       /* attribute of mosic region */
} RGN_CHN_ATTR_U;

/* attribute of a region */
typedef struct tsRGN_ATTR_S {
	RGN_TYPE_E enType; /* region type */
	RGN_ATTR_U unAttr; /* region attribute */
} RGN_ATTR_S;

/* attribute of a region */
typedef struct tsRGN_CHN_ATTR_S {
	TS_BOOL bShow;
	RGN_TYPE_E enType;        /* region type */
	RGN_CHN_ATTR_U unChnAttr; /* region attribute */
} RGN_CHN_ATTR_S;

/* attribute of a region */
typedef struct tsRGN_GROUP_ATTR_S {
	TS_BOOL bShow;
	RGN_TYPE_E enType;        /* region type */
	RGN_CHN_ATTR_U unChnAttr; /* region attribute */
} RGN_GROUP_ATTR_S;

typedef struct tsRGN_BMP_UPDATE_S {
	POINT_S stPoint;
	BITMAP_S stBmp;
	TS_U32 u32Stride;
} RGN_BMP_UPDATE_S;

typedef struct tsRGN_BMP_UPDATE_CFG_S {
	TS_U32 u32BmpCnt;
	RGN_BMP_UPDATE_S astBmpUpdate[RGN_MAX_BMP_UPDATE_NUM];
} RGN_BMP_UPDATE_CFG_S;

typedef struct tsRGN_CANVAS_INFO_S {
	//TS_U32 u32PhyAddr;
	//TS_U32 *p32VirtAddr;
    TS_U64 u64PhyAddr;
    TS_U64 u64VirtAddr;
	SIZE_S stSize;
	TS_U32 u32Stride;
	PIXEL_FORMAT_E enPixelFmt;
	TS_U32 u32VbPoolId;
} RGN_CANVAS_INFO_S;

/* PingPong buffer change when set attr, it needs to remap memory in mpi interface */
#define TS_NOTICE_RGN_BUFFER_CHANGE TS_DEF_ERR(TS_ID_RGN, EN_ERR_LEVEL_NOTICE, TS_SUCCESS)

/* invlalid device ID */
#define TS_ERR_RGN_INVALID_DEVID TS_DEF_ERR(TS_ID_RGN, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_DEVID)
/* invlalid channel ID */
#define TS_ERR_RGN_INVALID_CHNID TS_DEF_ERR(TS_ID_RGN, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_CHNID)
/* at lease one parameter is illagal ,eg, an illegal enumeration value  */
#define TS_ERR_RGN_ILLEGAL_PARAM TS_DEF_ERR(TS_ID_RGN, EN_ERR_LEVEL_ERROR, EN_ERR_ILLEGAL_PARAM)
/* channel exists */
#define TS_ERR_RGN_EXIST TS_DEF_ERR(TS_ID_RGN, EN_ERR_LEVEL_ERROR, EN_ERR_EXIST)
/* UN exist */
#define TS_ERR_RGN_UNEXIST TS_DEF_ERR(TS_ID_RGN, EN_ERR_LEVEL_ERROR, EN_ERR_UNEXIST)
/* using a NULL point */
#define TS_ERR_RGN_NULL_PTR TS_DEF_ERR(TS_ID_RGN, EN_ERR_LEVEL_ERROR, EN_ERR_NULL_PTR)
/* try to enable or initialize system,device or channel, before configing attribute */
#define TS_ERR_RGN_NOT_CONFIG TS_DEF_ERR(TS_ID_RGN, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_CONFIG)
/* operation is not supported by NOW */
#define TS_ERR_RGN_NOT_SUPPORT TS_DEF_ERR(TS_ID_RGN, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_SUPPORT)
/* operation is not permitted ,eg, try to change stati attribute */
#define TS_ERR_RGN_NOT_PERM TS_DEF_ERR(TS_ID_RGN, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_PERM)
/* failure caused by malloc memory */
#define TS_ERR_RGN_NOMEM TS_DEF_ERR(TS_ID_RGN, EN_ERR_LEVEL_ERROR, EN_ERR_NOMEM)
/* failure caused by malloc buffer */
#define TS_ERR_RGN_NOBUF TS_DEF_ERR(TS_ID_RGN, EN_ERR_LEVEL_ERROR, EN_ERR_NOBUF)
/* no data in buffer */
#define TS_ERR_RGN_BUF_EMPTY TS_DEF_ERR(TS_ID_RGN, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_EMPTY)
/* no buffer for new data */
#define TS_ERR_RGN_BUF_FULL TS_DEF_ERR(TS_ID_RGN, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_FULL)
/* bad address, eg. used for copy_from_user & copy_to_user */
#define TS_ERR_RGN_BADADDR TS_DEF_ERR(TS_ID_RGN, EN_ERR_LEVEL_ERROR, EN_ERR_BADADDR)
/* resource is busy, eg. destroy a venc chn without unregistering it */
#define TS_ERR_RGN_BUSY TS_DEF_ERR(TS_ID_RGN, EN_ERR_LEVEL_ERROR, EN_ERR_BUSY)

/* System is not ready,maybe not initialed or loaded.
 * Returning the error code when opening a device file failed.
 */
#define TS_ERR_RGN_NOTREADY TS_DEF_ERR(TS_ID_RGN, EN_ERR_LEVEL_ERROR, EN_ERR_SYS_NOTREADY)


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef __TS_COMM_REGION_H__ */



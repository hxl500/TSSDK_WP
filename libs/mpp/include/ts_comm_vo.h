/*
 * Copyright © 2019 TSING MICRO.com.cn All Rights Reserved
 * Description: ts_comm_vo.h
 * Author:TsingMicro multimedia software group
 * Create: 2022/06/02
 */

#ifndef __TS_COMM_VO_H__
#define __TS_COMM_VO_H__

#include "ts_type.h"
#include "ts_common.h"
#include "ts_errno.h"
#include "ts_comm_video.h"
#include <pthread.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */
typedef enum tsVO_FORMAT_INFT_E
{
/* YUV semi-planar */
 VO_FORMAT_NV12,
 /* RGB16 */
 VO_FORMAT_RG16,
 VO_FORMAT_AR24
} VO_FORMAT_INFT_E;

typedef enum tsEN_VOU_ERR_CODE_E {
    EN_ERR_VO_DEV_NOT_CONFIG = 0x40,
    EN_ERR_VO_DEV_NOT_ENABLE = 0x41,
    EN_ERR_VO_DEV_HAS_ENABLED = 0x42,
    EN_ERR_VO_DEV_HAS_BINDED = 0x43,
    EN_ERR_VO_DEV_NOT_BINDED = 0x44,

    ERR_VO_NOT_ENABLE = 0x45,
    ERR_VO_NOT_DISABLE = 0x46,
    ERR_VO_NOT_CONFIG = 0x47,

    ERR_VO_CHN_NOT_DISABLE = 0x48,
    ERR_VO_CHN_NOT_ENABLE = 0x49,
    ERR_VO_CHN_NOT_CONFIG = 0x4a,
    ERR_VO_CHN_NOT_ALLOC = 0x4b,

    ERR_VO_CCD_INVALID_PAT = 0x4c,
    ERR_VO_CCD_INVALID_POS = 0x4d,

    ERR_VO_WAIT_TIMEOUT = 0x4e,
    ERR_VO_INVALID_VFRAME = 0x4f,
    ERR_VO_INVALID_RECT_PARA = 0x50,
    ERR_VO_SETBEGIN_ALREADY = 0x51,
    ERR_VO_SETBEGIN_NOTYET = 0x52,
    ERR_VO_SETEND_ALREADY = 0x53,
    ERR_VO_SETEND_NOTYET = 0x54,

    ERR_VO_GRP_INVALID_ID = 0x55,
    ERR_VO_GRP_NOT_CREATE = 0x56,
    ERR_VO_GRP_HAS_CREATED = 0x57,
    ERR_VO_GRP_NOT_DESTROY = 0x58,
    ERR_VO_GRP_CHN_FULL = 0x59,
    ERR_VO_GRP_CHN_EMPTY = 0x5a,
    ERR_VO_GRP_CHN_NOT_EMPTY = 0x5b,
    ERR_VO_GRP_INVALID_SYN_MODE = 0x5c,
    ERR_VO_GRP_INVALID_BASE_PTS = 0x5d,
    ERR_VO_GRP_NOT_START = 0x5e,
    ERR_VO_GRP_NOT_STOP = 0x5f,
    ERR_VO_GRP_INVALID_FRMRATE = 0x60,
    ERR_VO_GRP_CHN_HAS_REG = 0x61,
    ERR_VO_GRP_CHN_NOT_REG = 0x62,
    ERR_VO_GRP_CHN_NOT_UNREG = 0x63,
    ERR_VO_GRP_BASE_NOT_CFG = 0x64,

    ERR_GFX_NOT_DISABLE = 0x65,
    ERR_GFX_NOT_BIND = 0x66,
    ERR_GFX_NOT_UNBIND = 0x67,
    ERR_GFX_INVALID_ID = 0x68,

    ERR_VO_WBC_NOT_DISABLE = 0x69,
    ERR_VO_WBC_NOT_CONFIG = 0x6a,

    ERR_VO_CHN_AREA_OVERLAP = 0x6b,

    EN_ERR_INVALID_WBCID = 0x6c,
    EN_ERR_INVALID_LAYERID = 0x6d,
    EN_ERR_VO_VIDEO_HAS_BINDED = 0x6e,
    EN_ERR_VO_VIDEO_NOT_BINDED = 0x6f,
    ERR_VO_WBC_HAS_BIND = 0x70,
    ERR_VO_WBC_HAS_CONFIG = 0x71,
    ERR_VO_WBC_NOT_BIND = 0x72,

    /* new added */
    ERR_VO_BUTT
} EN_VOU_ERR_CODE_E;

/* System define error code */
#define TS_ERR_VO_BUSY                 TS_DEF_ERR(TS_ID_VO, EN_ERR_LEVEL_ERROR, EN_ERR_BUSY)
#define TS_ERR_VO_NO_MEM               TS_DEF_ERR(TS_ID_VO, EN_ERR_LEVEL_ERROR, EN_ERR_NOMEM)
#define TS_ERR_VO_NULL_PTR             TS_DEF_ERR(TS_ID_VO, EN_ERR_LEVEL_ERROR, EN_ERR_NULL_PTR)
#define TS_ERR_VO_SYS_NOTREADY         TS_DEF_ERR(TS_ID_VO, EN_ERR_LEVEL_ERROR, EN_ERR_SYS_NOTREADY)
#define TS_ERR_VO_INVALID_DEVID        TS_DEF_ERR(TS_ID_VO, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_DEVID)
#define TS_ERR_VO_INVALID_CHNID        TS_DEF_ERR(TS_ID_VO, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_CHNID)
#define TS_ERR_VO_ILLEGAL_PARAM        TS_DEF_ERR(TS_ID_VO, EN_ERR_LEVEL_ERROR, EN_ERR_ILLEGAL_PARAM)
#define TS_ERR_VO_NOT_SUPPORT          TS_DEF_ERR(TS_ID_VO, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_SUPPORT)
#define TS_ERR_VO_NOT_PERMIT           TS_DEF_ERR(TS_ID_VO, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_PERM)
#define TS_ERR_VO_INVALID_WBCID        TS_DEF_ERR(TS_ID_VO, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_WBCID)
#define TS_ERR_VO_INVALID_LAYERID      TS_DEF_ERR(TS_ID_VO, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_LAYERID)

/* Device relative error code */
#define TS_ERR_VO_DEV_NOT_CONFIG       TS_DEF_ERR(TS_ID_VO, EN_ERR_LEVEL_ERROR, EN_ERR_VO_DEV_NOT_CONFIG)
#define TS_ERR_VO_DEV_NOT_ENABLE       TS_DEF_ERR(TS_ID_VO, EN_ERR_LEVEL_ERROR, EN_ERR_VO_DEV_NOT_ENABLE)
#define TS_ERR_VO_DEV_HAS_ENABLED      TS_DEF_ERR(TS_ID_VO, EN_ERR_LEVEL_ERROR, EN_ERR_VO_DEV_HAS_ENABLED)
#define TS_ERR_VO_DEV_HAS_BINDED       TS_DEF_ERR(TS_ID_VO, EN_ERR_LEVEL_ERROR, EN_ERR_VO_DEV_HAS_BINDED)
#define TS_ERR_VO_DEV_NOT_BINDED       TS_DEF_ERR(TS_ID_VO, EN_ERR_LEVEL_ERROR, EN_ERR_VO_DEV_NOT_BINDED)

/* Video layer relative error code */
#define TS_ERR_VO_VIDEO_NOT_ENABLE     TS_DEF_ERR(TS_ID_VO, EN_ERR_LEVEL_ERROR, ERR_VO_NOT_ENABLE)
#define TS_ERR_VO_VIDEO_NOT_DISABLE    TS_DEF_ERR(TS_ID_VO, EN_ERR_LEVEL_ERROR, ERR_VO_NOT_DISABLE)
#define TS_ERR_VO_VIDEO_NOT_CONFIG     TS_DEF_ERR(TS_ID_VO, EN_ERR_LEVEL_ERROR, ERR_VO_NOT_CONFIG)
#define TS_ERR_VO_VIDEO_HAS_BINDED     TS_DEF_ERR(TS_ID_VO, EN_ERR_LEVEL_ERROR, EN_ERR_VO_VIDEO_HAS_BINDED)
#define TS_ERR_VO_VIDEO_NOT_BINDED     TS_DEF_ERR(TS_ID_VO, EN_ERR_LEVEL_ERROR, EN_ERR_VO_VIDEO_NOT_BINDED)

/* WBC Relative error code */
#define TS_ERR_VO_WBC_NOT_DISABLE      TS_DEF_ERR(TS_ID_VO, EN_ERR_LEVEL_ERROR, ERR_VO_WBC_NOT_DISABLE)
#define TS_ERR_VO_WBC_NOT_CONFIG       TS_DEF_ERR(TS_ID_VO, EN_ERR_LEVEL_ERROR, ERR_VO_WBC_NOT_CONFIG)
#define TS_ERR_VO_WBC_HAS_CONFIG       TS_DEF_ERR(TS_ID_VO, EN_ERR_LEVEL_ERROR, ERR_VO_WBC_HAS_CONFIG)
#define TS_ERR_VO_WBC_NOT_BIND         TS_DEF_ERR(TS_ID_VO, EN_ERR_LEVEL_ERROR, ERR_VO_WBC_NOT_BIND)
#define TS_ERR_VO_WBC_HAS_BIND         TS_DEF_ERR(TS_ID_VO, EN_ERR_LEVEL_ERROR, ERR_VO_WBC_HAS_BIND)

/* Channel Relative error code */
#define TS_ERR_VO_CHN_NOT_DISABLE      TS_DEF_ERR(TS_ID_VO, EN_ERR_LEVEL_ERROR, ERR_VO_CHN_NOT_DISABLE)
#define TS_ERR_VO_CHN_NOT_ENABLE       TS_DEF_ERR(TS_ID_VO, EN_ERR_LEVEL_ERROR, ERR_VO_CHN_NOT_ENABLE)
#define TS_ERR_VO_CHN_NOT_CONFIG       TS_DEF_ERR(TS_ID_VO, EN_ERR_LEVEL_ERROR, ERR_VO_CHN_NOT_CONFIG)
#define TS_ERR_VO_CHN_NOT_ALLOC        TS_DEF_ERR(TS_ID_VO, EN_ERR_LEVEL_ERROR, ERR_VO_CHN_NOT_ALLOC)
#define TS_ERR_VO_CHN_AREA_OVERLAP     TS_DEF_ERR(TS_ID_VO, EN_ERR_LEVEL_ERROR, ERR_VO_CHN_AREA_OVERLAP)

/* Cascade Relatvie error code */
#define TS_ERR_VO_INVALID_PATTERN      TS_DEF_ERR(TS_ID_VO, EN_ERR_LEVEL_ERROR, ERR_VO_CCD_INVALID_PAT)
#define TS_ERR_VO_INVALID_POSITION     TS_DEF_ERR(TS_ID_VO, EN_ERR_LEVEL_ERROR, ERR_VO_CCD_INVALID_POS)

/* MISCellaneous error code */
#define TS_ERR_VO_WAIT_TIMEOUT         TS_DEF_ERR(TS_ID_VO, EN_ERR_LEVEL_ERROR, ERR_VO_WAIT_TIMEOUT)
#define TS_ERR_VO_INVALID_VFRAME       TS_DEF_ERR(TS_ID_VO, EN_ERR_LEVEL_ERROR, ERR_VO_INVALID_VFRAME)
#define TS_ERR_VO_INVALID_RECT_PARA    TS_DEF_ERR(TS_ID_VO, EN_ERR_LEVEL_ERROR, ERR_VO_INVALID_RECT_PARA)
#define TS_ERR_VO_SETBEGIN_ALREADY     TS_DEF_ERR(TS_ID_VO, EN_ERR_LEVEL_ERROR, ERR_VO_SETBEGIN_ALREADY)
#define TS_ERR_VO_SETBEGIN_NOTYET      TS_DEF_ERR(TS_ID_VO, EN_ERR_LEVEL_ERROR, ERR_VO_SETBEGIN_NOTYET)
#define TS_ERR_VO_SETEND_ALREADY       TS_DEF_ERR(TS_ID_VO, EN_ERR_LEVEL_ERROR, ERR_VO_SETEND_ALREADY)
#define TS_ERR_VO_SETEND_NOTYET        TS_DEF_ERR(TS_ID_VO, EN_ERR_LEVEL_ERROR, ERR_VO_SETEND_NOTYET)

/* Synchronization group relative error code */
#define TS_ERR_VO_GRP_INVALID_ID       TS_DEF_ERR(TS_ID_VO, EN_ERR_LEVEL_ERROR, ERR_VO_GRP_INVALID_ID)
#define TS_ERR_VO_GRP_NOT_CREATE       TS_DEF_ERR(TS_ID_VO, EN_ERR_LEVEL_ERROR, ERR_VO_GRP_NOT_CREATE)
#define TS_ERR_VO_GRP_HAS_CREATED      TS_DEF_ERR(TS_ID_VO, EN_ERR_LEVEL_ERROR, ERR_VO_GRP_HAS_CREATED)
#define TS_ERR_VO_GRP_NOT_DESTROY      TS_DEF_ERR(TS_ID_VO, EN_ERR_LEVEL_ERROR, ERR_VO_GRP_NOT_DESTROY)
#define TS_ERR_VO_GRP_CHN_FULL         TS_DEF_ERR(TS_ID_VO, EN_ERR_LEVEL_ERROR, ERR_VO_GRP_CHN_FULL)
#define TS_ERR_VO_GRP_CHN_EMPTY        TS_DEF_ERR(TS_ID_VO, EN_ERR_LEVEL_ERROR, ERR_VO_GRP_CHN_EMPTY)
#define TS_ERR_VO_GRP_CHN_NOT_EMPTY    TS_DEF_ERR(TS_ID_VO, EN_ERR_LEVEL_ERROR, ERR_VO_GRP_CHN_NOT_EMPTY)
#define TS_ERR_VO_GRP_INVALID_SYN_MODE TS_DEF_ERR(TS_ID_VO, EN_ERR_LEVEL_ERROR, ERR_VO_GRP_INVALID_SYN_MODE)
#define TS_ERR_VO_GRP_INVALID_BASE_PTS TS_DEF_ERR(TS_ID_VO, EN_ERR_LEVEL_ERROR, ERR_VO_GRP_INVALID_BASE_PTS)
#define TS_ERR_VO_GRP_NOT_START        TS_DEF_ERR(TS_ID_VO, EN_ERR_LEVEL_ERROR, ERR_VO_GRP_NOT_START)
#define TS_ERR_VO_GRP_NOT_STOP         TS_DEF_ERR(TS_ID_VO, EN_ERR_LEVEL_ERROR, ERR_VO_GRP_NOT_STOP)
#define TS_ERR_VO_GRP_INVALID_FRMRATE  TS_DEF_ERR(TS_ID_VO, EN_ERR_LEVEL_ERROR, ERR_VO_GRP_INVALID_FRMRATE)
#define TS_ERR_VO_GRP_CHN_HAS_REG      TS_DEF_ERR(TS_ID_VO, EN_ERR_LEVEL_ERROR, ERR_VO_GRP_CHN_HAS_REG)
#define TS_ERR_VO_GRP_CHN_NOT_REG      TS_DEF_ERR(TS_ID_VO, EN_ERR_LEVEL_ERROR, ERR_VO_GRP_CHN_NOT_REG)
#define TS_ERR_VO_GRP_CHN_NOT_UNREG    TS_DEF_ERR(TS_ID_VO, EN_ERR_LEVEL_ERROR, ERR_VO_GRP_CHN_NOT_UNREG)
#define TS_ERR_VO_GRP_BASE_NOT_CFG     TS_DEF_ERR(TS_ID_VO, EN_ERR_LEVEL_ERROR, ERR_VO_GRP_BASE_NOT_CFG)

/* Graphics layer relative error code */
#define TS_ERR_VO_GFX_NOT_DISABLE      TS_DEF_ERR(TS_ID_VO, EN_ERR_LEVEL_ERROR, ERR_GFX_NOT_DISABLE)
#define TS_ERR_VO_GFX_NOT_BIND         TS_DEF_ERR(TS_ID_VO, EN_ERR_LEVEL_ERROR, ERR_GFX_NOT_BIND)
#define TS_ERR_VO_GFX_NOT_UNBIND       TS_DEF_ERR(TS_ID_VO, EN_ERR_LEVEL_ERROR, ERR_GFX_NOT_UNBIND)
#define TS_ERR_VO_GFX_INVALID_ID       TS_DEF_ERR(TS_ID_VO, EN_ERR_LEVEL_ERROR, ERR_GFX_INVALID_ID)

#define TS_ERR_VO_COMP_INIT            TS_DEF_ERR(TS_ID_VO, EN_ERR_LEVEL_ERROR, EN_ERR_COMP_INIT)
#define TS_ERR_VO_COMP_GET            TS_DEF_ERR(TS_ID_VO, EN_ERR_LEVEL_ERROR, EN_ERR_COMP_GET)
#define TS_ERR_VO_COMP_INTERNAL       TS_DEF_ERR(TS_ID_VO, EN_ERR_LEVEL_ERROR, EN_ERR_COMP_INTERNAL)

/* VO video output interface type */
#if 1
#define VO_INTF_CVBS                   (0x01L << 0)
#define VO_INTF_YPBPR                  (0x01L << 1)
#define VO_INTF_VGA                    (0x01L << 2)
#define VO_INTF_BT656                  (0x01L << 3)
#define VO_INTF_BT1120                 (0x01L << 4)
#endif
#define VO_INTF_HDMI                   (0x01L << 5)
#if 1
#define VO_INTF_LCD                    (0x01L << 6)
#define VO_INTF_BT656_H                (0x01L << 7)
#define VO_INTF_BT656_L                (0x01L << 8)
#define VO_INTF_LCD_6BIT               (0x01L << 9)
#define VO_INTF_LCD_8BIT               (0x01L << 10)
#define VO_INTF_LCD_16BIT              (0x01L << 11)
#define VO_INTF_LCD_18BIT              (0x01L << 12)
#define VO_INTF_LCD_24BIT              (0x01L << 13)
#endif
#define VO_INTF_MIPI                   (0x01L << 14)
#if 1
#define VO_INTF_MIPI_SLAVE             (0x01L << 15)
#endif

typedef TS_U32 VO_INTF_TYPE_E;

typedef enum tsVO_INTF_SYNC_E {
	#if 1
    VO_OUTPUT_PAL = 0, /* PAL standard */
    VO_OUTPUT_NTSC, /* NTSC standard */

    VO_OUTPUT_1080P24, /* 1920 x 1080 at 24 Hz. */
    VO_OUTPUT_1080P25, /* 1920 x 1080 at 25 Hz. */
    VO_OUTPUT_1080P30, /* 1920 x 1080 at 30 Hz. */

    VO_OUTPUT_720P50, /* 1280 x  720 at 50 Hz. */
	#endif

    VO_OUTPUT_720P60, /* 1280 x  720 at 60 Hz. */
    VO_OUTPUT_768P60, /* 1280 x  768 at 60 Hz. */

	#if 1
    VO_OUTPUT_1080I50, /* 1920 x 1080 at 50 Hz, interlace. */
    VO_OUTPUT_1080I60, /* 1920 x 1080 at 60 Hz, interlace. */
    VO_OUTPUT_1080P50, /* 1920 x 1080 at 50 Hz. */
	#endif

    VO_OUTPUT_1080P60, /* 1920 x 1080 at 60 Hz. */

	#if 1
    VO_OUTPUT_576P50, /* 720  x  576 at 50 Hz. */
    VO_OUTPUT_480P60, /* 720  x  480 at 60 Hz. */

    VO_OUTPUT_800x600_60, /* VESA 800 x 600 at 60 Hz (non-interlaced) */
    VO_OUTPUT_1024x768_60, /* VESA 1024 x 768 at 60 Hz (non-interlaced) */
    VO_OUTPUT_1280x1024_60, /* VESA 1280 x 1024 at 60 Hz (non-interlaced) */
    VO_OUTPUT_1366x768_60, /* VESA 1366 x 768 at 60 Hz (non-interlaced) */
    VO_OUTPUT_1440x900_60, /* VESA 1440 x 900 at 60 Hz (non-interlaced) CVT Compliant */
    VO_OUTPUT_1280x800_60, /* 1280*800@60Hz VGA@60Hz */
    VO_OUTPUT_1600x1200_60, /* VESA 1600 x 1200 at 60 Hz (non-interlaced) */
    VO_OUTPUT_1680x1050_60, /* VESA 1680 x 1050 at 60 Hz (non-interlaced) */
    VO_OUTPUT_1920x1200_60, /* VESA 1920 x 1600 at 60 Hz (non-interlaced) CVT (Reduced Blanking) */
    VO_OUTPUT_640x480_60, /* VESA 640 x 480 at 60 Hz (non-interlaced) CVT */
    VO_OUTPUT_960H_PAL, /* ITU-R BT.1302 960 x 576 at 50 Hz (interlaced) */
    VO_OUTPUT_960H_NTSC, /* ITU-R BT.1302 960 x 480 at 60 Hz (interlaced) */
    VO_OUTPUT_1920x2160_30, /* 1920x2160_30 */
    VO_OUTPUT_2560x1440_30, /* 2560x1440_30 */
    VO_OUTPUT_2560x1440_60, /* 2560x1440_60 */
    VO_OUTPUT_2560x1600_60, /* 2560x1600_60 */
    VO_OUTPUT_3840x2160_24, /* 3840x2160_24 */
    VO_OUTPUT_3840x2160_25, /* 3840x2160_25 */
    VO_OUTPUT_3840x2160_30, /* 3840x2160_30 */
    VO_OUTPUT_3840x2160_50, /* 3840x2160_50 */
    VO_OUTPUT_3840x2160_60, /* 3840x2160_60 */
    VO_OUTPUT_4096x2160_24, /* 4096x2160_24 */
    VO_OUTPUT_4096x2160_25, /* 4096x2160_25 */
    VO_OUTPUT_4096x2160_30, /* 4096x2160_30 */
    VO_OUTPUT_4096x2160_50, /* 4096x2160_50 */
    VO_OUTPUT_4096x2160_60, /* 4096x2160_60 */
    VO_OUTPUT_320x240_60, /* For ota5182 at 60 Hz (8bit)  */
    VO_OUTPUT_320x240_50, /* For ili9342 at 50 Hz (6bit)  */
    VO_OUTPUT_240x320_50, /* For ili9341 at 50 Hz (6bit) For st7789 at 50Hz(6bit) */
    VO_OUTPUT_240x320_60, /* For ili9341 at 60 Hz (16bit) */
    VO_OUTPUT_800x600_50, /* For LCD     at 50 Hz (24bit) */
    VO_OUTPUT_720x1280_60, /* For MIPI DSI Tx 720 x1280 at 60 Hz */
    VO_OUTPUT_1080x1920_60, /* For MIPI DSI Tx 1080x1920 at 60 Hz */
    VO_OUTPUT_7680x4320_30, /* For HDMI2.1 at 30 Hz         */
    VO_OUTPUT_USER, /* User timing. */
	#endif

    VO_OUTPUT_BUTT
} VO_INTF_SYNC_E;

#if 1//Period 2
typedef enum tsVO_ZOOM_IN_E {
    VO_ZOOM_IN_RECT = 0, /* Zoom in by rect */
    VO_ZOOM_IN_RATIO, /* Zoom in by ratio */
    VO_ZOOM_IN_BUTT
} VO_ZOOM_IN_E;
#endif

typedef enum tsVO_CSC_MATRIX_E {
    VO_CSC_MATRIX_IDENTITY = 0, /* Identity CSC matrix. */

	#if 1//To be confirmed：无效参数，只是为了方便复用sample接口
    VO_CSC_MATRIX_BT601_TO_BT709, /* BT601 to BT709 */
    VO_CSC_MATRIX_BT709_TO_BT601, /* BT709 to BT601 */

    VO_CSC_MATRIX_BT601_TO_RGB_PC, /* BT601 to RGB */
    VO_CSC_MATRIX_BT709_TO_RGB_PC, /* BT709 to RGB */

    VO_CSC_MATRIX_RGB_TO_BT601_PC, /* RGB to BT601 FULL */
    VO_CSC_MATRIX_RGB_TO_BT709_PC, /* RGB to BT709 FULL */

    VO_CSC_MATRIX_RGB_TO_BT2020_PC, /* RGB to BT.2020 */
    VO_CSC_MATRIX_BT2020_TO_RGB_PC, /* BT.2020 to RGB */

    VO_CSC_MATRIX_RGB_TO_BT601_TV, /* RGB to BT601 LIMIT */
    VO_CSC_MATRIX_RGB_TO_BT709_TV, /* RGB to BT709 LIMIT */
	#endif

    VO_CSC_MATRIX_BUTT
} VO_CSC_MATRIX_E;

typedef struct tsVO_CHN_ATTR_S {
    TS_U32 u32Priority; /* Video out overlay pri sd */
    RECT_S stRect; /* Rectangle of video output channel */
    TS_BOOL bDeflicker; /* To be confirmed */ /* Deflicker or not sd */
} VO_CHN_ATTR_S;


typedef struct tsVO_CHN_PARAM_S {
    ASPECT_RATIO_S stAspectRatio; /* RW; aspect ratio */
} VO_CHN_PARAM_S;

#if 1//Period 2
typedef struct tsVO_BORDER_S {
    TS_BOOL bBorderEn; /* RW; Do frame or not */
    BORDER_S stBorder; /* RW; frame's top, bottom, left, right width and color */
} VO_BORDER_S;

typedef struct tsVO_QUERY_STATUS_S {
    TS_U32 u32ChnBufUsed; /* Channel buffer that been occupied */
} VO_QUERY_STATUS_S;
#endif

typedef struct tsVO_SYNC_INFO_S {
    TS_BOOL bSynm;      /* RW; sync mode(0:timing,as BT.656; 1:signal,as LCD) */
    TS_BOOL bIop;       /* RW; interlaced or progressive display(0:i; 1:p) */
    TS_U8 u8Intfb;      /* RW; interlace bit width while output */
    
    TS_UL  pixelclock;	/* pixelclock in Hz */
    TS_U16 u16Vact;     /* RW; vertical active area */
    TS_U16 u16Vbb;      /* RW; vertical back blank porch */
    TS_U16 u16Vfb;      /* RW; vertical front blank porch */

    TS_U16 u16Hact;     /* RW; horizontal active area */
    TS_U16 u16Hbb;      /* RW; horizontal back blank porch */
    TS_U16 u16Hfb;      /* RW; horizontal front blank porch */
    TS_U16 u16Hmid;     /* RW; bottom horizontal active area */

    TS_U16 u16Bvact;    /* RW; bottom vertical active area */
    TS_U16 u16Bvbb;     /* RW; bottom vertical back blank porch */
    TS_U16 u16Bvfb;     /* RW; bottom vertical front blank porch */

    TS_U16 u16Hpw;      /* RW; horizontal pulse width */
    TS_U16 u16Vpw;      /* RW; vertical pulse width */

    TS_BOOL bIdv;       /* RW; inverse data valid of output */
    TS_BOOL bIhs;       /* RW; inverse horizontal synch signal */
    TS_BOOL bIvs;       /* RW; inverse vertical synch signal */
} VO_SYNC_INFO_S;

typedef struct tsVO_PUB_ATTR_S {
    TS_U32 u32BgColor;          /* RW; Background color of a device, in RGB format. */
    VO_INTF_TYPE_E enIntfType; /* RW; Type of a VO interface */
    VO_INTF_SYNC_E enIntfSync; /* RW; Type of a VO interface timing */
    VO_SYNC_INFO_S stSyncInfo; /* RW; Information about VO interface timings */
    VO_FORMAT_INFT_E format;
} VO_PUB_ATTR_S;

typedef struct tsVO_LAYER_RESIZE_S
{ 
    SIZE_S     image_size;
	TS_BOOL    refresh;
} VO_LAYER_RESIZE_S;

typedef enum tsFLIP_E {
	FLIP_X	 = 1,
	FLIP_Y  = 2,
	FLIP_X_BUTT
} FLIP_E;

typedef struct tsVO_LAYER_FLIP_S
{
    FLIP_E     flip;
	TS_BOOL    refresh;
} VO_LAYER_FLIP_S;

typedef struct tsVO_DEV_GAMMA_S
{
    TS_U8   gamma[255];
	TS_BOOL refresh;
} VO_DEV_GAMMA_S;

typedef struct tsVO_FAKE_S {

} VO_FAKE_S;

//无效参数，只是为了方便复用SAMPLE_COMM_VO_GetDefConfig
typedef enum tsVO_PART_MODE_E {
    VO_PART_MODE_SINGLE = 0, /* single partition, which use software to make multi-picture in one hardware cell */
    VO_PART_MODE_MULTI = 1, /* muliti partition, each partition is a hardware cell */
    VO_PART_MODE_BUTT
} VO_PART_MODE_E;

typedef struct tsVO_VIDEO_LAYER_ATTR_S {
    RECT_S stDispRect; /* RW; Display resolution */
    SIZE_S stImageSize; /* RW; Canvas size of the video layer */
    TS_U32 u32DispFrmRt; /* RW; Display frame rate */
    PIXEL_FORMAT_E enPixFormat; /* RW; Pixel format of the video layer */
	#if 1//无效参数，只是为了方便复用sample接口
    TS_BOOL bDoubleFrame; /* RW; Whether to double frames */
    TS_BOOL bClusterMode; /* RW; Whether to take Cluster way to use memory */
	#endif
    DYNAMIC_RANGE_E enDstDynamicRange; /* RW; Video Layer output dynamic range type */
} VO_VIDEO_LAYER_ATTR_S;

typedef struct tsVO_LAYER_PARAM_S {
    ASPECT_RATIO_S stAspectRatio; /* RW; aspect ratio */
} VO_LAYER_PARAM_S;

#if 1//Period 2
typedef struct tsVO_ZOOM_RATIO_S {
    TS_U32 u32XRatio; /* RW; Range: [0, 1000]; XRatio = x * 1000 / W,
                              x means the start point to be zoomed, W means displaying channel's width. */
    TS_U32 u32YRatio; /* RW; Range: [0, 1000]; YRatio = y * 1000 / H,
                              y means the start point to be zoomed, H means displaying channel's height. */
    TS_U32 u32WRatio; /* RW; Range: [0, 1000]; WRatio = w * 1000 / W,
                              w means the width to be zoomed, W means displaying channel's width. */
    TS_U32 u32HRatio; /* RW; Range: [0, 1000]; HRatio = h * 1000 / H,
                              h means the height to be zoomed, H means displaying channel's height. */
} VO_ZOOM_RATIO_S;

typedef struct tsVO_ZOOM_ATTR_S {
    VO_ZOOM_IN_E enZoomType; /* choose the type of zoom in */
    union {
        RECT_S stZoomRect; /* zoom in by rect */
        VO_ZOOM_RATIO_S stZoomRatio; /* zoom in by ratio */
    };
} VO_ZOOM_ATTR_S;
#endif

typedef struct tsVO_CSC_S {
    VO_CSC_MATRIX_E enCscMatrix; /* CSC matrix */
    TS_U32 u32Luma; /* RW; Range:    [0, 100]; luminance, default: 50 */
    TS_U32 u32Contrast; /* RW; Range:    [0, 100]; contrast, default: 50 */
    TS_U32 u32Hue; /* RW; Range:    [0, 100]; hue, default: 50 */
    TS_U32 u32Satuature; /* RW; Range:    [0, 100]; satuature, default: 50 */
} VO_CSC_S;

#if 1 //Period 2
typedef struct tsVO_REGION_INFO_S {
    TS_U32 u32RegionNum; /* count of the region */
    RECT_S *ATTRIBUTE pstRegion; /* region attribute */
} VO_REGION_INFO_S;

typedef struct tsVO_LAYER_BOUNDARY_S {
    TS_U32 u32Width;
    TS_U32 u32Color[2];
} VO_LAYER_BOUNDARY_S;

typedef struct tsVO_CHN_BOUNDARY_S {
    TS_BOOL bBoundaryEn; /* do Frame or not */
    TS_U32 u32ColorIndex; /* the index of Frame color,[0,1] */
} VO_CHN_BOUNDARY_S;

typedef struct tsVO_MOD_PARAM_S {
    TS_BOOL bTransparentTransmit; /* RW, Range: [0, 1];  YC(Luminance and Chrominance) changes or not
                                            when passing through VO */
    TS_BOOL bExitDev;
    TS_BOOL bWbcBgBlackEn;
    TS_BOOL bDevClkExtEn;
    TS_BOOL bSaveBufMode[VO_MAX_PHY_DEV_NUM]; /* save buff mode */
} VO_MOD_PARAM_S;

typedef enum tsVO_CLK_SOURCE_E {
    VO_CLK_SOURCE_PLL,
    VO_CLK_SOURCE_LCDMCLK,

    VO_CLK_SOURCE_BUTT
} VO_CLK_SOURCE_E;

typedef struct tsVO_USER_INTFSYNC_PLL_S {
    TS_U32 u32Fbdiv;
    TS_U32 u32Frac;
    TS_U32 u32Refdiv;
    TS_U32 u32Postdiv1;
    TS_U32 u32Postdiv2;
} VO_USER_INTFSYNC_PLL_S;

typedef struct tsVO_USER_INTFSYNC_ATTR_S {
    VO_CLK_SOURCE_E enClkSource;

    union {
        VO_USER_INTFSYNC_PLL_S stUserSyncPll;
        TS_U32 u32LcdMClkDiv;
    };
} VO_USER_INTFSYNC_ATTR_S;

typedef struct tsVO_USER_INTFSYNC_INFO_S {
    VO_USER_INTFSYNC_ATTR_S stUserIntfSyncAttr;
    TS_U32 u32PreDiv;
    TS_U32 u32DevDiv;
    TS_BOOL bClkReverse;
} VO_USER_INTFSYNC_INFO_S;
#endif

typedef struct {
    TS_CHAR            szIpName[8];
    TS_VOID *          pCompHdl;
	TS_S32				iPort;
	/*
    后续如果需要，可以扩展 TS_BOOL *pbFuncList、_mpiADP_FUNC_MNG_S* pstFuncMng 成员
    以支持额外组件实现功能点
	*/
}_mpi_vo_TUPLE_S;


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef __TS_COMM_VO_H__ */



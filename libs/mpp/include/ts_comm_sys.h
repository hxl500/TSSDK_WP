/*
 * Copyright © 2019 TSING MICRO.com.cn All Rights Reserved
 * Description: ts_comm_sys.h
 * Author:TsingMicro multimedia software group
 * Create: 2022/06/02
 */

#ifndef __TS_COMM_SYS_H__
#define __TS_COMM_SYS_H__

#include "ts_type.h"
#include "ts_errno.h"
#include "ts_debug.h"
#include "ts_comm_video.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#define BIND_DEST_MAXNUM         64

#define TS_TRACE_SYS(level, fmt, ...)                                                                         \
    do {                                                                                                      \
        TS_TRACE(level, TS_ID_SYS, "[Func]:%s [Line]:%d [Info]:" fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
    } while (0)

typedef struct tsVPSS_VENC_WRAP_PARAM_S {
    TS_BOOL bAllOnline;

    TS_U32 u32FrameRate; /* Input frame rate of VIPROC(linear/Frame mode WDR is the sensor frame rate,
                          * and the line mode WDR is the frame rate after synthesis) */
    TS_U32 u32FullLinesStd; /* Total height of sensor, include of VBlank */

    SIZE_S stLargeStreamSize; /* VENC Large Stream Size */
    SIZE_S stSmallStreamSize; /* VENC Small Stream Size */

} VPSS_VENC_WRAP_PARAM_S;

typedef struct tsMPP_SYS_CONFIG_S {
    TS_U32 u32Align;

} MPP_SYS_CONFIG_S;

typedef struct tsSYS_VIRMEM_INFO_S {
    TS_U64 u64PhyAddr;
    TS_BOOL bCached;
} SYS_VIRMEM_INFO_S;

typedef struct tsMPP_BIND_DEST_S {
    TS_U32 u32Num;
    MPP_CHN_S astMppChn[BIND_DEST_MAXNUM];
} MPP_BIND_DEST_S;

typedef	enum tsSPECIAL_MODE_E {
	SYS_SPECIAL_MODE_NONE = 0,
	SYS_SPECIAL_MODE_FASTBOOT,
	SYS_SPECIAL_MODE_STITCH,
	SYS_SPECIAL_MODE_RESET,
	SYS_SPECIAL_MODE_AIISP,
	SYS_SPECIAL_MODE_LOOP,
	SYS_SPECIAL_MODE_BUTT
}MPP_SYS_SPECIAL_MODE_E;

#define SYS_SPECIAL_MASK_FASTBOOT (1<<(SYS_SPECIAL_MODE_FASTBOOT-1))
#define SYS_SPECIAL_MASK_STITCH (1<<(SYS_SPECIAL_MODE_STITCH-1))
#define SYS_SPECIAL_MASK_RESET (1<<(SYS_SPECIAL_MODE_RESET-1))
#define SYS_SPECIAL_MASK_AIISP (1<<(SYS_SPECIAL_MODE_AIISP-1))
#define SYS_SPECIAL_MASK_LOOP (1<<(SYS_SPECIAL_MODE_LOOP-1))


typedef enum tsSCALE_RANGE_E {
    SCALE_RANGE_0 = 0, /* scale range <   8/64 */
    SCALE_RANGE_1, /* scale range >=  8/64 */
    SCALE_RANGE_2, /* scale range >= 10/64 */
    SCALE_RANGE_3, /* scale range >= 15/64 */
    SCALE_RANGE_4, /* scale range >= 19/64 */
    SCALE_RANGE_5, /* scale range >= 24/64 */
    SCALE_RANGE_6, /* scale range >= 29/64 */
    SCALE_RANGE_7, /* scale range >= 33/64 */
    SCALE_RANGE_8, /* scale range >= 35/64 */
    SCALE_RANGE_9, /* scale range >= 38/64 */
    SCALE_RANGE_10, /* scale range >= 42/64 */
    SCALE_RANGE_11, /* scale range >= 45/64 */
    SCALE_RANGE_12, /* scale range >= 48/64 */
    SCALE_RANGE_13, /* scale range >= 51/64 */
    SCALE_RANGE_14, /* scale range >= 53/64 */
    SCALE_RANGE_15, /* scale range >= 55/64 */
    SCALE_RANGE_16, /* scale range >= 57/64 */
    SCALE_RANGE_17, /* scale range >= 60/64 */
    SCALE_RANGE_18, /* scale range >  1     */
    SCALE_RANGE_BUTT,
} SCALE_RANGE_E;

typedef enum tsCOEFF_LEVEL_E {
    COEFF_LEVEL_0 = 0, /* coefficient level 0 */
    COEFF_LEVEL_1, /* coefficient level 1 */
    COEFF_LEVEL_2, /* coefficient level 2 */
    COEFF_LEVEL_3, /* coefficient level 3 */
    COEFF_LEVEL_4, /* coefficient level 4 */
    COEFF_LEVEL_5, /* coefficient level 5 */
    COEFF_LEVEL_6, /* coefficient level 6 */
    COEFF_LEVEL_7, /* coefficient level 7 */
    COEFF_LEVEL_8, /* coefficient level 8 */
    COEFF_LEVEL_9, /* coefficient level 9 */
    COEFF_LEVEL_10, /* coefficient level 10 */
    COEFF_LEVEL_11, /* coefficient level 11 */
    COEFF_LEVEL_12, /* coefficient level 12 */
    COEFF_LEVEL_13, /* coefficient level 13 */
    COEFF_LEVEL_14, /* coefficient level 14 */
    COEFF_LEVEL_15, /* coefficient level 15 */
    COEFF_LEVEL_16, /* coefficient level 16 */
    COEFF_LEVEL_17, /* coefficient level 17 */
    COEFF_LEVEL_18, /* coefficient level 18 */
    COEFF_LEVEL_BUTT,
} COEFF_LEVEL_E;

typedef struct tsSCALE_COEFF_LEVEL_S {
    COEFF_LEVEL_E enHorLum; /* horizontal luminance   coefficient level */
    COEFF_LEVEL_E enHorChr; /* horizontal chrominance coefficient level */
    COEFF_LEVEL_E enVerLum; /* vertical   luminance   coefficient level */
    COEFF_LEVEL_E enVerChr; /* vertical   chrominance coefficient level */
} SCALE_COEFF_LEVEL_S;

typedef struct tsSCALE_RANGE_S {
    SCALE_RANGE_E enHorizontal;
    SCALE_RANGE_E enVertical;
} SCALE_RANGE_S;

typedef struct tsSCALE_COEFF_INFO_S {
    SCALE_RANGE_S stScaleRange;
    SCALE_COEFF_LEVEL_S stScaleCoeffLevel;
} SCALE_COEFF_INFO_S;

typedef struct tsGPS_INFO_S {
    TS_CHAR chGPSLatitudeRef; /* GPS LatitudeRef Indicates whether the latitude is north or south latitude,
                               * 'N'/'S', default 'N' */
    TS_U32 au32GPSLatitude[3][2]; /* GPS Latitude is expressed as degrees, minutes and seconds, a typical format
                                   * like "dd/1, mm/1, ss/1", default 0/0, 0/0, 0/0 */
    TS_CHAR chGPSLongitudeRef; /* GPS LongitudeRef Indicates whether the longitude is east or west longitude,
                                * 'E'/'W', default 'E' */
    TS_U32 au32GPSLongitude[3][2]; /* GPS Longitude is expressed as degrees, minutes and seconds, a typical format
                                    * like "dd/1, mm/1, ss/1", default 0/0, 0/0, 0/0 */
    TS_U8 u8GPSAltitudeRef; /* GPS AltitudeRef Indicates the reference altitude used, 0 - above sea level,
                             * 1 - below sea level default 0 */
    TS_U32 au32GPSAltitude[2]; /* GPS AltitudeRef Indicates the altitude based on the reference u8GPSAltitudeRef,
                                * the reference unit is meters, default 0/0 */
} GPS_INFO_S;

typedef enum tsVI_VPSS_MODE_E {
    VI_OFFLINE_VPSS_OFFLINE = 0,
    VI_OFFLINE_VPSS_ONLINE,
    VI_ONLINE_VPSS_OFFLINE,
    VI_ONLINE_VPSS_ONLINE,
    VI_VPSS_MODE_BUTT
} VI_VPSS_MODE_E;

typedef struct tsVI_VPSS_MODE_S {
	TS_S32			s32ModeCount;	/* no more than TS_VIVPSS_MODE_MAX_NUM */
	VI_VPSS_MODE_E * aenMode;
} VI_VPSS_MODE_S;

typedef struct tsRAW_FRAME_COMPRESS_PARAM_S {
    TS_U32 u32CompRatio_10Bit; /* compression ratio of 10 bit */
    TS_U32 u32CompRatio_12Bit; /* compression ratio of 12 bit */
    TS_U32 u32CompRatio_14Bit; /* compression ratio of 14 bit */
} RAW_FRAME_COMPRESS_PARAM_S;

#define TS_ERR_SYS_NULL_PTR      TS_DEF_ERR(TS_ID_SYS, EN_ERR_LEVEL_ERROR, EN_ERR_NULL_PTR)
#define TS_ERR_SYS_NOTREADY      TS_DEF_ERR(TS_ID_SYS, EN_ERR_LEVEL_ERROR, EN_ERR_SYS_NOTREADY)
#define TS_ERR_SYS_NOT_PERM      TS_DEF_ERR(TS_ID_SYS, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_PERM)
#define TS_ERR_SYS_NOMEM         TS_DEF_ERR(TS_ID_SYS, EN_ERR_LEVEL_ERROR, EN_ERR_NOMEM)
#define TS_ERR_SYS_ILLEGAL_PARAM TS_DEF_ERR(TS_ID_SYS, EN_ERR_LEVEL_ERROR, EN_ERR_ILLEGAL_PARAM)
#define TS_ERR_SYS_BUSY          TS_DEF_ERR(TS_ID_SYS, EN_ERR_LEVEL_ERROR, EN_ERR_BUSY)
#define TS_ERR_SYS_NOT_SUPPORT   TS_DEF_ERR(TS_ID_SYS, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_SUPPORT)
#define TS_ERR_SYS_COMP_INTERNAL TS_DEF_ERR(TS_ID_SYS, EN_ERR_LEVEL_ERROR, EN_ERR_COMP_INTERNAL)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __TS_COMM_SYS_H__ */


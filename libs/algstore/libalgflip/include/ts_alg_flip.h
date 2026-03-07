/*
 * Copyright (c) Beijing Tsing Micro Co.,Ltd. All rights reserved.
 * Description: the definitions of body detection
 * Author: Tsing Micro solution-application group
 * Create: 2023/04/27
 */
#ifndef _TS_ALG_FLIP_H_
#define _TS_ALG_FLIP_H_

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#include "ts_alg_type.h"
#include "ts_alg_info.h"

typedef enum tsALG_FLIP_IMG_MODE_E {
    ALG_FLIP_ARGB1555 = 0,
    ALG_FLIP_ARGB8888,
    ALG_FLIP_1BIT,
    ALG_FLIP_2BIT,
} ALG_FLIP_IMG_MODE_E;

/**
 * @brief flip process
 *
 * @param pSrc        : input image address
 * @param pDst        : output image address
 * @param u32ImgW     : input image width
 * @param u32ImgH     : input image height
 * @param eImgMode    : input image mode
 * @param u32FlipMode : flip mode: > 0: horizontal;  
 *                                 = 0: vertical;
 *                                 < 0: vertical & horizontal
 *
 * @return error code
 */
TS_S32 TS_ALG_Flip(TS_VOID *pSrc, TS_VOID *pDst, TS_U32 u32ImgW, TS_U32 u32ImgH, ALG_FLIP_IMG_MODE_E eImgMode, TS_S32 u32FlipMode);

/**
 * @brief get version
 *
 * @param void
 *
 * @return version
 */
const TS_CHAR *TS_ALG_Flip_GetVersion(TS_VOID);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif

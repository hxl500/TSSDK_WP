/*
 * Copyright (c) Beijing Tsing Micro Co.,Ltd. All rights reserved.
 * Description: color convert (NV12 -> RGB(RGBA))
 * Author: Tsing Micro solution-application group
 * Create: 2023/06/19
 */
#ifndef _TS_ALG_COLORCONVERT_H_
#define _TS_ALG_COLORCONVERT_H_

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#include "ts_alg_type.h"
#include "ts_alg_log.h"

typedef enum tsALG_COLOR_TYPE_E {
	ALG_COLOR_TYPE_RGB24,
	ALG_COLOR_TYPE_RGBA32,
} ALG_COLOR_TYPE_E;

/**
 * @brief convert nv12 -> rgb/rgba
 *
 * @param pYuv    : input yuv data address
 * @param pRgb    : output rgb data address
 * @param u32ImgW : input/output image width (must be multiple of 16)
 * @param u32ImgH : input/output image height (must be multiple of 2)
 * @param eRgb    : output ALG_COLOR_TYPE_E type
 *
 * @return 0 or error code
 */
TS_S32 TS_ALG_NV12ToRGB(TS_U8 *pYuv, TS_U8 *pRgb, TS_U32 u32ImgW, TS_U32 u32ImgH, ALG_COLOR_TYPE_E eRgb);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* _TS_ALG_COLORCONVERT__H_ */

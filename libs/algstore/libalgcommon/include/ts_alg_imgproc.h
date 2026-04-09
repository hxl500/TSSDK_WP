/*
 * Copyright (c) Beijing Tsing Micro Co.,Ltd. All rights reserved.
 * Description: base type definitions of alg
 * Author: Tsing Micro solution-application group
 * Create: 2024/06/01
 */
#ifndef _TS_ALG_IMGPROC_H_
#define _TS_ALG_IMGPROC_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "ts_alg_type.h"

typedef enum tsALG_RGB_TYPE_E {
	ALG_RGB_TYPE_RGB24,
	ALG_RGB_TYPE_RGBA32,
	ALG_RGB_TYPE_BGR24,
	ALG_RGB_TYPE_BGRA32,
} ALG_RGB_TYPE_E;

/**
 * @brief convert yuv to rgb
 * @param[in] pY, pointer of Y data
 * @param[in] pUv, pointer of UV data
 * @param[out] pRgb, rgb result
 * @param[in] u32SrcW, src img width
 * @param[in] u32SrcH, src img height
 * @param[in] u32ImgW, dst img width
 * @param[in] u32ImgH, dst img height
 * @param[in] eRgb, conver type
 * @return nms result num
 */
TS_VOID TS_ALG_YUV2RGB(TS_U8 *pY, TS_U8 *pUv, TS_U8 *pRgb, TS_U32 u32SrcW, TS_U32 u32SrcH, TS_U32 u32ImgW, TS_U32 u32ImgH, ALG_RGB_TYPE_E eRgb);

/**
 * @brief convert yuv to rgb, rgba, bgr bgra
 * @param[in] pY, pointer of Y data
 * @param[in] pUv, pointer of UV data
 * @param[out] pRgb, rgb result
 * @param[in] u32SrcW, src img width
 * @param[in] u32SrcH, src img height
 * @param[in] u32ImgW, dst img width
 * @param[in] u32ImgH, dst img height
 * @param[in] eRgb, conver type
 * @return nms result num
 */
TS_VOID TS_ALG_YUVCvtColor(TS_U8 *pY, TS_U8 *pUv, TS_U8 *pRgb, TS_U32 u32SrcW, TS_U32 u32SrcH, TS_U32 u32ImgW, TS_U32 u32ImgH, ALG_RGB_TYPE_E eRgb);

/**
 * @brief expand 1 channel to 3/4 channel
 *        1 ch -> 3 ch : a1, a2, a3 -> a1, a1, a1, a2, a2, a2, a3, a3, a3
 *        1 ch -> 4 ch : a1, a2, a3 -> a1, a1, a1, a1, a2, a2, a2, a2, a3, a3, a3, a3
 * @return 0 succeed, -1 failed
 * @note   len must be multiple of 16
 */
TS_S32 TS_ALG_Chan_Expand(TS_U8 *pSrc, TS_U8 *pDst, TS_U32 u32Len, TS_U32 u32DstCh);

/**
 * @brief compression 3/4 channel to 1 channel
 *        3 ch -> 1 ch : a1, a1, a1, a2, a2, a2, a3, a3, a3 -> a1, a2, a3
 *        4 ch -> 1 ch : a1, a1, a1, a1, a2, a2, a2, a2, a3, a3, a3, a3 -> a1, a2, a3
 * @return 0 succeed, -1 failed
 * @note   len must be multiple of 16
 */
TS_S32 TS_ALG_Chan_Compress(TS_U8 *pSrc, TS_U8 *pDst, TS_U32 u32Len, TS_U32 u32SrcCh);


typedef TS_S32 (*TS_ALG_ResizeFunc)(const TS_U8 *pSrc, TS_U8 *pDst, 
									TS_S32 s32SrcW, TS_S32 s32SrcH,
									TS_S32 s32DstW, TS_S32 s32DstH, TS_S32 s32Channels);

TS_S32 TS_ALG_ResizeCbk_Register(TS_ALG_ResizeFunc fCbk);

TS_VOID TS_ALG_ResizeCbk_Unregister(TS_VOID);

TS_S32 TS_ALG_Resize(const TS_U8 *pSrc, TS_U8 *pDst, TS_S32 s32SrcW, TS_S32 s32SrcH, 
					TS_S32 s32DstW, TS_S32 s32DstH, TS_S32 s32Channels);

extern TS_S32 TS_MPI_TRP_CVE_Resize(TS_U64 srcPhy, TS_VOID *src, TS_U64 dstPhy, TS_VOID *dst,
										TS_S32 srcW, TS_S32 srcH, TS_S32 subW, TS_S32 subH,
										TS_S32 dstW, TS_S32 dstH, TS_S32 channels);


extern TS_S32 TS_MPI_TRP_CVE_Init(TS_VOID);

extern TS_S32 TS_MPI_TRP_CVE_Exit(TS_VOID);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

#ifndef __VIDEO_ALG_AIPERIMETER_H__
#define __VIDEO_ALG_AIPERIMETER_H__

#ifdef __cplusplus
extern "C" {
#endif 

#include "sample_alg_cpm.h"
#include "ts_alg_type.h"

#if ALG_AIPERIMETER

TS_S32 VIDEO_ALG_AIPerimeter_Init(TS_VOID **handle);

TS_S32 VIDEO_ALG_AIPerimeter_Proc(TS_VOID *handle, ALG_IMAGE_S *pImageOri, ALG_IMAGE_S *pImageDet, TS_VOID *result);

TS_VOID VIDEO_ALG_AIPerimeter_ResultProc(TS_U8 *YuvBuf,  TS_U32 width, TS_U32 height,TS_U32 ImageRatio, TS_VOID *pAlg_result);

TS_S32 VIDEO_ALG_AIPerimeter_Exit(TS_VOID *handle);

#endif

#ifdef __cplusplus
}
#endif /* End of #ifdef __cplusplus */

#endif /* __VIDEO_ALG_AIPERIMETER_H__ */

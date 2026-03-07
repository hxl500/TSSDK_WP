#ifndef __VIDEO_ALG_MOTIONDETECH_H__
#define __VIDEO_ALG_MOTIONDETECH_H__

#ifdef __cplusplus
extern "C" {
#endif 

#include "sample_alg_cpm.h"
#include "ts_alg_type.h"

#if ALG_MOTIONDETECT
TS_S32 VIDEO_ALG_Motion_Init(TS_VOID **pHandle);

TS_S32 VIDEO_ALG_Motion_Proc(TS_VOID *pHandle, ALG_IMAGE_S *pImage, ALG_IMAGE_S *pImageDet, TS_VOID *pResult);

TS_VOID  VIDEO_ALG_Motion_ResultProc(TS_U8 *pYuvBuf,  TS_U32 width, TS_U32 height, TS_U32 u32ImageRatio, TS_VOID *pAlgResult);

TS_S32 VIDEO_ALG_Motion_Exit(TS_VOID *pHandle);

#endif

#ifdef __cplusplus
}
#endif /* End of #ifdef __cplusplus */

#endif

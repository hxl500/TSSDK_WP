#ifndef __VIDEO_ALG_BODYDET1_V16_H__
#define __VIDEO_ALG_BODYDET1_V16_H__


#include "sample_alg_cpm.h"
#include "ts_alg_type.h"
#include "video_alg_catdetect-api.h"
TS_S32 VIDEO_ALG_CatDetect_Init(TS_VOID **pHandle);

TS_S32 VIDEO_ALG_CatDetect_Proc_user(TS_VOID *pHandle, ALG_IMAGE_S *pImage, ALG_IMAGE_S *pImageDet, TS_VOID *pResult, TS_U8 cam_id);
//TS_S32 VIDEO_ALG_CatDetect_Proc(TS_VOID *pHandle, ALG_IMAGE_S *pImage, ALG_IMAGE_S *pImageDet, TS_VOID *pResult);

TS_VOID  VIDEO_ALG_CatDetect_ResultProc(TS_U8 *pYuvBuf,  TS_U32 width, TS_U32 height, TS_U32 u32ImageRatio, TS_VOID *pAlgResult);

//TS_S32 VIDEO_ALG_CatDetect_Exit(TS_VOID *pHandle);

#endif


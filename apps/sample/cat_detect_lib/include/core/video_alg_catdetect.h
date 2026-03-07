#ifndef __VIDEO_ALG_BODYDET_V19_H__
#define __VIDEO_ALG_BODYDET_V19_H__


#include "sample_alg_cpm.h"
//#include "sample_comm.h"
#include "ts_alg_type.h"
typedef unsigned char Uint8;
// osd font text info
typedef struct
{
    int x, y;
    Uint8 alpha;
}TS_BITMAP_CHAT_S;
#include "video_alg_catdetect-api.h"
//#define ALG_CAT_DETECT 1
TS_S32 VIDEO_ALG_CatDetect_Init(TS_VOID **pHandle);

//TS_S32 VIDEO_ALG_CatDetect_Proc(TS_VOID *pHandle, ALG_IMAGE_S *pImage, ALG_IMAGE_S *pImageDet, TS_VOID *pResult);

//TS_VOID  (TS_U8 *pYuvBuf,  TS_U32 width, TS_U32 height, TS_U32 u32ImageRatio, TS_VOID *pAlgResult);

TS_S32 VIDEO_ALG_CatDetect_Exit(TS_VOID *pHandle);


#endif

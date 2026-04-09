#ifndef __AUDIO_ALG_CRYDETECT_H__
#define __AUDIO_ALG_CRYDETECT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "ts_alg_type.h"

#if ALG_CRYDETECT

TS_S32 AUDIO_ALG_CryDetect_Init(TS_VOID **pHandle);

TS_S32 AUDIO_ALG_CryDetect_Proc(TS_VOID *pHandle, AUDIO_FRAME_S *pIn);

TS_S32 AUDIO_ALG_CryDetect_Exit(TS_VOID *pHandle);

#endif

#ifdef __cplusplus
}
#endif /* End of #ifdef __cplusplus */

#endif

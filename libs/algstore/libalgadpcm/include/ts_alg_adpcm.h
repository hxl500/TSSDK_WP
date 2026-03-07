#ifndef TS_ALG_ADPCM_H
#define TS_ALG_ADPCM_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#include "ts_alg_type.h"

typedef struct{
	TS_S16 *pSmpl;
	TS_U32 u32ByteLen;
}ALG_ADPCM_SMPL_S;

typedef struct{
	TS_S8 *pStream;
	TS_U32 u32ByteLen;
} ALG_ADPCM_STREAM_S;

TS_S32 TS_ALG_ADPCM_Init(TS_VOID **ppHandle);

TS_VOID TS_ALG_ADPCM_Exit(TS_VOID **ppHandle);

TS_VOID TS_ALG_ADPCM_Encode(TS_VOID *pHandle, ALG_ADPCM_SMPL_S *pInput, ALG_ADPCM_STREAM_S *pOutput);

TS_VOID TS_ALG_ADPCM_Decode(TS_VOID *pHandle, ALG_ADPCM_STREAM_S *pInput, ALG_ADPCM_SMPL_S *pOutput);

const TS_CHAR *TS_ALG_ADPCM_GetVersion(TS_VOID);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

#ifndef __TS_ALG_NOISEGATE_H__
#define __TS_ALG_NOISEGATE_H__

#include "ts_alg_type.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct{
	TS_S32 s32SmplRatio;
	TS_FLOAT fAttackTime; //uint:s
	TS_FLOAT fReleaseTime; //uint:s
	TS_S32 s32Threshold; // unit:dB
}ALG_NOISEGATE_INIT_S;

typedef struct{
	TS_S16 *pIn;
	TS_U32 u32InLen;
}ALG_NOISEGATE_PROC_S;

typedef struct{
	TS_S16 *pOut;
	TS_U32 u32OutLen;
} ALG_NOISEGATE_RSLT_S;


TS_S32 TS_ALG_NoiseGate_Init(TS_VOID **ppHandle, ALG_NOISEGATE_INIT_S *pParam);

TS_S32 TS_ALG_NoiseGate_Process(TS_VOID *pHandle, ALG_NOISEGATE_PROC_S *pInput, ALG_NOISEGATE_RSLT_S *pOutput);

TS_S32 TS_ALG_NoiseGate_Exit(TS_VOID *pHandle);

const TS_CHAR *TS_ALG_NoiseGate_GetVersion(TS_VOID);


#ifdef __cplusplus
}
#endif

#endif /* __TS_ALG_NOISEGATE_H__ */

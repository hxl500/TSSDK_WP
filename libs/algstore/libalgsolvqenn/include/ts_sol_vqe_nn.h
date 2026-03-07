

#ifndef __TS_SOL_VQE_NN_H__
#define __TS_SOL_VQE_NN_H__


#include "ts_alg_type.h"
#include "ts_alg_anr_aec.h"
#include "ts_alg_webrtc_agc.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define SOL_ANR_MASK				0x1
#define SOL_AEC_ANR_MASK			0x2
#define SOL_AGC_MASK				0x4

typedef struct{
	ALG_ANR_AEC_INIT_S algAnrAecInit;
	ALG_WEBRTC_AGC_INIT_S algAgcInit;
	ALG_WEBRTC_AGC_PARAM_S algAgcParam;
}SOL_VQE_NN_INIT_S;

typedef struct{
	ALG_ANR_AEC_PORC_S algAnrAecProcParam;
	ALG_WEBRTC_AGC_PROC_S algAgcProcParam;
}SOL_VQE_NN_PROC_S;

typedef struct{
	// TS_S32 s32VqeSetParam;
	ALG_ANR_AEC_PARAM_S algAnrAecSetParam;
} SOL_VQE_NN_SET_PARAM_S;

typedef struct{
	TS_U32 u32OpenMask;
	TS_U32 u32SmplRate;
	TS_U16 u16FrameLen;
	TS_U8 u8ChanNum;
	SOL_VQE_NN_INIT_S solVqeInit;
	SOL_VQE_NN_PROC_S solVqeProc;
	SOL_VQE_NN_SET_PARAM_S *solVqeSetParam; // if use default values, please set null
}SOL_VQE_NN_CONF_PARAM;


TS_S32 TS_SOL_VQE_NN_Init(TS_VOID **ppHandle, SOL_VQE_NN_CONF_PARAM *pParam);

/*
 * pu32InSmplLen must equal to frame length
*/
TS_S32 TS_SOL_VQE_NN_Process(TS_VOID* pHandle, TS_S16 *ps16InBuf, TS_S16 *ps16RefBuf, TS_U32 *pu32InSmplLen, TS_S16 *ps16OutBuf, TS_U32 *u32OutSmplLen);


// TS_S32 TS_SOL_VQE_NN_SetParam(TS_VOID* pHandle, SOL_VQE_NN_CONF_PARAM *pConfig);


TS_S32 TS_SOL_VQE_NN_GetParam(TS_VOID* pHandle, SOL_VQE_NN_SET_PARAM_S* pConfig);


TS_S32 TS_SOL_VQE_NN_Exit(TS_VOID *pHandle);


const TS_CHAR *TS_SOL_VQE_NN_GetVersion(TS_VOID);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

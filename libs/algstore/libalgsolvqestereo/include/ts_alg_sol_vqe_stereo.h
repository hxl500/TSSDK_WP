#ifndef __TS_ALG_SOL_VQE_STEREO_H__
#define __TS_ALG_SOL_VQE_STEREO_H__

#include "ts_alg_webrtc_ns.h"
#include "ts_alg_webrtc_agc.h"
#include "ts_alg_mmsens.h"
#include "ts_alg_omlsa.h"
#include "ts_alg_noisegate.h"
#include "ts_alg_adaptaec.h"
// #include "ts_alg_anr_nn.h"

#include "ts_alg_type.h"

#if defined(__cplusplus)
extern "C"
{
#endif

#define ALG_VQE_SOL_ADAPT_AEC_MASK 0x01
#define ALG_VQE_SOL_WEBRTC_ANR_MASK 0x02
#define ALG_VQE_SOL_WEBRTC_AGC_MASK 0x04
#define ALG_VQE_MMSENS_MASK 0x08
#define ALG_VQE_OMLSA_MASK 0x10
#define ALG_VQE_NOISEGATE_MASK 0x20
// #define ALG_VQE_NNANR_MASK 0x40

typedef struct{
}ALG_SOL_VQE_STEREO_PROC_S;

typedef struct{
	ALG_WEBRTC_NSX_PARAM_S algAnrParams;
	ALG_WEBRTC_AGC_PARAM_S algAgcParams;
} ALG_SOL_VQE_STEREO_SET_PARAM_S;

typedef struct{
	TS_U32 u32OpenMask;
	TS_U32 u32SmplRate;
	TS_U16 u16FrameLen;
	TS_U8 u8MicChanNum;
	TS_U8 u8RefChanNum;
	TS_S16 s16AgcMode;
	TS_S32 s32NoiseSuppress;
	TS_FLOAT fAttackTime; //uint:s
	TS_FLOAT fReleaseTime; //uint:s
	TS_S32 s32Threshold; // unit:dB
	ALG_ADAPT_AEC_MODE_E eNlpMode;
	ALG_SOL_VQE_STEREO_PROC_S solVqeProc;
	ALG_SOL_VQE_STEREO_SET_PARAM_S solVqeSetParam;
}ALG_SOL_VQE_STEREO_CONF_PARAM;


TS_S32 TS_ALG_SolVqeStereo_Init(TS_VOID **ppHandle, ALG_SOL_VQE_STEREO_CONF_PARAM *pParam);


TS_S32 TS_ALG_SolVqeStereo_Process(TS_VOID* pHandle, TS_S16 *ps16InBuf, TS_S16 *ps16RefBuf, TS_U32 *pu32InSmplLen, TS_S16 *ps16OutBuf, TS_U32 *u32OutSmplLen);


// TS_S32 TS_ALG_SolVqeStereo_SetParam(TS_VOID* pHandle, ALG_SOL_VQE_STEREO_CONF_PARAM *pConfig);


TS_S32 TS_ALG_SolVqeStereo_GetParam(TS_VOID* pHandle, ALG_SOL_VQE_STEREO_SET_PARAM_S* pConfig);


TS_S32 TS_ALG_SolVqeStereo_Exit(TS_VOID *pHandle);


const TS_CHAR *TS_ALG_SolVqeStereo_GetVersion(TS_VOID);


#if defined(__cplusplus)
}
#endif

#endif /* __TS_ALG_SOL_VQE_STEREO_H__ */

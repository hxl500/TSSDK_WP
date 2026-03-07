#ifndef __TS_ALG_VQE_SOL_WEBRTC3A_H__
#define __TS_ALG_VQE_SOL_WEBRTC3A_H__

#include "ts_alg_webrtc_ns.h"
#include "ts_alg_webrtc_aec.h"
#include "ts_alg_webrtc_agc.h"
#include "ts_alg_mmsens.h"
#include "ts_alg_omlsa.h"
#include "ts_alg_noisegate.h"
#include "ts_alg_anr_nn.h"

#include "ts_alg_type.h"

#if defined(__cplusplus)
extern "C"
{
#endif

#define ALG_VQE_SOL_WEBRTC_AEC_MASK 0x01
#define ALG_VQE_SOL_WEBRTC_ANR_MASK 0x02
#define ALG_VQE_SOL_WEBRTC_AGC_MASK 0x04
#define ALG_VQE_MMSENS_MASK 0x08
#define ALG_VQE_OMLSA_MASK 0x10
#define ALG_VQE_NOISEGATE_MASK 0x20
#define ALG_VQE_NNANR_MASK 0x40

typedef struct{
	TS_S16 s16MsInSndCardBuf;
	TS_S32 s32Skew;
}ALG_VQE_SOL_WEBRTC3A_PROC_S;

typedef struct{
	ALG_WEBRTC_AEC_PARAM_S algAecParams;
	ALG_WEBRTC_NSX_PARAM_S algAnrParams;
	ALG_WEBRTC_AGC_PARAM_S algAgcParams;
} ALG_VQE_SOL_WEBRTC3A_SET_PARAM_S;

typedef struct{
	TS_U32 u32OpenMask;
	TS_U32 u32SmplRate;
	TS_U16 u16FrameLen;
	TS_U8 u8ChanNum;
	TS_S16 s16AgcMode;
	TS_S32 s32NoiseSuppress;
	TS_FLOAT fAttackTime; //uint:s
	TS_FLOAT fReleaseTime; //uint:s
	TS_S32 s32Threshold; // unit:dB
	TS_U8 *pGraph;
	TS_U8 *pWeight;
	TS_U32 u32GraphSize;
	TS_VOID *vpSelfBuf;
	TS_S32 s32AecThreshold;
	ALG_VQE_SOL_WEBRTC3A_PROC_S solVqeProc;
	ALG_VQE_SOL_WEBRTC3A_SET_PARAM_S solVqeSetParam;
}ALG_VQE_SOL_WEBRTC3A_CONF_PARAM;


TS_S32 TS_ALG_VQESolWebRTC3A_Init(TS_VOID **ppHandle, ALG_VQE_SOL_WEBRTC3A_CONF_PARAM *pParam);


TS_S32 TS_ALG_VQESolWebRTC3A_Process(TS_VOID* pHandle, TS_S16 *ps16InBuf, TS_S16 *ps16RefBuf, TS_U32 *pu32InSmplLen, TS_S16 *ps16OutBuf, TS_U32 *u32OutSmplLen);


// TS_S32 TS_ALG_VQESolWebRTC3A_SetParam(TS_VOID* pHandle, ALG_VQE_SOL_WEBRTC3A_CONF_PARAM *pConfig);


TS_S32 TS_ALG_VQESolWebRTC3A_GetParam(TS_VOID* pHandle, ALG_VQE_SOL_WEBRTC3A_SET_PARAM_S* pConfig);


TS_S32 TS_ALG_VQESolWebRTC3A_Exit(TS_VOID *pHandle);


const TS_CHAR *TS_ALG_VQESolWebRTC3A_GetVersion(TS_VOID);


#if defined(__cplusplus)
}
#endif

#endif /* __TS_ALG_VQE_SOL_WEBRTC3A_H__ */

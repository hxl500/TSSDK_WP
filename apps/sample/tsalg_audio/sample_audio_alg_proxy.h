#ifndef __SAMPLE_AUDIO_ALG_PROXY_H__
#define __SAMPLE_AUDIO_ALG_PROXY_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <pthread.h>
#include <semaphore.h>

#include "audio_alg_config.h"
#include "ts_alg_type.h"
#include "ts_alg_log.h"

typedef TS_S32 (*pf_AUDIO_ALG_Init)(TS_VOID **ppHdl);
typedef TS_S32 (*pf_AUDIO_ALG_Process)(TS_VOID *pHdl, AUDIO_FRAME_S *pIn);
typedef TS_S32 (*pf_AUDIO_ALG_Exit)(TS_VOID *pHdl);

typedef struct {
	SAMPLE_AUDIO_ALG_TYPE_E enAlgType;
	pf_AUDIO_ALG_Process pfProcess;
	pf_AUDIO_ALG_Exit pfExit;

	TS_VOID *pHandle;
} SAMPLE_AUDIO_ALG_INSTANCE_S;

typedef struct {
	AUDIO_DEV AiDev;
	AI_CHN AiChn;
	AUDIO_DEV AoDev;
	AO_CHN AoChn;
	AUDIO_SAMPLE_RATE_E eAudioFs; //音频采样率

	TS_U32 u32AlgNum; //外部配置的算法数目
	TS_U32 u32ActualAlgNum; //实际支持的算法数目

	SAMPLE_AUDIO_ALG_TYPE_E enAlgMask[AUDIO_ALG_MAX_NUM];
	SAMPLE_AUDIO_ALG_INSTANCE_S *pstAlgInstList[AUDIO_ALG_MAX_NUM];

	pthread_t stAlgProcPid;
	TS_BOOL bAlgProcRunFlag;

} SAMPLE_AUDIO_ALG_PROXY;

typedef TS_S32 (*pf_AUDIO_ALG_INSTANCE_Init)(TS_VOID **pHandle);
typedef struct AUDIO_ALG_INIT_S {
	SAMPLE_AUDIO_ALG_TYPE_E eMask;
	AUDIO_SAMPLE_RATE_E eAudioFs;
	pf_AUDIO_ALG_INSTANCE_Init pInit;
	TS_U8 u8AlgName[30];
	TS_S32 s32FrameLen;
} SAMPLE_AUDIO_ALG_INIT_CFG_S;

TS_S32 SAMPLE_AUDIO_ALG_Config(AUDIO_SAMPLE_RATE_E efs, AUDIO_DEV AiDev, AI_CHN AiChn, AUDIO_DEV AoDev, AO_CHN AoChn,
			       SAMPLE_AUDIO_ALG_TYPE_E *penAlgType, TS_U32 u32AlgNum);
TS_VOID SAMPLE_AUDIO_ALG_Stop();
SAMPLE_AUDIO_ALG_INIT_CFG_S *SAMPLE_AUDIO_ALG_GetInitCfg(TS_U32 *pNum);

#ifdef __cplusplus
}
#endif /* End of #ifdef __cplusplus */

#endif

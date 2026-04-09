/*
 * Copyright (c) Beijing Tsing Micro Co.,Ltd. All rights reserved.
 * Description: ANR
 * Author: Tsing Micro solution-application group
 * Create: 2023/02/10
 */
#ifndef _TS_ALG_ANR_AEC_H_
#define _TS_ALG_ANR_AEC_H_

#include "ts_alg_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

typedef enum{
	SKEW_ALIGN, // adjust ref and mic delay on time
	SKEW_FIX, // adjust ref and mic accroding to setted delayms
	SKEW_NONE,
}AEC_SKEW_MODE;

typedef enum{
	LAEC_FAST,
	LAEC_PBNLMS,
	LAEC_DISABLE,
}AEC_LAEC_MODE;

typedef struct{
	TS_U8 *pGraph[2]; // mode graph address
	TS_U8 *pWeight[2]; // mode weight address
	TS_U32 u32GraphSize[2]; // mode graph size
	TS_U32 u32WeightSize[2]; // mode weight size
	TS_VOID *vpSelfBuf[2]; // blob mem
	TS_U8 bRneOff; // open or not open RNE device, revent multiple operations

	TS_S32 s32SmplRatio;
	TS_S16 s16FrameLen;
	TS_S16 s16ChanNum;
	AEC_SKEW_MODE aecAlignMode;
} ALG_ANR_AEC_INIT_S;

typedef struct{
	TS_S16 *pMicData;
	TS_U32 u32MicDataLen;
	TS_S16 *pRefData;
	TS_U32 u32RefDataLen;
	TS_S16 s16MsInSndCardBuf; // only meaning when SKEW_FIX mode
}ALG_ANR_AEC_PORC_S;

typedef struct{
	TS_S16 *pOutData;
	TS_U32 u32OutLen;
	TS_S32 s32RefDelayBlock; // only meaning when SKEW_ALIGN mode
} ALG_ANR_AEC_RSLT_S;

typedef struct {
	/**
	 * @brief default: 50
	 */
	TS_S16 ps16HistNum;

	/**
	 * @brief default: 0.2f
	 */
	TS_FLOAT f32MaxCohxThreshold;

	/**
	 * @brief default: LAEC_FAST
	 */
	AEC_LAEC_MODE laecMode;

	/**
	 * @brief default: MVDR
	 * 0:NN
	 * 1:MVDR
	 */
	TS_U8 stereoAnrMode;
} ALG_ANR_AEC_PARAM_S;


/**
 * @brief anr+aec initiate
 * only support sample ratio:16K/8K
 * only support frame length: 256(16K sample ratio)/128(8K sample ratio)
 *
 * @param ppHandle : handle
 * @param pParam   : input param
 *
 * @attention : If the function fails, you also need to call the release api
 * @return error code
 */
TS_S32 TS_ALG_ANR_AEC_Init(TS_VOID **ppHandle, ALG_ANR_AEC_INIT_S *pParam);

/**
 * @brief anr+aec  process
 *
 * @param pHandle : handle
 * @param pInput : input parameters instructure
 * @param pOutput : output parameters instructure
 *
 * @return error or result : 0: sucess;  others: error;
 */
TS_S32 TS_ALG_ANR_AEC_Process(TS_VOID *pHandle, ALG_ANR_AEC_PORC_S *pInput, ALG_ANR_AEC_RSLT_S *pOutput);


/**
 * @brief exit anr+aec
 *
 * @param pHandle : handle
 *
 * @return error code 0: success; others: error
 */
TS_S32 TS_ALG_ANR_AEC_Exit(TS_VOID *pHandle);

TS_S32 TS_ALG_ANR_AEC_SetParam(TS_VOID* pHandle, ALG_ANR_AEC_PARAM_S *pConfig);

TS_S32 TS_ALG_ANR_AEC_GetParam(TS_VOID* pHandle, ALG_ANR_AEC_PARAM_S *pConfig);

/**
 * @brief anr+aec get version
 *
 * @param void
 *
 * @return version
 */
const TS_CHAR* TS_ALG_ANR_AEC_GetVersion(TS_VOID);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _TS_ALG_ANR_AEC_H_ */

/*
 * Copyright (c) Beijing Tsing Micro Co.,Ltd. All rights reserved.
 * Description: ANR
 * Author: Tsing Micro solution-application group
 * Create: 2023/02/10
 */
#ifndef _TS_ALG_ICD_H_
#define _TS_ALG_ICD_H_

#include "ts_alg_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

typedef struct{
	TS_U8 *pCfg;
	TS_U8 *pWeight;
	TS_VOID *pSelfBuf;
	TS_S16 s16FrameLen;
	TS_S32 s32GraphSize;
} ALG_ICD_INIT_S;

typedef struct{
	TS_S16 *pIn;
	TS_U32 u32InLen;
}ALG_ICD_PROC_S;

typedef struct{
	TS_U8 u8CryFlg;
	TS_U8 u8AlarmFlg;
	TS_FLOAT fscore; // for test
} ALG_ICD_RSLT_S;

typedef struct {
	/**
	 * @brief MUST be integer. Default: 38. The size of probability buffer and weight buffer.
	 */
	TS_U32 u32SmoothFrameNum;

	/**
	 * @brief MUST >= 0 & <= 1. Default: 0.35.
	 */
	TS_FLOAT f32SmoothThreshold;

	/**
	 * @brief default: MUST be 1 or 0. Default: 1.
	 */
	TS_U32 u32JumpTrigger;

	/**
	 * @brief MUST be integer. Default: 226, minute = alarm_interval*16*16/1000/60
	 * unalarm: INT_MAX
	 */
	TS_U32 u32AlarmInterval;
} ALG_ICD_PARAM_S;


/**
 * @brief icd initiate
 * only support sample ratio:8K
 *
 * @param ppHandle : handle
 * @param pParam   : input param
 *
 * @attention : If the function fails, you also need to call the release api
 * @return error code
 */
TS_S32 TS_ALG_ICD_Init(TS_VOID **ppHandle, ALG_ICD_INIT_S *pParam);

/**
 * @brief icd  process
 *
 * @param pHandle : handle
 * @param pInput : input parameters instructure
 * @param pOutput : output parameters instructure
 *
 * @return error or result : 0: sucess;  others: error;
 */
TS_S32 TS_ALG_ICD_Process(TS_VOID *pHandle, ALG_ICD_PROC_S *pInput, ALG_ICD_RSLT_S *pOutput);

/**
 * @brief exit icd
 *
 * @param pHandle : handle
 *
 * @return error code 0: success; others: error
 */
TS_S32 TS_ALG_ICD_Exit(TS_VOID *pHandle);

TS_S32 TS_ALG_ICD_SetParam(TS_VOID* pHandle, ALG_ICD_PARAM_S *pConfig);

TS_S32 TS_ALG_ICD_GetParam(TS_VOID* pHandle, ALG_ICD_PARAM_S *pConfig);

/**
 * @brief icd get version
 *
 * @param void
 *
 * @return version
 */
const TS_CHAR* TS_ALG_ICD_GetVersion(TS_VOID);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _TS_ALG_ICD_H_ */

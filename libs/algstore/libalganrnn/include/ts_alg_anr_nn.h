/*
 * Copyright (c) Beijing Tsing Micro Co.,Ltd. All rights reserved.
 * Description: ANR
 * Author: Tsing Micro solution-application group
 * Create: 2023/02/10
 */
#ifndef _TS_ALG_ANR_NN_H_
#define _TS_ALG_ANR_NN_H_

#include "ts_alg_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define ANRNN_MVersion 32

typedef struct{
	TS_U8 *pGraph;
	TS_U8 *pWeight;
	TS_U32 u32GraphSize;
	TS_VOID *vpSelfBuf;

	TS_S32 s32SmplRatio;
	TS_S16 s16FrameLen;
	TS_S16 s16ChanNum;
} ALG_ANR_NN_INIT_S;

typedef struct{
	TS_S16 *ps16Data;
	TS_U32 u32DataLen;
}ALG_ANR_NN_PORC_S;

typedef struct{
	TS_S16 *ps16OutData;
	TS_U32 u32OutLen;
} ALG_ANR_NN_RSLT_S;

typedef struct {
	TS_S32 s32IsRneNotOff; // 1:no control; others: control; default: ctrl;
} ALG_ANR_NN_PARAM_S;


/**
 * @brief anr initiate
 * only support sample ratio:16K/8K
 * only support frame length: 256(16K sample ratio)/128(8K sample ratio)
 *
 * @param ppHandle : handle
 * @param pParam   : input param
 *
 * @attention : If the function fails, you also need to call the release api
 * @return error code
 */
TS_S32 TS_ALG_ANR_NN_Init(TS_VOID **ppHandle, ALG_ANR_NN_INIT_S *pParam);

/**
 * @brief anr  process
 *
 * @param pHandle : handle
 * @param pInput : input parameters instructure
 * @param pOutput : output parameters instructure
 *
 * @return error or result : 0: sucess;  others: error;
 */
TS_S32 TS_ALG_ANR_NN_Process(TS_VOID *pHandle, ALG_ANR_NN_PORC_S *pInput, ALG_ANR_NN_RSLT_S *pOutput);


/**
 * @brief exit anr
 *
 * @param pHandle : handle
 *
 * @return error code 0: success; others: error
 */
TS_S32 TS_ALG_ANR_NN_Exit(TS_VOID *pHandle);

TS_S32 TS_ALG_ANR_NN_SetParam(TS_VOID* pHandle, ALG_ANR_NN_PARAM_S *Config);

TS_S32 TS_ALG_ANR_NN_GetParam(TS_VOID* pHandle, ALG_ANR_NN_PARAM_S *Config);

/**
 * @brief anr get version
 *
 * @param void
 *
 * @return version
 */
const TS_CHAR* TS_ALG_ANR_NN_GetVersion(TS_VOID);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _TS_ALG_ANR_NN_H_ */

/*
 * Copyright (c) Beijing Tsing Micro Co.,Ltd. All rights reserved.
 * Description: the definitions of speex resample
 * Author: Tsing Micro solution-application group
 * Create: 2023/09/01
 */
#ifndef  _TS_ALG_RESAMPLE_H_
#define _TS_ALG_RESAMPLE_H_

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#include "ts_alg_type.h"

enum {
	RESAMPLER_ERR_SUCCESS = 0,
	RESAMPLER_ERR_ALLOC_FAILED = 1,
	RESAMPLER_ERR_BAD_STATE = 2,
	RESAMPLER_ERR_INVALID_ARG = 3,
	RESAMPLER_ERR_PTR_OVERLAP = 4,
	RESAMPLER_ERR_MAX_ERROR
};

typedef struct{
	int chanNum;    /* -I : channel num 1 mono 2 stereo */
	int inRate;     /* -I : input rate, input sample rate */
	int outRate;    /* -I : output rate, output sample rate*/
	int quality;     /* -I : quality of resampler, 0(minimum cost & low quality),1,2..., 9(most cost & high quality) */
}ALG_RESAMPLE_INIT_PARAM_S;

typedef struct{
	void* in;       /* -I: input data */
	int inlen;      /* -I: input data length */
	int inBitWidth; /* -I: bit width of input data, only support 16 or 32*/
}ALG_RESAMPLE_PROC_PARAM_S;

typedef struct{
	void* out;      /* -O: output data */
	int* outlen;    /* -O: output data length, it should be preset to tell the resmpler you have enough buffer size  */
}ALG_RESAMPLE_RESULT_PARAM_S;

typedef struct{
 	int quality;     /* -I : quality of resampler, 0(minimum cost & low quality),1,2..., 9(most cost & high quality) */
}ALG_RESAMPLE_SET_PARAM_S;

/**
 * @brief init the resampler
 *
 * @param ppHandle : resampler's handle
 * @param pParam   : input parameters
 *
 * @attention : If the function fails, you also need to call the release api
 * @return error code, 0: success; others: error code
 */
TS_S32 TS_ALG_Resample_Init(TS_VOID **ppHandle, ALG_RESAMPLE_INIT_PARAM_S *pParam);


/**
 * @brief resampler process:
 * support in data type is int16_t/int32_t, output data type is same with in
 *
 * @param pHandle : resampler's handle
 * @param pImage  : input parameters address
 * @param pResult : output result
 *
 * @return error code
 */
TS_S32 TS_ALG_Resample_Process(TS_VOID *pHandle, ALG_RESAMPLE_PROC_PARAM_S *pParam, ALG_RESAMPLE_RESULT_PARAM_S *pResult);

/**
 * @brief resampler release memory
 *
 * @param pHandle : resampler's handle
 *
 * @return error code
 */
TS_S32 TS_ALG_Resample_Exit(TS_VOID *pHandle);

/**
 * @brief 1.set resampler param; 2.when set to different qualilty, should exit and reinit.
 *
 * @param pHandle : handle
 * @param pParam  : input param
 *
 * @return 0 or error code
 */
TS_S32 TS_ALG_Resample_SetParam(TS_VOID *pHandle, ALG_RESAMPLE_SET_PARAM_S *pParam);

/**
 * @brief get resampler param
 *
 * @param pHandle : Net handle
 * @param pParam  : Onput param
 *
 * @return 0 or error code
 */
TS_S32 TS_ALG_Resample_GetParam(TS_VOID *pHandle, ALG_RESAMPLE_SET_PARAM_S *pParam);

/**
 * @brief resampler get version
 *
 * @param void
 *
 * @return version
 */
const TS_CHAR* TS_ALG_Resample_GetVersion(TS_VOID);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* _TS_ALG_RESAMPLE_H_ */

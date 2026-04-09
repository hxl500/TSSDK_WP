/*
 * Copyright (c) Beijing Tsing Micro Co.,Ltd. All rights reserved.
 * Description: VAD
 * Author: Tsing Micro solution-application group
 * Create: 2023/02/10
 */
#ifndef _TS_ALG_VAD_H_
#define _TS_ALG_VAD_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "ts_alg_type.h"

typedef struct tsALG_VAD_INIT_S{
    TS_U32 u32SampleRate;      	// sample rate
    TS_U32 u32NumChannels;		// input channel
    TS_U32 u32FrameLen;		    // sample number per frame

	TS_S32 s32MinMaxEnv;           // to control the environment noise level, db value within [-65dB,-40dB]
    TS_S32 s32SmoothSilenceTh;     // to control the silence counter, default 15
    TS_S32 s32SmoothActiveTh;      // to control the active counter, default 5
} ALG_VAD_INIT_S;

typedef struct tsALG_VAD_PARAM_S{
	TS_U32 u32SampleNum;		// input sample number or outpuut data number
	TS_S16 *pBitStream;			// input/output  bitstream
} ALG_VAD_PARAM_S;

/**
 * @brief aac encoder initiate
 *
 * @param ppHandle : handle
 * @param pParam   : input param
 *
 * @attention : If the function fails, you also need to call the release api
 * @return error code
 */
TS_S32 TS_ALG_VAD_Init(TS_VOID **ppHandle, ALG_VAD_INIT_S *pParam);

/**
 * @brief aac encoder process
 *
 * @param pHandle   : Net handle
 * @param pImage    : input image address
 * @param pResult   : aac encoder  result
 *
 * @return error or result : < 0: error;  =0: silence;    1: voice
 */
TS_S32 TS_ALG_VAD_Process(TS_VOID *pHandle, ALG_VAD_PARAM_S *pInput);


/**
 * @brief exit aac encoder
 *
 * @param pHandle   : Net handle
 *
 * @return error code >0: no error
 */
TS_S32 TS_ALG_VAD_Exit(TS_VOID *pHandle);

/**
 * @brief aac encoder get version
 *
 * @param void
 *
 * @return version
 */
const TS_CHAR* TS_ALG_VAD_GetVersion(TS_VOID);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif 

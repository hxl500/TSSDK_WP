/*
 * Copyright (c) Beijing Tsing Micro Co.,Ltd. All rights reserved.
 * Description: the definitions of aac encoder
 * Author: Tsing Micro solution-application group
 * Create: 2023/02/01
 */
#ifndef _TS_ALG_AAC_ENC_H_
#define _TS_ALG_AAC_ENC_H_

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#include "ts_alg_type.h"

typedef struct tsALG_AAC_ENC_INIT_S {
	TS_U32 u32SampleRate;		// sample rate
	TS_U32 u32NumChannels;		// input channel
	TS_U32 u32OutputMode;		// /*0 - raw; 1 - ADTS*/
	TS_U32 *pInputSamples;		// input bytes per time
	TS_U32 *pMaxOutputBytes;	// max output bytes
} ALG_AAC_ENC_INIT_S;

typedef struct tsALG_AAC_ENC_PARAM_S {
	TS_U32 u32SampleNum;		// input/output bytes
	TS_U8  *pBitStream;			// input/output  bitstream
} ALG_AAC_ENC_PARAM_S;

/**
 * @brief aac encoder initiate
 *
 * @param ppHandle : handle
 * @param pParam   : input param
 *
 * @attention : If the function fails, you also need to call the release api
 * @return error code
 */
TS_S32 TS_ALG_AAC_ENC_Init(TS_VOID **ppHandle, ALG_AAC_ENC_INIT_S *pParam);

/**
 * @brief aac encoder process
 *
 * @param pHandle   : Net handle
 * @param pImage    : input image address
 * @param pResult   : aac encoder  result
 *
 * @return error or result : >0: face number
 */
TS_S32 TS_ALG_AAC_ENC_Process(TS_VOID *pHandle, ALG_AAC_ENC_PARAM_S *pInput, ALG_AAC_ENC_PARAM_S *pOutput);


/**
 * @brief exit aac encoder
 *
 * @param pHandle   : Net handle
 *
 * @return error code >0: no error
 */
TS_S32 TS_ALG_AAC_ENC_Exit(TS_VOID *pHandle);

/**
 * @brief aac encoder get version
 *
 * @param void
 *
 * @return version
 */
const TS_CHAR *TS_ALG_AAC_ENC_GetVersion();

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _TS_ALG_AAC_ENC_H_ */

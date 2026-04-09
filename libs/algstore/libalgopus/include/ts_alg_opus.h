/*
 * Copyright (c) Beijing Tsing Micro Co.,Ltd. All rights reserved.
 * Description: the definitions of opus
 * Author: Tsing Micro solution-application group
 * Create: 2023/09/22
 */
#ifndef _TS_ALG_OPUS_H_
#define _TS_ALG_OPUS_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "ts_alg_type.h"

typedef struct tsALG_OPUS_ENC_PARAM_S{
    TS_S32 s32SampleRate; // only support 8k / 16k
    TS_S32 s32ChanNum;    // only support 1ch / 2ch
    TS_S32 s32SampleNum;  // only support 160 / 320
    TS_S32 s32Bps;        // bit rate per second
} ALG_OPUS_ENC_PARAM_S;

typedef struct tsALG_OPUS_ENC_RESULT_S {
	TS_U32 u32ByteNum;   //byte num
	TS_U8 *pBitstream;
} ALG_OPUS_ENC_RESULT_S;

typedef struct tsALG_OPUS_DEC_PARAM_S {
    TS_S32 s32SampleRate; // only support 8k / 16k
    TS_S32 s32ChanNum;    // only support 1ch / 2ch
} ALG_OPUS_DEC_PARAM_S;

typedef struct tsALG_OPUS_DEC_RESULT_S {
    TS_U32 u32ByteNum;   //byte num
    TS_S16* pSampleData;
} ALG_OPUS_DEC_RESULT_S;

/**
 * @brief opus enc initiate
 *
 * @param ppHandle : opus enc handle
 * @param pParam   : input param
 *
 * @attention : If the function fails, you also need to call the release api
 * @return error code
 */
TS_S32 TS_ALG_OpusEnc_Init(TS_VOID **ppHandle, ALG_OPUS_ENC_PARAM_S*pParam);

/**
 * @brief opus enc process
 *
 * @param pHandle : opus handle
 * @param pImage  : input data
 * @param pResult : opus result
 *
 * @return error code
 */
TS_S32 TS_ALG_OpusEnc_Process(TS_VOID *pHandle, ALG_IMAGE_S *pImage, ALG_OPUS_ENC_RESULT_S*pResult);

/**
 * @brief opus enc release memory
 *
 * @param pHandle : opus handle
 *
 * @return error code
 */
TS_S32 TS_ALG_OpusEnc_Exit(TS_VOID *pHandle);

/**
 * @brief opus dec initiate
 *
 * @param ppHandle : opus dec handle
 * @param pParam   : input param
 *
 * @attention : If the function fails, you also need to call the release api
 * @return error code
 */
TS_S32 TS_ALG_OpusDec_Init(TS_VOID** ppHandle, ALG_OPUS_DEC_PARAM_S* pParam);

/**
 * @brief opus dec process
 *
 * @param pHandle : opus dec handle
 * @param pImage  : input data
 * @param pResult : opus result
 *
 * @return error code
 */
TS_S32 TS_ALG_OpusDec_Process(TS_VOID* pHandle, ALG_IMAGE_S* pImage, ALG_OPUS_DEC_RESULT_S* pResult);

/**
 * @brief opus release memory
 *
 * @param pHandle : opus handle
 *
 * @return error code
 */
TS_S32 TS_ALG_OpusDec_Exit(TS_VOID* pHandle);

/**
 * @brief opus get version
 *
 * @param void
 *
 * @return version
 */
const TS_CHAR* TS_ALG_Opus_GetVersion();

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

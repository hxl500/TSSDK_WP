/*
 * Copyright (c) Beijing Tsing Micro Co.,Ltd. All rights reserved.
 * Description: the definitions of aac decoder
 * Author: Tsing Micro solution-application group
 * Create: 2023/02/04
 */
#ifndef _TS_ALG_AAC_DEC_H_
#define _TS_ALG_AAC_DEC_H_

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#include "ts_alg_type.h"

typedef struct tsALG_AAC_DEC_INIT_S{
	TS_U8  *pBitStream;			// input bitstream
	TS_U32 u32ByteNum;		    // input byte number
	TS_U32 u32SampleRate;		// output sample rate
	TS_U32 u32NumChannels;		// output channel
} ALG_AAC_DEC_INIT_S;

typedef struct tsALG_AAC_DEC_PARAM_S{
	TS_U32 u32InputByteNum;		// input byte number
	TS_U8  *pBitStream;		    // input bitstream
	TS_U32 u32DecoderByteNum;	// output decoder byte number
	TS_U8  *pOutStream;		    // output sample data
	TS_U32 u32OutputSamples;		// output samples number per time
} ALG_AAC_DEC_PARAM_S;

/**
 * @brief aac encoder initiate
 *
 * @param ppHandle : handle
 * @param pParam   : input param
 *
 * @attention : If the function fails, you also need to call the release api
 * @return error code
 */
TS_S32 TS_ALG_AAC_DEC_Init(TS_VOID **ppHandle, ALG_AAC_DEC_INIT_S *pParam);

/**
 * @brief aac encoder process
 *
 * @param pHandle   : Net handle
 * @param pImage    : input image address
 * @param pResult   : aac encoder  result
 *
 * @return error or result : >0: face number
 */
TS_S32 TS_ALG_AAC_DEC_Process(TS_VOID *pHandle, ALG_AAC_DEC_PARAM_S *pParam);


/**
 * @brief exit aac encoder
 *
 * @param pHandle   : Net handle
 *
 * @return error code >0: no error
 */
TS_S32 TS_ALG_AAC_DEC_Exit(TS_VOID *pHandle);

/**
 * @brief aac encoder get version
 *
 * @param void
 *
 * @return version
 */
const TS_CHAR* TS_ALG_AAC_DEC_GetVersion(TS_VOID);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _TS_ALG_AAC_DEC_H_ */

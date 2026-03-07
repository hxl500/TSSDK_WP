#ifndef _TS_ALG_KWSTP_H_
#define _TS_ALG_KWSTP_H_

#include "ts_alg_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define KWSTP_MVersion 35

#define KWSTP_RSLT_NULL (-1)
#define KWSTP_RSLT_WAIT (-2)

typedef struct
{
	TS_S32 nn_out_size;
	TS_S32 nn_out_Q;
	TS_S32 mul_scale;
	TS_FLOAT asr_score_thresh;
	TS_FLOAT sensitive_score_thresh;
	TS_FLOAT test_score_thresh;
	TS_S32 asr_post_smooth_length;
	TS_S32 average_state_frames;
	TS_FLOAT wakeup_score_thresh;
	TS_S32 average_wakeup_state_frames;
	TS_S32 number_state;
	TS_S32 start_state;
	TS_S32 decoder_mode;
	TS_S32 wakeup_id;
	TS_S32 Cmd_id;
	TS_FLOAT cmd_score_thresh;
	TS_S32 average_cmd_state_frames;
	TS_S32 frames_per_second;
	TS_S32 sensitive_duration;
	TS_S32 wakeup_duration;
	TS_FLOAT sil_score_thresh;
	TS_FLOAT *pAsrScoreThreshMin;
	TS_FLOAT *pAsrScoreThreshMax;

	TS_S16 *pfillerflag;
	TS_S16 *pfinalflag;
	TS_S16 *pArcOffset;
	TS_S16 *pIlabel;
	TS_S16 *pOlabel;
	TS_S16 *pNextState;
}ZkyNieParam;

/**
 * @brief ALG_ASR_STR_S structure: for ASR algorithm initialization structure
 */
typedef struct
{
	ZkyNieParam * pZkyNieParam;

	/**
	 * @brief pConvCfg: model's config paramter
	 */
	TS_U8 *pCfg;

	/**
	 * @brief pConvWeight: model's weight paramter
	 */
	TS_U8 *pWeight;

	/**
	 * @brief pConvWeight: model's buffer addr
	 */
	TS_VOID *pSelfBuf;

	/**
	 * @brief s32ConvCfgLen: model's config paramter byte number
	 */
	TS_S32 s32CfgLen;

	/**
	 * @brief pCfg: model's config paramter
	 */
	TS_U8 *pFbankCfg;

	/**
	 * @brief pWeight: model's weight paramter
	 */
	TS_U8 *pFbankWeight;

	/**
	 * @brief pWeight: model's buffer addr
	 */
	TS_VOID *pFbankSelfBuf;

	/**
	 * @brief s32CfgLen: model's config paramter byte number
	 */
	TS_S32 s32FbankCfgLen;

	/**
	 * @brief smplPerSec: sample rating
	 */
	TS_S32 s32SmplRatio;

	/**
	 * @brief smplPerInt: samples number per interrupt
	 */
	TS_S16 s16FrameLen;
}ALG_KWSTP_INIT_S;

typedef struct{
	TS_S16 *pIn;
	TS_U32 u32InLen;
}ALG_KWSTP_PROC_S;

typedef struct{
	TS_S32 s32Result;
	TS_FLOAT fscore;
} ALG_KWSTP_RSLT_S;

typedef struct{
	TS_S32 s32IsRneNotOff; // 1: exit won't do TS_MPI_TRP_RNE_CloseDevice; others: does, default: ctrl
} ALG_KWSTP_SET_PARAM_S;

/**
 * @brief kws initiate
 * only support sample ratio:8K
 *
 * @param ppHandle : handle
 * @param pParam   : input param
 *
 * @attention : If the function fails, you also need to call the release api
 * @return error code
 */
TS_S32 TS_ALG_KwsTp_Init(TS_VOID **ppHandle, ALG_KWSTP_INIT_S *pParam);

/**
 * @brief kws  process
 *
 * @param pHandle : handle
 * @param pInput : input parameters instructure
 * @param pOutput : output parameters instructure
 *
 * @return error or result : 0: sucess;  others: error;
 */
TS_S32 TS_ALG_KwsTp_Process(TS_VOID *pHandle, ALG_KWSTP_PROC_S *pInput, ALG_KWSTP_RSLT_S *pOutput);

/**
 * @brief exit kws
 *
 * @param pHandle : handle
 *
 * @return error code 0: success; others: error
 */
TS_S32 TS_ALG_KwsTp_Exit(TS_VOID *pHandle);

TS_S32 TS_ALG_KwsTp_SetParam(TS_VOID* pHandle, ALG_KWSTP_SET_PARAM_S *pConfig);

TS_S32 TS_ALG_KwsTp_GetParam(TS_VOID* pHandle, ALG_KWSTP_SET_PARAM_S *pConfig);

/**
 * @brief kws get version
 *
 * @param void
 *
 * @return version
 */
const TS_CHAR* TS_ALG_KwsTp_GetVersion(TS_VOID);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _TS_ALG_KWSTP_H_ */

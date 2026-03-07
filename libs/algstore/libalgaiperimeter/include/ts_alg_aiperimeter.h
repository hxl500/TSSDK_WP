/**
 * @file 	ts_alg_byte_track.h
 * @author 	yhc
 * @brief 	ai 周界 
 * @version 1.0
 * @date 	2024-04-09
 *
 * @copyright Copyright (c) 2022 tsingmicro
 *
 */

#ifndef _TS_ALG_AI_PERIMETER_H_
#define _TS_ALG_AI_PERIMETER_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "ts_alg_type.h"
#include "ts_alg_pbcedetect.h"
#include "ts_alg_pts.h"

//#define MAX_AI_PERIMETER_NUM     25		// max face detect number

typedef struct tsALG_AI_PERIMETER_INIT_S {
	TS_U32 u32OriImgW;
	TS_U32 u32OriImgH;
	ALG_MODEL_INIT_S stBodyDet;     // pbce detect
	ALG_PTS_INPUT_S stPts;
	TS_U8 bRegionalInstrusion;		// 使能区域入侵 0 不使能 1 使能
	TS_U8 bCrossBorderDetection;	// 使能越界 0 不使能 1 双向使能 2 上单向使能 3 下单向使能
	TS_U8 bEnclosure;				// 使能围栏 0 不使能 1 使能
	TS_U32 roiRect[4];				// 入侵区域或围栏区域 左上角x,y 右下角x,y
	TS_U8 bRneOff;			        // not open RNE device, revent multiple operations
} ALG_AI_PERIMETER_INIT_S;

typedef struct tsALG_AI_PERIMETER_PARAM_S{
	ALG_PBCEDETECT_PARAM_S bodyParam;	// 0.25 0.45 夜晚 0.1 0.45 现在程序里默认是0.4
	ALG_PTS_PARAM_S ptsParam;			// 20 0.4 0.9 夜晚 20 0.1 0.9
	TS_U8 bRegionalInstrusion;			// 使能区域入侵 0 不使能 1 使能
	TS_U8 bCrossBorderDetection;		// 使能越界 0 不使能 1 双向使能 2 上单向使能 3 下单向使能
	TS_U8 bEnclosure;					// 使能围栏 0 不使能 1 使能
	TS_U32 roiRect[4];					// 入侵区域或围栏区域 左上角x,y 右下角x,y
} ALG_AI_PERIMETER_PARAM_S;

typedef struct tsALG_AI_PERIMETER_RESULTS_S {
	TS_U32 u32Num;
	TS_U8  u8AlarmType;					// 0 无告警 1 区域入侵 2 越界侦测 3 围栏
	ALG_PBCEDETECT_RESULT_S stBodydet;
	ALG_PTS_RESULTS_S stPts;
	ALG_PTS_S alarm;					//报警的框
} ALG_AI_PERIMETER_RESULTS_S;

/**
 * @brief ai perimeter init
 * @param ppHandle : Net handle
 * @param pParam   : input param
 * @attention : If the function fails, you also need to call the release api
 * @return error code
 */
TS_S32 TS_ALG_AiPerimeter_Init(TS_VOID **ppHandle, ALG_AI_PERIMETER_INIT_S *pParam);

/**
 * @brief ai perimeter process
 *
 * @param pHandle   : Net handle
 * @param pImage    : input face address (w*h = 640*384)
 * @param pResult   : ai perimeter result
 *
 * @return error code : =0: ok   <0: error
 */
TS_S32 TS_ALG_AiPerimeter_Process(TS_VOID *pHandle, ALG_IMAGE_S *pOriImage, ALG_IMAGE_S *pImage, ALG_AI_PERIMETER_RESULTS_S *pResult);

/**
 * @brief ai perimeter exit
 * @param pHandle   : Net handle
 * @return error code : =0: ok   <0: error
 */
TS_S32 TS_ALG_AiPerimeter_Exit(TS_VOID *pHandle);

/**
 * @brief set ai perimeter param
 * @param pHandle : Net handle
 * @param pParam  : ai perimeter thresh
 *
 * @return 0 or error code
 */
TS_S32 TS_ALG_AiPerimeter_SetParam(TS_VOID *pHandle, ALG_AI_PERIMETER_PARAM_S *pParam);

/**
 * @brief get ai perimeter param
 *
 * @param pHandle : Net handle
 * @param pParam  : ai perimeter thresh
 *
 * @return 0 or error code
 */
TS_S32 TS_ALG_AiPerimeter_GetParam(TS_VOID *pHandle, ALG_AI_PERIMETER_PARAM_S *pParam);

/**
 * @brief ai perimeter get version
 * @return version string
 */
const TS_CHAR *TS_ALG_AiPerimeter_GetVersion();


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif		// _TS_ALG_CAR_SOL_H_


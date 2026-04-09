/**
 * @file 	ts_alg_pts_solution.h
 * @author 	yhc
 * @brief 	Passenger traffic statistics track
 * @version 1.0
 * @date 	2023-10-13
 *
 * @copyright Copyright (c) 2022 tsingmicro
 *
 */

#ifndef _TS_ALG_PTS_SLN_H_
#define _TS_ALG_PTS_SLN_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "ts_alg_type.h"
#include "ts_alg_pbcedetect.h"
#include "ts_alg_pts.h"

#define MAX_PTS_SOL_NUM     25		// max face detect number

/**
 * 关于人车非检测与车辆方案的说明
 * 1、当bByteTrack为TS_TRUE时，车辆方案模式打开
 * 2、当bByteTrack为TS_FALSE时，车辆方案模式关闭，只开启人车非检测模式
*/
typedef struct tsALG_PTS_SLN_INIT_S {
	TS_U32 u32OriImgW;
	TS_U32 u32OriImgH;

	ALG_MODEL_INIT_S stPbceDet;     	// pbce detect
	ALG_PTS_INPUT_S stPts;
	TS_U8 bRneOff;			        // not open RNE device, revent multiple operations
} ALG_PTS_SLN_INIT_S;

typedef struct tsALG_PTS_SLN_PARAM_S{
	ALG_PBCEDETECT_PARAM_S		pbceParam;			// 0.25 0.45 夜晚 0.1 0.45 现在程序里默认是0.4
	ALG_PTS_PARAM_S		ptsParam;		// 20 0.4 0.9 夜晚 20 0.1 0.9
} ALG_PTS_SLN_PARAM_S;

typedef struct tsALG_PTS_SLN_RESULTS_S {
	TS_U32 u32Num;
	ALG_PBCEDETECT_RESULT_S 	stPbceDet;
	ALG_PTS_RESULTS_S 			stPts;
} ALG_PTS_SLN_RESULTS_S;

/**
 * @brief multi object track init
 *
 * @param ppHandle : Net handle
 * @param pParam   : input param
 *
 * @attention : If the function fails, you also need to call the release api
 * @return error code
 */
TS_S32 TS_ALG_PtsSolution_Init(TS_VOID **ppHandle, ALG_PTS_SLN_INIT_S *pParam);

/**
 * @brief multi object track process
 *
 * @param pHandle   : Net handle
 * @param pImage    : input face address (w*h = 640*384)
 * @param pResult   : multi object track result
 *
 * @return error code : =0: ok   <0: error
 */
TS_S32 TS_ALG_PtsSolution_Process(TS_VOID *pHandle, ALG_IMAGE_S *pOriImage, ALG_IMAGE_S *pImage, ALG_PTS_SLN_RESULTS_S *pResult);

/**
 * @brief multi object track exit
 *
 * @param pHandle   : Net handle
 *
 * @return error code : =0: ok   <0: error
 */
TS_S32 TS_ALG_PtsSolution_Exit(TS_VOID *pHandle);

/**
 * @brief set multi object track param
 *
 * @param pHandle : Net handle
 * @param pParam  : obj detect thresh
 *
 * @return 0 or error code
 */
TS_S32 TS_ALG_PtsSolution_SetParam(TS_VOID *pHandle, ALG_PTS_SLN_PARAM_S *pParam);

/**
 * @brief get multi object track param
 *
 * @param pHandle : Net handle
 * @param pParam  : obj detect thresh
 *
 * @return 0 or error code
 */
TS_S32 TS_ALG_PtsSolution_GetParam(TS_VOID *pHandle, ALG_PTS_SLN_PARAM_S *pParam);

/**
 * @brief multi object track get version
 *
 * @return version string
 */
const TS_CHAR *TS_ALG_PtsSolution_GetVersion();


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif		// _TS_ALG_CAR_SOL_H_


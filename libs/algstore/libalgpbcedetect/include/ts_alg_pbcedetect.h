/*
 * Copyright (c) Beijing Tsing Micro Co.,Ltd. All rights reserved.
 * Description: the definitions of pcpp(pedestrian, car, pet, package) detection
 * Author: Tsing Micro solution-application group
 * Create: 2023/11/10
 */
#ifndef _TS_ALG_PBCEDETECT_H_
#define _TS_ALG_PBCEDETECT_H_

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#include "ts_alg_type.h"
#include "ts_alg_info.h"

#define PBCEDETECT_MVersion 11

#define MAX_PBCE_DETECT_NUM     25		// max obj detect number

// 人 自行车 车 电动车 三轮车
typedef enum tsALG_PBCE_TYPE_E {
	ALG_PBCE_DET_PEDESTRIAN=0,
	ALG_PBCE_DET_BICYCLE,
	ALG_PBCE_DET_CAR,
	ALG_PBCE_DET_EBIKE,
	ALG_PBCE_DET_ETRICYCLE,
	ALG_PBCE_DET_NUM
} ALG_PBCE_TYPE_E;

typedef struct tsALG_PBCEDETECT_BOX_S {
    // the range of x and y is 0.0f~1.0f
    TS_FLOAT f32Xmin;
    TS_FLOAT f32Ymin;
    TS_FLOAT f32Xmax;
    TS_FLOAT f32Ymax;
    TS_FLOAT f32Score;
    TS_FLOAT f32Reserve;
} ALG_PBCEDETECT_BOX_S;

typedef struct tsALG_PBCEDETECT_RESULT_S {
	TS_U32 u32ObjNum;
	ALG_PBCE_TYPE_E u8ObjNo[MAX_PBCE_DETECT_NUM];
	ALG_PBCEDETECT_BOX_S stBox[MAX_PBCE_DETECT_NUM];
} ALG_PBCEDETECT_RESULT_S;

typedef struct tsALG_PBCEDETECT_PARAM_S {
	TS_FLOAT f32ConfThresh;	//default 0.4
	TS_FLOAT f32IouThresh;	//default 0.45
} ALG_PBCEDETECT_PARAM_S;

/**
 * @brief pbce detect solution initiate
 *
 * @param ppHandle : pbce handle
 * @param pParam   : input param
 *
 * @attention : If the function fails, you also need to call the release api
 * @return error code
 */
TS_S32 TS_ALG_PbceDetect_Init(TS_VOID **ppHandle, ALG_MODEL_INIT_S *pParam);

/**
 * @brief pbce detect process
 *
 * @param pHandle : pbce detect handle
 * @param pImage  : input image address
 * @param pResult : pbce detect result
 *
 * @return error code
 */
TS_S32 TS_ALG_PbceDetect_Process(TS_VOID *pHandle, ALG_IMAGE_S *pImage, ALG_PBCEDETECT_RESULT_S *pResult);

/**
 * @brief pbce detect release memory
 *
 * @param pHandle : pbce detect handle
 *
 * @return error code
 */
TS_S32 TS_ALG_PbceDetect_Exit(TS_VOID *pHandle);

/**
 * @brief set pbce detect param
 *
 * @param pHandle : Net handle
 * @param pParam  : input param
 *
 * @return 0 or error code
 */
TS_S32 TS_ALG_PbceDetect_SetParam(TS_VOID *pHandle, ALG_PBCEDETECT_PARAM_S *pParam);

/**
 * @brief get pbce detect param
 *
 * @param pHandle : Net handle
 * @param pParam  : input param
 *
 * @return 0 or error code
 */
TS_S32 TS_ALG_PbceDetect_GetParam(TS_VOID *pHandle, ALG_PBCEDETECT_PARAM_S *pParam);

/**
 * @brief pbce detect get version
 *
 * @param void
 *
 * @return version
 */
const TS_CHAR *TS_ALG_PbceDetect_GetVersion();

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* _TS_ALG_PCPP_DETECT_H_ */

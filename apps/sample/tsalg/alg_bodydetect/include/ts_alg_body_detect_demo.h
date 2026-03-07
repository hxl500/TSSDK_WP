/*
 * Copyright (c) Beijing Tsing Micro Co.,Ltd. All rights reserved.
 * Description: the definitions of body detection
 * Author: Tsing Micro solution-application group
 * Create: 2022/08/24
 */
#ifndef  _TS_ALG_BODY_DETECT_DEMO_H_
#define _TS_ALG_BODY_DETECT_DEMO_H_

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#include "ts_alg_type.h"

#define MAX_BODY_DETECT_NUM     25		// max body detect number


typedef struct tsALG_BODY_BOX_S{
    // the range of x and y is 0.0f~1.0f
    TS_FLOAT f32Xmin;
    TS_FLOAT f32Ymin;
    TS_FLOAT f32Xmax;
    TS_FLOAT f32Ymax;
    TS_FLOAT f32Score;
    TS_FLOAT f32Reserve;
} ALG_BODY_BOX_S;

typedef struct tsALG_BODY_RESULT_S{
	TS_U32 u32BodyNum;
	ALG_BODY_BOX_S stBox[MAX_BODY_DETECT_NUM];
} ALG_BODY_RESULT_S;

typedef struct tsALG_BODY_PARAM_S{
	TS_FLOAT f32Thresh;
} ALG_BODY_PARAM_S;

/**
 * @brief body detect solution initiate
 *
 * @param ppHandle : bodydetect handle
 * @param pParam   : input param
 *
 * @attention : If the function fails, you also need to call the release api
 * @return error code
 */
TS_S32 TS_ALG_BodyDetect_Init(TS_VOID **ppHandle, ALG_MODEL_INIT_S *pParam);

/**
 * @brief body detect process
 *
 * @param pHandle : body detect handle
 * @param pImage  : input image address
 * @param pResult : body detect result
 *
 * @return error code
 */
TS_S32 TS_ALG_BodyDetect_Process(TS_VOID *pHandle, ALG_IMAGE_S *pImage, ALG_BODY_RESULT_S *pResult);

/**
 * @brief body detect release memory
 *
 * @param pHandle : body detect handle
 *
 * @return error code
 */
TS_S32 TS_ALG_BodyDetect_Exit(TS_VOID *pHandle);

/**
 * @brief set body detect param
 *
 * @param pHandle : Net handle
 * @param pParam  : input param
 *
 * @return 0 or error code
 */
TS_S32 TS_ALG_BodyDetect_SetParam(TS_VOID *pHandle, ALG_BODY_PARAM_S *pParam);

/**
 * @brief get body detect param
 *
 * @param pHandle : Net handle
 * @param pParam  : input param
 *
 * @return 0 or error code
 */
TS_S32 TS_ALG_BodyDetect_GetParam(TS_VOID *pHandle, ALG_BODY_PARAM_S *pParam);

/**
 * @brief body detect get version
 *
 * @param void
 *
 * @return version
 */
const TS_CHAR* TS_ALG_BodyDetect_GetVersion(TS_VOID);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* _TS_ALG_BODY_DETECT_H_ */

/*
 * Copyright (c) Beijing Tsing Micro Co.,Ltd. All rights reserved.
 * Description: the definitions of body detection
 * Author: Tsing Micro solution-application group
 * Create: 2022/08/24
 */
#ifndef _TS_ALG_BODY_DETECT_H_
#define _TS_ALG_BODY_DETECT_H_

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#include "ts_alg_type.h"
#include "ts_alg_info.h"

#define BODYDETECT_MVersion    16

#define MAX_BODYDET_NUM     25		 // max body detect number


typedef struct tsALG_BODYDET_BOX_S{
    // the range of x and y is 0.0f~1.0f
    TS_FLOAT f32Xmin;
    TS_FLOAT f32Ymin;
    TS_FLOAT f32Xmax;
    TS_FLOAT f32Ymax;
    TS_FLOAT f32Score;
    TS_FLOAT f32Reserve;
} ALG_BODYDET_BOX_S;

typedef struct tsALG_BODYDET_RESULT_S{
	TS_U32 u32BodyNum;
	ALG_BODYDET_BOX_S stBox[MAX_BODYDET_NUM];
} ALG_BODYDET_RESULT_S;

typedef struct tsALG_BODYDET_PARAM_S{
	TS_FLOAT f32Thresh;
} ALG_BODYDET_PARAM_S;


/**
 * @brief body detect initiate
 *
 * @param ppHandle : bodydetect handle
 * @param pParam   : input param
 *
 * @attention : If the function fails, you also need to call the release api
 * @return error code
 */

#ifdef RNE_0COPY
TS_S32 TS_ALG_BodyDet_Init(TS_VOID **handle, ALG_MODEL_INIT_S *param, ALG_IMAGE_S *image);
#else
TS_S32 TS_ALG_BodyDet_Init(TS_VOID **handle, ALG_MODEL_INIT_S *param);
#endif

/**
 * @brief body detect process
 *
 * @param pHandle : body detect handle
 * @param pImage  : input image address
 * @param pResult : body detect result
 *
 * @return error code
 */
TS_S32 TS_ALG_BodyDet_Process(TS_VOID *pHandle, ALG_IMAGE_S *pImage, ALG_BODYDET_RESULT_S *pResult);

/**
 * @brief body detect release memory
 *
 * @param pHandle : body detect handle
 *
 * @return error code
 */
TS_S32 TS_ALG_BodyDet_Exit(TS_VOID *pHandle);

/**
 * @brief set body detect param
 *
 * @param pHandle : Net handle
 * @param pParam  : input param
 *
 * @return 0 or error code
 */
TS_S32 TS_ALG_BodyDet_SetParam(TS_VOID *pHandle, ALG_BODYDET_PARAM_S *pParam);

/**
 * @brief get body detect param
 *
 * @param pHandle : Net handle
 * @param pParam  : input param
 *
 * @return 0 or error code
 */
TS_S32 TS_ALG_BodyDet_GetParam(TS_VOID *pHandle, ALG_BODYDET_PARAM_S *pParam);


/**
 * @brief body detect get version
 *
 * @param void
 *
 * @return version
 */
const TS_CHAR* TS_ALG_BodyDet_GetVersion();

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif

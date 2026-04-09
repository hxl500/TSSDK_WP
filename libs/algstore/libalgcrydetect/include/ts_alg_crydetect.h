/*
 * Copyright (c) Beijing Tsing Micro Co.,Ltd. All rights reserved.
 * Description: the definitions of cry detection
 * Author: Tsing Micro solution-application group
 * Create: 2023/08/03
 */
#ifndef _TS_ALG_CRYDETECT_H_
#define _TS_ALG_CRYDETECT_H_

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#include "ts_alg_type.h"
#include "ts_alg_info.h"

#define CRYDETECT_MVersion 10

typedef struct tsALG_CRY_RESULT_S{
	TS_S32 s32CryRes;	// 0: no cry;  1: cry
} ALG_CRY_RESULT_S;

typedef struct tsALG_CRY_PARAM_S{
	TS_FLOAT f32Thresh;
} ALG_CRY_PARAM_S;

/**
 * @brief cry detect solution initiate
 *
 * @param ppHandle : cry detect handle
 * @param pParam   : input param
 *
 * @attention : If the function fails, you also need to call the release api
 * @return error code
 */
TS_S32 TS_ALG_CryDetect_Init(TS_VOID **ppHandle, ALG_MODEL_INIT_S *pParam);

/**
 * @brief cry detect process
 *
 * @param pHandle : cry detect handle
 * @param pImage  : input image address
 * @param pResult : cry detect result
 *
 * @return error code
 */
TS_S32 TS_ALG_CryDetect_Process(TS_VOID *pHandle, ALG_IMAGE_S *pImage, ALG_CRY_RESULT_S *pResult);

/**
 * @brief cry detect release memory
 *
 * @param pHandle : cry detect handle
 *
 * @return error code
 */
TS_S32 TS_ALG_CryDetect_Exit(TS_VOID *pHandle);

/**
 * @brief set cry detect param, just reserve this api, not used
 *
 * @param pHandle : Net handle
 * @param pParam  : input param
 *
 * @return 0 or error code
 */
TS_S32 TS_ALG_CryDetect_SetParam(TS_VOID *pHandle, ALG_CRY_PARAM_S *pParam);

/**
 * @brief get cry detect param, just reserve this api, not used
 *
 * @param pHandle : Net handle
 * @param pParam  : input param
 *
 * @return 0 or error code
 */
TS_S32 TS_ALG_CryDetect_GetParam(TS_VOID *pHandle, ALG_CRY_PARAM_S *pParam);

/**
 * @brief cry detect get version
 *
 * @param void
 *
 * @return version
 */
const TS_CHAR* TS_ALG_CryDetect_GetVersion(TS_VOID);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif 

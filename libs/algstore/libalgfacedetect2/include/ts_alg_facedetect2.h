/**
 * @file ts_alg_facedetect2.h
 * @author lml
 * @brief face detect2
 * @date 2022-08-24
 *
 * @copyright Copyright (c) 2022 tsingmicro
 *
 */
#ifndef _TS_ALG_FACEDETECT2_H_
#define _TS_ALG_FACEDETECT2_H_

#include "ts_alg_type.h"
#include "ts_alg_info.h"

#ifdef __cplusplus
extern "C" {
#endif

#define FACEDETECT2_MVersion 17

#define MAX_FACEDET2_NUM     25		// max face detect number

typedef struct tsALG_FACEDET2_BOX_S{
    // the range of x and y is 0.0f~1.0f
    TS_FLOAT f32Xmin;
    TS_FLOAT f32Ymin;
    TS_FLOAT f32Xmax;
    TS_FLOAT f32Ymax;
    TS_FLOAT f32Score;
    TS_FLOAT f32Reserve;
} ALG_FACEDET2_BOX_S;

typedef struct tsALG_FACEDET2_RESULT_S{
	TS_U32 u32FaceNum;
	ALG_FACEDET2_BOX_S stBox[MAX_FACEDET2_NUM];
} ALG_FACEDET2_RESULT_S;

typedef struct tsALG_FACEDET2_PARAM_S{
	TS_FLOAT f32Thresh;
} ALG_FACEDET2_PARAM_S;


/**
 * @brief face detect init
 *
 * @param ppHandle : Net handle
 * @param pParam   : input param
 *
 * @attention : If the function fails, you also need to call the release api
 * @return error code
 */
TS_S32 TS_ALG_FaceDetect2_Init(TS_VOID **ppHandle, ALG_MODEL_INIT_S *pParam);

/**
 * @brief face detect process
 *
 * @param pHandle   : Net handle
 * @param pImage    : input image address
 * @param pResult   : Face detect result
 *
 * @return error or result : >0: face number
 */
TS_S32 TS_ALG_FaceDetect2_Process(TS_VOID *pHandle, ALG_IMAGE_S *pImage, ALG_FACEDET2_RESULT_S *pResult);

/**
 * @brief exit face detect
 *
 * @param pHandle   : Net handle
 *
 * @return error or result : >0: face number
 */
TS_S32 TS_ALG_FaceDetect2_Exit(TS_VOID *pHandle);

/**
 * @brief set face detect param
 *
 * @param pHandle : Net handle
 * @param pParam  : input param
 *
 * @return 0 or error code
 */
TS_S32 TS_ALG_FaceDetect2_SetParam(TS_VOID *pHandle, ALG_FACEDET2_PARAM_S *pParam);

/**
 * @brief get face detect param
 *
 * @param pHandle : Net handle
 * @param pParam  : input param
 *
 * @return 0 or error code
 */
TS_S32 TS_ALG_FaceDetect2_GetParam(TS_VOID *pHandle, ALG_FACEDET2_PARAM_S *pParam);

/**
 * @brief face detect get version
 *
 * @return version string
 */
const TS_CHAR* TS_ALG_FaceDetect2_GetVersion();

#ifdef __cplusplus
}
#endif

#endif

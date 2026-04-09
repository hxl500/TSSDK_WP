/*
 * Copyright (c) Beijing Tsing Micro Co.,Ltd. All rights reserved.
 * Description: the definitions of body detection
 * Author: Tsing Micro solution-application group
 * Create: 2023/03/10
 */
#ifndef _TS_ALG_MOTIONDETECT_H_
#define _TS_ALG_MOTIONDETECT_H_

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#include "ts_alg_type.h"
#include "ts_alg_log.h"

#define MAX_MOTION_DETECT_NUM     100		// max detect number

typedef struct tsALG_MOTION_DETECT_INIT_S {
	TS_U32 u32ImgH;
	TS_U32 u32ImgW;
	TS_U32 u32ImgC;
} ALG_MOTION_DETECT_INIT_S;

typedef struct tsALG_MOTION_DETECT_BOX_S {
	// the range of x and y is 0.0f~1.0f
	TS_FLOAT f32Xmin;
	TS_FLOAT f32Ymin;
	TS_FLOAT f32Xmax;
	TS_FLOAT f32Ymax;
	TS_FLOAT f32Score;
	TS_FLOAT f32Reserve;
} ALG_MOTION_DETECT_BOX_S;

typedef struct tsALG_MOTION_DETECT_RESULT_S {
	TS_U32 u32ObjNum;
	ALG_MOTION_DETECT_BOX_S stBox[MAX_MOTION_DETECT_NUM];
} ALG_MOTION_DETECT_RESULT_S;

typedef struct tsALG_MOTION_DETECT_PARAM_S {
	TS_U32 u32DiffThres;	// diff thres default: 20
	TS_U32 u32AreaThres;	// obj area = obj_w * obj_h; default : 100
} ALG_MOTION_DETECT_PARAM_S;

/**
 * @brief motion detect solution initiate
 *
 * @param ppHandle : detect handle
 * @param pParam   : input param
 *
 * @attention : If the function fails, you also need to call the release api
 * @return error code
 */
TS_S32 TS_ALG_MotionDetect_Init(TS_VOID **ppHandle, ALG_MOTION_DETECT_INIT_S *pParam);

/**
 * @brief motion detect process
 *
 * @param pHandle : detect handle
 * @param pImage  : input image struct
 * @param pResult : detect result
 *
 * @return error code
 */
int TS_ALG_MotionDetect_Process(TS_VOID *pHandle, ALG_IMAGE_S *pImage, ALG_MOTION_DETECT_RESULT_S *pResult);

/**
 * @brief motion detect release memory
 *
 * @param pHandle : detect handle
 *
 * @return error code
 */
TS_S32 TS_ALG_MotionDetect_Exit(TS_VOID *pHandle);

/**
 * @brief set motion detect param
 *
 * @param pHandle : detect handle
 * @param pParam  : input param
 *
 * @return 0 or error code
 */
TS_S32 TS_ALG_MotionDetect_SetParam(TS_VOID *pHandle, ALG_MOTION_DETECT_PARAM_S *pParam);

/**
 * @brief get motion detect param
 *
 * @param pHandle : detect handle
 * @param pParam  : output param
 *
 * @return 0 or error code
 */
TS_S32 TS_ALG_MotionDetect_GetParam(TS_VOID *pHandle, ALG_MOTION_DETECT_PARAM_S *pParam);

/**
 * @brief motion detect get version
 *
 * @param void
 *
 * @return version
 */
const TS_CHAR *TS_ALG_MotionDetect_GetVersion();

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /*  */

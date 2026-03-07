/*
 * Copyright (c) Beijing Tsing Micro Co.,Ltd. All rights reserved.
 * Description: the definitions of pcpp(pedestrian, car, pet, package) detection
 * Author: Tsing Micro solution-application group
 * Create: 2023/07/26
 */
#ifndef _TS_ALG_PCPP_DETECT_H_
#define _TS_ALG_PCPP_DETECT_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "ts_alg_type.h"
#include "ts_alg_info.h"


#define PCPPDETECT_MVersion 11

// #define RNE_0COPY

#define CPU_OPTIMIZE

#define MAX_PCPP_DET_NUM     25		// max obj detect number

enum{
	ALG_PCPP_DET_PEDESTRIAN,
	ALG_PCPP_DET_CAR,
	ALG_PCPP_DET_PET,
	ALG_PCPP_DET_PACKAGE,
	ALG_PCPP_DET_NUM
};

typedef struct tsALG_PCPP_DET_BOX_S {
    // the range of x and y is 0.0f~1.0f
    TS_FLOAT f32Xmin;
    TS_FLOAT f32Ymin;
    TS_FLOAT f32Xmax;
    TS_FLOAT f32Ymax;
    TS_FLOAT f32Score;
    TS_FLOAT f32Reserve;
} ALG_PCPP_DET_BOX_S;

typedef struct tsALG_PCPP_DET_RESULT_S {
	TS_U32 u32ObjNum;
	ALG_PCPP_DET_BOX_S stBox[MAX_PCPP_DET_NUM];

	TS_U8 u8ObjNo[MAX_PCPP_DET_NUM];
} ALG_PCPP_DET_RESULT_S;

typedef struct tsALG_PCPP_DET_PARAM_S {

	TS_FLOAT f32PedThresh;	//default 0.4
	TS_FLOAT f32CarThresh;	//default 0.4
	TS_FLOAT f32PetThresh;	//default 0.4
	TS_FLOAT f32PckThresh;	//default 0.4
	TS_FLOAT f32IouThresh;	//default 0.45
} ALG_PCPP_DET_PARAM_S;

/**
 * @brief pcpp detect solution initiate
 *
 * @param ppHandle : pcpp handle
 * @param pParam   : input param
 *
 * @attention : If the function fails, you also need to call the release api
 * @return error code
*/
TS_S32 TS_ALG_PcppDet_Init(TS_VOID **ppHandle, ALG_MODEL_INIT_S *pParam);


/**
 * @brief pcpp detect solution initiate
 *
 * @param ppHandle : pcpp handle
 * @param pParam   : input param
 * @param image   : input image mem
 *
 * @attention : If the function fails, you also need to call the release api
 * @return error code
 */
TS_S32 TS_ALG_PcppDet_Init_MemFromRne(TS_VOID **handle, ALG_MODEL_INIT_S *param, ALG_IMAGE_S *image);

/**
 * @brief pcpp detect process
 *
 * @param pHandle : pcpp detect handle
 * @param pImage  : input image address
 * @param pResult : pcpp detect result
 *
 * @return error code
 */
TS_S32 TS_ALG_PcppDet_Process(TS_VOID *pHandle, ALG_IMAGE_S *pImage, ALG_PCPP_DET_RESULT_S *pResult);

/**
 * @brief pcpp detect release memory
 *
 * @param pHandle : pcpp detect handle
 *
 * @return error code
 */
TS_S32 TS_ALG_PcppDet_Exit(TS_VOID *pHandle);

/**
 * @brief set pcpp detect param
 *
 * @param pHandle : Net handle
 * @param pParam  : input param
 *
 * @return 0 or error code
 */
TS_S32 TS_ALG_PcppDet_SetParam(TS_VOID *pHandle, ALG_PCPP_DET_PARAM_S *pParam);

/**
 * @brief get pcpp detect param
 *
 * @param pHandle : Net handle
 * @param pParam  : input param
 *
 * @return 0 or error code
 */
TS_S32 TS_ALG_PcppDet_GetParam(TS_VOID *pHandle, ALG_PCPP_DET_PARAM_S *pParam);

/**
 * @brief pcpp detect get version
 *
 * @param void
 *
 * @return version
 */
const TS_CHAR *TS_ALG_PcppDet_GetVersion();

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif 

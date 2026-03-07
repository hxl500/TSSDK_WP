

/*
 * Copyright (c) Beijing Tsing Micro Co.,Ltd. All rights reserved.
 * Description: the definitions of common functions
 * Author: Tsing Micro solution-application group
 * Create: 2024/01/22
 */

#ifndef __VIDEO_COMMON_UTILS_H__
#define __VIDEO_COMMON_UTILS_H__

#ifdef __cplusplus
extern "C" {
#endif 

#include <stdio.h>
#include "ts_alg_type.h"

/**
 * @brief get file size by file name
 *
 * @param pf : file handle
 *
 * @return Success returns file size, failure returns 0
 */
TS_U32 SAMPLE_ALGO_GetFileSize(FILE *pf);

/**
 * @brief get model size and data pointers
 *
 * @param fileName : file name
 * @param pModelSize : return model size
 * @param ppAddr : return model data pointer potinter
 *
 * @return Success returns TS_SUCCESS, failure returns TS_FAILURE
 */
TS_S32 SAMPLE_ALG_LOAD_MODEL(const TS_CHAR* fileName, TS_U32 *pModelSize, TS_U8 **ppAddr);

#ifdef __cplusplus
}
#endif /* End of #ifdef __cplusplus */

#endif /* __VIDEO_ALG_PTS_V1_0_H__ */

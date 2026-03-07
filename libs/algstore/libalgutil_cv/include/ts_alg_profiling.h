/*
 * Copyright (c) Beijing Tsing Micro Co.,Ltd. All rights reserved.
 * Description: base type definitions of alg
 * Author: Tsing Micro solution-application group
 * Create: 2024/06/01
 */

#ifndef _TS_ALG_PROFILING_H_
#define _TS_ALG_PROFILING_H_

#include "ts_alg_type.h"
#include "ts_alg_log.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct
{
    TS_FLOAT cpu_occupy;
    TS_U32   mem;
    TS_U32   virtualmem;
}ALG_PerfInfo_S;


/**
 * @brief get cpu occupancy and mem info
 * @param[out] perfInfo, pointer of result
 * @return none
 */
void TS_ALG_GetPerfInfo(ALG_PerfInfo_S *perfInfo);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif


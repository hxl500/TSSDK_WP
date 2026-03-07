/*
 * Copyright © 2019 TSING MICRO.com.cn All Rights Reserved
 * Description: mpi_aiisp.h
 * Author:TsingMicro multimedia software group
 * Create: 2025/01/03
 */
#ifndef __MPI_AIISP_H__
#define __MPI_AIISP_H__

#include "ts_comm_vi.h"
#include "ts_comm_vb.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

TS_S32 TS_MPI_VI_AIISP_Init(VI_PIPE ViPipe);
TS_S32 TS_MPI_VI_AIISP_Exit(VI_PIPE ViPipe);

TS_S32 TS_MPI_VI_AIISP_LoadModel(VI_PIPE ViPipe, const VI_AIISP_MODEL_GROUP_S *pstGroup);
TS_S32 TS_MPI_VI_AIISP_UnloadModel(VI_PIPE ViPipe, const VI_AIISP_MODEL_GROUP_S *pstGroup);
TS_S32 TS_MPI_VI_AIISP_QueryModelList(VI_PIPE ViPipe, VI_AIISP_MODEL_GROUP_S *pstGroup);

TS_S32 TS_MPI_VI_AIISP_Enable(VI_PIPE ViPipe);
TS_S32 TS_MPI_VI_AIISP_Disable(VI_PIPE ViPipe);

TS_S32 TS_MPI_VI_AIISP_SetManualModel(VI_PIPE ViPipe, VI_AIISP_MODEL_TYPE_E modelType, TS_U8 modelId);
TS_S32 TS_MPI_VI_AIISP_GetManualModel(VI_PIPE ViPipe, VI_AIISP_MODEL_TYPE_E *pModelType, TS_U8 *pModelId);

TS_S32 TS_MPI_VI_AIISP_SetAttr(VI_PIPE ViPipe, const VI_AIISP_ATTR_S *pstAiisp);
TS_S32 TS_MPI_VI_AIISP_GetAttr(VI_PIPE ViPipe, VI_AIISP_ATTR_S *pstAiisp);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __MPI_AIISP_H__ */



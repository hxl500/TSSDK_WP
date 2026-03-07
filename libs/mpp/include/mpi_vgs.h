/*
 * Copyright © 2019 TSING MICRO.com.cn All Rights Reserved
 * Description: mpi_vgs.h
 * Author:TsingMicro multimedia software group
 * Create: 2022/06/02
 */

#ifndef __MPI_VGS_H__
#define __MPI_VGS_H__

#include "ts_common.h"
#include "ts_comm_video.h"
#include "ts_comm_vgs.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

TS_S32 TS_MPI_VGS_BeginJob(VGS_HANDLE *phHandle);

TS_S32 TS_MPI_VGS_EndJob(VGS_HANDLE hHandle);

TS_S32 TS_MPI_VGS_CancelJob(VGS_HANDLE hHandle);

TS_S32 TS_MPI_VGS_AddScaleTask(VGS_HANDLE hHandle, const VGS_TASK_ATTR_S *p_sttask,
                               VGS_SCLCOEF_MODE_E enScaleCoefMode);
#ifdef HC_CV_VGS
TS_S32 TS_MPI_VGS_AddScaleExTask(VGS_HANDLE hHandle, const VGS_TASK_ATTR_S *p_sttask,
                               const VGS_SCALE_S *pstVgsScale);

TS_S32 TS_MPI_VGS_AddCropTask(VGS_HANDLE hHandle, const VGS_TASK_ATTR_S *p_sttask,
                                const VGS_CROP_S *pstVgsCrop);

TS_S32 TS_MPI_VGS_AddFlipTask(VGS_HANDLE hHandle, const VGS_TASK_ATTR_S *p_sttask, int direct);
#endif // HC_CV_VGS
TS_S32 TS_MPI_VGS_AddDrawLineTask(VGS_HANDLE hHandle, const VGS_TASK_ATTR_S *p_sttask,
                                  const VGS_DRAW_LINE_S *pstVgsDrawLine);

TS_S32 TS_MPI_VGS_AddCoverTask(VGS_HANDLE hHandle, const VGS_TASK_ATTR_S *p_sttask,
                               const VGS_ADD_COVER_S *pstVgsAddCover);

TS_S32 TS_MPI_VGS_AddOsdTask(VGS_HANDLE hHandle, const VGS_TASK_ATTR_S *p_sttask,
                             const VGS_ADD_OSD_S *pstVgsAddOsd);

TS_S32 TS_MPI_VGS_AddDrawLineTaskArray(VGS_HANDLE hHandle, const VGS_TASK_ATTR_S *p_sttask,
                                       const VGS_DRAW_LINE_S astVgsDrawLine[], TS_U32 u32ArraySize);

TS_S32 TS_MPI_VGS_AddCoverTaskArray(VGS_HANDLE hHandle, const VGS_TASK_ATTR_S *p_sttask,
                                    const VGS_ADD_COVER_S astVgsAddCover[], TS_U32 u32ArraySize);

TS_S32 TS_MPI_VGS_AddOsdTaskArray(VGS_HANDLE hHandle, const VGS_TASK_ATTR_S *p_sttask,
                                  const VGS_ADD_OSD_S astVgsAddOsd[], TS_U32 u32ArraySize);

TS_S32 TS_MPI_VGS_AddRotationTask(VGS_HANDLE hHandle, const VGS_TASK_ATTR_S *p_sttask, ROTATION_E enRotationAngle);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* end of __MPI_VGS_H__ */




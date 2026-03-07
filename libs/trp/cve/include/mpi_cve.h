/*
 * Copyright (c) Beijing Tsing Micro Co.,Ltd. All rights reserved.
 * Description: mpi definitions of cve
 * Author: Tsing Micro solution-application group
 * Create: 2022/07/20
 */

#ifndef _MPI_CVE_H_
#define _MPI_CVE_H_

#include "ts_type.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct tsCVE_AFFINE2D_FACTOR_S
{
    TS_S32 a;
    TS_S32 b;
    TS_S32 c;
    TS_S32 d;
    TS_S32 tx;
    TS_S32 ty;
    TS_S32 dstw;
    TS_S32 dsth;
}CVE_AFFINE2D_FACTOR_S;

TS_S32 TS_MPI_TRP_CVE_BeginJob(CVE_HANDLE *phHandle);

TS_S32 TS_MPI_TRP_CVE_AddTask(CVE_HANDLE hHandle, CVE_TASK_ATTR_S *pstTask);

TS_S32 TS_MPI_TRP_CVE_EndJob(CVE_HANDLE hHandle, CVE_RESULT_S *result);

TS_S32 TS_MPI_TRP_CVE_CancelJob(CVE_HANDLE hHandle);

TS_S32 TS_MPI_TRP_CVE_Init();

TS_S32 TS_MPI_TRP_CVE_Exit();

TS_S32 TS_MPI_TRP_CVE_Resize(TS_U64 srcPhy, TS_VOID *src, TS_U64 dstPhy, TS_VOID *dst,
										TS_S32 srcW, TS_S32 srcH, TS_S32 subW, TS_S32 subH,
										TS_S32 dstW, TS_S32 dstH, TS_S32 channels);

/**
 * @brief TS_MPI_TRP_CVE_GetAffineFactor Get affine transformation coefficients
 * @param srcW Input width
 * @param srcH Input height
 * @param cx Scaling factor in the X direction
 * @param cy Scaling factor in the Y direction
 * @param Angle Rotation angle [0, 360]
 * @param sx Translation in the X direction
 * @param sy Translation in the Y direction
 * @param factor Calculated reflection transformation coefficients
 * @return 1 if successful / 0 if failed
 */
TS_S32 TS_MPI_TRP_CVE_GetAffineFactor(TS_S32 srcW, TS_S32 srcH, TS_FLOAT cx, TS_FLOAT cy, TS_S32 Angle, 
										TS_FLOAT sx, TS_FLOAT sy, CVE_AFFINE2D_FACTOR_S *factor);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _MPI_CVE_H_ */

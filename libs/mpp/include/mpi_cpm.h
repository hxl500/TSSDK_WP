/*
 * Copyright © 2019 TSING MICRO.com.cn All Rights Reserved
 * Description: mpi_cpm.h
 * Author:TsingMicro multimedia software group
 * Create: 2022/06/02
 */
#ifndef __MPI_CPM_H_
#define __MPI_CPM_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "ts_common.h"
#include "ts_comm_cpm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */



#define _CPM_PRT(fmt...)   \
	do {\
		printf("[%s]-%d: ", __func__, __LINE__);\
		printf(fmt);\
	} while (0)


TS_S32 TS_MPI_CPM_CreateGrp(CPM_GRP CPMGrp, CPM_GRP_ATTR_S *pstGrpAttr);
TS_S32 TS_MPI_CPM_Register(CPM_GRP CPMGrp, CPM_Handle_S *pstAlgoHandle);
TS_S32 TS_MPI_CPM_StartGrp(CPM_GRP CPMGrp);
TS_S32 TS_MPI_CPM_StopGrp(CPM_GRP CPMGrp);
TS_S32 TS_MPI_CPM_GetGrpAttr(CPM_GRP          CPMGrp, CPM_GRP_ATTR_S *pstGrpAttr);
TS_S32 TS_MPI_CPM_SetGrpAttr(CPM_GRP          CPMGrp, CPM_GRP_ATTR_S *pstGrpAttr);
TS_S32 TS_MPI_CPM_Unregister(CPM_GRP          CPMGrp);
TS_S32 TS_MPI_CPM_SetParam(CPM_GRP CPMGrp, TS_VOID *pParam);
TS_S32 TS_MPI_CPM_GetParam(CPM_GRP CPMGrp, TS_VOID *pParam);
TS_S32 TS_MPI_CPM_GetResult(CPM_GRP          CPMGrp, TS_VOID *pResult);
TS_S32 TS_MPI_CPM_ReleaseResult(CPM_GRP           CPMGrp, TS_VOID *pResult);
TS_S32 TS_MPI_CPM_DestroyGrp(CPM_GRP CPMGrp);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif


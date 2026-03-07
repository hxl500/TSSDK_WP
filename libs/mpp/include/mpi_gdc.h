#ifndef _MPI_GDC_HEAD_DDD_H_
#define _MPI_GDC_HEAD_DDD_H_
#include <stdint.h>
#include "ts_comm_gdc.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

TS_S32 TS_MPI_GDC_BeginJob(GDC_HANDLE * hHandle);
TS_S32 TS_MPI_GDC_SetConfig(GDC_HANDLE hHandle, void *cfgdata);

TS_S32 TS_MPI_GDC_AddTask(GDC_HANDLE hHandle, void *task);

TS_S32 TS_MPI_GDC_EndJob(GDC_HANDLE hHandle);
TS_S32 TS_MPI_GDC_CancelJob(GDC_HANDLE hHandle);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif
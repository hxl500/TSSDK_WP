/*
 * Copyright © 2019 TSING MICRO.com.cn All Rights Reserved
 * Description: mpi_vb.h
 * Author:TsingMicro multimedia software group
 * Create: 2022/06/02
 */

#ifndef __MPI_LOG_H__
#define __MPI_LOG_H__

#include "ts_debug.h"
#include "ts_comm_video.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

TS_S32 TS_MPI_LOG_SetLevelConf(LOG_LEVEL_CONF_S *pstConf);

TS_S32 TS_MPI_LOG_GetLevelConf(LOG_LEVEL_CONF_S *pstConf);

TS_S32 TS_MPI_LOG_SetWaitFlag(TS_BOOL bWait);

TS_S32 TS_MPI_LOG_Read(TS_CHAR *pBuf, TS_U32 u32Size);

TS_VOID TS_MPI_LOG_Close(TS_VOID);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* __MPI_LOG_H__ */


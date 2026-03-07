/*
 * Copyright © 2019 TSING MICRO.com.cn All Rights Reserved
 * Description: mpi_vb.h
 * Author:TsingMicro multimedia software group
 * Create: 2022/06/02
 */

#ifndef __MPI_VB_H__
#define __MPI_VB_H__

#include "ts_comm_vb.h"
#include "ts_comm_video.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif
VB_POOL TS_MPI_VB_CreatePool(VB_POOL_CONFIG_S *pstVbPoolCfg);
TS_S32 TS_MPI_VB_DestroyPool(VB_POOL Pool);

VB_BLK TS_MPI_VB_GetBlock(VB_POOL Pool, TS_U64 u64BlkSize, const TS_CHAR *pcMmzName);
TS_S32 TS_MPI_VB_ReleaseBlock(VB_BLK Block);

VB_BLK TS_MPI_VB_PhysAddr2Handle(TS_U64 u64PhyAddr);
TS_U64 TS_MPI_VB_Handle2PhysAddr(VB_BLK Block);
VB_POOL TS_MPI_VB_Handle2PoolId(VB_BLK Block);

TS_S32 TS_MPI_VB_Init(TS_VOID);
TS_S32 TS_MPI_VB_Exit(TS_VOID);
TS_S32 TS_MPI_VB_SetConfig(const VB_CONFIG_S *pstVbConfig);
TS_S32 TS_MPI_VB_GetConfig(VB_CONFIG_S *pstVbConfig);

TS_S32 TS_MPI_VB_DupBlock(VB_POOL Pool, TS_U64 u64PhyAddr);
TS_S32 TS_MPI_VB_DupBlock_UID(VB_POOL Pool, TS_U64 u64PhyAddr, VB_UID_E uid);

TS_S32 TS_MPI_VB_MmapPool(VB_POOL Pool);
TS_S32 TS_MPI_VB_MunmapPool(VB_POOL Pool);

TS_S32 TS_MPI_VB_GetBlockVirAddr(VB_POOL Pool, TS_U64 u64PhyAddr, TS_VOID **ppVirAddr);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* __MPI_VB_H__ */


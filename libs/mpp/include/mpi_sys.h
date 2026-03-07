/*
 * Copyright © 2019 TSING MICRO.com.cn All Rights Reserved
 * Description: mpi_sys.h
 * Author:TsingMicro multimedia software group
 * Create: 2022/06/02
 */

#ifndef __MPI_SYS_H__
#define __MPI_SYS_H__

#include "ts_type.h"
#include "ts_common.h"
#include "ts_comm_sys.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */


TS_S32 TS_MPI_SYS_Init(TS_VOID);
TS_S32 TS_MPI_SYS_Exit(TS_VOID);

TS_S32 TS_MPI_SYS_SetConfig(const MPP_SYS_CONFIG_S *pstSysConfig);
TS_S32 TS_MPI_SYS_GetConfig(MPP_SYS_CONFIG_S *pstSysConfig);

TS_S32 TS_MPI_SYS_Bind(const MPP_CHN_S *pstSrcChn, const MPP_CHN_S *pstDestChn);
TS_S32 TS_MPI_SYS_UnBind(const MPP_CHN_S *pstSrcChn, const MPP_CHN_S *pstDestChn);
TS_S32 TS_MPI_SYS_GetBindbyDest(const MPP_CHN_S *pstDestChn, MPP_CHN_S *pstSrcChn);
TS_S32 TS_MPI_SYS_GetBindbySrc(const MPP_CHN_S *pstSrcChn, MPP_BIND_DEST_S *pstBindDest);

TS_S32 TS_MPI_SYS_SetSpecialMode(TS_U64 stMode);
TS_S32 TS_MPI_SYS_GetSpecialMode(TS_U64 *pstMode);

TS_S32 TS_MPI_SYS_GetVersion(MPP_VERSION_S *pstVersion);

TS_S32 TS_MPI_SYS_GetChipId(TS_U64 *pu32ChipId);
TS_S32 TS_MPI_SYS_GetPid(TS_U32 *pu32Pid);

TS_S32 TS_MPI_SYS_GetCurPTS(TS_U64 *pu64CurPTS);
TS_S32 TS_MPI_SYS_InitPTSBase(TS_U64 u64PTSBase);
TS_S32 TS_MPI_SYS_SyncPTS(TS_U64 u64PTSBase);

TS_S32 TS_MPI_SYS_MmzAlloc(TS_U64 *pu64PhyAddr, TS_VOID **ppVirAddr,
                           const TS_CHAR *strMmb, const TS_CHAR *strZone, TS_U32 u32Len);
TS_S32 TS_MPI_SYS_MmzAlloc_Cached(TS_U64 *pu64PhyAddr, TS_VOID **ppVirAddr,
                                  const TS_CHAR *pstrMmb, const TS_CHAR *pstrZone, TS_U32 u32Len);
TS_S32 TS_MPI_SYS_MmzFree(TS_U64 u64PhyAddr, TS_VOID *pVirAddr);
 TS_S32 TS_MPI_SYS_MmzFlushCache(TS_U64 u64PhyAddr, TS_VOID *pVirAddr, TS_U32 u32Size);

TS_VOID *TS_MPI_SYS_Mmap(TS_U64 u64PhyAddr, TS_U32 u32Size);
TS_VOID *TS_MPI_SYS_MmapCache(TS_U64 u64PhyAddr, TS_U32 u32Size);
TS_S32 TS_MPI_SYS_Munmap(TS_VOID *pVirAddr, TS_U32 u32Size);
TS_S32 TS_MPI_SYS_MflushCache(TS_U64 u64PhyAddr, TS_VOID *pVirAddr, TS_U32 u32Size);

TS_S32 TS_MPI_SYS_SetTimeZone(TS_S32 s32TimeZone);
TS_S32 TS_MPI_SYS_GetTimeZone(TS_S32 *ps32TimeZone);

TS_S32 TS_MPI_SYS_SetVIVPSSMode(const VI_VPSS_MODE_S *pstVIVPSSMode);
TS_S32 TS_MPI_SYS_GetVIVPSSMode(VI_VPSS_MODE_S *pstVIVPSSMode);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* __MPI_SYS_H__ */


/*
 * Copyright © 2019 TSING MICRO.com.cn All Rights Reserved
 * Description: mpi_region.h
 * Author:TsingMicro multimedia software group
 * Create: 2022/06/02
 */
#ifndef __MPI_REGION_H__
#define __MPI_REGION_H__

#include "ts_type.h"
#include "ts_comm_region.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

TS_S32 TS_MPI_RGN_Create(RGN_HANDLE Handle, const RGN_ATTR_S * pstRegion);
TS_S32 TS_MPI_RGN_Destroy(RGN_HANDLE Handle);
TS_S32 TS_MPI_RGN_GetAttr(RGN_HANDLE Handle, RGN_ATTR_S *pstRegion);
TS_S32 TS_MPI_RGN_SetAttr(RGN_HANDLE Handle, const RGN_ATTR_S *pstRegion);
TS_S32 TS_MPI_RGN_SetBitMap(RGN_HANDLE Handle, const BITMAP_S *pstBitmap);
TS_S32 TS_MPI_RGN_AttachToChn(RGN_HANDLE Handle, const MPP_CHN_S *pstChn, const RGN_CHN_ATTR_S *pstChnAttr);
TS_S32 TS_MPI_RGN_DetachFromChn(RGN_HANDLE Handle, const MPP_CHN_S *pstChn);
TS_S32 TS_MPI_RGN_DetachAllOSDInChn(const MPP_CHN_S *pstChn);//only for OSD
TS_S32 TS_MPI_RGN_SetDisplayAttr(RGN_HANDLE Handle, const MPP_CHN_S *pstChn, const RGN_CHN_ATTR_S *pstChnAttr);
TS_S32 TS_MPI_RGN_GetDisplayAttr(RGN_HANDLE Handle, const MPP_CHN_S *pstChn, RGN_CHN_ATTR_S *pstChnAttr);
TS_S32 TS_MPI_RGN_GetCanvasInfo(RGN_HANDLE Handle, RGN_CANVAS_INFO_S *pstCanvasInfo);
TS_S32 TS_MPI_RGN_UpdateCanvas(RGN_HANDLE Handle);
//TS_S32 TS_MPI_RGN_BatchBegin(RGN_HANDLEGROUP *pu32Group, TS_S32  u32Num, const RGN_HANDLE handle[]);
//TS_S32 TS_MPI_RGN_BatchEnd(RGN_HANDLEGROUP u32Group);
TS_S32 TS_MPI_RGN_BatchBegin(VPSS_GRP VpssGrp, VPSS_CHN VpssChn);
TS_S32 TS_MPI_RGN_BatchEnd(VPSS_GRP VpssGrp, VPSS_CHN VpssChn);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __MPI_REGION_H__ */



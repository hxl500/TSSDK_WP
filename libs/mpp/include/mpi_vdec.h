/*
 * Copyright © 2019 TSING MICRO.com.cn All Rights Reserved
 * Description: mpi_vdec.h
 * Author:TsingMicro multimedia software group
 * Create: 2022/06/02
 */

#ifndef __MPI_VDEC_H__
#define __MPI_VDEC_H__

#include "ts_common.h"
#include "ts_comm_video.h"
#include "ts_comm_vb.h"
#include "ts_comm_vdec.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

TS_S32 TS_MPI_VDEC_GetFreeChn(PAYLOAD_TYPE_E enType);

TS_S32 TS_MPI_VDEC_CreateChn(VDEC_CHN VdChn, const VDEC_CHN_ATTR_S *pstAttr);
TS_S32 TS_MPI_VDEC_DestroyChn(VDEC_CHN VdChn);

TS_S32 TS_MPI_VDEC_GetChnAttr(VDEC_CHN VdChn, VDEC_CHN_ATTR_S *pstAttr);
TS_S32 TS_MPI_VDEC_SetChnAttr(VDEC_CHN VdChn, const VDEC_CHN_ATTR_S *pstAttr);

TS_S32 TS_MPI_VDEC_StartRecvStream(VDEC_CHN VdChn);
TS_S32 TS_MPI_VDEC_StopRecvStream(VDEC_CHN VdChn);

TS_S32 TS_MPI_VDEC_QueryStatus(VDEC_CHN VdChn, VDEC_CHN_STATUS_S *pstStatus);

TS_S32 TS_MPI_VDEC_GetFd(VDEC_CHN VdChn);
TS_S32 TS_MPI_VDEC_CloseFd(VDEC_CHN VdChn);

/* s32MilliSec: -1 is block,0 is no block,other positive number is timeout */
TS_S32 TS_MPI_VDEC_SendStream(VDEC_CHN VdChn, const VDEC_STREAM_S *pstStream, TS_S32 s32MilliSec);

TS_S32 TS_MPI_VDEC_GetFrame(VDEC_CHN VdChn, VIDEO_FRAME_INFO_S *pstFrameInfo, TS_S32 s32MilliSec);
TS_S32 TS_MPI_VDEC_ReleaseFrame(VDEC_CHN VdChn, const VIDEO_FRAME_INFO_S *pstFrameInfo);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef  __MPI_VDEC_H__ */



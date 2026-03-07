/*
 * Copyright © 2019 TSING MICRO.com.cn All Rights Reserved
 * Description: mpi_vi.h
 * Author:TsingMicro multimedia software group
 * Create: 2022/06/02
 */
#ifndef __MPI_VI_H__
#define __MPI_VI_H__

#include "ts_comm_vi.h"
#include "ts_comm_vb.h"
#include "ts_comm_gdc.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

TS_S32 TS_MPI_VI_CreatePipe(VI_PIPE ViPipe, const VI_PIPE_ATTR_S *pstPipeAttr);
TS_S32 TS_MPI_VI_DestroyPipe(VI_PIPE ViPipe);

TS_S32 TS_MPI_VI_SetPipeAttr(VI_PIPE ViPipe, const VI_PIPE_ATTR_S *pstPipeAttr);
TS_S32 TS_MPI_VI_GetPipeAttr(VI_PIPE ViPipe, VI_PIPE_ATTR_S *pstPipeAttr);

TS_S32 TS_MPI_VI_StartPipe(VI_PIPE ViPipe);
TS_S32 TS_MPI_VI_StopPipe(VI_PIPE ViPipe);

TS_S32 TS_MPI_VI_DumpPipe(const VI_DUMP_ATTR_S *pstDumpAttr);

TS_S32 TS_MPI_VI_QueryPipeStatus(VI_PIPE ViPipe, VI_PIPE_STATUS_S *pstStatus);

TS_S32 TS_MPI_VI_SetChnAttr(VI_PIPE ViPipe, VI_CHN ViChn, const VI_CHN_ATTR_S *pstChnAttr);
TS_S32 TS_MPI_VI_GetChnAttr(VI_PIPE ViPipe, VI_CHN ViChn, VI_CHN_ATTR_S *pstChnAttr);

TS_S32 TS_MPI_VI_EnableChn(VI_PIPE ViPipe, VI_CHN ViChn);
TS_S32 TS_MPI_VI_DisableChn(VI_PIPE ViPipe, VI_CHN ViChn);

TS_S32 TS_MPI_VI_GetChnFrame(VI_PIPE ViPipe, VI_CHN ViChn, VIDEO_FRAME_INFO_S *pstFrameInfo, TS_S32 s32MilliSec);
TS_S32 TS_MPI_VI_ReleaseChnFrame(VI_PIPE ViPipe, VI_CHN ViChn, const VIDEO_FRAME_INFO_S *pstFrameInfo);

TS_S32 TS_MPI_VI_QueryChnStatus(VI_PIPE ViPipe, VI_CHN ViChn, VI_CHN_STATUS_S *pstChnStatus);

/* mode   0:day  1:ir */
TS_S32 TS_MPI_VI_SetDayNight(VI_PIPE ViPipe, TS_S32 mode);
TS_S32 TS_MPI_VI_GetDayNight(VI_PIPE ViPipe, TS_S32 * pMode);

TS_S32 TS_MPI_VI_SetUserPic(VI_PIPE ViPipe, const VI_USERPIC_ATTR_S *pstUsrPic);
TS_S32 TS_MPI_VI_EnableUserPic(VI_PIPE ViPipe);
TS_S32 TS_MPI_VI_DisableUserPic(VI_PIPE ViPipe);

TS_S32 TS_MPI_VI_SetChnLDCAttr(VI_PIPE ViPipe, VI_CHN ViChn, const GDC_CONFIG_S *pstLdcInfo);
TS_S32 TS_MPI_VI_GetChnLDCAttr(VI_PIPE ViPipe, VI_CHN ViChn, GDC_CONFIG_S *pstLdcInfo);

TS_S32 TS_MPI_VI_SetChnGdcFishCorrec(VI_PIPE ViPipe, VI_CHN ViChn, const GDC_CONFIG_S *pstFishCorrec);
TS_S32 TS_MPI_VI_GetChnGdcFishCorrec(VI_PIPE ViPipe, VI_CHN ViChn, GDC_CONFIG_S *pstFishCorrec);

TS_S32 TS_MPI_VI_SetPipeFrameSource(VI_PIPE ViPipe, const VI_PIPE_FRAME_SOURCE_E enSource);
TS_S32 TS_MPI_VI_GetPipeFrameSource(VI_PIPE ViPipe, VI_PIPE_FRAME_SOURCE_E *penSource);
TS_S32 TS_MPI_VI_SendPipeRaw(TS_U32 u32PipeNum, VI_PIPE PipeId[], VIDEO_FRAME_INFO_S *pstVideoFrame[], TS_S32 s32MilliSec);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __MPI_VI_H__ */



/*
 * Copyright © 2019 TSING MICRO.com.cn All Rights Reserved
 * Description: mpi_vo.h
 * Author:TsingMicro multimedia software group
 * Create: 2022/06/02
 */
#ifndef __MPI_VO_H__
#define __MPI_VO_H__

#include "ts_comm_vo.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/* Device Relative Settings */
TS_S32 TS_MPI_VO_SetPubAttr(VO_DEV VoDev, const VO_PUB_ATTR_S *pstPubAttr);
TS_S32 TS_MPI_VO_GetPubAttr(VO_DEV VoDev, VO_PUB_ATTR_S *pstPubAttr);

TS_S32 TS_MPI_VO_Enable(VO_DEV VoDev);
TS_S32 TS_MPI_VO_Disable(VO_DEV VoDev);

/* Video Relative Settings */
TS_S32 TS_MPI_VO_SetVideoLayerAttr(VO_LAYER VoLayer, const VO_VIDEO_LAYER_ATTR_S *pstLayerAttr);
TS_S32 TS_MPI_VO_GetVideoLayerAttr(VO_LAYER VoLayer, VO_VIDEO_LAYER_ATTR_S *pstLayerAttr);

TS_S32 TS_MPI_VO_EnableVideoLayer(VO_LAYER VoLayer);
TS_S32 TS_MPI_VO_DisableVideoLayer(VO_LAYER VoLayer);

TS_S32 TS_MPI_VO_SetVideoLayerCrop(VO_LAYER VoLayer, const CROP_INFO_S *pstCropInfo);
TS_S32 TS_MPI_VO_GetVideoLayerCrop(VO_LAYER VoLayer, CROP_INFO_S *pstCropInfo);

/* Channel Relative Operations */
TS_S32 TS_MPI_VO_SetChnAttr(VO_LAYER VoLayer, VO_CHN VoChn, const VO_CHN_ATTR_S *pstChnAttr);
TS_S32 TS_MPI_VO_GetChnAttr(VO_LAYER VoLayer, VO_CHN VoChn, VO_CHN_ATTR_S *pstChnAttr);

TS_S32 TS_MPI_VO_EnableChn(VO_LAYER VoLayer, VO_CHN VoChn);
TS_S32 TS_MPI_VO_DisableChn(VO_LAYER VoLayer, VO_CHN VoChn);

TS_S32 TS_MPI_VO_SetDevFrameRate(VO_DEV VoDev, const TS_U32 frameRate);
TS_S32 TS_MPI_VO_GetDevFrameRate(VO_DEV VoDev, TS_U32 *frameRate);

TS_S32 TS_MPI_VO_SetDevGamma(VO_DEV VoDev, const VO_DEV_GAMMA_S *pstDevGamma);
TS_S32 TS_MPI_VO_GetDevGamma(VO_DEV VoDev, VO_DEV_GAMMA_S *pstDevGamma);

TS_S32 TS_MPI_VO_CloseFd(TS_VOID);

TS_S32 TS_MPI_VO_SetVideoLayerPriority(VO_LAYER VoLayer, TS_U32 u32Priority);
TS_S32 TS_MPI_VO_GetVideoLayerPriority(VO_LAYER VoLayer, TS_U32 *pu32Priority);

TS_S32 TS_MPI_VO_SetVideoLayerDecompress(VO_LAYER VoLayer, TS_BOOL bSupportDecompress);
TS_S32 TS_MPI_VO_GetVideoLayerDecompress(VO_LAYER VoLayer, TS_BOOL *pbSupportDecompress); 

TS_S32 TS_MPI_VO_SetChnRotation(VO_LAYER VoLayer, VO_CHN VoChn, ROTATION_E enRotation);
TS_S32 TS_MPI_VO_GetChnRotation(VO_LAYER VoLayer, VO_CHN VoChn, ROTATION_E *penRotation);

TS_S32 TS_MPI_VO_SetLayerResize(VO_LAYER VoLayer, const VO_LAYER_RESIZE_S *pstLayerResize);
TS_S32 TS_MPI_VO_GetLayerResize(VO_LAYER VoLayer, VO_LAYER_RESIZE_S *pstLayerResize);

TS_S32 TS_MPI_VO_SetLayerFlip(VO_LAYER VoLayer, const VO_LAYER_FLIP_S *pstLayerFlip);
TS_S32 TS_MPI_VO_GetLayerFlip(VO_LAYER VoLayer, VO_LAYER_FLIP_S *pstLayerFlip);

TS_S32 TS_MPI_VO_PauseChn(VO_LAYER VoLayer, VO_CHN VoChn);
TS_S32 TS_MPI_VO_ShowChn(VO_LAYER VoLayer, VO_CHN VoChn);

TS_S32 TS_MPI_VO_SendFrame(VO_LAYER VoLayer, VO_CHN VoChn, VIDEO_FRAME_INFO_S *pstVFrame, TS_S32 s32MilliSec);
TS_S32 TS_MPI_VO_GUI_SendFrame(TS_S32 layer_id, TS_U32 width, TS_U32 height, TS_S32 size, TS_VOID *vaddr);

TS_S32 TS_MPI_VO_GetChnPTS(VO_LAYER VoLayer, VO_CHN VoChn, TS_U64 *pu64ChnPTS);

TS_S32 TS_MPI_VO_SetVideoLayerCSC(VO_LAYER VoLayer, const VO_CSC_S *pstVideoCSC);
TS_S32 TS_MPI_VO_GetVideoLayerCSC(VO_LAYER VoLayer, VO_CSC_S *pstVideoCSC);

TS_S32 TS_MPI_VO_SetDisplayBufLen(VO_LAYER VoLayer, TS_U32 u32BufLen);
TS_S32 TS_MPI_VO_GetDisplayBufLen(VO_LAYER VoLayer, TS_U32 *pu32BufLen);

// void TS_MPI_VO_ACTIVE();
// TS_S32 TS_MPI_VO_EnableWBC(VO_WBC VoWBC);

// TS_S32 TS_MPI_VO_ReleaseScreenFrame(VO_LAYER VoLayer, const VIDEO_FRAME_INFO_S *pstVFrame);
// TS_S32 TS_MPI_VO_GetScreenFrame(VO_LAYER VoLayer, VIDEO_FRAME_INFO_S *pstVFrame, TS_S32 s32MilliSec);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __MPI_VO_H__ */



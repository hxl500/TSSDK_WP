/*
 * Copyright © 2019 TSING MICRO.com.cn All Rights Reserved
 * Description: mpi_venc.h
 * Author:TsingMicro multimedia software group
 * Create: 2022/06/02
 */
#ifndef __MPI_VENC_H__
#define __MPI_VENC_H__

#include "ts_comm_venc.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

TS_S32 TS_MPI_VENC_GetFreeChn(PAYLOAD_TYPE_E enType);

TS_S32 TS_MPI_VENC_CreateChn(VENC_CHN VeChn, const VENC_CHN_ATTR_S *pstAttr);
TS_S32 TS_MPI_VENC_DestroyChn(VENC_CHN VeChn);

TS_S32 TS_MPI_VENC_ResetChn(VENC_CHN VeChn);

TS_S32 TS_MPI_VENC_StartRecvFrame(VENC_CHN VeChn, const VENC_RECV_PIC_PARAM_S *pstRecvParam);
TS_S32 TS_MPI_VENC_StopRecvFrame(VENC_CHN VeChn);

TS_S32 TS_MPI_VENC_QueryStatus(VENC_CHN VeChn, VENC_CHN_STATUS_S *pstStatus);

TS_S32 TS_MPI_VENC_SetChnAttr(VENC_CHN VeChn, const VENC_CHN_ATTR_S *pstAttr);
TS_S32 TS_MPI_VENC_GetChnAttr(VENC_CHN VeChn, VENC_CHN_ATTR_S *pstChnAttr);

TS_S32 TS_MPI_VENC_GetStream(VENC_CHN VeChn, VENC_STREAM_S *pstStream, TS_S32 s32MilliSec);
TS_S32 TS_MPI_VENC_ReleaseStream(VENC_CHN VeChn, VENC_STREAM_S *pstStream);

TS_S32 TS_MPI_VENC_SendFrame(VENC_CHN VeChn, const VIDEO_FRAME_INFO_S *pstFrame, TS_S32 s32MilliSec);

TS_S32 TS_MPI_VENC_RequestIDR(VENC_CHN VeChn, TS_BOOL bInstant);

int TS_MPI_VENC_SELECT(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);

TS_S32 TS_MPI_VENC_GetFd(VENC_CHN VeChn);
TS_S32 TS_MPI_VENC_CloseFd(VENC_CHN VeChn);

TS_S32 TS_MPI_VENC_SetRoiAttr(VENC_CHN VeChn, const VENC_ROI_ATTR_S *pstRoiAttr);
TS_S32 TS_MPI_VENC_GetRoiAttr(VENC_CHN VeChn, TS_U32 u32Index, VENC_ROI_ATTR_S *pstRoiAttr);

TS_S32 TS_MPI_VENC_SetJpegParam(VENC_CHN VeChn, const VENC_JPEG_PARAM_S *pstJpegParam);
TS_S32 TS_MPI_VENC_GetJpegParam(VENC_CHN VeChn, VENC_JPEG_PARAM_S *pstJpegParam);

TS_S32 TS_MPI_VENC_SetRcParam(VENC_CHN VeChn, const VENC_RC_PARAM_S *pstRcParam);
TS_S32 TS_MPI_VENC_GetRcParam(VENC_CHN VeChn, VENC_RC_PARAM_S *pstRcParam);

TS_S32 TS_MPI_VENC_SetChnParam(VENC_CHN VeChn, const VENC_CHN_PARAM_S *pstChnParam);
TS_S32 TS_MPI_VENC_GetChnParam(VENC_CHN VeChn, VENC_CHN_PARAM_S *pstChnParam);

TS_S32 TS_MPI_VENC_SetSmartEnable(VENC_CHN VeChn, TS_U8 bEnable);
TS_S32 TS_MPI_VENC_GetSmartEnable(VENC_CHN VeChn, TS_U8 *pbEnable);

TS_S32 TS_MPI_VENC_SetMotionDetectParam(VENC_CHN VeChn, const VENC_MOTION_DETECT_RESULT_S *pstMotionDetectResult);

TS_S32 TS_MPI_VENC_SetSmartEncodeParam(VENC_CHN VeChn, const VENC_SMART_ENC_PARA_S *pstSmartEncodeParam);
TS_S32 TS_MPI_VENC_GetSmartEncodeParam(VENC_CHN VeChn, VENC_SMART_ENC_PARA_S *pstSmartEncodeParam);

TS_S32 TS_MPI_VENC_EnableMms(VENC_CHN venc_chn);
TS_S32 TS_MPI_VENC_GetMms(VENC_CHN venc_chn, VENC_MMS_INFO_S *mms_info);

/**
 * @brief   Set the inclination of cu/mb to choose Skip mode
 * 
 * @param   VeChn       channel number
 * @param   pstSkipBias The tendency of cu/mb to choose Skip mode.
 * @return  TS_S32 
 */
TS_S32 TS_MPI_VENC_SetSkipBias(VENC_CHN VeChn, const VENC_SKIP_BIAS_S *pstSkipBias);

/**
 * @brief   Get the inclination of cu/mb to choose Skip mode
 * 
 * @param   VeChn       channel number
 * @param   pstSkipBias The tendency of cu/mb to choose Skip mode.
 * @return  TS_S32 
 */
TS_S32 TS_MPI_VENC_GetSkipBias(VENC_CHN VeChn, VENC_SKIP_BIAS_S *pstSkipBias);

/**
 * @brief   Set foreground protection parameters for channels
 * 
 * @param   VeChn       channel number
 * @param   pstFgProtect Image foreground protection parameters
 * @return  TS_S32 
 */
TS_S32 TS_MPI_VENC_SetForegroundProtect(VENC_CHN VeChn, const VENC_FOREGROUND_PROTECT_S *pstFgProtect);

/**
 * @brief   Get foreground protection parameters for channels
 * 
 * @param   VeChn       channel number
 * @param   pstFgProtect Image foreground protection parameters
 * @return  TS_S32 
 */
TS_S32 TS_MPI_VENC_GetForegroundProtect(VENC_CHN VeChn, VENC_FOREGROUND_PROTECT_S *pstFgProtect);

/**
 * @brief   Set the preference for CU mode selection
 * 
 * @param   VeChn       channel number
 * @param   pstCuPrediction Tendency parameters for CU mode selection
 * @return  TS_S32 
 */
TS_S32 TS_MPI_VENC_SetCuPrediction(VENC_CHN VeChn, const VENC_CU_PREDICTION_S *pstCuPrediction);

/**
 * @brief   Get the preference for CU mode selection
 * 
 * @param   VeChn       channel number
 * @param   pstCuPrediction Tendency parameters for CU mode selection
 * @return  TS_S32 
 */
TS_S32 TS_MPI_VENC_GetCuPrediction(VENC_CHN VeChn, VENC_CU_PREDICTION_S *pstCuPrediction);

/**
 * @brief   Set SSE properties for H.264/H.265 channels.
 * 
 * @param   VeChn       channel number
 * @param   pstSseRegion SSE region parameters
 * @return  TS_S32 
 */
TS_S32 TS_MPI_VENC_SetSSERegion(VENC_CHN VeChn, const VENC_SSE_CFG_S *pstSseRegion);

/**
 * @brief   Get SSE properties for H.264/H.265 channels
 * 
 * @param   VeChn       channel number
 * @param   pstSseRegion SSE region parameters
 * @return  TS_S32 
 */
TS_S32 TS_MPI_VENC_GetSSERegion(VENC_CHN VeChn, VENC_SSE_CFG_S *pstSseRegion);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __MPI_VENC_H__ */

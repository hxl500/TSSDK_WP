/*
 * Copyright © 2019 TSING MICRO.com.cn All Rights Reserved
 * Description: mpi_audio.h
 * Author:TsingMicro multimedia software group
 * Create: 2022/06/02
 */
#ifndef __MPI_AUDIO_H__
#define __MPI_AUDIO_H__

#include "ts_type.h"
#include "ts_common.h"
#include "ts_comm_aio.h"
#include "ts_comm_aenc.h"
#include "ts_comm_adec.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"
{
#endif
#endif /* __cplusplus */

#define AENC_ADAPT_MAGIC 0Xfcfcfcfc


/* AI function api. */
TS_S32 TS_MPI_AI_SetPubAttr(AUDIO_DEV AiDevId, const AIO_ATTR_S *pstAttr);
TS_S32 TS_MPI_AI_GetPubAttr(AUDIO_DEV AiDevId, AIO_ATTR_S *pstAttr);

TS_S32 TS_MPI_AI_SetChnAttr(AUDIO_DEV AiDevId, AI_CHN AiChn, const AI_CHN_ATTR_S *pstChnAttr);
TS_S32 TS_MPI_AI_GetChnAttr(AUDIO_DEV AiDevId, AI_CHN AiChn, AI_CHN_ATTR_S *pstChnAttr);

TS_S32 TS_MPI_AI_Enable(AUDIO_DEV AiDevId);
TS_S32 TS_MPI_AI_Disable(AUDIO_DEV AiDevId);

TS_S32 TS_MPI_AI_EnableChn(AUDIO_DEV AiDevId, AI_CHN AiChn);
TS_S32 TS_MPI_AI_DisableChn(AUDIO_DEV AiDevId, AI_CHN AiChn);

TS_S32 TS_MPI_AI_GetFrame(AUDIO_DEV AiDevId, AI_CHN AiChn, AUDIO_FRAME_S *pstFrm, AUDIO_FRAME_S *pstAecFrm, TS_S32 s32MilliSec);
TS_S32 TS_MPI_AI_ReleaseFrame(AUDIO_DEV AiDevId, AI_CHN AiChn, const AUDIO_FRAME_S *pstFrm, const AUDIO_FRAME_S *pstAecFrm);

TS_S32 TS_MPI_AI_GetRawFrame(AUDIO_DEV AiDevId, AI_CHN AiChn, AUDIO_FRAME_S *pstRawFrm, AUDIO_FRAME_S *pstAecFrm, TS_S32 s32MilliSec);
TS_S32 TS_MPI_AI_ReleaseRawFrame(AUDIO_DEV AiDevId, AI_CHN AiChn, const AUDIO_FRAME_S *pstRawFrm, const AUDIO_FRAME_S *pstAecFrm);

TS_S32 TS_MPI_AI_SetTalkVqeAttr(AUDIO_DEV AiDevId, AI_CHN AiChn , const AUDIO_VQE_CONFIG_S *pstVqeConfig);
TS_S32 TS_MPI_AI_GetTalkVqeAttr(AUDIO_DEV AiDevId, AI_CHN AiChn, AUDIO_VQE_CONFIG_S *pstVqeConfig);

TS_S32 TS_MPI_AI_RegisteredVqe(AUDIO_DEV AiDevId, AI_CHN AiChn, AUDIO_VQE_S *AiVqe);
TS_S32 TS_MPI_AI_UnRegisteredVqe(AUDIO_DEV AiDevId, AI_CHN AiChn);

TS_S32 TS_MPI_AI_EnableVqe(AUDIO_DEV AiDevId, AI_CHN AiChn);
TS_S32 TS_MPI_AI_DisableVqe(AUDIO_DEV AiDevId, AI_CHN AiChn);

TS_S32 TS_MPI_AI_GetFd(AUDIO_DEV AiDevId, AI_CHN AiChn);
TS_S32 TS_MPI_AI_SELECT(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);

TS_S32 TS_MPI_AI_SetVqeVolume(AUDIO_DEV AiDevId, AI_CHN AiChn, TS_S32 s32VolumeDb);
TS_S32 TS_MPI_AI_GetVqeVolume(AUDIO_DEV AiDevId, AI_CHN AiChn, TS_S32 *ps32VolumeDb);
TS_S32 TS_MPI_AI_SetMixer(AUDIO_DEV AiDevId, AI_CHN AiChn, const MIXER_CONFIG_S *MixerCfg);
TS_S32 TS_MPI_AI_GetMixer(AUDIO_DEV AiDevId, AI_CHN AiChn, MIXER_CONFIG_S *MixerCfg);
TS_S32 TS_MPI_AI_EnableAecRefFrame(AUDIO_DEV AiDevId, AI_CHN AiChn, AUDIO_DEV AoDevId, AO_CHN AoChn);
TS_S32 TS_MPI_AI_DisableAecRefFrame(AUDIO_DEV AiDevId, AI_CHN AiChn);

/* AO function api. */
TS_S32 TS_MPI_AO_SetPubAttr(AUDIO_DEV AoDevId, const AIO_ATTR_S *pstAttr);
TS_S32 TS_MPI_AO_GetPubAttr(AUDIO_DEV AoDevId, AIO_ATTR_S *pstAttr);

TS_S32 TS_MPI_AO_SetTalkVqeAttr(AUDIO_DEV AoDevId, AO_CHN AoChn, const AUDIO_VQE_CONFIG_S *pstVqeConfig);
TS_S32 TS_MPI_AO_GetTalkVqeAttr(AUDIO_DEV AoDevId, AO_CHN AoChn, AUDIO_VQE_CONFIG_S *pstVqeConfig);

TS_S32 TS_MPI_AO_RegisteredVqe(AUDIO_DEV AoDevId, AO_CHN AoChn, AUDIO_VQE_S *AoVqe);
TS_S32 TS_MPI_AO_UnRegisteredVqe(AUDIO_DEV AoDevId, AO_CHN AoChn);

TS_S32 TS_MPI_AO_Enable(AUDIO_DEV AoDevId);
TS_S32 TS_MPI_AO_Disable(AUDIO_DEV AoDevId);

TS_S32 TS_MPI_AO_EnableChn(AUDIO_DEV AoDevId, AO_CHN AoChn);
TS_S32 TS_MPI_AO_DisableChn(AUDIO_DEV AoDevId, AO_CHN AoChn);

TS_S32 TS_MPI_AO_SendFrame(AUDIO_DEV AoDevId, AO_CHN AoChn, const AUDIO_FRAME_S *pstData, TS_S32 s32MilliSec);

TS_S32 TS_MPI_AO_QueryChnStat(AUDIO_DEV AoDevId, AO_CHN AoChn, AO_CHN_STATE_S *pstStatus);
TS_S32 TS_MPI_AO_ClearChnBuf(AUDIO_DEV AoDevId, AO_CHN AoChn);
TS_S32 TS_MPI_AO_PauseChn(AUDIO_DEV AoDevId, AO_CHN AoChn);
TS_S32 TS_MPI_AO_ResumeChn(AUDIO_DEV AoDevId, AO_CHN AoChn);
TS_S32 TS_MPI_AO_GetChnDelay(AUDIO_DEV AoDevId, AO_CHN AoChn, TS_U32 *pMilliSec);

TS_S32 TS_MPI_AO_SetVolume(AUDIO_DEV AoDevId, TS_S32 s32VolumeDb);
TS_S32 TS_MPI_AO_GetVolume(AUDIO_DEV AoDevId, TS_S32 *ps32VolumeDb);

TS_S32 TS_MPI_AO_SetMute(AUDIO_DEV AoDevId, TS_BOOL bEnable, const AUDIO_FADE_S *pstFade);
TS_S32 TS_MPI_AO_GetMute(AUDIO_DEV AoDevId, TS_BOOL *pbEnable, AUDIO_FADE_S *pstFade);
TS_S32 TS_MPI_AO_SetMixer(AUDIO_DEV AoDevId, const MIXER_CONFIG_S *MixerCfg);
TS_S32 TS_MPI_AO_GetMixer(AUDIO_DEV AoDevId, MIXER_CONFIG_S *MixerCfg);

TS_S32 TS_MPI_AO_EnableVqe(AUDIO_DEV AoDevId, AO_CHN AoChn);
TS_S32 TS_MPI_AO_DisableVqe(AUDIO_DEV AoDevId, AO_CHN AoChn);



/* AENC function api. */
TS_S32 TS_MPI_AENC_CreateChn(AENC_CHN AeChn, const AENC_CHN_ATTR_S *pstAttr);
TS_S32 TS_MPI_AENC_DestroyChn(AENC_CHN AeChn);
TS_S32 TS_MPI_AENC_SendFrame(AENC_CHN AeChn, const AUDIO_FRAME_S *pstFrm);
TS_S32 TS_MPI_AENC_GetStream(AENC_CHN AeChn, AUDIO_STREAM_S *pstStream, TS_S32 s32MilliSec);
TS_S32 TS_MPI_AENC_ReleaseStream(AENC_CHN AeChn, const AUDIO_STREAM_S *pstStream);

TS_S32 TS_MPI_AENC_GetFd(AENC_CHN AeChn);

TS_S32 TS_MPI_AENC_SELECT(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);

TS_S32 TS_MPI_AENC_RegisterEncoder(AENC_CHN AeChn, const AENC_CHN_REGISTERED_T *regster);

TS_S32 TS_MPI_AENC_SetMute(AENC_CHN AeChn, TS_BOOL bEnable);
TS_S32 TS_MPI_AENC_GetMute(AENC_CHN AeChn, TS_BOOL *pbEnable);
TS_S32 TS_MPI_AENC_ClearChnBuf(AENC_CHN AeChn);

/* ADEC function api. */
TS_S32 TS_MPI_ADEC_CreateChn(ADEC_CHN AdChn, const ADEC_CHN_ATTR_S *pstAttr);
TS_S32 TS_MPI_ADEC_DestroyChn(ADEC_CHN AdChn);

TS_S32 TS_MPI_ADEC_SendStream(ADEC_CHN AdChn, const AUDIO_STREAM_S *pstStream, TS_BOOL bBlock);
TS_S32 TS_MPI_ADEC_QueryChnStat(ADEC_CHN AdChn, ADEC_CHN_STATE_S *pstBufferStatus);

TS_S32 TS_MPI_ADEC_RegisterDecoder(ADEC_CHN AdChn, const ADEC_CHN_REGISTERED_T *regster);
TS_S32 TS_MPI_ADEC_ClearChnBuf(ADEC_CHN AdChn);
TS_S32 TS_MPI_ADEC_GetFrameInfo(ADEC_CHN AdChn, TS_VOID *pstFrameInfo);

TS_S32 TS_MPI_ADEC_GetFrame(ADEC_CHN AdChn, AUDIO_FRAME_INFO_S *pstFrmInfo, TS_BOOL bBlock);
TS_S32 TS_MPI_ADEC_ReleaseFrame(ADEC_CHN AdChn, const AUDIO_FRAME_INFO_S *pstFrmInfo);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __MPI_AI_H__ */


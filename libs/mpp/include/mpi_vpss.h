/*
 * Copyright © 2019 TSING MICRO.com.cn All Rights Reserved
 * Description: mpi_vpss.h
 * Author:TsingMicro multimedia software group
 * Create: 2022/06/02
 */

#ifndef __MPI_VPSS_H__
#define __MPI_VPSS_H__

#include "ts_common.h"
#include "ts_comm_video.h"
#include "ts_comm_vb.h"
#include "ts_comm_vpss.h"
#include "ts_comm_gdc.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/* Group Settings */
TS_S32 TS_MPI_VPSS_CreateGrp(VPSS_GRP VpssGrp, const VPSS_GRP_ATTR_S *pstGrpAttr);
TS_S32 TS_MPI_VPSS_DestroyGrp(VPSS_GRP VpssGrp);
TS_S32 TS_MPI_VPSS_GetFreeGrp();
TS_S32 TS_MPI_VPSS_ProcessSingleFrame(VPSS_GRP VpssGrp,  VPSS_CHN VpssChn, VIDEO_FRAME_INFO_S *pstInFrame,
	VIDEO_FRAME_INFO_S *pstOutFrame, TS_S32 s32MilliSec);
TS_S32 TS_MPI_VPSS_Start_Camera(void);
TS_S32 TS_MPI_VPSS_StartGrp(VPSS_GRP VpssGrp);
TS_S32 TS_MPI_VPSS_StopGrp(VPSS_GRP VpssGrp);

TS_S32 TS_MPI_VPSS_GetGrpAttr(VPSS_GRP VpssGrp, VPSS_GRP_ATTR_S *pstGrpAttr);
TS_S32 TS_MPI_VPSS_SetGrpAttr(VPSS_GRP VpssGrp, const VPSS_GRP_ATTR_S *pstGrpAttr);
TS_S32 TS_MPI_VPSS_SetGrpCrop(VPSS_GRP VpssGrp, const VPSS_CROP_INFO_S *pstCropInfo);
TS_S32 TS_MPI_VPSS_GetGrpCrop(VPSS_GRP VpssGrp, VPSS_CROP_INFO_S *pstCropInfo);

TS_S32 TS_MPI_VPSS_SendFrame(VPSS_GRP VpssGrp, VPSS_GRP_PIPE VpssGrpPipe,
    const VIDEO_FRAME_INFO_S *pstVideoFrame, TS_S32 s32MilliSec);

TS_S32 TS_MPI_VPSS_SetChnAttr(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, const VPSS_CHN_ATTR_S *pstChnAttr);
TS_S32 TS_MPI_VPSS_GetChnAttr(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VPSS_CHN_ATTR_S *pstChnAttr);

TS_S32 TS_MPI_VPSS_SetChnOverlayLut(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, const VPSS_OVERLAY_LUT_S *pstChnLut);
TS_S32 TS_MPI_VPSS_GetChnOverlayLut(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VPSS_OVERLAY_LUT_S *pstChnLut);

TS_S32 TS_MPI_VPSS_EnableChn(VPSS_GRP VpssGrp, VPSS_CHN VpssChn);
TS_S32 TS_MPI_VPSS_DisableChn(VPSS_GRP VpssGrp, VPSS_CHN VpssChn);

TS_S32 TS_MPI_VPSS_SetChnCrop(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, const VPSS_CROP_INFO_S *pstCropInfo);
TS_S32 TS_MPI_VPSS_GetChnCrop(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VPSS_CROP_INFO_S *pstCropInfo);

TS_S32 TS_MPI_VPSS_RuntimeReset(void);
TS_S32 TS_MPI_VPSS_RuntimeRestart(void);

TS_S32 TS_MPI_VPSS_GetChnFrame(VPSS_GRP VpssGrp, VPSS_CHN VpssChn,
    VIDEO_FRAME_INFO_S *pstVideoFrame, TS_S32 s32MilliSec);
TS_S32 TS_MPI_VPSS_ReleaseChnFrame(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, const VIDEO_FRAME_INFO_S *pstVideoFrame);

TS_S32 TS_MPI_VPSS_SetChnLDCAttr(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, const GDC_CONFIG_S *pstLdcInfo);
TS_S32 TS_MPI_VPSS_GetChnLDCAttr(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, GDC_CONFIG_S *pstLdcInfo);

TS_S32 TS_MPI_VPSS_SetChnGdcFishCorrec(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, const GDC_CONFIG_S *pstFishCorrec);
TS_S32 TS_MPI_VPSS_GetChnGdcFishCorrec(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, GDC_CONFIG_S *pstLdcInfo);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __MPI_VPSS_H__ */



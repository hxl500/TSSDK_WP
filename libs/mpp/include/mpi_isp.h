/*
 * Copyright © 2019 TSING MICRO.com.cn All Rights Reserved
 * Description: mpi_isp.h
 * Author:TsingMicro multimedia software group
 * Create: 2022/06/02
 */
#ifndef __MPI_ISP_H__
#define __MPI_ISP_H__

#include "ts_comm_3a.h"
#include "ts_comm_video.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

TS_S32 TS_MPI_ISP_SensorRegCallBack(VI_PIPE ViPipe, ISP_SNS_ATTR_INFO_S	*pstSnsAttrInfo , ISP_SENSOR_REGISTER_S *pstRegister);
TS_S32 TS_MPI_ISP_SensorUnRegCallBack(VI_PIPE ViPipe, ISP_SNS_ATTR_INFO_S	*pstSnsAttrInfo , ISP_SENSOR_REGISTER_S *pstRegister);

/* Firmware Main Operation */
TS_S32 TS_MPI_ISP_Init(VI_PIPE ViPipe);
TS_S32 TS_MPI_ISP_MemInit(VI_PIPE ViPipe);
TS_S32 TS_MPI_ISP_Run(VI_PIPE ViPipe);
TS_S32 TS_MPI_ISP_RunOnce(VI_PIPE ViPipe);
TS_S32 TS_MPI_ISP_Exit(VI_PIPE ViPipe);

TS_S32 TS_MPI_ISP_SetPubAttr(VI_PIPE ViPipe, const ISP_PUB_ATTR_S *pstPubAttr);
TS_S32 TS_MPI_ISP_GetPubAttr(VI_PIPE ViPipe, ISP_PUB_ATTR_S *pstPubAttr);
TS_S32 TS_MPI_ISP_SetGeneralTop(VI_PIPE ViPipe, const ISP_GENERAL_TOP_S *pstGeneralTop);
TS_S32 TS_MPI_ISP_GetGeneralTop(VI_PIPE ViPipe, ISP_GENERAL_TOP_S *pstGeneralTop);

TS_S32 TS_MPI_ISP_SetSdeAttr(VI_PIPE ViPipe, const ISP_SDE_S *pstSdeInfo);
TS_S32 TS_MPI_ISP_GetSdeAttr(VI_PIPE ViPipe, ISP_SDE_S *pstSdeInfo);
TS_S32 TS_MPI_ISP_SetLshAttr(VI_PIPE ViPipe, const ISP_LSH_S *pstLshInfo);
TS_S32 TS_MPI_ISP_GetLshAttr(VI_PIPE ViPipe, ISP_LSH_S *pstLshInfo);

TS_S32 TS_MPI_ISP_GetVDTimeOut(VI_PIPE ViPipe, ISP_VD_TYPE_E enIspVDType, TS_U32 u32MilliSec);

TS_S32 TS_MPI_ISP_GetAEStatistics(VI_PIPE ViPipe, ISP_AE_STATISTICS_S *pstAeStat);
TS_S32 TS_MPI_ISP_GetWBStatistics(VI_PIPE ViPipe, ISP_WB_STATISTICS_S *pstWBStat);
TS_S32 TS_MPI_ISP_GetFocusStatistics(VI_PIPE ViPipe, ISP_AF_STATISTICS_S *pstAfStat);

TS_S32 TS_MPI_ISP_SetLtm(VI_PIPE ViPipe, ts_u32 mode);
TS_S32 TS_MPI_ISP_SetParamIndex(VI_PIPE ViPipe, TS_U32 paramSetIndex);
TS_S32 TS_MPI_ISP_GetParamIndex(VI_PIPE ViPipe, TS_U32 * pParamSetIndex);

TS_S32 TS_MPI_ISP_SetTnrAttr(VI_PIPE ViPipe, const ISP_TNR_S *pstTnrAttr);
TS_S32 TS_MPI_ISP_GetTnrAttr(VI_PIPE ViPipe, ISP_TNR_S *pstTnrAttr);

TS_S32 TS_MPI_ISP_SetMvdAttr(VI_PIPE ViPipe, const ISP_MVD_S *pstMvdAttr);
TS_S32 TS_MPI_ISP_GetMvdAttr(VI_PIPE ViPipe, ISP_MVD_S *pstMvdAttr);

TS_S32 TS_MPI_ISP_SetDmsAttr(VI_PIPE ViPipe, const ISP_DMS_S *pstDmsAttr);
TS_S32 TS_MPI_ISP_GetDmsAttr(VI_PIPE ViPipe, ISP_DMS_S *pstDmsAttr);

TS_S32 TS_MPI_ISP_SetGammaAttr(VI_PIPE ViPipe, const ISP_GAMMA_ATTR_S *pstGammaAttr);
TS_S32 TS_MPI_ISP_GetGammaAttr(VI_PIPE ViPipe, ISP_GAMMA_ATTR_S *pstGammaAttr);

TS_S32 TS_MPI_ISP_SetDehazeAttr(VI_PIPE ViPipe, const ISP_DEHAZE_ATTR_S *pstDehazeAttr);
TS_S32 TS_MPI_ISP_GetDehazeAttr(VI_PIPE ViPipe, ISP_DEHAZE_ATTR_S *pstDehazeAttr);

TS_S32 TS_MPI_ISP_SetLtmAttr(VI_PIPE ViPipe, const ISP_LTM_ATTR_S *pstLtmAttr);
TS_S32 TS_MPI_ISP_GetLtmAttr(VI_PIPE ViPipe, ISP_LTM_ATTR_S *pstLtmAttr);

TS_S32 TS_MPI_ISP_SetCmbAttr(VI_PIPE ViPipe, const ISP_CMB_ATTR_S *pstCmbAttr);
TS_S32 TS_MPI_ISP_GetCmbAttr(VI_PIPE ViPipe, ISP_CMB_ATTR_S *pstCmbAttr);

TS_S32 TS_MPI_ISP_SetUsrPreference(VI_PIPE ViPipe, ISP_USR_PREFERENCE_S * pstUsrPreference);
TS_S32 TS_MPI_ISP_GetUsrPreference(VI_PIPE ViPipe, ISP_USR_PREFERENCE_S * pstUsrPreference);

TS_S32 TS_MPI_ISP_SetDol2NpAttr(VI_PIPE ViPipe, const ISP_DOL2_NP_ATTR_S *ptsDol2NpAttr);
TS_S32 TS_MPI_ISP_GetDol2NpAttr(VI_PIPE ViPipe, ISP_DOL2_NP_ATTR_S *ptsDol2NpAttr);

TS_S32 TS_MPI_ISP_SetRnrAttr(VI_PIPE ViPipe, const ISP_RNR_ATTR_S *ptsRnrAttr);
TS_S32 TS_MPI_ISP_GetRnrAttr(VI_PIPE ViPipe, ISP_RNR_ATTR_S *ptsRnrAttr);

TS_S32 TS_MPI_ISP_SetLshDltiAttr(VI_PIPE ViPipe, const ISP_LSH_DLTI_ATTR_S *ptsLshDltiAttr);
TS_S32 TS_MPI_ISP_GetLshDltiAttr(VI_PIPE ViPipe, ISP_LSH_DLTI_ATTR_S *ptsLshDltiAttr);

TS_S32 TS_MPI_ISP_SetScaleAttr(VI_PIPE ViPipe, const ISP_SCALE_ATTR_S *ptsScaleAttr);
TS_S32 TS_MPI_ISP_GetScaleAttr(VI_PIPE ViPipe, ISP_SCALE_ATTR_S *ptsScaleAttr);

TS_S32 TS_MPI_ISP_SetCcmAttr(VI_PIPE ViPipe, const ISP_CCM_ATTR_S *pstCcmAttr);
TS_S32 TS_MPI_ISP_GetCcmAttr(VI_PIPE ViPipe, ISP_CCM_ATTR_S *pstCcmAttr);

TS_S32 TS_MPI_ISP_SetGeneralDumpAttr(VI_PIPE ViPipe, const ISP_GENERALDUMP_ATTR_S *pstGeneralDumpAttr);
TS_S32 TS_MPI_ISP_GetGeneralDumpAttr(VI_PIPE ViPipe, ISP_GENERALDUMP_ATTR_S *pstGeneralDumpAttr);

TS_S32 TS_MPI_ISP_SetDpcAttr(VI_PIPE ViPipe, const ISP_DPC_ATTR_S *pstDpcAttr);
TS_S32 TS_MPI_ISP_GetDpcAttr(VI_PIPE ViPipe, ISP_DPC_ATTR_S *pstDpcAttr);

TS_S32 TS_MPI_ISP_SetBlackLevelAttr(VI_PIPE ViPipe, const ISP_BLACKLEVEL_ATTR_S *pstBlackLevelAttr);
TS_S32 TS_MPI_ISP_GetBlackLevelAttr(VI_PIPE ViPipe, ISP_BLACKLEVEL_ATTR_S *pstBlackLevelAttr);

TS_S32 TS_MPI_ISP_SetLscAttr(VI_PIPE ViPipe, const ISP_LSC_ATTR_S *pstLscAttr);
TS_S32 TS_MPI_ISP_GetLscAttr(VI_PIPE ViPipe, ISP_LSC_ATTR_S *pstLscAttr);

TS_S32 TS_MPI_ISP_SetDpfAttr(VI_PIPE ViPipe, const ISP_DPF_ATTR_S *pstDpfAttr);
TS_S32 TS_MPI_ISP_GetDpfAttr(VI_PIPE ViPipe, ISP_DPF_ATTR_S *pstDpfAttr);

TS_S32 TS_MPI_ISP_SetCnrAttr(VI_PIPE ViPipe, const ISP_CNR_ATTR_S *pstCnrAttr);
TS_S32 TS_MPI_ISP_GetCnrAttr(VI_PIPE ViPipe, ISP_CNR_ATTR_S *pstCnrAttr);


TS_S32 TS_MPI_ISP_SetCnrAttr(VI_PIPE ViPipe, const ISP_CNR_ATTR_S *pstCnrAttr);
TS_S32 TS_MPI_ISP_GetCnrAttr(VI_PIPE ViPipe, ISP_CNR_ATTR_S *pstCnrAttr);
TS_VOID TS_MPI_ISP_ResetLock(TS_VOID);
TS_VOID TS_MPI_ISP_ResetUnLock(TS_VOID);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* __MPI_ISP_H__ */

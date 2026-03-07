/*
 * Copyright © 2019 TSING MICRO.com.cn All Rights Reserved
 * Description: mpi_hdmi.h
 * Author:TsingMicro multimedia software group
 * Create: 2022/06/02
 */
#ifndef __MPI_HDMI_H__
#define __MPI_HDMI_H__

#include "ts_comm_hdmi.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


TS_S32 TS_MPI_HDMI_Init(TS_VOID);
TS_S32 TS_MPI_HDMI_DeInit(TS_VOID);
TS_S32 TS_MPI_HDMI_Open(TS_HDMI_ID_E enHdmi);
TS_S32 TS_MPI_HDMI_Close(TS_HDMI_ID_E enHdmi);
TS_S32 TS_MPI_HDMI_GetSinkCapability(TS_HDMI_ID_E enHdmi, TS_HDMI_SINK_CAPABILITY_S *pstSinkCap);
TS_S32 TS_MPI_HDMI_SetAttr(TS_HDMI_ID_E enHdmi, const TS_HDMI_ATTR_S *pstAttr);
TS_S32 TS_MPI_HDMI_GetAttr(TS_HDMI_ID_E enHdmi, TS_HDMI_ATTR_S *pstAttr);
TS_S32 TS_MPI_HDMI_Start(TS_HDMI_ID_E enHdmi);
TS_S32 TS_MPI_HDMI_Stop(TS_HDMI_ID_E enHdmi);
TS_S32 TS_MPI_HDMI_Force_GetEDID(TS_HDMI_ID_E enHdmi, TS_HDMI_EDID_S *pstEdidData);
TS_S32 TS_MPI_HDMI_RegCallbackFunc(TS_HDMI_ID_E enHdmi, const TS_HDMI_CALLBACK_FUNC_S *pstCallbackFunc);
TS_S32 TS_MPI_HDMI_UnRegCallbackFunc(TS_HDMI_ID_E enHdmi, const TS_HDMI_CALLBACK_FUNC_S *pstCallbackFunc);
TS_S32 TS_MPI_HDMI_RegCECCallBackFunc(TS_HDMI_ID_E enHdmi, const TS_HDMI_CECCALLBACK_FUNC_S *pstCECCallback);
TS_S32 TS_MPI_HDMI_UnRegCECCallBackFunc(TS_HDMI_ID_E enHdmi, const TS_HDMI_CECCALLBACK_FUNC_S *pstCECCallback);
TS_S32 TS_MPI_HDMI_SetCECCommand(TS_HDMI_ID_E enHdmi, const TS_HDMI_CEC_CMD_S *pCECCmd);
TS_S32 TS_MPI_HDMI_CECStatus(TS_HDMI_ID_E enHdmi, TS_HDMI_CEC_STATUS_S *pStatus);
TS_S32 TS_MPI_HDMI_CEC_Enable(TS_HDMI_ID_E enHdmi);
TS_S32 TS_MPI_HDMI_CEC_Disable(TS_HDMI_ID_E enHdmi);

/* Module Parameter */
TS_S32 TS_MPI_HDMI_SetModParam(TS_HDMI_ID_E enHdmi, const TS_HDMI_MOD_PARAM_S *pstModParam);
TS_S32 TS_MPI_HDMI_GetModParam(TS_HDMI_ID_E enHdmi, TS_HDMI_MOD_PARAM_S *pstModParam);

/* Normally, these functions are not necessary */
TS_S32 TS_MPI_HDMI_SetInfoFrame(TS_HDMI_ID_E enHdmi, const TS_HDMI_INFOFRAME_S *pstInfoFrame);
TS_S32 TS_MPI_HDMI_GetInfoFrame(TS_HDMI_ID_E enHdmi, TS_HDMI_INFOFRAME_TYPE_E enInfoFrameType, TS_HDMI_INFOFRAME_S *pstInfoFrame);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /*__MPI_HDMI_H__ */


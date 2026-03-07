/*
 * Copyright © 2019 TSING MICRO.com.cn All Rights Reserved
 * Description: mpi_ae.h
 * Author:TsingMicro multimedia software group
 * Create: 2022/06/02
 */
#ifndef __MPI_AE_H__
#define __MPI_AE_H__

#include "ts_comm_3a.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

TS_S32 TS_MPI_AE_Register(VI_PIPE ViPipe, ALG_LIB_S *pstAeLib);
TS_S32 TS_MPI_AE_UnRegister(VI_PIPE ViPipe, ALG_LIB_S *pstAeLib);

TS_S32 TS_MPI_AE_SensorRegCallBack(VI_PIPE ViPipe, ALG_LIB_S *pstAeLib, 
	ISP_SNS_ATTR_INFO_S *pstSnsAttrInfo, AE_SENSOR_REGISTER_S *pstRegister);
TS_S32 TS_MPI_AE_SensorUnRegCallBack(VI_PIPE ViPipe, ALG_LIB_S *pstAeLib, 
	ISP_SNS_ATTR_INFO_S *pstSnsAttrInfo, AE_SENSOR_REGISTER_S *pstRegister);

TS_S32 TS_MPI_ISP_AELibRegCallBack(VI_PIPE ViPipe, ALG_LIB_S *pstAeLib, 
	ISP_AE_REGISTER_S *pstRegister);
TS_S32 TS_MPI_ISP_AELibUnRegCallBack(VI_PIPE ViPipe, ALG_LIB_S *pstAeLib, 
	ISP_AE_REGISTER_S *pstRegister);

TS_S32 TS_MPI_ISP_QueryExposureInfo(VI_PIPE ViPipe, ISP_EXP_INFO_S *pstExpInfo);

TS_S32 TS_MPI_ISP_SetExposureAttr(VI_PIPE ViPipe, ISP_EXPOSURE_ATTR_S *pstExpAttr);
TS_S32 TS_MPI_ISP_GetExposureAttr(VI_PIPE ViPipe, ISP_EXPOSURE_ATTR_S *pstExpAttr);

TS_S32 TS_MPI_ISP_SetAEFunctionAttr(VI_PIPE ViPipe, ISP_AE_FUNCTION_ATTR_S *pstAeFunctionAttr);
TS_S32 TS_MPI_ISP_GetAEFunctionAttr(VI_PIPE ViPipe, ISP_AE_FUNCTION_ATTR_S *pstAeFunctionAttr);

TS_S32 TS_MPI_ISP_SetAeParamAttr(VI_PIPE ViPipe, const ISP_AE_PARAM_ATTR_S *pstAeParamAttr);
TS_S32 TS_MPI_ISP_GetAeParamAttr(VI_PIPE ViPipe, ISP_AE_PARAM_ATTR_S *pstAeParamAttr);

TS_S32 TS_MPI_ISP_SetAeStrategy(VI_PIPE ViPipe, const ISP_AE_STRATEGY_E aeStrategy);
TS_S32 TS_MPI_ISP_GetAeStrategy(VI_PIPE ViPipe, ISP_AE_STRATEGY_E *pStrategy);

TS_S32 TS_MPI_ISP_SetAeFaceParam(VI_PIPE ViPipe, const ISP_AE_FACE_PARAM_S *pstAeFaceParamAttr);
TS_S32 TS_MPI_ISP_GetAeFaceParam(VI_PIPE ViPipe, ISP_AE_FACE_PARAM_S *pstAeFaceParamAttr);

TS_S32 TS_MPI_ISP_SetAeFaceInfo(VI_PIPE ViPipe, const ISP_AE_FACE_INFO_S *pstAeFaceInfo);
TS_S32 TS_MPI_ISP_GetAeFaceInfo(VI_PIPE ViPipe, ISP_AE_FACE_INFO_S *pstAeFaceInfo);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif

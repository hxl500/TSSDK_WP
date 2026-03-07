/*
 * Copyright © 2019 TSING MICRO.com.cn All Rights Reserved
 * Description: mpi_awb.h
 * Author:TsingMicro multimedia software group
 * Create: 2022/06/02
 */
#ifndef __MPI_AWB_H__
#define __MPI_AWB_H__

#include "ts_comm_3a.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

TS_S32 TS_MPI_AWB_Register(VI_PIPE ViPipe, ALG_LIB_S *pstAwbLib);
TS_S32 TS_MPI_AWB_UnRegister(VI_PIPE ViPipe, ALG_LIB_S *pstAwbLib);

TS_S32 TS_MPI_AWB_SensorRegCallBack (VI_PIPE ViPipe, ALG_LIB_S *pstAwbLib, 
	ISP_SNS_ATTR_INFO_S *pstSnsAttrInfo, AWB_SENSOR_REGISTER_S *pstRegister);
TS_S32 TS_MPI_AWB_SensorUnRegCallBack (VI_PIPE ViPipe, ALG_LIB_S *pstAwbLib, 
	ISP_SNS_ATTR_INFO_S *pstSnsAttrInfo, AWB_SENSOR_REGISTER_S *pstRegister);

TS_S32 TS_MPI_ISP_AWBLibRegCallBack(VI_PIPE ViPipe, ALG_LIB_S *pstAwbLib, 
	ISP_AWB_REGISTER_S *pstRegister);
TS_S32 TS_MPI_ISP_AWBLibUnRegCallBack(VI_PIPE ViPipe, ALG_LIB_S *pstAwbLib, 
	ISP_AWB_REGISTER_S *pstRegister);

TS_S32 TS_MPI_ISP_SetAwbEnableAttr(VI_PIPE ViPipe, const ISP_AWB_ENABLE_ATTR_S *pstAwbEnableAttr);
TS_S32 TS_MPI_ISP_GetAwbEnableAttr(VI_PIPE ViPipe, ISP_AWB_ENABLE_ATTR_S *pstAwbEnableAttr);

TS_S32 TS_MPI_ISP_SetAwbConfigAttr(VI_PIPE ViPipe, const ISP_AWB_CONFIG_ATTR_S *pstAwbConfigAttr);
TS_S32 TS_MPI_ISP_GetAwbConfigAttr(VI_PIPE ViPipe, ISP_AWB_CONFIG_ATTR_S *pstAwbConfigAttr);

TS_S32 TS_MPI_ISP_SetAWBParamAttr(VI_PIPE ViPipe, const ISP_AWB_PARAM_S *pstAWBParam);
TS_S32 TS_MPI_ISP_GetAWBParamAttr(VI_PIPE ViPipe, ISP_AWB_PARAM_S *pstAWBParam);

TS_S32 TS_MPI_ISP_SetAWBDetectAttr(VI_PIPE ViPipe, const ISP_AWB_DETECT_ZONE_S *pstAWBDetectZone);
TS_S32 TS_MPI_ISP_GetAWBDetectAttr(VI_PIPE ViPipe, ISP_AWB_DETECT_ZONE_S *pstAWBDetectZone);

TS_S32 TS_MPI_ISP_SetAWBMlczAttr(VI_PIPE ViPipe, const ISP_AWB_MLCZ_S *pstAWBMlcz);
TS_S32 TS_MPI_ISP_GetAWBMlczAttr(VI_PIPE ViPipe, ISP_AWB_MLCZ_S *pstAWBMlcz);

TS_S32 TS_MPI_ISP_SetAWBPrefColorAttr(VI_PIPE ViPipe, const ISP_AWB_PREFCOLOR_S *pstAWBPrefColor);
TS_S32 TS_MPI_ISP_GetAWBPrefColorAttr(VI_PIPE ViPipe, ISP_AWB_PREFCOLOR_S *pstAWBPrefColor);

TS_S32 TS_MPI_ISP_SetAWBExtraColorAttr(VI_PIPE ViPipe, const ISP_AWB_EXTRACOLOR_S *pstAWBExtraColor);
TS_S32 TS_MPI_ISP_GetAWBExtraColorAttr(VI_PIPE ViPipe, ISP_AWB_EXTRACOLOR_S *pstAWBExtraColor);

TS_S32 TS_MPI_ISP_SetAWBSpecialGainAttr(VI_PIPE ViPipe, const ISP_AWB_SPECIALGAIN_S *pstAWBSpecialGain);
TS_S32 TS_MPI_ISP_GetAWBSpecialGainAttr(VI_PIPE ViPipe, ISP_AWB_SPECIALGAIN_S *pstAWBSpecialGain);

TS_S32 TS_MPI_ISP_SetAWBSpecialGainWeightAttr(VI_PIPE ViPipe, const ISP_AWB_SPECIALGAINWEIGHT_S *pstAWBSpecialGainWeight);
TS_S32 TS_MPI_ISP_GetAWBSpecialGainWeightAttr(VI_PIPE ViPipe, ISP_AWB_SPECIALGAINWEIGHT_S *pstAWBSpecialGainWeight);

TS_S32 TS_MPI_ISP_SetAWBNewAttr(VI_PIPE ViPipe, const ISP_AWB_NEW_S *pstAWBNew);
TS_S32 TS_MPI_ISP_GetAWBNewAttr(VI_PIPE ViPipe, ISP_AWB_NEW_S *pstAWBNew);

TS_S32 TS_MPI_ISP_SetAWBLimitYAttr(VI_PIPE ViPipe, const ISP_AWB_LIMITY_S *pstAWBLimitY);
TS_S32 TS_MPI_ISP_GetAWBLimitYAttr(VI_PIPE ViPipe, ISP_AWB_LIMITY_S *pstAWBLimitY);

TS_S32 TS_MPI_ISP_SetAWBLightSourceAttr(VI_PIPE ViPipe, const ISP_AWB_LIGHTSOURCE_S *pstAWBLightSource);
TS_S32 TS_MPI_ISP_GetAWBLightSourceAttr(VI_PIPE ViPipe, ISP_AWB_LIGHTSOURCE_S *pstAWBLightSource);

TS_S32 TS_MPI_ISP_SetAWBFaceAttr(VI_PIPE ViPipe, const ISP_AWB_FACE_S *pstAWBFace);
TS_S32 TS_MPI_ISP_GetAWBFaceAttr(VI_PIPE ViPipe, ISP_AWB_FACE_S *pstAWBFace);

TS_S32 TS_MPI_ISP_QueryWBInfo(VI_PIPE ViPipe, ISP_WB_INFO_S *pstWBInfo);
	

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif

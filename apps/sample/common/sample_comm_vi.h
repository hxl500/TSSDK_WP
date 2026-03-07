

#ifndef __SAMPLE_COMM_VI_H__
#define __SAMPLE_COMM_VI_H__

#include <pthread.h>

#include "sample_comm_base.h"

#include "ts_comm_sys.h"

#include "mpi_vi.h"

typedef struct tsSAMPLE_PIPE_INFO_S {
	VI_PIPE         aPipe;
	VI_VPSS_MODE_E  enMastPipeMode;
	PIXEL_FORMAT_E  enPixFmt;
	DATA_BITWIDTH_E	enBitWid;
	ISP_BAYER_FORMAT_E  enBayer;
	WDR_MODE_E		enWdrMode;
	TS_BOOL			bIspByFly;
	TS_BOOL			bDynFpsSync;
	TS_U32			width;
	TS_U32			height;
	TS_FLOAT		frameRate;
} SAMPLE_PIPE_INFO_S;

typedef struct tsSAMPLE_CHN_INFO_S {
	VI_CHN			ViChn[VI_MAX_CHN_NUM];
	TS_U32			validChnlNum;
	PIXEL_FORMAT_E	enPixFormat;
	TS_U32			width[VI_MAX_CHN_NUM];
	TS_U32			height[VI_MAX_CHN_NUM];
} SAMPLE_CHN_INFO_S;

typedef struct tsSAMPLE_VI_INFO_S {
	SAMPLE_PIPE_INFO_S      stPipeInfo;
	SAMPLE_CHN_INFO_S       stChnInfo;
} SAMPLE_VI_INFO_S;

typedef struct tsSAMPLE_VI_CONFIG_S {
	SAMPLE_VI_INFO_S    astViInfo[VI_MAX_PHY_PIPE_NUM];
	TS_S32              s32WorkingViNum;
} SAMPLE_VI_CONFIG_S;


typedef struct tsSAMPLE_VI_USERPIC_S {
	VI_USERPIC_ATTR_S    astViUserpicInfo[VI_MAX_PHY_PIPE_NUM];
	TS_BOOL bUserpicSet[VI_MAX_PHY_PIPE_NUM];
	TS_S32              s32WorkingViNum;
} SAMPLE_VI_USERPIC_S;

/*******************************************************
 *   function announce
 *******************************************************/

TS_S32 SAMPLE_COMM_VI_StartChn(VI_CHN ViChn, RECT_S *pstCapRect, SIZE_S *pstTarSize, SAMPLE_VI_CONFIG_S *pstViConfig);
TS_S32 SAMPLE_COMM_VI_StartVi(SAMPLE_VI_CONFIG_S *pstViConfig);
TS_S32 SAMPLE_COMM_VI_StopVi(SAMPLE_VI_CONFIG_S *pstViConfig);
TS_VOID SAMPLE_COMM_VI_GetSensorInfo(SAMPLE_VI_CONFIG_S *pstViConfig);

TS_S32 SAMPLE_COMM_VI_SetParam(SAMPLE_VI_CONFIG_S *pstViConfig);

TS_S32 SAMPLE_COMM_VI_StartVi_And_Aiisp(SAMPLE_VI_CONFIG_S *pstViConfig);
TS_S32 SAMPLE_COMM_VI_StopVi_And_Aiisp(SAMPLE_VI_CONFIG_S *pstViConfig);


#endif /* End of #ifndef __SAMPLE_COMMON_H__ */

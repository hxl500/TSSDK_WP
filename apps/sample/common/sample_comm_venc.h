

#ifndef __SAMPLE_COMM_VENC_H__
#define __SAMPLE_COMM_VENC_H__

#include <pthread.h>

#include "sample_comm_base.h"

#include "ts_common.h"
#include "ts_buffer.h"
#include "ts_comm_venc.h"
#include "sample_comm_vpss.h"
#include "sample_comm_vi.h"
#include "mpi_venc.h"

typedef struct tsSAMPLE_VENC_UPDAETFPS_S {
	TS_BOOL bThreadStart;
	VENC_CHN VeChn[VENC_MAX_CHN_NUM];
	TS_S32  s32Cnt;
} SAMPLE_VENC_UPDAETFPS_S;

typedef struct tsSAMPLE_VENC_CHNL_ATTR_S{
	PAYLOAD_TYPE_E enType;
	PIC_SIZE_E enSize;
	TS_U32 u32FrameRate;
	SAMPLE_RC_E enRcMode;
	TS_U32 u32Profile;
	TS_U32 u32Bitrate;
	TS_U32 u32Gop;
	VENC_GOP_ATTR_S stGopAttr;
	TS_U32 u32Width;
	TS_U32 u32Height;
	TS_U32 u32AvgBitRate;
	TS_U32 u32MaxBitrate;
}SAMPLE_VENC_CHNL_ATTR_S;

typedef enum{

	SMART_ENCODE_TYPE_INVALID = 0x00, // 无效
	SMART_ENCODE_TYPE_DEFAULT = 0x01, // 默认标准
	SMART_ENCODE_TYPE_DIANXIN = 0x02, // 电信标准
	SMART_ENCODE_TYPE_YIDONG = 0x03, // 移动标准

}SAMPLE_SMART_ENCODE_TYPE_E;

/*******************************************************
 *   function announce
 *******************************************************/
TS_S32 SAMPLE_COMM_VENC_Creat(VENC_CHN VencChn, PAYLOAD_TYPE_E enType,  PIC_SIZE_E enSize, TS_U32 u32FrameRate, SAMPLE_RC_E enRcMode,
		TS_U32  u32Profile, VENC_GOP_ATTR_S *pstGopAttr);

TS_S32 SAMPLE_COMM_VENC_CreatAllocBufOnce(VENC_CHN VencChn, PAYLOAD_TYPE_E enType,  PIC_SIZE_E enSize, TS_U32 u32FrameRate, SAMPLE_RC_E enRcMode,
	TS_U32  u32Profile, VENC_GOP_ATTR_S *pstGopAttr);


TS_S32 SAMPLE_COMM_VENC_Start(VENC_CHN VencChn, PAYLOAD_TYPE_E enType, PIC_SIZE_E enSize, TS_U32 u32FrameRate, SAMPLE_RC_E enRcMode,
			TS_U32 u32Profile, VENC_GOP_ATTR_S *pstGopAttr);

TS_S32 SAMPLE_COMM_VENC_StartEx(VENC_CHN VencChn, SAMPLE_VENC_CHNL_ATTR_S * pChnlAttr);

TS_S32 SAMPLE_COMM_VENC_Start_By_Num(VENC_CHN VencChn, PAYLOAD_TYPE_E enType,  PIC_SIZE_E enSize, TS_U32 u32FrameRate, SAMPLE_RC_E enRcMode,
			TS_U32  u32Profile, VENC_GOP_ATTR_S *pstGopAttr, TS_S32 s32Num);

TS_S32 SAMPLE_COMM_VENC_Start_AllocBufOnce(VENC_CHN VencChn, PAYLOAD_TYPE_E enType,  PIC_SIZE_E enSize, TS_U32 u32FrameRate, SAMPLE_RC_E enRcMode,
        TS_U32  u32Profile, VENC_GOP_ATTR_S *pstGopAttr);

TS_S32 SAMPLE_COMM_VENC_Stop(VENC_CHN VencChn);
TS_S32 SAMPLE_COMM_VENC_SnapStart(VENC_CHN VencChn, SIZE_S *pstSize, TS_BOOL bSupportDCF);
TS_S32 SAMPLE_COMM_VENC_SnapProcess(VENC_CHN VencChn, TS_U32 SnapCnt, TS_BOOL bSaveJpg, TS_BOOL bSaveThm);
TS_S32 SAMPLE_COMM_VENC_UpdateFps(VENC_CHN VeChn[],TS_S32 s32Cnt);
TS_S32 SAMPLE_COMM_VENC_StopUpdateFps(void);

TS_S32 SAMPLE_COMM_VENC_SwitchResolution(VENC_CHN VeChn, TS_U32 width, TS_U32 height);
TS_S32 SAMPLE_COMM_VENC_SwitchEncoder(VENC_CHN VeChn, PAYLOAD_TYPE_E type,VENC_RC_MODE_E enRcMode);
TS_S32 SAMPLE_COMM_VENC_Manual_AREAJPEG(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VENC_CHN VencChn,
														TS_U32 x, TS_U32 y, TS_U32 w, TS_U32 h);
TS_S32 SAMPLE_COMM_VENC_SaveJpeg(VENC_CHN VencChn, TS_U32 SnapCnt);
TS_S32 SAMPLE_COMM_VENC_SnapStop(VENC_CHN VencChn);
TS_S32 SAMPLE_COMM_VENC_StartGetStream(VENC_CHN VeChn[], TS_S32 s32Cnt);
TS_S32 SAMPLE_COMM_VENC_StartGetStreamV2(VENC_CHN VeChn[],TS_S32 s32Cnt, SAMPLE_VENC_CONSUME_TYPE type);
TS_S32 SAMPLE_COMM_VENC_StopGetStream(void);
TS_S32 SAMPLE_COMM_VENC_GetGopAttr(VENC_GOP_MODE_E enGopMode, VENC_GOP_ATTR_S *pstGopAttr);
TS_S32 SAMPLE_COMM_VENC_StopSendFrame(void);
TS_S32 SAMPLE_COMM_VENC_SendFrame(VENC_CHN VencChn, VIDEO_FRAME_INFO_S * pFrame, TS_S32 s32MilliSec);
TS_S32 SAMPLE_COMM_VENC_CloseReEncode(VENC_CHN VencChn);
TS_S32 SAMPLE_COMM_VENC_SetSmartEnable(VENC_CHN VencChn, TS_U8 u8Enable);
TS_S32 SAMPLE_COMM_VENC_GetSmartEnable(VENC_CHN VencChn, TS_U8 *u8Enable);
TS_S32 SAMPLE_COMM_VENC_GetRecvNum(VENC_CHN VencChn);
TS_S32 SAMPLE_COMM_VENC_SaveStream(FILE* pFd, VENC_STREAM_S* pstStream);
TS_VOID SAMPLE_COMM_VENC_SetTestSavePath(TS_CHAR * pFilePath);
TS_S32 SAMPLE_COMM_VENC_SnapProcessUnBind(VENC_CHN VencChn, TS_U32 SnapCnt, TS_BOOL bSaveJpg, TS_BOOL bSaveThm);

TS_S32 SAMPLE_COMM_VENC_SetSmartEncodeType(VENC_CHN VencChn, TS_S32 s32Type);

#endif /* End of #ifndef __SAMPLE_COMMON_H__ */

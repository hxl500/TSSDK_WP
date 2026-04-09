

#ifndef __SAMPLE_COMM_VDEC_H__
#define __SAMPLE_COMM_VDEC_H__

#include <pthread.h>

#include "ts_common.h"
#include "ts_comm_vdec.h"

#include "mpi_vdec.h"




typedef struct tsSAMPLE_VDEC_VIDEO_ATTR {
	VIDEO_DEC_MODE_E enDecMode;
	TS_U32              u32RefFrameNum;
	DATA_BITWIDTH_E  enBitWidth;
} SAMPLE_VDEC_VIDEO_ATTR;


typedef struct tsSAMPLE_VDEC_PICTURE_ATTR {
	PIXEL_FORMAT_E enPixelFormat;
	TS_U32         u32Alpha;
} SAMPLE_VDEC_PICTURE_ATTR;


typedef struct tsSAMPLE_VDEC_ATTR {
	PAYLOAD_TYPE_E enType;
	VIDEO_MODE_E   enMode;
	TS_U32 u32Width;
	TS_U32 u32Height;
	TS_U32 u32FrameBufCnt;
	TS_U32 u32DisplayFrameNum;
	union {
		SAMPLE_VDEC_VIDEO_ATTR stSapmleVdecVideo;      /* structure with video ( h265/h264) */
		SAMPLE_VDEC_PICTURE_ATTR stSapmleVdecPicture; /* structure with picture (jpeg/mjpeg )*/
	};
} SAMPLE_VDEC_ATTR;

typedef enum tsTHREAD_CONTRL_E {
	THREAD_CTRL_START,
	THREAD_CTRL_PAUSE,
	THREAD_CTRL_STOP,
} THREAD_CONTRL_E;

typedef struct tsVDEC_THREAD_PARAM_S {
	TS_S32 s32ChnId;
	PAYLOAD_TYPE_E enType;
	TS_CHAR cFilePath[128];
	TS_CHAR cFileName[128];
	TS_CHAR cUrl[256];
	TS_CHAR cProgName[32];
	TS_S32 s32StreamMode;
	TS_S32 s32MilliSec;
	TS_S32 s32MinBufSize;
	TS_S32 s32IntervalTime;
	THREAD_CONTRL_E eThreadCtrl;
	TS_U64  u64PtsInit;
	TS_U64  u64PtsIncrease;
	TS_BOOL bCircleSend;
	TS_BOOL bUsedBuffer;
	TS_S32  s32RecvTotalSize;
} VDEC_THREAD_PARAM_S;

typedef struct tsSAMPLE_VDEC_BUF {
	TS_U32  u32PicBufSize;
	TS_U32  u32TmvBufSize;
	TS_BOOL bPicBufAlloc;
	TS_BOOL bTmvBufAlloc;
} SAMPLE_VDEC_BUF;


/*******************************************************
 *   function announce
 *******************************************************/
TS_VOID SAMPLE_COMM_VDEC_ExitVBPool(TS_VOID);
TS_S32 SAMPLE_COMM_VDEC_InitVBPool(TS_U32 ChnNum, SAMPLE_VDEC_ATTR *pastSampleVdec);
TS_VOID * SAMPLE_COMM_VDEC_SendStream(TS_VOID *pArgs);
TS_VOID SAMPLE_COMM_VDEC_CmdCtrl(TS_S32 s32ChnNum,VDEC_THREAD_PARAM_S *pstVdecSend, pthread_t *pVdecThread);
TS_VOID SAMPLE_COMM_VDEC_StartSendStream(TS_S32 s32ChnNum, VDEC_THREAD_PARAM_S *pstVdecSend, pthread_t *pVdecThread);
TS_VOID SAMPLE_COMM_VDEC_StopSendStream(TS_S32 s32ChnNum, VDEC_THREAD_PARAM_S *pstVdecSend, pthread_t *pVdecThread);
TS_VOID * SAMPLE_COMM_VDEC_GetPic(TS_VOID *pArgs);
TS_VOID SAMPLE_COMM_VDEC_StartGetPic(TS_S32 s32ChnNum, VDEC_THREAD_PARAM_S *pstVdecGet, pthread_t *pVdecThread);
TS_VOID SAMPLE_COMM_VDEC_StopGetPic(TS_S32 s32ChnNum, VDEC_THREAD_PARAM_S *pstVdecGet, pthread_t *pVdecThread);
TS_S32 SAMPLE_COMM_VDEC_Start(TS_S32 s32ChnNum, SAMPLE_VDEC_ATTR *pastSampleVdec);
TS_S32 SAMPLE_COMM_VDEC_Stop(TS_S32 s32ChnNum);
TS_S32 SAMPLE_COMM_VDEC_RTSP_StartCPM(AVS_GRP CPMGrp);
TS_S32 SAMPLE_COMM_VDEC_RTSP_StopCPM(AVS_GRP CPMGrp, TS_BOOL benChn1);
TS_S32 SAMPLE_COMM_VDEC_Bind_CPM(VDEC_CHN VdecChn, CPM_GRP CpmGrp, CPM_PIPE CpmPipe);
TS_S32 SAMPLE_COMM_VDEC_UnBind_CPM(VDEC_CHN VdecChn, CPM_GRP CpmGrp, CPM_PIPE CpmPipe);
TS_S32 SAMPLE_COMM_VDEC_UnBind_VO(VDEC_CHN VdecChn, VO_LAYER VoLayer, VO_CHN VoChn);
TS_S32 SAMPLE_COMM_VDEC_Bind_VO(VDEC_CHN VdecChn, VO_LAYER VoLayer, VO_CHN VoChn);
TS_VOID SAMPLE_COMM_VDEC_StartSendStream_From_URL(TS_S32 s32ChnNum, VDEC_THREAD_PARAM_S *pstVdecSend, pthread_t *pVdecThread);

#endif /* End of #ifndef __SAMPLE_COMMON_H__ */

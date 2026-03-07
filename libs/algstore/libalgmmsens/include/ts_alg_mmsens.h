#ifndef __TS_ALG_MMSENS_H__
#define __TS_ALG_MMSENS_H__

#include "ts_alg_type.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define INFO_RIGHT 0 // Succeed initialize SKV preprocessor
#define INFO_PARAM_ERROR 2 // Cann't support the parameter of audio that you input
#define INFO_EXCEEDDATE_ERROR 3 // Exceed the limited date
#define INFO_UNKNOW_ERROR 4 // Unknown error
#define FRAME_SIZE 128
#define FRAME_SIZE_PBNLMS 64

typedef struct{
	TS_S32 s32SmplRatio; // 采样率，目前只支持16000，8000，32000
	TS_S32 s32BitsPerSample; // 16
	TS_S32 s32NumSrcChannel; // mic通道数，目前传1即可
	TS_S32 s32NumRefChannel; // 参考信号通道数，传0
	TS_S32 s32FrameSize; // fft的窗移，目前8000采样率应为128
	TS_S32 s32BlockLen; // 每帧数据长度，目前8000采样率下，10ms为80
	TS_S32 s32NoiseSuppress;
} ALG_MMSENS_INIT_S;

typedef struct{
	TS_S16 *pIn;
	TS_S16 *pInmix;
	TS_U32 u32InLen;
} ALG_MMSENS_PROC_S;

typedef struct{
	TS_S16 *pOut;
	TS_U32 u32OutLen;
} ALG_MMSENS_RSLT_S;


TS_S32 TS_ALG_MMSENS_Init(TS_VOID **ppHandle, ALG_MMSENS_INIT_S *pParam);

TS_S32 TS_ALG_MMSENS_Process(TS_VOID *pHandle, ALG_MMSENS_PROC_S *pInput, ALG_MMSENS_RSLT_S *pOutput);

TS_S32 TS_ALG_MMSENS_Exit(TS_VOID *pHandle);

const TS_CHAR *TS_ALG_MMSENS_GetVersion(TS_VOID);


#ifdef __cplusplus
}
#endif

#endif /* __TS_ALG_MMSENS_H__ */

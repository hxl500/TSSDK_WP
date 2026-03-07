#ifndef __TS_ALG_ADAPTAEC_H__
#define __TS_ALG_ADAPTAEC_H__

#include "ts_alg_type.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef enum tsALG_ADAPT_AEC_MODE_E {
	ADAPT_AEC_MODE0=0, //0-回声抑制最弱，但是对语音破坏最小
	ADAPT_AEC_MODE1,
	ADAPT_AEC_MODE2, //2-回声抑制最狠，但是对语音破坏最大
	ADAPT_AEC_MODE_NUM
} ALG_ADAPT_AEC_MODE_E;

typedef struct tsALG_ADAPTAEC_INIT_S {
	TS_U32 u32SampleRate;       // 采样率，目前只支持16000，8000
	TS_U32 u32BitsPerSample;    // 采样位宽，16
	TS_U32 u32MicChnNum;       // mic通道数，可以是1或多个
	TS_U32 u32RefChnNum;       // 参考信号通道数，只支持1
	TS_U32 u32FrameLen;        // 帧长， 目前只支持8ms、16ms帧长
	ALG_ADAPT_AEC_MODE_E eNlpMode; //nlp 模式（非线性AEC的强度），可以为0、1、2； 
} ALG_ADAPTAEC_INIT_S;

typedef struct tsALG_ADAPTAEC_PROC_S {
	TS_S16 *pMicData;      //多通道时，交错存放
	TS_U32 u32MicDataLen; //总采样点数，每个通道采样点需/u32MicChnNum
    TS_U32 u32MicChnNum;    //通道数
	TS_S16 *pRefData;
	TS_U32 u32RefDataLen;
	TS_U32 u32RefChnNum; //目前只支持1通道
} ALG_ADAPTAEC_PROC_S;

typedef struct tsALG_ADAPTAEC_RSLT_S {
	TS_S16 *pOut;       //多通道时，交错存放
	TS_U32  u32ChnNum;  //通道数
	TS_U32  u32OutLen;  //总采样点数，每个通道采样点需/u32ChnNum
} ALG_ADAPTAEC_RSLT_S;


TS_S32 TS_ALG_ADAPTAEC_Init(TS_VOID **ppHandle, ALG_ADAPTAEC_INIT_S *pParam);

TS_S32 TS_ALG_ADAPTAEC_Process(TS_VOID *pHandle, ALG_ADAPTAEC_PROC_S *pInput, ALG_ADAPTAEC_RSLT_S *pOutput);

TS_S32 TS_ALG_ADAPTAEC_Exit(TS_VOID *pHandle);

const TS_CHAR *TS_ALG_ADAPTAEC_GetVersion(TS_VOID);


#ifdef __cplusplus
}
#endif

#endif

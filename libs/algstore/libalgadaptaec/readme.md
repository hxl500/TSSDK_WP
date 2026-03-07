[TOC]

## 多通道自适应回声消除算法


### 1.1. 算法类型

音频

### 1.2. 算法描述

adaptaec是一种自适应的快速回声消除算法, 支持多MIC，1个通道的参考信号。

### **1.3.** API参考

## 初始化函数
TS_S32 TS_ALG_ADAPTAEC_Init(TS_VOID **ppHandle, ALG_ADAPTAEC_INIT_S *pParam);

+ 参数

|参数     |说明       |
|:-------:|:---------:|
|ppHandle |返回句柄     |
|pParam   |需要用到的参数|

+ 数据类型
	+ ALG_ANR_AEC_INIT_S

`typedef struct{`

   `TS_U32 u32SampleRate;`

   `TS_U32 u32BitsPerSample;`

   `TS_U32 u32MicChnNum;`

   `TS_U32 u32RefChnNum;`

   `TS_U32 u32FrameLen;`

   `ALG_ADAPT_AEC_MODE_E eNlpMode;` 

`} ALG_ADAPTAEC_INIT_S;`


|成员             |说明|
|:--------------:|:----------------------:|
|u32SampleRate   |采样率，目前只支持16000，8000|
|u32BitsPerSample|采样位宽，目前为16|
|u32MicChnNum    |mic通道数，可以是1或多个|
|u32RefChnNum    |参考信号通道数，只支持1|
|u32FrameLen     |帧长， 目前只支持8ms、16ms帧长|
|eNlpMode        |nlp 模式（非线性AEC的强度），可以为0、1、2。|

	+ ALG_ADAPT_AEC_MODE_E

`typedef enum tsALG_ADAPT_AEC_MODE_E {`

   `ADAPT_AEC_MODE0=0,`

   `ADAPT_AEC_MODE1,`

   `ADAPT_AEC_MODE2,`

   `ADAPT_AEC_MODE_NUM`

`} ALG_ADAPT_AEC_MODE_E;`


|成员           |         说明                      |
|:--------------|:---------------------------------|
|ADAPT_AEC_MODE0|回声抑制最弱，但是对语音破坏最小|
|ADAPT_AEC_MODE1|回声抑制中，但是对语音破坏中|
|ADAPT_AEC_MODE2|回声抑制最狠，但是对语音破坏最大|


## 运行主处理函数
TS_S32 TS_ALG_ADAPTAEC_Process(TS_VOID *pHandle, ALG_ADAPTAEC_PROC_S *pInput, ALG_ADAPTAEC_RSLT_S *pOutput);

+参数

|参数       |说明               |
|:---------:|:------------------:|
|pHandle    |句柄|input       |
|pInput     |需要用到的参数    | 
|pOutput    |输出处理音频采样   | 

+ 数据类型
	+ ALG_ADAPTAEC_PROC_S

`typedef struct tsALG_ADAPTAEC_PROC_S {`

  `TS_S16 *pMicData;`

  `TS_U32 u32MicDataLen;`

  `TS_U32 u32MicChnNum;`

  `TS_S16 *pRefData;`

  `TS_U32 u32RefDataLen;`

  `TS_U32 u32RefChnNum;`

`} ALG_ADAPTAEC_PROC_S;`


|成员          |说明                           |
|:-------------|:--------------------------------------------------|
|pMicData      |mic采样地址，多通道时，不同通道数据交错存放|
|u32MicDataLen |Mic采集到信号的长度。每个通道采样点需/u32MicChnNum|
|u32MicChnNum  |Mic通道数|
|pRefData      |Ref信号的地址|
|u32RefDataLen |Ref信号的地址长度，当采样率为16kHz时，可配置为256/128；当采样率为8kHz，可配置为64/128。|
|u32RefChnNum  |Ref信号的通道数，只支持1|

	+ ALG_ADAPTAEC_RSLT_S

`typedef struct tsALG_ADAPTAEC_RSLT_S {`

   `TS_S16 *pOut;`

   `TS_U32  u32ChnNum;`

   `TS_U32  u32OutLen;`

`} ALG_ADAPTAEC_RSLT_S;`


|成员      |说明                    |
|:---------|:--------------------------|
|pOut      |处理后的音频采样, 多通道时，不同通道数据交错存放|
|u32ChnNum |输出采样长度|
|u32OutLen |总采样点数，每个通道采样点需/u32ChnNum|

## 退出函数
TS_S32 TS_ALG_ADAPTAEC_Exit(TS_VOID *pHandle);

+ 参数

|参数|说明|属性|
|:---:|:---:|:---:|
|pHandle|句柄|input|


## 版本获取函数
const TS_CHAR *TS_ALG_ADAPTAEC_GetVersion(TS_VOID);

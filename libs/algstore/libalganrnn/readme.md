[TOC]

## NN降噪


### 1.1. 算法类型

音频

### 1.2. 算法描述

用NN模型对回声进行消除和降噪。

### 1.3. API参考

## 初始化函数
TS_S32 TS_ALG_ANR_NN_Init(TS_VOID **ppHandle, ALG_ANR_NN_INIT_S *pParam);

+ 参数

|参数|说明|属性|
|:---:|:---:|:---:|
|ppHandle|返回句柄|output|
|pParam|需要用到的参数|input|

+ 数据类型
	+ ALG_ANR_NN_INIT_S

	|成员|说明|
	|:---|:---|
	|pGraph|用于配置ANR运行时需要用到的模型的config文件的地址。|
	|pWeight|用于配置ANR运行时需要用到的模型的weight文件的地址。|
	|u32GraphSize|用于配置ANR运行时需要用到的模型的config文件的大小。|
	|vpSelfBuf|用于配置ANR用到的模型所需内存的地址。|
	|s32SmplRatio|采样率。支持16000/8000|
	|s16FrameLen|帧长。当采样率为16kHz时，请配置为256；当采样率为8kHz，请配置为128。
	|s16ChanNum|通道数量。取值范围：1/2（支持单声道和双声道）。|


## 运行主处理函数
TS_S32 TS_ALG_ANR_NN_Process(TS_VOID *pHandle, ALG_ANR_NN_PORC_S *pInput, ALG_ANR_NN_RSLT_S *pOutput);

+参数

|参数|说明|属性|
|:---:|:---:|:---:|
|pHandle|句柄|input|
|pInput|需要用到的参数|input|
|pOutput|输出处理音频采样|output|

+ 数据类型
	+ ALG_ANR_NN_PORC_S

	|成员|说明|
	|:---|:---|
	|ps16Data|采样地址|
	|u32DataLen|采集到信号的长度。当采样率为16kHz时，请配置为256；当采样率为8kHz，请配置为128。|

	+ ALG_ANR_NN_RSLT_S

	|成员|说明|
	|:---|:---|
	|ps16OutData|处理后的音频采样|
	|u32OutLen|输出采样长度|

## 退出函数
TS_S32 TS_ALG_ANR_NN_Exit(TS_VOID *pHandle);

+ 参数

|参数|说明|属性|
|:---:|:---:|:---:|
|pHandle|句柄|input|

## 参数设置函数
TS_S32 TS_ALG_ANR_NN_SetParam(TS_VOID* pHandle, ALG_ANR_NN_PARAM_S *pConfig);

+ 参数

|参数|说明|属性|
|:---:|:---:|:---:|
|pHandle|句柄|input|
|pConfig|需要设置的参数|input|

+ 数据类型

## 参数获取函数
TS_S32 TS_ALG_ANR_NN_SetParam(TS_VOID* pHandle, ALG_ANR_NN_PARAM_S *pConfig);

+ 参数

|参数|说明|属性|
|:---:|:---:|:---:|
|pHandle|句柄|input|
|pConfig|需要设置的参数|output|

## 版本获取函数
const TS_CHAR *TS_ALG_ANR_NN_GetVersion(TS_VOID);

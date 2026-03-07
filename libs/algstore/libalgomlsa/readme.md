[TOC]

## 降噪

### 1.1. 算法类型

音频

### 1.2. 算法描述

降噪

### 1.3. API参考

## 初始化函数
TS_S32 TS_ALG_OMLSA_Init(TS_VOID **ppHandle, ALG_OMLSA_INIT_S *pParam);

+ 参数

|参数|说明|属性|
|:---:|:---:|:---:|
|ppHandle|返回句柄|output|
|pParam|需要用到的参数|input|

+ 数据类型
	+ TS_ALG_OMLSA_Init

	|成员|说明|
	|:---|:---|
	|s32SmplRatio|采样率。支持16000/8000|
	|s32BitsPerSample|采样位宽，16|
	|s32NumSrcChannel|mic通道数，1|
	|s32NumRefChannel|参考信号通道数，0|
	|s32FrameSize|fft的窗移，8K:128; 16k:256|
	|s32BlockLen|8k:80; 16k:160|
	|s32NoiseSuppress|噪声抑制强度，default:-15|


## 运行主处理函数
TS_S32 TS_ALG_OMLSA_Process(TS_VOID *pHandle, ALG_OMLSA_PROC_S *pInput, ALG_OMLSA_RSLT_S *pOutput);

+ 参数

|参数|说明|属性|
|:---:|:---:|:---:|
|pHandle|句柄|input|
|pInput|需要用到的参数|input|
|pOutput|输出处理音频采样|output|

+ 数据类型
	+ ALG_OMLSA_PROC_S

	|成员|说明|
	|:---|:---|
	|pIn|m采样地址|
	|u32InLen|采样长度|

	+ ALG_OMLSA_RSLT_S

	|成员|说明|
	|:---|:---|
	|pOut|输出采样地址|
	|u32OutLen|输出采样长度|

## 退出函数
TS_S32 TS_ALG_OMLSA_Exit(TS_VOID *pHandle);

+ 参数

|参数|说明|属性|
|:---:|:---:|:---:|
|pHandle|句柄|input|

## 版本获取函数
const TS_CHAR *TS_ALG_OMLSA_GetVersion(TS_VOID);

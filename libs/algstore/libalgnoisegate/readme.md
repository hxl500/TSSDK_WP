[TOC]

##关键词识别

### 1.1. 算法类型

音频

### 1.2. 算法描述

去除底噪。

### **1.3.** API参考

## 初始化函数
TS_S32 TS_ALG_NoiseGate_Init(TS_VOID **ppHandle, ALG_NOISEGATE_INIT_S *pParam);

+ 参数

|参数|说明|属性|
|:---:|:---:|:---:|
|ppHandle|返回句柄|output|
|pParam|需要用到的参数|input|

+ 数据类型
	+ ALG_NOISEGATE_INIT_S

	|成员|说明|
	|:---|:---|
	|s32SmplRatio|采样率。支持16000/8000|
	|fAttackTime|算法生效时间，单位：s|
	|fReleaseTime|算法退出时间，单位：s|
	|s32Threshold|噪声门限，单位：dB，推荐使用：-70|


## 运行主处理函数
TS_S32 TS_ALG_NoiseGate_Process(TS_VOID *pHandle, ALG_NOISEGATE_PROC_S *pInput, ALG_NOISEGATE_RSLT_S *pOutput);

+ 参数

|参数|说明|属性|
|:---:|:---:|:---:|
|pHandle|句柄|input|
|pInput|需要用到的参数|input|
|pOutput|输出处理音频采样|output|

+ 数据类型
	+ ALG_NOISEGATE_PROC_S

	|成员|说明|
	|:---|:---|
	|pIn|采样地址|
	|u32InLen|采样长度|

	+ ALG_NOISEGATE_RSLT_S

	|成员|说明|
	|:---|:---|
	|pOut|处理结果|
	|u32InLen|采样长度|

## 退出函数
TS_S32 TS_ALG_NoiseGate_Exit(TS_VOID *pHandle);

+ 参数

|参数|说明|属性|
|:---:|:---:|:---:|
|pHandle|句柄|input|

## 版本获取函数
const TS_CHAR *TS_ALG_NoiseGate_GetVersion(TS_VOID);

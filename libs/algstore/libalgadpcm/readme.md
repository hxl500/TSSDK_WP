[TOC]

## ADPCM编解码

### 1.1. 算法类型

音频

### 1.2. 算法描述

ADPCM编解码是把音频采样数据编码为ADPCM码流或把ADPCM码流转换为采样数据。

### 1.3. API参考

## 初始化函数
TS_S32 TS_ALG_ADPCM_Init(TS_VOID **ppHandle);

+ 参数

|参数|说明|属性|
|:---:|:---:|:---:|
|ppHandle|返回句柄|output|

## 运行主处理编码函数
TS_VOID TS_ALG_ADPCM_Encode(TS_VOID *pHandle, ALG_ADPCM_SMPL_S *pInput, ALG_ADPCM_STREAM_S *pOutput);
+ 参数

|参数|说明|属性|
|:---:|:---:|:---:|
|pHandle|句柄|input|
|pInput|需要用到的参数|input|
|pOutput|输出结果|output|

+ 数据类型
	+ ALG_ADPCM_SMPL_S

	|成员|说明|
	|:---|:---|
	|pSmpl|采样地址|
	|u32ByteLen|字节长度|

	+ ALG_ADPCM_STREAM_S

	|成员|说明|
	|:---|:---|
	|pStream|编码后比特流地址|
	|u32ByteLen|编码后输出字节数|

## 运行主处理解码函数
TS_VOID TS_ALG_ADPCM_Decode(TS_VOID *pHandle, ALG_ADPCM_STREAM_S *pInput, ALG_ADPCM_SMPL_S *pOutput);

+ 参数

|参数|说明|属性|
|:---:|:---:|:---:|
|pHandle|句柄|input|
|pInput|需要用到的参数|input|
|pOutput|输出结果|output|

+ 数据类型
	+ ALG_ADPCM_SMPL_S

	|成员|说明|
	|:---|:---|
	|pSmpl|编码后采样地址|
	|u32SmplLen|编码后采样长度|

	+ ALG_ADPCM_STREAM_S

	|成员|说明|
	|:---|:---|
	|pStream|需要译码的比特流地址|
	|u32ByteLen|需要译码的字节数|

## 退出函数
TS_VOID TS_ALG_ADPCM_Exit(TS_VOID **ppHandle);

+ 参数

|参数|说明|属性|
|:---:|:---:|:---:|
|ppHandle|句柄|input|

## 版本获取函数
const TS_CHAR *TS_ALG_ADPCM_GetVersion(TS_VOID);

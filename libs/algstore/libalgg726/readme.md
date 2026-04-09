[TOC]

## G726编解码

### 1.1. 算法类型

音频

### 1.2. 算法描述

G726编解码是把采样数据转为G726码流或是把G726码流转换为采样数据。

### **1.3.** API参考

## 初始化函数
TS_S32 TS_ALG_G726_Init(TS_VOID **ppHandle, ALG_G726_INIT_S *pParam);

+ 参数

|参数|说明|属性|
|:---:|:---:|:---:|
|ppHandle|返回句柄|output|
|pParam|需要用到的参数|input|

+ 数据类型
	+ ALG_G726_INIT_S

	|成员|说明|
	|:---|:---|
	|bps|g726编码后码率，当前支持ALG_G726_16K|


## 运行主处理编码函数
TS_VOID TS_ALG_G726_Encode(TS_VOID *pHandle, ALG_G726_SMPL_S *pInput, ALG_G726_STREAM_S *pOutput);

+ 参数

|参数|说明|属性|
|:---:|:---:|:---:|
|pHandle|句柄|input|
|pInput|需要用到的参数|input|
|pOutput|输出结果|output|

+ 数据类型
	+ ALG_G726_SMPL_S

	|成员|说明|
	|:---|:---|
	|pSmpl|采样地址|
	|u32SmplLen|采样长度|

	+ ALG_G726_STREAM_S

	|成员|说明|
	|:---|:---|
	|pStream|编码后比特流地址|
	|u32ByteLen|编码后输出字节数|

## 运行主处理解码函数
TS_VOID TS_ALG_G726_Decode(TS_VOID *pHandle, ALG_G726_STREAM_S *pInput, ALG_G726_SMPL_S *pOutput);

+ 参数

|参数|说明|属性|
|:---:|:---:|:---:|
|pHandle|句柄|input|
|pInput|需要用到的参数|input|
|pOutput|输出结果|output|

+ 数据类型
	+ ALG_G726_SMPL_S

	|成员|说明|
	|:---|:---|
	|pSmpl|编码后采样地址|
	|u32SmplLen|编码后采样长度|

	+ ALG_G726_STREAM_S

	|成员|说明|
	|:---|:---|
	|pStream|需要译码的比特流地址|
	|u32ByteLen|需要译码的字节数|

## 退出函数
TS_VOID TS_ALG_G726_Exit(TS_VOID **ppHandle);

+ 参数

|参数|说明|属性|
|:---:|:---:|:---:|
|ppHandle|句柄|input|

## 版本获取函数
const TS_CHAR *TS_ALG_G726_GetVersion(TS_VOID);

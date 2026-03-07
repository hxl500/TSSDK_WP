[TOC]

## 重采样

### 1.1. 算法类型

音频

### 1.2. 算法描述

重采样

### 1.3. API参考

## 初始化函数
TS_S32 TS_ALG_Resample_Init(TS_VOID **ppHandle, ALG_RESAMPLE_INIT_PARAM_S *pParam);

+ 参数

|参数|说明|属性|
|:---:|:---:|:---:|
|ppHandle|返回句柄|output|
|pParam|需要用到的参数|input|

+ 数据类型
	+ ALG_RESAMPLE_INIT_PARAM_S

	|成员|说明|
	|:---|:---|
	|chanNum|通道数量|
	|inRate|输入采样率|
	|outRate|输出采样率|
	|quality|重采样质量，0~9，等级越高耗时越长重采样质量越好|


## 运行主处理函数
TS_S32 TS_ALG_Resample_Process(TS_VOID *pHandle, ALG_RESAMPLE_PROC_PARAM_S *pParam, ALG_RESAMPLE_RESULT_PARAM_S *pResult);

+ 参数

|参数|说明|属性|
|:---:|:---:|:---:|
|pHandle|句柄|input|
|pParam|需要用到的参数|input|
|pResult|输出处理音频采样|output|

+ 数据类型
	+ ALG_RESAMPLE_PROC_PARAM_S

	|成员|说明|
	|:---|:---|
	|in|采样地址|
	|inlen|采样长度|
	|inBitWidth|采样位宽|

	+ ALG_RESAMPLE_RESULT_PARAM_S

	|成员|说明|
	|:---|:---|
	|out|输出采样地址|
	|outlen|输出长度地址|

## 退出函数
TS_S32 TS_ALG_Resample_Exit(TS_VOID *pHandle);

+ 参数

|参数|说明|属性|
|:---:|:---:|:---:|
|pHandle|句柄|input|

## 参数设置函数
TS_S32 TS_ALG_Resample_SetParam(TS_VOID* pHandle, ALG_RESAMPLE_SET_PARAM_S *pParam);

+ 参数

|参数|说明|属性|
|:---:|:---:|:---:|
|pHandle|句柄|input|
|pParam|需要设置的参数|input|

+ 数据类型
	+ ALG_RESAMPLE_SET_PARAM_S

	|成员|说明|
	|:---|:---|
	|quality|重采样质量，0~9，等级越高耗时越长重采样质量越好|

## 参数获取函数
TS_S32 TS_ALG_Resample_GetParam(TS_VOID* pHandle, ALG_RESAMPLE_SET_PARAM_S* pParam);

+ 参数

|参数|说明|属性|
|:---:|:---:|:---:|
|pHandle|句柄|input|
|pParam|需要设置的参数|output|

## 版本获取函数
const TS_CHAR *TS_ALG_Resample_GetVersion(TS_VOID);

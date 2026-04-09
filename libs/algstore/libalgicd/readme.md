[TOC]

## 婴儿哭声检测

### 1.1. 算法类型

音频

### 1.2. 算法描述

婴儿哭声检测，支持8K采样率

### **1.3.** API参考

## 初始化函数
TS_S32 TS_ALG_ICD_Init(TS_VOID **ppHandle, ALG_ICD_INIT_S *pParam);

+ 参数

|参数|说明|属性|
|:---:|:---:|:---:|
|ppHandle|返回句柄|output|
|pParam|需要用到的参数|input|

+ 数据类型
	+ ALG_ICD_INIT_S

	|成员|说明|
	|:---|:---|
	|pCfg|模型的cfg文件的地址|
	|pWeight|模型的weight文件的地址|
	|vpSelfBuf|指针数组，用于配置模型所需内存的地址|
	|s16FrameLen|帧长|


## 运行主处理函数
TS_S32 TS_ALG_ICD_Process(TS_VOID *pHandle, ALG_ICD_PROC_S *pInput, ALG_ICD_RSLT_S *pOutput);

+ 参数

|参数|说明|属性|
|:---:|:---:|:---:|
|pHandle|句柄|input|
|pInput|需要用到的参数|input|
|pOutput|输出处理音频采样|output|

+ 数据类型
	+ ALG_ICD_PROC_S

	|成员|说明|
	|:---|:---|
	|pIn|采样地址|
	|u32InLen|采样长度|

	+ ALG_ICD_RSLT_S

	|成员|说明|
	|:---|:---|
	|u8CryFlg|是否检测到哭声|
	|u8AlarmFlg|是否报警|

## 退出函数
TS_S32 TS_ALG_ICD_Exit(TS_VOID *pHandle);

+ 参数

|参数|说明|属性|
|:---:|:---:|:---:|
|pHandle|句柄|input|

## 参数设置函数
TS_S32 TS_ALG_ICD_SetParam(TS_VOID* pHandle, ALG_ICD_PARAM_S *pConfig);

+ 参数

|参数|说明|属性|
|:---:|:---:|:---:|
|pHandle|句柄|input|
|pConfig|需要设置的参数|input|

+ 数据类型
	+ ALG_ICD_PARAM_S

	|成员|说明|
	|:---|:---:|
	|u32SmoothFrameNum|检测窗长，minute = u32SmoothFrameNum\*16*16/1000/60, 默认：38|
	|f32SmoothThreshold|得分阈值, 默认：0.35|
	|u32JumpTrigger|是否启用报警间隔功能，默认：1|
	|u32AlarmInterval|报警间隔时间，minute = u32AlarmInterval\*16*16/1000/60|

## 参数获取函数
TS_S32 TS_ALG_ICD_GetParam(TS_VOID* pHandle, ALG_ICD_PARAM_S* pConfig);

+ 参数

|参数|说明|属性|
|:---:|:---:|:---:|
|pHandle|句柄|input|
|pConfig|需要设置的参数|output|

## 版本获取函数
const TS_CHAR *TS_ALG_ICD_GetVersion(TS_VOID);

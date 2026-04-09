[TOC]

## NN语音质量增强解决方案

### 1.1. 算法类型

音频

### 1.2. 算法描述

solvqenn是一套用NN模型对语音质量进行增强的解决方案。

### **1.3.** API参考

## 初始化函数
TS_S32 TS_SOL_VQE_NN_Init(TS_VOID **ppHandle, SOL_VQE_NN_CONF_PARAM *pParam);

+ 参数

|参数|说明|属性|
|:---:|:---:|:---:|
|ppHandle|返回句柄|output|
|pParam|需要用到的参数|input|

+ 数据类型
	+ SOL_VQE_NN_CONF_PARAM

	|成员|说明|
	|:---|:---|
	|u32OpenMask|alk VQE 的各功能使能的 Mask 值。可取值：SOL_ANR_MASK：表示VQE处理中，使能ANR功能。SOL_AEC_ANR_MASK：表示VQE处理中，使能AEC和ANR功能。SOL_AGC_MASK：表示VQE处理中，使能AGC功能。|
	|u32SmplRate|采样率。支持16000/8000|
	|u16FrameLen|帧长。当采样率为16kHz时，请配置为256；当采样率为8kHz，请配置为128。|
	|u8ChanNum|通道数量。取值范围：1/2（支持单声道和双声道）|
	|solVqeInit|初始化VQE用到的相关参数，具体见SOL_VQE_NN_INIT_S|
	|solVqeProc|VQE运行过程中用到的相关参数，具体见SOL_VQE_NN_PROC_S|
	|solVqeSetParam|设置VQE相关参数，具体见SOL_VQE_NN_SET_PARAM_S。如使用默认参数，请将该值赋为NULL|

	+ SOL_VQE_NN_INIT_S

	|成员|说明|
	|:---|:---|
	|algAnrAecInit|参见ALG_ANR_AEC_INIT_S|
	|algAgcInit|参见ALG_WEBRTC_AGC_INIT_S|

	+ SOL_VQE_NN_PROC_S

	|成员|说明|
	|:---|:---|
	|algAnrAecProcParam|参见ALG_ANR_AEC_PORC_S|
	|algAgcProcParam|参见ALG_WEBRTC_AGC_PROC_S|

	+ SOL_VQE_NN_SET_PARAM_S

	|成员|说明|
	|:---|:---|
	|algAnrAecSetParam|参见ALG_ANR_AEC_PARAM_S|


## 运行主处理函数
TS_S32 TS_SOL_VQE_NN_Process(TS_VOID* pHandle, TS_S16 *ps16InBuf, TS_S16 *ps16RefBuf, TS_U32 *pu32InSmplLen, TS_S16 *ps16OutBuf, TS_U32 *u32OutSmplLen);

+ 参数

|参数|说明|属性|
|:---:|:---:|:---:|
|pHandle|句柄|input|
|ps16InBuf|mic采集到的采样地址|input|
|ps16RefBuf|ref信号采样地址|output|
|pu32InSmplLen|输入采样长度|input|
|ps16OutBuf|处理后输出采样|output|
|u32OutSmplLen|输出采样长度|output|


## 退出函数
TS_S32 TS_SOL_VQE_NN_Exit(TS_VOID *pHandle);

+ 参数

|参数|说明|属性|
|:---:|:---:|:---:|
|pHandle|句柄|input|

## 参数获取函数
TS_S32 TS_SOL_VQE_NN_GetParam(TS_VOID* pHandle, SOL_VQE_NN_SET_PARAM_S* pConfig);

+ 参数

|参数|说明|属性|
|:---:|:---:|:---:|
|pHandle|句柄|input|
|pConfig|需要设置的参数|output|

## 版本获取函数
const TS_CHAR *TS_SOL_VQE_NN_GetVersion(TS_VOID);

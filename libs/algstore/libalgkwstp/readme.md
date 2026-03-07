[TOC]

## 关键字识别

### 1.1. 算法类型

音频

### 1.2. 算法描述

关键字识别

### 1.3. API参考

#### 初始化函数
TS_S32 TS_ALG_KwsTp_Init(TS_VOID **ppHandle, ALG_KWSTP_INIT_S *pParam);

+ 参数

|参数|说明|属性|
|:---:|:---:|:---:|
|ppHandle|返回句柄|output|
|pParam|需要用到的参数|input|

+ 数据类型
	+ ALG_KWSTP_INIT_S

	|成员|说明|
	|:---|:---|
	|pCfg|conv模型的cfg文件的地址|
	|pWeight|conv模型的weight文件的地址|
	|vpSelfBuf|用于配置conv模型所需内存的地址|
	|s32CfgLen|conv模型的cfg有多少byte|
	|pFbankCfg|fbank模型的cfg文件的地址|
	|pFbankWeight|fbank模型的weight文件的地址|
	|pFbankSelfBuf|用于配置bank模型所需内存的地址|
	|s32FbankCfgLen|fbank模型的cfg有多少byte|
	|s32SmplRatio|采样率|
	|s16FrameLen|帧长|


#### 运行主处理函数
TS_S32 TS_ALG_KwsTp_Process(TS_VOID *pHandle, ALG_KWSTP_PROC_S *pInput, ALG_KWSTP_RSLT_S *pOutput);

+ 参数

|参数|说明|属性|
|:---:|:---:|:---:|
|pHandle|句柄|input|
|pInput|需要用到的参数|input|
|pOutput|输出处理音频采样|output|

+ 数据类型
	+ ALG_KWSTP_PROC_S

	|成员|说明|
	|:---|:---|
	|pIn|采样地址|
	|u32InLen|采样长度|

	+ ALG_KWSTP_RSLT_S

	|成员|说明|
	|:---|:---|
	|s32Result|检测结果|
	|fscore|检测结果对应得分|

#### 退出函数
TS_S32 TS_ALG_KwsTp_Exit(TS_VOID *pHandle);

+ 参数

|参数|说明|属性|
|:---:|:---:|:---:|
|pHandle|句柄|input|

#### 参数设置函数
TS_S32 TS_ALG_KwsTp_SetParam(TS_VOID* pHandle, ALG_KWSTP_SET_PARAM_S *pConfig);

+ 参数

|参数|说明|属性|
|:---:|:---:|:---:|
|pHandle|句柄|input|
|pConfig|需要设置的参数|input|

+ 数据类型
	+ ALG_KWSTP_SET_PARAM_S

	|成员|说明|
	|:---|:---:|
	|s32IsRneNotOff|1:退出时不调用TS_MPI_TRP_RNE_CloseDevice；others:调用；default:调用TS_MPI_TRP_RNE_CloseDevice|

#### 参数获取函数
TS_S32 TS_ALG_KwsTp_GetParam(TS_VOID* pHandle, ALG_KWSTP_SET_PARAM_S* pConfig);

+ 参数

|参数|说明|属性|
|:---:|:---:|:---:|
|pHandle|句柄|input|
|pConfig|需要设置的参数|output|

#### 版本获取函数
const TS_CHAR *TS_ALG_KwsTp_GetVersion(TS_VOID);

[TOC]

## NN回声消除+降噪


### 1.1. 算法类型

音频

### 1.2. 算法描述

anraec是用NN模型对回声进行消除和降噪。

### **1.3.** API参考

## 初始化函数
TS_S32 TS_ALG_ANR_AEC_Init(TS_VOID **ppHandle, ALG_ANR_AEC_INIT_S *pParam);

+ 参数

|参数|说明|属性|
|:---:|:---:|:---:|
|ppHandle|返回句柄|output|
|pParam|需要用到的参数|input|

+ 数据类型
	+ ALG_ANR_AEC_INIT_S

	|成员|说明|
	|:---|:---|
	|pGraph|指针数组，用于配置ANR和AEC运行时需要用到的两个模型的config文件的地址。第一个指针，配置全带模型的config地址；第二个指针，配置子带模型的config地址。|
	|pWeight|指针数组，用于配置ANR和AEC运行时需要用到的两个模型的weight文件的地址。第一个指针，配置全带模型的weight地址；第二个指针，配置子带模型的weight地址。|
	|u32GraphSize|用于配置ANR和AEC运行时需要用到的两个模型的config文件的大小。第一个参数，配置全带模型的config大小；第二个指针，配置子带模型的config大小。|
	|u32WeightSize|用于配置ANR和AEC运行时需要用到的两个模型的weight文件的大小。第一个指针，配置全带模型的weight大小；第二个指针，配置子带模型的weight大小。|
	|vpSelfBuf|指针数组，用于配置ANR和AEC用到的两个模型所需内存的地址。第一个指针，配置全带模型所需内存的地址；第二个指针，配置子带模型所需内存的地址。|
	|bRneOff|RNE是否已经开启，开启：1；未开启：0。|
	|s32SmplRatio|采样率。支持16000/8000|
	|s16FrameLen|帧长。当采样率为16kHz时，请配置为256；当采样率为8kHz，请配置为128。
	|s16ChanNum|通道数量。取值范围：1/2（支持单声道和双声道）。|
	|aecAlignMode|mic信号和ref信号延时对齐模式，详见AEC_SKEW_MODE|

	+ AEC_SKEW_MODE

	|成员|说明|
	|:---|:---|
	|SKEW_ALIGN|实时计算mic信号和ref信号的相对延时，并进行调整，适用于mic信号和ref信号会随时间变化的场景。|
	|SKEW_FIX|提前配置mic信号和ref信号的相对延时，并进行调整，适用于mic信号和ref信号固定不变的场景。|
	|SKEW_NONE|不对mic信号和ref信号的相对延时进行计算和调整，适用于mic信号和ref信号延时固定不变且较小的场景。|


## 运行主处理函数
TS_S32 TS_ALG_ANR_AEC_Process(TS_VOID *pHandle, ALG_ANR_AEC_PORC_S *pInput, ALG_ANR_AEC_RSLT_S *pOutput);

+参数

|参数|说明|属性|
|:---:|:---:|:---:|
|pHandle|句柄|input|
|pInput|需要用到的参数|input|
|pOutput|输出处理音频采样|output|

+ 数据类型
	+ ALG_ANR_AEC_PORC_S

	|成员|说明|
	|:---|:---|
	|pMicData|mic采样地址|
	|u32MicDataLen|Mic采集到信号的长度。当采样率为16kHz时，请配置为256；当采样率为8kHz，请配置为128。|
	|pRefData|ref信号地址|
	|u32RefDataLen|Ref信号的地址长度。当采样率为16kHz时，请配置为256；当采样率为8kHz，请配置为128。|
	|s16MsInSndCardBuf|Mic信号和ref信号的固定时延，单位ms，取值范围：[0, 500]。当ALG_ANR_AEC_INIT_S 结构体的aecAlignMode配置为SKEW_FIX时，该参数所配值才有意义。|

	+ ALG_ANR_AEC_RSLT_S

	|成员|说明|
	|:---|:---|
	|pOutData|处理后的音频采样|
	|u32OutLen|输出采样长度|
	|s32RefDelayBlock|当ALG_ANR_AEC_INIT_S 结构体的aecAlignMode配置为SKEW_ALIGN时该参数有意义，表示延时的帧数|

## 退出函数
TS_S32 TS_ALG_ANR_AEC_Exit(TS_VOID *pHandle);

+ 参数

|参数|说明|属性|
|:---:|:---:|:---:|
|pHandle|句柄|input|

## 参数设置函数
TS_S32 TS_ALG_ANR_AEC_SetParam(TS_VOID* pHandle, ALG_ANR_AEC_PARAM_S *pConfig);

+ 参数

|参数|说明|属性|
|:---:|:---:|:---:|
|pHandle|句柄|input|
|pConfig|需要设置的参数|input|

+ 数据类型
	+ ALG_ANR_AEC_PARAM_S

	|成员|说明|
	|:---|:---|
	|ps16HistNum|计算实时延时所用的帧数。当aecAlignMode配置为SKEW_ALIGN是有效。默认：50。|
	|f32MaxCohxThreshold|判断是否进行延时调整的门限。当aecAlignMode配置为SKEW_ALIGN是有效。默认：0.2。|
	|laecMode|线性AEC的模式|
	|stereoAnrMode|双麦降噪所使用的模式。可选值：0：使用NN模型进行双麦降噪，需要考虑模型适用的场景。1：使用MVDR算法进行双麦降噪，无需考虑两个麦克风之间的距离和声源方向。默认：1，即MVDR模式|

	+ AEC_LAEC_MODE

	|成员|说明|
	|:---|:---|
	|LAEC_FAST|线性回声消除效果中等，资源占用较少|
	|LAEC_PBNLMS|线性回声消除效果更优，资源占用较多|
	|LAEC_DISABLE|不使用LAEC|

## 参数获取函数
TS_S32 TS_ALG_ANR_AEC_SetParam(TS_VOID* pHandle, ALG_ANR_AEC_PARAM_S *pConfig);

+ 参数

|参数|说明|属性|
|:---:|:---:|:---:|
|pHandle|句柄|input|
|pConfig|需要设置的参数|output|

## 版本获取函数
const TS_CHAR *TS_ALG_ANR_AEC_GetVersion(TS_VOID);

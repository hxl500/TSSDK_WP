[TOC]

## webrtc3a语音质量增强解决方案

### 1.1. 算法类型

音频

### 1.2. 算法描述

vqesolwebrtc3a是一套基于webrtc3a对语音质量进行增强的解决方案。

### **1.3.** API参考 

## 初始化函数
TS_S32 TS_ALG_VQESolWebRTC3A_Init(TS_VOID **ppHandle, ALG_VQE_SOL_WEBRTC3A_CONF_PARAM *pParam);

+ 参数

|参数|说明|属性|
|:---:|:---:|:---:|
|ppHandle|返回句柄|output|
|pParam|需要用到的参数|input|

+ 数据类型
	+ ALG_VQE_SOL_WEBRTC3A_CONF_PARAM

	|成员|说明|
	|:---|:---|
	|u32OpenMask|alk VQE 的各功能使能的 Mask 值。可取值：ALG_WEBRTC_AEC_INIT_S：表示使能AEC功能。ALG_WEBRTC_NSX_INIT_S：表示使能ANR功能。ALG_WEBRTC_AGC_INIT_S：表示使能AGC功能。|
	|u32SmplRate|采样率。支持16000/8000|
	|u16FrameLen|VQE 的帧长，即采样点数目。当采样率为16kHz时，请配置为160；当采样率为8kHz，请配置为80。|
	|u8ChanNum|通道数量。取值范围：1（仅支持单声道）。.|
	|s16AgcMode|AGC的处理模式，可取值：0（WEBRTC_AGC_MODE_UNCHANGED）：不做AGC模式；1（AUDIO_AGC_MODE_ADAPTIVE_ANALOG）：采用自适应模拟AGC模式；2（AUDIO_AGC_MODE_ADAPTIVE_DIGITAL）：采用自适应数字AGC模式；3（AUDIO_AGC_MODE_FIXED_DIGITAL）：采用固定数字增益模式|
	|solVqeProc|VQE运行过程中用到的相关参数，具体见ALG_VQE_SOL_WEBRTC3A_PROC_S|
	|solVqeSetParam|设置VQE相关参数，具体见ALG_VQE_SOL_WEBRTC3A_SET_PARAM_S。如使用默认参数，请将该值赋为NULL|

	+ ALG_VQE_SOL_WEBRTC3A_PROC_S

	|成员|说明|
	|:---|:---|
	|s16MsInSndCardBuf|Mic信号和ref信号的时延，单位ms，取值范围：[0, 500]|
	|s32Skew|表示ref信号和mic信号每帧的采样数存在偏差。当ALG_WEBRTC_AEC_PARAM_S结构体中的s16SkewMode使能时，需要用到此参数。单位：采样数。|

	+ ALG_VQE_SOL_WEBRTC3A_SET_PARAM_S

	|成员|说明|
	|:---|:---|
	|algAecParams|参见ALG_WEBRTC_AEC_PARAM_S|
	|algAnrParams|参见ALG_WEBRTC_NSX_PARAM_S|
	|algAgcParams|参见ALG_WEBRTC_AGC_PARAM_S|


## 运行主处理函数
TS_S32 TS_ALG_VQESolWebRTC3A_Process(TS_VOID* pHandle, TS_S16 *ps16InBuf, TS_S16 *ps16RefBuf, TS_U32 *pu32InSmplLen, TS_S16 *ps16OutBuf, TS_U32 *u32OutSmplLen);

+ 参数

|参数|说明|属性|
|:---:|:---:|:---:|
|pHandle|句柄|input|
|ps16InBuf|mic采集到的采样地址|input|
|ps16RefBuf|ref信号采样地址|output|
|pu32InSmplLen|输入采样长度|input|
|ps16OutBuf|处理后输出采样|output|
|u32OutSmplLen|输出采样长度|output|


##退出函数
TS_S32 TS_ALG_VQESolWebRTC3A_Exit(TS_VOID *pHandle);

+ 参数

|参数|说明|属性|
|:---:|:---:|:---:|
|pHandle|句柄|input|

## 参数获取函数
TS_S32 TS_ALG_VQESolWebRTC3A_GetParam(TS_VOID* pHandle, ALG_VQE_SOL_WEBRTC3A_SET_PARAM_S* pConfig);

+ 参数

|参数|说明|属性|
|:---:|:---:|:---:|
|pHandle|句柄|input|
|pConfig|需要设置的参数|output|

##版本获取函数
const TS_CHAR *TS_ALG_VQESolWebRTC3A_GetVersion(TS_VOID);

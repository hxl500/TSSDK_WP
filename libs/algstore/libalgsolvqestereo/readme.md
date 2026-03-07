[TOC]

## 多通道语音质量增强解决方案

### 1.1. 算法类型

音频

### 1.2. 算法描述

多通道语音质量增强解决方案。

### 1.3. API参考 

## 初始化函数
TS_S32 TS_ALG_SolVqeStereo_Init(TS_VOID **ppHandle, ALG_SOL_VQE_STEREO_CONF_PARAM *pParam);

+ 参数

|参数|说明|属性|
|:---:|:---:|:---:|
|ppHandle|返回句柄|output|
|pParam|需要用到的参数|input|

+ 数据类型
	+ ALG_SOL_VQE_STEREO_CONF_PARAM

	|成员|说明|
	|:---|:---|
	|u32OpenMask|alk VQE 的各功能使能的 Mask 值。可取值：ALG_VQE_SOL_ADAPT_AEC_MASK：表示使能AEC功能。ALG_VQE_SOL_WEBRTC_ANR_MASK：表示使能ANR功能。ALG_VQE_SOL_WEBRTC_AGC_MASK：表示使能AGC功能。ALG_VQE_MMSENS_MASK：表示使能MMSENS降噪功能。ALG_VQE_OMLSA_MASK：表示使能OMLSA降噪功能。ALG_VQE_NOISEGATE_MASK：表示使能noisegate功能|
	|u32SmplRate|采样率。支持16000/8000|
	|u16FrameLen|VQE 的帧长，即采样点数目。当采样率为16kHz时，请配置为256；当采样率为8kHz，请配置为128。|
	|u8MicChanNum|mic通道数量。取值范围：大于或等于1。|
	|u8RefChanNum|ref通道数量。取值范围：1。|
	|s16AgcMode|AGC的处理模式，可取值：0（WEBRTC_AGC_MODE_UNCHANGED）：不做AGC模式；1（AUDIO_AGC_MODE_ADAPTIVE_ANALOG）：采用自适应模拟AGC模式；2（AUDIO_AGC_MODE_ADAPTIVE_DIGITAL）：采用自适应数字AGC模式；3（AUDIO_AGC_MODE_FIXED_DIGITAL）：采用固定数字增益模式|
	|s32NoiseSuppress|参加libalgmmsens相同参数|
	|fAttackTime|参加libalgnoisegate相同参数|
	|fReleaseTime|参加libalgnoisegate相同参数|
	|s32Threshold|参加libalgnoisegate相同参数|
	|eNlpMode|参加libalgadaptaec相同参数|
	|solVqeProc|ALG_SOL_VQE_STEREO_PROC_S|
	|solVqeSetParam|设置VQE相关参数，具体见ALG_SOL_VQE_STEREO_PARAM_S。如使用默认参数，请将该值赋为NULL|

	+ ALG_SOL_VQE_STEREO_PROC_S

	|成员|说明|
	|:---|:---|

	+ ALG_SOL_VQE_STEREO_SET_PARAM_S

	|成员|说明|
	|:---|:---|
	|algAnrParams|参见ALG_WEBRTC_NSX_PARAM_S|
	|algAgcParams|参见ALG_WEBRTC_AGC_PARAM_S|


## 运行主处理函数
TS_S32 TS_ALG_SolVqeStereo_Process(TS_VOID* pHandle, TS_S16 *ps16InBuf, TS_S16 *ps16RefBuf, TS_U32 *pu32InSmplLen, TS_S16 *ps16OutBuf, TS_U32 *u32OutSmplLen);

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
TS_S32 TS_ALG_SolVqeStereo_Exit(TS_VOID *pHandle);

+ 参数

|参数|说明|属性|
|:---:|:---:|:---:|
|pHandle|句柄|input|

## 参数获取函数
TS_S32 TS_ALG_SolVqeStereo_GetParam(TS_VOID* pHandle, ALG_SOL_VQE_STEREO_PARAM_S* pConfig);

+ 参数

|参数|说明|属性|
|:---:|:---:|:---:|
|pHandle|句柄|input|
|pConfig|需要设置的参数|output|

##版本获取函数
const TS_CHAR *TS_ALG_SolVqeStereo_GetVersion(TS_VOID);

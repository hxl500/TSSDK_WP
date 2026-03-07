[TOC]

# 3A算法

3A算法是由回声消除、语音降噪、自动增益控制三个算法组合而成。

## 1. 回声消除

### 1.1. 算法类型

音频

### 1.2. 算法描述

回声消除是指消除麦克风采集到喇叭发出的声音(回声)。

### 1.3. API参考

- `TS_ALG_WebRTCAEC_Init`：初始化回声消除算法。
- `TS_ALG_WebRTCAEC_BufferFarend`：存储喇叭发出声音的数据。
- `TS_ALG_WebRTCAEC_Process`：执行回声消除算法。
- `TS_ALG_WebRTCAEC_Exit`：释放回声消除算法资源。
- `TS_ALG_WebRTCAEC_SetParam`：设置回声消除算法参数。
- `TS_ALG_WebRTCAEC_GetParam`：获取回声消除算法法参数。
- `TS_ALG_WebRTCAEC_GetVersion`：获取回声消除算法版本信息。

#### `TS_ALG_WebRTCAEC_Init`

**[描述]**

初始化回声消除算法。

**[语法]**

`TS_S32 TS_ALG_WebRTCAEC_Init(TS_VOID **ppHandle, ALG_WebRTC_AEC_INIT_S *pParam);`

**[参数]**

| 参数名称 |             描述              | 输入/输出 |
| :------: | :---------------------------: | :-------: |
| ppHandle | 用以返回算法的 HANDLE指针地址 |   输出    |
|  pParam  |       算法的初始化信息        |   输入    |

**[返回值]**

| 返回值 | 描述 |
| :----: | :--: |
|   0    | 成功 |
|  非0   | 失败 |



#### `TS_ALG_WebRTCAEC_BufferFarend`

**[描述]**

存储喇叭发出声音的数据。

**[语法]**

`TS_S32 TS_ALG_WebRTCAEC_BufferFarend(TS_VOID *pHandle, ALG_WEBRTC_AEC_FAREND_PARAM_S *pFarParam);`

**[参数]**

| 参数名称  |    描述    | 输入/输出 |
| :-------: | :--------: | :-------: |
|  pHandle  | HANDLE信息 |   输入    |
| pFarParam |  输入参数  |   输入    |

**[返回值]**

| 返回值 | 描述 |
| :----: | :--: |
|   0    | 成功 |
|  非0   | 失败 |



#### `TS_ALG_WebRTCAEC_Process`

**[描述]**

执行回声消除算法。

**[语法]**

TS_S32 `TS_ALG_WebRTCAEC_Process`(TS_VOID *pHandle, ALG_WEBRTC_AEC_PROC_S *pParam, ALG_WEBRTC_AEC_RESULT_S* *pRslt);

**[参数]**

| 参数名称 |    描述    | 输入/输出 |
| :------: | :--------: | :-------: |
| pHandle  | HANDLE信息 |   输入    |
|  pParam  |  输入参数  |   输入    |
|  pRslt   |  输出信息  |   输出    |

**[返回值]**

| 返回值 | 描述 |
| :----: | :--: |
|   0    | 成功 |
|  非0   | 失败 |



#### `TS_ALG_WebRTCAEC_Exit`

**[描述]**

释放回声消除算法资源。

**[语法]**

TS_S32 `TS_ALG_WebRTCAEC_Exit`(TS_VOID* pHandle);

**[参数]**

| 参数名称 |    描述    | 输入/输出 |
| :------: | :--------: | :-------: |
| pHandle  | HANDLE信息 |   输入    |

**[返回值]**

| 返回值 | 描述 |
| :----: | :--: |
|   0    | 成功 |
|  非0   | 失败 |



#### `TS_ALG_WebRTCAEC_SetParam`

**[描述]**

配置回声消除算法相关参数。

**[语法]**

TS_S32 `TS_ALG_WebRTCAEC_SetParam`(TS_VOID *pHandle, ALG_WEBRTC_AEC_PARAM_S *pParam)`;

**[参数]**

| 参数名称 |    描述    | 输入/输出 |
| :------: | :--------: | :-------: |
| pHandle  | HANDLE信息 |   输入    |
|  pParam  |  参数信息  |   输入    |

**[返回值]**

| 返回值 | 描述 |
| :----: | :--: |
|   0    | 成功 |
|  非0   | 失败 |



#### `TS_ALG_WebRTCAEC_GetParam`

**[描述]**

获取回声消除算法相关参数。

**[语法]**

TS_S32 `TS_ALG_WebRTCAEC_GetParam`(TS_VOID *pHandle, ALG_WEBRTC_AEC_PARAM_S *pParam);

**[参数]**

| 参数名称 |    描述    | 输入/输出 |
| :------: | :--------: | :-------: |
| pHandle  | HANDLE信息 |   输入    |
|  pParam  |  参数信息  |   输出    |

**[返回值]**

| 返回值 | 描述 |
| :----: | :--: |
|   0    | 成功 |
|  非0   | 失败 |



#### `TS_ALG_WebRTCAEC_GetVersion`

**[描述]**

获取回声消除算法版本信息。

**[语法]**

const `TS_CHAR`* `TS_ALG_WebRTCAEC_GetVersion`(TS_VOID);

**[返回值]**

| 返回值  |        描述        |
| :-----: | :----------------: |
| Version | 当前算法的版本信息 |



### **1.4.** 数据类型

- ALG_WEBRTC_AEC_INIT_S：定义回声消除初始化用到的参数信息。
- ALG_WEBRTC_AEC_PROC_S：定义回声消除运行时用到的参数信息。
- ALG_WEBRTC_AEC_FAREND_PARAM_S：定义喇叭的参数信息。
- ALG_WEBRTC_AEC_PARAM_S：定义回声消除配置参数信息。
- ALG_WEBRTC_AEC_RESULT_S: 定义回声消除算法结果信息。

#### ALG_WEBRTC_AEC_INIT_S

**[说明]**

定义回声消除初始化用到的参数信息。

**[定义]**

`typedef struct tsALG_WEBRTC_AEC_INIT_S{`

​	`TS_S32 s32SampFreq;`

​	`TS_S32 s32ScSampFreq;`

`}ALG_WEBRTC_AEC_INIT_S;`

**[成员]**

|   成员名称    |                 描述                 |
| :-----------: | :----------------------------------: |
|  s32SampFreq  | 麦克风采样率，支持：8000/16000/32000 |
| s32ScSampFreq |   声卡采样率，取值范围：[1, 96000]   |



#### ALG_WEBRTC_AEC_PROC_S

**[说明]**

定义回声消除运行时用到的参数信息。

**[定义]**

`typedef struct tsALG_WEBRTC_AEC_PROC_S{`

​	`const TS_S16* s16NearEnd;`

​	`const TS_S16* s16NearEndH;`

​	`const TS_S16* s16FarEnd;`

​	`TS_S16 s16NRofSamples;`

​	`TS_S16 s16MsInSndCardBuf;`

​	`TS_S32 s32Skew;`

`}ALG_WEBRTC_AEC_PROC_S;`

**[成员]**

|     成员名称      |                             描述                             |
| :---------------: | :----------------------------------------------------------: |
|    s16NearEnd     | 麦克风采集到数据地址，当采样率为32K时，表示麦克风采集到低段数据 |
|    s16NearEndH    | 采样率等于32K时需要配置，表示麦克风采集到高段数据，其他情况可设置为NULL |
|     s16FarEnd     |                   喇叭数据的地址，暂未使用                   |
|  s16NRofSamples   |                         输入采样长度                         |
| s16MsInSndCardBuf | 声卡接收和系统缓冲区的延时估计，单位ms，取值范围：[0, 500]。 |
|      s32Skew      | 播放和录制声音时声卡每帧的样本数量之间的差异。当ALG_AEC_PARAM_S结构体中的s16SkewMode使能时，会用到此参数。单位：采样数 |



#### ALG_WEBRTC_AEC_FAREND_PARAM_S

**[说明]**

定义喇叭的参数信息。

**[定义]**

`typedef struct tsALG_WEBRTC_AEC_FAREND_PARAM_S{`

​	`TS_S16 *ps16FarEnd;`

​	`TS_S16  s16NRofSamples;`

`}ALG_WEBRTC_AEC_FAREND_PARAM_S;`

**[成员]**

|    成员名称    |        描述        |
| :------------: | :----------------: |
|   ps16FarEnd   |   喇叭数据的地址   |
| s16NRofSamples | 喇叭传入采样的数量 |



#### ALG_WEBRTC_AEC_PARAM_S

**[说明]**

定义回声消除配置参数信息。

**[定义]**

`typedef struct tsALG_WEBRTC_AEC_PARAM_S{`

​	`TS_S16 s16NlpMode;`

​	`TS_S16 s16SkewMode;`

​	`TS_S16 s16MetricsMode;`

​	`TS_S32 s32DelayLogging;`

`} ALG_WEBRTC_AEC_PARAM_S;`

**[成员]**

|    成员名称     |                        描述                        |
| :-------------: | :------------------------------------------------: |
|   s16NlpMode    | 非线性处理器模式，默认：AUDIO_AEC_NLP_CONSERVATIVE |
|   s16SkewMode   |          时钟偏差校准模式，默认：TS_FALSE          |
| s16MetricsMode  |          衡量AEC效果模式，默认：TS_FALSE           |
| s32DelayLogging |          延时信息记录模式，默认：TS_FALSE          |

s16NlpMode可选模式:

|       模式名称       |   描述    |
| :------------------: | :-------: |
| AEC_NLP_CONSERVATIVE | 保守型NLP |
|   AEC_NLP_MODERATE   | 中等型NLP |
|  AEC_NLP_AGGRESSIVE  | 激进型NLP |



#### ALG_WEBRTC_AEC_RESULT_S

**[说明]**

定义回声消除算法结果信息。

**[定义]**

`typedef struct tsALG_WEBRTC_AEC_RESULT_S{`

​	`TS_S16* ps16Out;`

​	`TS_S16* ps16OutH;`

`}ALG_WEBRTC_AEC_RESULT_S;`

**[成员]**

| 成员名称 |                             描述                             |
| :------: | :----------------------------------------------------------: |
| ps16Out  | 完成回声消除的采样地址，当采样率为32K时，表示完成处理后的低段数据 |
| ps16OutH | 当采样率等于32时需要配置，表示完成处理后高段数据，其他情况未使用 |



### **1.5.** 需求

#### 头文件

`#include<ts_alg_webrtc_aec.h>`

#### 库文件

- `libalgcommon.a`

- `libalgwebrtc3a.a`

  

### **1.6.** 模型说明

无



### **1.7.** EXAMPLE

**[示例目录]**

example/alg_3a_main.c



## 2. 语音降噪

### 2.1. 算法类型

音频

### 2.2. 算法描述

语音降噪是指对语音信号内的噪声进行消减。

### **2.3.** API参考

- `TS_ALG_WebRTCNSX_Init`：初始化语音降噪算法。
- `TS_ALG_WebRTCNSX_Process`：执行语音降噪算法。
- `TS_ALG_WebRTCNSX_Exit`：释放语音降噪算法资源。
- `TS_ALG_WebRTCNSX_SetParam`：设置语音降噪算法参数。
- `TS_ALG_WebRTCNSX_GetParam`：获取语音降噪算法法参数。
- `TS_ALG_WebRTCNSX_GetVersion`：获取语音降噪算法版本信息。

#### `TS_ALG_WebRTCNSX_Init`

**[描述]**

初始化语音降噪算法。

**[语法]**

`TS_S32 TS_ALG_WebRTCNSX_Init(TS_VOID **ppHandle, ALG_WebRTC_NSX_INIT_S *pParam);`

**[参数]**

| 参数名称 |             描述              | 输入/输出 |
| :------: | :---------------------------: | :-------: |
| ppHandle | 用以返回算法的 HANDLE指针地址 |   输出    |
|  pParam  |       算法的初始化信息        |   输入    |

**[返回值]**

| 返回值 | 描述 |
| :----: | :--: |
|   0    | 成功 |
|  非0   | 失败 |



#### `TS_ALG_WebRTCNSX_Process`

**[描述]**

执行语音降噪算法。

**[语法]**

TS_S32 `TS_ALG_WebRTCNSX_Process`(TS_VOID *pHandle, ALG_WEBRTC_NSX_PROC_S *pParam, ALG_WEBRTC_NSX_RESULT_S* *pRslt);

**[参数]**

| 参数名称 |    描述    | 输入/输出 |
| :------: | :--------: | :-------: |
| pHandle  | HANDLE信息 |   输入    |
|  pParam  |  输入参数  |   输入    |
|  pRslt   |  输出信息  |   输出    |

**[返回值]**

| 返回值 | 描述 |
| :----: | :--: |
|   0    | 成功 |
|  非0   | 失败 |



#### `TS_ALG_WebRTCNSX_Exit`

**[描述]**

释放语音降噪算法资源。

**[语法]**

TS_S32 `TS_ALG_WebRTCNSX_Exit`(TS_VOID* pHandle);

**[参数]**

| 参数名称 |    描述    | 输入/输出 |
| :------: | :--------: | :-------: |
| pHandle  | HANDLE信息 |   输入    |

**[返回值]**

| 返回值 | 描述 |
| :----: | :--: |
|   0    | 成功 |
|  非0   | 失败 |



#### `TS_ALG_WebRTCNSX_SetParam`

**[描述]**

配置语音降噪算法相关参数。

**[语法]**

TS_S32 `TS_ALG_WebRTCNSX_SetParam`(TS_VOID *pHandle, ALG_WEBRTC_NSX_PARAM_S *pParam)`;

**[参数]**

| 参数名称 |    描述    | 输入/输出 |
| :------: | :--------: | :-------: |
| pHandle  | HANDLE信息 |   输入    |
|  pParam  |  参数信息  |   输入    |

**[返回值]**

| 返回值 | 描述 |
| :----: | :--: |
|   0    | 成功 |
|  非0   | 失败 |



#### `TS_ALG_WebRTCNSX_GetParam`

**[描述]**

获取语音降噪算法相关参数。

**[语法]**

TS_S32 `TS_ALG_WebRTCNSX_GetParam`(TS_VOID *pHandle, ALG_WEBRTC_AEC_PARAM_S *pParam);

**[参数]**

| 参数名称 |    描述    | 输入/输出 |
| :------: | :--------: | :-------: |
| pHandle  | HANDLE信息 |   输入    |
|  pParam  |  参数信息  |   输出    |

**[返回值]**

| 返回值 | 描述 |
| :----: | :--: |
|   0    | 成功 |
|  非0   | 失败 |



#### `TS_ALG_WebRTCNSX_GetVersion`

**[描述]**

获取语音降噪算法版本信息。

**[语法]**

const `TS_CHAR`* `TS_ALG_WebRTCNSX_GetVersion`(TS_VOID);

**[返回值]**

| 返回值  |        描述        |
| :-----: | :----------------: |
| Version | 当前算法的版本信息 |



### **2.4.** 数据类型

- ALG_WEBRTC_NSX_INIT_S：定义语音降噪初始化用到的参数信息。
- ALG_WEBRTC_NSX_PROC_S：定义语音降噪运行时用到的参数信息。
- ALG_WEBRTC_NSX_PARAM_S：定义语音降噪配置参数信息。
- ALG_WEBRTC_NSX_RESULT_S: 定义语音降噪算法结果信息。

#### ALG_WEBRTC_NSX_INIT_S

**[说明]**

定义语音降噪初始化用到的参数信息。

**[定义]**

`typedef struct tsALG_WEBRTC_NSX_INIT_S{`

​	`TS_U32 u32Fs;`

`}ALG_WEBRTC_NSX_INIT_S;`

**[成员]**

| 成员名称 |              描述              |
| :------: | :----------------------------: |
|  u32Fs   | 采样率，支持：8000/16000/32000 |



#### ALG_WEBRTC_NSX_PROC_S

**[说明]**

定义语音消除运行时用到的参数信息。

**[定义]**

`typedef struct tsALG_WEBRTC_NSX_RROC_S{`

​	`TS_S16* ps16SpeechFrame;`

​	`TS_S16* ps16SpeechFrameHB;`

`}ALG_WEBRTC_NSX_RROC_S;`

**[成员]**

|     成员名称      |                             描述                             |
| :---------------: | :----------------------------------------------------------: |
|  ps16SpeechFrame  |      语音数据地址，当采样率为32K时，表示语音的低段数据       |
| ps16SpeechFrameHB | 采样率等于32K时需要配置，表示语音的高段数据，其他情况可置位NULL |



#### ALG_WEBRTC_NSX_PARAM_S

**[说明]**

定义语音消除配置参数信息。

**[定义]**

`typedef struct tsALG_WEBRTC_NSX_PARAM_S{`

​	`TS_S32  s32Mode;`

`}ALG_WEBRTC_NSX_PARAM_S;`

**[成员]**

| 成员名称 |                             描述                             |
| :------: | :----------------------------------------------------------: |
| s32Mode  | 语音降噪模式化：0：保守模式；1：中等模式；2：激进模式；3：剧烈模式 |



#### ALG_WEBRTC_NSX_RESULT_S

**[说明]**

定义语音降噪算法结果信息。

**[定义]**

typedef struct tsALG_WEBRTC_NSX_RESULT_S{

​	TS_S16* ps16OutFrame;

​	TS_S16* ps16OutFrameHB;

}ALG_WEBRTC_NSX_RESULT_S;

**[成员]**

|    成员名称    |                             描述                             |
| :------------: | :----------------------------------------------------------: |
|  ps16OutFrame  | 完成语音降噪的采样地址，当采样率为32K时，表示完成处理后的低段数据 |
| ps16OutFrameHB | 当采样率等于32时需要配置，表示完成处理后高段数据，其他情况未使用 |



### **2.5.** 需求

#### 头文件

`#include<ts_alg_webrtc_ns.h>`

#### 库文件

- `libalgcommon.a`

- `libalgwebrtc3a.a`

  

### **2.6.** 模型说明

无



### **2.7.** EXAMPLE

**[示例目录]**

example/alg_3a_main.c



## 3. 自动增益控制

### 3.1. 算法类型

音频

### 3.2. 算法描述

自动增益是指对语音信号的增益进行自动调节。

### **3.3.** API参考

- `TS_ALG_WebRTCAGC_Init`：初始化自动增益算法。
- `TS_ALG_WebRTCAGC_Process`：执行自动增益算法。
- `TS_ALG_WebRTCAGC_Exit`：释放自动增益算法资源。
- `TS_ALG_WebRTCAGC_SetParam`：设置自动增益算法参数。
- `TS_ALG_WebRTCAGC_GetParam`：获取自动增益算法法参数。
- `TS_ALG_WebRTCAGC_GetVersion`：获取自动增益算法版本信息。

#### `TS_ALG_WebRTCAGC_Init`

**[描述]**

初始化自动增益算法。

**[语法]**

`TS_S32 TS_ALG_WebRTCAGC_Init(TS_VOID **ppHandle, ALG_WebRTC_AGC_INIT_S *pParam);`

**[参数]**

| 参数名称 |             描述              | 输入/输出 |
| :------: | :---------------------------: | :-------: |
| ppHandle | 用以返回算法的 HANDLE指针地址 |   输出    |
|  pParam  |       算法的初始化信息        |   输入    |

**[返回值]**

| 返回值 | 描述 |
| :----: | :--: |
|   0    | 成功 |
|  非0   | 失败 |



#### `TS_ALG_WebRTCAGC_Process`

**[描述]**

执行自动增益算法。

**[语法]**

TS_S32 `TS_ALG_WebRTCAGC_Process`(TS_VOID *pHandle, ALG_WEBRTC_AGC_PROC_S *pInParam, ALG_WEBRTC_AGC_RESULT_S* *pRslt);

**[参数]**

| 参数名称 |    描述    | 输入/输出 |
| :------: | :--------: | :-------: |
| pHandle  | HANDLE信息 |   输入    |
| pInParam |  输入参数  |   输入    |
|  pRslt   |  输出信息  |   输出    |

**[返回值]**

| 返回值 | 描述 |
| :----: | :--: |
|   0    | 成功 |
|  非0   | 失败 |



#### `TS_ALG_WebRTCAGC_Exit`

**[描述]**

释放自动增益算法资源。

**[语法]**

TS_S32 `TS_ALG_WebRTCAGC_Exit`(TS_VOID* pHandle);

**[参数]**

| 参数名称 |    描述    | 输入/输出 |
| :------: | :--------: | :-------: |
| pHandle  | HANDLE信息 |   输入    |

**[返回值]**

| 返回值 | 描述 |
| :----: | :--: |
|   0    | 成功 |
|  非0   | 失败 |



#### `TS_ALG_WebRTCAGC_SetParam`

**[描述]**

配置自动增益算法相关参数。

**[语法]**

TS_S32 `TS_ALG_WebRTCAGC_SetParam`(TS_VOID *pHandle, ALG_WEBRTC_AGC_PARAM_S *pConfig)`;

**[参数]**

| 参数名称 |    描述    | 输入/输出 |
| :------: | :--------: | :-------: |
| pHandle  | HANDLE信息 |   输入    |
| pConfig  |  参数信息  |   输入    |

**[返回值]**

| 返回值 | 描述 |
| :----: | :--: |
|   0    | 成功 |
|  非0   | 失败 |



#### `TS_ALG_WebRTCAGC_GetParam`

**[描述]**

获取自动增益算法相关参数。

**[语法]**

TS_S32 `TS_ALG_WebRTCAGC_GetParam`(TS_VOID *pHandle, ALG_WEBRTC_AGC_PARAM_S *pConfig);

**[参数]**

| 参数名称 |    描述    | 输入/输出 |
| :------: | :--------: | :-------: |
| pHandle  | HANDLE信息 |   输入    |
| pConfig  |  参数信息  |   输出    |

**[返回值]**

| 返回值 | 描述 |
| :----: | :--: |
|   0    | 成功 |
|  非0   | 失败 |



#### `TS_ALG_WebRTCAGC_GetVersion`

**[描述]**

获取自动增益算法版本信息。

**[语法]**

const `TS_CHAR`* `TS_ALG_WebRTCAGC_GetVersion`(TS_VOID);

**[返回值]**

| 返回值  |        描述        |
| :-----: | :----------------: |
| Version | 当前算法的版本信息 |



### **3.4.** 数据类型

- ALG_WEBRTC_AGC_INIT_S：定义自动增益初始化用到的参数信息。
- ALG_WEBRTC_AGC_PROC_S：定义自动增益运行时用到的参数信息。
- ALG_WEBRTC_AGC_PARAM_S：定义自动增益配置参数信息。
- ALG_WEBRTC_AGC_RESULT_S: 定义自动增益算法结果信息。

#### ALG_WEBRTC_AGC_INIT_S

**[说明]**

定义自动增益初始化用到的参数信息。

**[定义]**

`typedef struct tsALG_WEBRTC_AGC_INIT_S{`

​	`TS_S32 s32MinLevel;`

​	`TS_S32 s32MaxLevel;`

​	`TS_S16 s16AgcMode;`

​	`TS_U32 u32Fs;`

`}ALG_WEBRTC_AGC_INIT_S;`

**[成员]**

|  成员名称   |              描述              |
| :---------: | :----------------------------: |
| s32MinLevel |          mic最小音量           |
| s32MaxLevel |          mic最大音量           |
| s16AgcMode  |        自动增益控制模式        |
|    u32Fs    | 采样率，支持：8000/16000/32000 |

s16AgcMode可选模式：

|             模式名称             |           描述            |
| :------------------------------: | :-----------------------: |
|    WEBRTC_AGC_MODE_UNCHANGED     | AGC算法仅对输入做饱和处理 |
| WEBRTC_AGC_MODE_ADAPTIVE_ANALOG  |      采用模拟AGC模式      |
| WEBRTC_AGC_MODE_ADAPTIVE_DIGITAL |      采用数字AGC模式      |
|  WEBRTC_AGC_MODE_FIXED_DIGITAL   |   采用固定数字增益模式    |



#### ALG_WEBRTC_AGC_PROC_S

**[说明]**

定义自动增益运行时用到的参数信息。

**[定义]**

`typedef struct tsALG_WEBRTC_AGC_PROC_S{`

​	`const TS_S16* ps16InNear;`

​	`const TS_S16* ps16InNearH;`

​	`TS_S16 s16Samples;`

​	`TS_S32 s32InMicLevel;`

​	`TS_S16 s16Echo;`

`}ALG_WEBRTC_AGC_PROC_S;`

**[成员]**

|   成员名称    |                             描述                             |
| :-----------: | :----------------------------------------------------------: |
|  ps16InNear   |    语音信号地址，当采样率为32K时，表示语音信号的低段数据     |
|  ps16InNearH  | 采样率等于32K时需要配置，表示语音信号的高段数据，其他情况可置位NULL |
|  s16Samples   |                           采样长度                           |
| s32InMicLevel |                         输入信号音量                         |
|    s16Echo    |           输入信号是否有回声，0：无回声；1：有回声           |



#### ALG_WEBRTC_AGC_PARAM_S

**[说明]**

定义自动增益配置参数信息。

**[定义]**

`typedef struct tsALG_WEBRTC_AGC_PARAM_S{`

​	`TS_S16 s16TargetLevelDbfs;`

​	`TS_S16 s16CompressionGaindB;`

​	`TS_U8 u8LimiterEnable;`

`} ALG_WEBRTC_AGC_PARAM_S;`

**[成员]**

|       成员名称       |                      描述                      |
| :------------------: | :--------------------------------------------: |
|  s16TargetLevelDbfs  |      mic的目标音量，单位：-dBFS，默认：3       |
| s16CompressionGaindB |   AGC启用限幅器的增益限制，单位dB，默认：9。   |
|   u8LimiterEnable    | 启动限幅器标志，1：使能；0：禁用，默认：使能。 |



#### ALG_WEBRTC_AGC_RESULT_S

**[说明]**

定义自动增益算法结果信息。

**[定义]**

`typedef struct tsALG_WEBRTC_AGC_RESULT_S{`

​	`TS_S16* ps16Out;`

​	`TS_S16* ps16OutH;`

​	`TS_S32 s32OutMicLevel;`

​	`TS_U8 u8SaturationWarning;`

`}ALG_WEBRTC_AGC_RESULT_S;`

**[成员]**

|      成员名称       |                             描述                             |
| :-----------------: | :----------------------------------------------------------: |
|       ps16Out       | 完成自动增益冬至的采样地址，当采样率为32K时，表示完成处理后的低段数据 |
|      ps16OutH       | 当采样率等于32时需要配置，表示完成处理后高段数据，其他情况未使用 |
|   s32OutMicLevel    |                       调整后的mic增益                        |
| u8SaturationWarning |             饱和警告。1：出现饱和；2：未出现饱和             |



### **3.5.** 需求

#### 头文件

`#include<ts_alg_webrtc_agc.h>`

#### 库文件

- `libalgcommon.a`

- `libalgwebrtc3a.a`

  

### **3.6.** 模型说明

无



### **3.7.** EXAMPLE

**[示例目录]**

example/alg_3a_main.c
















[TOC]

## opus编解码

### 1.1. 算法类型

音频

### 1.2. 算法描述

opus编解码是把采样数据转为opus码流或是把opus码流转换为采样数据。

### **1.3.** API参考

- `TS_ALG_OpusEnc_Init`   ：初始化opus编码算法。
- `TS_ALG_OpusEnc_Process`：执行opus编码算法。
- `TS_ALG_OpusEnc_Exit`   ：释放opus编码算法资源。
- `TS_ALG_OpusDec_Init`   ：初始化opus解码算法。
- `TS_ALG_OpusDec_Process`：执行opus解码算法。
- `TS_ALG_OpusDec_Exit`   ：释放opus解码算法资源。
- `TS_ALG_Opus_GetVersion`   ：获取opus编解码算法版本信息。

#### `TS_ALG_OpusEnc_Init`

**[描述]**

初始化opus编码算法。

**[语法]**

`TS_S32 TS_ALG_OpusEnc_Init(TS_VOID **ppHandle, ALG_OPUS_ENC__PARAM_S*pParam);`

**[参数]**


| 参数名称 |                 描述                  | 输入/输出 |
| :------: | :-----------------------------------: | :-------: |
| ppHandle | 用以返回opus编码算法的 HANDLE指针地址 |   输出    |
|  pParam  |       opus编码算法的初始化信息        |   输入    |

**[返回值]**

| 返回值 |       描述       |
| :----: | :--------------: |
|   0    |       成功       |
|  非0   | 失败，参见错误码 |


#### `TS_ALG_OpusEnc_Process`

**[描述]**

执行opus编码算法。

**[语法]**

`TS_S32 TS_ALG_OpusEnc_Process(TS_VOID *pHandle, ALG_IMAGE_S *pImage, ALG_OPUS_ENC__RESULT_S*pResult);`

**[参数]**

| 参数名称 |     描述      | 输入/输出 |
| :------: | :----------: | :-------: |
| pHandle  |  HANDLE信息  |   输入    |
|  pImage  | 输入音频信息  |   输入    |
| pResult  |  输出编码信息 |   输出    |

**[返回值]**

| 返回值 |       描述       |
| :----: | :--------------: |
|   0    |       成功       |
|  非0   | 失败，参见错误码 |



#### `TS_ALG_OpusEnc_Exit`

**[描述]**

释放opus编码算法资源。

**[语法]**

`TS_S32 TS_ALG_OpusEnc_Exit(TS_VOID *pHandle)`

**[参数]**

| 参数名称 |    描述    | 输入/输出 |
| :------: | :--------: | :-------: |
| pHandle  | HANDLE信息 |   输入    |

**[返回值]**

| 返回值 |       描述       |
| :----: | :--------------: |
|   0    |       成功       |
|  非0   | 失败，参见错误码 |



#### `TS_ALG_OpusDec_Init`

**[描述]**

初始化opus解码算法。

**[语法]**

`TS_S32 TS_ALG_OpusDec_Init(TS_VOID** ppHandle, ALG_OPUS_DEC__PARAM_S* pParam);`

**[参数]**


| 参数名称 |                 描述                  | 输入/输出 |
| :------: | :-----------------------------------: | :-------: |
| ppHandle | 用以返回opus解码算法的 HANDLE指针地址 |   输出    |
|  pParam  |       opus解码算法的初始化信息        |   输入    |

**[返回值]**

| 返回值 |       描述       |
| :----: | :--------------: |
|   0    |       成功       |
|  非0   | 失败，参见错误码 |


#### `TS_ALG_OpusDec_Process`

**[描述]**

执行opus编码算法。

**[语法]**

`TS_S32 TS_ALG_OpusDec_Process(TS_VOID* pHandle, ALG_IMAGE_S* pImage, ALG_OPUS_DEC__RESULT_S* pResult);`

**[参数]**

| 参数名称 |     描述      | 输入/输出 |
| :------: | :----------: | :-------: |
| pHandle  |  HANDLE信息  |   输入    |
|  pImage  | 输入编码信息  |   输入    |
| pResult  |  输出解码信息 |   输出    |

**[返回值]**

| 返回值 |       描述       |
| :----: | :--------------: |
|   0    |       成功       |
|  非0   | 失败，参见错误码 |



#### `TS_ALG_OpusDec_Exit`

**[描述]**

释放opus解码算法资源。

**[语法]**

`TS_S32 TS_ALG_OpusDec_Exit(TS_VOID* pHandle)`

**[参数]**

| 参数名称 |    描述    | 输入/输出 |
| :------: | :--------: | :-------: |
| pHandle  | HANDLE信息 |   输入    |

**[返回值]**

| 返回值 |       描述       |
| :----: | :--------------: |
|   0    |       成功       |
|  非0   | 失败，参见错误码 |


#### `TS_ALG_Opus_GetVersion`

**[描述]**

获取opus编解码算法版本信息。

**[语法]**

`const TS_CHAR* TS_ALG_Opus_GetVersion(TS_VOID);`

**[返回值]**

| 返回值  |        描述        |
| :-----: | :----------------:|
| Version | 当前算法的版本信息  |


### **1.4.** 数据类型
- ALG_OPUS_ENC_PARAM_S：定义opus编码参数。
- ALG_OPUS_ENC_RESULT_S：定义opus编码结果。
- ALG_OPUS_DEC_PARAM_S：定义opus解码参数。
- ALG_OPUS_DEC_RESULT_S：定义opus解码结果。

#### ALG_OPUS_ENC_PARAM_S

**[说明]**

定义opus编码参数。

**[定义]**

`typedef struct tsALG_OPUS_ENC_PARAM_S{`

​	`TS_S32 s32SampleRate; // only support 8k / 16k`

​	`TS_S32 s32ChanNum;    // only support 1ch / 2ch`

​	`TS_S32 s32SampleNum;  // only support 160 / 320`

​	`TS_S32 s32Bps;        // bit rate per second`

`} ALG_OPUS_ENC_PARAM_S;`

**[成员]**

|       成员名称  |   描述     |
| :-------------: | :-------: |
|  s32SampleRate  | 采样率     |
|  s32ChanNum     | 通道数     |
|  s32SampleNum   | 帧长       |
|  s32Bps         | 每秒比特率  |

#### ALG_OPUS_ENC_RESULT_S

**[说明]**

定义opus编码结果。

**[定义]**

`typedef struct tsALG_OPUS_ENC_RESULT_S {`

​	`TS_U32 u32ByteNum;   //byte num`

​	`TS_U8 *pBitstream;`

`} ALG_OPUS_ENC_RESULT_S;`

**[成员]**

|       成员名称  |   描述    |
| :------------: | :-------: |
|  u32ByteNum    | 字节数     |
|  pBitstream    | opus编码流 |

#### ALG_OPUS_DEC_PARAM_S

**[说明]**

定义opus解码参数。

**[定义]**

`typedef struct tsALG_OPUS_DEC_PARAM_S {`

​	`TS_S32 s32SampleRate; // only support 8k / 16k`

​	`TS_S32 s32ChanNum;    // only support 1ch / 2ch`

`} ALG_OPUS_DEC_PARAM_S;`

**[成员]**

|       成员名称  |   描述    |
| :------------: | :-------: |
|  s32SampleRate | 采样率     |
|  s32ChanNum    | 通道数     |

#### ALG_OPUS_DEC_RESULT_S

**[说明]**

定义opus解码结果。

**[定义]**

`typedef struct tsALG_OPUS_DEC_RESULT_S {`

​	`TS_U32 u32ByteNum;   //byte num`

​	`TS_S16* pSampleData;`

`} ALG_OPUS_DEC_RESULT_S;`

**[成员]**

|      成员名称  |   描述    |
| :-----------: | :-------: |
|  u32ByteNum   |   字节数 |
|  pSampleData  |  采样数据 |



### **1.5.** 需求

#### 头文件

`#include<ts_alg_opus.h>`

#### 库文件

- `libalgcommon.a`

- `libalgopus.a`

  

### **1.6.** 模型说明

无

### **1.7.** EXAMPLE

**[示例目录]**

example/opus_main.c

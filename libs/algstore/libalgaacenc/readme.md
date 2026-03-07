[TOC]

## AAC编码

### 1.1. 算法类型

音频

### 1.2. 算法描述

AAC编码是把采样数据转换为aac码流。

### **1.3.** API参考

- `TS_ALG_AAC_ENC_Init`：初始化AAC编码算法。
- `TS_ALG_AAC_ENC_Process`：执行AAC编码算法。
- `TS_ALG_AAC_ENC_Exit`：释放AAC编码算法资源。
- `TS_ALG_AAC_ENC_GetVersion`：获取AAC编码算法版本信息。

#### `TS_ALG_AAC_ENC_Init`

**[描述]**

初始化AAC编码算法。

**[语法]**

`TS_S32 TS_ALG_AAC_ENC_Init(TS_VOID **ppHandle, ALG_AAC_ENC_INIT_S *pParam);`

**[参数]**

| 参数名称 |                 描述                 | 输入/输出 |
| :------: | :----------------------------------: | :-------: |
| ppHandle | 用以返回AAC编码算法的 HANDLE指针地址 |   输出    |
|  pParam  |       AAC编码算法的初始化信息        |   输入    |

**[返回值]**

| 返回值 |       描述       |
| :----: | :--------------: |
|   0    |       成功       |
|  非0   | 失败，参见错误码 |



#### `TS_ALG_AAC_ENC_Process`

**[描述]**

执行AAC编码算法。

**[语法]**

TS_S32 `TS_ALG_AAC_ENC_Process`(TS_VOID* pHandle, ALG_AAC_ENC_PARAM_S *pIuput, ALG_AAC_ENC_PARAM_S* pOutput);

**[参数]**

| 参数名称 |       描述        | 输入/输出 |
| :------: | :---------------: | :-------: |
| pHandle  |    HANDLE信息     |   输入    |
|  pInput  | AAC编码的采样数据 |   输入    |
| pOutput  |    AAC码流信息    |   输出    |

**[返回值]**

| 返回值 |       描述       |
| :----: | :--------------: |
|   0    |       成功       |
|  非0   | 失败，参见错误码 |



#### `TS_ALG_AAC_ENC_Exit`

**[描述]**

释放AAC编码算法资源。

**[语法]**

TS_S32 `TS_ALG_AAC_ENC_Exit`(TS_VOID* pHandle);

**[参数]**

| 参数名称 |    描述    | 输入/输出 |
| :------: | :--------: | :-------: |
| pHandle  | HANDLE信息 |   输入    |

**[返回值]**

| 返回值 |       描述       |
| :----: | :--------------: |
|   0    |       成功       |
|  非0   | 失败，参见错误码 |



#### `TS_ALG_AAC_ENC_GetVersion`

**[描述]**

获取AAC编码算法版本信息。

**[语法]**

const `TS_CHAR`* `TS_ALG_AAC_ENC_GetVersion`(TS_VOID);

**[返回值]**

| 返回值  |        描述        |
| :-----: | :----------------: |
| Version | 当前算法的版本信息 |



### **1.4.** 数据类型

- ALG_AAC_ENC_INIT_S：定义AAC编码的初始化信息。
- ALG_AAC_ENC_PARAM_S：定义AAC编码输入输出信息。

#### ALG_AAC_ENC_INIT_S

**[说明]**

定义AAC编码的初始化信息。

**[定义]**

`typedef struct tsALG_AAC_ENC_INIT_S{`

​	`TS_U32 u32SampleRate;`

​	`TS_U32 u32NumChannels;`

​	`TS_U32 u32OutputMode;`

​	`TS_U32 *pInputSamples;`

​	`TS_U32 *pMaxOutputBytes;`

`}ALG_AAC_ENC_INIT_S;`

**[成员]**

|    成员名称     |              描述              |
| :-------------: | :----------------------------: |
|  u32SampleRate  |             采样率             |
| u32NumChannels  |             通道数             |
|  u32OutputMode  | 输出码流格式 0 - raw；1 - ADTS |
|  pInputSample   |        每次输入的字节数        |
| pMaxOutputBytes |    每次编码输出的最大字节数    |



#### ALG_AAC_ENC_PARAM_S

**[说明]**

定义AAC编码输入输出信息。

**[定义]**

`typedef struct tsALG_AAC_ENC_PARAM_S {`

​	`TS_U32 u32SampleNum;`

​	`TS_U8  *pBitStream;`

`} ALG_AAC_ENC_PARAM_S;`

**[成员]**

|   成员名称   |        描述         |
| :----------: | :-----------------: |
| u32SampleNum |  输入/输出的字节数  |
|  pBitStream  | 输入/输出的数据地址 |



### **1.5.** 需求

#### 头文件

`#include<ts_alg_aac_enc.h>`

#### 库文件

- `libalgcommon.a`

- `libalgaacenc.a`

  

### **1.6.** 模型说明

无

### **1.7.** EXAMPLE

**[示例目录]**

example/aac_enc_main.c

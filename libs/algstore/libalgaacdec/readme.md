[TOC]

## AAC解码

### 1.1. 算法类型

音频

### 1.2. 算法描述

AAC解码是把aac码流转换为采样数据。

### **1.3.** API参考

- `TS_ALG_AAC_DEC_Init`：初始化AAC解码算法。
- `TS_ALG_AAC_DEC_Process`：执行AAC解码算法。
- `TS_ALG_AAC_DEC_Exit`：释放AAC解码算法资源。
- `TS_ALG_AAC_DEC_GetVersion`：获取AAC解码算法版本信息。

#### `TS_ALG_AAC_DEC_Init`

**[描述]**

初始化AAC解码算法。

**[语法]**

`TS_S32 TS_ALG_AAC_DEC_Init(TS_VOID **ppHandle, ALG_AAC_DEC_INIT_S *pParam);`

**[参数]**

| 参数名称 |                 描述                 | 输入/输出 |
| :------: | :----------------------------------: | :-------: |
| ppHandle | 用以返回AAC解码算法的 HANDLE指针地址 |   输出    |
|  pParam  |       AAC解码算法的初始化信息        |   输入    |

**[返回值]**

| 返回值 |       描述       |
| :----: | :--------------: |
|   0    |       成功       |
|  非0   | 失败，参见错误码 |



#### `TS_ALG_AAC_DEC_Process`

**[描述]**

执行AAC解码算法。

**[语法]**

TS_S32 `TS_ALG_AAC_DEC_Process`(TS_VOID* pHandle, ALG_AAC_DEC_PARAM_S *pParam);

**[参数]**

| 参数名称 |    描述     | 输入/输出 |
| :------: | :---------: | :-------: |
| pHandle  | HANDLE信息  |   输入    |
|  pParam  | AAC解码信息 | 输入/输出 |

**[返回值]**

| 返回值 |       描述       |
| :----: | :--------------: |
|   0    |       成功       |
|  非0   | 失败，参见错误码 |



#### `TS_ALG_AAC_DEC_Exit`

**[描述]**

释放AAC解码算法资源。

**[语法]**

TS_S32 `TS_ALG_AAC_DEC_Exit`(TS_VOID* pHandle);

**[参数]**

| 参数名称 |    描述    | 输入/输出 |
| :------: | :--------: | :-------: |
| pHandle  | HANDLE信息 |   输入    |

**[返回值]**

| 返回值 |       描述       |
| :----: | :--------------: |
|   0    |       成功       |
|  非0   | 失败，参见错误码 |



#### `TS_ALG_AAC_DEC_GetVersion`

**[描述]**

获取AAC解码算法版本信息。

**[语法]**

const `TS_CHAR`* `TS_ALG_AAC_DEC_GetVersion`(TS_VOID);

**[返回值]**

| 返回值  |        描述        |
| :-----: | :----------------: |
| Version | 当前算法的版本信息 |



### **1.4.** 数据类型

- ALG_AAC_DEC_INIT_S：定义AAC解码的初始化信息。
- ALG_AAC_DEC_PARAM_S：定义AAC解码输入输出信息。

#### ALG_AAC_DEC_INIT_S

**[说明]**

定义AAC解码的初始化信息。

**[定义]**

`typedef struct tsALG_AAC_DEC_INIT_S{`

​	`TS_U8  *pBitStream;`

​	`TS_U32 u32ByteNum;`

​	`TS_U32 u32SampleRate;`

​	`TS_U32 u32NumChannels;`

`} ALG_AAC_DEC_INIT_S;`

**[成员]**

|    成员名称    |       描述       |
| :------------: | :--------------: |
|   pBitStream   |     输入码流     |
|   u32ByteNum   | 输入码流的字节数 |
| u32SampleRate  |    输出采样率    |
| u32NumChannels |   输出通道信息   |



#### ALG_AAC_DEC_PARAM_S

**[说明]**

定义AAC解码输入输出信息。

**[定义]**

`typedef struct tsALG_AAC_DEC_PARAM_S {`

​	`TS_U32 u32InputByteNum;`

​	`TS_U8  *pBitStream;`

​	`TS_U32 u32DecoderByteNum;`

​	`TS_U8  *pOutStream;`

​	`TS_U32 u32OutputSamples;`

`} ALG_AAC_DEC_PARAM_S;`

**[成员]**

|     成员名称      |         描述         |
| :---------------: | :------------------: |
|  u32InputByteNum  |     输入的字节数     |
|    pBitStream     |    输入的数据地址    |
| u32DecoderByteNum |     解码的字节数     |
|    pOutStream     |     解码数据地址     |
| u32OutputSamples  | 解码输出的采样点数目 |



### **1.5.** 需求

#### 头文件

`#include<ts_alg_aac_dec.h>`

#### 库文件

- `libalgcommon.a`

- `libalgaacdec.a`

  

### **1.6.** 模型说明

无

### **1.7.** EXAMPLE

**[示例目录]**

example/aac_dec_main.c

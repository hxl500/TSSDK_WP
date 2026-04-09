[TOC]

## 语音活动检测

### 1.1. 算法类型

音频

### 1.2. 算法描述

检测是否有语音活动。

### **1.3.** API参考

- `TS_ALG_VAD_Init`：初始化语音活动检测算法。
- `TS_ALG_VAD_Process`：执行语音活动检测算法。
- `TS_ALG_VAD_Exit`：释放语音活动检测算法资源。
- `TS_ALG_VAD_GetVersion`：获取语音活动检测算法版本信息。

#### `TS_ALG_VAD_Init`

**[描述]**

初始化语音活动检测算法。

**[语法]**

`TS_S32 TS_ALG_VAD_Init(TS_VOID **ppHandle, ALG_VAD_INIT_S *pParam);`

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



#### `TS_ALG_VAD_Process`

**[描述]**

执行语音活动检测算法。

**[语法]**

TS_S32 `TS_ALG_VAD_Process`(TS_VOID* pHandle, ALG_VAD_PARAM_S* pInput);

**[参数]**

| 参数名称 |    描述    | 输入/输出 |
| :------: | :--------: | :-------: |
| pHandle  | HANDLE信息 |   输入    |
|  pInput  |  输入参数  |   输入    |

**[返回值]**

| 返回值 |       描述       |
| :----: | :--------------: |
|   0    | 未检测到语音活动 |
|   1    |  检测到语音活动  |
|  其它  |       失败       |



#### `TS_ALG_VAD_Exit`

**[描述]**

释放语音活动检测算法资源。

**[语法]**

TS_S32 `TS_ALG_VAD_Exit`(TS_VOID* pHandle);

**[参数]**

| 参数名称 |    描述    | 输入/输出 |
| :------: | :--------: | :-------: |
| pHandle  | HANDLE信息 |   输入    |

**[返回值]**

| 返回值 | 描述 |
| :----: | :--: |
|   0    | 成功 |
|  非0   | 失败 |



#### `TS_ALG_VAD_GetVersion`

**[描述]**

获取语音活动检测算法版本信息。

**[语法]**

const `TS_CHAR`* `TS_ALG_VAD_GetVersion`(TS_VOID);

**[返回值]**

| 返回值  |        描述        |
| :-----: | :----------------: |
| Version | 当前算法的版本信息 |



### **1.4.** 数据类型

- ALG_VAD_INIT_S：定义语音活动检测初始化用到的参数。
- ALG_VAD_PARAM_S：定义语音活动检测运行过程中用到的参数。

#### ALG_VAD_INIT_S

**[说明]**

定义语音活动检测初始化用到的参数。

**[定义]**

`typedef struct tsALG_VAD_INIT_S{`

​	`TS_U32 u32SampleRate;`

​	`TS_U32 u32NumChannels;`

​	`TS_U32 u32FrameLen;`

​	`TS_S32 s32MinMaxEnv;`

​	`TS_S32 s32SmoothSilenceTh;`

​	`TS_S32 s32SmoothActiveTh;`

`} ALG_VAD_INIT_S;`

**[成员]**

|      成员名称      |                     描述                     |
| :----------------: | :------------------------------------------: |
|   u32SampleRate    |                采样率:8K/16K                 |
|   u32NumChannels   |                   通道数量                   |
|    u32FrameLen     |                     帧长                     |
|    s32MinMaxEnv    |         环境噪声的信噪比[-65db~40db]         |
| s32SmoothSilenceTh | 静音计数器，默认是15帧，触发之后15帧不再触发 |
| s32SmoothActiveTh  |   检测计数器，默认是5帧，连续5帧检测到触发   |



#### ALG_VAD_PARAM_S

**[说明]**

定义语音活动检测运行过程中用到的参数。

**[定义]**

`typedef struct tsALG_VAD_PARAM_S{`

​	`TS_U32 u32SampleNum;		// input sample number or outpuut data number`

​	`TS_S16 *pBitStream;		// input/output  bitstream`

`} ALG_VAD_PARAM_S;`

**[成员]**

|   成员名称   |     描述     |
| :----------: | :----------: |
| u32SampleNum | 输入采样长度 |
|  pBitStream  | 输入数据地址 |



### **1.5.** 需求

#### 头文件

`#include "ts_alg_vad.h"`

#### 库文件

- `libalgcommon.a`

- `libalgvad.a`

  

### **1.6.** 模型说明

无

### **1.7.** EXAMPLE

**[示例目录]**

example/alg_vad_main.c

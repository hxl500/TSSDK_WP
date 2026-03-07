[TOC]

## 哭声检测

### 1.1. 算法类型

音频

### 1.2. 算法描述

哭声检测算法是用来检测婴儿哭声。

### **1.3.** API参考

- `TS_ALG_CryDetect_Init`：初始化哭声检测算法。
- `TS_ALG_CryDetect_Process`：执行哭声检测算法。
- `TS_ALG_CryDetect_Exit`：释放哭声检测算法资源。
- `TS_ALG_CryDetect_SetParam`：设置哭声检测算法参数。
- `TS_ALG_CryDetect_GetParam`：获取哭声检测算法法参数。
- `TS_ALG_CryDetect_GetVersion`：获取哭声检测算法版本信息。

#### `TS_ALG_CryDetect_Init`

**[描述]**

初始化哭声检测算法。

**[语法]**

`TS_S32 TS_ALG_CryDetect_Init(TS_VOID **ppHandle, ALG_MODEL_INIT_S *pParam);`

**[参数]**

| 参数名称 |                 描述                  | 输入/输出 |
| :------: | :-----------------------------------: | :-------: |
| ppHandle | 用以返回哭声检测算法的 HANDLE指针地址 |   输出    |
|  pParam  |       哭声检测算法的初始化信息        |   输入    |

**[返回值]**

| 返回值 |       描述       |
| :----: | :--------------: |
|   0    |       成功       |
|  非0   | 失败，参见错误码 |



#### `TS_ALG_CryDetect_Process`

**[描述]**

执行哭声检测算法。

**[语法]**

TS_S32 `TS_ALG_CryDetect_Process`(TS_VOID* pHandle, ALG_IMAGE_S *pImage, ALG_CRY _RESULT_S* pResult);

**[参数]**

| 参数名称 |        描述        | 输入/输出 |
| :------: | :----------------: | :-------: |
| pHandle  |     HANDLE信息     |   输入    |
|  pImage  | 哭声检测的采样数据 |   输入    |
| pResult  |    哭声检测结果    |   输出    |

**[返回值]**

| 返回值 |       描述       |
| :----: | :--------------: |
|   0    |       成功       |
|  非0   | 失败，参见错误码 |



#### `TS_ALG_CryDetect_Exit`

**[描述]**

释放哭声检测算法资源。

**[语法]**

TS_S32 `TS_ALG_CryDetect_Exit`(TS_VOID* pHandle);

**[参数]**

| 参数名称 |    描述    | 输入/输出 |
| :------: | :--------: | :-------: |
| pHandle  | HANDLE信息 |   输入    |

**[返回值]**

| 返回值 |       描述       |
| :----: | :--------------: |
|   0    |       成功       |
|  非0   | 失败，参见错误码 |



#### `TS_ALG_CryDetect_SetParam`

**[描述]**

配置哭声检测阈值。

**[语法]**

TS_S32 `TS_ALG_CryDetect_SetParam`(TS_VOID* pHandle, ALG_CRY_PARAM_S *pParam)`;

**[参数]**

| 参数名称 |    描述    | 输入/输出 |
| :------: | :--------: | :-------: |
| pHandle  | HANDLE信息 |   输入    |
|  pParam  |  参数信息  |   输入    |

**[返回值]**

| 返回值 |       描述       |
| :----: | :--------------: |
|   0    |       成功       |
|  非0   | 失败，参见错误码 |



#### `TS_ALG_CryDetect_GetParam`

**[描述]**

获取哭声检测阈值。

**[语法]**

TS_S32 `TS_ALG_CryDetect_GetParam`(TS_VOID* pHandle, ALG_CRY_PARAM_S *pParam);

**[参数]**

| 参数名称 |    描述    | 输入/输出 |
| :------: | :--------: | :-------: |
| pHandle  | HANDLE信息 |   输入    |
|  pParam  |  参数信息  |   输出    |

**[返回值]**

| 返回值 |       描述       |
| :----: | :--------------: |
|   0    |       成功       |
|  非0   | 失败，参见错误码 |



#### `TS_ALG_CryDetect_GetVersion`

**[描述]**

获取哭声检测算法版本信息。

**[语法]**

const `TS_CHAR`* `TS_ALG_CryDetect_GetVersion`(TS_VOID);

**[返回值]**

| 返回值  |        描述        |
| :-----: | :----------------: |
| Version | 当前算法的版本信息 |



### **1.4.** 数据类型

- ALG_CRY_RESULT S：定义哭声检测结果信息。
- ALG_CRY_PARAM_S：定义哭声检测配置信息。

#### ALG_CRY_RESULT_S

**[说明]**

定义哭声检测结果信息。

**[定义]**

`typedef struct tsALG_CRY_RESULT_S{`

  `TS_S32 s32CryRes;`

`} ALG_CRY_RESULT_S;`

**[成员]**

| 成员名称  |              描述               |
| :-------: | :-----------------------------: |
| s32CryRes | 哭声检测结果：0：no cry; 1：cry |



#### ALG_CRY_PARAM_S

**[说明]**

定义哭声检测配置信息。

**[定义]**

`typedef struct ts ALG_CRY_PARAM_S {`

  `TS_FLOAT f32Threshe`

`} ALG_CRY_PARAM_S;`

**[成员]**

|  成员名称  |     描述     |
| :--------: | :----------: |
| f32Threshe | 哭声检测阈值 |



### **1.5.** 需求

#### 头文件

`#include<ts_alg_crydetect.h>`

#### 库文件

- `libalgcommon.a`

- `libalgcrydetect.a`

  

### **1.6.** 模型说明
详情见model目录下的readme文件

### **1.7.** EXAMPLE

**[示例目录]**

example/crydetect_main.c

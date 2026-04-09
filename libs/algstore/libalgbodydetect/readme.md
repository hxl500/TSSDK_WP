[TOC]

## 人形检测

### 1.1. 算法类型

图像

### 1.2. 算法描述

人形检测是检测图片中是否存在人形，返回图片中人形box框区域，以及置信率。

### **1.3.** API参考

- `TS_ALG_BodyDet_Init`：初始化人形检测算法。
- `TS_ALG_BodyDet_Process`：执行人形检测算法。
- `TS_ALG_BodyDet_Exit`：释放人形检测算法资源。
- `TS_ALG_BodyDet_SetParam`：设置人形检测算法参数。
- `TS_ALG_BodyDet_GetParam`：获取人形检测算法参数。
- `TS_ALG_BodyDet_GetVersion`：获取人形检测算法版本信息。

#### `TS_ALG_BodyDet_Init`

**[描述]**

初始化人形检测算法。

**[语法]**

`TS_S32 TS_ALG_BodyDet_Init(TS_VOID **ppHandle, ALG_MODEL_INIT_S *pParam);`

**[参数]**

| 参数名称 |                 描述                  | 输入/输出 |
| :------: | :-----------------------------------: | :-------: |
| ppHandle | 用以返回人形检测算法的 HANDLE指针地址 |   输出    |
|  pParam  |       人形检测算法的初始化信息        |   输入    |

**[返回值]**

| 返回值 |       描述       |
| :----: | :--------------: |
|   0    |       成功       |
|  非0   | 失败，参见错误码 |



#### `TS_ALG_BodyDet_Process`

**[描述]**

执行人形检测算法。

**[语法]**

`TS_S32 TS_ALG_BodyDet_Process(TS_VOID* pHandle, ALG_IMAGE_S *pImage, ALG_BODYDET_RESULT_S* pResult);`

**[参数]**

| 参数名称 |      描述      | 输入/输出 |
| :------: | :------------: | :-------: |
| pHandle  |   HANDLE信息   |   输入    |
|  pImage  |  输入图片信息  |   输入    |
| pResult  | 人形检测结果框 |   输出    |

**[返回值]**

| 返回值 |       描述       |
| :----: | :--------------: |
|   0    |       成功       |
|  非0   | 失败，参见错误码 |



#### `TS_ALG_BodyDet_Exit`

**[描述]**

释放人形检测算法资源。

**[语法]**

`TS_S32 TS_ALG_BodyDet_Exit(TS_VOID* pHandle);`

**[参数]**

| 参数名称 |    描述    | 输入/输出 |
| :------: | :--------: | :-------: |
| pHandle  | HANDLE信息 |   输入    |

**[返回值]**

| 返回值 |       描述       |
| :----: | :--------------: |
|   0    |       成功       |
|  非0   | 失败，参见错误码 |



#### `TS_ALG_BodyDet_SetParam`

**[描述]**

配置人形检测算法参数。

**[语法]**

`TS_S32 TS_ALG_BodyDet_SetParam(TS_VOID* pHandle, ALG_BODYDET__PARAM_S *pParam)`;`

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



#### `TS_ALG_BodyDet_GetParam`

**[描述]**

获取人形检测算法参数。

**[语法]**

TS_S32 `TS_ALG_BodyDet_GetParam`(TS_VOID* pHandle, ALG_BODYDET_PARAM_S *pParam);

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



#### `TS_ALG_BodyDet_GetVersion`

**[描述]**

获取人形检测算法版本信息。

**[语法]**

`const TS_CHAR* TS_ALG_BodyDet_GetVersion();`

**[返回值]**

| 返回值  |        描述        |
| :-----: | :----------------: |
| Version | 当前算法的版本信息 |



### **1.4.** 数据类型

- ALG_BODYDET_BOX_S：定义人形检测结果框。
- ALG_BODYDET_RESULT_S：定义人形检测结果信息。
- ALG_BODYDET_PARAM_S：定义人形检测参数信息。

#### ALG_BODYDET_BOX_S

**[说明]**

定义人形检测结果框。

**[定义]**

`typedef struct tsALG_BODYDET_BOX_S{`

​	`TS_FLOAT f32Xmin;`

​	`TS_FLOAT f32Ymin;`

​	`TS_FLOAT f32Xmax;`

​	`TS_FLOAT f32Ymax;`

​	`TS_FLOAT f32Score;`

​	`TS_FLOAT f32Reserve;`

`} ALG_BODYDET_BOX_S;`

**[成员]**

|  成员名称  |     描述     |
| :--------: | :----------: |
|  f32Xmin   |  x轴最小值   |
|  f32Ymin   |  y轴最小值   |
|  f32Xmax   |  x轴最大值   |
|  f32Ymax   |  y轴最大值   |
|  f32Score  | 人形结果得分 |
| f32Reserve |   保留字段   |



#### ALG_BODYDET_RESULT_S

**[说明]**

定义人形检测结果信息。

**[定义]**

`typedef struct tsALG_BODYDET_RESULT_S{`

​	`TS_U32 u32BodyNum;`

​	`ALG_BODYDET_BOX_S stBox[MAX_BODYDET_NUM];`

`} ALG_BODYDET_RESULT_S;`

**[成员]**

|  成员名称  |   描述   |
| :--------: | :------: |
| u32BodyNum | 人形数量 |
|   stBox    |  人形框  |

**[注意事项]**

最大支持25个人形框，即MAX_BODYDET_NUM = 25。



#### ALG_BODYDET_PARAM_S

**[说明]**

定义人形检测参数信息。

**[定义]**

`typedef struct tsALG_BODYDET_PARAM_S{`

​	`TS_FLOAT f32Thresh;`

`} ALG_BODYDET_PARAM_S;`

**[成员]**

| 成员名称  |     描述     |
| :-------: | :----------: |
| f32Thresh | 人形检测阈值 |



### **1.5.** 需求

#### 头文件

`#include<ts_alg_body_detect.h>`

#### 库文件

- `libalgcommon.a`

- `libalgbodydetect.a`

  

### **1.6.** 模型说明
详情见model目录下的readme文件

### **1.7.** EXAMPLE

**[示例目录]**

example/bodydetect_main.c

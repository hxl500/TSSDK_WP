[TOC]

## 人车非检测

### 1.1. 算法类型

图像

### 1.2. 算法描述

人车非检测是检测图片中是否存在行人(pedestrian)、自行车(bicycle)、汽车(car)、电动车(ebike)、三轮车(etricycle)，返回图片中目标框box，以及置信率。

### **1.3.** API参考

- `TS_ALG_PbceDetect_Init`：初始化人车非检测算法。
- `TS_ALG_PbceDetect_Process`：执行人车非检测算法。
- `TS_ALG_PbceDetect_Exit`：释放人车非检测算法资源。
- `TS_ALG_PbceDetect_SetParam`：设置人车非检测算法参数。
- `TS_ALG_PbceDetect_GetParam`：获取人车非检测算法法参数。
- `TS_ALG_PbceDetect_GetVersion`：获取人车非检测算法版本信息。

#### `TS_ALG_PbceDetect_Init`

**[描述]**

初始化人车非检测算法。

**[语法]**

`TS_S32 TS_ALG_PbceDetect_Init(TS_VOID **ppHandle, ALG_MODEL_INIT_S *pParam);`

**[参数]**

| 参数名称 |                  描述                   | 输入/输出 |
| :------: | :-------------------------------------: | :-------: |
| ppHandle | 用以返回人车非检测算法的 HANDLE指针地址 |   输出    |
|  pParam  |       人车非检测算法的初始化信息        |   输入    |

**[返回值]**

| 返回值 |       描述       |
| :----: | :--------------: |
|   0    |       成功       |
|  非0   | 失败，参见错误码 |



#### `TS_ALG_PbceDetect_Process`

**[描述]**

执行人车非检测算法。

**[语法]**

TS_S32 `TS_ALG_PbceDetect_Process`(TS_VOID* pHandle, ALG_IMAGE_S *pImage, ALG_PBCEDETECT_RESULT_S* *pResult);

**[参数]**

| 参数名称 |       描述       | 输入/输出 |
| :------: | :--------------: | :-------: |
| pHandle  |    HANDLE信息    |   输入    |
|  pImage  |   输入图片信息   |   输入    |
| pResult  | 人车非检测结果框 |   输出    |

**[返回值]**

| 返回值 |       描述       |
| :----: | :--------------: |
|   0    |       成功       |
|  非0   | 失败，参见错误码 |



#### `TS_ALG_PbceDetect_Exit`

**[描述]**

释放人车非检测算法资源。

**[语法]**

TS_S32 `TS_ALG_PbceDetect_Exit`(TS_VOID* pHandle);

**[参数]**

| 参数名称 |    描述    | 输入/输出 |
| :------: | :--------: | :-------: |
| pHandle  | HANDLE信息 |   输入    |

**[返回值]**

| 返回值 |       描述       |
| :----: | :--------------: |
|   0    |       成功       |
|  非0   | 失败，参见错误码 |



#### `TS_ALG_PbceDetect_SetParam`

**[描述]**

配置人车非检测算法参数。

**[语法]**

TS_S32 `TS_ALG_PbceDetect_SetParam`(TS_VOID* pHandle, ALG_PBCEDETECT_PARAM_S *pParam)`;

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



#### `TS_ALG_PbceDetect_GetParam`

**[描述]**

获取人车非检测算法参数。

**[语法]**

TS_S32 `TS_ALG_PbceDetect_GetParam`(TS_VOID* pHandle, ALG_PBCEDETECT_PARAM_S *pParam);

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



#### `TS_ALG_PbceDetect_GetVersion`

**[描述]**

获取人车非检测算法版本信息。

**[语法]**

const `TS_CHAR`* `TS_ALG_PbceDetect_GetVersion`();

**[返回值]**

| 返回值  |        描述        |
| :-----: | :----------------: |
| Version | 当前算法的版本信息 |



### **1.4.** 数据类型

- ALG_PBCE_TYPE_E：定义人车非检测类型枚举。
- ALG_PBCEDETECT_BOX_S：定义人车非检测结果框。
- ALG_PBCEDETECT_PARAM_S：定义人车非检测参数信息。
- ALG_PBCEDETECT_RESULT_S: 定义人车非检测结果信息。

#### ALG_PBCE_TYPE_E

**[说明]**

定义人车非检测类型枚举。

**[定义]**

`typedef enum tsALG_PBCE_TYPE_E{`

​	`ALG_PBCE_DET_PEDESTRIAN=0,`

​	`ALG_PBCE_DET_BICYCLE,`

​	`ALG_PBCE_DET_CAR,`

​	`ALG_PBCE_DET_EBIKE,`

​	`ALG_PBCE_DET_ETRICYCLE,`

​	`ALG_PBCE_DET_NUM`

`} ALG_PBCE_TYPE_E;`

**[成员]**

|  成员名称  |   描述    |
| :--------: | :-------: |
|  ALG_PBCE_DET_PEDESTRIAN   | 人 |
|  ALG_PBCE_DET_BICYCLE   | 自行车 |
|  ALG_PBCE_DET_CAR   | 汽车 |
|  ALG_PBCE_DET_EBIKE   | 电动车 |
|  ALG_PBCE_DET_ETRICYCLE  | 三轮车  |
|  ALG_PBCE_DET_NUM | 保留字段  |



#### ALG_PBCEDETECT_BOX_S

**[说明]**

定义人车非检测结果框。

**[定义]**

`typedef struct tsALG_PBCEDETECT_BOX_S{`

​	`TS_FLOAT f32Xmin;`

​	`TS_FLOAT f32Ymin;`

​	`TS_FLOAT f32Xmax;`

​	`TS_FLOAT f32Ymax;`

​	`TS_FLOAT f32Score;`

​	`TS_FLOAT f32Reserve;`

`} ALG_PBCEDETECT_BOX_S;`

**[成员]**

|  成员名称  |   描述    |
| :--------: | :-------: |
|  f32Xmin   | X轴最小值 |
|  f32Ymin   | Y轴最小值 |
|  f32Xmax   | X轴最大值 |
|  f32Ymax   | Y轴最大值 |
|  f32Score  | 目标得分  |
| f32Reserve | 保留字段  |



#### ALG_PBCEDETECT_PARAM_S

**[说明]**

定义人车非检测参数信息。

**[定义]**

`typedef struct tsALG_PBCEDETECT_PARAM_S{`

​	`TS_FLOAT f32ConfThresh;`

​	`TS_FLOAT f32IouThresh;`

`} ALG_PBCEDETECT_PARAM_S;`

**[成员]**

|   成员名称   |   描述   |
| :----------: | :------: |
| f32ConfThresh | 算法阈值 |
| f32IouThresh  | IOU算法阈值 |



#### ALG_PBCEDETECT_RESULT_S

**[说明]**

定义人车非检测结果信息。

**[定义]**

`typedef struct tsALG_PBCEDETECT_RESULT_S{`

​	`TS_U32 u32ObjNum;`

​	`ALG_PBCE_TYPE_E u8ObjNo[MAX_PBCE_DETECT_NUM];`

​	`ALG_PBCEDETECT_BOX_S stBox[MAX_PBCE_DETECT_NUM];`

`} ALG_PBCEDETECT_RESULT_S;`

**[成员]**

|  成员名称  |                         描述                          |
| :--------: | :---------------------------------------------------: |
| u32ObjNum  |                       目标数量                        |
|  u8ObjNo   |                人车非检测类型枚举返回值                |
|  stBox     |                        目标框                          |

**[注意事项]**

最大支持25个目标框，即MAX_PBCE_DETECT_NUM = 25。



### **1.5.** 需求

#### 头文件

`#include<ts_alg_pbcedetect.h>`

#### 库文件

- `libalgcommon.a`

- `libalgpbcedetect.a`

  

### **1.6.** 模型说明
详情见model目录下的readme文件

### **1.7.** EXAMPLE

**[示例目录]**

example/pbcedetect_main.c

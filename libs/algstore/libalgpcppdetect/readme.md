[TOC]

## 多目标检测

### 1.1. 算法类型

图像

### 1.2. 算法描述

多目标检测是检测图片中是否存在行人(pedestrian)、车辆(car)、宠物(pet)、包裹(package)，返回图片中目标框box，以及置信率。

### **1.3.** API参考

- `TS_ALG_PcppDet_Init`：初始化多目标检测算法。
- `TS_ALG_PcppDet_Process`：执行多目标检测算法。
- `TS_ALG_PcppDet_Exit`：释放多目标检测算法资源。
- `TS_ALG_PcppDet_SetParam`：设置多目标检测算法参数。
- `TS_ALG_PcppDet_GetParam`：获取多目标检测算法法参数。
- `TS_ALG_PcppDet_GetVersion`：获取多目标检测算法版本信息。

#### `TS_ALG_PcppDet_Init`

**[描述]**

初始化多目标检测算法。

**[语法]**

`TS_S32 TS_ALG_PcppDet_Init(TS_VOID **ppHandle, ALG_MODEL_INIT_S *pParam);`

**[参数]**

| 参数名称 |                  描述                   | 输入/输出 |
| :------: | :-------------------------------------: | :-------: |
| ppHandle | 用以返回多目标检测算法的 HANDLE指针地址 |   输出    |
|  pParam  |       多目标检测算法的初始化信息        |   输入    |

**[返回值]**

| 返回值 |       描述       |
| :----: | :--------------: |
|   0    |       成功       |
|  非0   | 失败，参见错误码 |



#### `TS_ALG_PcppDet_Process`

**[描述]**

执行多目标检测算法。

**[语法]**

TS_S32 `TS_ALG_PcppDet_Process`(TS_VOID* pHandle, ALG_IMAGE_S *pImage, ALG_PCPP_DET_RESULT_S* *pResult);

**[参数]**

| 参数名称 |       描述       | 输入/输出 |
| :------: | :--------------: | :-------: |
| pHandle  |    HANDLE信息    |   输入    |
|  pImage  |   输入图片信息   |   输入    |
| pResult  | 多目标检测结果框 |   输出    |

**[返回值]**

| 返回值 |       描述       |
| :----: | :--------------: |
|   0    |       成功       |
|  非0   | 失败，参见错误码 |



#### `TS_ALG_PcppDet_Exit`

**[描述]**

释放多目标检测算法资源。

**[语法]**

TS_S32 `TS_ALG_PcppDet_Exit`(TS_VOID* pHandle);

**[参数]**

| 参数名称 |    描述    | 输入/输出 |
| :------: | :--------: | :-------: |
| pHandle  | HANDLE信息 |   输入    |

**[返回值]**

| 返回值 |       描述       |
| :----: | :--------------: |
|   0    |       成功       |
|  非0   | 失败，参见错误码 |



#### `TS_ALG_PcppDet_SetParam`

**[描述]**

配置多目标检测算法参数。

**[语法]**

TS_S32 `TS_ALG_PcppDet_SetParam`(TS_VOID* pHandle, ALG_PCPP_DET_PARAM_S *pParam)`;

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



#### `TS_ALG_PcppDet_GetParam`

**[描述]**

获取多目标检测算法参数。

**[语法]**

TS_S32 `TS_ALG_PcppDet_GetParam`(TS_VOID* pHandle, ALG_PCPP_DET_PARAM_S *pParam);

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



#### `TS_ALG_PcppDet_GetVersion`

**[描述]**

获取多目标检测算法版本信息。

**[语法]**

const `TS_CHAR`* `TS_ALG_PcppDet_GetVersion`();

**[返回值]**

| 返回值  |        描述        |
| :-----: | :----------------: |
| Version | 当前算法的版本信息 |



### **1.4.** 数据类型

- ALG_PCPP_DET_BOX_S：定义多目标检测结果框。
- ALG_PCPP_DET_PARAM_S：定义多目标检测参数信息。
- ALG_PCPP_DET_RESULT_S: 定义多目标检测结果信息。

#### ALG_PCPP_DET_BOX_S

**[说明]**

定义多目标检测结果框。

**[定义]**

`typedef struct tsALG_PCPP_DET_BOX_S{`

​	`TS_FLOAT f32Xmin;`

​	`TS_FLOAT f32Ymin;`

​	`TS_FLOAT f32Xmax;`

​	`TS_FLOAT f32Ymax;`

​	`TS_FLOAT f32Score;`

​	`TS_FLOAT f32Reserve;`

`} ALG_PCPP_DET_BOX_S;`

**[成员]**

|  成员名称  |   描述    |
| :--------: | :-------: |
|  f32Xmin   | X轴最小值 |
|  f32Ymin   | Y轴最小值 |
|  f32Xmax   | X轴最大值 |
|  f32Ymax   | Y轴最大值 |
|  f32Score  | 目标得分  |
| f32Reserve | 保留字段  |



#### ALG_PCPP_DET_PARAM_S

**[说明]**

定义多目标检测参数信息。

**[定义]**

`typedef struct tsALG_PCPP_DET_PARAM_S{`

​	`TS_FLOAT f32PedThresh;`

​	`TS_FLOAT f32CarThresh;`

​	`TS_FLOAT f32PetThresh;`

​	`TS_FLOAT f32PckThresh;`

`} ALG_PCPP_DET_PARAM_S;`

**[成员]**

|   成员名称   |   描述   |
| :----------: | :------: |
| f32PedThresh | 行人阈值 |
| f32CarThresh | 车辆阈值 |
| f32PetThresh | 宠物阈值 |
| f32PckThresh | 包裹阈值 |



#### ALG_PCPP_DET_RESULT_S

**[说明]**

定义多目标检测结果信息。

**[定义]**

`typedef struct tsALG_PCPP_DET_RESULT_S{`

​	`TS_U32 u32ObjNum;`

​	`ALG_PCPP_DET_BOX_S stBox[MAX_PCPP_DET_NUM];`

​	`TS_U8 u8ObjNo[MAX_PCPP_DET_NUM];`

`} ALG_PCPP_DET_RESULT_S;`

**[成员]**

|  成员名称  |                         描述                          |
| :--------: | :---------------------------------------------------: |
| u32FaceNum |                       目标数量                        |
|   stBox    |                        目标框                         |
|  u8ObjNo   | 目标id (pedestrian：0) (car：1) (pet：2) (package：3) |

**[注意事项]**

最大支持25个目标框，即MAX_PCPP_DET_NUM = 25。



### **1.5.** 需求

#### 头文件

`#include<ts_alg_pcpp_detect.h>`

#### 库文件

- `libalgcommon.a`

- `libalgpcppdetect.a`

  

### **1.6.** 模型说明

详情见model目录下的readme文件

### **1.7.** EXAMPLE

**[示例目录]**

example/pcppdetect_main.c

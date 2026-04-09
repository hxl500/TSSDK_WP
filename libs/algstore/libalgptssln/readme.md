[TOC]

## 客流量统计方案

### 1.1. 算法类型

图像

### 1.2. 算法描述

客流量统计方案是检测连续的图片中是否存在人形，并对检测到的人形进行跟踪，返回图片中目标框box，人形id以及置信率。客流量统计中增加了基线(左边点坐标和右边点坐标)和感兴趣区域(左上角，右下角)，基线是当人型检测框的一半进入时，算法会认为人是进入了；基线是当人型检测框的一半离开时，算法会认为人是出去了。感兴趣区域就是客流统计时关注的区域
1.对一张图处理时先进行人型检测，检测出人型框的位置，然后带入到轨迹跟踪中进行轨迹跟踪处理
2.对轨迹跟踪后的结果进行客流统计算法的检测，判断该图中人的活动方向和记录轨迹点

### **1.3.** API参考

- `TS_ALG_PtsSolution_Init`：初始化客流量统计方案算法。
- `TS_ALG_PtsSolution_Process`：执行客流量统计方案算法。
- `TS_ALG_PtsSolution_Exit`：释放客流量统计方案算法资源。
- `TS_ALG_PtsSolution_SetParam`：设置客流量统计方案算法参数。
- `TS_ALG_PtsSolution_GetParam`：获取客流量统计方案算法法参数。
- `TS_ALG_PtsSolution_GetVersion`：获取客流量统计方案算法版本信息。

#### `TS_ALG_PtsSolution_Init`

**[描述]**

初始化客流量统计方案算法。

**[语法]**

`TS_S32 TS_ALG_PtsSolution_Init(TS_VOID **ppHandle, ALG_PTS_SLN_INIT_S *pParam);`

**[参数]**

| 参数名称 |                  描述                   | 输入/输出 |
| :------: | :-------------------------------------: | :-------: |
| ppHandle | 用以返回算法的 HANDLE指针地址 |   输出    |
|  pParam  |       算法的初始化信息        |   输入    |

**[返回值]**

| 返回值 |       描述       |
| :----: | :--------------: |
|   0    |       成功       |
|  非0   | 失败，参见错误码 |



#### `TS_ALG_PtsSolution_Process`

**[描述]**

执行客流量统计方案算法。

**[语法]**

TS_S32 `TS_ALG_PtsSolution_Process`(TS_VOID* pHandle, ALG_IMAGE_S *pOriImage, ALG_IMAGE_S *pImageDet, ALG_PTS_SLN_RESULTS_S *pResult);

**[参数]**

| 参数名称 |       描述       | 输入/输出 |
| :------: | :--------------: | :-------: |
| pHandle  |    HANDLE信息    |   输入    |
|  pOriImage  |   输入原始图片信息   |   输入    |
|  pImageDet  |   输入图片信息   |   输入    |
| pResult  |      结果框      |   输出    |

**[返回值]**

| 返回值 |       描述       |
| :----: | :--------------: |
|   0    |       成功       |
|  非0   | 失败，参见错误码 |



#### `TS_ALG_PtsSolution_Exit`

**[描述]**

释放客流量统计方案算法资源。

**[语法]**

TS_S32 `TS_ALG_PtsSolution_Exit`(TS_VOID* pHandle);

**[参数]**

| 参数名称 |    描述    | 输入/输出 |
| :------: | :--------: | :-------: |
| pHandle  | HANDLE信息 |   输入    |

**[返回值]**

| 返回值 |       描述       |
| :----: | :--------------: |
|   0    |       成功       |
|  非0   | 失败，参见错误码 |



#### `TS_ALG_PtsSolution_SetParam`

**[描述]**

配置客流量统计方案算法参数。

**[语法]**

TS_S32 `TS_ALG_PtsSolution_SetParam`(TS_VOID* pHandle, ALG_PTS_SLN_PARAM_S *pParam)`;

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



#### `TS_ALG_PtsSolution_GetParam`

**[描述]**

获取客流量统计方案算法参数。

**[语法]**

TS_S32 `TS_ALG_PtsSolution_GetParam`(TS_VOID* pHandle, ALG_PTS_SLN_PARAM_S *pParam);

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



#### `TS_ALG_PtsSolution_GetVersion`

**[描述]**

获取客流量统计方案算法版本信息。

**[语法]**

const `TS_CHAR`* `TS_ALG_PtsSolution_GetVersion`();

**[返回值]**

| 返回值  |        描述        |
| :-----: | :----------------: |
| Version | 当前算法的版本信息 |



### **1.4.** 数据类型

- ALG_PTS_SLN_INIT_S：定义客流量统计方案初始化信息。
- ALG_PTS_SLN_PARAM_S：定义客流量统计方案参数信息。
- ALG_PTS_SLN_RESULTS_S: 定义客流量统计方案结果信息。

#### ALG_PTS_SLN_INIT_S

**[说明]**

定义客流量统计方案结果框。

**[定义]**

`typedef struct tsALG_PTS_SLN_INIT_S{`

​	`TS_U32 u32OriImgW;`

  `TS_U32 u32OriImgH;`

​	`ALG_MODEL_INIT_S stBodyDet;`

​	`ALG_PTS_INPUT_S stPts;`

​	`TS_U8 bRneOff;`

`} ALG_PTS_SLN_INIT_S;`

**[成员]**

|  成员名称  |   描述    |
| :--------: | :-------: |
|  u32OriImgW   | 原始图像的宽度 |
|  u32OriImgH   | 原始图像的高度 |
|  stBodyDet   | 人形检测初始化信息 |
|  stPts   | 客流量初始化信息 |
|  bRneOff  | rne开关  |



#### ALG_PTS_SLN_PARAM_S

**[说明]**

定义客流量统计方案参数信息。

**[定义]**

`typedef struct tsALG_PTS_SLN_PARAM_S{`

​	`ALG_BODYDET_PARAM_S bodyParam;`

​	`ALG_PTS_PARAM_S		ptsParam;`

`} ALG_PTS_SLN_PARAM_S;`

**[成员]**

|   成员名称   |   描述   |
| :----------: | :------: |
| bodyParam | 人形检测参数信息 |
| ptsParam  | 客流量统计参数信息 |



#### ALG_PTS_SLN_RESULTS_S

**[说明]**

定义客流量统计方案结果信息。

**[定义]**

`typedef struct tsALG_PTS_SLN_RESULTS_S{`

​	`TS_U32 u32Num;`

​	`ALG_BODYDET_RESULT_S stObjRes;`

​	`ALG_PTS_RESULTS_S ptsRes;`

`} ALG_PTS_SLN_RESULTS_S;`

**[成员]**

|  成员名称  |                         描述                          |
| :--------: | :---------------------------------------------------: |
| u32Num     |                       人形跟踪数量                     |
| stObjRes   |                       人形检测结果                     |
|  ptsRes    |                       客流量统计结果                   |



### **1.5.** 需求

#### 头文件

`#include "ts_alg_pts_solution.h"`

#### 库文件

- `libalgcommon.a`

- `libalgbodydetect.a`

- `libalgpts.a`

  

### **1.6.** 模型说明
无

### **1.7.** EXAMPLE

**[示例目录]**

example/ptssln_main.cpp

[TOC]

## 移动侦测

### 1.1. 算法类型

图像

### 1.2. 算法描述

移动侦测是检测图片中是否存在移动的物体，返回图片中移动目标数目和box框。

### **1.3.** API参考

- `TS_ALG_MotionDetect_Init`：初始化移动侦测算法。
- `TS_ALG_MotionDetect_Process`：执行移动侦测算法。
- `TS_ALG_MotionDetect_Exit`：释放移动侦测算法资源。
- `TS_ALG_MotionDetect_SetParam`：设置移动侦测算法参数。
- `TS_ALG_MotionDetect_GetParam`：获取移动侦测算法法参数。
- `TS_ALG_MotionDetect_GetVersion`：获取移动侦测算法版本信息。

#### `TS_ALG_MotionDetect_Init`

**[描述]**

初始化移动侦测算法。

**[语法]**

`TS_S32 TS_ALG_MotionDetect_Init(TS_VOID **ppHandle, ALG_MOTION_DETECT_INIT_S *pParam);`

**[参数]**

| 参数名称 |                 描述                  | 输入/输出 |
| :------: | :-----------------------------------: | :-------: |
| ppHandle | 用以返回移动侦测算法的 HANDLE指针地址 |   输出    |
|  pParam  |       移动侦测算法的初始化信息        |   输入    |

**[返回值]**

| 返回值 |       描述       |
| :----: | :--------------: |
|   0    |       成功       |
|  非0   | 失败，参见错误码 |



#### `TS_ALG_MotionDetect_Process`

**[描述]**

执行移动侦测算法。

**[语法]**

TS_S32 `TS_ALG_MotionDetect_Process`(TS_VOID* pHandle, ALG_IMAGE_S *pImage, ALG_MOTION_DETECT_RESULT_S* pResult);

**[参数]**

| 参数名称 |     描述     | 输入/输出 |
| :------: | :----------: | :-------: |
| pHandle  |  HANDLE信息  |   输入    |
|  pImage  | 输入图片信息 |   输入    |
| pResult  | 移动侦测结果 |   输出    |

**[返回值]**

| 返回值 |       描述       |
| :----: | :--------------: |
|   0    |       成功       |
|  非0   | 失败，参见错误码 |



#### `TS_ALG_MotionDetect_Exit`

**[描述]**

释放移动侦测算法资源。

**[语法]**

TS_S32 `TS_ALG_MotionDetect_Exit`(TS_VOID* pHandle);

**[参数]**

| 参数名称 |    描述    | 输入/输出 |
| :------: | :--------: | :-------: |
| pHandle  | HANDLE信息 |   输入    |

**[返回值]**

| 返回值 |       描述       |
| :----: | :--------------: |
|   0    |       成功       |
|  非0   | 失败，参见错误码 |



#### `TS_ALG_MotionDetect_SetParam`

**[描述]**

配置移动侦测算法参数。

**[语法]**

TS_S32 `TS_ALG_MotionDetect_SetParam`(TS_VOID* pHandle, ALG_MOTION_DETECT_PARAM_S *pParam)`;

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



#### `TS_ALG_MotionDetect_GetParam`

**[描述]**

获取移动侦测算法参数。

**[语法]**

TS_S32 `TS_ALG_MotionDetect_GetParam`(TS_VOID* pHandle, ALG_MOTION_DETECT_PARAM_S *pParam);

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



#### `TS_ALG_MotionDetect_GetVersion`

**[描述]**

获取移动侦测算法版本信息。

**[语法]**

const `TS_CHAR`* `TS_ALG_MotionDetect_GetVersion`(TS_VOID);

**[返回值]**

| 返回值  |        描述        |
| :-----: | :----------------: |
| Version | 当前算法的版本信息 |



### **1.4.** 数据类型

- ALG_MOTION_DETECT_INIT_S：定义移动侦测初始化信息。
- ALG_MOTION_DETECT_BOX_S：定义移动侦测结果框。
- ALG_MOTION_DETECT_PARAM_S：定义移动侦测参数信息。
- ALG_MOTION_DETECT_RESULT_S: 定义移动侦测结果信息。

#### ALG_MOTION_DETECT_INIT_S

**[说明]**

定义移动侦测初始化信息。

**[定义]**

`typedef struct tsALG_MOTION_DETECT_INIT_S{`

​	`TS_U32 u32ImgH;`

​	`TS_U32 u32ImgW;`

​	`TS_U32 u32ImgC;`

`} ALG_MOTION_DETECT_INIT_S;`

**[成员]**

| 成员名称 |      描述      |
| :------: | :------------: |
| u32ImgH  |  输入图像高度  |
| u32ImgW  |  输入图像宽度  |
| u32ImgC  | 输入图像通道数 |



#### ALG_MOTION_DETECT_BOX_S

**[说明]**

定义移动侦测结果框。

**[定义]**

`typedef struct tsALG_MOTION_DETECT_BOX_S{`

​	`TS_FLOAT f32Xmin;`

​	`TS_FLOAT f32Ymin;`

​	`TS_FLOAT f32Xmax;`

​	`TS_FLOAT f32Ymax;`

​	`TS_FLOAT f32Score;`

​	`TS_FLOAT f32Reserve;`

`} ALG_MOTION_DETECT_BOX_S;`

**[成员]**

|  成员名称  |   描述    |
| :--------: | :-------: |
|  f32Xmin   | X轴最小值 |
|  f32Ymin   | Y轴最小值 |
|  f32Xmax   | X轴最大值 |
|  f32Ymax   | Y轴最大值 |
|  f32Score  | 保留字段  |
| f32Reserve | 保留字段  |



#### ALG_MOTION_DETECT_PARAM_S

**[说明]**

定义移动侦测参数信息。

**[定义]**

`typedef struct tsALG_MOTION_DETECT_PARAM_S{`

​	`TS_U32 u32AreaThres;`

`} ALG_MOTION_DETECT_PARAM_S;`

**[成员]**

|   成员名称   |      描述      |
| :----------: | :------------: |
| u32AreaThres | 目标区域的面积 |



#### ALG_MOTION_DETECT_RESULT_S

**[说明]**

定义移动侦测结果信息。

**[定义]**

`typedef struct tsALG_MOTION_DETECT_RESULT_S{`

​	`TS_U32 u32ObjNum;`

​	`ALG_MOTION_DETECT_BOX_S stBox[MAX_MOTION_DETECT_NUM];`

`} ALG_MOTION_DETECT_RESULT_S;`

**[成员]**

| 成员名称  |   描述   |
| :-------: | :------: |
| u32ObjNum | 目标数量 |
|   stBox   |  目标框  |

**[注意事项]**

最大支持100个目标框，即MAX_MOTION_DETECT_NUM = 100。



### **1.5.** 需求

#### 头文件

`#include "ts_alg_motiondetect.h"`

#### 库文件

- `libalgcommon.a`

- `libalgmotiondetect.a`

  

### **1.6.** 模型说明

无

### **1.7.** EXAMPLE

**[示例目录]**

example/motiondetect_main.c


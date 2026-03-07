[TOC]

## 人脸检测

### 1.1. 算法类型

图像

### 1.2. 算法描述

人脸检测是检测图片中是否存在人脸，返回图片中人脸box框区域，以及置信率。

### **1.3.** API参考

- `TS_ALG_FaceDetect_Init`：初始化人脸检测算法。
- `TS_ALG_FaceDetect_Process`：执行人脸检测算法。
- `TS_ALG_FaceDetect_Exit`：释放人脸检测算法资源。
- `TS_ALG_FaceDetect_SetParam`：设置人脸检测算法参数。
- `TS_ALG_FaceDetect_GetParam`：获取人脸检测算法参数。
- `TS_ALG_FaceDetect_GetVersion`：获取人脸检测算法版本信息。

#### `TS_ALG_FaceDetect_Init`

**[描述]**

初始化人脸检测算法。

**[语法]**

`TS_S32 TS_ALG_FaceDetect_Init(TS_VOID **ppHandle, ALG_MODEL_INIT_S *pParam);`

**[参数]**

| 参数名称 |                 描述                  | 输入/输出 |
| :------: | :-----------------------------------: | :-------: |
| ppHandle | 用以返回人脸检测算法的 HANDLE指针地址 |   输出    |
|  pParam  |       人脸检测算法的初始化信息        |   输入    |

**[返回值]**

| 返回值 |       描述       |
| :----: | :--------------: |
|   0    |       成功       |
|  非0   | 失败，参见错误码 |



#### `TS_ALG_FaceDetect_Process`

**[描述]**

执行人脸检测算法。

**[语法]**

TS_S32 `TS_ALG_FaceDetect_Process`(TS_VOID* pHandle, ALG_IMAGE_S *pImage, ALG_FACE_DETECT_RESULT_S *pResult);

**[参数]**

| 参数名称 |     描述     | 输入/输出 |
| :------: | :----------: | :-------: |
| pHandle  |  HANDLE信息  |   输入    |
|  pImage  | 输入图片信息 |   输入    |
| pResult  | 人脸检测结果 |   输出    |

**[返回值]**

| 返回值 |       描述       |
| :----: | :--------------: |
|   0    |       成功       |
|  非0   | 失败，参见错误码 |



#### `TS_ALG_FaceDetect_Exit`

**[描述]**

释放人脸检测算法资源。

**[语法]**

TS_S32 `TS_ALG_FaceDetect_Exit`(TS_VOID* pHandle);

**[参数]**

| 参数名称 |    描述    | 输入/输出 |
| :------: | :--------: | :-------: |
| pHandle  | HANDLE信息 |   输入    |

**[返回值]**

| 返回值 |       描述       |
| :----: | :--------------: |
|   0    |       成功       |
|  非0   | 失败，参见错误码 |



#### `TS_ALG_FaceDetect_SetParam`

**[描述]**

配置人脸检测算法参数。

**[语法]**

TS_S32 `TS_ALG_FaceDetect_SetParam`(TS_VOID* pHandle, ALG_FACE_DETECT_PARAM_S *pParam)`;

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



#### `TS_ALG_FaceDetect_GetParam`

**[描述]**

获取人脸检测算法参数。

**[语法]**

TS_S32 `TS_ALG_FaceDetect_GetParam`(TS_VOID* pHandle, ALG_FACE_DETECT_PARAM_S *pParam);

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



#### `TS_ALG_FaceDetect_GetVersion`

**[描述]**

获取人脸检测算法版本信息。

**[语法]**

const `TS_CHAR`* `TS_ALG_FaceDetect_GetVersion`(TS_VOID);

**[返回值]**

| 返回值  |        描述        |
| :-----: | :----------------: |
| Version | 当前算法的版本信息 |



### **1.4.** 数据类型

- ALG_FACEDET_BOX_S：定义人脸检测结果框。
- ALG_FACEDET_PARAM_S：定义人脸检测参数信息。
- ALG_FACEDET_RESULT_S: 定义人脸检测结果信息。

#### ALG_FACEDET_BOX_S

**[说明]**

定义人脸检测结果框。

**[定义]**

`typedef struct tsALGFACEDET_BOX_S{`

​	`TS_FLOAT f32Xmin;`

​	`TS_FLOAT f32Ymin;`

​	`TS_FLOAT f32Xmax;`

​	`TS_FLOAT f32Ymax;`

​	`TS_FLOAT f32Score;`

​	`TS_FLOAT f32Reserve;`

`} ALG_FACEDET_BOX_S;`

**[成员]**

|  成员名称  |   描述    |
| :--------: | :-------: |
|  f32Xmin   | X轴最小值 |
|  f32Ymin   | Y轴最小值 |
|  f32Xmax   | X轴最大值 |
|  f32Ymax   | Y轴最大值 |
|  f32Score  | 目标得分  |
| f32Reserve | 保留字段  |



#### ALG_FACEDET_PARAM_S

**[说明]**

定义人脸检测参数信息。

**[定义]**

`typedef struct tsALG_FACEDET_PARAM_S{`

​	`TS_FLOAT f32Thresh;`

`} ALG_FACEDET_PARAM_S;`

**[成员]**

| 成员名称  |     描述     |
| :-------: | :----------: |
| f32Thresh | 人脸检测阈值 |



#### ALG_FACEDET_RESULT_S

**[说明]**

定义人脸检测结果信息。

**[定义]**

`typedef struct tsALG_FACEDET_RESULT_S{`

​	`TS_U32 u32FaceNum;`

​	`ALG_FACEDET_BOX_S stBox[MAX_FACEDET_NUM];`

`} ALG_FACEDET_RESULT_S;`

**[成员]**

|  成员名称  |   描述   |
| :--------: | :------: |
| u32FaceNum | 人脸数量 |
|   stBox    |  人脸框  |

**[注意事项]**

最大支持25个人脸框，即MAX_FACEDET_NUM = 25。



### **1.5.** 需求

#### 头文件

`#include<ts_alg_face_detect.h>`

#### 库文件

- `libalgcommon.a`

- `libalgfacedetect.a`

  

### **1.6.** 模型说明
详情见model目录下的readme文件

### **1.7.** EXAMPLE

**[示例目录]**

example/facedetect_main.c

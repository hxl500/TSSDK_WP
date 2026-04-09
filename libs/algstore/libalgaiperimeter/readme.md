[TOC]

## AI周界检测

### 1.1. 算法类型

图像

### 1.2. 算法描述

AI周界检测是检测图片中指定区域是否存在区域入侵、区域越界（围栏模式），并进行区域内活动的相关统计。

### **1.3.** API参考

- `TS_ALG_AiPerimeter_Init`：初始化AI周界检测算法。
- `TS_ALG_AiPerimeter_Process`：执行AI周界检测算法。
- `TS_ALG_AiPerimeter_Exit`：释放AI周界检测算法资源。
- `TS_ALG_AiPerimeter_SetParam`：设置AI周界检测算法参数。
- `TS_ALG_AiPerimeter_GetParam`：获取AI周界检测算法参数。
- `TS_ALG_AiPerimeter_GetVersion`：获取AI周界检测算法版本信息。

#### `TS_ALG_AiPerimeter_Init`

**[描述]**

初始化AI周界检测算法。

**[语法]**

`TS_S32 TS_ALG_AiPerimeter_Init(TS_VOID **ppHandle, ALG_AI_PERIMETER_INIT_S *pParam)`

**[参数]**

| 参数名称 |                 描述                  | 输入/输出 |
| :------: | :-----------------------------------: | :-------: |
| ppHandle | 用以返回AI周界检测算法的 HANDLE指针地址 |   输出    |
|  pParam  |       AI周界检测算法的初始化信息        |   输入    |

**[返回值]**

| 返回值 |       描述       |
| :----: | :--------------: |
|   0    |       成功       |
|  非0   | 失败，参见错误码 |



#### `TS_ALG_AiPerimeter_Process`

**[描述]**

执行AI周界检测算法。

**[语法]**

`TS_S32 TS_ALG_AiPerimeter_Process(TS_VOID *pHandle, ALG_IMAGE_S *pOriImage, ALG_IMAGE_S *pImage, ALG_AI_PERIMETER_RESULTS_S *pResult)`

**[参数]**

| 参数名称 |     描述     | 输入/输出 |
| :------: | :----------: | :-------: |
| pHandle  |  HANDLE信息  |   输入    |
| pOriImage| 感兴趣区域信息 |   输入    |
|  pImage  | 输入图片信息 |   输入    |
| pResult  | AI周界检测结果 |   输出    |

**[返回值]**

| 返回值 |       描述       |
| :----: | :--------------: |
|   0    |       成功       |
|  非0   | 失败，参见错误码 |



#### `TS_ALG_AiPerimeter_Exit`

**[描述]**

释放AI周界检测算法资源。

**[语法]**

`TS_S32 TS_ALG_AiPerimeter_Exit(TS_VOID *pHandle)`

**[参数]**

| 参数名称 |    描述    | 输入/输出 |
| :------: | :--------: | :-------: |
| pHandle  | HANDLE信息 |   输入    |

**[返回值]**

| 返回值 |       描述       |
| :----: | :--------------: |
|   0    |       成功       |
|  非0   | 失败，参见错误码 |



#### `TS_ALG_AiPerimeter_SetParam`

**[描述]**

配置AI周界检测算法参数。

**[语法]**

`TS_S32 TS_ALG_AiPerimeter_SetParam(TS_VOID *pHandle, ALG_AI_PERIMETER_PARAM_S *pParam)`

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



#### `TS_ALG_AiPerimeter_GetParam`

**[描述]**

获取AI周界检测算法参数。

**[语法]**

`TS_S32 TS_ALG_AiPerimeter_GetParam(TS_VOID *pHandle, ALG_AI_PERIMETER_PARAM_S *pParam)`

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



#### `TS_ALG_AiPerimeter_GetVersion`

**[描述]**

获取AI周界检测算法版本信息。

**[语法]**

`const TS_CHAR* TS_ALG_AiPerimeter_GetVersion()`

**[返回值]**

| 返回值  |        描述        |
| :-----: | :----------------: |
| Version | 当前算法的版本信息 |



### **1.4.** 数据类型
- ALG_AI_PERIMETER_INIT_S：定义AI周界检测初始化信息。
- ALG_AI_PERIMETER_PARAM_S：定义AI周界检测参数信息。
- ALG_AI_PERIMETER_RESULTS_S: 定义AI周界检测结果信息。

#### ALG_AI_PERIMETER_INIT_S

**[说明]**

定义AI周界检测初始化信息。

**[定义]**

`typedef struct tsALG_AI_PERIMETER_INIT_S {`

​	`TS_U32 u32OriImgW;`

​	`TS_U32 u32OriImgH;`

​	`ALG_MODEL_INIT_S stBodyDet;  `

​	`ALG_PTS_INPUT_S stPts;`

​	`TS_U8 					bRegionalInstrusion;`

​	`TS_U8					bCrossBorderDetection;`

​	`TS_U8					bEnclosure;`

​	`TS_U32					roiRect[4];	`

​	`TS_U8 bRneOff;`

`} ALG_AI_PERIMETER_INIT_S;`

**[成员]**

|        成员名称        |   描述    |
| :-------------------: | :-------: |
|  u32OriImgW           | 感兴趣区域宽度 |
|  u32OriImgH           | 感兴趣区域高度 |
|  stBodyDet            | 人形检测模型初始化信息 |
|  stPts                | 区域统计参数 |
|  bRegionalInstrusion  | 使能区域入侵 0 不使能 1 使能  |
| bCrossBorderDetection | 使能越界 0 不使能 1 双向使能 2 上单向使能 3 下单向使能  |
|  bEnclosure           | 使能围栏 0 不使能 1 使能  |
| roiRect               | 入侵区域或围栏区域 左上角x,y 右下角x,y |
| bRneOff               | rne设备是否关闭  |


#### ALG_AI_PERIMETER_PARAM_S

**[说明]**

定义AI周界检测参数信息。

**[定义]**

`typedef struct tsALG_AI_PERIMETER_PARAM_S{`

​	`ALG_PBCEDETECT_PARAM_S	bodyParam;`		

​	`ALG_PTS_PARAM_S		ptsParam;`				

​	`TS_U8 					bRegionalInstrusion;`	

​	`TS_U8					bCrossBorderDetection;`	

​	`TS_U8					bEnclosure;`			

​	`TS_U32					roiRect[4];`			

`} ALG_AI_PERIMETER_PARAM_S;`

**[成员]**

|        成员名称        |                        描述                         |
| :-------------------: | :-------------------------------------------------: |
| bodyParam             | 人形检测参数    |
| ptsParam              | pts参数信息     |
| bRegionalInstrusion   | 使能区域入侵 0 不使能 1 使能 |
| bCrossBorderDetection | 使能越界 0 不使能 1 双向使能 2 上单向使能 3 下单向使能 |
| bEnclosure            | 使能围栏 0 不使能 1 使能 |
| roiRect               | 入侵区域或围栏区域 左上角x,y 右下角x,y |

#### ALG_AI_PERIMETER_RESULTS_S

**[说明]**

定义AI周界检测结果信息。

**[定义]**

`typedef struct tsALG_AI_PERIMETER_RESULTS_S {`
​	`TS_U32 u32Num;`
​	`TS_U8  u8AlarmType;	`	
​	`ALG_PBCEDETECT_RESULT_S 		stBodydet;`
​	`ALG_PTS_RESULTS_S 			stPts;`
​	`ALG_PTS_S					alarm;`	
`} ALG_AI_PERIMETER_RESULTS_S;`

**[成员]**

|     成员名称  |                  描述               |
| :----------: | :---------------------------------: |
| u32Num       | 结果数目                            |
| u8AlarmType  | 0 无告警 1 区域入侵 2 越界侦测 3 围栏 |
| stBodydet    | 人形检测结果                         |
| stPts        | 区域统计信息                         |
| alarm        | 报警框信息                           |



### **1.5.** 需求

#### 头文件

`#include "ts_alg_face_aiperimeter.h" `

#### 库文件

- `libalgcommon.a`

- `libalgpbcedetect.a`

- `liblgpts.a`

- `libalgfacedetect.a`

  

### **1.6.** 模型说明
见libalgpbcedetect下的model说明

### **1.7.** EXAMPLE

**[示例目录]**

example/aiperimeter_main.cpp

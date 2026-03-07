[TOC]

## 图像翻转

### 1.1. 算法类型

图像

### 1.2. 算法描述

图像翻转是实现图像水平、垂直或是水平+垂直的翻转。

### **1.3.** API参考

- `TS_ALG_Flip`：图像翻转。

#### `TS_ALG_Flip`

**[描述]**

图像翻转。

**[语法]**

`TS_S32 TS_ALG_Flip(TS_VOID *pSrc, TS_VOID *pDst, TS_U32 u32ImgW, TS_U32 u32ImgH, `

​										`ALG_FLIP_IMG_MODE_E eImgMode, TS_S32 u32FlipMode);`

**[参数]**

|  参数名称   |                        描述                         | 输入/输出 |
| :---------: | :-------------------------------------------------: | :-------: |
|    pSrc     |                    输入图像地址                     |   输入    |
|    pDst     |                    输出图像地址                     |   输出    |
|   u32ImgW   |                    输入图像宽度                     |   输入    |
|   u32ImgH   |                    输入图像高度                     |   输入    |
|  eImgMode   |                    输入图像格式                     |   输入    |
| u32FlipMode | 图像翻转模式 （>0：水平、 =0：垂直、<0：水平+垂直） |   输入    |

**[返回值]**

| 返回值 |       描述       |
| :----: | :--------------: |
|   0    |       成功       |
|  非0   | 失败，参见错误码 |



### **1.4.** 数据类型

- ALG_FLIP_IMG_MODE_E：输入图像格式。

#### ALG_FLIP_IMG_MODE_E

**[说明]**

定义图像格式。

**[定义]**

`typedef enum ts ALG_FLIP_IMG_MODE_E {`

   `ALG_FLIP_ARGB1555 = 0,`

​	`ALG_FLIP_ARGB8888,`

​	`ALG_FLIP_1BIT,`

​	`ALG_FLIP_2BIT`

`} ALG_FLIP_IMG_MODE_E;`

**[成员]**

|     成员名称      |   描述   |
| :---------------: | :------: |
| ALG_FLIP_ARGB1555 | Argb1555 |
| ALG_FLIP_ARGB8888 | Argb8888 |
|   ALG_FLIP_1BIT   |  1 bit   |
|   ALG_FLIP_2BIT   |  2 bit   |



### **1.5.** 需求

#### 头文件

`#include "ts_alg_flip.h"`

#### 库文件

- `libalgcommon.a`

- `libalgflip.a`

  

### **1.6.** 模型说明

无

### **1.7.** EXAMPLE

**[示例目录]**

example/flip_main.c

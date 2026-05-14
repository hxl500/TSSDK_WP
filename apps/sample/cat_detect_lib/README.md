# Cat Detect Lib

猫咪检测算法库的优化版本，具有清晰的文件结构和高可读性。

## 📁 目录结构

```
cat_detect_lib/
├── include/                    # 头文件目录
│   ├── api/                   # 对外API接口
│   │   └── video_alg_catdetect-api.h
│   ├── core/                  # 核心功能头文件
│   │   ├── video_alg_catdetect.h
│   │   ├── ts_alg_body_detect_v2.h
│   │   └── tsalg_alg_lib.h
│   └── utils/                 # 工具类头文件
│       ├── qi_nv12_yuv.h
│       ├── arrr_diff.h
│       ├── file_sync.h
│       ├── iot_net.h
│       ├── ipc_base.h
│       ├── sample_alg_cpm.h
│       └── cJSON.h
├── src/                       # 源文件目录
│   ├── core/                  # 核心检测算法
│   │   ├── lib_video_alg_catdetect.c
│   │   ├── lib_ts_alg_body_detect.cpp
│   │   ├── lib_tsalg_alg_lib.c
│   │   ├── lib_tsalg_alg_rsn.c
│   │   └── lib_food_rsn.c
│   ├── utils/                 # 工具类实现
│   │   ├── qi_nv12_yuv.cpp
│   │   ├── lib_act.c
│   │   ├── lib_array_diff.c
│   │   ├── lib_file_sync.c
│   │   ├── lib_dirsync.c
│   │   └── lib_iot_net.c
│   └── record_file/           # 录像相关
│       ├── ts_rne_record_file.h
│       └── ts_rne_record_file.c
├── config/                    # 配置文件
│   └── ota_bin.ini
└── 架构分析与优化方案.md     # 架构优化文档
```

## 📦 模块说明

### 核心检测模块 (src/core/)

- **lib_video_alg_catdetect.c**: 猫咪检测核心实现
- **lib_ts_alg_body_detect.cpp**: 人体检测实现
- **lib_tsalg_alg_lib.c**: 算法库基础实现
- **lib_tsalg_alg_rsn.c**: RSN算法实现
- **lib_food_rsn.c**: 食物检测实现

### 工具模块 (src/utils/)

- **qi_nv12_yuv.cpp**: YUV图像处理
- **lib_act.c**: 行为检测
- **lib_array_diff.c**: 数组差异处理
- **lib_file_sync.c**: 文件同步
- **lib_dirsync.c**: 目录同步
- **lib_iot_net.c**: IoT网络通信

### 录像模块 (src/record_file/)

- 录像文件处理相关功能

## 🔧 编译说明

### 头文件包含路径

在您的项目中，需要添加以下包含路径：

```cmake
include_directories(
    path/to/cat_detect_lib/include
    path/to/cat_detect_lib/include/api
    path/to/cat_detect_lib/include/core
    path/to/cat_detect_lib/include/utils
)
```

### 源文件列表

```cmake
set(SOURCES
    src/core/lib_video_alg_catdetect.c
    src/core/lib_ts_alg_body_detect.cpp
    src/core/lib_tsalg_alg_lib.c
    src/core/lib_tsalg_alg_rsn.c
    src/core/lib_food_rsn.c
    src/utils/qi_nv12_yuv.cpp
    src/utils/lib_act.c
    src/utils/lib_array_diff.c
    src/utils/lib_file_sync.c
    src/utils/lib_dirsync.c
    src/utils/lib_iot_net.c
    src/record_file/ts_rne_record_file.c
)
```

## 📝 使用示例

```c
#include "video_alg_catdetect-api.h"

// 初始化
ALG_CAT_MODEL_INIT_S init_param;
TS_VOID *handle = NULL;
TS_ALG_CatDetect_Init(&handle, &init_param);

// 处理图像
ALG_IMAGE_S image;
ALG_CatDetect_DET_RESULT_S result;
VIDEO_ALG_CatDetect_Proc(handle, &image, NULL, &result, 0);

// 释放资源
VIDEO_ALG_CatDetect_Exit(handle);
```

## 📚 文档

- 详细的架构分析和优化方案请参考：`架构分析与优化方案.md`

## ✨ 优化优势

1. **可读性提升**: 清晰的目录结构，一眼就能找到所需文件
2. **可维护性提升**: 模块边界清晰，修改一个模块不影响其他模块
3. **可扩展性提升**: 新增功能可以按模块添加到对应目录
4. **代码质量提升**: 强制按模块组织，减少耦合

## 📄 许可证

本项目代码仅供内部使用。

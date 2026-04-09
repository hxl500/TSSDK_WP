# CMakeLists.txt 变更说明

## 变更日期
2026-03-07

## 变更概述
本次变更主要是为了适配新的文件结构，将 cat_detect_lib 的文件组织更加清晰化。

## 变更详情

### 1. 新增 include 路径 (第94-98行)

**变更前：**
```cmake
# include_directories(${SAMPLE_CATDETECT_DIR})
# include_directories(${SAMPLE_CATDETECT_DIR}/record_file)
```

**变更后：**
```cmake
# include_directories(${SAMPLE_CATDETECT_DIR})
# include_directories(${SAMPLE_CATDETECT_DIR}/record_file)
include_directories(${SAMPLE_CATDETECT_DIR}/include)
include_directories(${SAMPLE_CATDETECT_DIR}/include/api)
include_directories(${SAMPLE_CATDETECT_DIR}/include/core)
include_directories(${SAMPLE_CATDETECT_DIR}/include/utils)
include_directories(${SAMPLE_CATDETECT_DIR}/src/record_file)
```

**说明：**
- 添加了新的 include 目录路径
- 按照新的文件结构组织头文件包含

---

### 2. 更新源文件列表 (第131-145行)

**变更前：**
```cmake
add_library(${SAMPLE_LIB_CATDETECT} STATIC
	${SAMPLE_CATDETECT_DIR}/lib_ts_alg_body_detect.cpp
	${SAMPLE_CATDETECT_DIR}/lib_video_alg_catdetect.c 
	${SAMPLE_CATDETECT_DIR}/lib_tsalg_alg_lib.c
	${SAMPLE_CATDETECT_DIR}/lib_tsalg_alg_rsn.c
	${SAMPLE_CATDETECT_DIR}/lib_food_rsn.c
	${SAMPLE_CATDETECT_DIR}/lib_array_diff.c
	${SAMPLE_CATDETECT_DIR}/lib_act.c
	${SAMPLE_CATDETECT_DIR}/lib_dirsync.c
	${SAMPLE_CATDETECT_DIR}/lib_file_sync.c
	${SAMPLE_CATDETECT_DIR}/lib_iot_net.c
	#${SAMPLE_CATDETECT_DIR}/qi_nv12_yuv.cpp
	# ${SAMPLE_CATDETECT_DIR}/record_file/ts_rne_record_file.c
	 
)
```

**变更后：**
```cmake
add_library(${SAMPLE_LIB_CATDETECT} STATIC
	${SAMPLE_CATDETECT_DIR}/src/core/lib_ts_alg_body_detect.cpp
	${SAMPLE_CATDETECT_DIR}/src/core/lib_video_alg_catdetect.c 
	${SAMPLE_CATDETECT_DIR}/src/core/lib_tsalg_alg_lib.c
	${SAMPLE_CATDETECT_DIR}/src/core/lib_tsalg_alg_rsn.c
	${SAMPLE_CATDETECT_DIR}/src/core/lib_food_rsn.c
	${SAMPLE_CATDETECT_DIR}/src/utils/lib_array_diff.c
	${SAMPLE_CATDETECT_DIR}/src/utils/lib_act.c
	${SAMPLE_CATDETECT_DIR}/src/utils/lib_dirsync.c
	${SAMPLE_CATDETECT_DIR}/src/utils/lib_file_sync.c
	${SAMPLE_CATDETECT_DIR}/src/utils/lib_iot_net.c
	${SAMPLE_CATDETECT_DIR}/src/utils/qi_nv12_yuv.cpp
	${SAMPLE_CATDETECT_DIR}/src/record_file/ts_rne_record_file.c
	 
)
```

**说明：**
- 所有核心算法源文件移动到 `src/core/` 目录
- 所有工具类源文件移动到 `src/utils/` 目录
- 录像相关文件移动到 `src/record_file/` 目录
- 启用了之前被注释掉的 `qi_nv12_yuv.cpp` 和 `ts_rne_record_file.c`

---

## 新的文件结构

```
cat_detect_lib/
├── include/                    # 头文件目录
│   ├── api/                   # 对外API接口
│   ├── core/                  # 核心功能头文件
│   └── utils/                 # 工具类头文件
├── src/                       # 源文件目录
│   ├── core/                  # 核心检测算法
│   ├── utils/                 # 工具类实现
│   └── record_file/           # 录像相关
├── config/                    # 配置文件
├── README.md                  # 使用说明文档
└── 架构分析与优化方案.md     # 架构优化文档
```

---

## 验证步骤

1. 确保所有文件已正确移动到新的目录结构
2. 检查 CMakeLists.txt 中的路径是否正确
3. 执行构建，验证编译是否成功
4. 运行测试，验证功能是否正常

---

## 注意事项

- 旧的 include 路径已被注释掉，保留作为参考
- 如果需要回退，可以取消注释旧的路径并恢复源文件位置
- 建议在修改前备份原始文件

---

## 相关文档

- `README.md`: 项目使用说明
- `架构分析与优化方案.md`: 详细的架构分析和优化方案

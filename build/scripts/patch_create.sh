#!/bin/bash

BASELINE_DIR=$1
TARGET_DIR=$2
PATCH_FILE=$3

# 检查输入目录是否存在
if [ ! -d "$BASELINE_DIR" ]; then
    echo "基线目录 $BASELINE_DIR 不存在！"
    exit 1
fi

if [ ! -d "$TARGET_DIR" ]; then
    echo "目标目录 $TARGET_DIR 不存在！"
    exit 1
fi

# 检查 rdiffdir 是否安装
if ! command -v rdiffdir &> /dev/null; then
    echo "rdiff-backup 工具不存在。请先安装 rdiff-backup。"
    exit 1
fi

# 生成 diff 数据
rdiffdir signature "$BASELINE_DIR" "$BASELINE_DIR.sig"
rdiffdir delta "$BASELINE_DIR.sig" "$TARGET_DIR" "$PATCH_FILE"

# 清理中间文件
rm "$BASELINE_DIR.sig"

echo "补丁文件已生成：$PATCH_FILE"

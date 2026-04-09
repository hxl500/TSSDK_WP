#!/bin/bash

BASELINE_DIR=$1
PATCH_FILE=$2
APPLY_DIR=$3

# 检查输入目录是否存在
if [ ! -d "$BASELINE_DIR" ]; then
    echo "基线目录 $BASELINE_DIR 不存在！"
    exit 1
fi

if [ ! -f "$PATCH_FILE" ]; then
    echo "补丁文件 $PATCH_FILE 不存在！"
    exit 1
fi

# 检查 rdiffdir 是否安装
if ! command -v rdiffdir &> /dev/null; then
    echo "rdiff-backup 工具不存在。请先安装 rdiff-backup。"
    exit 1
fi

# 先从基线目录复制到应用目录
cp -a "$BASELINE_DIR" "$APPLY_DIR"

# 应用补丁文件
rdiffdir patch "$APPLY_DIR" "$PATCH_FILE"

echo "补丁文件已应用到：$APPLY_DIR"

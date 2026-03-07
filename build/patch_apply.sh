#!/bin/bash

PATCH_FILE=$1
APPLY_DIR=$2

if [ ! -f "$PATCH_FILE" ]; then
    echo "补丁文件 $PATCH_FILE 不存在！"
    exit 1
fi

# 检查 rdiffdir 是否安装
if ! command -v rdiffdir &> /dev/null; then
    echo "rdiffdir 工具不存在。请先安装 rdiffdir。"
    echo "安装命令:sudo apt-get install rdiff-backup"
    exit 1
fi

# 应用补丁文件
rdiffdir patch "$APPLY_DIR" "$PATCH_FILE"

echo "补丁文件已应用到：$APPLY_DIR"

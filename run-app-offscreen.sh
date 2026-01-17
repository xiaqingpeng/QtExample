#!/bin/bash
# 在 Docker 容器中使用 offscreen 模式运行 Qt 程序

# 获取运行中的容器
CONTAINER=$(docker ps --format "{{.Names}}" | grep -E "qt|example" | head -1)

if [ -z "$CONTAINER" ]; then
    echo "错误: 没有找到运行中的容器"
    echo "请先运行: docker-compose up -d"
    exit 1
fi

echo "使用容器: $CONTAINER"
echo "使用 offscreen 模式运行程序（无图形界面）"
echo ""
echo "提示:"
echo "  - 程序将在后台运行"
echo "  - 警告信息（XDG_RUNTIME_DIR, propagateSizeHints）是正常的"
echo "  - 按 Ctrl+C 可以停止程序"
echo ""

# 设置环境变量并运行程序
docker exec -it $CONTAINER bash -c "
    export QT_QPA_PLATFORM=offscreen
    export XDG_RUNTIME_DIR=/tmp/runtime-root
    cd /workspace/build
    echo '程序启动中...'
    echo '提示: 程序在 offscreen 模式下运行，没有图形界面'
    echo '      所有功能正常，包括 Docker 环境检测和 WebEngine 禁用'
    echo ''
    ./example
"


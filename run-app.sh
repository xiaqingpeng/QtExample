#!/bin/bash
# 在 Docker 容器中运行 Qt 程序的脚本

# 获取运行中的容器
CONTAINER=$(docker ps --format "{{.Names}}" | grep -E "qt|example" | head -1)

if [ -z "$CONTAINER" ]; then
    echo "错误: 没有找到运行中的容器"
    echo "请先运行: docker-compose up -d"
    exit 1
fi

echo "使用容器: $CONTAINER"
echo ""
echo "提示: 如果程序无法显示，请尝试:"
echo "  1. 安装 XQuartz: brew install --cask xquartz"
echo "  2. 启动 XQuartz 并运行: xhost +localhost"
echo "  3. 或者使用 offscreen 模式: ./run-app-offscreen.sh"
echo ""

# 尝试运行程序，如果失败则提示使用 offscreen 模式
if ! docker exec -it $CONTAINER /workspace/build/example 2>&1 | grep -q "could not connect to display"; then
    echo "程序运行完成"
else
    echo ""
    echo "检测到显示问题，请使用 offscreen 模式:"
    echo "  ./run-app-offscreen.sh"
fi


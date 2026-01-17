#!/bin/bash
# 快速运行脚本 - 自动检测并选择最佳模式

echo "=== Qt 程序快速运行脚本 ==="
echo ""

# 检查 XQuartz 是否运行
USE_X11=false
if pgrep -x "Xquartz" > /dev/null; then
    # 检查 xhost 是否允许连接
    if xhost | grep -q "localhost"; then
        USE_X11=true
        echo "检测到 XQuartz 已运行，使用 X11 图形界面模式"
    else
        echo "XQuartz 运行中，但未允许 localhost 连接"
        echo "运行: xhost +localhost"
    fi
else
    echo "XQuartz 未运行，使用 offscreen 模式（无图形界面）"
fi

# 获取运行中的容器
CONTAINER=$(docker ps --format "{{.Names}}" | grep -E "qt|example" | head -1)

if [ -z "$CONTAINER" ]; then
    if [ "$USE_X11" = true ]; then
        echo "使用 X11 配置启动新容器..."
        docker-compose -f docker-compose.x11.yml up -d
    else
        echo "使用 offscreen 配置启动新容器..."
        docker-compose up -d
    fi
    sleep 3
    CONTAINER=$(docker ps --format "{{.Names}}" | grep -E "qt|example" | head -1)
fi

if [ -z "$CONTAINER" ]; then
    echo "错误: 无法启动容器"
    exit 1
fi

echo "使用容器: $CONTAINER"
echo ""

# 运行程序
if [ "$USE_X11" = true ]; then
    DOCKER_DISPLAY="host.docker.internal:0"
    docker exec -it $CONTAINER bash -c "
        export DISPLAY=$DOCKER_DISPLAY
        export QT_QPA_PLATFORM=xcb
        cd /workspace/build
        echo '启动程序（X11 图形界面模式）...'
        echo 'DISPLAY='\$DISPLAY
        ./example
    "
else
    docker exec -it $CONTAINER bash -c "
        export QT_QPA_PLATFORM=offscreen
        export XDG_RUNTIME_DIR=/tmp/runtime-root
        cd /workspace/build
        echo '启动程序（offscreen 模式，无图形界面）...'
        ./example
    "
fi


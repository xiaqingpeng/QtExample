#!/bin/bash
# 在 Docker 容器中使用 X11 图形界面模式运行 Qt 程序

echo "=== Qt 程序 X11 图形界面模式 ==="
echo ""

# 检查 XQuartz 是否运行
if ! pgrep -x "Xquartz" > /dev/null; then
    echo "❌ 错误: XQuartz 未运行"
    echo "请先启动 XQuartz 应用，然后运行: xhost +localhost"
    exit 1
fi

# 检查 XQuartz 网络连接设置
echo "提示: 如果连接失败，请检查 XQuartz 偏好设置："
echo "  1. 打开 XQuartz -> 偏好设置"
echo "  2. 勾选 'Allow connections from network clients'"
echo "  3. 重启 XQuartz"
echo "  4. 运行: xhost +localhost"
echo ""

# 在 macOS 上，容器需要使用 host.docker.internal:0 或 IP 地址来连接 XQuartz
# 获取 macOS 的 IP 地址
MACOS_IP=$(ipconfig getifaddr en0 2>/dev/null || ipconfig getifaddr en1 2>/dev/null || echo "")

# 优先尝试 host.docker.internal，如果失败则使用 IP 地址
if [ -n "$MACOS_IP" ]; then
    echo "检测到 macOS IP: $MACOS_IP"
    echo "将尝试两种连接方式："
    echo "  1. host.docker.internal:0"
    echo "  2. $MACOS_IP:0 (如果方式1失败)"
    DOCKER_DISPLAY="host.docker.internal:0"
    FALLBACK_DISPLAY="$MACOS_IP:0"
else
    echo "警告: 无法获取 macOS IP 地址"
    DOCKER_DISPLAY="host.docker.internal:0"
    FALLBACK_DISPLAY=""
fi

# 获取运行中的容器
CONTAINER=$(docker ps --format "{{.Names}}" | grep -E "qt|example" | head -1)

# 如果没有运行中的容器，检查是否有已退出的容器
if [ -z "$CONTAINER" ]; then
    STOPPED_CONTAINER=$(docker ps -a --format "{{.Names}}" | grep -E "qt|example" | head -1)
    if [ -n "$STOPPED_CONTAINER" ]; then
        echo "发现已停止的容器: $STOPPED_CONTAINER，正在重新启动..."
        docker start "$STOPPED_CONTAINER"
        sleep 2
        CONTAINER=$(docker ps --format "{{.Names}}" | grep -E "qt|example" | head -1)
    fi
fi

# 如果还是没有运行中的容器，启动新容器
if [ -z "$CONTAINER" ]; then
    echo "没有运行中的容器，使用 X11 配置启动新容器..."
    docker-compose -f docker-compose.x11.yml up -d
    sleep 3
    CONTAINER=$(docker ps --format "{{.Names}}" | grep -E "qt|example" | head -1)
fi

if [ -z "$CONTAINER" ]; then
    echo "错误: 无法启动容器"
    echo "提示: 请检查 docker-compose.x11.yml 配置和 Docker 服务状态"
    exit 1
fi

echo "使用容器: $CONTAINER"
echo "使用 X11 图形界面模式"
echo ""

# 运行程序，先尝试 host.docker.internal，失败则使用 IP
if [ -n "$FALLBACK_DISPLAY" ]; then
    echo "尝试使用 host.docker.internal:0..."
    docker exec -it $CONTAINER bash -c "
        export DISPLAY=$DOCKER_DISPLAY
        export QT_QPA_PLATFORM=xcb
        export XDG_RUNTIME_DIR=/tmp/runtime-root
        cd /workspace/build
        echo '启动程序（X11 图形界面模式）...'
        echo 'DISPLAY='\$DISPLAY
        if ./example 2>&1 | grep -q 'could not connect to display'; then
            echo ''
            echo 'host.docker.internal 连接失败，尝试使用 IP 地址...'
            export DISPLAY=$FALLBACK_DISPLAY
            echo 'DISPLAY='\$DISPLAY
            ./example
        else
            ./example
        fi
    " || {
        echo ""
        echo "尝试使用 IP 地址连接..."
        docker exec -it $CONTAINER bash -c "
            export DISPLAY=$FALLBACK_DISPLAY
            export QT_QPA_PLATFORM=xcb
            export XDG_RUNTIME_DIR=/tmp/runtime-root
            cd /workspace/build
            echo '启动程序（X11 图形界面模式，使用 IP）...'
            echo 'DISPLAY='\$DISPLAY
            ./example
        "
    }
else
    docker exec -it $CONTAINER bash -c "
        export DISPLAY=$DOCKER_DISPLAY
        export QT_QPA_PLATFORM=xcb
        export XDG_RUNTIME_DIR=/tmp/runtime-root
        cd /workspace/build
        echo '启动程序（X11 图形界面模式）...'
        echo 'DISPLAY='\$DISPLAY
        ./example
    "
fi


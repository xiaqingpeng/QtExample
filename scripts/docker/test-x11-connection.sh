#!/bin/bash
# 测试 X11 连接

echo "=== 测试 X11 连接 ==="
echo ""

# 检查 XQuartz
if ! pgrep -x "Xquartz" > /dev/null; then
    echo "❌ XQuartz 未运行"
    echo "请启动 XQuartz 应用"
    exit 1
fi
echo "✓ XQuartz 正在运行"

# 检查 xhost
if xhost | grep -q "localhost"; then
    echo "✓ xhost 已允许 localhost 连接"
else
    echo "⚠ xhost 未允许 localhost，运行: xhost +localhost"
fi

# 获取容器
CONTAINER=$(docker ps --format "{{.Names}}" | grep -E "qt|example" | head -1)
if [ -z "$CONTAINER" ]; then
    echo "❌ 没有运行中的容器"
    exit 1
fi

echo "✓ 使用容器: $CONTAINER"
echo ""

# 测试不同的 DISPLAY 设置
echo "测试连接方式："
echo ""

# 方式1: host.docker.internal:0
echo "1. 测试 host.docker.internal:0"
docker exec $CONTAINER bash -c "export DISPLAY=host.docker.internal:0 && xdpyinfo -display host.docker.internal:0 >/dev/null 2>&1" && echo "   ✓ 连接成功" || echo "   ✗ 连接失败"

# 方式2: 使用 IP 地址
MACOS_IP=$(ipconfig getifaddr en0 2>/dev/null || ipconfig getifaddr en1 2>/dev/null)
if [ -n "$MACOS_IP" ]; then
    echo ""
    echo "2. 测试 $MACOS_IP:0"
    docker exec $CONTAINER bash -c "export DISPLAY=$MACOS_IP:0 && xdpyinfo -display $MACOS_IP:0 >/dev/null 2>&1" && echo "   ✓ 连接成功" || echo "   ✗ 连接失败"
fi

# 方式3: 检查 X11 socket
echo ""
echo "3. 检查 X11 socket 挂载"
docker exec $CONTAINER bash -c "test -S /tmp/.X11-unix/X0 && echo '   ✓ X11 socket 已挂载' || echo '   ✗ X11 socket 未挂载'"

echo ""
echo "建议："
echo "  如果所有方式都失败，请确保："
echo "  1. XQuartz 偏好设置中启用 'Allow connections from network clients'"
echo "  2. 重启 XQuartz"
echo "  3. 运行: xhost +localhost"


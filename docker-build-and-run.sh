#!/bin/bash
# Docker 环境下的构建和运行脚本

set -e

echo "🐳 Docker Qt Linux 构建和运行..."
echo ""

# 检查 Docker
if ! docker info &> /dev/null; then
    echo "❌ Docker 未运行，请启动 Docker Desktop"
    exit 1
fi

echo "✅ Docker 已就绪"
echo ""

# 检查文件共享
CURRENT_PATH=$(pwd)
if ! docker run --rm -v "$CURRENT_PATH:/test" alpine ls /test &> /dev/null 2>&1; then
    echo "❌ 文件共享未配置"
    echo ""
    echo "请先配置文件共享："
    echo "   1. 打开 Docker Desktop"
    echo "   2. Settings → Resources → File Sharing"
    echo "   3. 添加路径: $CURRENT_PATH"
    echo "   4. 点击 Apply & Restart"
    echo ""
    echo "详细步骤: ./fix-docker-sharing.sh"
    exit 1
fi

echo "✅ 文件共享已配置"
echo ""

# 构建镜像（如果需要）
echo "🔨 检查 Docker 镜像..."
if ! docker images | grep -q "example-qt-dev"; then
    echo "构建 Docker 镜像..."
    docker-compose build qt-dev
else
    echo "✅ Docker 镜像已存在"
    echo "💡 如果遇到 WebEngine 错误，请重新构建："
    echo "   docker-compose build --no-cache qt-dev"
fi

# 编译项目
echo ""
echo "⚙️  编译 Linux 版本..."
docker-compose run --rm qt-dev bash -c "
    echo '进入容器，开始编译...'
    mkdir -p build
    cd build
    if [ ! -f CMakeCache.txt ]; then
        echo '运行 CMake...'
        cmake ..
    else
        echo 'CMake 缓存已存在，跳过配置'
    fi
    echo '编译项目...'
    make -j\$(nproc)
    echo ''
    echo '✅ 编译完成！'
    echo ''
    echo '可执行文件位置: build/example'
    echo ''
    if [ -f example ]; then
        echo '✅ 找到可执行文件: build/example'
        ls -lh example
    else
        echo '⚠️  未找到可执行文件，检查编译输出...'
        ls -la
    fi
"

echo ""
echo "📦 构建完成！"
echo ""
echo "运行程序："
echo "  方式 1: 在容器中运行"
echo "    docker-compose run --rm qt-dev bash"
echo "    cd /workspace/build && ./example"
echo ""
echo "  方式 2: 直接运行（无界面）"
echo "    docker-compose run --rm qt-dev bash -c 'cd /workspace/build && QT_QPA_PLATFORM=offscreen ./example'"
echo ""
echo "  方式 3: 带 GUI 运行（需要 X11）"
echo "    xhost +localhost"
echo "    docker-compose run --rm qt-dev bash -c 'cd /workspace/build && ./example'"

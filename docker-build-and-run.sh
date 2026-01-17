#!/bin/bash
# Docker 环境下的构建和运行脚本
# 整合了 build-docker-linux.sh 的功能

set -e

echo "🐳 Docker Qt Linux 构建和运行..."
echo ""

# 检查 Docker
if ! command -v docker &> /dev/null; then
    echo "❌ 未找到 Docker，请先安装 Docker Desktop"
    echo "   macOS: https://www.docker.com/products/docker-desktop/"
    echo "   Linux: https://docs.docker.com/engine/install/"
    exit 1
fi

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

# 构建镜像
echo "🔨 构建 Docker 镜像..."
docker-compose build qt-dev

# 编译项目
echo ""
echo "⚙️  编译 Linux 版本..."
docker-compose run --rm qt-dev bash -c "
    cd /workspace
    mkdir -p build
    cd build
    if [ ! -f CMakeCache.txt ]; then
        echo '运行 CMake...'
        cmake .. || (echo '❌ CMake 配置失败' && exit 1)
    else
        echo 'CMake 缓存已存在，跳过配置'
    fi
    echo '编译项目...'
    make -j\$(nproc) || (echo '❌ 编译失败' && exit 1)
    echo ''
    echo '✅ 编译完成！'
    echo ''
    if [ -f example ]; then
        echo '✅ 可执行文件: build/example'
        ls -lh example
        echo ''
        echo '📋 检查依赖库：'
        ldd example | grep -i qt || echo '未找到 Qt 依赖（可能使用静态链接）'
    else
        echo '❌ 未找到可执行文件 example'
        exit 1
    fi
"

echo ""
echo "📦 构建完成！"
echo ""
echo "运行程序："
echo "  方式 1: 快速运行（自动检测模式）"
echo "    ./quick-run.sh"
echo ""
echo "  方式 2: X11 图形界面模式"
echo "    ./run-app-x11.sh"
echo ""
echo "  方式 3: Offscreen 模式（无图形界面）"
echo "    ./run-app-offscreen.sh"
echo ""
echo "  方式 4: 手动运行"
echo "    docker-compose run --rm qt-dev bash"
echo "    cd /workspace/build && ./example"

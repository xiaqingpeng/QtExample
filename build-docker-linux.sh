#!/bin/bash
# Docker Qt Linux 桌面版本构建脚本

set -e

echo "🐳 Docker Qt Linux 构建..."

# 检查 Docker
if ! command -v docker &> /dev/null; then
    echo "❌ 未找到 Docker，请先安装 Docker Desktop"
    echo "   macOS: https://www.docker.com/products/docker-desktop/"
    echo "   Linux: https://docs.docker.com/engine/install/"
    exit 1
fi

# 检查 Docker 是否运行
if ! docker info &> /dev/null; then
    echo "❌ Docker 未运行，请启动 Docker Desktop"
    exit 1
fi

echo "✅ Docker 已就绪"

# 构建镜像
echo "🔨 构建 Docker 镜像..."
docker-compose build qt-dev

# 编译项目（在容器里执行 CMake 构建）
echo "⚙️  编译 Linux 版本..."
docker-compose run --rm qt-dev bash -c "
    cd /workspace
    # 清理旧的构建产物
    rm -rf build/*
    mkdir -p build
    cd build
    # 重新配置和编译
    cmake .. || (echo '❌ CMake 配置失败' && exit 1)
    make -j\$(nproc) || (echo '❌ 编译失败' && exit 1)
    echo ''
    echo '✅ Linux 版本构建完成！'
    echo '编译产物目录: /workspace/build/'
    if [ -f example ]; then
        echo '✅ 可执行文件: build/example'
        ls -lh example
        # 检查依赖库
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
echo "运行 Qt 程序（需要 X11）："
echo "  1. 安装 XQuartz: brew install --cask xquartz"
echo "  2. 启动 XQuartz 并运行: xhost +localhost"
echo "  3. 运行容器并执行程序："
echo "     docker-compose run --rm qt-dev bash"
echo "     cd /workspace/build && ./example"
echo ""
echo "无界面运行（offscreen 模式）："
echo "     docker-compose run --rm qt-dev bash"
echo "     cd /workspace/build && QT_QPA_PLATFORM=offscreen ./example"


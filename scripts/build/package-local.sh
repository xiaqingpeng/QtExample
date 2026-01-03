#!/bin/bash

# 本地打包脚本 - 根据 CI/CD 流程打包应用
# 支持 macOS、Linux 和 Windows 平台

set -e

# 配置参数
QT_VERSION="6.6.1"
BUILD_DIR="build-release"
CMAKE_BUILD_TYPE="Release"
VERSION=${1:-"v1.0.0"}

# 获取操作系统类型
OS_TYPE=$(uname -s)

# 根据操作系统设置参数
if [[ "$OS_TYPE" == "Darwin" ]]; then
    PLATFORM="macOS"
    ARCHIVE_EXT="zip"
    QT_ARCH="clang_64"
    EXECUTABLE="example.app"
elif [[ "$OS_TYPE" == "Linux" ]]; then
    PLATFORM="Linux"
    ARCHIVE_EXT="tar.gz"
    QT_ARCH="gcc_64"
    EXECUTABLE="example"
else
    echo "错误：不支持的操作系统: $OS_TYPE"
    echo "此脚本仅支持 macOS 和 Linux"
    exit 1
fi

ARCHIVE_NAME="example-${VERSION}-${PLATFORM}-qt${QT_VERSION}"

echo "========================================"
echo "开始打包应用"
echo "========================================"
echo "平台: $PLATFORM"
echo "Qt 版本: $QT_VERSION"
echo "Qt 架构: $QT_ARCH"
echo "构建类型: $CMAKE_BUILD_TYPE"
echo "版本: $VERSION"
echo "========================================"

# 步骤 1: 清理旧的构建目录
echo "\n[1/6] 清理旧的构建目录..."
rm -rf "$BUILD_DIR"

# 步骤 2: 配置 CMake
echo "\n[2/6] 配置 CMake..."
if [[ "$OS_TYPE" == "Darwin" ]]; then
    cmake -B "$BUILD_DIR" -G "Ninja" \
        -DCMAKE_BUILD_TYPE="$CMAKE_BUILD_TYPE" \
        -DCMAKE_PREFIX_PATH="$Qt6_DIR" \
        -DCMAKE_CXX_STANDARD=17 \
        -DCMAKE_OSX_DEPLOYMENT_TARGET="10.15"
else
    cmake -B "$BUILD_DIR" -G "Ninja" \
        -DCMAKE_BUILD_TYPE="$CMAKE_BUILD_TYPE" \
        -DCMAKE_PREFIX_PATH="$Qt6_DIR" \
        -DCMAKE_CXX_STANDARD=17
fi

# 步骤 3: 构建项目
echo "\n[3/6] 构建项目..."
cmake --build "$BUILD_DIR" --config "$CMAKE_BUILD_TYPE" --parallel

# 步骤 4: 清理旧的打包目录
echo "\n[4/6] 清理旧的打包目录..."
rm -rf "$ARCHIVE_NAME"

# 步骤 5: 打包应用
echo "\n[5/6] 打包应用..."
mkdir -p "$ARCHIVE_NAME"

if [[ "$OS_TYPE" == "Darwin" ]]; then
    # macOS 打包
    echo "复制 example.app..."
    cp -R "$BUILD_DIR/example.app" "$ARCHIVE_NAME/"
    
    echo "复制 images 文件夹..."
    cp -r images "$ARCHIVE_NAME/"
    
    echo "创建 zip 压缩包..."
    zip -r "$ARCHIVE_NAME.zip" "$ARCHIVE_NAME"
    
elif [[ "$OS_TYPE" == "Linux" ]]; then
    # Linux 打包
    echo "复制可执行文件..."
    cp "$BUILD_DIR/example" "$ARCHIVE_NAME/"
    
    echo "复制共享库..."
    cp "$BUILD_DIR"/*.so* "$ARCHIVE_NAME/" 2>/dev/null || true
    
    echo "复制 images 文件夹..."
    cp -r images "$ARCHIVE_NAME/"
    
    echo "创建 tar.gz 压缩包..."
    tar -czf "$ARCHIVE_NAME.tar.gz" "$ARCHIVE_NAME"
fi

# 步骤 6: 完成
echo "\n[6/6] 打包完成！"
echo "========================================"
echo "打包产物:"
if [[ "$OS_TYPE" == "Darwin" ]]; then
    echo "  - $ARCHIVE_NAME.zip"
    ls -lh "$ARCHIVE_NAME.zip"
elif [[ "$OS_TYPE" == "Linux" ]]; then
    echo "  - $ARCHIVE_NAME.tar.gz"
    ls -lh "$ARCHIVE_NAME.tar.gz"
fi
echo "========================================"
echo ""
echo "提示:"
echo "  - 使用不同的版本号: ./package-local.sh v1.0.1"
echo "  - 清理构建产物: rm -rf $BUILD_DIR $ARCHIVE_NAME*"
echo "========================================"
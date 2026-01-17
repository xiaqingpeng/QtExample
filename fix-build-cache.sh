#!/bin/bash
# 修复 CMake 缓存路径问题

echo "🔧 修复 CMake 缓存路径问题..."
echo ""

CURRENT_PATH=$(pwd)
BUILD_DIR="$CURRENT_PATH/build"

if [ -d "$BUILD_DIR" ]; then
    echo "📁 清理 build 目录中的 CMake 缓存..."
    
    # 备份旧的缓存（如果需要）
    if [ -f "$BUILD_DIR/CMakeCache.txt" ]; then
        echo "  发现 CMakeCache.txt，将清理..."
        rm -f "$BUILD_DIR/CMakeCache.txt"
    fi
    
    # 清理 CMakeFiles
    if [ -d "$BUILD_DIR/CMakeFiles" ]; then
        echo "  清理 CMakeFiles 目录..."
        rm -rf "$BUILD_DIR/CMakeFiles"
    fi
    
    echo "✅ 清理完成"
    echo ""
    echo "💡 现在可以在容器内重新编译："
    echo "   docker-compose run --rm qt-dev bash"
    echo "   cd /workspace/build && cmake .. && make -j\$(nproc)"
    echo ""
    echo "或者直接运行构建脚本："
    echo "   ./docker-build-and-run.sh"
else
    echo "✅ build 目录不存在，无需清理"
fi

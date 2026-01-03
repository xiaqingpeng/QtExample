#!/bin/bash

# ========================================
# 本地构建测试脚本
# ========================================

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}    本地构建测试${NC}"
echo -e "${BLUE}========================================${NC}"

# 检查是否在正确的目录
if [ ! -f "CMakeLists.txt" ]; then
    echo -e "${RED}✗ 错误: 未找到CMakeLists.txt文件${NC}"
    echo -e "${YELLOW}请在项目根目录运行此脚本${NC}"
    exit 1
fi

# 清理之前的构建
echo -e "${YELLOW}[1/4] 清理构建目录...${NC}"
if [ -d "build" ]; then
    rm -rf build
    echo -e "${GREEN}✓ 构建目录已清理${NC}"
else
    echo -e "${CYAN}构建目录不存在，跳过清理${NC}"
fi

# 创建构建目录
echo -e "${YELLOW}[2/4] 创建构建目录...${NC}"
mkdir -p build
cd build

# 配置CMake
echo -e "${YELLOW}[3/4] 配置CMake...${NC}"
if cmake .. -DCMAKE_BUILD_TYPE=Release; then
    echo -e "${GREEN}✓ CMake配置成功${NC}"
else
    echo -e "${RED}✗ CMake配置失败${NC}"
    exit 1
fi

# 构建项目
echo -e "${YELLOW}[4/4] 构建项目...${NC}"
if cmake --build . --config Release --parallel; then
    echo -e "${GREEN}✓ 项目构建成功${NC}"
else
    echo -e "${RED}✗ 项目构建失败${NC}"
    exit 1
fi

# 检查构建结果
echo ""
echo -e "${CYAN}构建结果检查:${NC}"
echo -e "${CYAN}构建目录内容:${NC}"
ls -la

# 检查可执行文件
if [[ "$OSTYPE" == "darwin"* ]]; then
    # macOS
    if [ -d "example.app" ]; then
        echo -e "${GREEN}✓ 找到macOS应用包: example.app${NC}"
        echo -e "${CYAN}应用包内容:${NC}"
        ls -la example.app/Contents/MacOS/
    else
        echo -e "${RED}✗ 未找到macOS应用包${NC}"
    fi
elif [[ "$OSTYPE" == "msys" ]] || [[ "$OSTYPE" == "win32" ]]; then
    # Windows
    if [ -f "example.exe" ]; then
        echo -e "${GREEN}✓ 找到Windows可执行文件: example.exe${NC}"
    else
        echo -e "${RED}✗ 未找到Windows可执行文件${NC}"
    fi
else
    # Linux
    if [ -f "example" ]; then
        echo -e "${GREEN}✓ 找到Linux可执行文件: example${NC}"
        echo -e "${CYAN}文件信息:${NC}"
        file example
        ls -la example
    else
        echo -e "${RED}✗ 未找到Linux可执行文件${NC}"
    fi
fi

# 检查共享库
echo ""
echo -e "${CYAN}共享库检查:${NC}"
if [[ "$OSTYPE" == "darwin"* ]]; then
    echo "macOS应用包会自动处理依赖"
elif [[ "$OSTYPE" == "linux-gnu"* ]]; then
    SO_FILES=$(find . -name "*.so*" 2>/dev/null)
    if [ -n "$SO_FILES" ]; then
        echo -e "${GREEN}✓ 找到共享库:${NC}"
        echo "$SO_FILES"
    else
        echo -e "${YELLOW}⚠️  未找到共享库文件${NC}"
    fi
fi

cd ..

echo ""
echo -e "${BLUE}========================================${NC}"
echo -e "${GREEN}✓ 构建测试完成！${NC}"
echo -e "${BLUE}========================================${NC}"

# 提供下一步建议
echo ""
echo -e "${CYAN}下一步操作建议:${NC}"
if [[ "$OSTYPE" == "darwin"* ]]; then
    echo -e "  1. 测试应用程序: ${YELLOW}open ./build/example.app${NC}"
    echo -e "  2. 跨平台构建: ${YELLOW}./build-all-platforms.sh --no-upload${NC}"
elif [[ "$OSTYPE" == "linux-gnu"* ]]; then
    echo -e "  1. 测试应用程序: ${YELLOW}./build/example${NC}"
    echo -e "  2. 跨平台构建: ${YELLOW}./build-all-platforms.sh --no-upload${NC}"
elif [[ "$OSTYPE" == "msys" ]] || [[ "$OSTYPE" == "win32" ]]; then
    echo -e "  1. 测试应用程序: ${YELLOW}./build/example.exe${NC}"
    echo -e "  2. Windows打包: ${YELLOW}.\\package-windows.ps1${NC}"
    echo -e "  3. 修复DLL问题: ${YELLOW}.\\fix-all-windows-dll.ps1${NC}"
else
    echo -e "  1. 测试应用程序运行"
    echo -e "  2. 跨平台构建: ${YELLOW}./build-all-platforms.sh --no-upload${NC}"
fi

echo -e "  3. 完整发布流程: ${YELLOW}./complete-release.sh${NC}"
echo -e "  4. 查看所有选项: ${YELLOW}./show-build-options.sh${NC}"

echo ""
echo -e "${YELLOW}Windows用户注意:${NC}"
echo -e "  如果遇到DLL缺失错误，请运行:"
echo -e "  ${CYAN}.\\fix-all-windows-dll.ps1${NC}      # 一键修复所有DLL问题"
echo -e "  ${CYAN}.\\diagnose-windows-dll.ps1${NC}     # 诊断DLL问题"
echo -e "  ${CYAN}.\\complete-release-windows.ps1${NC} # Windows完整发布流程"
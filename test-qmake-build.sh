#!/bin/bash

# ========================================
# qmake构建测试脚本
# ========================================

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}    qmake构建测试${NC}"
echo -e "${BLUE}========================================${NC}"

# 清理旧的构建文件
echo -e "${YELLOW}清理旧的构建文件...${NC}"
make clean 2>/dev/null || true
rm -rf build-qmake
rm -f Makefile*
find . -name "*.o" -delete 2>/dev/null || true
find . -name "moc_*" -delete 2>/dev/null || true
find . -name "ui_*" -delete 2>/dev/null || true

# 创建构建目录
mkdir -p build-qmake
cd build-qmake

# 运行qmake
echo -e "${YELLOW}运行qmake配置...${NC}"
if qmake ../example.pro; then
    echo -e "${GREEN}✓ qmake配置成功${NC}"
else
    echo -e "${RED}✗ qmake配置失败${NC}"
    exit 1
fi

# 编译
echo -e "${YELLOW}开始编译...${NC}"
echo -e "${BLUE}注意: macOS版本兼容性警告是正常的，不影响功能${NC}"

if make -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4); then
    echo -e "${GREEN}✓ 编译成功${NC}"
else
    echo -e "${RED}✗ 编译失败${NC}"
    echo -e "${YELLOW}尝试单线程编译以获取更详细的错误信息...${NC}"
    make
    exit 1
fi

echo -e "${GREEN}========================================${NC}"
echo -e "${GREEN}✓ qmake构建完成！${NC}"
echo -e "${GREEN}========================================${NC}"

# 显示构建结果
echo -e "${BLUE}构建结果:${NC}"
find . -name "*.app" -o -name "*.exe" -o -name "example" | head -5

# 检查应用程序是否可以运行
if [ -d "app/example.app" ]; then
    echo -e "${CYAN}macOS应用包: app/example.app${NC}"
    echo -e "${CYAN}可执行文件: app/example.app/Contents/MacOS/example${NC}"
    
    # 检查可执行文件
    if [ -f "app/example.app/Contents/MacOS/example" ]; then
        echo -e "${GREEN}✓ 可执行文件存在${NC}"
        
        # 显示文件信息
        echo -e "${BLUE}文件信息:${NC}"
        ls -la "app/example.app/Contents/MacOS/example"
        
        echo -e "${YELLOW}要运行应用程序，请执行:${NC}"
        echo -e "${CYAN}open app/example.app${NC}"
        echo -e "${CYAN}或者: ./app/example.app/Contents/MacOS/example${NC}"
    else
        echo -e "${RED}✗ 可执行文件不存在${NC}"
    fi
elif [ -f "app/example" ]; then
    echo -e "${CYAN}可执行文件: app/example${NC}"
    echo -e "${GREEN}✓ 可执行文件存在${NC}"
    
    # 显示文件信息
    echo -e "${BLUE}文件信息:${NC}"
    ls -la "app/example"
    
    echo -e "${YELLOW}要运行应用程序，请执行:${NC}"
    echo -e "${CYAN}./app/example${NC}"
else
    echo -e "${RED}✗ 未找到可执行文件${NC}"
fi
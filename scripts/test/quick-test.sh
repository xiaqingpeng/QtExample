#!/bin/bash

# ========================================
# 快速测试脚本 - 验证所有脚本可执行
# ========================================

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}    快速脚本测试${NC}"
echo -e "${BLUE}========================================${NC}"

# 要测试的脚本列表
SCRIPTS=(
    "recreate-tag.sh"
    "check-pipeline.sh"
    "test-build.sh"
    "upload-release.sh"
    "complete-release.sh"
    "create-lite-tag.sh"
)

echo -e "${CYAN}检查脚本文件...${NC}"

all_good=true
for script in "${SCRIPTS[@]}"; do
    if [ -f "$script" ]; then
        if [ -x "$script" ]; then
            echo -e "  ${GREEN}✓${NC} $script (存在且可执行)"
        else
            echo -e "  ${YELLOW}⚠️${NC} $script (存在但不可执行)"
            chmod +x "$script"
            echo -e "    ${GREEN}✓${NC} 已添加执行权限"
        fi
    else
        echo -e "  ${RED}✗${NC} $script (不存在)"
        all_good=false
    fi
done

echo ""

if [ "$all_good" = true ]; then
    echo -e "${GREEN}✓ 所有脚本文件检查通过${NC}"
else
    echo -e "${RED}✗ 部分脚本文件缺失${NC}"
    exit 1
fi

# 检查语法
echo -e "${CYAN}检查脚本语法...${NC}"

for script in "${SCRIPTS[@]}"; do
    if bash -n "$script" 2>/dev/null; then
        echo -e "  ${GREEN}✓${NC} $script (语法正确)"
    else
        echo -e "  ${RED}✗${NC} $script (语法错误)"
        all_good=false
    fi
done

echo ""

if [ "$all_good" = true ]; then
    echo -e "${GREEN}✓ 所有脚本语法检查通过${NC}"
else
    echo -e "${RED}✗ 部分脚本存在语法错误${NC}"
    exit 1
fi

# 检查依赖工具
echo -e "${CYAN}检查依赖工具...${NC}"

TOOLS=(
    "git:Git版本控制"
    "cmake:CMake构建系统"
    "curl:网络请求工具"
)

OPTIONAL_TOOLS=(
    "gh:GitHub CLI (推荐)"
    "jq:JSON处理工具 (推荐)"
)

for tool_info in "${TOOLS[@]}"; do
    IFS=':' read -r tool desc <<< "$tool_info"
    if command -v "$tool" &> /dev/null; then
        echo -e "  ${GREEN}✓${NC} $tool ($desc)"
    else
        echo -e "  ${RED}✗${NC} $tool ($desc) - 必需"
        all_good=false
    fi
done

for tool_info in "${OPTIONAL_TOOLS[@]}"; do
    IFS=':' read -r tool desc <<< "$tool_info"
    if command -v "$tool" &> /dev/null; then
        echo -e "  ${GREEN}✓${NC} $tool ($desc)"
    else
        echo -e "  ${YELLOW}⚠️${NC} $tool ($desc) - 可选"
    fi
done

echo ""

# 检查Git仓库状态
echo -e "${CYAN}检查Git仓库状态...${NC}"

if git rev-parse --git-dir > /dev/null 2>&1; then
    echo -e "  ${GREEN}✓${NC} 当前目录是Git仓库"
    
    # 检查远程仓库
    if git remote get-url origin > /dev/null 2>&1; then
        remote_url=$(git remote get-url origin)
        echo -e "  ${GREEN}✓${NC} 远程仓库已配置: $remote_url"
        
        if [[ "$remote_url" == *"github.com"* ]]; then
            echo -e "  ${GREEN}✓${NC} 这是一个GitHub仓库"
        else
            echo -e "  ${YELLOW}⚠️${NC} 不是GitHub仓库，部分功能可能不可用"
        fi
    else
        echo -e "  ${YELLOW}⚠️${NC} 未配置远程仓库"
    fi
    
    # 检查当前分支
    current_branch=$(git branch --show-current)
    echo -e "  ${CYAN}当前分支:${NC} $current_branch"
    
    # 检查是否有未提交的更改
    if git diff --quiet && git diff --staged --quiet; then
        echo -e "  ${GREEN}✓${NC} 工作目录干净"
    else
        echo -e "  ${YELLOW}⚠️${NC} 有未提交的更改"
    fi
else
    echo -e "  ${RED}✗${NC} 当前目录不是Git仓库"
    all_good=false
fi

echo ""

# 检查项目文件
echo -e "${CYAN}检查项目文件...${NC}"

PROJECT_FILES=(
    "CMakeLists.txt:CMake配置文件"
    "main.cpp:主程序文件"
    "mainuiwindow.h:主窗口头文件"
    "mainuiwindow.cpp:主窗口实现文件"
)

for file_info in "${PROJECT_FILES[@]}"; do
    IFS=':' read -r file desc <<< "$file_info"
    if [ -f "$file" ]; then
        echo -e "  ${GREEN}✓${NC} $file ($desc)"
    else
        echo -e "  ${YELLOW}⚠️${NC} $file ($desc) - 未找到"
    fi
done

# 检查GitHub Actions配置
if [ -f ".github/workflows/release.yml" ]; then
    echo -e "  ${GREEN}✓${NC} GitHub Actions配置文件存在"
else
    echo -e "  ${YELLOW}⚠️${NC} GitHub Actions配置文件不存在"
fi

echo ""
echo -e "${BLUE}========================================${NC}"

if [ "$all_good" = true ]; then
    echo -e "${GREEN}✓ 所有检查通过！环境已准备就绪${NC}"
    echo ""
    echo -e "${CYAN}可以使用的命令:${NC}"
    echo -e "  ${YELLOW}./complete-release.sh${NC} - 完整发布流程"
    echo -e "  ${YELLOW}./test-build.sh${NC} - 本地构建测试"
    echo -e "  ${YELLOW}./recreate-tag.sh${NC} - 重建标签"
    echo -e "  ${YELLOW}./create-lite-tag.sh${NC} - 创建轻量级标签 (避免超时)"
    echo -e "  ${YELLOW}./check-pipeline.sh${NC} - 检测流水线"
else
    echo -e "${RED}✗ 部分检查失败，请解决上述问题${NC}"
    exit 1
fi

echo -e "${BLUE}========================================${NC}"
#!/bin/bash

# ========================================
# 完整的发布流程脚本
# ========================================

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# 配置
TAG_NAME="v1.0.0"
REMOTE_NAME="origin"

echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}    完整发布流程${NC}"
echo -e "${BLUE}========================================${NC}"

# 显示当前配置
echo -e "${CYAN}配置信息:${NC}"
echo -e "  标签名称: ${TAG_NAME}"
echo -e "  远程仓库: ${REMOTE_NAME}"
echo ""

# 步骤选择菜单
echo -e "${YELLOW}请选择要执行的步骤:${NC}"
echo -e "  ${CYAN}1.${NC} 本地构建测试"
echo -e "  ${CYAN}2.${NC} 重建标签并推送"
echo -e "  ${CYAN}3.${NC} 检测流水线状态"
echo -e "  ${CYAN}4.${NC} 手动上传Release资产"
echo -e "  ${CYAN}5.${NC} 完整流程 (1→2→3)"
echo -e "  ${CYAN}6.${NC} 创建轻量级标签 (避免超时)"
echo -e "  ${CYAN}0.${NC} 退出"
echo ""

read -p "请输入选择 [1-6,0]: " choice

case $choice in
    1)
        echo -e "${YELLOW}执行本地构建测试...${NC}"
        if [ -f "./test-build.sh" ]; then
            ./test-build.sh
        else
            echo -e "${RED}✗ 错误: test-build.sh 不存在${NC}"
            exit 1
        fi
        ;;
    2)
        echo -e "${YELLOW}执行标签重建...${NC}"
        if [ -f "./recreate-tag.sh" ]; then
            ./recreate-tag.sh
        else
            echo -e "${RED}✗ 错误: recreate-tag.sh 不存在${NC}"
            exit 1
        fi
        ;;
    3)
        echo -e "${YELLOW}检测流水线状态...${NC}"
        if [ -f "./check-pipeline.sh" ]; then
            ./check-pipeline.sh
        else
            echo -e "${RED}✗ 错误: check-pipeline.sh 不存在${NC}"
            exit 1
        fi
        ;;
    4)
        echo -e "${YELLOW}手动上传Release资产...${NC}"
        echo ""
        echo -e "${CYAN}可用的构建文件:${NC}"
        
        # 查找可能的构建文件
        BUILD_FILES=()
        if [ -d "build" ]; then
            # macOS .app 包
            if [ -d "build/example.app" ]; then
                echo -e "  ${GREEN}macOS:${NC} build/example.app (需要先打包)"
            fi
            
            # Linux 可执行文件
            if [ -f "build/example" ]; then
                echo -e "  ${GREEN}Linux:${NC} build/example (需要先打包)"
            fi
            
            # Windows 可执行文件
            if [ -f "build/example.exe" ]; then
                echo -e "  ${GREEN}Windows:${NC} build/example.exe (需要先打包)"
            fi
        fi
        
        # 查找已打包的文件
        ARCHIVE_FILES=$(find . -maxdepth 1 -name "example-v*.zip" -o -name "example-v*.tar.gz" 2>/dev/null)
        if [ -n "$ARCHIVE_FILES" ]; then
            echo -e "${CYAN}已打包的文件:${NC}"
            for file in $ARCHIVE_FILES; do
                echo -e "  ${GREEN}✓${NC} $file"
                BUILD_FILES+=("$file")
            done
        fi
        
        if [ ${#BUILD_FILES[@]} -eq 0 ]; then
            echo -e "${YELLOW}⚠️  未找到可上传的文件${NC}"
            echo -e "${YELLOW}请先运行构建和打包步骤${NC}"
            exit 1
        fi
        
        echo ""
        echo -e "${YELLOW}选择要上传的文件:${NC}"
        for i in "${!BUILD_FILES[@]}"; do
            echo -e "  $((i+1)). ${BUILD_FILES[i]}"
        done
        
        read -p "请输入文件编号: " file_choice
        if [[ "$file_choice" =~ ^[0-9]+$ ]] && [ "$file_choice" -ge 1 ] && [ "$file_choice" -le ${#BUILD_FILES[@]} ]; then
            selected_file="${BUILD_FILES[$((file_choice-1))]}"
            echo -e "${CYAN}选择的文件: ${selected_file}${NC}"
            
            if [ -f "./upload-release.sh" ]; then
                ./upload-release.sh "$selected_file" "$TAG_NAME"
            else
                echo -e "${RED}✗ 错误: upload-release.sh 不存在${NC}"
                exit 1
            fi
        else
            echo -e "${RED}✗ 无效的选择${NC}"
            exit 1
        fi
        ;;
    5)
        echo -e "${YELLOW}执行完整发布流程...${NC}"
        echo ""
        
        # 步骤1: 本地构建测试
        echo -e "${BLUE}[1/3] 本地构建测试${NC}"
        if [ -f "./test-build.sh" ]; then
            if ./test-build.sh; then
                echo -e "${GREEN}✓ 本地构建测试成功${NC}"
            else
                echo -e "${RED}✗ 本地构建测试失败${NC}"
                exit 1
            fi
        else
            echo -e "${RED}✗ 错误: test-build.sh 不存在${NC}"
            exit 1
        fi
        
        echo ""
        read -p "构建测试成功，是否继续推送标签? [y/N]: " continue_push
        if [[ ! "$continue_push" =~ ^[Yy]$ ]]; then
            echo -e "${YELLOW}用户取消操作${NC}"
            exit 0
        fi
        
        # 步骤2: 重建标签并推送
        echo ""
        echo -e "${BLUE}[2/3] 重建标签并推送${NC}"
        if [ -f "./recreate-tag.sh" ]; then
            if ./recreate-tag.sh; then
                echo -e "${GREEN}✓ 标签重建和推送成功${NC}"
            else
                echo -e "${RED}✗ 标签重建和推送失败${NC}"
                exit 1
            fi
        else
            echo -e "${RED}✗ 错误: recreate-tag.sh 不存在${NC}"
            exit 1
        fi
        
        # 步骤3: 检测流水线状态
        echo ""
        echo -e "${BLUE}[3/3] 检测流水线状态${NC}"
        if [ -f "./check-pipeline.sh" ]; then
            ./check-pipeline.sh
        else
            echo -e "${RED}✗ 错误: check-pipeline.sh 不存在${NC}"
            exit 1
        fi
        ;;
    6)
        echo -e "${YELLOW}创建轻量级标签...${NC}"
        if [ -f "./create-lite-tag.sh" ]; then
            ./create-lite-tag.sh
        else
            echo -e "${RED}✗ 错误: create-lite-tag.sh 不存在${NC}"
            exit 1
        fi
        ;;
    0)
        echo -e "${YELLOW}退出${NC}"
        exit 0
        ;;
    *)
        echo -e "${RED}✗ 无效的选择${NC}"
        exit 1
        ;;
esac

echo ""
echo -e "${BLUE}========================================${NC}"
echo -e "${GREEN}✓ 操作完成！${NC}"
echo -e "${BLUE}========================================${NC}"

# 显示有用的链接
REMOTE_URL=$(git remote get-url "$REMOTE_NAME" 2>/dev/null)
if [[ "$REMOTE_URL" == *"github.com"* ]] && [[ "$REMOTE_URL" =~ github\.com[:/]([^/]+)/([^/]+)(\.git)?$ ]]; then
    GITHUB_USER="${BASH_REMATCH[1]}"
    GITHUB_REPO="${BASH_REMATCH[2]}"
    GITHUB_REPO="${GITHUB_REPO%.git}"
    
    echo ""
    echo -e "${CYAN}有用的链接:${NC}"
    echo -e "  📦 Release页面: https://github.com/${GITHUB_USER}/${GITHUB_REPO}/releases/tag/${TAG_NAME}"
    echo -e "  🔄 Actions页面: https://github.com/${GITHUB_USER}/${GITHUB_REPO}/actions"
    echo -e "  📊 仓库主页: https://github.com/${GITHUB_USER}/${GITHUB_REPO}"
fi
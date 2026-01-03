#!/bin/bash

# ========================================
# GitHub Release 资产上传脚本
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
MAX_RETRIES=3
RETRY_DELAY=10

echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}    GitHub Release 资产上传${NC}"
echo -e "${BLUE}========================================${NC}"

# 检查参数
if [ $# -eq 0 ]; then
    echo -e "${YELLOW}用法: $0 <文件路径> [标签名]${NC}"
    echo -e "${YELLOW}示例: $0 example-v1.0.0-macOS-qt6.6.1.zip${NC}"
    exit 1
fi

ASSET_FILE="$1"
if [ $# -ge 2 ]; then
    TAG_NAME="$2"
fi

echo -e "${CYAN}标签: ${TAG_NAME}${NC}"
echo -e "${CYAN}文件: ${ASSET_FILE}${NC}"

# 检查文件是否存在
if [ ! -f "$ASSET_FILE" ]; then
    echo -e "${RED}✗ 错误: 文件不存在: ${ASSET_FILE}${NC}"
    exit 1
fi

# 显示文件信息
echo -e "${CYAN}文件信息:${NC}"
ls -lh "$ASSET_FILE"
echo ""

# 检查是否在Git仓库中
if ! git rev-parse --git-dir > /dev/null 2>&1; then
    echo -e "${RED}✗ 错误: 当前目录不是Git仓库${NC}"
    exit 1
fi

# 获取仓库信息
REMOTE_URL=$(git remote get-url "$REMOTE_NAME" 2>/dev/null)
if [ $? -ne 0 ]; then
    echo -e "${RED}✗ 错误: 无法获取远程仓库信息${NC}"
    exit 1
fi

# 检查是否为GitHub仓库
if [[ "$REMOTE_URL" != *"github.com"* ]]; then
    echo -e "${RED}✗ 错误: 不是GitHub仓库${NC}"
    exit 1
fi

# 提取GitHub仓库信息
if [[ "$REMOTE_URL" =~ github\.com[:/]([^/]+)/([^/]+)(\.git)?$ ]]; then
    GITHUB_USER="${BASH_REMATCH[1]}"
    GITHUB_REPO="${BASH_REMATCH[2]}"
    GITHUB_REPO="${GITHUB_REPO%.git}"
else
    echo -e "${RED}✗ 错误: 无法解析GitHub仓库信息${NC}"
    exit 1
fi

echo -e "${CYAN}仓库: ${GITHUB_USER}/${GITHUB_REPO}${NC}"
echo ""

# 检查GitHub CLI
if ! command -v gh &> /dev/null; then
    echo -e "${RED}✗ 错误: 需要安装GitHub CLI${NC}"
    echo -e "${YELLOW}安装方法:${NC}"
    echo -e "  macOS: brew install gh${NC}"
    echo -e "  Linux: 参考 https://cli.github.com/manual/installation${NC}"
    exit 1
fi

# 检查GitHub CLI认证
if ! gh auth status &> /dev/null 2>&1; then
    echo -e "${RED}✗ 错误: GitHub CLI未认证${NC}"
    echo -e "${YELLOW}请运行: gh auth login${NC}"
    exit 1
fi

echo -e "${GREEN}✓ GitHub CLI已准备就绪${NC}"

# 检查Release是否存在
echo -e "${YELLOW}检查Release状态...${NC}"
if gh release view "$TAG_NAME" --repo "${GITHUB_USER}/${GITHUB_REPO}" &> /dev/null; then
    echo -e "${GREEN}✓ Release ${TAG_NAME} 已存在${NC}"
else
    echo -e "${YELLOW}⚠️  Release ${TAG_NAME} 不存在，将创建新的Release${NC}"
    
    read -p "是否创建新的Release? [y/N]: " create_release
    if [[ "$create_release" =~ ^[Yy]$ ]]; then
        echo -e "${CYAN}创建Release...${NC}"
        if gh release create "$TAG_NAME" --repo "${GITHUB_USER}/${GITHUB_REPO}" --generate-notes; then
            echo -e "${GREEN}✓ Release创建成功${NC}"
        else
            echo -e "${RED}✗ Release创建失败${NC}"
            exit 1
        fi
    else
        echo -e "${YELLOW}取消操作${NC}"
        exit 0
    fi
fi

# 上传文件（带重试机制）
echo -e "${YELLOW}上传文件到Release...${NC}"

upload_success=false
for ((i=1; i<=MAX_RETRIES; i++)); do
    echo -e "${CYAN}尝试 ${i}/${MAX_RETRIES}...${NC}"
    
    if gh release upload "$TAG_NAME" "$ASSET_FILE" --repo "${GITHUB_USER}/${GITHUB_REPO}" --clobber; then
        echo -e "${GREEN}✓ 文件上传成功！${NC}"
        upload_success=true
        break
    else
        echo -e "${RED}✗ 上传失败${NC}"
        
        if [ $i -lt $MAX_RETRIES ]; then
            echo -e "${YELLOW}等待 ${RETRY_DELAY} 秒后重试...${NC}"
            sleep $RETRY_DELAY
        fi
    fi
done

if [ "$upload_success" = false ]; then
    echo -e "${RED}✗ 所有重试都失败了${NC}"
    echo ""
    echo -e "${YELLOW}故障排除建议:${NC}"
    echo -e "  1. 检查网络连接"
    echo -e "  2. 验证GitHub token权限"
    echo -e "  3. 检查文件大小限制 (GitHub限制100MB)"
    echo -e "  4. 查看GitHub状态: https://www.githubstatus.com/"
    echo -e "  5. 手动上传: 访问 https://github.com/${GITHUB_USER}/${GITHUB_REPO}/releases/tag/${TAG_NAME}"
    exit 1
fi

# 验证上传结果
echo ""
echo -e "${CYAN}验证上传结果...${NC}"
if gh release view "$TAG_NAME" --repo "${GITHUB_USER}/${GITHUB_REPO}" | grep -q "$(basename "$ASSET_FILE")"; then
    echo -e "${GREEN}✓ 文件已成功添加到Release${NC}"
else
    echo -e "${YELLOW}⚠️  无法验证文件是否成功上传${NC}"
fi

echo ""
echo -e "${BLUE}========================================${NC}"
echo -e "${GREEN}✓ 上传完成！${NC}"
echo -e "${BLUE}========================================${NC}"

# 显示Release信息
echo ""
echo -e "${CYAN}Release信息:${NC}"
echo -e "  标签: ${TAG_NAME}"
echo -e "  仓库: ${GITHUB_USER}/${GITHUB_REPO}"
echo -e "  链接: https://github.com/${GITHUB_USER}/${GITHUB_REPO}/releases/tag/${TAG_NAME}"

echo ""
echo -e "${CYAN}上传的文件:${NC}"
echo -e "  文件名: $(basename "$ASSET_FILE")"
echo -e "  大小: $(du -h "$ASSET_FILE" | cut -f1)"
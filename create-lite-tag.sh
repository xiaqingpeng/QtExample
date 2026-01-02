#!/bin/bash

# ========================================
# 创建轻量级标签脚本 - 避免GitHub Actions超时
# ========================================

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# 配置
BASE_TAG="v1.0.0"
LITE_TAG="${BASE_TAG}-lite"
REMOTE_NAME="origin"

echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}    创建轻量级发布标签${NC}"
echo -e "${BLUE}========================================${NC}"
echo -e "${YELLOW}基础标签: ${BASE_TAG}${NC}"
echo -e "${YELLOW}轻量级标签: ${LITE_TAG}${NC}"
echo -e "${YELLOW}远程仓库: ${REMOTE_NAME}${NC}"
echo ""

# 检查是否在Git仓库中
if ! git rev-parse --git-dir > /dev/null 2>&1; then
    echo -e "${RED}✗ 错误: 当前目录不是Git仓库${NC}"
    exit 1
fi

# 检查远程仓库是否存在
if ! git remote | grep -q "^${REMOTE_NAME}$"; then
    echo -e "${RED}✗ 错误: 远程仓库 '${REMOTE_NAME}' 不存在${NC}"
    echo -e "${YELLOW}可用的远程仓库:${NC}"
    git remote -v
    exit 1
fi

echo -e "${GREEN}✓ Git仓库检查通过${NC}"
echo ""

# 说明轻量级版本的区别
echo -e "${CYAN}轻量级版本特点:${NC}"
echo -e "  • 只安装必需的Qt模块 (qtwebengine)"
echo -e "  • 更短的超时时间设置"
echo -e "  • 优化的缓存策略"
echo -e "  • 仅支持 macOS 和 Linux"
echo -e "  • 标记为预发布版本"
echo ""

read -p "是否继续创建轻量级标签? [y/N]: " confirm
if [[ ! "$confirm" =~ ^[Yy]$ ]]; then
    echo -e "${YELLOW}用户取消操作${NC}"
    exit 0
fi

# 步骤1: 删除本地轻量级标签
echo -e "${YELLOW}[1/4] 删除本地标签 ${LITE_TAG}...${NC}"
if git tag -d "$LITE_TAG" 2>/dev/null; then
    echo -e "${GREEN}✓ 本地标签删除成功${NC}"
else
    echo -e "${YELLOW}⚠️  本地标签不存在或删除失败${NC}"
fi
echo ""

# 步骤2: 删除远程轻量级标签
echo -e "${YELLOW}[2/4] 删除远程标签 ${LITE_TAG}...${NC}"
if git push "$REMOTE_NAME" ":refs/tags/$LITE_TAG" 2>/dev/null; then
    echo -e "${GREEN}✓ 远程标签删除成功${NC}"
else
    echo -e "${YELLOW}⚠️  远程标签删除失败（可能不存在）${NC}"
fi
echo ""

# 步骤3: 创建新的轻量级标签
echo -e "${YELLOW}[3/4] 创建轻量级标签 ${LITE_TAG}...${NC}"
if git tag "$LITE_TAG"; then
    echo -e "${GREEN}✓ 轻量级标签创建成功${NC}"
    
    # 显示标签信息
    echo -e "${CYAN}标签信息:${NC}"
    echo -e "  提交: $(git rev-parse --short $LITE_TAG)"
    echo -e "  日期: $(git log -1 --format=%cd --date=format:'%Y-%m-%d %H:%M:%S' $LITE_TAG)"
    echo -e "  消息: $(git log -1 --format=%s $LITE_TAG)"
else
    echo -e "${RED}✗ 轻量级标签创建失败${NC}"
    exit 1
fi
echo ""

# 步骤4: 推送轻量级标签到远程
echo -e "${YELLOW}[4/4] 推送轻量级标签到远程仓库...${NC}"
if git push "$REMOTE_NAME" "$LITE_TAG"; then
    echo -e "${GREEN}✓ 轻量级标签推送成功！${NC}"
    
    # 显示远程仓库信息
    echo ""
    echo -e "${CYAN}远程仓库信息:${NC}"
    git remote get-url "$REMOTE_NAME"
    
    # 检查是否为GitHub仓库，提供Release链接
    REMOTE_URL=$(git remote get-url "$REMOTE_NAME")
    if [[ "$REMOTE_URL" == *"github.com"* ]]; then
        # 提取仓库信息
        if [[ "$REMOTE_URL" =~ github\.com[:/]([^/]+)/([^/]+)(\.git)?$ ]]; then
            GITHUB_USER="${BASH_REMATCH[1]}"
            GITHUB_REPO="${BASH_REMATCH[2]}"
            GITHUB_REPO="${GITHUB_REPO%.git}"
            
            echo ""
            echo -e "${CYAN}GitHub Release链接:${NC}"
            echo -e "  https://github.com/${GITHUB_USER}/${GITHUB_REPO}/releases/tag/${LITE_TAG}"
            echo -e "${YELLOW}提示: 轻量级版本将使用 release-lite.yml 工作流${NC}"
            echo ""
            echo -e "${CYAN}GitHub Actions链接:${NC}"
            echo -e "  https://github.com/${GITHUB_USER}/${GITHUB_REPO}/actions"
        fi
    fi
else
    echo -e "${RED}✗ 轻量级标签推送失败${NC}"
    echo -e "${YELLOW}可能的原因:${NC}"
    echo -e "  1. 网络连接问题"
    echo -e "  2. 权限不足"
    echo -e "  3. 远程仓库不可访问"
    exit 1
fi

echo ""
echo -e "${BLUE}========================================${NC}"
echo -e "${GREEN}✓ 轻量级标签创建完成！${NC}"
echo -e "${BLUE}========================================${NC}"

# 显示最终状态
echo ""
echo -e "${YELLOW}最终状态:${NC}"
echo -e "${CYAN}本地标签:${NC}"
git tag -l "$LITE_TAG"

echo -e "${CYAN}远程标签:${NC}"
git ls-remote --tags "$REMOTE_NAME" | grep "$LITE_TAG" || echo -e "${YELLOW}  (查询远程标签失败，但推送已成功)${NC}"

echo ""
echo -e "${CYAN}下一步:${NC}"
echo -e "  1. 监控GitHub Actions执行: 使用 ${YELLOW}./check-pipeline.sh${NC}"
echo -e "  2. 如果轻量级版本成功，可以尝试完整版本"
echo -e "  3. 轻量级版本将标记为预发布，可以后续升级为正式发布"

echo ""
echo -e "${GREEN}========================================${NC}"
echo -e "${GREEN}✓ 脚本执行完成！${NC}"
echo -e "${GREEN}========================================${NC}"
#!/bin/bash

# ========================================
# 标签重建脚本：删除并重新创建Git标签
# ========================================

# 原始命令参考：
# 删除本地标签
git tag -d v1.0.0
# 删除远程标签
git push origin :refs/tags/v1.0.0
# 重新创建标签
git tag v1.0.0
# 推送新标签（这次会成功创建 Release！）
git push origin v1.0.0

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
PIPELINE_CHECK_TIMEOUT=300  # 5分钟超时
PIPELINE_CHECK_INTERVAL=10  # 每10秒检查一次

# GitHub Actions流水线检测函数
check_github_actions() {
    local user="$1"
    local repo="$2"
    local tag="$3"
    
    echo -e "${CYAN}正在检测GitHub Actions流水线...${NC}"
    
    # 方法1: 使用GitHub CLI (推荐)
    if command -v gh &> /dev/null && gh auth status &> /dev/null; then
        check_with_gh_cli "$user" "$repo" "$tag"
        return $?
    fi
    
    # 方法2: 使用curl和GitHub API (备用)
    echo -e "${YELLOW}GitHub CLI不可用，尝试使用API检测...${NC}"
    check_with_api "$user" "$repo" "$tag"
    return $?
}

# 使用GitHub CLI检测
check_with_gh_cli() {
    local user="$1"
    local repo="$2"
    local tag="$3"
    
    local start_time=$(date +%s)
    local timeout_time=$((start_time + PIPELINE_CHECK_TIMEOUT))
    
    echo -e "${CYAN}等待流水线启动...${NC}"
    sleep 5  # 等待GitHub触发流水线
    
    while [ $(date +%s) -lt $timeout_time ]; do
        # 获取最新的workflow runs
        local runs=$(gh run list --repo "${user}/${repo}" --limit 5 --json status,conclusion,event,headSha,workflowName,url 2>/dev/null)
        
        if [ $? -eq 0 ] && [ -n "$runs" ]; then
            # 查找与当前标签相关的运行
            local tag_commit=$(git rev-parse "$tag" 2>/dev/null)
            local tag_runs=$(echo "$runs" | jq -r --arg commit "$tag_commit" '.[] | select(.headSha == $commit or .event == "push") | "\(.status)|\(.conclusion)|\(.workflowName)|\(.url)"' 2>/dev/null)
            
            if [ -n "$tag_runs" ]; then
                echo -e "${CYAN}发现相关流水线:${NC}"
                
                local all_completed=true
                local has_failure=false
                
                while IFS='|' read -r status conclusion workflow_name url; do
                    if [ -n "$status" ]; then
                        echo -e "  工作流: ${workflow_name}"
                        echo -e "  状态: ${status}"
                        
                        case "$status" in
                            "completed")
                                case "$conclusion" in
                                    "success")
                                        echo -e "  结果: ${GREEN}✓ 成功${NC}"
                                        ;;
                                    "failure")
                                        echo -e "  结果: ${RED}✗ 失败${NC}"
                                        has_failure=true
                                        ;;
                                    "cancelled")
                                        echo -e "  结果: ${YELLOW}⚠️  已取消${NC}"
                                        has_failure=true
                                        ;;
                                    *)
                                        echo -e "  结果: ${YELLOW}${conclusion}${NC}"
                                        ;;
                                esac
                                ;;
                            "in_progress"|"queued")
                                echo -e "  结果: ${YELLOW}⏳ 运行中...${NC}"
                                all_completed=false
                                ;;
                            *)
                                echo -e "  结果: ${YELLOW}${status}${NC}"
                                all_completed=false
                                ;;
                        esac
                        
                        echo -e "  链接: ${url}"
                        echo ""
                    fi
                done <<< "$tag_runs"
                
                if [ "$all_completed" = true ]; then
                    if [ "$has_failure" = true ]; then
                        echo -e "${RED}✗ 流水线执行失败！${NC}"
                        return 1
                    else
                        echo -e "${GREEN}✓ 所有流水线执行成功！${NC}"
                        return 0
                    fi
                fi
            else
                echo -e "${YELLOW}⏳ 等待流水线启动...${NC}"
            fi
        else
            echo -e "${YELLOW}⚠️  无法获取流水线信息${NC}"
        fi
        
        sleep $PIPELINE_CHECK_INTERVAL
        echo -e "${CYAN}继续检测中... ($(( (timeout_time - $(date +%s)) / 60 ))分钟后超时)${NC}"
    done
    
    echo -e "${YELLOW}⚠️  流水线检测超时${NC}"
    echo -e "${YELLOW}手动检查链接: https://github.com/${user}/${repo}/actions${NC}"
    return 2
}

# 使用API检测 (简化版)
check_with_api() {
    local user="$1"
    local repo="$2"
    local tag="$3"
    
    echo -e "${CYAN}使用GitHub API检测流水线...${NC}"
    
    # 获取最新的workflow runs
    local api_url="https://api.github.com/repos/${user}/${repo}/actions/runs?per_page=5"
    local response=$(curl -s "$api_url" 2>/dev/null)
    
    if [ $? -eq 0 ] && [ -n "$response" ]; then
        # 简单检查是否有运行中的workflow
        local running_count=$(echo "$response" | grep -o '"status":"in_progress"' | wc -l)
        local queued_count=$(echo "$response" | grep -o '"status":"queued"' | wc -l)
        
        if [ "$running_count" -gt 0 ] || [ "$queued_count" -gt 0 ]; then
            echo -e "${YELLOW}⏳ 检测到运行中的流水线${NC}"
            echo -e "${CYAN}手动检查链接: https://github.com/${user}/${repo}/actions${NC}"
            return 0
        else
            echo -e "${GREEN}✓ 未检测到运行中的流水线${NC}"
            echo -e "${CYAN}手动检查链接: https://github.com/${user}/${repo}/actions${NC}"
            return 0
        fi
    else
        echo -e "${YELLOW}⚠️  无法访问GitHub API${NC}"
        echo -e "${CYAN}手动检查链接: https://github.com/${user}/${repo}/actions${NC}"
        return 2
    fi
}

echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}    标签重建脚本${NC}"
echo -e "${BLUE}========================================${NC}"
echo -e "${YELLOW}标签名称: ${TAG_NAME}${NC}"
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

# 步骤1: 删除本地标签
echo -e "${YELLOW}[1/4] 删除本地标签 ${TAG_NAME}...${NC}"
if git tag -d "$TAG_NAME" 2>/dev/null; then
    echo -e "${GREEN}✓ 本地标签删除成功${NC}"
else
    echo -e "${YELLOW}⚠️  本地标签不存在或删除失败（可能不存在）${NC}"
fi
echo ""

# 步骤2: 删除远程标签
echo -e "${YELLOW}[2/4] 删除远程标签 ${TAG_NAME}...${NC}"
if git push "$REMOTE_NAME" ":refs/tags/$TAG_NAME" 2>/dev/null; then
    echo -e "${GREEN}✓ 远程标签删除成功${NC}"
else
    echo -e "${YELLOW}⚠️  远程标签删除失败（可能不存在）${NC}"
fi
echo ""

# 步骤3: 重新创建本地标签
echo -e "${YELLOW}[3/4] 重新创建本地标签 ${TAG_NAME}...${NC}"
if git tag "$TAG_NAME"; then
    echo -e "${GREEN}✓ 本地标签创建成功${NC}"
    
    # 显示标签信息
    echo -e "${CYAN}标签信息:${NC}"
    echo -e "  提交: $(git rev-parse --short $TAG_NAME)"
    echo -e "  日期: $(git log -1 --format=%cd --date=format:'%Y-%m-%d %H:%M:%S' $TAG_NAME)"
    echo -e "  消息: $(git log -1 --format=%s $TAG_NAME)"
else
    echo -e "${RED}✗ 本地标签创建失败${NC}"
    exit 1
fi
echo ""

# 步骤4: 推送新标签到远程
echo -e "${YELLOW}[4/4] 推送标签到远程仓库...${NC}"
if git push "$REMOTE_NAME" "$TAG_NAME"; then
    echo -e "${GREEN}✓ 标签推送成功！${NC}"
    
    # 显示远程仓库信息
    echo ""
    echo -e "${CYAN}远程仓库信息:${NC}"
    git remote get-url "$REMOTE_NAME"
    
    # 检查是否为GitHub仓库，提供Release链接并检测流水线
    REMOTE_URL=$(git remote get-url "$REMOTE_NAME")
    if [[ "$REMOTE_URL" == *"github.com"* ]]; then
        # 提取仓库信息
        if [[ "$REMOTE_URL" =~ github\.com[:/]([^/]+)/([^/]+)(\.git)?$ ]]; then
            GITHUB_USER="${BASH_REMATCH[1]}"
            GITHUB_REPO="${BASH_REMATCH[2]}"
            GITHUB_REPO="${GITHUB_REPO%.git}"  # 移除.git后缀
            
            echo ""
            echo -e "${CYAN}GitHub Release链接:${NC}"
            echo -e "  https://github.com/${GITHUB_USER}/${GITHUB_REPO}/releases/tag/${TAG_NAME}"
            echo -e "${YELLOW}提示: 访问上述链接创建Release或查看自动创建的Release${NC}"
            
            # 检测GitHub Actions流水线状态
            echo ""
            echo -e "${YELLOW}[5/5] 检测GitHub Actions流水线状态...${NC}"
            
            # 提供手动检查选项
            echo -e "${CYAN}流水线检查选项:${NC}"
            echo -e "  1. 自动检测 (需要GitHub CLI)"
            echo -e "  2. 跳过检测"
            echo -e "  3. 手动检查链接: https://github.com/${GITHUB_USER}/${GITHUB_REPO}/actions"
            echo ""
            
            read -p "选择操作 [1/2/3] (默认: 1): " choice
            choice=${choice:-1}
            
            case $choice in
                1)
                    check_github_actions "$GITHUB_USER" "$GITHUB_REPO" "$TAG_NAME"
                    ;;
                2)
                    echo -e "${YELLOW}⚠️  跳过流水线检测${NC}"
                    ;;
                3)
                    echo -e "${CYAN}请手动访问以下链接检查流水线状态:${NC}"
                    echo -e "  https://github.com/${GITHUB_USER}/${GITHUB_REPO}/actions"
                    ;;
                *)
                    echo -e "${YELLOW}⚠️  无效选择，跳过流水线检测${NC}"
                    ;;
            esac
        fi
    fi
else
    echo -e "${RED}✗ 标签推送失败${NC}"
    echo -e "${YELLOW}可能的原因:${NC}"
    echo -e "  1. 网络连接问题"
    echo -e "  2. 权限不足"
    echo -e "  3. 远程仓库不可访问"
    echo ""
    echo -e "${YELLOW}建议操作:${NC}"
    echo -e "  1. 检查网络连接"
    echo -e "  2. 验证Git凭据: git config --list | grep user"
    echo -e "  3. 手动推送: git push ${REMOTE_NAME} ${TAG_NAME}"
    exit 1
fi

echo ""
echo -e "${BLUE}========================================${NC}"

# 根据流水线检测结果显示最终状态
if [[ "$REMOTE_URL" == *"github.com"* ]] && [[ "$REMOTE_URL" =~ github\.com[:/]([^/]+)/([^/]+)(\.git)?$ ]]; then
    # 如果检测了流水线，根据结果显示状态
    PIPELINE_EXIT_CODE=$?
    case $PIPELINE_EXIT_CODE in
        0)
            echo -e "${GREEN}✓ 标签重建和流水线执行完成！${NC}"
            ;;
        1)
            echo -e "${RED}✗ 标签重建完成，但流水线执行失败！${NC}"
            ;;
        2)
            echo -e "${YELLOW}⚠️  标签重建完成，流水线状态检测超时${NC}"
            ;;
        *)
            echo -e "${GREEN}✓ 标签重建完成！${NC}"
            ;;
    esac
else
    echo -e "${GREEN}✓ 标签重建完成！${NC}"
fi

echo -e "${BLUE}========================================${NC}"

# 显示最终状态
echo ""
echo -e "${YELLOW}最终状态检查:${NC}"
echo -e "${CYAN}本地标签:${NC}"
git tag -l "$TAG_NAME"

echo -e "${CYAN}远程标签:${NC}"
git ls-remote --tags "$REMOTE_NAME" | grep "$TAG_NAME" || echo -e "${YELLOW}  (查询远程标签失败，但推送已成功)${NC}"

echo ""
echo -e "${GREEN}========================================${NC}"
echo -e "${GREEN}✓ 脚本执行完成！${NC}"
echo -e "${GREEN}========================================${NC}"
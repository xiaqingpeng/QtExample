#!/bin/bash

# ========================================
# GitHub Actions 流水线状态检测脚本
# ========================================

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# 配置
REMOTE_NAME="origin"
CHECK_TIMEOUT=60  # 1分钟超时
CHECK_INTERVAL=5  # 每5秒检查一次

echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}    GitHub Actions 流水线检测${NC}"
echo -e "${BLUE}========================================${NC}"

# 检查是否在Git仓库中
if ! git rev-parse --git-dir > /dev/null 2>&1; then
    echo -e "${RED}✗ 错误: 当前目录不是Git仓库${NC}"
    exit 1
fi

# 获取远程仓库信息
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
echo -e "${CYAN}Actions链接: https://github.com/${GITHUB_USER}/${GITHUB_REPO}/actions${NC}"
echo ""

# 检测方法选择
if command -v gh &> /dev/null && gh auth status &> /dev/null 2>&1; then
    echo -e "${GREEN}✓ 使用GitHub CLI检测${NC}"
    check_method="gh"
elif command -v curl &> /dev/null; then
    echo -e "${YELLOW}⚠️  使用API检测 (功能有限)${NC}"
    check_method="api"
else
    echo -e "${RED}✗ 错误: 需要GitHub CLI或curl${NC}"
    echo -e "${YELLOW}安装GitHub CLI: brew install gh${NC}"
    exit 1
fi

echo ""

# 使用GitHub CLI检测
if [ "$check_method" = "gh" ]; then
    echo -e "${CYAN}获取最新流水线状态...${NC}"
    
    # 获取最新的5个workflow runs
    runs=$(gh run list --repo "${GITHUB_USER}/${GITHUB_REPO}" --limit 10 --json status,conclusion,event,workflowName,url,createdAt 2>/dev/null)
    
    if [ $? -eq 0 ] && [ -n "$runs" ]; then
        echo -e "${CYAN}最近的流水线运行:${NC}"
        echo ""
        
        # 解析并显示结果
        echo "$runs" | jq -r '.[] | "\(.status)|\(.conclusion)|\(.workflowName)|\(.url)|\(.createdAt)"' | head -5 | while IFS='|' read -r status conclusion workflow_name url created_at; do
            if [ -n "$status" ]; then
                echo -e "  ${BLUE}工作流:${NC} ${workflow_name}"
                echo -e "  ${BLUE}时间:${NC} ${created_at}"
                
                case "$status" in
                    "completed")
                        case "$conclusion" in
                            "success")
                                echo -e "  ${BLUE}状态:${NC} ${GREEN}✓ 成功${NC}"
                                ;;
                            "failure")
                                echo -e "  ${BLUE}状态:${NC} ${RED}✗ 失败${NC}"
                                echo -e "  ${YELLOW}💡 常见失败原因:${NC}"
                                echo -e "    - 构建文件路径不匹配"
                                echo -e "    - 依赖库缺失"
                                echo -e "    - CMake配置错误"
                                echo -e "    - 权限问题"
                                ;;
                            "cancelled")
                                echo -e "  ${BLUE}状态:${NC} ${YELLOW}⚠️  已取消${NC}"
                                ;;
                            *)
                                echo -e "  ${BLUE}状态:${NC} ${YELLOW}${conclusion}${NC}"
                                ;;
                        esac
                        ;;
                    "in_progress")
                        echo -e "  ${BLUE}状态:${NC} ${YELLOW}⏳ 运行中${NC}"
                        ;;
                    "queued")
                        echo -e "  ${BLUE}状态:${NC} ${YELLOW}⏳ 队列中${NC}"
                        ;;
                    *)
                        echo -e "  ${BLUE}状态:${NC} ${YELLOW}${status}${NC}"
                        ;;
                esac
                
                echo -e "  ${BLUE}链接:${NC} ${url}"
                echo ""
            fi
        done
        
        # 检查是否有运行中的流水线
        running_count=$(echo "$runs" | jq -r '.[] | select(.status == "in_progress" or .status == "queued") | .status' | wc -l)
        
        if [ "$running_count" -gt 0 ]; then
            echo -e "${YELLOW}⏳ 检测到 ${running_count} 个运行中的流水线${NC}"
            
            # 询问是否等待完成
            read -p "是否等待流水线完成? [y/N]: " wait_choice
            if [[ "$wait_choice" =~ ^[Yy]$ ]]; then
                echo -e "${CYAN}等待流水线完成...${NC}"
                
                start_time=$(date +%s)
                timeout_time=$((start_time + CHECK_TIMEOUT))
                
                while [ $(date +%s) -lt $timeout_time ]; do
                    sleep $CHECK_INTERVAL
                    
                    current_runs=$(gh run list --repo "${GITHUB_USER}/${GITHUB_REPO}" --limit 5 --json status,conclusion 2>/dev/null)
                    current_running=$(echo "$current_runs" | jq -r '.[] | select(.status == "in_progress" or .status == "queued") | .status' | wc -l)
                    
                    if [ "$current_running" -eq 0 ]; then
                        echo -e "${GREEN}✓ 所有流水线已完成${NC}"
                        
                        # 显示最终结果
                        final_runs=$(gh run list --repo "${GITHUB_USER}/${GITHUB_REPO}" --limit 3 --json status,conclusion,workflowName 2>/dev/null)
                        echo "$final_runs" | jq -r '.[] | "\(.conclusion)|\(.workflowName)"' | while IFS='|' read -r conclusion workflow_name; do
                            case "$conclusion" in
                                "success")
                                    echo -e "  ${workflow_name}: ${GREEN}✓ 成功${NC}"
                                    ;;
                                "failure")
                                    echo -e "  ${workflow_name}: ${RED}✗ 失败${NC}"
                                    ;;
                                *)
                                    echo -e "  ${workflow_name}: ${YELLOW}${conclusion}${NC}"
                                    ;;
                            esac
                        done
                        break
                    fi
                    
                    echo -e "${CYAN}仍有 ${current_running} 个流水线运行中...${NC}"
                done
                
                if [ $(date +%s) -ge $timeout_time ]; then
                    echo -e "${YELLOW}⚠️  等待超时${NC}"
                fi
            fi
        else
            echo -e "${GREEN}✓ 没有运行中的流水线${NC}"
        fi
        
    else
        echo -e "${RED}✗ 无法获取流水线信息${NC}"
        exit 1
    fi

# 使用API检测 (简化版)
else
    echo -e "${CYAN}使用GitHub API检测...${NC}"
    
    api_url="https://api.github.com/repos/${GITHUB_USER}/${GITHUB_REPO}/actions/runs?per_page=5"
    response=$(curl -s "$api_url" 2>/dev/null)
    
    if [ $? -eq 0 ] && [ -n "$response" ]; then
        # 检查API响应是否有效
        if echo "$response" | grep -q '"workflow_runs"'; then
            running_count=$(echo "$response" | grep -o '"status":"in_progress"' | wc -l)
            queued_count=$(echo "$response" | grep -o '"status":"queued"' | wc -l)
            total_active=$((running_count + queued_count))
            
            if [ "$total_active" -gt 0 ]; then
                echo -e "${YELLOW}⏳ 检测到 ${total_active} 个活跃的流水线${NC}"
                echo -e "  运行中: ${running_count}"
                echo -e "  队列中: ${queued_count}"
            else
                echo -e "${GREEN}✓ 没有运行中的流水线${NC}"
            fi
        else
            echo -e "${YELLOW}⚠️  API响应异常，可能是私有仓库或API限制${NC}"
        fi
    else
        echo -e "${RED}✗ 无法访问GitHub API${NC}"
    fi
fi

echo ""
echo -e "${BLUE}========================================${NC}"
echo -e "${CYAN}手动检查链接: https://github.com/${GITHUB_USER}/${GITHUB_REPO}/actions${NC}"
echo -e "${BLUE}========================================${NC}"
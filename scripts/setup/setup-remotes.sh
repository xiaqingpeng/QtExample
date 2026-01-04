#!/bin/bash

# ========================================
# 远程仓库配置脚本
# 帮助用户设置GitHub和GitLab远程仓库
# ========================================

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}    远程仓库配置助手${NC}"
echo -e "${BLUE}========================================${NC}"

# 检查Git仓库
check_git_repo() {
    if ! git rev-parse --git-dir > /dev/null 2>&1; then
        echo -e "${RED}✗ 错误: 当前目录不是Git仓库${NC}"
        exit 1
    fi
    echo -e "${GREEN}✓ Git仓库检查通过${NC}"
}

# 显示当前远程仓库
show_current_remotes() {
    echo -e "${CYAN}当前远程仓库配置:${NC}"
    if git remote -v | grep -q .; then
        git remote -v
    else
        echo -e "${YELLOW}  (无远程仓库)${NC}"
    fi
    echo ""
}

# 设置GitHub远程仓库
setup_github_remote() {
    local remote_name="origin"
    
    echo -e "${YELLOW}设置GitHub远程仓库...${NC}"
    
    # 检查是否已存在
    if git remote get-url "$remote_name" >/dev/null 2>&1; then
        local current_url=$(git remote get-url "$remote_name")
        echo -e "${CYAN}当前GitHub远程仓库 (${remote_name}): ${current_url}${NC}"
        
        read -p "是否更新GitHub远程仓库URL? [y/N]: " update_github
        if [[ ! "$update_github" =~ ^[Yy]$ ]]; then
            return 0
        fi
    fi
    
    # 输入GitHub仓库URL
    echo -e "${CYAN}请输入GitHub仓库URL:${NC}"
    echo -e "${YELLOW}格式示例:${NC}"
    echo -e "  HTTPS: https://github.com/username/repository.git"
    echo -e "  SSH:   git@github.com:username/repository.git"
    echo ""
    
    read -p "GitHub URL: " github_url
    
    if [ -z "$github_url" ]; then
        echo -e "${YELLOW}跳过GitHub远程仓库设置${NC}"
        return 0
    fi
    
    # 验证URL格式
    if [[ ! "$github_url" =~ github\.com ]]; then
        echo -e "${RED}✗ 错误: 不是有效的GitHub URL${NC}"
        return 1
    fi
    
    # 设置或更新远程仓库
    if git remote get-url "$remote_name" >/dev/null 2>&1; then
        git remote set-url "$remote_name" "$github_url"
        echo -e "${GREEN}✓ 已更新GitHub远程仓库${NC}"
    else
        git remote add "$remote_name" "$github_url"
        echo -e "${GREEN}✓ 已添加GitHub远程仓库${NC}"
    fi
}

# 设置GitLab远程仓库
setup_gitlab_remote() {
    local remote_name="gitlab"
    
    echo -e "${YELLOW}设置GitLab远程仓库...${NC}"
    
    # 检查是否已存在
    if git remote get-url "$remote_name" >/dev/null 2>&1; then
        local current_url=$(git remote get-url "$remote_name")
        echo -e "${CYAN}当前GitLab远程仓库 (${remote_name}): ${current_url}${NC}"
        
        read -p "是否更新GitLab远程仓库URL? [y/N]: " update_gitlab
        if [[ ! "$update_gitlab" =~ ^[Yy]$ ]]; then
            return 0
        fi
    fi
    
    # 输入GitLab仓库URL
    echo -e "${CYAN}请输入GitLab仓库URL:${NC}"
    echo -e "${YELLOW}格式示例:${NC}"
    echo -e "  HTTPS: https://gitlab.com/username/repository.git"
    echo -e "  SSH:   git@gitlab.com:username/repository.git"
    echo -e "  自建:  https://your-gitlab.com/username/repository.git"
    echo ""
    
    read -p "GitLab URL (留空跳过): " gitlab_url
    
    if [ -z "$gitlab_url" ]; then
        echo -e "${YELLOW}跳过GitLab远程仓库设置${NC}"
        return 0
    fi
    
    # 验证URL格式
    if [[ ! "$gitlab_url" =~ gitlab ]]; then
        echo -e "${YELLOW}⚠️  警告: URL中不包含'gitlab'，请确认这是正确的GitLab URL${NC}"
        read -p "是否继续? [y/N]: " continue_setup
        if [[ ! "$continue_setup" =~ ^[Yy]$ ]]; then
            return 0
        fi
    fi
    
    # 设置或更新远程仓库
    if git remote get-url "$remote_name" >/dev/null 2>&1; then
        git remote set-url "$remote_name" "$gitlab_url"
        echo -e "${GREEN}✓ 已更新GitLab远程仓库${NC}"
    else
        git remote add "$remote_name" "$gitlab_url"
        echo -e "${GREEN}✓ 已添加GitLab远程仓库${NC}"
    fi
}

# 测试远程仓库连接
test_remote_connection() {
    echo -e "${YELLOW}测试远程仓库连接...${NC}"
    
    # 测试GitHub连接
    if git remote get-url "origin" >/dev/null 2>&1; then
        echo -e "${CYAN}测试GitHub连接...${NC}"
        if git ls-remote --heads origin >/dev/null 2>&1; then
            echo -e "${GREEN}✓ GitHub连接正常${NC}"
        else
            echo -e "${RED}✗ GitHub连接失败${NC}"
            echo -e "${YELLOW}  请检查网络连接和认证配置${NC}"
        fi
    fi
    
    # 测试GitLab连接
    if git remote get-url "gitlab" >/dev/null 2>&1; then
        echo -e "${CYAN}测试GitLab连接...${NC}"
        if git ls-remote --heads gitlab >/dev/null 2>&1; then
            echo -e "${GREEN}✓ GitLab连接正常${NC}"
        else
            echo -e "${RED}✗ GitLab连接失败${NC}"
            echo -e "${YELLOW}  请检查网络连接和认证配置${NC}"
        fi
    fi
}

# 显示使用指南
show_usage_guide() {
    echo -e "${BLUE}========================================${NC}"
    echo -e "${BLUE}    配置完成！${NC}"
    echo -e "${BLUE}========================================${NC}"
    
    echo -e "${CYAN}现在你可以使用以下命令:${NC}"
    echo ""
    
    echo -e "${YELLOW}1. 构建并推送到两个远程仓库:${NC}"
    echo -e "   ./scripts/build/build-and-push.sh"
    echo ""
    
    echo -e "${YELLOW}2. 只推送到GitHub:${NC}"
    echo -e "   ./scripts/build/build-and-push.sh --github-only"
    echo ""
    
    echo -e "${YELLOW}3. 只推送到GitLab:${NC}"
    echo -e "   ./scripts/build/build-and-push.sh --gitlab-only"
    echo ""
    
    echo -e "${YELLOW}4. 跨平台构建并推送:${NC}"
    echo -e "   ./scripts/build/build-all-platforms.sh"
    echo ""
    
    echo -e "${YELLOW}5. 查看远程仓库状态:${NC}"
    echo -e "   git remote -v"
    echo ""
    
    echo -e "${CYAN}认证配置提示:${NC}"
    echo -e "${YELLOW}GitHub:${NC}"
    echo -e "  - HTTPS: 使用Personal Access Token"
    echo -e "  - SSH: 配置SSH密钥"
    echo ""
    echo -e "${YELLOW}GitLab:${NC}"
    echo -e "  - HTTPS: 使用Personal Access Token"
    echo -e "  - SSH: 配置SSH密钥"
    echo ""
    
    echo -e "${CYAN}相关文档:${NC}"
    echo -e "  - GitHub SSH: https://docs.github.com/en/authentication/connecting-to-github-with-ssh"
    echo -e "  - GitLab SSH: https://docs.gitlab.com/ee/user/ssh.html"
}

# 主函数
main() {
    check_git_repo
    show_current_remotes
    
    echo -e "${CYAN}开始配置远程仓库...${NC}"
    echo ""
    
    setup_github_remote
    echo ""
    
    setup_gitlab_remote
    echo ""
    
    show_current_remotes
    test_remote_connection
    echo ""
    
    show_usage_guide
}

# 运行主函数
main "$@"
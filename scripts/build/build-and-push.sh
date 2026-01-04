#!/bin/bash

# ========================================
# 本地构建测试并推送到远程仓库脚本
# 功能：本地构建成功后自动推送到GitHub和GitLab
# ========================================

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
MAGENTA='\033[0;35m'
NC='\033[0m' # No Color

# 配置参数
GITHUB_REMOTE="origin"      # GitHub远程仓库名
GITLAB_REMOTE="gitlab"      # GitLab远程仓库名
CURRENT_BRANCH=""           # 当前分支名
FORCE_PUSH=false            # 是否强制推送
SKIP_BUILD=false            # 是否跳过构建
PUSH_TO_GITHUB=true         # 是否推送到GitHub
PUSH_TO_GITLAB=true         # 是否推送到GitLab

echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}    本地构建测试并推送到远程仓库${NC}"
echo -e "${BLUE}========================================${NC}"

# 显示帮助信息
show_help() {
    echo -e "${YELLOW}用法:${NC} $0 [选项]"
    echo ""
    echo -e "${YELLOW}选项:${NC}"
    echo -e "  ${CYAN}--skip-build${NC}         跳过本地构建，直接推送"
    echo -e "  ${CYAN}--force-push${NC}         强制推送到远程仓库"
    echo -e "  ${CYAN}--github-only${NC}        只推送到GitHub"
    echo -e "  ${CYAN}--gitlab-only${NC}        只推送到GitLab"
    echo -e "  ${CYAN}--github-remote NAME${NC} 设置GitHub远程仓库名 (默认: origin)"
    echo -e "  ${CYAN}--gitlab-remote NAME${NC} 设置GitLab远程仓库名 (默认: gitlab)"
    echo -e "  ${CYAN}--help${NC}               显示此帮助信息"
    echo ""
    echo -e "${YELLOW}使用示例:${NC}"
    echo -e "  ${GREEN}# 构建并推送到两个远程仓库${NC}"
    echo -e "  $0"
    echo ""
    echo -e "  ${GREEN}# 跳过构建，直接推送${NC}"
    echo -e "  $0 --skip-build"
    echo ""
    echo -e "  ${GREEN}# 只推送到GitHub${NC}"
    echo -e "  $0 --github-only"
    echo ""
    echo -e "  ${GREEN}# 强制推送${NC}"
    echo -e "  $0 --force-push"
    echo ""
    echo -e "  ${GREEN}# 自定义远程仓库名${NC}"
    echo -e "  $0 --github-remote upstream --gitlab-remote gitlab-origin"
    echo ""
}

# 解析命令行参数
parse_arguments() {
    while [[ $# -gt 0 ]]; do
        case $1 in
            --skip-build)
                SKIP_BUILD=true
                shift
                ;;
            --force-push)
                FORCE_PUSH=true
                shift
                ;;
            --github-only)
                PUSH_TO_GITLAB=false
                shift
                ;;
            --gitlab-only)
                PUSH_TO_GITHUB=false
                shift
                ;;
            --github-remote)
                GITHUB_REMOTE="$2"
                shift 2
                ;;
            --gitlab-remote)
                GITLAB_REMOTE="$2"
                shift 2
                ;;
            --help)
                show_help
                exit 0
                ;;
            *)
                echo -e "${RED}未知选项: $1${NC}"
                echo -e "${YELLOW}使用 --help 查看帮助信息${NC}"
                exit 1
                ;;
        esac
    done
}

# 检查Git仓库状态
check_git_repo() {
    echo -e "${YELLOW}[1/6] 检查Git仓库状态...${NC}"
    
    # 检查是否在Git仓库中
    if ! git rev-parse --git-dir > /dev/null 2>&1; then
        echo -e "${RED}✗ 错误: 当前目录不是Git仓库${NC}"
        exit 1
    fi
    
    # 获取当前分支名
    CURRENT_BRANCH=$(git branch --show-current)
    if [ -z "$CURRENT_BRANCH" ]; then
        echo -e "${RED}✗ 错误: 无法获取当前分支名${NC}"
        exit 1
    fi
    
    echo -e "${CYAN}当前分支: ${CURRENT_BRANCH}${NC}"
    
    # 检查是否有未提交的更改
    if ! git diff --quiet || ! git diff --staged --quiet; then
        echo -e "${YELLOW}⚠️  检测到未提交的更改${NC}"
        echo -e "${CYAN}未提交的文件:${NC}"
        git status --porcelain
        echo ""
        
        read -p "是否先提交这些更改? [y/N]: " commit_changes
        if [[ "$commit_changes" =~ ^[Yy]$ ]]; then
            commit_changes
        else
            echo -e "${YELLOW}⚠️  将继续处理，但建议先提交更改${NC}"
        fi
    fi
    
    echo -e "${GREEN}✓ Git仓库状态检查完成${NC}"
}

# 提交更改
commit_changes() {
    echo -e "${CYAN}提交未提交的更改...${NC}"
    
    # 显示当前状态
    git status
    
    # 添加所有更改
    read -p "是否添加所有更改到暂存区? [y/N]: " add_all
    if [[ "$add_all" =~ ^[Yy]$ ]]; then
        git add .
        echo -e "${GREEN}✓ 已添加所有更改到暂存区${NC}"
    else
        echo -e "${YELLOW}请手动添加需要提交的文件${NC}"
        return 1
    fi
    
    # 输入提交信息
    echo -e "${CYAN}请输入提交信息:${NC}"
    read -p "提交信息: " commit_message
    
    if [ -z "$commit_message" ]; then
        commit_message="Auto commit before build and push - $(date '+%Y-%m-%d %H:%M:%S')"
        echo -e "${YELLOW}使用默认提交信息: ${commit_message}${NC}"
    fi
    
    # 提交更改
    if git commit -m "$commit_message"; then
        echo -e "${GREEN}✓ 更改已提交${NC}"
    else
        echo -e "${RED}✗ 提交失败${NC}"
        exit 1
    fi
}

# 检查远程仓库
check_remotes() {
    echo -e "${YELLOW}[2/6] 检查远程仓库配置...${NC}"
    
    # 检查GitHub远程仓库
    if [ "$PUSH_TO_GITHUB" = true ]; then
        if git remote get-url "$GITHUB_REMOTE" >/dev/null 2>&1; then
            local github_url=$(git remote get-url "$GITHUB_REMOTE")
            echo -e "${GREEN}✓ GitHub远程仓库 (${GITHUB_REMOTE}): ${github_url}${NC}"
        else
            echo -e "${RED}✗ GitHub远程仓库 '${GITHUB_REMOTE}' 不存在${NC}"
            echo -e "${YELLOW}请添加GitHub远程仓库: git remote add ${GITHUB_REMOTE} <github-url>${NC}"
            exit 1
        fi
    fi
    
    # 检查GitLab远程仓库
    if [ "$PUSH_TO_GITLAB" = true ]; then
        if git remote get-url "$GITLAB_REMOTE" >/dev/null 2>&1; then
            local gitlab_url=$(git remote get-url "$GITLAB_REMOTE")
            echo -e "${GREEN}✓ GitLab远程仓库 (${GITLAB_REMOTE}): ${gitlab_url}${NC}"
        else
            echo -e "${YELLOW}⚠️  GitLab远程仓库 '${GITLAB_REMOTE}' 不存在${NC}"
            
            read -p "是否添加GitLab远程仓库? [y/N]: " add_gitlab
            if [[ "$add_gitlab" =~ ^[Yy]$ ]]; then
                read -p "请输入GitLab仓库URL: " gitlab_url
                if [ -n "$gitlab_url" ]; then
                    git remote add "$GITLAB_REMOTE" "$gitlab_url"
                    echo -e "${GREEN}✓ 已添加GitLab远程仓库${NC}"
                else
                    echo -e "${YELLOW}跳过GitLab推送${NC}"
                    PUSH_TO_GITLAB=false
                fi
            else
                echo -e "${YELLOW}跳过GitLab推送${NC}"
                PUSH_TO_GITLAB=false
            fi
        fi
    fi
    
    echo -e "${GREEN}✓ 远程仓库配置检查完成${NC}"
}

# 执行本地构建测试
run_local_build() {
    if [ "$SKIP_BUILD" = true ]; then
        echo -e "${YELLOW}[3/6] 跳过本地构建测试${NC}"
        return 0
    fi
    
    echo -e "${YELLOW}[3/6] 执行本地构建测试...${NC}"
    
    # 检查是否在正确的目录
    if [ ! -f "CMakeLists.txt" ]; then
        echo -e "${RED}✗ 错误: 未找到CMakeLists.txt文件${NC}"
        echo -e "${YELLOW}请在项目根目录运行此脚本${NC}"
        exit 1
    fi
    
    # 清理之前的构建
    echo -e "${CYAN}清理构建目录...${NC}"
    if [ -d "build" ]; then
        rm -rf build
    fi
    
    # 创建构建目录
    mkdir -p build
    cd build
    
    # 配置CMake
    echo -e "${CYAN}配置CMake...${NC}"
    if ! cmake .. -DCMAKE_BUILD_TYPE=Release; then
        echo -e "${RED}✗ CMake配置失败${NC}"
        cd ..
        exit 1
    fi
    
    # 构建项目
    echo -e "${CYAN}构建项目...${NC}"
    if ! cmake --build . --config Release --parallel; then
        echo -e "${RED}✗ 项目构建失败${NC}"
        cd ..
        exit 1
    fi
    
    cd ..
    
    # 检查构建结果
    echo -e "${CYAN}检查构建结果...${NC}"
    local build_success=false
    
    if [[ "$OSTYPE" == "darwin"* ]]; then
        # macOS
        if [ -d "build/example.app" ]; then
            echo -e "${GREEN}✓ 找到macOS应用包${NC}"
            build_success=true
        fi
    elif [[ "$OSTYPE" == "msys" ]] || [[ "$OSTYPE" == "win32" ]]; then
        # Windows
        if [ -f "build/example.exe" ] || [ -f "build/Release/example.exe" ]; then
            echo -e "${GREEN}✓ 找到Windows可执行文件${NC}"
            build_success=true
        fi
    else
        # Linux
        if [ -f "build/example" ]; then
            echo -e "${GREEN}✓ 找到Linux可执行文件${NC}"
            build_success=true
        fi
    fi
    
    if [ "$build_success" = false ]; then
        echo -e "${RED}✗ 构建失败: 未找到可执行文件${NC}"
        exit 1
    fi
    
    echo -e "${GREEN}✓ 本地构建测试成功${NC}"
}

# 推送到GitHub
push_to_github() {
    if [ "$PUSH_TO_GITHUB" != true ]; then
        return 0
    fi
    
    echo -e "${YELLOW}[4/6] 推送到GitHub...${NC}"
    
    local push_args=()
    if [ "$FORCE_PUSH" = true ]; then
        push_args+=("--force")
        echo -e "${YELLOW}⚠️  使用强制推送${NC}"
    fi
    
    echo -e "${CYAN}推送分支 '${CURRENT_BRANCH}' 到 GitHub (${GITHUB_REMOTE})...${NC}"
    
    if git push "${push_args[@]}" "$GITHUB_REMOTE" "$CURRENT_BRANCH"; then
        echo -e "${GREEN}✓ 成功推送到GitHub${NC}"
        
        # 显示GitHub仓库链接
        local github_url=$(git remote get-url "$GITHUB_REMOTE")
        if [[ "$github_url" == *"github.com"* ]]; then
            # 提取仓库信息
            local repo_info=$(echo "$github_url" | sed -E 's/.*github\.com[:/]([^/]+\/[^/]+)(\.git)?$/\1/')
            echo -e "${CYAN}GitHub仓库: https://github.com/${repo_info}${NC}"
            echo -e "${CYAN}分支链接: https://github.com/${repo_info}/tree/${CURRENT_BRANCH}${NC}"
        fi
    else
        echo -e "${RED}✗ 推送到GitHub失败${NC}"
        
        # 提供解决建议
        echo -e "${YELLOW}可能的解决方案:${NC}"
        echo -e "  1. 检查网络连接"
        echo -e "  2. 检查GitHub认证: git config --list | grep user"
        echo -e "  3. 尝试强制推送: $0 --force-push"
        echo -e "  4. 检查分支是否存在冲突"
        
        read -p "是否继续推送到GitLab? [y/N]: " continue_gitlab
        if [[ ! "$continue_gitlab" =~ ^[Yy]$ ]]; then
            exit 1
        fi
    fi
}

# 推送到GitLab
push_to_gitlab() {
    if [ "$PUSH_TO_GITLAB" != true ]; then
        return 0
    fi
    
    echo -e "${YELLOW}[5/6] 推送到GitLab...${NC}"
    
    local push_args=()
    if [ "$FORCE_PUSH" = true ]; then
        push_args+=("--force")
        echo -e "${YELLOW}⚠️  使用强制推送${NC}"
    fi
    
    echo -e "${CYAN}推送分支 '${CURRENT_BRANCH}' 到 GitLab (${GITLAB_REMOTE})...${NC}"
    
    if git push "${push_args[@]}" "$GITLAB_REMOTE" "$CURRENT_BRANCH"; then
        echo -e "${GREEN}✓ 成功推送到GitLab${NC}"
        
        # 显示GitLab仓库链接
        local gitlab_url=$(git remote get-url "$GITLAB_REMOTE")
        if [[ "$gitlab_url" == *"gitlab"* ]]; then
            echo -e "${CYAN}GitLab仓库: ${gitlab_url}${NC}"
        fi
    else
        echo -e "${RED}✗ 推送到GitLab失败${NC}"
        
        # 提供解决建议
        echo -e "${YELLOW}可能的解决方案:${NC}"
        echo -e "  1. 检查网络连接"
        echo -e "  2. 检查GitLab认证"
        echo -e "  3. 尝试强制推送: $0 --force-push"
        echo -e "  4. 检查GitLab仓库URL是否正确"
        
        return 1
    fi
}

# 显示推送结果
show_push_results() {
    echo -e "${YELLOW}[6/6] 推送结果总结${NC}"
    
    echo -e "${BLUE}========================================${NC}"
    echo -e "${BLUE}    推送结果总结${NC}"
    echo -e "${BLUE}========================================${NC}"
    
    echo -e "${CYAN}分支信息:${NC}"
    echo -e "  当前分支: ${CURRENT_BRANCH}"
    echo -e "  最新提交: $(git log -1 --oneline)"
    echo ""
    
    echo -e "${CYAN}推送状态:${NC}"
    if [ "$PUSH_TO_GITHUB" = true ]; then
        echo -e "  ${GREEN}✓${NC} GitHub (${GITHUB_REMOTE})"
    fi
    if [ "$PUSH_TO_GITLAB" = true ]; then
        echo -e "  ${GREEN}✓${NC} GitLab (${GITLAB_REMOTE})"
    fi
    echo ""
    
    echo -e "${CYAN}远程仓库链接:${NC}"
    if [ "$PUSH_TO_GITHUB" = true ]; then
        local github_url=$(git remote get-url "$GITHUB_REMOTE")
        echo -e "  GitHub: ${github_url}"
    fi
    if [ "$PUSH_TO_GITLAB" = true ]; then
        local gitlab_url=$(git remote get-url "$GITLAB_REMOTE")
        echo -e "  GitLab: ${gitlab_url}"
    fi
    echo ""
    
    echo -e "${CYAN}下一步建议:${NC}"
    echo -e "  1. 检查远程仓库是否正确更新"
    echo -e "  2. 如需发布版本，运行: ${YELLOW}./scripts/build/build-all-platforms.sh${NC}"
    echo -e "  3. 如需创建Pull Request，访问相应的仓库页面"
    echo ""
}

# 主函数
main() {
    # 解析命令行参数
    parse_arguments "$@"
    
    # 显示配置信息
    echo -e "${CYAN}配置信息:${NC}"
    echo -e "  GitHub远程仓库: ${GITHUB_REMOTE}"
    echo -e "  GitLab远程仓库: ${GITLAB_REMOTE}"
    echo -e "  跳过构建: ${SKIP_BUILD}"
    echo -e "  强制推送: ${FORCE_PUSH}"
    echo -e "  推送到GitHub: ${PUSH_TO_GITHUB}"
    echo -e "  推送到GitLab: ${PUSH_TO_GITLAB}"
    echo ""
    
    # 执行主要流程
    check_git_repo
    check_remotes
    run_local_build
    push_to_github
    push_to_gitlab
    show_push_results
    
    echo -e "${GREEN}========================================${NC}"
    echo -e "${GREEN}✓ 构建和推送流程完成！${NC}"
    echo -e "${GREEN}========================================${NC}"
}

# 运行主函数
main "$@"
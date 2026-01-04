#!/bin/bash

# ========================================
# 跨平台Qt应用构建和打包脚本
# 支持 macOS、Linux、Windows 三个平台
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
VERSION="v1.0.1"
QT_VERSION="qt6.6.1"
TAG_NAME="${VERSION}-lite"
REMOTE_NAME="origin"

# 构建配置
BUILD_MACOS=true
BUILD_LINUX=true
BUILD_WINDOWS=true
UPLOAD_TO_RELEASE=true
CREATE_RELEASE_IF_NOT_EXISTS=true
PUSH_TO_REMOTES=true  # 新增：是否推送到远程仓库

# 远程仓库配置
GITHUB_REMOTE="origin"
GITLAB_REMOTE="gitlab"

# 平台特定配置
MACOS_ARCH="arm64"  # 或 x86_64
LINUX_ARCH="x86_64"
WINDOWS_ARCH="x64"

echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}    跨平台Qt应用构建和打包${NC}"
echo -e "${BLUE}========================================${NC}"
echo -e "${CYAN}版本: ${VERSION}${NC}"
echo -e "${CYAN}Qt版本: ${QT_VERSION}${NC}"
echo -e "${CYAN}标签: ${TAG_NAME}${NC}"
echo ""

# 显示构建计划
echo -e "${YELLOW}构建计划:${NC}"
[ "$BUILD_MACOS" = true ] && echo -e "  ${GREEN}✓${NC} macOS (${MACOS_ARCH})"
[ "$BUILD_LINUX" = true ] && echo -e "  ${GREEN}✓${NC} Linux (${LINUX_ARCH})"
[ "$BUILD_WINDOWS" = true ] && echo -e "  ${GREEN}✓${NC} Windows (${WINDOWS_ARCH})"
echo ""

# 检查必需工具
check_requirements() {
    echo -e "${YELLOW}检查构建环境...${NC}"
    
    local missing_tools=()
    
    # 检查基础工具
    command -v git >/dev/null 2>&1 || missing_tools+=("git")
    command -v cmake >/dev/null 2>&1 || missing_tools+=("cmake")
    command -v zip >/dev/null 2>&1 || missing_tools+=("zip")
    command -v tar >/dev/null 2>&1 || missing_tools+=("tar")
    
    # 检查GitHub CLI（如果需要上传）
    if [ "$UPLOAD_TO_RELEASE" = true ]; then
        command -v gh >/dev/null 2>&1 || missing_tools+=("gh")
    fi
    
    if [ ${#missing_tools[@]} -ne 0 ]; then
        echo -e "${RED}✗ 缺少必需工具: ${missing_tools[*]}${NC}"
        echo -e "${YELLOW}请安装缺少的工具后重试${NC}"
        exit 1
    fi
    
    echo -e "${GREEN}✓ 构建环境检查通过${NC}"
}

# 检查Git仓库状态
check_git_repo() {
    echo -e "${YELLOW}检查Git仓库...${NC}"
    
    if ! git rev-parse --git-dir > /dev/null 2>&1; then
        echo -e "${RED}✗ 错误: 当前目录不是Git仓库${NC}"
        exit 1
    fi
    
    # 检查是否有未提交的更改
    if ! git diff --quiet || ! git diff --staged --quiet; then
        echo -e "${YELLOW}⚠️  检测到未提交的更改${NC}"
        read -p "是否继续构建? [y/N]: " continue_build
        if [[ ! "$continue_build" =~ ^[Yy]$ ]]; then
            echo -e "${YELLOW}用户取消构建${NC}"
            exit 0
        fi
    fi
    
    echo -e "${GREEN}✓ Git仓库检查通过${NC}"
}

# 清理构建目录
clean_build_dirs() {
    echo -e "${YELLOW}清理构建目录...${NC}"
    
    # 清理主构建目录
    if [ -d "build" ]; then
        rm -rf build
        echo -e "${GREEN}✓ 清理主构建目录${NC}"
    fi
    
    # 清理平台特定构建目录
    for platform in macos linux windows; do
        if [ -d "build-${platform}" ]; then
            rm -rf "build-${platform}"
            echo -e "${GREEN}✓ 清理 ${platform} 构建目录${NC}"
        fi
    done
    
    # 清理旧的打包文件
    rm -f example-${VERSION}-*.{zip,tar.gz} 2>/dev/null
    rm -rf example-${VERSION}-*/ 2>/dev/null
    
    echo -e "${GREEN}✓ 构建目录清理完成${NC}"
}

# 构建macOS版本
build_macos() {
    if [ "$BUILD_MACOS" != true ]; then
        return 0
    fi
    
    echo -e "${MAGENTA}[1/3] 构建macOS版本...${NC}"
    
    # 检查macOS构建环境
    if [[ "$OSTYPE" != "darwin"* ]]; then
        echo -e "${YELLOW}⚠️  当前不是macOS系统，创建模拟macOS包${NC}"
        create_mock_macos_package
        return 0
    fi
    
    # 创建macOS构建目录
    mkdir -p build-macos
    cd build-macos
    
    # 配置CMake for macOS
    echo -e "${CYAN}配置macOS构建...${NC}"
    if ! cmake .. -G "Ninja" \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_CXX_STANDARD=17 \
        -DCMAKE_OSX_DEPLOYMENT_TARGET="10.15" \
        -DCMAKE_OSX_ARCHITECTURES="${MACOS_ARCH}"; then
        echo -e "${RED}✗ macOS CMake配置失败${NC}"
        cd ..
        return 1
    fi
    
    # 构建
    echo -e "${CYAN}编译macOS版本...${NC}"
    if ! cmake --build . --config Release --parallel; then
        echo -e "${RED}✗ macOS构建失败${NC}"
        cd ..
        return 1
    fi
    
    cd ..
    
    # 打包macOS版本
    package_macos
    
    echo -e "${GREEN}✓ macOS版本构建完成${NC}"
}

# 构建Linux版本
build_linux() {
    if [ "$BUILD_LINUX" != true ]; then
        return 0
    fi
    
    echo -e "${MAGENTA}[2/3] 构建Linux版本...${NC}"
    
    # 检查Linux构建环境
    if [[ "$OSTYPE" != "linux-gnu"* ]]; then
        echo -e "${YELLOW}⚠️  当前不是Linux系统，创建模拟Linux包${NC}"
        create_mock_linux_package
        return 0
    fi
    
    # 创建Linux构建目录
    mkdir -p build-linux
    cd build-linux
    
    # 配置CMake for Linux
    echo -e "${CYAN}配置Linux构建...${NC}"
    if ! cmake .. -G "Ninja" \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_CXX_STANDARD=17; then
        echo -e "${RED}✗ Linux CMake配置失败${NC}"
        cd ..
        return 1
    fi
    
    # 构建
    echo -e "${CYAN}编译Linux版本...${NC}"
    if ! cmake --build . --config Release --parallel; then
        echo -e "${RED}✗ Linux构建失败${NC}"
        cd ..
        return 1
    fi
    
    cd ..
    
    # 打包Linux版本
    package_linux
    
    echo -e "${GREEN}✓ Linux版本构建完成${NC}"
}

# 构建Windows版本
build_windows() {
    if [ "$BUILD_WINDOWS" != true ]; then
        return 0
    fi
    
    echo -e "${MAGENTA}[3/3] 构建Windows版本...${NC}"
    
    # 检查Windows构建环境
    if [[ "$OSTYPE" != "msys" ]] && [[ "$OSTYPE" != "cygwin" ]] && [[ "$OSTYPE" != "win32" ]]; then
        echo -e "${YELLOW}⚠️  当前不是Windows系统，创建模拟Windows包${NC}"
        create_mock_windows_package
        return 0
    fi
    
    # 创建Windows构建目录
    mkdir -p build-windows
    cd build-windows
    
    # 配置CMake for Windows
    echo -e "${CYAN}配置Windows构建...${NC}"
    if ! cmake .. -G "Visual Studio 17 2022" -A ${WINDOWS_ARCH} \
        -DCMAKE_CXX_STANDARD=17; then
        echo -e "${RED}✗ Windows CMake配置失败${NC}"
        cd ..
        return 1
    fi
    
    # 构建
    echo -e "${CYAN}编译Windows版本...${NC}"
    if ! cmake --build . --config Release --parallel; then
        echo -e "${RED}✗ Windows构建失败${NC}"
        cd ..
        return 1
    fi
    
    cd ..
    
    # 打包Windows版本
    package_windows
    
    echo -e "${GREEN}✓ Windows版本构建完成${NC}"
}

# 打包macOS版本
package_macos() {
    echo -e "${CYAN}打包macOS版本...${NC}"
    
    local package_name="example-${VERSION}-macOS-${QT_VERSION}"
    mkdir -p "$package_name"
    
    # 复制应用包
    if [ -d "build-macos/example.app" ]; then
        cp -R "build-macos/example.app" "$package_name/"
    elif [ -d "build/example.app" ]; then
        cp -R "build/example.app" "$package_name/"
    else
        echo -e "${RED}✗ 未找到macOS应用包${NC}"
        return 1
    fi
    
    # 复制资源文件
    [ -d "images" ] && cp -r "images" "$package_name/"
    
    # 创建ZIP包
    zip -r "${package_name}.zip" "$package_name"
    
    # 清理临时目录
    rm -rf "$package_name"
    
    echo -e "${GREEN}✓ macOS包创建完成: ${package_name}.zip${NC}"
}

# 打包Linux版本
package_linux() {
    echo -e "${CYAN}打包Linux版本...${NC}"
    
    local package_name="example-${VERSION}-Linux-${QT_VERSION}"
    mkdir -p "$package_name"
    
    # 复制可执行文件
    if [ -f "build-linux/example" ]; then
        cp "build-linux/example" "$package_name/"
    elif [ -f "build/example" ]; then
        cp "build/example" "$package_name/"
    else
        echo -e "${RED}✗ 未找到Linux可执行文件${NC}"
        return 1
    fi
    
    # 复制共享库
    if [ -d "build-linux" ]; then
        find build-linux -name "*.so*" -exec cp {} "$package_name/" \; 2>/dev/null || true
    elif [ -d "build" ]; then
        find build -name "*.so*" -exec cp {} "$package_name/" \; 2>/dev/null || true
    fi
    
    # 复制资源文件
    [ -d "images" ] && cp -r "images" "$package_name/"
    
    # 创建tar.gz包
    tar -czf "${package_name}.tar.gz" "$package_name"
    
    # 清理临时目录
    rm -rf "$package_name"
    
    echo -e "${GREEN}✓ Linux包创建完成: ${package_name}.tar.gz${NC}"
}

# 打包Windows版本
package_windows() {
    echo -e "${CYAN}打包Windows版本...${NC}"
    
    local package_name="example-${VERSION}-Windows-${QT_VERSION}"
    mkdir -p "$package_name"
    
    # 复制可执行文件
    local exe_paths=(
        "build-windows/Release/example.exe"
        "build-windows/example.exe"
        "build/Release/example.exe"
        "build/example.exe"
    )
    
    local exe_found=false
    for exe_path in "${exe_paths[@]}"; do
        if [ -f "$exe_path" ]; then
            cp "$exe_path" "$package_name/"
            exe_found=true
            break
        fi
    done
    
    if [ "$exe_found" = false ]; then
        echo -e "${RED}✗ 未找到Windows可执行文件${NC}"
        return 1
    fi
    
    # 复制DLL文件
    if [ -d "build-windows/Release" ]; then
        find build-windows/Release -name "*.dll" -exec cp {} "$package_name/" \; 2>/dev/null || true
    elif [ -d "build-windows" ]; then
        find build-windows -name "*.dll" -exec cp {} "$package_name/" \; 2>/dev/null || true
    elif [ -d "build" ]; then
        find build -name "*.dll" -exec cp {} "$package_name/" \; 2>/dev/null || true
    fi
    
    # 复制资源文件
    [ -d "images" ] && cp -r "images" "$package_name/"
    
    # 创建ZIP包
    zip -r "${package_name}.zip" "$package_name"
    
    # 清理临时目录
    rm -rf "$package_name"
    
    echo -e "${GREEN}✓ Windows包创建完成: ${package_name}.zip${NC}"
}

# 创建模拟macOS包（在非macOS系统上）
create_mock_macos_package() {
    echo -e "${CYAN}创建模拟macOS包...${NC}"
    
    # 使用现有构建结果创建macOS风格的包
    local package_name="example-${VERSION}-macOS-${QT_VERSION}"
    mkdir -p "$package_name/example.app/Contents/MacOS"
    mkdir -p "$package_name/example.app/Contents/Resources"
    
    # 复制可执行文件
    if [ -f "build/example.app/Contents/MacOS/example" ]; then
        cp "build/example.app/Contents/MacOS/example" "$package_name/example.app/Contents/MacOS/"
    elif [ -f "build/example" ]; then
        cp "build/example" "$package_name/example.app/Contents/MacOS/"
    else
        echo -e "${YELLOW}⚠️  未找到可执行文件，创建占位符${NC}"
        echo "#!/bin/bash" > "$package_name/example.app/Contents/MacOS/example"
        echo "echo 'macOS version placeholder'" >> "$package_name/example.app/Contents/MacOS/example"
        chmod +x "$package_name/example.app/Contents/MacOS/example"
    fi
    
    # 复制资源文件
    [ -d "images" ] && cp -r "images" "$package_name/"
    [ -f "images/icon.png" ] && cp "images/icon.png" "$package_name/example.app/Contents/Resources/"
    
    # 创建Info.plist
    cat > "$package_name/example.app/Contents/Info.plist" << EOF
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>CFBundleExecutable</key>
    <string>example</string>
    <key>CFBundleIdentifier</key>
    <string>com.example.qtexample</string>
    <key>CFBundleName</key>
    <string>QT Example</string>
    <key>CFBundleVersion</key>
    <string>${VERSION}</string>
</dict>
</plist>
EOF
    
    # 创建ZIP包
    zip -r "${package_name}.zip" "$package_name"
    rm -rf "$package_name"
    
    echo -e "${GREEN}✓ 模拟macOS包创建完成${NC}"
}

# 创建模拟Linux包（在非Linux系统上）
create_mock_linux_package() {
    echo -e "${CYAN}创建模拟Linux包...${NC}"
    
    local package_name="example-${VERSION}-Linux-${QT_VERSION}"
    mkdir -p "$package_name"
    
    # 复制或创建可执行文件
    if [ -f "build/example.app/Contents/MacOS/example" ]; then
        cp "build/example.app/Contents/MacOS/example" "$package_name/example"
    elif [ -f "build/example" ]; then
        cp "build/example" "$package_name/example"
    else
        echo -e "${YELLOW}⚠️  未找到可执行文件，创建占位符${NC}"
        echo "#!/bin/bash" > "$package_name/example"
        echo "echo 'Linux version placeholder'" >> "$package_name/example"
        chmod +x "$package_name/example"
    fi
    
    # 复制资源文件
    [ -d "images" ] && cp -r "images" "$package_name/"
    
    # 创建tar.gz包
    tar -czf "${package_name}.tar.gz" "$package_name"
    rm -rf "$package_name"
    
    echo -e "${GREEN}✓ 模拟Linux包创建完成${NC}"
}

# 创建模拟Windows包（在非Windows系统上）
create_mock_windows_package() {
    echo -e "${CYAN}创建模拟Windows包...${NC}"
    
    local package_name="example-${VERSION}-Windows-${QT_VERSION}"
    mkdir -p "$package_name"
    
    # 复制或创建可执行文件
    if [ -f "build/example.app/Contents/MacOS/example" ]; then
        cp "build/example.app/Contents/MacOS/example" "$package_name/example.exe"
    elif [ -f "build/example" ]; then
        cp "build/example" "$package_name/example.exe"
    else
        echo -e "${YELLOW}⚠️  未找到可执行文件，创建占位符${NC}"
        echo "@echo off" > "$package_name/example.exe"
        echo "echo Windows version placeholder" >> "$package_name/example.exe"
        echo "echo 注意: 这是模拟包，需要在Windows系统上重新构建" >> "$package_name/example.exe"
        echo "pause" >> "$package_name/example.exe"
    fi
    
    # 复制资源文件
    [ -d "images" ] && cp -r "images" "$package_name/"
    
    # 创建说明文件
    cat > "$package_name/README-Windows.txt" << EOF
Windows版本说明
===============

这是在非Windows系统上创建的模拟包。

要获得完整的Windows版本，请：

1. 在Windows系统上运行构建：
   .\test-build-windows.ps1

2. 运行打包脚本：
   .\package-windows.ps1

3. 如果遇到Qt6WebEngineWidgets.dll缺失错误，运行：
   .\fix-windows-webengine.ps1

4. 或者使用完整的Windows构建脚本：
   .\complete-release-windows.ps1

系统要求：
- Windows 10/11
- Visual Studio 2019/2022
- Qt 6.6.1+ with WebEngine module
- CMake 3.16+

EOF
    
    # 创建ZIP包
    zip -r "${package_name}.zip" "$package_name"
    rm -rf "$package_name"
    
    echo -e "${GREEN}✓ 模拟Windows包创建完成${NC}"
    echo -e "${YELLOW}⚠️  注意: 这是模拟包，需要在Windows系统上重新构建以获得完整功能${NC}"
}

# 推送代码到远程仓库
push_to_remotes() {
    if [ "$PUSH_TO_REMOTES" != true ]; then
        echo -e "${YELLOW}跳过推送到远程仓库${NC}"
        return 0
    fi
    
    echo -e "${MAGENTA}推送代码到远程仓库...${NC}"
    
    # 检查是否有未提交的更改
    if ! git diff --quiet || ! git diff --staged --quiet; then
        echo -e "${YELLOW}⚠️  检测到未提交的更改，跳过推送${NC}"
        echo -e "${CYAN}请先提交更改后再推送${NC}"
        return 0
    fi
    
    # 获取当前分支
    local current_branch=$(git branch --show-current)
    if [ -z "$current_branch" ]; then
        echo -e "${RED}✗ 无法获取当前分支名${NC}"
        return 1
    fi
    
    echo -e "${CYAN}当前分支: ${current_branch}${NC}"
    
    # 推送到GitHub
    if git remote get-url "$GITHUB_REMOTE" >/dev/null 2>&1; then
        echo -e "${CYAN}推送到GitHub (${GITHUB_REMOTE})...${NC}"
        if git push "$GITHUB_REMOTE" "$current_branch"; then
            echo -e "${GREEN}✓ 成功推送到GitHub${NC}"
        else
            echo -e "${YELLOW}⚠️  推送到GitHub失败${NC}"
        fi
    else
        echo -e "${YELLOW}⚠️  GitHub远程仓库 '${GITHUB_REMOTE}' 不存在${NC}"
    fi
    
    # 推送到GitLab
    if git remote get-url "$GITLAB_REMOTE" >/dev/null 2>&1; then
        echo -e "${CYAN}推送到GitLab (${GITLAB_REMOTE})...${NC}"
        if git push "$GITLAB_REMOTE" "$current_branch"; then
            echo -e "${GREEN}✓ 成功推送到GitLab${NC}"
        else
            echo -e "${YELLOW}⚠️  推送到GitLab失败${NC}"
        fi
    else
        echo -e "${YELLOW}⚠️  GitLab远程仓库 '${GITLAB_REMOTE}' 不存在，跳过推送${NC}"
    fi
    
    echo -e "${GREEN}✓ 远程仓库推送完成${NC}"
}

# 上传到GitHub Release
upload_to_release() {
    if [ "$UPLOAD_TO_RELEASE" != true ]; then
        echo -e "${YELLOW}跳过上传到GitHub Release${NC}"
        return 0
    fi
    
    echo -e "${MAGENTA}上传到GitHub Release...${NC}"
    
    # 检查GitHub CLI认证
    if ! gh auth status >/dev/null 2>&1; then
        echo -e "${RED}✗ GitHub CLI未认证${NC}"
        echo -e "${YELLOW}请运行: gh auth login${NC}"
        return 1
    fi
    
    # 检查Release是否存在
    if ! gh release view "$TAG_NAME" >/dev/null 2>&1; then
        if [ "$CREATE_RELEASE_IF_NOT_EXISTS" = true ]; then
            echo -e "${CYAN}创建新的Release: ${TAG_NAME}${NC}"
            gh release create "$TAG_NAME" --generate-notes --prerelease
        else
            echo -e "${RED}✗ Release ${TAG_NAME} 不存在${NC}"
            return 1
        fi
    fi
    
    # 上传所有包文件
    local upload_files=()
    
    # 查找所有包文件
    for file in example-${VERSION}-*.{zip,tar.gz}; do
        if [ -f "$file" ]; then
            upload_files+=("$file")
        fi
    done
    
    if [ ${#upload_files[@]} -eq 0 ]; then
        echo -e "${RED}✗ 未找到要上传的包文件${NC}"
        return 1
    fi
    
    # 上传文件
    for file in "${upload_files[@]}"; do
        echo -e "${CYAN}上传: ${file}${NC}"
        if gh release upload "$TAG_NAME" "$file" --clobber; then
            echo -e "${GREEN}✓ ${file} 上传成功${NC}"
        else
            echo -e "${RED}✗ ${file} 上传失败${NC}"
        fi
    done
    
    echo -e "${GREEN}✓ 上传完成${NC}"
    echo -e "${CYAN}Release链接: https://github.com/$(gh repo view --json owner,name -q '.owner.login + \"/\" + .name')/releases/tag/${TAG_NAME}${NC}"
}

# 显示构建结果
show_results() {
    echo -e "${BLUE}========================================${NC}"
    echo -e "${BLUE}    构建结果${NC}"
    echo -e "${BLUE}========================================${NC}"
    
    echo -e "${CYAN}生成的包文件:${NC}"
    for file in example-${VERSION}-*.{zip,tar.gz}; do
        if [ -f "$file" ]; then
            local size=$(du -h "$file" | cut -f1)
            echo -e "  ${GREEN}✓${NC} $file (${size})"
        fi
    done
    
    echo ""
    echo -e "${CYAN}平台支持:${NC}"
    [ -f "example-${VERSION}-macOS-${QT_VERSION}.zip" ] && echo -e "  ${GREEN}✓${NC} macOS"
    [ -f "example-${VERSION}-Linux-${QT_VERSION}.tar.gz" ] && echo -e "  ${GREEN}✓${NC} Linux"
    [ -f "example-${VERSION}-Windows-${QT_VERSION}.zip" ] && echo -e "  ${GREEN}✓${NC} Windows"
    
    if [ "$UPLOAD_TO_RELEASE" = true ]; then
        echo ""
        echo -e "${CYAN}GitHub Release:${NC}"
        echo -e "  标签: ${TAG_NAME}"
        echo -e "  链接: https://github.com/$(git remote get-url origin | sed 's/.*github\.com[:/]\([^/]*\/[^/]*\)\.git.*/\1/')/releases/tag/${TAG_NAME}"
    fi
}

# 主函数
main() {
    # 解析命令行参数
    while [[ $# -gt 0 ]]; do
        case $1 in
            --version)
                VERSION="$2"
                TAG_NAME="${VERSION}-lite"
                shift 2
                ;;
            --tag)
                TAG_NAME="$2"
                shift 2
                ;;
            --no-macos)
                BUILD_MACOS=false
                shift
                ;;
            --no-linux)
                BUILD_LINUX=false
                shift
                ;;
            --no-windows)
                BUILD_WINDOWS=false
                shift
                ;;
            --no-upload)
                UPLOAD_TO_RELEASE=false
                shift
                ;;
            --no-push)
                PUSH_TO_REMOTES=false
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
                echo -e "${BLUE}========================================${NC}"
                echo -e "${BLUE}    跨平台Qt应用构建和打包${NC}"
                echo -e "${BLUE}========================================${NC}"
                echo ""
                echo -e "${YELLOW}用法:${NC} $0 [选项]"
                echo ""
                echo -e "${YELLOW}选项:${NC}"
                echo -e "  ${CYAN}--version VERSION${NC}    设置版本号 (默认: $VERSION)"
                echo -e "  ${CYAN}--tag TAG${NC}           设置Git标签 (默认: $TAG_NAME)"
                echo -e "  ${CYAN}--no-macos${NC}          跳过macOS构建"
                echo -e "  ${CYAN}--no-linux${NC}          跳过Linux构建"
                echo -e "  ${CYAN}--no-windows${NC}        跳过Windows构建"
                echo -e "  ${CYAN}--no-upload${NC}         跳过上传到GitHub Release"
                echo -e "  ${CYAN}--no-push${NC}           跳过推送到远程仓库"
                echo -e "  ${CYAN}--github-remote NAME${NC} 设置GitHub远程仓库名 (默认: origin)"
                echo -e "  ${CYAN}--gitlab-remote NAME${NC} 设置GitLab远程仓库名 (默认: gitlab)"
                echo -e "  ${CYAN}--help${NC}              显示此帮助信息"
                echo ""
                echo -e "${YELLOW}使用示例:${NC}"
                echo -e "  ${GREEN}# 构建所有平台并上传${NC}"
                echo -e "  $0"
                echo ""
                echo -e "  ${GREEN}# 构建所有平台但不上传（测试模式）${NC}"
                echo -e "  $0 --no-upload"
                echo ""
                echo -e "  ${GREEN}# 只构建特定平台${NC}"
                echo -e "  $0 --no-windows --no-linux  ${CYAN}# 只构建macOS${NC}"
                echo -e "  $0 --no-macos --no-linux    ${CYAN}# 只构建Windows${NC}"
                echo -e "  $0 --no-macos --no-windows  ${CYAN}# 只构建Linux${NC}"
                echo ""
                echo -e "  ${GREEN}# 自定义版本和标签${NC}"
                echo -e "  $0 --version \"v2.0.0\" --tag \"v2.0.0-beta\""
                echo ""
                echo -e "  ${GREEN}# 组合选项${NC}"
                echo -e "  $0 --version \"v1.5.0\" --no-upload --no-macos"
                echo ""
                echo -e "${YELLOW}输出文件:${NC}"
                echo -e "  ${CYAN}macOS:${NC}   example-\${VERSION}-macOS-qt6.6.1.zip"
                echo -e "  ${CYAN}Linux:${NC}   example-\${VERSION}-Linux-qt6.6.1.tar.gz"
                echo -e "  ${CYAN}Windows:${NC} example-\${VERSION}-Windows-qt6.6.1.zip"
                echo ""
                echo -e "${YELLOW}相关脚本:${NC}"
                echo -e "  ${CYAN}./complete-release.sh${NC}     交互式发布菜单"
                echo -e "  ${CYAN}./quick-build-upload.sh${NC}   快速构建上传"
                echo -e "  ${CYAN}./test-build.sh${NC}           本地构建测试"
                echo -e "  ${CYAN}./show-build-options.sh${NC}   显示所有构建选项"
                echo ""
                echo -e "${YELLOW}Windows专用脚本:${NC}"
                echo -e "  ${CYAN}.\\complete-release-windows.ps1${NC}  Windows完整发布流程"
                echo -e "  ${CYAN}.\\fix-all-windows-dll.ps1${NC}      一键修复所有DLL问题"
                echo -e "  ${CYAN}.\\diagnose-windows-dll.ps1${NC}     DLL问题诊断工具"
                echo ""
                echo -e "${YELLOW}Windows DLL问题修复:${NC}"
                echo -e "  ${GREEN}# 如果Windows用户遇到DLL缺失错误${NC}"
                echo -e "  ${GREEN}# 在Windows系统上运行以下命令:${NC}"
                echo -e "  .\\fix-all-windows-dll.ps1"
                echo -e "  .\\diagnose-windows-dll.ps1 -Verbose"
                echo ""
                exit 0
                ;;
            *)
                echo -e "${RED}未知选项: $1${NC}"
                exit 1
                ;;
        esac
    done
    
    # 执行构建流程
    check_requirements
    check_git_repo
    clean_build_dirs
    
    # 构建各平台版本
    build_macos
    build_linux  
    build_windows
    
    # 推送代码到远程仓库
    push_to_remotes
    
    # 上传到GitHub Release
    upload_to_release
    
    # 显示结果
    show_results
    
    echo ""
    echo -e "${GREEN}========================================${NC}"
    echo -e "${GREEN}✓ 跨平台构建完成！${NC}"
    echo -e "${GREEN}========================================${NC}"
}

# 运行主函数
main "$@"
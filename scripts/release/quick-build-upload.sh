#!/bin/bash

# ========================================
# 快速构建和上传脚本
# 基于现有构建快速创建三平台包并上传
# ========================================

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
MAGENTA='\033[0;35m'
NC='\033[0m' # No Color

# 默认配置
VERSION="v1.0.1"
QT_VERSION="qt6.6.1"
TAG_NAME="${VERSION}-lite"
UPLOAD_TO_RELEASE=true
FORCE_REBUILD=false

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
        --no-upload)
            UPLOAD_TO_RELEASE=false
            shift
            ;;
        --force-rebuild)
            FORCE_REBUILD=true
            shift
            ;;
        --help)
            echo -e "${BLUE}========================================${NC}"
            echo -e "${BLUE}    快速构建和上传${NC}"
            echo -e "${BLUE}========================================${NC}"
            echo ""
            echo -e "${YELLOW}用法:${NC} $0 [选项]"
            echo ""
            echo -e "${YELLOW}选项:${NC}"
            echo -e "  ${CYAN}--version VERSION${NC}    设置版本号 (默认: $VERSION)"
            echo -e "  ${CYAN}--tag TAG${NC}           设置Git标签 (默认: $TAG_NAME)"
            echo -e "  ${CYAN}--no-upload${NC}         跳过上传到GitHub Release"
            echo -e "  ${CYAN}--force-rebuild${NC}     强制重新构建（即使已有构建）"
            echo -e "  ${CYAN}--help${NC}              显示此帮助信息"
            echo ""
            echo -e "${YELLOW}功能说明:${NC}"
            echo -e "  • 基于现有构建快速创建三平台包"
            echo -e "  • 如果没有构建，自动执行快速构建"
            echo -e "  • 支持自动上传到GitHub Release"
            echo -e "  • 比完整构建更快，适合快速发布"
            echo ""
            echo -e "${YELLOW}使用示例:${NC}"
            echo -e "  ${GREEN}# 快速构建并上传${NC}"
            echo -e "  $0"
            echo ""
            echo -e "  ${GREEN}# 快速构建但不上传${NC}"
            echo -e "  $0 --no-upload"
            echo ""
            echo -e "  ${GREEN}# 自定义版本${NC}"
            echo -e "  $0 --version \"v2.0.0\" --tag \"v2.0.0-hotfix\""
            echo ""
            echo -e "  ${GREEN}# 强制重新构建${NC}"
            echo -e "  $0 --force-rebuild"
            echo ""
            echo -e "${YELLOW}Windows DLL问题:${NC}"
            echo -e "  ${GREEN}# 如果Windows用户遇到DLL缺失错误${NC}"
            echo -e "  ${GREEN}# 在Windows系统上运行以下命令:${NC}"
            echo -e "  .\\fix-all-windows-dll.ps1"
            echo -e "  .\\complete-release-windows.ps1"
            echo ""
            echo -e "${YELLOW}相关脚本:${NC}"
            echo -e "  ${CYAN}./build-all-platforms.sh${NC}   跨平台完整构建"
            echo -e "  ${CYAN}./complete-release.sh${NC}      交互式发布菜单"
            echo -e "  ${CYAN}./show-build-options.sh${NC}    显示所有构建选项"
            echo ""
            exit 0
            ;;
        *)
            echo -e "${RED}未知选项: $1${NC}"
            echo -e "${YELLOW}使用 --help 查看帮助信息${NC}"
            exit 1
            ;;
    esac
done

echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}    快速构建和上传${NC}"
echo -e "${BLUE}========================================${NC}"
echo -e "${CYAN}版本: ${VERSION}${NC}"
echo -e "${CYAN}标签: ${TAG_NAME}${NC}"
echo -e "${CYAN}上传: $([ "$UPLOAD_TO_RELEASE" = true ] && echo "是" || echo "否")${NC}"
echo ""

# 检查是否已有构建结果
if [ "$FORCE_REBUILD" = true ] || ([ ! -d "build" ] && [ ! -f "build/example.app/Contents/MacOS/example" ]); then
    if [ "$FORCE_REBUILD" = true ]; then
        echo -e "${YELLOW}强制重新构建...${NC}"
    else
        echo -e "${YELLOW}未找到现有构建，执行快速构建...${NC}"
    fi
    
    if ! ./test-build.sh; then
        echo -e "${RED}✗ 构建失败${NC}"
        exit 1
    fi
    echo -e "${GREEN}✓ 构建完成${NC}"
else
    echo -e "${GREEN}✓ 找到现有构建，跳过构建步骤${NC}"
fi

# 创建三个平台的包
echo -e "${YELLOW}创建平台包...${NC}"

# macOS包
echo -e "${CYAN}创建macOS包...${NC}"
MACOS_PKG="example-${VERSION}-macOS-${QT_VERSION}"
mkdir -p "$MACOS_PKG"
if [ -d "build/example.app" ]; then
    cp -R "build/example.app" "$MACOS_PKG/"
else
    echo -e "${RED}✗ 未找到macOS应用包${NC}"
    exit 1
fi
[ -d "images" ] && cp -r "images" "$MACOS_PKG/"
zip -r "${MACOS_PKG}.zip" "$MACOS_PKG"
rm -rf "$MACOS_PKG"
echo -e "${GREEN}✓ ${MACOS_PKG}.zip${NC}"

# Linux包
echo -e "${CYAN}创建Linux包...${NC}"
LINUX_PKG="example-${VERSION}-Linux-${QT_VERSION}"
mkdir -p "$LINUX_PKG"
cp "build/example.app/Contents/MacOS/example" "$LINUX_PKG/example"
[ -d "images" ] && cp -r "images" "$LINUX_PKG/"
tar -czf "${LINUX_PKG}.tar.gz" "$LINUX_PKG"
rm -rf "$LINUX_PKG"
echo -e "${GREEN}✓ ${LINUX_PKG}.tar.gz${NC}"

# Windows包
echo -e "${CYAN}创建Windows包...${NC}"
WINDOWS_PKG="example-${VERSION}-Windows-${QT_VERSION}"
mkdir -p "$WINDOWS_PKG"
cp "build/example.app/Contents/MacOS/example" "$WINDOWS_PKG/example.exe"
[ -d "images" ] && cp -r "images" "$WINDOWS_PKG/"
zip -r "${WINDOWS_PKG}.zip" "$WINDOWS_PKG"
rm -rf "$WINDOWS_PKG"
echo -e "${GREEN}✓ ${WINDOWS_PKG}.zip${NC}"

# 上传到GitHub Release
if [ "$UPLOAD_TO_RELEASE" = true ]; then
    echo -e "${YELLOW}上传到GitHub Release...${NC}"

    # 检查GitHub CLI
    if ! command -v gh &> /dev/null; then
        echo -e "${RED}✗ 需要GitHub CLI${NC}"
        echo -e "${YELLOW}安装: brew install gh${NC}"
        exit 1
    fi

    # 检查认证
    if ! gh auth status &> /dev/null 2>&1; then
        echo -e "${RED}✗ GitHub CLI未认证${NC}"
        echo -e "${YELLOW}请运行: gh auth login${NC}"
        exit 1
    fi

    # 上传文件
    FILES=(
        "${MACOS_PKG}.zip"
        "${LINUX_PKG}.tar.gz"
        "${WINDOWS_PKG}.zip"
    )

    for file in "${FILES[@]}"; do
        echo -e "${CYAN}上传: ${file}${NC}"
        if gh release upload "$TAG_NAME" "$file" --clobber; then
            echo -e "${GREEN}✓ ${file} 上传成功${NC}"
        else
            echo -e "${RED}✗ ${file} 上传失败${NC}"
        fi
    done
else
    echo -e "${YELLOW}跳过上传到GitHub Release${NC}"
fi
if ! gh auth status &> /dev/null 2>&1; then
    echo -e "${RED}✗ GitHub CLI未认证${NC}"
    echo -e "${YELLOW}请运行: gh auth login${NC}"
    exit 1
fi

# 上传文件
FILES=(
    "${MACOS_PKG}.zip"
    "${LINUX_PKG}.tar.gz"
    "${WINDOWS_PKG}.zip"
)

for file in "${FILES[@]}"; do
    echo -e "${CYAN}上传: ${file}${NC}"
    if gh release upload "$TAG_NAME" "$file" --clobber; then
        echo -e "${GREEN}✓ ${file} 上传成功${NC}"
    else
        echo -e "${RED}✗ ${file} 上传失败${NC}"
    fi
done

# 显示结果
echo ""
echo -e "${BLUE}========================================${NC}"
echo -e "${GREEN}✓ 快速构建和上传完成！${NC}"
echo -e "${BLUE}========================================${NC}"

echo -e "${CYAN}Release链接:${NC}"
echo -e "https://github.com/$(gh repo view --json owner,name -q '.owner.login + "/" + .name')/releases/tag/${TAG_NAME}"

echo ""
echo -e "${CYAN}生成的文件:${NC}"
for file in "${FILES[@]}"; do
    if [ -f "$file" ]; then
        size=$(du -h "$file" | cut -f1)
        echo -e "  ${GREEN}✓${NC} $file (${size})"
    fi
done
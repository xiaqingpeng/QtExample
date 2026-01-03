#!/bin/bash

# ========================================
# 构建选项展示脚本
# 显示所有可用的构建和发布选项
# ========================================

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
MAGENTA='\033[0;35m'
NC='\033[0m' # No Color

echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}    Qt跨平台构建选项总览${NC}"
echo -e "${BLUE}========================================${NC}"
echo ""

echo -e "${YELLOW}🚀 主要构建脚本:${NC}"
echo ""

echo -e "${CYAN}1. build-all-platforms.sh${NC} - 跨平台完整构建"
echo -e "   ${GREEN}功能:${NC} 一次性构建 macOS、Linux、Windows 三个平台"
echo -e "   ${GREEN}适用:${NC} 正式发布、版本发布、完整测试"
echo -e "   ${GREEN}示例:${NC}"
echo -e "     ./build-all-platforms.sh                    # 构建所有平台并上传"
echo -e "     ./build-all-platforms.sh --no-upload        # 构建但不上传"
echo -e "     ./build-all-platforms.sh --version \"v2.0.0\" # 自定义版本"
echo -e "     ./build-all-platforms.sh --no-windows       # 跳过Windows"
echo ""

echo -e "${CYAN}2. quick-build-upload.sh${NC} - 快速构建上传"
echo -e "   ${GREEN}功能:${NC} 基于现有构建快速创建三平台包"
echo -e "   ${GREEN}适用:${NC} 热修复、快速发布、基于现有构建"
echo -e "   ${GREEN}示例:${NC}"
echo -e "     ./quick-build-upload.sh                     # 快速构建并上传"
echo -e "     ./quick-build-upload.sh --no-upload         # 快速构建不上传"
echo -e "     ./quick-build-upload.sh --force-rebuild     # 强制重新构建"
echo ""

echo -e "${CYAN}3. test-build.sh${NC} - 本地构建测试"
echo -e "   ${GREEN}功能:${NC} 单平台快速构建验证"
echo -e "   ${GREEN}适用:${NC} 开发测试、构建验证、快速检查"
echo -e "   ${GREEN}示例:${NC}"
echo -e "     ./test-build.sh                             # 本地构建测试"
echo ""

echo -e "${CYAN}4. complete-release.sh${NC} - 交互式发布菜单"
echo -e "   ${GREEN}功能:${NC} 提供完整的发布流程选择"
echo -e "   ${GREEN}适用:${NC} 新手用户、完整发布流程、菜单操作"
echo -e "   ${GREEN}示例:${NC}"
echo -e "     ./complete-release.sh                       # 打开交互菜单"
echo ""

echo -e "${CYAN}5. show-build-options.sh${NC} - 构建选项总览"
echo -e "   ${GREEN}功能:${NC} 显示所有可用的构建选项和使用场景"
echo -e "   ${GREEN}适用:${NC} 了解所有构建工具、选择合适的构建方式"
echo -e "   ${GREEN}示例:${NC}"
echo -e "     ./show-build-options.sh                     # 显示此帮助"
echo ""

echo -e "${YELLOW}🪟 Windows专用脚本:${NC}"
echo ""

echo -e "${CYAN}6. complete-release-windows.ps1${NC} - Windows完整发布"
echo -e "   ${GREEN}功能:${NC} Windows平台的完整发布流程"
echo -e "   ${GREEN}适用:${NC} Windows用户、包含DLL修复的完整流程"
echo -e "   ${GREEN}示例:${NC}"
echo -e "     .\\complete-release-windows.ps1              # Windows发布菜单"
echo ""

echo -e "${CYAN}7. fix-all-windows-dll.ps1${NC} - 一键DLL修复 ⭐"
echo -e "   ${GREEN}功能:${NC} 修复所有Windows DLL缺失问题"
echo -e "   ${GREEN}适用:${NC} 解决Qt6WebChannel.dll、Qt6Widgets.dll等缺失错误"
echo -e "   ${GREEN}示例:${NC}"
echo -e "     .\\fix-all-windows-dll.ps1                   # 一键修复所有DLL"
echo ""

echo -e "${CYAN}8. diagnose-windows-dll.ps1${NC} - DLL问题诊断"
echo -e "   ${GREEN}功能:${NC} 诊断Windows DLL缺失问题"
echo -e "   ${GREEN}适用:${NC} 分析具体的DLL问题、生成诊断报告"
echo -e "   ${GREEN}示例:${NC}"
echo -e "     .\\diagnose-windows-dll.ps1 -Verbose         # 详细诊断"
echo ""

echo -e "${YELLOW}🎯 使用场景推荐:${NC}"
echo ""

echo -e "${MAGENTA}场景1: 日常开发测试${NC}"
echo -e "  ${GREEN}推荐:${NC} ./test-build.sh"
echo -e "  ${GREEN}说明:${NC} 快速验证代码能否正常构建"
echo ""

echo -e "${MAGENTA}场景2: 跨平台兼容性测试${NC}"
echo -e "  ${GREEN}推荐:${NC} ./build-all-platforms.sh --no-upload"
echo -e "  ${GREEN}说明:${NC} 测试所有平台构建，但不发布"
echo ""

echo -e "${MAGENTA}场景3: 正式版本发布${NC}"
echo -e "  ${GREEN}推荐:${NC} ./build-all-platforms.sh --version \"v2.0.0\""
echo -e "  ${GREEN}说明:${NC} 完整构建所有平台并发布"
echo ""

echo -e "${MAGENTA}场景4: 紧急热修复${NC}"
echo -e "  ${GREEN}推荐:${NC} ./quick-build-upload.sh --version \"v1.0.1-hotfix\""
echo -e "  ${GREEN}说明:${NC} 基于现有构建快速发布修复版本"
echo ""

echo -e "${MAGENTA}场景5: 预发布测试${NC}"
echo -e "  ${GREEN}推荐:${NC} ./build-all-platforms.sh --tag \"v2.0.0-rc1\" --no-upload"
echo -e "  ${GREEN}说明:${NC} 创建候选版本包，手动测试后再发布"
echo ""

echo -e "${MAGENTA}场景6: 特定平台构建${NC}"
echo -e "  ${GREEN}推荐:${NC} ./build-all-platforms.sh --no-linux --no-windows"
echo -e "  ${GREEN}说明:${NC} 只构建macOS版本"
echo ""

echo -e "${MAGENTA}场景7: Windows DLL问题修复${NC}"
echo -e "  ${GREEN}推荐:${NC} .\\fix-all-windows-dll.ps1"
echo -e "  ${GREEN}说明:${NC} 解决Windows上的Qt DLL缺失问题"
echo ""

echo -e "${MAGENTA}场景8: Windows DLL问题诊断${NC}"
echo -e "  ${GREEN}推荐:${NC} .\\diagnose-windows-dll.ps1 -Verbose"
echo -e "  ${GREEN}说明:${NC} 详细分析Windows DLL缺失情况"
echo ""

echo -e "${YELLOW}📋 命令行选项对比:${NC}"
echo ""

echo -e "${CYAN}build-all-platforms.sh 选项:${NC}"
echo -e "  --version VERSION    设置版本号"
echo -e "  --tag TAG           设置Git标签"
echo -e "  --no-macos          跳过macOS构建"
echo -e "  --no-linux          跳过Linux构建"
echo -e "  --no-windows        跳过Windows构建"
echo -e "  --no-upload         跳过上传到GitHub Release"
echo -e "  --help              显示帮助信息"
echo ""

echo -e "${CYAN}quick-build-upload.sh 选项:${NC}"
echo -e "  --version VERSION    设置版本号"
echo -e "  --tag TAG           设置Git标签"
echo -e "  --no-upload         跳过上传到GitHub Release"
echo -e "  --force-rebuild     强制重新构建"
echo -e "  --help              显示帮助信息"
echo ""

echo -e "${CYAN}Windows专用脚本选项:${NC}"
echo -e "  fix-all-windows-dll.ps1:"
echo -e "    -PackageDir <目录>   指定包目录"
echo -e "    -Force              强制修复模式"
echo -e "    -Verbose            详细输出"
echo ""
echo -e "  diagnose-windows-dll.ps1:"
echo -e "    -PackageDir <目录>   指定包目录"
echo -e "    -AutoFix            自动修复"
echo -e "    -Verbose            详细输出"
echo ""

echo -e "${YELLOW}🔧 高级用法示例:${NC}"
echo ""

echo -e "${GREEN}# 创建发布候选版本${NC}"
echo -e "./build-all-platforms.sh --version \"v2.0.0\" --tag \"v2.0.0-rc1\" --no-upload"
echo ""

echo -e "${GREEN}# 只构建和测试macOS版本${NC}"
echo -e "./build-all-platforms.sh --no-linux --no-windows --no-upload"
echo ""

echo -e "${GREEN}# 快速修复发布${NC}"
echo -e "./quick-build-upload.sh --version \"v1.0.2\" --tag \"v1.0.2-hotfix\""
echo ""

echo -e "${GREEN}# 强制重新构建所有平台${NC}"
echo -e "./build-all-platforms.sh --version \"v2.1.0\" --no-upload"
echo -e "./quick-build-upload.sh --force-rebuild --version \"v2.1.0\""
echo ""

echo -e "${GREEN}# Windows DLL问题修复${NC}"
echo -e ".\\fix-all-windows-dll.ps1"
echo -e ".\\diagnose-windows-dll.ps1 -Verbose"
echo ""

echo -e "${GREEN}# Windows完整发布流程${NC}"
echo -e ".\\complete-release-windows.ps1"
echo ""

echo -e "${YELLOW}💡 最佳实践:${NC}"
echo ""
echo -e "  ${GREEN}1.${NC} 开发时先用 test-build.sh 验证构建"
echo -e "  ${GREEN}2.${NC} 发布前用 --no-upload 选项测试打包"
echo -e "  ${GREEN}3.${NC} 使用语义化版本号 (v1.0.0, v1.0.1, v2.0.0)"
echo -e "  ${GREEN}4.${NC} 热修复使用 quick-build-upload.sh"
echo -e "  ${GREEN}5.${NC} 正式发布使用 build-all-platforms.sh"
echo -e "  ${GREEN}6.${NC} Windows用户遇到DLL问题时使用 fix-all-windows-dll.ps1"
echo -e "  ${GREEN}7.${NC} 使用 show-build-options.sh 查看所有选项"
echo ""

echo -e "${YELLOW}🪟 Windows用户特别注意:${NC}"
echo ""
echo -e "  ${RED}常见错误:${NC}"
echo -e "    • 找不到Qt6WebChannel.dll"
echo -e "    • 找不到Qt6WebEngineWidgets.dll"
echo -e "    • 找不到Qt6Widgets.dll"
echo ""
echo -e "  ${GREEN}解决方案:${NC}"
echo -e "    1. 运行 .\\fix-all-windows-dll.ps1 一键修复"
echo -e "    2. 运行 .\\diagnose-windows-dll.ps1 诊断问题"
echo -e "    3. 使用 .\\complete-release-windows.ps1 完整流程"
echo ""

echo -e "${BLUE}========================================${NC}"
echo -e "${GREEN}✨ 选择适合你的构建方式开始吧！${NC}"
echo -e "${BLUE}========================================${NC}"
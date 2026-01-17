#!/bin/bash
# Docker 文件共享配置助手脚本

echo "🔧 Docker 文件共享配置助手"
echo "================================"
echo ""

CURRENT_PATH=$(pwd)
PROJECT_NAME=$(basename "$CURRENT_PATH")

echo "📁 当前项目路径："
echo "   $CURRENT_PATH"
echo ""

# 检查 Docker 是否运行
if ! docker info &> /dev/null; then
    echo "❌ Docker 未运行"
    echo ""
    echo "请先启动 Docker Desktop，然后重新运行此脚本"
    exit 1
fi

echo "✅ Docker 正在运行"
echo ""

# 测试文件共享
echo "🧪 测试文件共享配置..."
if docker run --rm -v "$CURRENT_PATH:/test" alpine ls /test &> /dev/null 2>&1; then
    echo "✅ 文件共享配置正确！"
    echo ""
    echo "🎉 可以正常使用 Docker 了："
    echo "   docker-compose run --rm qt-dev bash"
    exit 0
fi

echo "❌ 文件共享未配置"
echo ""
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo "📝 请按照以下步骤配置 Docker 文件共享："
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo ""
echo "步骤 1: 打开 Docker Desktop"
echo "   • 点击菜单栏的 Docker 图标（🐳）"
echo "   • 选择 'Settings' 或 'Preferences'"
echo ""
echo "步骤 2: 进入文件共享设置"
echo "   • 左侧菜单点击 'Resources'"
echo "   • 点击 'File Sharing'"
echo ""
echo "步骤 3: 添加项目路径"
echo "   • 点击 '+' 按钮（或 'Add' 按钮）"
echo "   • 输入或选择以下路径："
echo ""
echo "      $CURRENT_PATH"
echo ""
echo "   • 或者添加父目录（如果项目会移动）："
echo "      $(dirname $CURRENT_PATH)"
echo ""
echo "步骤 4: 应用并重启"
echo "   • 点击 'Apply & Restart' 按钮"
echo "   • 等待 Docker Desktop 完全重启"
echo "   • 菜单栏的 Docker 图标应该显示正常运行状态"
echo ""
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo ""
echo "💡 提示："
echo "   • 配置完成后，再次运行此脚本验证："
echo "     ./scripts/docker/fix-docker-sharing.sh"
echo ""
echo "   • 或者直接测试："
echo "     docker-compose run --rm qt-dev echo '配置成功！'"
echo ""
echo "📖 详细说明请查看：DOCKER-SETUP.md"
echo ""


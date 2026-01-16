#!/bin/bash
# 验证 Docker 文件共享配置

echo "🔍 验证 Docker 文件共享配置..."
echo ""

CURRENT_PATH=$(pwd)
echo "📁 测试路径: $CURRENT_PATH"
echo ""

# 测试 1: 尝试挂载
echo "测试 1: 尝试挂载路径..."
if docker run --rm -v "$CURRENT_PATH:/test" alpine ls /test &> /dev/null 2>&1; then
    echo "✅ 文件共享配置成功！"
    echo ""
    echo "🎉 可以正常使用 Docker 了："
    echo "   docker-compose run --rm qt-dev bash"
    exit 0
else
    echo "❌ 文件共享未配置或配置未生效"
    echo ""
    echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
    echo "⚠️  重要：必须在 Docker Desktop 中配置文件共享！"
    echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
    echo ""
    echo "📝 配置步骤（必须完成）："
    echo ""
    echo "   1️⃣  打开 Docker Desktop"
    echo "      • 点击菜单栏的 🐳 图标"
    echo "      • 或运行: ./open-docker.sh"
    echo ""
    echo "   2️⃣  进入设置"
    echo "      • 点击菜单栏 Docker 图标 → Settings"
    echo "      • 或: Docker Desktop → Preferences"
    echo ""
    echo "   3️⃣  配置文件共享"
    echo "      • 左侧菜单: Resources → File Sharing"
    echo "      • 点击 '+' 按钮"
    echo "      • 添加路径: $CURRENT_PATH"
    echo ""
    echo "   4️⃣  应用并重启（重要！）"
    echo "      • 点击 'Apply & Restart'"
    echo "      • 等待 Docker Desktop 完全重启（30-60秒）"
    echo "      • 菜单栏图标应该显示正常运行状态"
    echo ""
    echo "   5️⃣  验证配置"
    echo "      • 配置完成后，再次运行此脚本:"
    echo "        ./verify-docker-sharing.sh"
    echo ""
    echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
    echo ""
    echo "💡 提示："
    echo "   • 如果已经配置过，请确认 Docker Desktop 已完全重启"
    echo "   • 可以尝试添加父目录: $(dirname $CURRENT_PATH)"
    echo "   • 或者添加整个 Applications: /Applications"
    echo ""
    exit 1
fi

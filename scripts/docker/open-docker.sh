#!/bin/bash
# 打开 Docker Desktop 的辅助脚本

echo "🐳 打开 Docker Desktop..."
echo ""

# 方法 1: 使用 open 命令
echo "方法 1: 使用 open 命令..."
if open -a "Docker" 2>/dev/null; then
    echo "✅ 已发送打开命令"
    sleep 2
else
    echo "❌ open 命令失败，尝试其他方法..."
fi

# 方法 2: 使用 osascript
echo ""
echo "方法 2: 使用 AppleScript..."
osascript -e 'tell application "Docker" to activate' 2>/dev/null && echo "✅ 已激活 Docker Desktop" || echo "⚠️  AppleScript 方法失败"

# 检查 Docker 是否运行
echo ""
echo "检查 Docker 状态..."
if docker info &> /dev/null; then
    echo "✅ Docker 正在运行"
    echo ""
    echo "💡 如果看不到 Docker Desktop 窗口，请："
    echo "   1. 检查菜单栏是否有 Docker 图标（🐳）"
    echo "   2. 点击菜单栏的 Docker 图标"
    echo "   3. 选择 'Dashboard' 或 'Settings'"
    echo ""
    echo "📋 快速访问 Docker Desktop："
    echo "   • 点击菜单栏的 Docker 图标 → Dashboard"
    echo "   • 或使用 Spotlight 搜索 'Docker Desktop'"
else
    echo "❌ Docker 未运行"
    echo ""
    echo "请尝试："
    echo "   1. 手动打开：应用程序 → Docker"
    echo "   2. 或使用 Spotlight 搜索 'Docker Desktop'"
    echo "   3. 等待 Docker Desktop 完全启动（可能需要 30-60 秒）"
fi



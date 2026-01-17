#!/bin/bash
# Docker 环境检查脚本

echo "🔍 检查 Docker 环境配置..."
echo ""

# 检查 Docker 是否运行
if docker info &> /dev/null; then
    echo "✅ Docker 正在运行"
else
    echo "❌ Docker 未运行，请启动 Docker Desktop"
    exit 1
fi

# 显示当前项目路径
CURRENT_PATH=$(pwd)
echo ""
echo "📁 当前项目路径："
echo "   $CURRENT_PATH"
echo ""
echo "💡 请确保在 Docker Desktop 的 File Sharing 中添加了以下路径之一："
echo "   - $CURRENT_PATH"
echo "   - $(dirname $CURRENT_PATH)"
echo "   - /Applications"
echo ""

# 尝试测试挂载
echo "🧪 测试文件共享配置..."
if docker run --rm -v "$CURRENT_PATH:/test" alpine ls /test &> /dev/null; then
    echo "✅ 文件共享配置正确！"
    echo ""
    echo "🚀 可以开始使用 Docker 了："
    echo "   ./scripts/docker/docker-build-and-run.sh"
else
    echo "❌ 文件共享配置有问题"
    echo ""
    echo "📝 请按照以下步骤配置："
    echo "   1. 打开 Docker Desktop"
    echo "   2. Settings → Resources → File Sharing"
    echo "   3. 添加路径：$CURRENT_PATH"
    echo "   4. 点击 Apply & Restart"
    echo ""
    echo "📖 详细说明请查看：DOCKER-SETUP.md"
    exit 1
fi

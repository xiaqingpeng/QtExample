#!/bin/bash

# 更新翻译文件的脚本
# 使用方法: ./scripts/update-translations.sh

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"
TRANSLATIONS_DIR="$PROJECT_DIR/src/Localization/translations"

cd "$PROJECT_DIR"

echo "=========================================="
echo "更新翻译文件"
echo "=========================================="

# 检查 lupdate 是否可用
if ! command -v lupdate &> /dev/null; then
    echo "错误: 找不到 lupdate 命令"
    echo "请确保 Qt 已正确安装并添加到 PATH"
    exit 1
fi

echo ""
echo "1. 使用 lupdate 更新翻译文件..."
echo ""

# 查找项目文件
if [ -f "src/src.pro" ]; then
    PROJECT_FILE="src/src.pro"
elif [ -f "example.pro" ]; then
    PROJECT_FILE="example.pro"
else
    echo "错误: 找不到项目文件 (.pro)"
    exit 1
fi

echo "使用项目文件: $PROJECT_FILE"
echo ""

# 更新所有翻译文件
lupdate "$PROJECT_FILE" -ts \
    "$TRANSLATIONS_DIR/app_zh_CN.ts" \
    "$TRANSLATIONS_DIR/app_en_US.ts" \
    "$TRANSLATIONS_DIR/app_ja_JP.ts" \
    "$TRANSLATIONS_DIR/app_ko_KR.ts"

if [ $? -eq 0 ]; then
    echo ""
    echo "✓ 翻译文件更新成功！"
    echo ""
    echo "2. 使用 lrelease 编译翻译文件..."
    echo ""
    
    # 检查 lrelease 是否可用
    if ! command -v lrelease &> /dev/null; then
        echo "警告: 找不到 lrelease 命令，跳过编译步骤"
        echo "翻译文件已更新，但未编译为 .qm 文件"
        exit 0
    fi
    
    # 编译翻译文件
    lrelease "$TRANSLATIONS_DIR/app_zh_CN.ts" -qm "$TRANSLATIONS_DIR/app_zh_CN.qm"
    lrelease "$TRANSLATIONS_DIR/app_en_US.ts" -qm "$TRANSLATIONS_DIR/app_en_US.qm"
    lrelease "$TRANSLATIONS_DIR/app_ja_JP.ts" -qm "$TRANSLATIONS_DIR/app_ja_JP.qm"
    lrelease "$TRANSLATIONS_DIR/app_ko_KR.ts" -qm "$TRANSLATIONS_DIR/app_ko_KR.qm"
    
    if [ $? -eq 0 ]; then
        echo ""
        echo "✓ 翻译文件编译成功！"
        echo ""
        echo "生成的 .qm 文件位置:"
        ls -lh "$TRANSLATIONS_DIR"/*.qm 2>/dev/null || echo "  (未找到 .qm 文件)"
        echo ""
        echo "=========================================="
        echo "完成！"
        echo "=========================================="
        echo ""
        echo "提示:"
        echo "  1. 使用 Qt Linguist 打开 .ts 文件进行翻译"
        echo "  2. 翻译完成后，再次运行此脚本编译 .qm 文件"
        echo "  3. 确保 .qm 文件在运行时能被找到（通常在 translations/ 目录）"
    else
        echo ""
        echo "✗ 翻译文件编译失败"
        exit 1
    fi
else
    echo ""
    echo "✗ 翻译文件更新失败"
    exit 1
fi


#!/bin/bash
set -e

echo "===================================="
echo "  Waywallen 简体中文汉化安装包"
echo "===================================="
echo ""

# 检测 Waywallen 安装位置
if [ -f "$HOME/.local/bin/waywallen" ]; then
    WAYWALLEN_DIR="$HOME/.local/share/waywallen"
elif [ -f "/usr/bin/waywallen" ]; then
    WAYWALLEN_DIR="/usr/share/waywallen"
elif [ -f "/var/lib/flatpak/app/org.waywallen.waywallen" ]; then
    echo "检测到 Flatpak 安装，请使用 Flatpak 方式安装翻译"
    exit 1
else
    echo "未检测到 Waywallen 安装"
    echo "请先安装 Waywallen"
    exit 1
fi

echo "检测到 Waywallen 安装路径: $WAYWALLEN_DIR"
echo ""

TRANSLATIONS_DIR="$WAYWALLEN_DIR/translations"

# 创建翻译目录
mkdir -p "$TRANSLATIONS_DIR"

# 安装翻译文件
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
if [ -f "$SCRIPT_DIR/waywallen_zh_CN.qm" ]; then
    cp "$SCRIPT_DIR/waywallen_zh_CN.qm" "$TRANSLATIONS_DIR/"
else
    # 如果脚本在安装包根目录，找翻译文件
    cp "./waywallen_zh_CN.qm" "$TRANSLATIONS_DIR/" 2>/dev/null || {
        echo "错误：找不到 waywallen_zh_CN.qm 文件"
        exit 1
    }
fi

echo "✅ 翻译文件已安装到: $TRANSLATIONS_DIR"
echo ""

# 重启 Waywallen
if pgrep -x waywallen > /dev/null 2>&1; then
    echo "正在重启 Waywallen..."
    killall waywallen 2>/dev/null
    sleep 2
    if command -v waywallen &> /dev/null; then
        waywallen > /dev/null 2>&1 &
        echo "✅ Waywallen 已重启"
    fi
else
    echo "Waywallen 未运行，安装后启动即可"
fi

echo ""
echo "===================================="
echo "  🎉 汉化完成！"
echo "  请重启 Waywallen 查看中文界面"
echo "===================================="

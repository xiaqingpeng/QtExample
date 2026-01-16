# 🌐 Docker Linux 环境多语言字体配置说明

## 问题描述

在 Linux 桌面环境中，只有英文显示正常，中文、日文、韩语无法正常显示（显示为方块或乱码）。

## 原因分析

Linux 系统默认可能没有安装中、日、韩（CJK）字体，导致 Qt 应用无法正确渲染这些字符。

## 解决方案

### 1. Dockerfile 已添加字体包

已更新 `Dockerfile`，添加了以下字体包：

- **fonts-noto-cjk** - Google Noto Sans CJK 字体（支持中文、日文、韩文）
- **fonts-noto-cjk-extra** - 额外的 Noto CJK 字体变体
- **fonts-wqy-microhei** - 文泉驿微米黑（中文）
- **fonts-wqy-zenhei** - 文泉驿正黑（中文）

### 2. 应用程序字体配置

已更新 `main.cpp`，配置了字体回退机制：

- **Linux 平台**：优先使用 `Noto Sans CJK SC`（简体中文）
- **字体回退列表**：包含多个 CJK 字体，确保字符能正确显示
- **跨平台支持**：macOS 和 Windows 也有相应的字体配置

## 使用步骤

### 1. 重新构建 Docker 镜像

```bash
# 清理旧的镜像（可选）
docker-compose down

# 重新构建镜像（包含字体包）
docker-compose build --no-cache qt-dev
```

### 2. 重新编译项目

```bash
./build-docker-linux.sh
```

### 3. 验证字体安装

在容器中检查字体是否已安装：

```bash
docker-compose run --rm qt-dev bash

# 检查字体是否安装
fc-list | grep -i "noto\|wqy"

# 应该能看到类似输出：
# /usr/share/fonts/truetype/noto/NotoSansCJK-Regular.ttc: Noto Sans CJK SC:style=Regular
# /usr/share/fonts/truetype/wqy/wqy-microhei.ttc: WenQuanYi Micro Hei:style=Regular
```

### 4. 运行程序测试

```bash
# 在容器中运行程序
docker-compose run --rm qt-dev bash
cd /workspace/build
./example
```

## 字体列表

安装后的字体包括：

### Noto Sans CJK 系列
- **Noto Sans CJK SC** - 简体中文
- **Noto Sans CJK TC** - 繁体中文
- **Noto Sans CJK JP** - 日文
- **Noto Sans CJK KR** - 韩文

### 文泉驿字体
- **WenQuanYi Micro Hei** - 文泉驿微米黑
- **WenQuanYi Zen Hei** - 文泉驿正黑

## 故障排除

### Q: 重新构建后仍然显示方块？

**检查步骤**：

1. **确认字体已安装**：
   ```bash
   docker-compose run --rm qt-dev bash
   fc-list | grep -i cjk
   ```

2. **检查翻译文件是否存在**：
   ```bash
   ls -lh /workspace/build/translations/
   # 应该看到：app_zh_CN.qm, app_ja_JP.qm, app_ko_KR.qm
   ```

3. **检查应用程序字体配置**：
   - 确认 `main.cpp` 中的字体配置已更新
   - 确认重新编译了应用程序

4. **清除字体缓存**（如果需要）：
   ```bash
   docker-compose run --rm qt-dev bash
   fc-cache -fv
   ```

### Q: 如何验证字体是否正确加载？

在应用程序中添加调试代码：

```cpp
QFontInfo fontInfo(QApplication::font());
qDebug() << "Font family:" << fontInfo.family();
qDebug() << "Font families:" << QApplication::font().families();
```

### Q: 可以添加其他字体吗？

可以！在 `Dockerfile` 中添加其他字体包：

```dockerfile
RUN apt-get install -y \
    fonts-noto-cjk \
    fonts-noto-cjk-extra \
    fonts-wqy-microhei \
    fonts-wqy-zenhei \
    fonts-arphic-uming \  # 文鼎字体
    fonts-arphic-ukai \   # 文鼎字体
    ...
```

## 字体配置原理

Qt 应用程序的字体渲染流程：

1. **应用程序设置字体**：`QApplication::setFont()` 设置默认字体
2. **字体回退机制**：当某个字符在当前字体中不存在时，Qt 会：
   - 检查字体回退列表（`setFamilies()`）
   - 使用系统字体配置（`fontconfig`）
   - 查找能显示该字符的字体

3. **Linux 字体系统**：
   - 使用 `fontconfig` 管理字体
   - 字体文件通常位于 `/usr/share/fonts/`
   - 使用 `fc-list` 查看可用字体

## 参考资源

- [Qt 字体文档](https://doc.qt.io/qt-6/qfont.html)
- [Noto 字体项目](https://www.google.com/get/noto/)
- [fontconfig 文档](https://www.freedesktop.org/wiki/Software/fontconfig/)

---

**配置完成后，中、日、韩文字符应该能正常显示了！** 🎉


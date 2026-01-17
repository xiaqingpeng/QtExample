# 跨平台 UI 视觉优化指南

## 问题分析

Qt 应用在不同平台上展现的视觉效果存在差异，主要原因包括：

### 1. **字体渲染差异**

#### macOS
- ✅ **优势**：使用 Core Text 渲染引擎，字体渲染质量最高
- ✅ 系统字体（Helvetica、PingFang SC）质量优秀
- ✅ 自动启用抗锯齿和子像素渲染
- ✅ 高 DPI 支持完善

#### Linux
- ⚠️ **问题**：使用 FreeType 渲染，质量取决于系统配置
- ⚠️ 字体可能缺失（Noto Sans CJK 需要手动安装）
- ⚠️ 字体渲染可能较粗糙，特别是小字号
- ⚠️ 不同发行版的字体配置差异大

#### Windows
- ⚠️ **问题**：使用 DirectWrite，但默认字体渲染较 macOS 粗糙
- ⚠️ 中文字体（Microsoft YaHei）渲染质量一般
- ⚠️ ClearType 可能影响某些字体的显示效果

### 2. **Qt 样式系统未使用原生样式**

当前代码中：
```cpp
// main.cpp 中只有：
QApplication::setAttribute(Qt::AA_DontShowIconsInMenus, true);
// 没有设置平台原生样式！
```

**问题**：
- 没有使用 `QApplication::setStyle()` 设置平台原生样式
- 所有控件都使用 Qt 的 Fusion 样式（默认），而不是：
  - macOS: `QStyleFactory::create("macintosh")`
  - Windows: `QStyleFactory::create("windowsvista")` 或 `"windows11"`
  - Linux: `QStyleFactory::create("gtk2")` 或 `"gtk3"`

### 3. **DPI/高分辨率支持不完善**

**问题**：
- 没有设置高 DPI 缩放属性
- 字体大小使用固定像素值（`setPixelSize(13)`），在不同 DPI 下显示不一致
- 没有使用 `Qt::AA_EnableHighDpiScaling` 或 `Qt::AA_UseHighDpiPixmaps`

### 4. **字体配置问题**

**当前配置**（main.cpp）：
```cpp
#ifdef Q_OS_MACOS
    defaultFont.setFamily("Helvetica");
    fontFamilies << "Helvetica" << "PingFang SC" << "Hiragino Sans GB" << "STHeiti";
#elif defined(Q_OS_LINUX)
    defaultFont.setFamily("Noto Sans CJK SC");  // 可能不存在！
    fontFamilies << "Noto Sans CJK SC" << "Noto Sans CJK JP" << ...
#elif defined(Q_OS_WIN)
    defaultFont.setFamily("Microsoft YaHei");
    fontFamilies << "Microsoft YaHei" << "SimSun" << ...
#endif
```

**问题**：
- Linux 上 `Noto Sans CJK SC` 可能未安装
- 字体回退机制不够完善
- 没有检测字体是否存在

### 5. **样式表（Stylesheet）渲染差异**

**问题**：
- CSS 样式表在不同平台的渲染效果不同
- 某些 CSS 属性（如 `letter-spacing`、`line-height`）在不同平台表现不一致
- 阴影效果（`box-shadow`）在 Linux/Windows 上可能不如 macOS 平滑

## 解决方案

### 方案 1：启用平台原生样式（推荐）

在 `main.cpp` 中添加：

```cpp
#include <QStyleFactory>

int main(int argc, char *argv[])
{
    Application app(argc, argv);
    
    // 启用高 DPI 支持
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling, true);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps, true);
    
    // 设置平台原生样式
    #ifdef Q_OS_MACOS
        // macOS 使用原生样式
        QApplication::setStyle(QStyleFactory::create("macintosh"));
    #elif defined(Q_OS_WIN)
        // Windows 使用 Windows 11 样式（如果可用）
        QStringList availableStyles = QStyleFactory::keys();
        if (availableStyles.contains("windows11")) {
            QApplication::setStyle(QStyleFactory::create("windows11"));
        } else if (availableStyles.contains("windowsvista")) {
            QApplication::setStyle(QStyleFactory::create("windowsvista"));
        }
    #elif defined(Q_OS_LINUX)
        // Linux 使用 GTK 样式（如果可用）
        QStringList availableStyles = QStyleFactory::keys();
        if (availableStyles.contains("gtk3")) {
            QApplication::setStyle(QStyleFactory::create("gtk3"));
        } else if (availableStyles.contains("gtk2")) {
            QApplication::setStyle(QStyleFactory::create("gtk2"));
        }
    #endif
    
    // ... 其他初始化代码
}
```

### 方案 2：改进字体配置

```cpp
#include <QFontDatabase>

// 检测并设置最佳字体
QString getBestFontFamily() {
    QFontDatabase fontDb;
    
    #ifdef Q_OS_MACOS
        if (fontDb.families().contains("PingFang SC")) {
            return "PingFang SC";
        }
        return "Helvetica";
    #elif defined(Q_OS_LINUX)
        // 按优先级检测字体
        QStringList preferredFonts = {
            "Noto Sans CJK SC",
            "WenQuanYi Micro Hei",
            "WenQuanYi Zen Hei",
            "Source Han Sans SC",
            "DejaVu Sans"
        };
        for (const QString &font : preferredFonts) {
            if (fontDb.families().contains(font)) {
                return font;
            }
        }
        return "sans-serif";
    #elif defined(Q_OS_WIN)
        if (fontDb.families().contains("Microsoft YaHei UI")) {
            return "Microsoft YaHei UI";  // 比 Microsoft YaHei 更现代
        }
        if (fontDb.families().contains("Microsoft YaHei")) {
            return "Microsoft YaHei";
        }
        return "Segoe UI";
    #endif
}

// 使用点大小而不是像素大小（DPI 自适应）
QFont defaultFont = QApplication::font();
defaultFont.setFamily(getBestFontFamily());
defaultFont.setPointSize(10);  // 使用 setPointSize 而不是 setPixelSize
QApplication::setFont(defaultFont);
```

### 方案 3：优化字体渲染设置

```cpp
// 在 main.cpp 中
QApplication::setAttribute(Qt::AA_UseStyleSheetPropagationInWidgetStyles, true);

// 针对不同平台调整字体渲染
#ifdef Q_OS_LINUX
    // Linux 上启用更好的字体渲染
    QFont::insertSubstitution("sans-serif", getBestFontFamily());
    QApplication::setFont(QFont(getBestFontFamily(), 10));
#endif

#ifdef Q_OS_WIN
    // Windows 上使用更清晰的字体渲染
    QFont::insertSubstitution("Microsoft YaHei", "Microsoft YaHei UI");
#endif
```

### 方案 4：统一样式表渲染

在 `theme_manager.cpp` 中，为不同平台提供优化的样式：

```cpp
// 在 getButtonStyle() 等方法中
QString ThemeManager::getButtonStyle(const QString& variant) const
{
    // 平台特定的字体大小调整
    int fontSize = Typography::FONT_SIZE_SM;
    #ifdef Q_OS_LINUX
        fontSize = 13;  // Linux 上稍微增大字体
    #elif defined(Q_OS_WIN)
        fontSize = 12;  // Windows 上稍微减小字体
    #endif
    
    QString baseStyle = QString(
        "QPushButton { "
        "    font-family: %1; "
        "    font-size: %2px; "
        "    font-weight: 500; "
        // ... 其他样式
    ).arg(Typography::FONT_FAMILY)
     .arg(fontSize);
    
    // ... 其他代码
}
```

### 方案 5：添加字体平滑处理

```cpp
// 在 main.cpp 中
QFont defaultFont = QApplication::font();
defaultFont.setFamily(getBestFontFamily());
defaultFont.setPointSize(10);

// 启用字体平滑
defaultFont.setStyleStrategy(QFont::PreferAntialias);
defaultFont.setHintingPreference(QFont::PreferFullHinting);

QApplication::setFont(defaultFont);
```

## 实施建议

### 优先级 1（立即实施）
1. ✅ 启用高 DPI 支持
2. ✅ 设置平台原生样式
3. ✅ 改进字体检测和回退机制

### 优先级 2（短期优化）
1. ✅ 使用点大小替代像素大小
2. ✅ 优化字体渲染设置
3. ✅ 平台特定的字体大小调整

### 优先级 3（长期优化）
1. ✅ 统一样式表渲染
2. ✅ 添加字体平滑处理
3. ✅ 考虑使用 Qt Quick Controls 2（更现代的 UI 框架）

## 测试建议

1. **在不同平台测试**：
   - macOS: 测试 Retina 和非 Retina 显示器
   - Linux: 测试不同发行版（Ubuntu、Fedora、Arch）
   - Windows: 测试不同 DPI 设置（100%、125%、150%、200%）

2. **字体测试**：
   - 检查中文字符显示
   - 检查日文、韩文字符显示
   - 检查数字和英文字符显示

3. **样式测试**：
   - 检查按钮、输入框、列表等控件的视觉效果
   - 检查阴影、圆角、边框等效果
   - 检查主题切换后的视觉效果

## 参考资源

- [Qt 高 DPI 支持文档](https://doc.qt.io/qt-6/highdpi.html)
- [Qt 样式系统文档](https://doc.qt.io/qt-6/qstyle.html)
- [Qt 字体渲染文档](https://doc.qt.io/qt-6/qfont.html)


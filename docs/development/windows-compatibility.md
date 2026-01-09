# Windows 平台兼容性说明

## 概述

由于 Qt6::WebEngineWidgets 模块在 Windows 平台上存在兼容性问题，本项目已实现了完整的跨平台兼容性方案。

## WebEngine 功能状态

### 支持的平台
- ✅ **macOS**: 完整支持 WebEngine 功能
- ✅ **Linux**: 完整支持 WebEngine 功能  
- ❌ **Windows**: WebEngine 功能已禁用，使用替代界面

### 受影响的功能模块

1. **Charts 模块** (`src/Charts/`)
   - ECharts 图表显示
   - 数据可视化功能

2. **Reports 模块** (`src/Reports/`)
   - 趋势图表
   - 活动图表

3. **UserProfile 模块** (`src/UserProfile/`)
   - 兴趣分析图表
   - 价值评估图表

## 技术实现

### 条件编译

项目使用 `WEBENGINE_AVAILABLE` 宏进行条件编译：

```cpp
#ifdef WEBENGINE_AVAILABLE
    // WebEngine 可用时的实现
    m_webView = new QWebEngineView();
#else
    // WebEngine 不可用时的替代实现
    QWidget* fallbackWidget = new QWidget();
    // ... 创建友好的替代界面
#endif
```

### 构建系统配置

#### CMake 配置
```cmake
# 检查WebEngine是否可用，特别处理Windows平台
set(WEBENGINE_AVAILABLE FALSE)
if(Qt6WebEngineWidgets_FOUND AND NOT WIN32)
    message(STATUS "Qt WebEngine found - enabling web features")
    set(WEBENGINE_AVAILABLE TRUE)
    add_definitions(-DWEBENGINE_AVAILABLE)
    set(WEBENGINE_LIBRARIES Qt6::WebEngineWidgets)
elseif(WIN32)
    message(STATUS "Windows platform detected - WebEngine features disabled for compatibility")
    set(WEBENGINE_LIBRARIES "")
endif()
```

#### qmake 配置
```qmake
# Windows平台禁用WebEngine以确保兼容性
win32 {
    message("Windows platform detected - WebEngine features disabled for compatibility")
} else {
    # 非Windows平台检查WebEngine可用性
    qtHaveModule(webenginewidgets) {
        QT += webenginewidgets
        DEFINES += WEBENGINE_AVAILABLE
        message("WebEngine available - enabling web features")
    }
}
```

## Windows 平台构建

### 使用 CMake 构建

1. 运行构建脚本：
   ```cmd
   scripts\build\build-windows.bat
   ```

2. 或手动构建：
   ```cmd
   mkdir build-windows
   cd build-windows
   cmake -G "Visual Studio 17 2022" -A x64 -DCMAKE_PREFIX_PATH="%QTDIR%" ..
   cmake --build . --config Release
   ```

### 使用 qmake 构建

1. 运行构建脚本：
   ```cmd
   scripts\build\build-windows-qmake.bat
   ```

2. 或手动构建：
   ```cmd
   mkdir build-windows-qmake
   cd build-windows-qmake
   qmake CONFIG+=release ..\example.pro
   nmake
   ```

## 用户体验

### Windows 平台替代界面

当 WebEngine 不可用时，项目会显示友好的替代界面：

- **标题**: 清晰标识功能名称
- **说明**: 解释功能不可用的原因
- **建议**: 推荐使用其他平台获得完整体验

### 示例界面内容

```
图表功能

当前平台不支持 WebEngine 组件
图表功能已禁用

建议使用 macOS 或 Linux 平台获得完整的图表体验
```

## 开发指南

### 添加新的 WebEngine 功能

1. **使用条件编译**：
   ```cpp
   #ifdef WEBENGINE_AVAILABLE
       // WebEngine 实现
   #else
       // 替代实现
   #endif
   ```

2. **更新构建配置**：
   - 在相关的 CMakeLists.txt 中添加 WebEngine 检查
   - 在相关的 .pro 文件中添加平台检查

3. **提供替代界面**：
   - 创建友好的用户提示
   - 保持界面布局的一致性

### 测试不同平台

- **macOS/Linux**: 验证 WebEngine 功能正常
- **Windows**: 验证替代界面显示正确
- **构建测试**: 确保所有平台都能成功构建

## 常见问题

### Q: 为什么 Windows 不支持 WebEngine？
A: Qt6::WebEngineWidgets 在 Windows 平台上存在依赖和兼容性问题，可能导致应用程序无法正常运行。

### Q: 能否在 Windows 上强制启用 WebEngine？
A: 不建议这样做。如果确实需要，可以修改构建配置，但可能会遇到运行时问题。

### Q: 替代界面能否显示静态图表？
A: 可以考虑使用 Qt Charts 或其他图表库作为替代方案，但需要额外的开发工作。

## 未来改进

1. **Qt Charts 集成**: 考虑使用 Qt Charts 作为 Windows 平台的图表解决方案
2. **数据导出**: 提供图表数据的 CSV/Excel 导出功能
3. **外部查看器**: 支持在外部浏览器中查看图表

## 相关文件

- `CMakeLists.txt` - 主构建配置
- `src/*/CMakeLists.txt` - 模块构建配置
- `*.pro` - qmake 构建配置
- `scripts/build/build-windows*.bat` - Windows 构建脚本
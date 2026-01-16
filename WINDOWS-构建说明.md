# 🪟 Windows 平台构建说明

本项目已配置为跨平台兼容，支持 **Linux**、**macOS** 和 **Windows** 平台。

## 📋 Windows 平台特性

### WebEngine 支持

**Windows 平台上 WebEngine 功能被禁用**，原因：
- Qt WebEngine 在 Windows 上可能存在兼容性问题
- 项目已实现降级方案，WebEngine 功能会自动使用替代界面

**影响**：
- ECharts 图表功能会显示替代界面
- ReportsTab 中的图表会显示占位符
- UserProfileTab 中的图表会显示占位符
- Device 模块中的 WebView 会显示占位符

**核心功能不受影响**：
- ✅ 登录/注册功能正常
- ✅ 用户信息管理正常
- ✅ 数据展示正常
- ✅ 多语言支持正常

### 字体支持

Windows 平台已配置中、日、韩字体支持：

- **中文**：Microsoft YaHei（微软雅黑）、SimSun（宋体）
- **日文**：Meiryo（メイリオ）
- **韩文**：Malgun Gothic（맑은 고딕）

这些字体是 Windows 系统自带的，无需额外安装。

## 🔨 Windows 构建步骤

### 前置要求

1. **安装 Qt6**
   - 下载并安装 [Qt 6.x](https://www.qt.io/download)
   - 确保安装以下组件：
     - Qt 6.x (MSVC 2019 64-bit) 或 MinGW 64-bit
     - Qt Creator（可选，但推荐）
     - CMake（如果未安装）

2. **安装 Visual Studio**（如果使用 MSVC）
   - Visual Studio 2019 或更高版本
   - 包含 "Desktop development with C++" 工作负载

3. **安装 CMake**
   - 下载 [CMake](https://cmake.org/download/)
   - 或通过 Qt 安装程序安装

### 构建方法

#### 方法 1：使用 Qt Creator（推荐）

1. 打开 Qt Creator
2. 选择 `File` → `Open File or Project`
3. 选择项目根目录的 `CMakeLists.txt`
4. 配置构建：
   - **Kit**：选择已安装的 Qt 6.x 套件
   - **Build Directory**：选择或创建 `build-windows` 目录
5. 点击 `Configure Project`
6. 点击 `Build` → `Build Project`

#### 方法 2：使用命令行（CMake）

```powershell
# 创建构建目录
mkdir build-windows
cd build-windows

# 配置项目（使用 MSVC）
cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_PREFIX_PATH="C:/Qt/6.x.x/msvc2019_64"

# 或使用 MinGW
cmake .. -G "MinGW Makefiles" -DCMAKE_PREFIX_PATH="C:/Qt/6.x.x/mingw_64"

# 编译
cmake --build . --config Release

# 或使用 Visual Studio
cmake --build . --config Release -j 8
```

#### 方法 3：使用 Visual Studio

1. 打开 Visual Studio
2. `File` → `Open` → `CMake...`
3. 选择项目根目录的 `CMakeLists.txt`
4. Visual Studio 会自动配置并开始构建

### 运行程序

编译完成后，可执行文件位于：
- **MSVC**：`build-windows/Release/example.exe`
- **MinGW**：`build-windows/example.exe`

直接双击运行即可。

## 🔧 Windows 特定配置

### CMake 配置

项目已自动检测 Windows 平台并应用相应配置：

```cmake
if(WIN32)
    # Windows 特定配置
    # WebEngine 自动禁用
    # 字体配置已包含 Windows 字体
endif()
```

### 字体配置

`main.cpp` 中已配置 Windows 字体：

```cpp
#elif defined(Q_OS_WIN)
    defaultFont.setFamily("Microsoft YaHei");
    fontFamilies << "Microsoft YaHei" << "SimSun" << "Malgun Gothic" << "Meiryo";
#endif
```

### 翻译文件

翻译文件会自动复制到可执行文件目录的 `translations/` 子目录：

```
build-windows/
  ├── example.exe
  └── translations/
      ├── app_zh_CN.qm
      ├── app_en_US.qm
      ├── app_ja_JP.qm
      └── app_ko_KR.qm
```

## 📝 与 Linux 平台的兼容性

### 共享的配置

以下配置在 Linux 和 Windows 上**完全相同**：

1. **WebEngine 处理**：
   - Linux（ARM64）：WebEngine 不可用 → 使用降级方案
   - Windows：WebEngine 被禁用 → 使用降级方案
   - 代码逻辑完全一致

2. **字体回退机制**：
   - 两个平台都使用字体回退列表
   - 确保 CJK 字符能正确显示

3. **多语言支持**：
   - 翻译文件格式相同（.qm）
   - 加载逻辑相同
   - 语言切换机制相同

4. **条件编译**：
   - 使用 `#ifdef WEBENGINE_AVAILABLE` 统一处理
   - 使用 `#ifdef Q_OS_LINUX` / `#ifdef Q_OS_WIN` 处理平台差异

### 平台差异

| 特性 | Linux | Windows |
|------|-------|---------|
| WebEngine | 不可用（ARM64） | 禁用（兼容性） |
| 字体 | Noto Sans CJK | Microsoft YaHei |
| 构建系统 | CMake + Make | CMake + MSVC/Mingw |
| 可执行文件 | `example` | `example.exe` |

## ❓ 常见问题

### Q: Windows 上编译失败，提示找不到 Qt6？

**解决**：
1. 确认 Qt6 已正确安装
2. 设置 `CMAKE_PREFIX_PATH` 环境变量：
   ```powershell
   $env:CMAKE_PREFIX_PATH="C:/Qt/6.x.x/msvc2019_64"
   ```
3. 或在 CMake 配置时指定：
   ```powershell
   cmake .. -DCMAKE_PREFIX_PATH="C:/Qt/6.x.x/msvc2019_64"
   ```

### Q: Windows 上中文显示为方块？

**解决**：
1. 确认翻译文件存在：`build-windows/translations/app_zh_CN.qm`
2. 确认字体配置已应用（代码中已配置）
3. 检查 Windows 系统字体是否正常

### Q: 如何在 Windows 上启用 WebEngine？

**说明**：不推荐，因为可能存在兼容性问题。如果确实需要：

1. 修改 `CMakeLists.txt`：
   ```cmake
   # 移除 WIN32 检查
   if(Qt6WebEngineWidgets_FOUND)
       set(WEBENGINE_AVAILABLE TRUE)
   endif()
   ```

2. 确保安装了 Qt WebEngine 组件

3. 重新编译

### Q: Windows 上运行程序缺少 DLL？

**解决**：
1. 使用 Qt 的 `windeployqt` 工具：
   ```powershell
   windeployqt.exe build-windows/Release/example.exe
   ```

2. 或手动复制所需的 DLL 到可执行文件目录

## 🎯 跨平台兼容性总结

项目已实现**完全跨平台兼容**：

- ✅ **Linux**：Docker + X11 环境，支持 ARM64
- ✅ **macOS**：原生支持，完整功能
- ✅ **Windows**：MSVC/MinGW 支持，WebEngine 禁用但核心功能完整

所有平台共享相同的：
- 代码逻辑
- 多语言支持
- 字体回退机制
- WebEngine 降级方案

---

**Windows 平台构建和运行说明完成！** 🎉


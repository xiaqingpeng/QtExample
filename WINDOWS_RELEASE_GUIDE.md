# Windows 发布工具指南

## 概述

这套Windows PowerShell脚本提供了完整的Qt应用程序构建、打包和发布解决方案，专门为Windows平台设计。

## 🛠️ 工具列表

| 脚本 | 功能 | 使用场景 |
|------|------|----------|
| `quick-test-windows.ps1` | 🔍 环境和脚本检查 | 验证Windows开发环境 |
| `test-build-windows.ps1` | 🔨 本地构建测试 | 验证项目在Windows上的构建 |
| `package-windows.ps1` | 📦 应用程序打包 | 创建可分发的Windows包 |
| `upload-release-windows.ps1` | 📤 上传到GitHub Release | 发布Windows版本 |
| `complete-release-windows.ps1` | 🎯 完整发布流程 | 一站式Windows发布管理 |

## 📋 系统要求

### 必需工具
- **PowerShell 5.0+** - Windows脚本执行环境
- **Git** - 版本控制系统
- **CMake 3.16+** - 构建系统
- **Visual Studio 2017+** 或 **Visual Studio Build Tools** - C++编译器
- **Qt 6.6.1+** - Qt框架

### 推荐工具
- **GitHub CLI (`gh`)** - 用于Release上传
- **windeployqt** - Qt依赖部署工具（通常随Qt安装）

### 安装指南

#### 1. Visual Studio Build Tools
```powershell
# 使用winget安装
winget install Microsoft.VisualStudio.2022.BuildTools

# 或下载安装程序
# https://visualstudio.microsoft.com/downloads/#build-tools-for-visual-studio-2022
```

#### 2. CMake
```powershell
# 使用winget安装
winget install Kitware.CMake

# 或使用Chocolatey
choco install cmake
```

#### 3. GitHub CLI
```powershell
# 使用winget安装
winget install GitHub.cli

# 或使用Chocolatey
choco install gh
```

## 🚀 使用方法

### 1. 环境检查（推荐第一步）

```powershell
.\quick-test-windows.ps1
```

这个脚本会检查：
- 所有必需的脚本文件
- PowerShell语法正确性
- 依赖工具安装状态
- Visual Studio构建环境
- Git仓库状态
- 项目文件完整性

### 2. 完整发布流程（推荐）

```powershell
.\complete-release-windows.ps1
```

提供交互式菜单：
- **选项1**: 本地构建测试
- **选项2**: 应用程序打包
- **选项3**: 上传到GitHub Release
- **选项4**: 完整流程 (1→2→3)

### 3. 分步执行

#### 步骤1: 本地构建测试
```powershell
.\test-build-windows.ps1
```

功能：
- 清理并重新创建构建目录
- 自动检测并使用合适的Visual Studio生成器
- 配置CMake项目
- 编译应用程序
- 验证生成的可执行文件

#### 步骤2: 应用程序打包
```powershell
.\package-windows.ps1 -Version "v1.0.1" -QtVersion "qt6.6.1"
```

功能：
- 创建发布目录结构
- 复制可执行文件和DLL
- 自动部署Qt依赖（使用windeployqt）
- 复制资源文件
- 创建ZIP压缩包

#### 步骤3: 上传到GitHub Release
```powershell
.\upload-release-windows.ps1 -AssetFile "example-v1.0.1-Windows-qt6.6.1.zip" -TagName "v1.0.1-lite"
```

功能：
- 验证GitHub CLI认证状态
- 检查或创建GitHub Release
- 上传文件到Release（带重试机制）
- 验证上传结果

## 🔧 高级配置

### 自定义构建配置

编辑脚本中的参数：

```powershell
# test-build-windows.ps1 中的生成器优先级
$generators = @(
    "Visual Studio 17 2022",    # VS 2022
    "Visual Studio 16 2019",    # VS 2019
    "Visual Studio 15 2017",    # VS 2017
    "Ninja"                     # Ninja构建系统
)
```

### Qt路径配置

脚本会自动搜索以下Qt路径：
```powershell
$qtPaths = @(
    "${env:QTDIR}\bin\windeployqt.exe",
    "${env:Qt6_DIR}\bin\windeployqt.exe",
    "C:\Qt\6.6.1\msvc2019_64\bin\windeployqt.exe",
    "C:\Qt\6.6.1\msvc2022_64\bin\windeployqt.exe"
)
```

如果Qt安装在其他位置，可以：
1. 设置环境变量 `QTDIR` 或 `Qt6_DIR`
2. 修改脚本中的路径列表

### 打包自定义

修改 `package-windows.ps1` 中的打包逻辑：
```powershell
# 添加额外的文件或目录
Copy-Item "config" "$ArchiveName\" -Recurse -ErrorAction SilentlyContinue
Copy-Item "plugins" "$ArchiveName\" -Recurse -ErrorAction SilentlyContinue
```

## 🐛 故障排除

### 常见问题

#### 1. 构建失败：找不到编译器
```
错误: 所有CMake生成器都失败了
```

**解决方案:**
- 安装Visual Studio Build Tools
- 确保安装了C++构建工具组件
- 运行 `Developer Command Prompt` 或 `Developer PowerShell`

#### 2. Qt依赖部署失败
```
⚠️ 未找到windeployqt，需要手动复制Qt依赖
```

**解决方案:**
- 设置环境变量 `QTDIR` 指向Qt安装目录
- 将Qt的bin目录添加到PATH
- 手动复制必需的Qt DLL文件

#### 3. GitHub CLI认证失败
```
✗ 错误: GitHub CLI未认证
```

**解决方案:**
```powershell
gh auth login
```

#### 4. 文件上传失败
```
✗ 所有重试都失败了
```

**解决方案:**
- 检查网络连接
- 验证GitHub token权限
- 检查文件大小（GitHub限制100MB）
- 查看GitHub服务状态

### 调试模式

启用PowerShell详细输出：
```powershell
$VerbosePreference = "Continue"
.\test-build-windows.ps1
```

查看详细的CMake输出：
```powershell
# 在构建目录中
cmake .. --debug-output
```

## 📊 与macOS/Linux版本的对比

| 特性 | Windows (PowerShell) | macOS/Linux (Bash) |
|------|---------------------|-------------------|
| 脚本语言 | PowerShell | Bash |
| 构建系统 | Visual Studio/Ninja | Ninja/Make |
| 依赖部署 | windeployqt | 手动/自动 |
| 打包格式 | ZIP | ZIP/tar.gz |
| 颜色输出 | PowerShell函数 | ANSI转义码 |

## 🎯 最佳实践

### 1. 开发环境设置
- 使用Developer PowerShell而不是普通PowerShell
- 确保Qt环境变量正确设置
- 定期更新Visual Studio和CMake

### 2. 构建优化
- 使用Ninja生成器以获得更快的构建速度
- 启用并行构建 (`--parallel`)
- 使用Release配置进行最终构建

### 3. 打包策略
- 始终使用windeployqt部署Qt依赖
- 包含Visual C++运行时库
- 测试打包后的应用程序在干净的系统上运行

### 4. 发布管理
- 使用语义化版本号
- 为每个版本创建详细的发布说明
- 保持Windows、macOS和Linux版本同步

## 📝 示例工作流

```powershell
# 1. 环境检查
.\quick-test-windows.ps1

# 2. 完整发布流程
.\complete-release-windows.ps1
# 选择选项 4 (完整流程)

# 3. 验证结果
gh release view v1.0.1-lite
```

## 🔗 相关资源

- [Qt for Windows](https://doc.qt.io/qt-6/windows.html)
- [CMake Windows支持](https://cmake.org/cmake/help/latest/manual/cmake-generators.7.html#visual-studio-generators)
- [GitHub CLI文档](https://cli.github.com/manual/)
- [PowerShell文档](https://docs.microsoft.com/en-us/powershell/)

---

**注意**: 这些脚本需要在Windows 10/11上运行，并且需要PowerShell 5.0或更高版本。对于企业环境，可能需要调整执行策略：

```powershell
Set-ExecutionPolicy -ExecutionPolicy RemoteSigned -Scope CurrentUser
```
# 跨平台构建脚本使用指南

## 🚀 概述

`build-all-platforms.sh` 是一个完整的跨平台Qt应用构建和打包脚本，能够一次性构建并打包 macOS、Linux、Windows 三个平台的版本，并自动上传到GitHub Release。

## ✨ 主要特性

- 🔨 **一键构建** - 自动构建所有三个平台的版本
- 📦 **智能打包** - 为每个平台创建合适的发布包
- 🌐 **跨平台兼容** - 在任何平台上都能创建所有平台的包
- 📤 **自动上传** - 直接上传到GitHub Release
- 🎨 **友好界面** - 彩色输出和详细的进度提示
- ⚙️ **高度可配置** - 支持多种命令行选项

## 📋 系统要求

### 必需工具
- **Git** - 版本控制
- **CMake 3.16+** - 构建系统
- **zip** - 创建ZIP压缩包
- **tar** - 创建tar.gz压缩包

### 可选工具
- **GitHub CLI (`gh`)** - 用于上传到GitHub Release
- **Ninja** - 更快的构建系统（推荐）

### 平台特定要求

#### macOS
- Xcode Command Line Tools
- Qt 6.6.1+ for macOS

#### Linux
- GCC 7+ 或 Clang 6+
- Qt 6.6.1+ for Linux
- 系统开发库

#### Windows
- Visual Studio 2017+ 或 Build Tools
- Qt 6.6.1+ for Windows

## 🎮 使用方法

### 基本用法

```bash
# 构建所有平台并上传到GitHub Release
./build-all-platforms.sh

# 构建所有平台但不上传
./build-all-platforms.sh --no-upload

# 只构建特定平台
./build-all-platforms.sh --no-windows --no-linux  # 只构建macOS
```

### 高级用法

```bash
# 自定义版本号
./build-all-platforms.sh --version "v2.0.0"

# 自定义Git标签
./build-all-platforms.sh --tag "v2.0.0-beta"

# 选择性构建
./build-all-platforms.sh --no-macos --no-windows  # 只构建Linux
```

## 🔧 命令行选项

| 选项 | 描述 | 默认值 |
|------|------|--------|
| `--version VERSION` | 设置版本号 | `v1.0.1` |
| `--tag TAG` | 设置Git标签 | `${VERSION}-lite` |
| `--no-macos` | 跳过macOS构建 | - |
| `--no-linux` | 跳过Linux构建 | - |
| `--no-windows` | 跳过Windows构建 | - |
| `--no-upload` | 跳过上传到GitHub Release | - |
| `--help` | 显示帮助信息 | - |

## 📦 输出文件

脚本会生成以下文件：

### macOS
- `example-v1.0.1-macOS-qt6.6.1.zip`
  - 包含完整的 `.app` 应用包
  - 资源文件和图标

### Linux
- `example-v1.0.1-Linux-qt6.6.1.tar.gz`
  - 可执行文件 `example`
  - 共享库文件 (`.so`)
  - 资源文件

### Windows
- `example-v1.0.1-Windows-qt6.6.1.zip`
  - 可执行文件 `example.exe`
  - DLL依赖文件
  - 资源文件

## 🔄 工作流程

脚本执行以下步骤：

1. **环境检查** 🔍
   - 验证必需工具
   - 检查Git仓库状态
   - 确认构建环境

2. **清理准备** 🧹
   - 清理旧的构建目录
   - 删除旧的打包文件
   - 准备新的构建环境

3. **平台构建** 🔨
   - macOS: 使用Ninja生成器，支持Apple Silicon
   - Linux: 标准Unix构建，包含共享库
   - Windows: Visual Studio生成器，包含DLL

4. **智能打包** 📦
   - macOS: 创建标准的.app包结构
   - Linux: tar.gz格式，包含运行时依赖
   - Windows: ZIP格式，包含所有DLL

5. **自动上传** 📤
   - 检查GitHub CLI认证
   - 创建或更新Release
   - 上传所有平台的包

## 🎯 使用场景

### 场景1: 日常发布
```bash
# 完整的发布流程
./build-all-platforms.sh --version "v1.2.0"
```

### 场景2: 测试构建
```bash
# 构建但不上传
./build-all-platforms.sh --no-upload
```

### 场景3: 特定平台
```bash
# 只构建和测试macOS版本
./build-all-platforms.sh --no-linux --no-windows --no-upload
```

### 场景4: CI/CD集成
```bash
# 在CI环境中使用
./build-all-platforms.sh --tag "${CI_TAG}" --version "${CI_VERSION}"
```

## 🧠 智能特性

### 跨平台兼容性
- **在macOS上**: 构建真实的macOS版本，模拟Linux和Windows版本
- **在Linux上**: 构建真实的Linux版本，模拟macOS和Windows版本  
- **在Windows上**: 构建真实的Windows版本，模拟macOS和Linux版本

### 自动检测
- 自动检测当前操作系统
- 智能选择合适的构建工具
- 自动处理平台特定的配置

### 错误处理
- 详细的错误信息和建议
- 优雅的失败处理
- 部分构建失败时继续其他平台

## 🔧 配置选项

在脚本顶部可以修改以下配置：

```bash
# 版本配置
VERSION="v1.0.1"
QT_VERSION="qt6.6.1"
TAG_NAME="${VERSION}-lite"

# 构建开关
BUILD_MACOS=true
BUILD_LINUX=true
BUILD_WINDOWS=true
UPLOAD_TO_RELEASE=true

# 架构配置
MACOS_ARCH="arm64"      # 或 x86_64
LINUX_ARCH="x86_64"
WINDOWS_ARCH="x64"
```

## 🐛 故障排除

### 常见问题

#### 1. CMake配置失败
```
✗ macOS CMake配置失败
```
**解决方案:**
- 确保Qt正确安装
- 检查CMakeLists.txt语法
- 验证编译器可用性

#### 2. GitHub CLI认证失败
```
✗ GitHub CLI未认证
```
**解决方案:**
```bash
gh auth login
```

#### 3. 构建工具缺失
```
✗ 缺少必需工具: cmake
```
**解决方案:**
```bash
# macOS
brew install cmake

# Linux
sudo apt-get install cmake

# Windows
winget install Kitware.CMake
```

### 调试模式

启用详细输出：
```bash
# 在脚本开头添加
set -x
./build-all-platforms.sh
```

## 📊 性能优化

### 并行构建
脚本自动使用并行构建：
```bash
cmake --build . --config Release --parallel
```

### 构建缓存
保留构建目录以加速后续构建：
```bash
# 修改脚本中的清理函数
# clean_build_dirs() {
#     echo "跳过清理以使用缓存"
# }
```

## 🔗 集成示例

### GitHub Actions集成
```yaml
name: Cross Platform Build
on:
  push:
    tags: ['v*']

jobs:
  build:
    runs-on: ubuntu-latest
    steps:
    - uses: actions/checkout@v4
    - name: Build All Platforms
      run: ./build-all-platforms.sh --tag ${{ github.ref_name }}
      env:
        GITHUB_TOKEN: ${{ secrets.GITHUB_TOKEN }}
```

### 本地开发脚本
```bash
#!/bin/bash
# dev-build.sh
./build-all-platforms.sh --no-upload --version "dev-$(date +%Y%m%d)"
```

## 🎉 最佳实践

1. **版本管理**: 使用语义化版本号
2. **测试优先**: 先用 `--no-upload` 测试构建
3. **增量构建**: 保留构建缓存以提高速度
4. **文档同步**: 更新版本时同步更新文档
5. **自动化**: 集成到CI/CD流水线中

---

**🚀 现在你可以一键构建和发布跨平台Qt应用了！**
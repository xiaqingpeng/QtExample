# QtExample

一个跨平台的Qt应用程序，支持macOS、Linux、Windows三个平台的自动化构建和发布。

## 🚀 快速开始

### 查看所有构建选项
```bash
./show-build-options.sh
```

### 基本构建命令

```bash
# 🚀 构建所有平台并自动上传到GitHub Release
./build-all-platforms.sh

# 🧪 构建所有平台但不上传（测试模式）
./build-all-platforms.sh --no-upload

# 📦 自定义版本号
./build-all-platforms.sh --version "v2.0.0"

# 🎯 只构建特定平台
./build-all-platforms.sh --no-windows --no-linux  # 只构建macOS
./build-all-platforms.sh --no-macos --no-linux    # 只构建Windows
./build-all-platforms.sh --no-macos --no-windows  # 只构建Linux

# 🎛️ 使用交互式菜单
./complete-release.sh
# 然后选择选项 7 (跨平台构建) 或选项 8 (快速构建)
```

### 高级用法

```bash
# 🔧 组合选项
./build-all-platforms.sh --version "v1.5.0" --tag "v1.5.0-rc1" --no-upload

# ⚡ 快速构建上传（基于现有构建）
./quick-build-upload.sh

# 🔄 强制重新构建
./quick-build-upload.sh --force-rebuild

# 🏷️ 自定义标签
./build-all-platforms.sh --tag "v2.0.0-beta"
```

## 📋 使用场景

| 场景 | 推荐命令 | 说明 |
|------|----------|------|
| 日常开发测试 | `./test-build.sh` | 快速验证构建 |
| 跨平台测试 | `./build-all-platforms.sh --no-upload` | 测试所有平台构建 |
| 正式发布 | `./build-all-platforms.sh --version "v2.0.0"` | 完整构建并发布 |
| 紧急修复 | `./quick-build-upload.sh --version "v1.0.1-hotfix"` | 快速修复发布 |
| 预发布测试 | `./build-all-platforms.sh --tag "v2.0.0-rc1" --no-upload` | 创建候选版本 |

## 🛠️ 构建工具

- **`build-all-platforms.sh`** - 跨平台完整构建
- **`quick-build-upload.sh`** - 快速构建上传
- **`test-build.sh`** - 本地构建测试
- **`complete-release.sh`** - 交互式发布菜单
- **`show-build-options.sh`** - 显示所有构建选项

## 📦 输出文件

构建完成后会生成以下文件：

- **macOS**: `example-v1.0.1-macOS-qt6.6.1.zip`
- **Linux**: `example-v1.0.1-Linux-qt6.6.1.tar.gz`
- **Windows**: `example-v1.0.1-Windows-qt6.6.1.zip`

## 🔧 系统要求

### macOS
- macOS 10.15+
- Xcode Command Line Tools
- Qt 6.6.1+
- CMake 3.16+

### Linux
- Ubuntu 20.04+ / CentOS 8+
- GCC 7+ 或 Clang 6+
- Qt 6.6.1+
- CMake 3.16+

### Windows
- Windows 10/11
- Visual Studio 2019/2022
- Qt 6.6.1+ (包含WebEngine模块)
- CMake 3.16+

## 📚 详细文档

- [跨平台构建指南](BUILD_ALL_PLATFORMS_GUIDE.md)
- [Windows发布指南](WINDOWS_RELEASE_GUIDE.md)
- [Windows WebEngine解决方案](WINDOWS_WEBENGINE_SOLUTION.md)
- [流水线检测指南](PIPELINE_DETECTION_GUIDE.md)
- [完整解决方案总结](FINAL_SOLUTION_SUMMARY.md)


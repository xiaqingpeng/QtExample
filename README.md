# QtExample

一个跨平台的Qt应用程序，支持macOS、Linux、Windows三个平台的自动化构建和发布。

## 🌟 平台兼容性

| 平台 | 支持状态 | WebEngine功能 | 说明 |
|------|----------|---------------|------|
| 🍎 **macOS** | ✅ 完全支持 | ✅ 完整功能 | 推荐开发平台 |
| 🐧 **Linux** | ✅ 完全支持 | ✅ 完整功能 | 服务器部署首选 |
| 🪟 **Windows** | ✅ 兼容支持 | ❌ 功能禁用 | 使用替代界面 |

> **Windows用户注意**: 由于Qt6::WebEngineWidgets在Windows平台的兼容性问题，图表功能将显示友好的替代界面。详见 [Windows兼容性说明](docs/development/windows-compatibility.md)。

## 📁 项目结构

```
├── docs/                    # 📚 文档目录
│   ├── DOCKER.md           # Docker 开发环境完整指南
│   ├── build/              # 构建相关文档
│   ├── release/            # 发布相关文档
│   ├── development/        # 开发相关文档
│   ├── api/                # API相关文档
│   └── troubleshooting/    # 故障排除文档
├── scripts/                # 🔧 脚本目录
│   ├── build/              # 构建脚本
│   ├── release/            # 发布脚本
│   ├── test/               # 测试脚本
│   ├── setup/              # 设置脚本
│   └── windows/            # Windows专用脚本
├── src/                    # 源代码目录
└── README.md              # 项目说明
```

## 🚀 快速开始

### Docker 开发环境（推荐）

**快速开始**：
```bash
# 启动容器
docker-compose up -d

# 运行程序（自动检测最佳模式）
./quick-run.sh
```

**详细文档**：请查看 [Docker 开发环境指南](docs/DOCKER.md)

---

## 📦 本地构建

### 查看所有构建选项
```bash
./scripts/test/show-build-options.sh
```

### 基本构建命令

```bash
# 🚀 构建所有平台并自动上传到GitHub Release
./scripts/build/build-all-platforms.sh

# 🧪 构建所有平台但不上传（测试模式）
./scripts/build/build-all-platforms.sh --no-upload

# 📦 自定义版本号
./scripts/build/build-all-platforms.sh --version "v2.0.0"

# 🎯 只构建特定平台
./scripts/build/build-all-platforms.sh --no-windows --no-linux  # 只构建macOS
./scripts/build/build-all-platforms.sh --no-macos --no-linux    # 只构建Windows
./scripts/build/build-all-platforms.sh --no-macos --no-windows  # 只构建Linux

# 🎛️ 使用交互式菜单
./scripts/release/complete-release.sh
# 然后选择选项 7 (跨平台构建) 或选项 8 (快速构建)
```

### 高级用法

```bash
# 🔧 组合选项
./scripts/build/build-all-platforms.sh --version "v1.5.0" --tag "v1.5.0-rc1" --no-upload

# ⚡ 快速构建上传（基于现有构建）
./scripts/release/quick-build-upload.sh

# 🔄 强制重新构建
./scripts/release/quick-build-upload.sh --force-rebuild

# 🏷️ 自定义标签
./scripts/build/build-all-platforms.sh --tag "v2.0.0-beta"

# 🚀 本地构建成功后推送到GitHub和GitLab
./scripts/build/build-and-push.sh

# 📤 跳过构建，直接推送代码
./scripts/build/build-and-push.sh --skip-build

# 🎯 只推送到特定平台
./scripts/build/build-and-push.sh --github-only
./scripts/build/build-and-push.sh --gitlab-only

# 💪 强制推送（谨慎使用）
./scripts/build/build-and-push.sh --force-push
```

### 🔧 初始设置
```bash
# 🛠️ 配置GitHub和GitLab远程仓库
./scripts/setup/setup-remotes.sh
```

### 🪟 Windows 平台构建

Windows用户可以使用专门的构建脚本：

```cmd
REM 🔧 CMake构建（推荐）
scripts\build\build-windows.bat

REM 🔨 qmake构建
scripts\build\build-windows-qmake.bat

REM 🧪 兼容性测试
scripts\test\test-windows-compatibility.bat
```

**Windows构建要求**:
- Qt 6.x (已安装并配置环境变量)
- Visual Studio 2019/2022 或 MinGW
- CMake 3.16+ (CMake构建) 或 qmake (qmake构建)

**注意事项**:
- WebEngine功能在Windows平台自动禁用
- 图表模块将显示友好的替代界面
- 详细说明请参考 [Windows兼容性文档](docs/development/windows-compatibility.md)

## 📋 使用场景

| 场景 | 推荐命令 | 说明 |
|------|----------|------|
| 日常开发测试 | `./scripts/build/test-build.sh` | 快速验证构建 |
| 构建并推送代码 | `./scripts/build/build-and-push.sh` | 本地构建成功后推送到远程仓库 |
| 跨平台测试 | `./scripts/build/build-all-platforms.sh --no-upload` | 测试所有平台构建 |
| 正式发布 | `./scripts/build/build-all-platforms.sh --version "v2.0.0"` | 完整构建并发布 |
| 紧急修复 | `./scripts/release/quick-build-upload.sh --version "v1.0.1-hotfix"` | 快速修复发布 |
| 预发布测试 | `./scripts/build/build-all-platforms.sh --tag "v2.0.0-rc1" --no-upload` | 创建候选版本 |

## 🛠️ 构建工具

- **`scripts/build/build-all-platforms.sh`** - 跨平台完整构建
- **`scripts/release/quick-build-upload.sh`** - 快速构建上传
- **`scripts/build/test-build.sh`** - 本地构建测试
- **`scripts/build/build-and-push.sh`** - 构建并推送到远程仓库 ⭐
- **`scripts/release/complete-release.sh`** - 交互式发布菜单
- **`scripts/test/show-build-options.sh`** - 显示所有构建选项
- **`scripts/setup/setup-remotes.sh`** - 配置远程仓库助手 🔧

## 🪟 Windows专用工具

- **`scripts/windows/complete-release-windows.ps1`** - Windows完整发布流程
- **`scripts/windows/fix-all-windows-dll.ps1`** - 一键修复所有DLL问题 ⭐
- **`scripts/windows/diagnose-windows-dll.ps1`** - DLL问题诊断
- **`scripts/windows/fix-windows-webengine.ps1`** - 修复WebEngine依赖

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

### 构建文档
- [跨平台构建指南](docs/build/BUILD_ALL_PLATFORMS_GUIDE.md)
- [构建并推送代码指南](docs/build/BUILD_AND_PUSH_GUIDE.md) ⭐

### 发布文档
- [跨平台发布指南](docs/release/CROSS_PLATFORM_RELEASE_README.md)
- [Windows发布指南](docs/release/WINDOWS_RELEASE_GUIDE.md)
- [发布自动化总结](docs/release/RELEASE_AUTOMATION_SUMMARY.md)

### 故障排除文档
- [GitHub Actions故障排除](docs/troubleshooting/GITHUB_ACTIONS_TROUBLESHOOTING.md)
- [流水线检测指南](docs/troubleshooting/PIPELINE_DETECTION_GUIDE.md)
- [Windows WebEngine解决方案](docs/troubleshooting/WINDOWS_WEBENGINE_SOLUTION.md)

### 开发文档
- [主题系统修复](docs/development/THEME_FIXES.md)

### API文档
- [用户画像API](docs/api/USER_PROFILE_API.md)


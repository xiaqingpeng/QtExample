# 跨平台Qt应用发布工具集

## 🌟 概述

这是一套完整的跨平台Qt应用程序发布自动化工具，支持macOS、Linux和Windows平台。解决了GitHub Actions构建超时、文件路径不匹配、Release上传失败等常见问题。

## 🎯 核心特性

- ✅ **跨平台支持** - macOS、Linux、Windows
- ✅ **完整工具链** - 从构建到发布的全流程自动化
- ✅ **智能错误处理** - 自动重试和备用方案
- ✅ **流水线监控** - 实时GitHub Actions状态检测
- ✅ **交互式界面** - 友好的用户体验
- ✅ **详细文档** - 完善的使用指南和故障排除

## 📁 工具结构

```
├── macOS/Linux (Bash脚本)
│   ├── complete-release.sh          # 完整发布流程
│   ├── test-build.sh               # 本地构建测试
│   ├── recreate-tag.sh             # 标签重建和推送
│   ├── check-pipeline.sh           # 流水线状态检测
│   ├── upload-release.sh           # 手动上传Release
│   ├── create-lite-tag.sh          # 轻量级标签创建
│   └── quick-test.sh               # 环境检查
│
├── Windows (PowerShell脚本)
│   ├── complete-release-windows.ps1    # Windows完整发布流程
│   ├── test-build-windows.ps1         # Windows构建测试
│   ├── package-windows.ps1            # Windows应用打包
│   ├── upload-release-windows.ps1     # Windows Release上传
│   └── quick-test-windows.ps1         # Windows环境检查
│
├── GitHub Actions工作流
│   ├── .github/workflows/release.yml      # 完整版工作流
│   └── .github/workflows/release-lite.yml # 轻量级工作流
│
└── 文档
    ├── PIPELINE_DETECTION_GUIDE.md        # 流水线检测指南
    ├── GITHUB_ACTIONS_TROUBLESHOOTING.md  # 故障排除指南
    ├── WINDOWS_RELEASE_GUIDE.md           # Windows发布指南
    └── RELEASE_AUTOMATION_SUMMARY.md      # 完整总结
```

## 🚀 快速开始

### 1. 环境检查

#### macOS/Linux
```bash
chmod +x *.sh
./quick-test.sh
```

#### Windows
```powershell
Set-ExecutionPolicy -ExecutionPolicy RemoteSigned -Scope CurrentUser
.\quick-test-windows.ps1
```

### 2. 完整发布流程

#### macOS/Linux
```bash
./complete-release.sh
```

#### Windows
```powershell
.\complete-release-windows.ps1
```

## 📋 系统要求

### 通用要求
- Git 2.0+
- CMake 3.16+
- Qt 6.6.1+
- GitHub CLI (推荐)

### macOS特定
- Xcode Command Line Tools
- macOS 10.15+

### Linux特定
- GCC 7+ 或 Clang 6+
- 必要的系统库 (libgl1-mesa-dev, libxcb-*, 等)

### Windows特定
- PowerShell 5.0+
- Visual Studio 2017+ 或 Build Tools
- Windows 10/11

## 🎮 使用场景

### 场景1: 日常开发发布
```bash
# macOS/Linux
./complete-release.sh
# 选择选项 5 (完整流程)

# Windows
.\complete-release-windows.ps1
# 选择选项 4 (完整流程)
```

### 场景2: GitHub Actions超时问题
```bash
# 使用轻量级版本
./create-lite-tag.sh
./check-pipeline.sh
```

### 场景3: 手动构建和发布
```bash
# macOS/Linux
./test-build.sh
zip -r example-v1.0.1-macOS-qt6.6.1.zip build/example.app
./upload-release.sh example-v1.0.1-macOS-qt6.6.1.zip

# Windows
.\test-build-windows.ps1
.\package-windows.ps1
.\upload-release-windows.ps1 -AssetFile "example-v1.0.1-Windows-qt6.6.1.zip"
```

### 场景4: 流水线状态监控
```bash
# 实时监控GitHub Actions
./check-pipeline.sh
```

## 🔧 高级配置

### 自定义版本号
```bash
# 修改脚本中的版本配置
TAG_NAME="v2.0.0"
```

### 自定义Qt版本
```bash
# macOS/Linux
QT_VERSION="6.7.0"

# Windows
.\package-windows.ps1 -Version "v2.0.0" -QtVersion "qt6.7.0"
```

### 自定义构建配置
编辑CMakeLists.txt或使用CMakeLists-lite.txt（轻量级版本）

## 🐛 常见问题解决

### 1. GitHub Actions超时
**解决方案**: 使用轻量级工作流
```bash
./create-lite-tag.sh  # 创建 v1.0.0-lite 标签
```

### 2. 构建文件路径不匹配
**解决方案**: 已在脚本中统一文件名为 `example`

### 3. Release上传失败
**解决方案**: 使用手动上传脚本
```bash
./upload-release.sh <文件路径>
```

### 4. 依赖库缺失
**解决方案**: 
- macOS: 使用macdeployqt
- Linux: 复制.so文件
- Windows: 使用windeployqt

## 📊 平台对比

| 特性 | macOS | Linux | Windows |
|------|-------|-------|---------|
| 脚本语言 | Bash | Bash | PowerShell |
| 构建系统 | Ninja/Xcode | Ninja/Make | Visual Studio/Ninja |
| 包格式 | .app + ZIP | 可执行文件 + tar.gz | .exe + ZIP |
| 依赖部署 | 自动 | 手动 | windeployqt |
| 流水线检测 | ✅ | ✅ | 计划中 |

## 🎯 最佳实践

### 1. 开发流程
1. 本地开发和测试
2. 运行环境检查脚本
3. 执行本地构建测试
4. 使用完整发布流程

### 2. 版本管理
- 使用语义化版本号 (v1.0.0)
- 为测试版本使用 -lite 后缀
- 保持跨平台版本同步

### 3. 质量保证
- 在每个平台上测试构建
- 验证打包后的应用程序
- 检查所有依赖库

### 4. 发布策略
- 优先使用自动化流程
- 遇到问题时使用手动备用方案
- 监控GitHub Actions状态

## 🔗 相关资源

- [Qt跨平台开发指南](https://doc.qt.io/qt-6/supported-platforms.html)
- [CMake跨平台构建](https://cmake.org/cmake/help/latest/manual/cmake-platforms.7.html)
- [GitHub Actions文档](https://docs.github.com/en/actions)
- [GitHub CLI手册](https://cli.github.com/manual/)

## 🤝 贡献指南

欢迎提交问题报告和改进建议！

### 报告问题
1. 描述问题现象
2. 提供系统环境信息
3. 包含错误日志
4. 说明重现步骤

### 提交改进
1. Fork项目
2. 创建功能分支
3. 提交更改
4. 创建Pull Request

## 📄 许可证

本项目遵循项目主许可证条款。

---

**🎉 现在你拥有了一套完整的跨平台Qt应用发布解决方案！**

无论是在macOS、Linux还是Windows上，都能可靠地构建、打包和发布你的Qt应用程序。
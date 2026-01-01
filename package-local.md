# 本地打包指南

本文档说明如何在本地打包应用，支持 macOS、Linux 和 Windows 平台。

## 前提条件

### 通用要求
- 已安装 Qt 6.6.1（对应架构）
- 已安装 CMake（3.15 或更高版本）
- 已安装 Ninja 构建工具
- 已配置 `Qt6_DIR` 环境变量

### macOS
- Xcode 命令行工具
- macOS 10.15 或更高版本

### Linux
- GCC 编译器
- 构建工具：`build-essential`, `ninja-build`
- 依赖库：`libgl1-mesa-dev`, `libxcb-xinerama0`, `libxcb-cursor0`, `libfontconfig1`, `libxkbcommon-x11-0`

### Windows
- Visual Studio 2019 或更高版本
- MSVC 编译器
- PowerShell

## 使用方法

### macOS 和 Linux

#### 1. 设置环境变量

```bash
# 设置 Qt6_DIR 环境变量（根据您的 Qt 安装路径调整）
export Qt6_DIR=/path/to/Qt/6.6.1/clang_64  # macOS
export Qt6_DIR=/path/to/Qt/6.6.1/gcc_64    # Linux
```

#### 2. 给脚本添加执行权限

```bash
chmod +x package-local.sh
```

#### 3. 运行打包脚本

```bash
# 使用默认版本号 v1.0.0
./package-local.sh

# 指定版本号
./package-local.sh v1.0.1
```

### Windows

#### 1. 设置环境变量

在 PowerShell 中设置 `Qt6_DIR` 环境变量：

```powershell
$env:Qt6_DIR = "C:\Qt\6.6.1\msvc2019_64"
```

或者永久设置（系统环境变量）：

```powershell
[System.Environment]::SetEnvironmentVariable('Qt6_DIR', 'C:\Qt\6.6.1\msvc2019_64', 'User')
```

#### 2. 打开 PowerShell

以管理员身份打开 PowerShell，并进入项目目录。

#### 3. 运行打包脚本

```powershell
# 使用默认版本号 v1.0.0
.\package-local.ps1

# 指定版本号
.\package-local.ps1 v1.0.1
```

**注意**：如果 PowerShell 执行策略限制脚本运行，需要先执行：

```powershell
Set-ExecutionPolicy -ExecutionPolicy RemoteSigned -Scope CurrentUser
```

## 打包产物

### macOS
- 文件名：`example-{version}-macOS-qt6.6.1.zip`
- 内容：
  - `example.app` 应用包
  - `images/` 资源文件夹

### Linux
- 文件名：`example-{version}-Linux-qt6.6.1.tar.gz`
- 内容：
  - `example` 可执行文件
  - 共享库（`.so` 文件）
  - `images/` 资源文件夹

### Windows
- 文件名：`example-{version}-Windows-qt6.6.1.zip`
- 内容：
  - `example.exe` 可执行文件
  - 所需的 `.dll` 文件
  - `images/` 资源文件夹

## 清理构建产物

### macOS 和 Linux

```bash
rm -rf build-release example-*.zip example-*.tar.gz
```

### Windows

```powershell
Remove-Item -Recurse -Force build-release, example-*
```

## 常见问题

### 1. CMake 找不到 Qt

**错误信息**：`Could not find CMAKE_PREFIX_PATH` 或 `Could not find Qt6`

**解决方法**：确保 `Qt6_DIR` 环境变量已正确设置，并指向 Qt 6.6.1 的安装路径。

### 2. 构建失败

**错误信息**：编译错误或链接错误

**解决方法**：
- 检查 Qt 版本和架构是否匹配
- 确保所有依赖库已安装
- 查看详细的错误日志

### 3. PowerShell 脚本无法执行

**错误信息**：`无法加载文件，因为在此系统上禁止运行脚本`

**解决方法**：

```powershell
Set-ExecutionPolicy -ExecutionPolicy RemoteSigned -Scope CurrentUser
```

### 4. Windows 上缺少 DLL

**错误信息**：运行 `example.exe` 时提示缺少 DLL

**解决方法**：确保打包脚本正确复制了所有 DLL 文件。如果仍有问题，可以手动添加缺失的 DLL。

## 与 CI/CD 的区别

| 特性 | 本地打包 | CI/CD 打包 |
|------|---------|-----------|
| 触发方式 | 手动执行 | 推送 tag 时自动触发 |
| 构建平台 | 仅当前平台 | Windows、macOS、Linux 并行构建 |
| 上传位置 | 本地文件系统 | GitHub Release |
| 构建时间 | 较快（单平台） | 较慢（三平台并行） |
| 适用场景 | 本地测试、快速验证 | 正式发布、多平台分发 |

## 下一步

1. 在本地测试打包脚本
2. 验证打包产物的完整性
3. 在目标平台上测试运行
4. 确认无误后，使用 CI/CD 进行正式发布

## 相关文档

- [CI/CD 工作流](.github/workflows/release.yml)
- [发布流程](.github/workflows/release.md)
- [标签管理](.github/workflows/tag.md)
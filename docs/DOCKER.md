# 🐳 Docker 开发环境指南

> 完整的 Docker 开发环境配置和使用指南

## 📋 目录

- [快速开始](#快速开始)
- [环境准备](#环境准备)
- [运行模式](#运行模式)
- [常见问题](#常见问题)
- [高级配置](#高级配置)

---

## 快速开始

### 1. 启动容器

```bash
# 默认使用 offscreen 模式（推荐）
docker-compose up -d

# 或使用 X11 图形界面模式
docker-compose -f docker-compose.x11.yml up -d

# 或使用 offscreen 模式（显式指定）
docker-compose -f docker-compose.offscreen.yml up -d
```

### 2. 运行程序

#### 方式一：快速运行（自动检测模式）

```bash
./quick-run.sh
```

脚本会自动检测 XQuartz 是否运行，并选择最佳模式。

#### 方式二：指定运行模式

```bash
# X11 图形界面模式
./run-app-x11.sh

# Offscreen 模式（无图形界面）
./run-app-offscreen.sh
```

---

## 环境准备

### macOS 用户

#### 1. 安装 Docker Desktop

```bash
brew install --cask docker
```

#### 2. 配置文件共享（重要！）

如果遇到 "mounts denied" 错误，必须配置：

1. 打开 Docker Desktop
2. 进入 `Settings` → `Resources` → `File Sharing`
3. 点击 **"+"** 按钮
4. 添加项目路径：`/Applications/qingpengxia/qt/qt6/example`
5. 点击 `Apply & Restart`

**快速检查配置**：
```bash
./check-docker-setup.sh
```

#### 3. 配置 X11（可选，仅图形界面需要）

**安装 XQuartz**：
```bash
brew install --cask xquartz
```

**配置 XQuartz**：
1. 打开 XQuartz 应用
2. 打开偏好设置：`XQuartz -> 偏好设置` (或 `Cmd + ,`)
3. 切换到 `安全性` 标签
4. **勾选** `Allow connections from network clients`
5. 完全退出并重启 XQuartz

**允许本地连接**：
```bash
xhost +localhost
```

**验证连接**：
```bash
./test-x11-connection.sh
```

### Linux 用户

Linux 用户通常不需要额外配置，直接使用即可。

---

## 运行模式

### Offscreen 模式（推荐）

**优点**：
- ✅ 无需配置 X11
- ✅ 程序功能完全正常
- ✅ 不会出现显示相关错误
- ✅ 适合服务器环境

**使用方法**：
```bash
docker-compose -f docker-compose.offscreen.yml up -d
./run-app-offscreen.sh
```

### X11 图形界面模式

**优点**：
- ✅ 可以看到图形界面
- ✅ 适合调试 GUI 问题

**缺点**：
- ❌ 需要配置 XQuartz（macOS）
- ❌ 可能出现显示连接问题

**使用方法**：
```bash
# 确保 XQuartz 已配置（见上方）
docker-compose -f docker-compose.x11.yml up -d
./run-app-x11.sh
```

---

## 常见问题

### 问题 1: `could not connect to display`

**原因**：X11 显示服务器未正确配置

**解决方案**：

**方案 A：使用 Offscreen 模式（推荐）**
```bash
./run-app-offscreen.sh
```

**方案 B：配置 X11**
1. 确保 XQuartz 已安装并运行
2. 配置 XQuartz 允许网络连接（见上方）
3. 运行 `xhost +localhost`
4. 使用 `./run-app-x11.sh`

### 问题 2: `mounts denied`

**原因**：Docker Desktop 未配置文件共享

**解决方案**：
1. 打开 Docker Desktop
2. 进入 `Settings` → `Resources` → `File Sharing`
3. 添加项目路径
4. 重启 Docker Desktop

**快速修复**：
```bash
./fix-docker-sharing.sh
```

### 问题 3: 程序崩溃（Segmentation fault）

**原因**：在 Docker 环境中，`QWebEngineView` 可能导致崩溃

**解决方案**：
程序已自动检测 Docker 环境并禁用 WebEngine，使用 `QLabel` 替代。如果仍然崩溃，请检查：
1. 程序是否重新编译
2. 查看日志输出中的警告信息

### 问题 4: `XDG_RUNTIME_DIR not set`

**状态**：✅ 已自动修复

脚本和 docker-compose 配置已自动设置 `XDG_RUNTIME_DIR=/tmp/runtime-root`。

### 问题 5: `This plugin does not support propagateSizeHints()`

**状态**：⚠️ 正常警告，可忽略

这是 Qt 在容器环境中的正常警告，不影响程序功能。

---

## 高级配置

### 使用国内镜像源

如果遇到下载速度慢的问题，可以使用国内镜像源：

```bash
# 使用中国镜像配置
docker-compose -f docker-compose.cn.yml up -d
```

### 容器管理

**查看运行中的容器**：
```bash
docker ps
```

**进入容器**：
```bash
docker exec -it <容器名> bash
```

**重新编译**：
```bash
docker exec -it <容器名> bash -c "cd /workspace/build && make -j\$(nproc)"
```

**停止容器**：
```bash
docker-compose down
```

### 镜像加速配置

如果 Docker 镜像下载慢，可以配置镜像加速器。详见 `Docker-镜像加速配置.md`。

---

## 构建脚本

### 一键构建和运行

```bash
# 构建 Docker 镜像并编译项目
./docker-build-and-run.sh
```

### 仅构建

```bash
# 构建 Docker 镜像
docker-compose build qt-dev

# 在容器中编译
docker-compose run --rm qt-dev bash -c "cd /workspace/build && cmake .. && make -j\$(nproc)"
```

### 修复构建缓存

如果遇到 CMake 缓存路径问题：

```bash
./fix-build-cache.sh
```

---

## 相关文档

- [X11/XQuartz 设置详细说明](../X11-SETUP.md) - X11 配置的详细步骤
- [Windows 构建说明](../WINDOWS-构建说明.md) - Windows 平台构建指南
- [跨平台兼容性说明](../跨平台兼容性说明.md) - 跨平台开发注意事项
- [Docker 镜像加速配置](../Docker-镜像加速配置.md) - 配置国内镜像源

---

## 脚本说明

| 脚本 | 说明 |
|------|------|
| `quick-run.sh` | 快速运行（自动检测模式） |
| `run-app-x11.sh` | X11 图形界面模式 |
| `run-app-offscreen.sh` | Offscreen 模式 |
| `test-x11-connection.sh` | 测试 X11 连接 |
| `check-docker-setup.sh` | 检查 Docker 配置 |
| `fix-docker-sharing.sh` | 修复 Docker 文件共享 |

---

**最后更新**：2026-01-17


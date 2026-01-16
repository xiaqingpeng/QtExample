# 🎯 Qt 项目 Docker 开发环境通用指南

> **本文档适用于任意 Qt 桌面项目**  
> 使用前请将文档中的 `{{PROJECT_NAME}}` 和 `{{EXECUTABLE_NAME}}` 替换为你的实际项目名和可执行文件名。

---

## 📋 目录

- [1. 快速开始](#1-快速开始)
- [2. 环境准备](#2-环境准备)
- [3. 使用 Docker 构建](#3-使用-docker-构建)
- [4. 运行 Qt GUI 程序](#4-运行-qt-gui-程序)
- [5. 无界面运行](#5-无界面运行)
- [6. 常用命令](#6-常用命令)
- [7. 项目结构说明](#7-项目结构说明)
- [8. 常见问题](#8-常见问题)

---

## 1. 快速开始

### 1.1 目录结构

你的项目目录建议如下：

```text
{{PROJECT_NAME}}/
  CMakeLists.txt          # CMake 构建配置
  main.cpp                # 主程序入口
  ... 其他源码文件 ...
  Dockerfile              # Docker 镜像定义（从模板复制）
  docker-compose.yml      # Docker 编排配置（从模板复制）
  build-docker-linux.sh   # 一键构建脚本（从模板复制）
```

### 1.2 使用模板文件

1. **复制模板文件到你的项目**：
   ```bash
   # 假设你在项目根目录
   cp Dockerfile.template Dockerfile
   cp docker-compose.yml.template docker-compose.yml
   cp build-docker-linux.sh.template build-docker-linux.sh
   chmod +x build-docker-linux.sh
   ```

2. **修改模板中的占位符**：
   - 在 `docker-compose.yml` 中，将 `{{PROJECT_NAME}}` 替换为你的项目名
   - 在 `build-docker-linux.sh` 中，将 `{{EXECUTABLE_NAME}}` 替换为你的可执行文件名

3. **一键构建**：
   ```bash
   ./build-docker-linux.sh
   ```

---

## 2. 环境准备

### 2.1 安装 Docker Desktop

- **macOS**: 下载并安装 [Docker Desktop for Mac](https://www.docker.com/products/docker-desktop/)
- **Windows**: 下载并安装 [Docker Desktop for Windows](https://www.docker.com/products/docker-desktop/)
- **Linux**: 安装 Docker Engine（参考 [官方文档](https://docs.docker.com/engine/install/)）

安装完成后，启动 Docker Desktop，确认 Docker 正常运行：

```bash
docker info
```

### 2.2 macOS 配置文件共享路径

如果你的项目不在默认的用户目录下（如 `/Users/username/...`），需要配置 Docker 文件共享：

1. 打开 **Docker Desktop**
2. 进入：`Settings` → `Resources` → `File Sharing`
3. 点击 **"+"** 按钮，添加你的项目所在路径，例如：
   - `/Users/<yourname>/projects`
   - `/Applications/<yourname>/qt-projects`
4. 点击 **Apply & Restart**，等待 Docker 重启完成

### 2.3 macOS 安装 XQuartz（仅 GUI 程序需要）

如果你需要在 Mac 上看到 Linux Qt 窗口，需要安装 XQuartz：

```bash
brew install --cask xquartz
```

安装完成后：
1. **注销并重新登录**（或重启电脑）
2. 启动 XQuartz（应用程序 → 实用工具 → XQuartz）
3. 配置 XQuartz：
   - `XQuartz` → `Preferences…` → `Security`
   - 勾选：**Allow connections from network clients**
4. 关闭并重新启动 XQuartz

---

## 3. 使用 Docker 构建

### 3.1 一键构建脚本

在项目根目录执行：

```bash
./build-docker-linux.sh
```

脚本会自动完成：

- ✅ 检查 Docker 是否安装并运行
- ✅ 使用 `Dockerfile` 构建镜像（例如 `{{PROJECT_NAME}}-dev`）
- ✅ 在容器中执行 CMake 构建：
  - `mkdir -p build && cd build`
  - `cmake ..`
  - `make -j$(nproc)`
- ✅ 编译产物默认在宿主机的 `build/` 目录下

### 3.2 查看编译产物

构建完成后，查看编译产物：

```bash
ls -lh build/
# 应该能看到你的可执行文件，例如：build/{{EXECUTABLE_NAME}}
```

---

## 4. 运行 Qt GUI 程序

### 4.1 允许 X11 连接（macOS）

在 macOS 终端执行：

```bash
xhost +localhost
# 如果 xhost 不在 PATH，使用：
/opt/X11/bin/xhost +localhost
```

看到类似输出即可：

```text
localhost being added to access control list
```

> **提示**：为方便调试，你也可以临时使用 `xhost +`（允许所有连接），测试完成后再用 `xhost -` 收紧权限。

### 4.2 进入开发容器并运行程序

在项目根目录执行：

```bash
docker-compose run --rm qt-dev bash
```

进入容器后：

```bash
cd /workspace/build
./{{EXECUTABLE_NAME}}
```

如果 XQuartz 和权限都配置正确，应该能在宿主机上看到 Qt 窗口。

### 4.3 后台运行容器（可选）

如果你想让容器保持运行，可以：

```bash
# 启动容器（后台运行）
docker-compose up -d

# 进入已运行的容器
docker-compose exec qt-dev bash

# 在容器中运行程序
cd /workspace/build
./{{EXECUTABLE_NAME}}
```

---

## 5. 无界面运行

如果你只想验证程序逻辑，不需要显示窗口，可以使用 offscreen 模式：

```bash
docker-compose run --rm qt-dev bash
cd /workspace/build
QT_QPA_PLATFORM=offscreen ./{{EXECUTABLE_NAME}}
```

这种方式不会尝试连接 X11，也不会弹出窗口，适合：
- CI/CD 自动化测试
- 纯逻辑验证
- 服务器环境

---

## 6. 常用命令

### 6.1 构建相关

```bash
# 一键构建（推荐）
./build-docker-linux.sh

# 手动构建镜像
docker-compose build qt-dev

# 手动编译（在容器中）
docker-compose run --rm qt-dev bash -c "cd build && cmake .. && make"
```

### 6.2 容器管理

```bash
# 进入开发容器（交互式）
docker-compose run --rm qt-dev bash

# 后台启动服务
docker-compose up -d

# 进入已运行的容器
docker-compose exec qt-dev bash

# 查看容器状态
docker-compose ps

# 查看日志
docker-compose logs -f qt-dev

# 停止并删除容器
docker-compose down

# 停止并删除容器和卷（清理构建缓存）
docker-compose down -v
```

### 6.3 镜像管理

```bash
# 查看镜像
docker images | grep {{PROJECT_NAME}}

# 删除镜像
docker rmi {{PROJECT_NAME}}-dev

# 清理未使用的镜像
docker image prune
```

---

## 7. 项目结构说明

### 7.1 Docker 配置文件

| 文件 | 说明 |
|------|------|
| `Dockerfile` | 定义 Qt 开发环境（Ubuntu + Qt6） |
| `docker-compose.yml` | Docker 编排配置，定义服务、挂载、环境变量 |
| `build-docker-linux.sh` | 一键构建脚本，方便快速上手 |

### 7.2 自定义构建系统

如果你的项目使用 **qmake** 而不是 CMake，可以修改 `docker-compose.yml` 中的 `command`：

```yaml
command: >
  /bin/bash -c "mkdir -p build &&
                cd build &&
                qmake .. &&
                make -j\$(nproc)"
```

或者修改 `build-docker-linux.sh` 中的构建命令。

### 7.3 修改可执行文件名

如果你的可执行文件名不是 `{{EXECUTABLE_NAME}}`，需要修改：

1. **CMakeLists.txt** 中的 `add_executable` 或 `qt_add_executable`
2. **build-docker-linux.sh** 中提示文字里的可执行文件名
3. **本文档** 中所有 `{{EXECUTABLE_NAME}}` 占位符

---

## 8. 常见问题

### Q1: `Cannot connect to the Docker daemon`

**原因**：Docker Desktop 未启动

**解决**：
1. 打开 Docker Desktop
2. 等待 Docker 完全启动（菜单栏图标不再显示 "Starting..."）
3. 在终端执行 `docker info` 确认连接成功

---

### Q2: `mounts denied: path is not shared from the host`

**原因**：项目路径未添加到 Docker 文件共享列表

**解决**：
1. Docker Desktop → `Settings` → `Resources` → `File Sharing`
2. 添加你的项目目录（或上层目录）
3. 点击 `Apply & Restart`
4. 重新执行构建命令

---

### Q3: `qt.qpa.xcb: could not connect to display`

**原因**：X11 显示服务器未正确配置

**解决步骤**：

1. **确认 XQuartz 已启动**：
   - 在应用程序中找到 XQuartz，双击启动
   - 确认菜单栏有 XQuartz 图标

2. **确认 XQuartz 允许网络连接**：
   - `XQuartz` → `Preferences…` → `Security`
   - 勾选：**Allow connections from network clients**
   - 重启 XQuartz

3. **确认 xhost 权限**：
   ```bash
   xhost +localhost
   # 或临时允许所有连接：
   xhost +
   ```

4. **在容器中检查 DISPLAY**：
   ```bash
   docker-compose run --rm qt-dev bash
   echo $DISPLAY
   # 应该输出：host.docker.internal:0
   ```

---

### Q4: `Authorization required, but no authorization protocol specified`

**原因**：X11 授权失败

**解决**：
```bash
# 在 macOS 终端执行
xhost +localhost
# 或临时允许所有连接：
xhost +
```

---

### Q5: 只想验证逻辑，不想折腾图形环境

**解决**：使用 offscreen 模式：

```bash
docker-compose run --rm qt-dev bash
cd /workspace/build
QT_QPA_PLATFORM=offscreen ./{{EXECUTABLE_NAME}}
```

---

### Q6: 容器内编译很慢

**可能原因**：
- Docker Desktop 资源分配不足
- 未使用并行编译

**解决**：
1. Docker Desktop → `Settings` → `Resources` → 增加 CPU 和内存分配
2. 确认构建脚本使用了 `make -j$(nproc)`（并行编译）

---

### Q7: 如何清理 Docker 资源

```bash
# 停止并删除容器
docker-compose down

# 删除未使用的镜像
docker image prune

# 删除未使用的卷（注意：会删除构建缓存）
docker volume prune

# 全面清理（谨慎使用）
docker system prune -a
```

---

## 9. 进阶配置

### 9.1 自定义 Qt 版本

修改 `Dockerfile` 中的 Qt 包版本：

```dockerfile
# 例如：安装 Qt 5
RUN apt-get update && apt-get install -y \
    qt5-default \
    qtbase5-dev \
    qttools5-dev \
    ...
```

### 9.2 添加额外的系统依赖

在 `Dockerfile` 中添加：

```dockerfile
RUN apt-get update && apt-get install -y \
    your-package-name \
    ...
```

### 9.3 使用自定义构建脚本

创建 `build.sh`，然后在 `docker-compose.yml` 中挂载并执行：

```yaml
volumes:
  - .:/workspace
  - ./build.sh:/workspace/build.sh
command: /bin/bash /workspace/build.sh
```

---

## 10. 总结

使用 Docker 进行 Qt 开发的优势：

- ✅ **环境一致性**：团队成员使用相同的开发环境
- ✅ **跨平台**：在 macOS/Windows 上也能编译 Linux 版本
- ✅ **隔离性**：不影响本地系统环境
- ✅ **可移植性**：配置即代码，易于版本控制

---

## 📚 参考资源

- [Docker 官方文档](https://docs.docker.com/)
- [Qt 官方文档](https://doc.qt.io/)
- [XQuartz 官网](https://www.xquartz.org/)
- [CMake 官方文档](https://cmake.org/documentation/)

---

**祝开发愉快！** 🎉


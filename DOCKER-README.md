# 🐳 Docker + X11 Linux 环境使用说明

本项目已配置好 Docker + X11 环境，可以在 macOS/Windows 上编译和运行 Linux 版本的 Qt 应用。

## 📋 快速开始

### 1. 环境准备

#### macOS 用户

1. **安装 Docker Desktop**
   ```bash
   # 通过 Homebrew 安装
   brew install --cask docker
   ```

2. **配置 Docker 文件共享（重要！）**
   
   ⚠️ **如果遇到 "mounts denied" 错误，必须配置此步骤！**
   
   - 打开 Docker Desktop
   - 进入 `Settings` → `Resources` → `File Sharing`
   - 点击 **"+"** 按钮
   - 添加项目路径：`/Applications/qingpengxia/qt/qt6/example`
   - 点击 `Apply & Restart`
   - 等待 Docker Desktop 完全重启
   
   **快速检查配置**：
   ```bash
   ./check-docker-setup.sh
   ```
   
   📖 **详细配置说明**：查看 `DOCKER-SETUP.md`

3. **安装并配置 XQuartz（用于显示 GUI）**
   ```bash
   brew install --cask xquartz
   ```
   - 安装后**注销并重新登录**（或重启电脑）
   - 启动 XQuartz（应用程序 → 实用工具 → XQuartz）
   - `XQuartz` → `Preferences…` → `Security`
   - 勾选：**Allow connections from network clients**
   - 重启 XQuartz

#### Linux 用户

1. **安装 Docker Engine**
   ```bash
   # Ubuntu/Debian
   sudo apt-get update
   sudo apt-get install docker.io docker-compose
   sudo systemctl start docker
   sudo systemctl enable docker
   ```

2. **配置 X11 转发**
   ```bash
   xhost +local:docker
   ```

### 2. 构建项目

在项目根目录执行：

```bash
./build-docker-linux.sh
```

脚本会自动：
- ✅ 检查 Docker 环境
- ✅ 构建 Docker 镜像
- ✅ 在容器中编译项目
- ✅ 生成可执行文件到 `build/example`

### 3. 运行程序

#### 方式一：带 GUI 运行（需要 X11）

**macOS：**
```bash
# 1. 允许 X11 连接
xhost +localhost

# 2. 进入容器
docker-compose run --rm qt-dev bash

# 3. 在容器中运行程序
cd /workspace/build
./example
```

**Linux：**
```bash
# 1. 进入容器
docker-compose run --rm qt-dev bash

# 2. 在容器中运行程序
cd /workspace/build
./example
```

#### 方式二：无界面运行（offscreen 模式）

```bash
docker-compose run --rm qt-dev bash
cd /workspace/build
QT_QPA_PLATFORM=offscreen ./example
```

## 📁 文件说明

| 文件 | 说明 |
|------|------|
| `Dockerfile` | Docker 镜像定义，包含 Ubuntu 22.04 + Qt6 开发环境 |
| `docker-compose.yml` | Docker 编排配置，定义服务、挂载、环境变量 |
| `build-docker-linux.sh` | 一键构建脚本 |
| `.dockerignore` | Docker 构建时忽略的文件列表 |

## 🔧 常用命令

### 构建相关
```bash
# 一键构建
./build-docker-linux.sh

# 手动构建镜像
docker-compose build qt-dev

# 手动编译
docker-compose run --rm qt-dev bash -c "cd build && cmake .. && make -j\$(nproc)"
```

### 容器管理
```bash
# 进入开发容器
docker-compose run --rm qt-dev bash

# 后台启动服务
docker-compose up -d

# 进入已运行的容器
docker-compose exec qt-dev bash

# 查看容器状态
docker-compose ps

# 停止并删除容器
docker-compose down
```

### 清理资源
```bash
# 停止并删除容器和卷
docker-compose down -v

# 删除未使用的镜像
docker image prune
```

## ❓ 常见问题

### Q0: Docker 构建时出现 `502 Bad Gateway` 或网络错误

**原因**：Ubuntu 软件源网络连接不稳定

**解决方案**：

1. **使用国内镜像源（推荐，如果在中国）**：
   ```bash
   # 使用国内镜像源的 Dockerfile
   docker build -f Dockerfile.cn -t qt-example-dev .
   
   # 然后修改 docker-compose.yml 中的 build.dockerfile 为 Dockerfile.cn
   ```

2. **重试构建**：
   ```bash
   # 直接重试，Dockerfile 已包含重试机制
   ./build-docker-linux.sh
   ```

3. **手动构建并重试**：
   ```bash
   docker-compose build --no-cache qt-dev
   ```

### Q1: `Cannot connect to the Docker daemon`
**解决**：启动 Docker Desktop，等待完全启动后再试。

### Q2: `mounts denied: path is not shared`
**解决**：
1. 打开 Docker Desktop → Settings → Resources → File Sharing
2. 点击 "+" 添加项目路径：`/Applications/qingpengxia/qt/qt6/example`
3. 点击 Apply & Restart
4. 等待 Docker Desktop 完全重启
5. 运行 `./check-docker-setup.sh` 验证配置

📖 **详细步骤**：查看 `DOCKER-SETUP.md`

### Q3: `qt.qpa.xcb: could not connect to display`
**解决**：
1. 确认 XQuartz 已启动并允许网络连接
2. 执行 `xhost +localhost`
3. 检查容器中的 `DISPLAY` 环境变量：`echo $DISPLAY`

### Q4: 只想验证逻辑，不想配置图形环境
**解决**：使用 offscreen 模式：
```bash
QT_QPA_PLATFORM=offscreen ./example
```

## 📚 详细文档

更多详细信息请参考：
- `Qt-Docker-通用开发指南.md` - 完整的使用指南
- `README-模板使用说明.md` - 模板文件使用说明

---

**祝开发愉快！** 🎉


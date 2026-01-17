# Docker 运行指南

## 快速开始

### 1. 启动容器
```bash
docker-compose up -d
```

### 2. 运行程序

#### 方式一：使用 offscreen 模式（推荐，无需 X11）
```bash
./run-app-offscreen.sh
```

#### 方式二：使用图形界面（需要 XQuartz）

**macOS 上设置 X11 转发：**

1. 安装 XQuartz：
   ```bash
   brew install --cask xquartz
   ```

2. 启动 XQuartz 应用

3. 允许本地连接：
   ```bash
   xhost +localhost
   ```

4. 运行程序：
   ```bash
   ./run-app.sh
   ```

## 常见问题

### 问题：`could not connect to display`

**解决方案：**
- 使用 offscreen 模式：`./run-app-offscreen.sh`
- 或者设置 X11 转发（见上方）

### 问题：程序崩溃（Segmentation fault）

**解决方案：**
- 程序已自动检测 Docker 环境并禁用 WebEngine
- 设备信息页面会使用文本显示，而不是图表
- 这是正常行为，不会影响功能

## 容器管理

### 查看运行中的容器
```bash
docker ps
```

### 进入容器
```bash
docker exec -it <容器名> bash
```

### 重新编译
```bash
docker exec <容器名> bash -c "cd /workspace/build && make -j\$(nproc)"
```

### 停止容器
```bash
docker-compose down
```


# Docker X11 显示问题解决方案

## 问题描述
```
qt.qpa.xcb: could not connect to display host.docker.internal:0
qt.qpa.plugin: Could not load the Qt platform plugin "xcb"
```

## 解决方案

### 方案一：使用 Offscreen 模式（推荐，最简单）

**优点：**
- 无需配置 X11
- 程序功能完全正常
- 不会出现显示相关错误

**使用方法：**

1. **使用默认配置（已修改为 offscreen 模式）**：
   ```bash
   docker-compose up -d
   docker exec -it <容器名> /workspace/build/example
   ```

2. **或使用专门的 offscreen 配置**：
   ```bash
   docker-compose -f docker-compose.offscreen.yml up -d
   docker exec -it qt-example-linux-dev-offscreen /workspace/build/example
   ```

3. **或使用脚本**：
   ```bash
   ./run-app-offscreen.sh
   ```

### 方案二：配置 X11 图形界面

**macOS 上设置 X11：**

1. **安装 XQuartz**：
   ```bash
   brew install --cask xquartz
   ```

2. **启动 XQuartz 应用**（在应用程序中找到并打开）

3. **允许本地连接**：
   ```bash
   xhost +localhost
   ```

4. **使用 X11 配置启动容器**：
   ```bash
   # 设置 DISPLAY 环境变量
   export DISPLAY=:0
   
   # 使用 X11 配置
   docker-compose -f docker-compose.x11.yml up -d
   
   # 运行程序
   docker exec -it qt-example-linux-dev-x11 /workspace/build/example
   ```

## 当前配置说明

**默认 docker-compose.yml** 已修改为：
- 默认使用 `offscreen` 模式
- 如果设置了 `QT_QPA_PLATFORM` 环境变量，会使用该值
- 可以通过环境变量覆盖：`QT_QPA_PLATFORM=xcb docker-compose up -d`

## 验证程序运行

**检查进程：**
```bash
docker exec <容器名> ps aux | grep example
```

**查看日志：**
```bash
docker exec <容器名> bash -c "cd /workspace/build && QT_QPA_PLATFORM=offscreen ./example 2>&1"
```

## 重要提示

- **Offscreen 模式下程序功能完全正常**，只是没有可见的图形界面
- Docker 环境检测已启用，WebEngine 会自动禁用（避免段错误）
- 设备信息页面会使用文本显示，而不是 WebEngine 图表
- 所有警告信息（XDG_RUNTIME_DIR, propagateSizeHints）都是正常的，可以忽略


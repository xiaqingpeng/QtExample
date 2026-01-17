# X11/XQuartz 设置指南

## 问题：无法连接到 X11 显示服务器

如果遇到 `could not connect to display` 错误，请按以下步骤操作：

## 步骤 1: 配置 XQuartz

1. **打开 XQuartz 应用**（在应用程序中找到）

2. **打开偏好设置**：
   - 点击菜单：`XQuartz -> 偏好设置` (Preferences)
   - 或使用快捷键：`Cmd + ,`

3. **启用网络连接**：
   - 切换到 `安全性` (Security) 标签
   - **勾选** `Allow connections from network clients`
   - 这个选项允许来自网络的 X11 连接

4. **重启 XQuartz**：
   - 完全退出 XQuartz（右键 Dock 图标 -> 退出）
   - 重新启动 XQuartz

## 步骤 2: 配置 xhost

在终端中运行：

```bash
xhost +localhost
```

或者允许所有连接（不推荐，但用于测试）：

```bash
xhost +
```

## 步骤 3: 运行程序

```bash
./run-app-x11.sh
```

脚本会自动：
1. 先尝试 `host.docker.internal:0`
2. 如果失败，自动切换到 IP 地址（如 `192.168.0.194:0`）

## 验证连接

运行测试脚本：

```bash
../../scripts/docker/test-x11-connection.sh
```

## 常见警告

### `QStandardPaths: XDG_RUNTIME_DIR not set`
**状态**: ✅ 已修复
- 脚本和 docker-compose 配置已自动设置 `XDG_RUNTIME_DIR=/tmp/runtime-root`
- 如果手动运行，请设置：`export XDG_RUNTIME_DIR=/tmp/runtime-root`

### `This plugin does not support propagateSizeHints()`
**状态**: ⚠️ 正常警告，可忽略
- 这是 Qt 在容器环境中的正常警告
- 不影响程序功能
- 与 X11 平台插件的某些特性相关
- 可以安全忽略

## 常见问题

### Q: 仍然无法连接
A: 检查防火墙设置，确保允许 X11 连接（端口 6000）

### Q: 连接成功但窗口不显示
A: 检查 XQuartz 窗口是否被最小化，或尝试点击 XQuartz 图标

### Q: 想使用 offscreen 模式
A: 运行 `./run-app-offscreen.sh`，无需 X11 配置

## 备选方案：使用 IP 地址

如果 `host.docker.internal` 不工作，可以直接使用 IP 地址：

```bash
# 获取 macOS IP
ipconfig getifaddr en0

# 在容器内使用
export DISPLAY=<你的IP>:0
./example
```


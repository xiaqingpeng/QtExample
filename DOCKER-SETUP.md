# 🛠️ Docker 环境配置指南

## ⚠️ 重要：macOS 文件共享配置

如果你看到以下错误：
```
Error response from daemon: mounts denied: 
The path /Applications/qingpengxia/qt/qt6/example is not shared from the host and is not known to Docker.
```

**这是因为 Docker Desktop 需要明确配置哪些路径可以共享给容器。**

## 📝 解决步骤

### 步骤 1：打开 Docker Desktop

1. 点击菜单栏的 Docker 图标（🐳）
2. 选择 **Settings**（或 **Preferences**）

### 步骤 2：配置文件共享

1. 在左侧菜单中，点击 **Resources** → **File Sharing**
2. 点击 **"+"** 按钮（或 **Add** 按钮）
3. 添加以下路径之一：

   **选项 A：添加项目目录（推荐）**
   ```
   /Applications/qingpengxia/qt/qt6/example
   ```

   **选项 B：添加父目录（如果项目会移动）**
   ```
   /Applications/qingpengxia/qt/qt6
   ```

   **选项 C：添加整个 Applications 目录（最简单，但安全性较低）**
   ```
   /Applications
   ```

4. 点击 **Apply & Restart**
5. 等待 Docker Desktop 重启完成（菜单栏图标会显示 "Starting..." 然后恢复正常）

### 步骤 3：验证配置

在终端执行：

```bash
# 检查 Docker 是否正常运行
docker info

# 尝试运行容器（应该不再报错）
docker-compose run --rm qt-dev echo "Hello from container"
```

如果看到 "Hello from container" 输出，说明配置成功！

## 🔍 快速检查脚本

运行以下命令检查配置：

```bash
# 检查 Docker 是否运行
if docker info &> /dev/null; then
    echo "✅ Docker 正在运行"
else
    echo "❌ Docker 未运行，请启动 Docker Desktop"
    exit 1
fi

# 检查文件共享配置
echo "📁 当前项目路径："
pwd

echo ""
echo "💡 请确保在 Docker Desktop 的 File Sharing 中添加了以下路径之一："
echo "   - $(pwd)"
echo "   - $(dirname $(pwd))"
echo "   - /Applications"
```

## 🚀 配置完成后

配置完成后，你可以：

1. **构建项目**：
   ```bash
   ./build-docker-linux.sh
   ```

2. **进入容器**：
   ```bash
   docker-compose run --rm qt-dev bash
   ```

3. **运行程序**：
   ```bash
   cd /workspace/build
   ./example
   ```

## ❓ 常见问题

### Q: 添加路径后仍然报错？

**解决**：
1. 确认 Docker Desktop 已完全重启（等待菜单栏图标恢复正常）
2. 确认路径拼写正确（区分大小写）
3. 尝试添加父目录而不是项目目录
4. 重启 Docker Desktop：菜单栏 → Docker → Quit Docker Desktop，然后重新启动

### Q: 如何查看已配置的共享路径？

**方法**：
1. Docker Desktop → Settings → Resources → File Sharing
2. 查看列表中的路径

### Q: 可以添加多个路径吗？

**可以**！你可以添加多个路径，每个路径一行。

### Q: 添加 `/Applications` 安全吗？

**说明**：
- 功能上：完全安全，Docker 容器只能访问你明确挂载的目录
- 性能上：可能会稍微影响 Docker Desktop 的启动速度（因为需要索引更多文件）
- 建议：如果项目位置固定，只添加项目目录更合适

## 📚 参考

- [Docker Desktop 文件共享文档](https://docs.docker.com/desktop/settings/mac/#file-sharing)
- [Docker Desktop macOS 文件共享指南](https://docs.docker.com/go/mac-file-sharing/)

---

**配置完成后，就可以正常使用 Docker 环境了！** 🎉


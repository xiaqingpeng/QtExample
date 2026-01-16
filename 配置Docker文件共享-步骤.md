# 🔧 配置 Docker 文件共享 - 详细步骤

## ⚠️ 当前问题

你看到这个错误：
```
Error response from daemon: mounts denied: 
The path /Applications/qingpengxia/qt/qt6/example is not shared from the host
```

**这是因为 Docker Desktop 需要明确配置哪些路径可以共享给容器。**

---

## 📋 配置步骤（必须完成）

### 步骤 1: 打开 Docker Desktop

**方法 A：通过菜单栏（推荐）**
1. 查看屏幕顶部菜单栏
2. 找到 Docker 图标（🐳）
3. 点击图标

**方法 B：通过 Spotlight**
1. 按 `⌘ + Space`
2. 输入 `Docker Desktop`
3. 按回车

**方法 C：使用脚本**
```bash
./open-docker.sh
```

### 步骤 2: 进入设置

1. 点击菜单栏的 Docker 图标（🐳）
2. 选择 **Settings**（或 **Preferences**）

   > 如果看不到菜单，Docker Desktop 可能正在启动，等待 30-60 秒

### 步骤 3: 找到文件共享设置

1. 在左侧菜单中，点击 **Resources**
2. 点击 **File Sharing**

### 步骤 4: 添加项目路径

1. 点击 **"+"** 按钮（或 **Add** 按钮）
2. 在输入框中输入或选择以下路径：

   ```
   /Applications/qingpengxia/qt/qt6/example
   ```

   > 💡 **提示**：你也可以添加父目录 `/Applications/qingpengxia/qt/qt6` 或整个 `/Applications`

3. 路径会出现在列表中

### 步骤 5: 应用并重启（重要！）

1. 点击 **Apply & Restart** 按钮
2. **等待 Docker Desktop 完全重启**
   - 菜单栏图标会显示 "Starting..."
   - 等待图标恢复正常（不再是 "Starting..."）
   - 通常需要 30-60 秒

### 步骤 6: 验证配置

重启完成后，运行验证脚本：

```bash
./verify-docker-sharing.sh
```

如果看到 "✅ 文件共享配置成功！"，说明配置完成！

---

## 🎯 快速验证

配置完成后，测试：

```bash
# 方法 1: 使用验证脚本
./verify-docker-sharing.sh

# 方法 2: 直接测试 Docker
docker-compose run --rm qt-dev echo "配置成功！"
```

如果成功，你应该看到 "配置成功！" 的输出。

---

## ❓ 常见问题

### Q: 添加路径后仍然报错？

**可能原因**：
1. Docker Desktop 没有完全重启
2. 路径拼写错误
3. 权限问题

**解决**：
1. 确认 Docker Desktop 已完全重启（菜单栏图标恢复正常）
2. 检查路径是否正确（区分大小写）
3. 尝试添加父目录而不是项目目录
4. 重启 Docker Desktop：菜单栏 → Docker → Quit Docker Desktop，然后重新打开

### Q: 如何确认 Docker Desktop 已重启？

**检查方法**：
1. 菜单栏的 Docker 图标应该显示正常运行状态
2. 点击图标，应该能看到菜单（Dashboard, Settings 等）
3. 运行 `docker info` 应该能正常输出

### Q: 可以添加多个路径吗？

**可以**！你可以添加多个路径，每个路径一行。

### Q: 添加 `/Applications` 安全吗？

**说明**：
- **功能上**：完全安全，Docker 容器只能访问你明确挂载的目录
- **性能上**：可能会稍微影响 Docker Desktop 的启动速度
- **建议**：如果项目位置固定，只添加项目目录更合适

---

## 📸 配置界面位置

Docker Desktop 设置界面结构：
```
Docker Desktop
├── General
├── Resources
│   ├── Advanced
│   ├── CPUs
│   ├── Memory
│   ├── Disk
│   └── File Sharing  ← 在这里！
├── Docker Engine
├── Features in development
└── ...
```

---

## ✅ 配置完成后的下一步

配置完成后，你可以：

```bash
# 1. 构建项目
./build-docker-linux.sh

# 2. 进入容器
docker-compose run --rm qt-dev bash

# 3. 在容器中运行程序
cd /workspace/build
./example
```

---

**重要提示**：配置后必须等待 Docker Desktop 完全重启，文件共享才会生效！


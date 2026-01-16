# 📦 Qt Docker 模板文件使用说明

本目录包含了一套通用的 Qt Docker 开发环境模板，可以快速应用到任意 Qt 项目中。

---

## 📁 模板文件列表

| 文件 | 说明 | 用途 |
|------|------|------|
| `Qt-Docker-通用开发指南.md` | 完整的使用文档 | 参考文档，包含详细的使用说明和常见问题 |
| `Dockerfile.template` | Docker 镜像定义模板 | 定义 Qt 开发环境（Ubuntu + Qt6） |
| `docker-compose.yml.template` | Docker 编排配置模板 | 定义服务、挂载、环境变量 |
| `build-docker-linux.sh.template` | 一键构建脚本模板 | 自动化构建流程 |

---

## 🚀 快速开始

### 步骤 1: 复制模板文件到你的项目

假设你的项目目录是 `~/projects/my-qt-app`：

```bash
cd ~/projects/my-qt-app

# 复制模板文件（去掉 .template 后缀）
cp /path/to/templates/Dockerfile.template Dockerfile
cp /path/to/templates/docker-compose.yml.template docker-compose.yml
cp /path/to/templates/build-docker-linux.sh.template build-docker-linux.sh

# 添加执行权限
chmod +x build-docker-linux.sh
```

### 步骤 2: 修改占位符

#### 2.1 修改 `docker-compose.yml`

将 `{{PROJECT_NAME}}` 替换为你的项目名：

```yaml
# 例如：项目名是 my-qt-app
container_name: my-qt-app-linux-dev
```

#### 2.2 修改 `build-docker-linux.sh`

将 `{{EXECUTABLE_NAME}}` 替换为你的可执行文件名：

```bash
# 例如：可执行文件名是 my-app
echo "     cd /workspace/build && ./my-app"
```

#### 2.3 确认构建系统

如果你的项目使用 **qmake** 而不是 CMake，需要修改 `docker-compose.yml` 中的 `command`：

```yaml
command: >
  /bin/bash -c "mkdir -p build &&
                cd build &&
                qmake .. &&
                make -j\$(nproc)"
```

### 步骤 3: 使用

```bash
# 一键构建
./build-docker-linux.sh

# 进入容器
docker-compose run --rm qt-dev bash
```

---

## 📝 占位符说明

| 占位符 | 说明 | 示例 |
|--------|------|------|
| `{{PROJECT_NAME}}` | 项目名称 | `my-qt-app` |
| `{{EXECUTABLE_NAME}}` | 可执行文件名 | `my-app` |

---

## 🔧 自定义配置

### 修改 Qt 版本

编辑 `Dockerfile`，将 Qt6 改为 Qt5：

```dockerfile
# Qt 5 示例
RUN apt-get update && apt-get install -y \
    qt5-default \
    qtbase5-dev \
    qttools5-dev \
    ...
```

### 添加额外的系统依赖

在 `Dockerfile` 中添加：

```dockerfile
RUN apt-get update && apt-get install -y \
    your-package-name \
    ...
```

### 修改构建命令

如果你的项目使用其他构建系统（如 Meson、Bazel），修改 `docker-compose.yml` 中的 `command` 或 `build-docker-linux.sh` 中的构建命令。

---

## ✅ 检查清单

使用模板前，确认：

- [ ] 项目使用 CMake 或 qmake（或其他支持的构建系统）
- [ ] 已安装 Docker Desktop
- [ ] 已复制模板文件并修改占位符
- [ ] 已添加 `build-docker-linux.sh` 的执行权限
- [ ] （macOS）已配置 Docker 文件共享路径（如需要）
- [ ] （macOS + GUI）已安装并配置 XQuartz（如需要）

---

## 📚 详细文档

查看 `Qt-Docker-通用开发指南.md` 获取：

- 完整的环境准备步骤
- 详细的构建和运行说明
- 常见问题解决方案
- 进阶配置选项

---

## 💡 提示

1. **首次使用**：建议先阅读 `Qt-Docker-通用开发指南.md`
2. **快速测试**：使用 `QT_QPA_PLATFORM=offscreen` 可以跳过图形环境配置
3. **清理资源**：定期使用 `docker-compose down -v` 清理未使用的容器和卷

---

**祝开发愉快！** 🎉


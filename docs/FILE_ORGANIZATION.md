# 📁 项目文件组织说明

> 本文档说明项目根目录下的文件组织和整理情况

## 📋 整理总结

### ✅ 已删除的重复文件（8个）

#### Docker 相关文档（7个）
- `README-DOCKER.md` - 已整合到 `docs/DOCKER.md`
- `README-DOCKER-FIX.md` - 已整合到 `docs/DOCKER.md`
- `DOCKER-README.md` - 已整合到 `docs/DOCKER.md`
- `DOCKER-SETUP.md` - 已整合到 `docs/DOCKER.md`
- `DOCKER-打开说明.md` - 已整合到 `docs/DOCKER.md`
- `DOCKER-字体配置说明.md` - 已整合到 `docs/DOCKER.md`
- `配置Docker文件共享-步骤.md` - 已整合到 `docs/DOCKER.md`

#### 脚本文件（1个）
- `run-app.sh` - 功能已整合到 `quick-run.sh`

### ✅ 新创建的统一文档

- `docs/DOCKER.md` - 整合所有 Docker 相关内容的完整指南

---

## 📁 当前文件结构

### 📄 Markdown 文档

| 文件 | 说明 |
|------|------|
| `README.md` | 项目主文档，包含快速开始和基本说明 |
| `docs/DOCKER.md` | Docker 开发环境完整指南（**推荐阅读**） |
| `X11-SETUP.md` | X11/XQuartz 详细配置说明 |
| `Docker-镜像加速配置.md` | Docker 镜像加速器配置 |
| `Qt-Docker-通用开发指南.md` | 通用 Qt 项目 Docker 模板指南 |
| `WINDOWS-构建说明.md` | Windows 平台构建指南 |
| `跨平台兼容性说明.md` | 跨平台开发注意事项 |
| `README-模板使用说明.md` | 模板文件使用说明 |
| `AGENTS.md` | AI Agent 配置说明 |

### 🔧 Shell 脚本

#### 运行脚本
- `quick-run.sh` - **快速运行（推荐）**，自动检测最佳模式
- `run-app-x11.sh` - X11 图形界面模式
- `run-app-offscreen.sh` - Offscreen 模式（无图形界面）

#### 构建脚本
- `build-docker-linux.sh` - Docker Linux 构建脚本
- `docker-build-and-run.sh` - Docker 构建并运行脚本

#### 工具脚本
- `check-docker-setup.sh` - 检查 Docker 配置
- `test-x11-connection.sh` - 测试 X11 连接
- `fix-docker-sharing.sh` - 修复 Docker 文件共享
- `verify-docker-sharing.sh` - 验证 Docker 文件共享
- `fix-build-cache.sh` - 修复构建缓存问题
- `open-docker.sh` - 打开 Docker Desktop

### 🐳 Docker 配置文件

#### Docker Compose 配置
- `docker-compose.yml` - 主配置（默认 offscreen 模式）
- `docker-compose.x11.yml` - X11 图形界面配置
- `docker-compose.offscreen.yml` - Offscreen 模式配置
- `docker-compose.cn.yml` - 中国镜像源配置
- `docker-compose.yml.template` - 通用模板

#### Dockerfile
- `Dockerfile` - 主 Dockerfile
- `Dockerfile.cn` - 使用中国镜像源的 Dockerfile
- `Dockerfile.template` - 通用模板

### 📋 模板文件

所有 `.template` 文件都是通用模板，供其他 Qt 项目使用：
- `Dockerfile.template`
- `docker-compose.yml.template`
- `build-docker-linux.sh.template`

---

## 🎯 使用建议

### 新用户

1. **阅读主文档**：`README.md`
2. **Docker 开发**：`docs/DOCKER.md`
3. **快速运行**：`./quick-run.sh`

### 遇到问题

1. **Docker 相关问题**：查看 `docs/DOCKER.md` 的"常见问题"部分
2. **X11 显示问题**：查看 `X11-SETUP.md`
3. **构建问题**：查看 `docs/build/` 下的文档

### 开发工作流

```bash
# 1. 启动容器
docker-compose up -d

# 2. 运行程序（自动检测模式）
./quick-run.sh

# 3. 或指定模式
./run-app-x11.sh      # X11 图形界面
./run-app-offscreen.sh # Offscreen 模式
```

---

## 📝 文档维护

- 所有 Docker 相关内容统一维护在 `docs/DOCKER.md`
- 新增 Docker 相关文档时，请更新 `docs/DOCKER.md` 而不是创建新文件
- 脚本文件保持单一职责，避免功能重复

---

**最后更新**：2026-01-17


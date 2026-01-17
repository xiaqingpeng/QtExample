# 📁 项目文件组织说明

> 本文档说明项目根目录下的文件组织和整理情况

## 📋 整理总结

### ✅ 已整理的文件

#### Shell 脚本组织

**根目录（常用脚本，保留在根目录便于快速访问）：**
- `quick-run.sh` - **快速运行（推荐）**，自动检测最佳模式
- `run-app-x11.sh` - X11 图形界面模式
- `run-app-offscreen.sh` - Offscreen 模式（无图形界面）

**scripts/docker/（Docker 相关工具脚本）：**
- `docker-build-and-run.sh` - Docker 构建并运行脚本
- `check-docker-setup.sh` - 检查 Docker 配置
- `test-x11-connection.sh` - 测试 X11 连接
- `verify-docker-sharing.sh` - 验证 Docker 文件共享
- `fix-docker-sharing.sh` - 修复 Docker 文件共享
- `fix-build-cache.sh` - 修复构建缓存问题
- `open-docker.sh` - 打开 Docker Desktop

**scripts/build/（构建脚本）：**
- `build-all-platforms.sh` - 跨平台完整构建
- `build-and-push.sh` - 构建并推送到远程仓库
- `test-build.sh` - 本地构建测试
- `package-local.sh` - 本地打包
- `test-qmake-build.sh` - qmake 构建测试

**scripts/release/（发布脚本）：**
- `complete-release.sh` - 交互式发布菜单
- `quick-build-upload.sh` - 快速构建上传
- `upload-release.sh` - 上传发布包
- `recreate-tag.sh` - 重新创建标签
- `create-lite-tag.sh` - 创建轻量标签

**scripts/test/（测试脚本）：**
- `show-build-options.sh` - 显示所有构建选项
- `quick-test.sh` - 快速测试
- `check-pipeline.sh` - 检查 CI/CD 流水线

**scripts/setup/（设置脚本）：**
- `setup-remotes.sh` - 配置远程仓库

**scripts/windows/（Windows 专用脚本）：**
- PowerShell 脚本集合

#### Markdown 文档组织

**根目录（主文档，保留在根目录）：**
- `README.md` - 项目主文档，包含快速开始和基本说明

**docs/（所有文档统一放在 docs 目录）：**
- `DOCKER.md` - Docker 开发环境完整指南（**推荐阅读**）
- `FILE_ORGANIZATION.md` - 本文件，文件组织说明

**docs/docker/（Docker 相关文档）：**
- `X11-SETUP.md` - X11/XQuartz 详细配置说明
- `Docker-镜像加速配置.md` - Docker 镜像加速器配置
- `Qt-Docker-通用开发指南.md` - 通用 Qt 项目 Docker 模板指南

**docs/build/（构建相关文档）：**
- `BUILD_ALL_PLATFORMS_GUIDE.md` - 跨平台构建指南
- `BUILD_AND_PUSH_GUIDE.md` - 构建并推送代码指南
- `WINDOWS-构建说明.md` - Windows 平台构建指南

**docs/release/（发布相关文档）：**
- `CROSS_PLATFORM_RELEASE_README.md` - 跨平台发布指南
- `WINDOWS_RELEASE_GUIDE.md` - Windows 发布指南
- `RELEASE_AUTOMATION_SUMMARY.md` - 发布自动化总结

**docs/development/（开发相关文档）：**
- `windows-compatibility.md` - Windows 兼容性说明
- `CROSS_PLATFORM_UI_OPTIMIZATION.md` - 跨平台 UI 优化指南
- `THEME_FIXES.md` - 主题系统修复
- `THEME_SWITCHING_FIX.md` - 主题切换修复
- `ENTERPRISE_ARCHITECTURE_PLAN.md` - 企业级架构计划
- `ENTERPRISE_TRANSFORMATION_SUMMARY.md` - 企业级转型总结
- `MIGRATION_SUMMARY.md` - 迁移总结
- `PROJECT_MIGRATION_TO_SRC.md` - 项目迁移到 src 目录说明
- `webengine-compatibility-summary.md` - WebEngine 兼容性总结
- `跨平台兼容性说明.md` - 跨平台开发注意事项

**docs/troubleshooting/（故障排除文档）：**
- `GITHUB_ACTIONS_TROUBLESHOOTING.md` - GitHub Actions 故障排除
- `PIPELINE_DETECTION_GUIDE.md` - 流水线检测指南
- `WINDOWS_WEBENGINE_SOLUTION.md` - Windows WebEngine 解决方案

**docs/api/（API 文档）：**
- `USER_PROFILE_API.md` - 用户画像 API

**docs/（其他文档）：**
- `AGENTS.md` - AI Agent 配置说明
- `README-模板使用说明.md` - 模板文件使用说明

#### Docker 配置文件

**根目录（主配置文件，保留在根目录）：**
- `docker-compose.yml` - 主配置（默认 offscreen 模式）
- `docker-compose.x11.yml` - X11 图形界面配置
- `docker-compose.cn.yml` - 中国镜像源配置
- `Dockerfile` - 主 Dockerfile
- `Dockerfile.cn` - 使用中国镜像源的 Dockerfile

**docs/templates/（模板文件）：**
- `docker-compose.yml.template` - Docker Compose 通用模板
- `Dockerfile.template` - Dockerfile 通用模板
- `build-docker-linux.sh.template` - Docker 构建脚本模板

---

## 🎯 使用建议

### 新用户

1. **阅读主文档**：`README.md`
2. **Docker 开发**：`docs/DOCKER.md`
3. **快速运行**：`./quick-run.sh`

### 遇到问题

1. **Docker 相关问题**：查看 `docs/DOCKER.md` 的"常见问题"部分
2. **X11 显示问题**：查看 `docs/docker/X11-SETUP.md`
3. **构建问题**：查看 `docs/build/` 下的文档
4. **故障排除**：查看 `docs/troubleshooting/` 下的文档

### 开发工作流

```bash
# 1. 启动容器
docker-compose up -d

# 2. 运行程序（自动检测模式）
./quick-run.sh

# 3. 或指定模式
./run-app-x11.sh      # X11 图形界面
./run-app-offscreen.sh # Offscreen 模式

# 4. 使用 Docker 工具脚本
./scripts/docker/check-docker-setup.sh  # 检查配置
./scripts/docker/test-x11-connection.sh # 测试连接
```

---

## 📝 文件维护原则

1. **根目录保持简洁**：只保留最常用的脚本和主文档
2. **脚本分类存放**：按功能分类到 `scripts/` 子目录
3. **文档统一管理**：所有文档放在 `docs/` 目录，按主题分类
4. **模板文件归档**：通用模板放在 `docs/templates/` 目录
5. **避免重复**：新增文档时先检查是否已有相关内容，优先更新现有文档

---

## 📊 文件统计

- **根目录脚本**：3 个（快速运行相关）
- **scripts/docker/**：7 个（Docker 工具脚本）
- **scripts/build/**：5 个（构建脚本）
- **scripts/release/**：5 个（发布脚本）
- **scripts/test/**：3 个（测试脚本）
- **scripts/setup/**：1 个（设置脚本）
- **scripts/windows/**：10 个（Windows PowerShell 脚本）

- **根目录文档**：1 个（README.md）
- **docs/**：30+ 个（各类文档）

- **Docker 配置文件**：5 个（根目录）
- **模板文件**：3 个（docs/templates/）

---

**最后更新**：2026-01-17

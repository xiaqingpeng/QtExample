# 构建并推送代码指南

## 概述

本指南介绍如何使用新增的构建并推送功能，在本地构建成功后自动将代码推送到GitHub和GitLab远程仓库。

## 功能特性

### ✨ 主要功能
- 🏗️ 本地构建验证
- 🚀 自动推送到GitHub
- 🚀 自动推送到GitLab
- 🔧 灵活的配置选项
- 🛡️ 安全检查机制

### 🔧 配置选项
- 跳过构建，直接推送
- 选择性推送到特定远程仓库
- 强制推送支持
- 自定义远程仓库名称

## 快速开始

### 1. 初始设置

首次使用前，需要配置远程仓库：

```bash
# 运行远程仓库配置助手
./scripts/setup/setup-remotes.sh
```

这个脚本会帮助你：
- 配置GitHub远程仓库（通常命名为 `origin`）
- 配置GitLab远程仓库（通常命名为 `gitlab`）
- 测试远程仓库连接

### 2. 基本使用

```bash
# 构建并推送到所有远程仓库
./scripts/build/build-and-push.sh
```

这个命令会：
1. 检查Git仓库状态
2. 检查远程仓库配置
3. 执行本地构建测试
4. 推送到GitHub
5. 推送到GitLab
6. 显示推送结果

## 详细用法

### 命令行选项

```bash
./scripts/build/build-and-push.sh [选项]
```

#### 可用选项

| 选项 | 说明 | 示例 |
|------|------|------|
| `--skip-build` | 跳过本地构建，直接推送 | `./scripts/build/build-and-push.sh --skip-build` |
| `--force-push` | 强制推送到远程仓库 | `./scripts/build/build-and-push.sh --force-push` |
| `--github-only` | 只推送到GitHub | `./scripts/build/build-and-push.sh --github-only` |
| `--gitlab-only` | 只推送到GitLab | `./scripts/build/build-and-push.sh --gitlab-only` |
| `--github-remote NAME` | 设置GitHub远程仓库名 | `./scripts/build/build-and-push.sh --github-remote upstream` |
| `--gitlab-remote NAME` | 设置GitLab远程仓库名 | `./scripts/build/build-and-push.sh --gitlab-remote gitlab-origin` |
| `--help` | 显示帮助信息 | `./scripts/build/build-and-push.sh --help` |

### 使用场景

#### 场景1: 日常开发推送
```bash
# 修改代码后，构建验证并推送
./scripts/build/build-and-push.sh
```

#### 场景2: 紧急修复推送
```bash
# 跳过构建，直接推送已验证的代码
./scripts/build/build-and-push.sh --skip-build
```

#### 场景3: 只推送到GitHub
```bash
# 只推送到GitHub，不推送到GitLab
./scripts/build/build-and-push.sh --github-only
```

#### 场景4: 强制推送
```bash
# 强制推送（谨慎使用）
./scripts/build/build-and-push.sh --force-push
```

#### 场景5: 自定义远程仓库
```bash
# 使用自定义的远程仓库名称
./scripts/build/build-and-push.sh --github-remote upstream --gitlab-remote gitlab-mirror
```

## 集成到现有工作流

### 跨平台构建集成

`build-all-platforms.sh` 脚本已经集成了推送功能：

```bash
# 构建所有平台并推送代码
./scripts/build/build-all-platforms.sh

# 构建所有平台但跳过推送
./scripts/build/build-all-platforms.sh --no-push

# 自定义远程仓库
./scripts/build/build-all-platforms.sh --github-remote upstream --gitlab-remote gitlab-origin
```

### 本地构建测试集成

`test-build.sh` 脚本在构建成功后会提示使用推送功能：

```bash
./scripts/build/test-build.sh
# 构建成功后会显示：
# 5. 构建并推送代码: ./scripts/build/build-and-push.sh
```

## 安全检查

### 自动检查项目

1. **Git仓库检查**：确保在Git仓库中运行
2. **未提交更改检查**：检测未提交的更改并提示用户
3. **远程仓库检查**：验证远程仓库配置
4. **构建验证**：确保本地构建成功
5. **分支检查**：获取并显示当前分支信息

### 安全提示

- ⚠️ 使用 `--force-push` 时要特别小心，可能会覆盖远程更改
- 🔒 确保已正确配置Git认证（SSH密钥或Personal Access Token）
- 📋 推送前建议先检查 `git status` 和 `git log`

## 故障排除

### 常见问题

#### Q1: 推送失败，提示认证错误
**解决方案：**
```bash
# 检查Git配置
git config --list | grep user

# 对于HTTPS，配置Personal Access Token
git config --global credential.helper store

# 对于SSH，检查SSH密钥
ssh -T git@github.com
ssh -T git@gitlab.com
```

#### Q2: GitLab远程仓库不存在
**解决方案：**
```bash
# 运行远程仓库配置助手
./scripts/setup/setup-remotes.sh

# 或手动添加GitLab远程仓库
git remote add gitlab https://gitlab.com/username/repository.git
```

#### Q3: 构建失败
**解决方案：**
```bash
# 检查构建环境
./scripts/build/test-build.sh

# 跳过构建，直接推送
./scripts/build/build-and-push.sh --skip-build
```

#### Q4: 分支冲突
**解决方案：**
```bash
# 先拉取远程更改
git pull origin main

# 解决冲突后再推送
./scripts/build/build-and-push.sh
```

### 调试技巧

1. **查看详细输出**：脚本会显示每个步骤的详细信息
2. **检查远程仓库**：使用 `git remote -v` 查看配置
3. **测试连接**：使用 `git ls-remote origin` 测试连接
4. **查看日志**：使用 `git log --oneline -5` 查看最近提交

## 最佳实践

### 开发工作流建议

1. **开发阶段**：
   ```bash
   # 修改代码
   # 本地测试
   ./scripts/build/test-build.sh
   
   # 提交更改
   git add .
   git commit -m "feat: add new feature"
   
   # 构建并推送
   ./scripts/build/build-and-push.sh
   ```

2. **发布阶段**：
   ```bash
   # 跨平台构建并推送
   ./scripts/build/build-all-platforms.sh --version "v1.2.0"
   ```

3. **紧急修复**：
   ```bash
   # 快速修复并推送
   git add .
   git commit -m "fix: critical bug fix"
   ./scripts/build/build-and-push.sh --skip-build
   ```

### 团队协作建议

1. **统一远程仓库命名**：
   - GitHub: `origin`
   - GitLab: `gitlab`

2. **分支策略**：
   - 主分支：`main` 或 `master`
   - 开发分支：`develop`
   - 功能分支：`feature/xxx`

3. **提交规范**：
   - 使用语义化提交信息
   - 推送前确保代码质量

## 相关文档

- [跨平台构建指南](BUILD_ALL_PLATFORMS_GUIDE.md)
- [GitHub SSH配置](https://docs.github.com/en/authentication/connecting-to-github-with-ssh)
- [GitLab SSH配置](https://docs.gitlab.com/ee/user/ssh.html)

---

**文档版本**: v1.0  
**最后更新**: 2026年1月3日  
**维护者**: 开发团队
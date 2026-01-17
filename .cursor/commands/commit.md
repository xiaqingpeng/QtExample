# commit

生成符合 Conventional Commits 规范的 Git Commit Message，支持{language_type1}{language_type2}动态切换。

## 语言类型配置

**可配置变量**（用户可以根据需要修改这些值）：
- `{language_type1}` = "中文"
- `{language_type2}` = "英文"

**使用说明**：以下所有出现 `{language_type1}` 和 `{language_type2}` 的地方，将自动替换为上述配置的值。

## 核心原则

1. **基于实际代码变更**：commit 信息必须基于**实际代码变更内容**生成，而不是仅根据文件名猜测。
2. **关注具体变更**：专注于具体的代码改动，避免泛泛而谈。描述要具体到修改了什么功能、添加了什么代码、修复了什么问题。
3. **隐私保护**：
   - 不要输出姓名、邮箱地址或其他个人身份信息（除非在代码变更中明确存在且是必要的）
   - 不要输出 bug ID、issue ID、ticket ID 或其他唯一标识符（除非在代码变更中明确存在且是必要的）
4. **简洁准确**：commit 信息要简洁明了，准确反映代码的实际变更内容，不超过 60 字符。

## 使用方法

在 Chat 中输入以下命令：
- `/commit {language_type1}` - 生成{language_type1} commit 信息
- `/commit {language_type2}` - 生成{language_type2} commit 信息
- `/commit` - 默认生成{language_type1} commit 信息

## 执行流程

1. **检查暂存区和工作区**：执行 `git status --short` 检查暂存区和工作区状态，如果没有暂存文件，自动暂存工作区的变更
2. **获取代码变更**：执行 `git --no-pager diff --cached` 获取**完整代码变更内容**（使用 --no-pager 避免分页器阻塞）
3. **分析代码变更**：阅读实际代码，**专注于具体的改动**，理解：
   - 具体修改了什么功能？（例如：添加了用户登录验证逻辑）
   - 具体添加了什么新代码？（例如：新增了 token 过期检查函数）
   - 具体修复了什么问题？（例如：修复了登录后页面跳转失败，原因是缺少 await）
   - 避免使用"更新代码"、"修改文件"等模糊描述
4. **生成 commit message**：基于实际变更内容，按 Conventional Commits 规范生成，subject 应包含主要变更要点
5. **显示结果并询问确认**：展示变更内容摘要和跨平台提交命令
6. **执行提交**：用户确认后执行 git commit

## Commit 类型

- `feat`: 新功能
- `fix`: 修复 bug
- `docs`: 文档变更
- `style`: 代码格式
- `refactor`: 代码重构
- `perf`: 性能优化
- `test`: 测试相关
- `chore`: 构建/工具

## 输出格式

---

**📋 暂存区变更信息：**

变更文件：
- <文件1> (状态)

**主要变更内容：**
<基于 git diff 的实际代码变更，简要说明改了什么>

变更摘要：新增 X 个 | 修改 Y 个 | 删除 Z 个

---

**📝 生成的 Commit Message：**

```
<type>(<scope>): <subject>

- <主要变更1>
- <主要变更2>
- <主要变更3>
```

---

**💻 提交命令（跨平台）：**

**Windows PowerShell（避免乱码）:**
```powershell
$msg = "<commit message>"; $bytes = [System.Text.Encoding]::UTF8.GetBytes($msg); [System.IO.File]::WriteAllBytes("$env:TEMP\commit-msg.txt", $bytes); git commit -F "$env:TEMP\commit-msg.txt"
```

**Mac/Linux/Git Bash:**
```bash
git commit -m "<commit message>"
```

---

**❓ 确认提交：**

是否需要执行此 commit？回复'好的'、'可以'、'yes'等即可执行。

---

## 确认词

用户回复以下任一词语即执行提交：
- {language_type1}：好的、可以、行、没问题、执行、提交、确认
- {language_type2}：yes、y、ok、okay、sure、go、execute、commit

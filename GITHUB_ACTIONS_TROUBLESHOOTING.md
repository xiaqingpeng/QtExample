# GitHub Actions 故障排除指南

## 常见构建失败问题

### 1. 文件路径不匹配错误

**错误信息:**
```
cp: cannot stat 'build/example': No such file or directory
```

**原因分析:**
- CMakeLists.txt中定义的可执行文件名与GitHub Actions脚本中期望的文件名不匹配
- 构建过程没有成功生成可执行文件

**解决方案:**
1. 检查CMakeLists.txt中的`OUTPUT_NAME`设置
2. 确保构建步骤成功完成
3. 在打包步骤前添加调试输出，列出build目录内容

**修复示例:**
```yaml
# 在打包前添加调试信息
- name: Debug build directory
  run: |
    echo "=== Build directory contents ==="
    ls -la build/
```

### 2. CMake配置失败

**错误信息:**
```
CMake Error: Could not find CMAKE_CXX_COMPILER
```

**原因分析:**
- 编译器未正确安装或配置
- Qt路径配置错误
- 依赖库缺失

**解决方案:**
1. 确保安装了正确的编译器
2. 验证Qt安装路径
3. 检查系统依赖库

### 3. Qt模块缺失

**错误信息:**
```
Could not find a package configuration file provided by "Qt6WebEngineWidgets"
```

**解决方案:**
在install-qt-action中添加所需模块：
```yaml
- name: Install Qt
  uses: jurplel/install-qt-action@v3
  with:
    modules: 'qtwebengine qtwebchannel qtpositioning'
```

### 5. Release资产上传失败

**错误信息:**
```
❌ Too many retries. Aborting...
Error: Too many retries.
```

**原因分析:**
- GitHub API网络连接问题
- 文件过大（超过100MB限制）
- GitHub服务临时不可用
- API速率限制

**解决方案:**
1. 检查文件大小是否超过GitHub限制
2. 使用备用上传脚本重试
3. 检查GitHub服务状态
4. 手动上传到Release页面

**修复示例:**
```bash
# 使用备用上传脚本
./upload-release.sh example-v1.0.0-macOS-qt6.6.1.zip

# 或手动上传
# 访问: https://github.com/用户名/仓库名/releases/tag/v1.0.0
```

### 6. 权限问题

**错误信息:**
```
Permission denied
```

**解决方案:**
确保文件有执行权限：
```yaml
- name: Set permissions
  run: chmod +x build/example
```

## 调试步骤

### 1. 本地构建测试

在推送标签前，先在本地测试构建：

```bash
# 使用提供的测试脚本
./test-build.sh

# 或手动测试
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release --parallel
```

### 2. 检查构建输出

在GitHub Actions中添加调试步骤：

```yaml
- name: List build contents
  run: |
    echo "=== Build directory ==="
    ls -la build/
    echo "=== Executable files ==="
    find build/ -type f -executable
```

### 3. 验证文件名

确保CMakeLists.txt和GitHub Actions脚本中的文件名一致：

**CMakeLists.txt:**
```cmake
set_target_properties(QTExample PROPERTIES
    OUTPUT_NAME "example"  # 确保这里的名称正确
)
```

**GitHub Actions:**
```yaml
cp build/example "$ARCHIVE_NAME/"  # 确保这里的名称匹配
```

## 平台特定问题

### macOS

**问题:** 应用包名称不匹配
```bash
# 错误
cp -R build/example.app "$ARCHIVE_NAME/"

# 正确 - 检查实际的.app包名称
if [ -d "build/example.app" ]; then
    cp -R "build/example.app" "$ARCHIVE_NAME/"
fi
```

### Linux

**问题:** 共享库缺失
```bash
# 添加共享库复制
cp build/*.so* "$ARCHIVE_NAME/" 2>/dev/null || true
```

### Windows

**问题:** DLL文件缺失
```powershell
# 确保复制所有必要的DLL
Get-ChildItem "build/*.dll" -ErrorAction SilentlyContinue | Copy-Item -Destination "$env:ARCHIVE_NAME/"
```

## 工作流优化建议

### 1. 添加构建缓存

```yaml
- name: Cache Qt
  uses: actions/cache@v3
  with:
    path: ${{ runner.temp }}/Qt
    key: ${{ runner.os }}-Qt-${{ matrix.qt_version }}
```

### 2. 并行构建

```yaml
- name: Build project
  run: |
    cmake --build build --config Release --parallel $(nproc)
```

### 3. 错误处理

```yaml
- name: Build with error handling
  run: |
    if ! cmake --build build --config Release --parallel; then
      echo "Build failed, showing error logs:"
      cat build/CMakeFiles/CMakeError.log || true
      exit 1
    fi
```

## 监控和通知

### 1. 使用流水线检测脚本

```bash
# 检测流水线状态
./check-pipeline.sh

# 重建标签并检测
./recreate-tag.sh
```

### 2. 添加Slack通知

```yaml
- name: Notify on failure
  if: failure()
  uses: 8398a7/action-slack@v3
  with:
    status: failure
    text: "Build failed for ${{ github.ref_name }}"
```

## 常用调试命令

### 本地调试
```bash
# 检查CMake配置
cmake -B build -DCMAKE_BUILD_TYPE=Release --debug-output

# 详细构建输出
cmake --build build --config Release --verbose

# 检查依赖
ldd build/example  # Linux
otool -L build/example.app/Contents/MacOS/example  # macOS
```

### GitHub Actions调试
```yaml
# 启用调试模式
- name: Setup tmate session
  uses: mxschmitt/action-tmate@v3
  if: failure()
```

## 备用解决方案

### 手动上传Release资产

如果GitHub Actions上传失败，可以使用备用脚本：

```bash
# 构建完成后，手动上传
./upload-release.sh build/example-v1.0.0-macOS-qt6.6.1.zip v1.0.0

# 或者让脚本自动检测标签
./upload-release.sh build/example-v1.0.0-Linux-qt6.6.1.tar.gz
```

### 完全手动流程

1. 访问GitHub仓库的Releases页面
2. 找到对应的标签Release
3. 点击"Edit release"
4. 拖拽文件到"Attach binaries"区域
5. 保存Release

## 预防措施

1. **定期测试:** 在本地环境中定期运行完整的构建和打包流程
2. **版本锁定:** 固定Qt版本和其他依赖的版本号
3. **文档更新:** 保持构建文档和脚本同步更新
4. **自动化测试:** 添加单元测试和集成测试

## 获取帮助

如果问题仍然存在：

1. 检查GitHub Actions日志的完整输出
2. 在本地复现问题
3. 查看Qt和CMake的官方文档
4. 搜索相关的GitHub Issues和Stack Overflow

记住：大多数构建问题都是由于路径不匹配、依赖缺失或配置错误造成的。系统性地检查这些方面通常能快速定位问题。
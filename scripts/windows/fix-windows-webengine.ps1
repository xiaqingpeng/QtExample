# ========================================
# Windows WebEngine 依赖修复脚本
# 专门解决 Qt6WebEngineWidgets.dll 缺失问题
# ========================================

param(
    [string]$PackageDir = "",
    [switch]$Verbose = $false
)

# 颜色定义函数
function Write-ColorOutput($ForegroundColor) {
    $fc = $host.UI.RawUI.ForegroundColor
    $host.UI.RawUI.ForegroundColor = $ForegroundColor
    if ($args) {
        Write-Output $args
    }
    $host.UI.RawUI.ForegroundColor = $fc
}

function Write-Red($text) { Write-ColorOutput Red $text }
function Write-Green($text) { Write-ColorOutput Green $text }
function Write-Yellow($text) { Write-ColorOutput Yellow $text }
function Write-Blue($text) { Write-ColorOutput Blue $text }
function Write-Cyan($text) { Write-ColorOutput Cyan $text }

Write-Blue "========================================"
Write-Blue "    Windows WebEngine 依赖修复"
Write-Blue "========================================"

# 如果没有指定包目录，自动查找
if (-not $PackageDir) {
    $possibleDirs = Get-ChildItem -Directory -Name "example-v*-Windows-*" | Sort-Object -Descending
    if ($possibleDirs) {
        $PackageDir = $possibleDirs[0]
        Write-Cyan "自动找到包目录: $PackageDir"
    } else {
        Write-Red "✗ 未找到Windows包目录"
        Write-Cyan "请指定包目录: .\fix-windows-webengine.ps1 -PackageDir 'example-v1.0.1-Windows-qt6.6.1'"
        exit 1
    }
}

if (-not (Test-Path $PackageDir)) {
    Write-Red "✗ 包目录不存在: $PackageDir"
    exit 1
}

Write-Yellow "检查包目录: $PackageDir"

# 检查可执行文件
$exePath = Join-Path $PackageDir "example.exe"
if (-not (Test-Path $exePath)) {
    Write-Red "✗ 未找到可执行文件: $exePath"
    exit 1
}

Write-Green "✓ 找到可执行文件: $exePath"

# 定义所需的WebEngine文件
$requiredWebEngineFiles = @(
    "Qt6WebEngineWidgets.dll",
    "Qt6WebEngineCore.dll",
    "Qt6WebEngine.dll",
    "Qt6WebEngineQuick.dll",
    "Qt6WebChannel.dll",
    "Qt6Positioning.dll",
    "Qt6Quick.dll",
    "Qt6Qml.dll",
    "Qt6QmlModels.dll"
)

# 定义所需的核心Qt文件
$requiredCoreFiles = @(
    "Qt6Core.dll",
    "Qt6Gui.dll", 
    "Qt6Widgets.dll",
    "Qt6Network.dll",
    "Qt6PrintSupport.dll"
)

# 定义所需的运行时文件
$requiredRuntimeFiles = @(
    "msvcp140.dll",
    "vcruntime140.dll",
    "vcruntime140_1.dll"
)

# 合并所有必需文件
$allRequiredFiles = $requiredCoreFiles + $requiredWebEngineFiles + $requiredRuntimeFiles

# 检查缺失的文件
Write-Yellow "[1/4] 检查Qt依赖文件..."
$missingFiles = @()
$existingFiles = @()

Write-Cyan "检查核心Qt文件:"
foreach ($file in $requiredCoreFiles) {
    $filePath = Join-Path $PackageDir $file
    if (Test-Path $filePath) {
        $existingFiles += $file
        if ($Verbose) { Write-Green "  ✓ $file" }
    } else {
        $missingFiles += $file
        Write-Red "  ✗ $file"
    }
}

Write-Cyan "检查WebEngine文件:"
foreach ($file in $requiredWebEngineFiles) {
    $filePath = Join-Path $PackageDir $file
    if (Test-Path $filePath) {
        $existingFiles += $file
        if ($Verbose) { Write-Green "  ✓ $file" }
    } else {
        $missingFiles += $file
        Write-Red "  ✗ $file"
    }
}

Write-Cyan "检查运行时文件:"
foreach ($file in $requiredRuntimeFiles) {
    $filePath = Join-Path $PackageDir $file
    if (Test-Path $filePath) {
        $existingFiles += $file
        if ($Verbose) { Write-Green "  ✓ $file" }
    } else {
        $missingFiles += $file
        Write-Yellow "  ⚠️  $file (可能由系统提供)"
    }
}

Write-Cyan "现有文件: $($existingFiles.Count)/$($allRequiredFiles.Count)"
Write-Cyan "缺失文件: $($missingFiles.Count)"

if ($missingFiles.Count -eq 0) {
    Write-Green "✓ 所有必需文件都存在"
    exit 0
}

# 查找Qt安装目录
Write-Yellow "[2/4] 查找Qt安装目录..."
$qtBinDirs = @()

# 从环境变量查找
$envPaths = @($env:QTDIR, $env:Qt6_DIR, $env:QT_ROOT)
foreach ($envPath in $envPaths) {
    if ($envPath) {
        $binPath = Join-Path $envPath "bin"
        if (Test-Path $binPath) {
            $qtBinDirs += $binPath
        }
    }
}

# 从常见安装路径查找
$commonPaths = @(
    "C:\Qt\6.6.1\msvc2019_64\bin",
    "C:\Qt\6.6.1\msvc2022_64\bin",
    "C:\Qt\6.7.0\msvc2019_64\bin",
    "C:\Qt\6.7.0\msvc2022_64\bin",
    "C:\Qt\6.8.0\msvc2019_64\bin",
    "C:\Qt\6.8.0\msvc2022_64\bin"
)

foreach ($path in $commonPaths) {
    if (Test-Path $path) {
        $qtBinDirs += $path
    }
}

# 从PATH查找windeployqt
$windeployqt = Get-Command windeployqt.exe -ErrorAction SilentlyContinue
if ($windeployqt) {
    $qtBinDir = Split-Path $windeployqt.Source -Parent
    $qtBinDirs += $qtBinDir
}

# 去重
$qtBinDirs = $qtBinDirs | Sort-Object -Unique

if ($qtBinDirs.Count -eq 0) {
    Write-Red "✗ 未找到Qt安装目录"
    Write-Cyan "请确保:"
    Write-Cyan "  1. Qt已正确安装"
    Write-Cyan "  2. 设置环境变量 QTDIR 或 Qt6_DIR"
    Write-Cyan "  3. 或将Qt的bin目录添加到PATH"
    exit 1
}

Write-Green "✓ 找到Qt安装目录:"
foreach ($dir in $qtBinDirs) {
    Write-Cyan "  $dir"
}

# 复制缺失的文件
Write-Yellow "[3/4] 复制缺失的WebEngine文件..."
$copiedFiles = @()
$stillMissingFiles = @()

foreach ($file in $missingFiles) {
    $copied = $false
    
    foreach ($qtBinDir in $qtBinDirs) {
        $sourcePath = Join-Path $qtBinDir $file
        if (Test-Path $sourcePath) {
            try {
                $destPath = Join-Path $PackageDir $file
                Copy-Item $sourcePath $destPath -Force
                $copiedFiles += $file
                Write-Green "  ✓ 复制: $file"
                $copied = $true
                break
            }
            catch {
                Write-Red "  ✗ 复制失败: $file - $($_.Exception.Message)"
            }
        }
    }
    
    if (-not $copied) {
        $stillMissingFiles += $file
        Write-Red "  ✗ 未找到: $file"
    }
}

# 运行windeployqt进行完整部署
Write-Yellow "[4/4] 运行windeployqt进行完整部署..."
$windeployqtPath = $null

foreach ($qtBinDir in $qtBinDirs) {
    $deployPath = Join-Path $qtBinDir "windeployqt.exe"
    if (Test-Path $deployPath) {
        $windeployqtPath = $deployPath
        break
    }
}

if ($windeployqtPath) {
    Write-Cyan "使用windeployqt: $windeployqtPath"
    
    try {
        $deployArgs = @(
            $exePath,
            "--dir", $PackageDir,
            "--compiler-runtime",
            "--webenginewidgets",
            "--force"
        )
        
        if ($Verbose) {
            $deployArgs += "--verbose", "2"
        }
        
        Write-Cyan "执行: windeployqt $($deployArgs -join ' ')"
        & $windeployqtPath @deployArgs
        
        if ($LASTEXITCODE -eq 0) {
            Write-Green "✓ windeployqt执行成功"
        } else {
            Write-Yellow "⚠️  windeployqt执行完成，但可能有警告 (退出代码: $LASTEXITCODE)"
        }
    }
    catch {
        Write-Red "✗ windeployqt执行失败: $($_.Exception.Message)"
    }
} else {
    Write-Yellow "⚠️  未找到windeployqt.exe"
}

# 最终验证
Write-Yellow "最终验证..."
$finalMissingFiles = @()

foreach ($file in $allRequiredFiles) {
    $filePath = Join-Path $PackageDir $file
    if (-not (Test-Path $filePath)) {
        $finalMissingFiles += $file
    }
}

# 显示结果
Write-Blue "========================================"
if ($finalMissingFiles.Count -eq 0) {
    Write-Green "✓ Qt依赖修复完成！"
    Write-Green "✓ 所有必需文件都已存在"
} else {
    Write-Yellow "⚠️  部分文件仍然缺失:"
    foreach ($file in $finalMissingFiles) {
        Write-Red "  ✗ $file"
    }
}
Write-Blue "========================================"

Write-Cyan "包目录内容:"
Get-ChildItem $PackageDir -Name "*.dll" | Sort-Object | ForEach-Object {
    Write-Output "  $_"
}

Write-Cyan ""
Write-Cyan "下一步操作:"
Write-Output "  1. 测试应用程序: .\$PackageDir\example.exe"
Write-Output "  2. 如果仍有问题，请检查Qt安装是否完整"
Write-Output "  3. 确保安装了Qt WebEngine模块"
Write-Output "  4. 运行包测试: .\test-windows-package.ps1 -PackageDir '$PackageDir'"

if ($finalMissingFiles.Count -gt 0) {
    Write-Cyan ""
    Write-Yellow "如果问题持续存在，请尝试:"
    Write-Output "  1. 重新安装Qt，确保包含WebEngine组件"
    Write-Output "  2. 使用Qt Maintenance Tool添加WebEngine模块"
    Write-Output "  3. 检查Qt版本是否与项目兼容"
    Write-Output "  4. 安装Visual C++ Redistributable"
    Write-Output "  5. 手动复制缺失的DLL文件"
    exit 1
} else {
    exit 0
}
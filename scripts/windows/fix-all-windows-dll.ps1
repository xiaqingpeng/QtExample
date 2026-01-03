# ========================================
# Windows DLL一键修复脚本
# 解决所有常见的Qt DLL缺失问题
# ========================================

param(
    [string]$PackageDir = "",
    [switch]$Force = $false,
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
Write-Blue "    Windows DLL一键修复"
Write-Blue "========================================"

# 如果没有指定包目录，自动查找
if (-not $PackageDir) {
    $possibleDirs = Get-ChildItem -Directory -Name "example-v*-Windows-*" | Sort-Object -Descending
    if ($possibleDirs) {
        $PackageDir = $possibleDirs[0]
        Write-Cyan "自动找到包目录: $PackageDir"
    } else {
        Write-Red "✗ 未找到Windows包目录"
        Write-Cyan "请指定包目录: .\fix-all-windows-dll.ps1 -PackageDir 'example-v1.0.1-Windows-qt6.6.1'"
        exit 1
    }
}

if (-not (Test-Path $PackageDir)) {
    Write-Red "✗ 包目录不存在: $PackageDir"
    exit 1
}

Write-Yellow "修复包目录: $PackageDir"

# 定义所有需要的DLL文件
$allRequiredDlls = @(
    # Qt核心库
    "Qt6Core.dll",
    "Qt6Gui.dll", 
    "Qt6Widgets.dll",
    "Qt6Network.dll",
    "Qt6PrintSupport.dll",
    
    # WebEngine模块
    "Qt6WebEngineWidgets.dll",
    "Qt6WebEngineCore.dll",
    "Qt6WebEngine.dll",
    "Qt6WebChannel.dll",
    "Qt6Positioning.dll",
    "Qt6Quick.dll",
    "Qt6Qml.dll",
    "Qt6QmlModels.dll",
    "Qt6QmlWorkerScript.dll",
    "Qt6QmlMeta.dll",
    
    # 运行时库
    "msvcp140.dll",
    "vcruntime140.dll",
    "vcruntime140_1.dll"
)

# 步骤1: 诊断当前状态
Write-Yellow "[1/5] 诊断当前状态..."
$missingDlls = @()
$existingDlls = @()

foreach ($dll in $allRequiredDlls) {
    $dllPath = Join-Path $PackageDir $dll
    if (Test-Path $dllPath) {
        $existingDlls += $dll
        if ($Verbose) { Write-Green "  ✓ $dll" }
    } else {
        $missingDlls += $dll
        Write-Red "  ✗ $dll"
    }
}

Write-Cyan "状态: $($existingDlls.Count)/$($allRequiredDlls.Count) 个DLL存在"
Write-Cyan "需要修复: $($missingDlls.Count) 个DLL"

if ($missingDlls.Count -eq 0 -and -not $Force) {
    Write-Green "✓ 所有DLL都已存在，无需修复"
    exit 0
}

# 步骤2: 查找Qt安装
Write-Yellow "[2/5] 查找Qt安装..."
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

# 从常见路径查找
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

# 从PATH查找
$windeployqt = Get-Command windeployqt.exe -ErrorAction SilentlyContinue
if ($windeployqt) {
    $qtBinDir = Split-Path $windeployqt.Source -Parent
    $qtBinDirs += $qtBinDir
}

# 去重
$qtBinDirs = $qtBinDirs | Sort-Object -Unique

if ($qtBinDirs.Count -eq 0) {
    Write-Red "✗ 未找到Qt安装目录"
    Write-Yellow "请确保Qt已正确安装并设置环境变量"
    exit 1
}

Write-Green "✓ 找到Qt安装目录:"
foreach ($dir in $qtBinDirs) {
    Write-Cyan "  $dir"
}

# 步骤3: 手动复制缺失的DLL
Write-Yellow "[3/5] 手动复制缺失的DLL..."
$copiedDlls = @()
$stillMissingDlls = @()

foreach ($dll in $missingDlls) {
    $copied = $false
    
    foreach ($qtBinDir in $qtBinDirs) {
        $sourcePath = Join-Path $qtBinDir $dll
        if (Test-Path $sourcePath) {
            try {
                $destPath = Join-Path $PackageDir $dll
                Copy-Item $sourcePath $destPath -Force
                $copiedDlls += $dll
                Write-Green "  ✓ 复制: $dll"
                $copied = $true
                break
            }
            catch {
                Write-Red "  ✗ 复制失败: $dll - $($_.Exception.Message)"
            }
        }
    }
    
    if (-not $copied) {
        $stillMissingDlls += $dll
        Write-Yellow "  ⚠️  未找到: $dll"
    }
}

# 步骤4: 运行windeployqt
Write-Yellow "[4/5] 运行windeployqt..."
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
    
    $exePath = Join-Path $PackageDir "example.exe"
    
    try {
        $deployArgs = @(
            $exePath,
            "--dir", $PackageDir,
            "--compiler-runtime",
            "--webenginewidgets",
            "--force",
            "--no-translations"
        )
        
        if ($Verbose) {
            $deployArgs += "--verbose", "2"
        }
        
        Write-Cyan "执行: windeployqt $($deployArgs -join ' ')"
        & $windeployqtPath @deployArgs
        
        if ($LASTEXITCODE -eq 0) {
            Write-Green "✓ windeployqt执行成功"
        } else {
            Write-Yellow "⚠️  windeployqt执行完成，但可能有警告"
        }
    }
    catch {
        Write-Red "✗ windeployqt执行失败: $($_.Exception.Message)"
    }
} else {
    Write-Yellow "⚠️  未找到windeployqt.exe"
}

# 步骤5: 最终验证和清理
Write-Yellow "[5/5] 最终验证..."
$finalMissingDlls = @()
$finalExistingDlls = @()

foreach ($dll in $allRequiredDlls) {
    $dllPath = Join-Path $PackageDir $dll
    if (Test-Path $dllPath) {
        $finalExistingDlls += $dll
    } else {
        $finalMissingDlls += $dll
    }
}

# 清理重复的DLL（保留最新的）
Write-Cyan "清理重复文件..."
$duplicatePatterns = @("api-ms-win-*.dll", "ucrtbase*.dll")
foreach ($pattern in $duplicatePatterns) {
    $duplicates = Get-ChildItem $PackageDir -Name $pattern -ErrorAction SilentlyContinue
    if ($duplicates.Count -gt 1) {
        $duplicates | Sort-Object LastWriteTime | Select-Object -SkipLast 1 | ForEach-Object {
            $filePath = Join-Path $PackageDir $_
            Remove-Item $filePath -Force -ErrorAction SilentlyContinue
            if ($Verbose) { Write-Cyan "  清理重复文件: $_" }
        }
    }
}

# 显示最终结果
Write-Blue "========================================"
Write-Blue "    修复结果"
Write-Blue "========================================"

Write-Cyan "修复统计:"
Write-Output "  原有DLL: $($existingDlls.Count)"
Write-Output "  复制DLL: $($copiedDlls.Count)"
Write-Output "  最终存在: $($finalExistingDlls.Count)/$($allRequiredDlls.Count)"
Write-Output "  仍然缺失: $($finalMissingDlls.Count)"

if ($finalMissingDlls.Count -eq 0) {
    Write-Green "✓ 所有DLL修复完成！"
    Write-Green "✓ 应用程序应该可以正常运行"
} else {
    Write-Yellow "⚠️  部分DLL仍然缺失:"
    foreach ($dll in $finalMissingDlls) {
        Write-Red "  ✗ $dll"
    }
}

# 显示包内容
Write-Cyan ""
Write-Cyan "包目录DLL文件:"
$dllFiles = Get-ChildItem $PackageDir -Name "*.dll" | Sort-Object
$dllFiles | ForEach-Object { Write-Output "  $_" }

Write-Cyan ""
Write-Cyan "包大小:"
$packageSize = (Get-ChildItem $PackageDir -Recurse -File | Measure-Object -Property Length -Sum).Sum
$packageSizeMB = [math]::Round($packageSize / 1MB, 2)
Write-Output "  $packageSizeMB MB"

# 提供下一步建议
Write-Cyan ""
Write-Cyan "下一步操作:"
Write-Output "  1. 测试应用程序: .\$PackageDir\example.exe"
Write-Output "  2. 运行包测试: .\test-windows-package.ps1 -PackageDir '$PackageDir'"
Write-Output "  3. 如果仍有问题，运行诊断: .\diagnose-windows-dll.ps1 -PackageDir '$PackageDir'"

if ($finalMissingDlls.Count -gt 0) {
    Write-Cyan ""
    Write-Yellow "如果应用程序仍无法运行:"
    Write-Output "  1. 安装Visual C++ Redistributable 2015-2022"
    Write-Output "  2. 重新安装Qt，确保包含所有必需模块"
    Write-Output "  3. 检查Windows系统更新"
    Write-Output "  4. 在不同的Windows系统上测试"
    exit 1
} else {
    Write-Green ""
    Write-Green "🎉 修复完成！应用程序应该可以正常运行了。"
    exit 0
}
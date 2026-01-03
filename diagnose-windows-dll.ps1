# ========================================
# Windows DLL问题诊断和修复脚本
# 专门解决Qt应用程序DLL缺失问题
# ========================================

param(
    [string]$PackageDir = "",
    [switch]$AutoFix = $false,
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
Write-Blue "    Windows DLL问题诊断和修复"
Write-Blue "========================================"

# 如果没有指定包目录，自动查找
if (-not $PackageDir) {
    $possibleDirs = Get-ChildItem -Directory -Name "example-v*-Windows-*" | Sort-Object -Descending
    if ($possibleDirs) {
        $PackageDir = $possibleDirs[0]
        Write-Cyan "自动找到包目录: $PackageDir"
    } else {
        Write-Red "✗ 未找到Windows包目录"
        Write-Cyan "请指定包目录: .\diagnose-windows-dll.ps1 -PackageDir 'example-v1.0.1-Windows-qt6.6.1'"
        exit 1
    }
}

if (-not (Test-Path $PackageDir)) {
    Write-Red "✗ 包目录不存在: $PackageDir"
    exit 1
}

Write-Yellow "诊断包目录: $PackageDir"

# 定义常见的DLL错误和对应的文件
$dllErrors = @{
    "Qt6WebChannel.dll" = @{
        "description" = "Qt WebChannel模块 - 用于Web通信"
        "category" = "WebEngine"
        "critical" = $true
    }
    "Qt6WebEngineWidgets.dll" = @{
        "description" = "Qt WebEngine Widgets模块 - Web引擎UI组件"
        "category" = "WebEngine"
        "critical" = $true
    }
    "Qt6Widgets.dll" = @{
        "description" = "Qt Widgets模块 - 基础UI组件"
        "category" = "Core"
        "critical" = $true
    }
    "Qt6Core.dll" = @{
        "description" = "Qt Core模块 - 核心功能"
        "category" = "Core"
        "critical" = $true
    }
    "Qt6Gui.dll" = @{
        "description" = "Qt GUI模块 - 图形界面"
        "category" = "Core"
        "critical" = $true
    }
    "Qt6Network.dll" = @{
        "description" = "Qt Network模块 - 网络功能"
        "category" = "Core"
        "critical" = $true
    }
    "Qt6WebEngineCore.dll" = @{
        "description" = "Qt WebEngine Core模块 - Web引擎核心"
        "category" = "WebEngine"
        "critical" = $true
    }
    "Qt6WebEngine.dll" = @{
        "description" = "Qt WebEngine模块 - Web引擎基础"
        "category" = "WebEngine"
        "critical" = $true
    }
    "Qt6Positioning.dll" = @{
        "description" = "Qt Positioning模块 - 地理位置"
        "category" = "WebEngine"
        "critical" = $false
    }
    "Qt6Quick.dll" = @{
        "description" = "Qt Quick模块 - 快速UI"
        "category" = "WebEngine"
        "critical" = $false
    }
    "Qt6Qml.dll" = @{
        "description" = "Qt QML模块 - QML引擎"
        "category" = "WebEngine"
        "critical" = $false
    }
    "msvcp140.dll" = @{
        "description" = "Microsoft Visual C++ 2015-2022 Redistributable"
        "category" = "Runtime"
        "critical" = $true
    }
    "vcruntime140.dll" = @{
        "description" = "Visual C++ Runtime"
        "category" = "Runtime"
        "critical" = $true
    }
    "vcruntime140_1.dll" = @{
        "description" = "Visual C++ Runtime (additional)"
        "category" = "Runtime"
        "critical" = $false
    }
}

# 检查可执行文件
$exePath = Join-Path $PackageDir "example.exe"
if (-not (Test-Path $exePath)) {
    Write-Red "✗ 未找到可执行文件: $exePath"
    exit 1
}

Write-Green "✓ 找到可执行文件: $exePath"

# 诊断DLL文件
Write-Yellow "[1/4] 诊断DLL文件..."
$missingDlls = @()
$existingDlls = @()
$criticalMissing = @()

foreach ($dll in $dllErrors.Keys) {
    $dllPath = Join-Path $PackageDir $dll
    $info = $dllErrors[$dll]
    
    if (Test-Path $dllPath) {
        $existingDlls += $dll
        if ($Verbose) {
            Write-Green "  ✓ $dll - $($info.description)"
        }
    } else {
        $missingDlls += $dll
        if ($info.critical) {
            $criticalMissing += $dll
            Write-Red "  ✗ $dll - $($info.description) [关键]"
        } else {
            Write-Yellow "  ⚠️  $dll - $($info.description) [可选]"
        }
    }
}

# 按类别统计
$categories = @{}
foreach ($dll in $dllErrors.Keys) {
    $category = $dllErrors[$dll].category
    if (-not $categories.ContainsKey($category)) {
        $categories[$category] = @{ "total" = 0; "missing" = 0; "existing" = 0 }
    }
    $categories[$category].total++
    
    $dllPath = Join-Path $PackageDir $dll
    if (Test-Path $dllPath) {
        $categories[$category].existing++
    } else {
        $categories[$category].missing++
    }
}

Write-Yellow "[2/4] 类别统计..."
foreach ($category in $categories.Keys) {
    $stats = $categories[$category]
    $status = if ($stats.missing -eq 0) { "✓" } elseif ($stats.missing -eq $stats.total) { "✗" } else { "⚠️" }
    Write-Cyan "  $status $category : $($stats.existing)/$($stats.total) 个文件存在"
}

# 检查Qt安装
Write-Yellow "[3/4] 检查Qt安装..."
$qtPaths = @(
    "${env:QTDIR}\bin",
    "${env:Qt6_DIR}\bin",
    "${env:QT_ROOT}\bin",
    "C:\Qt\6.6.1\msvc2019_64\bin",
    "C:\Qt\6.6.1\msvc2022_64\bin",
    "C:\Qt\6.7.0\msvc2019_64\bin",
    "C:\Qt\6.7.0\msvc2022_64\bin"
)

$qtFound = $false
$qtBinPath = $null

foreach ($path in $qtPaths) {
    if ($path -and (Test-Path $path)) {
        $qtBinPath = $path
        $qtFound = $true
        Write-Green "  ✓ 找到Qt安装: $path"
        break
    }
}

if (-not $qtFound) {
    Write-Red "  ✗ 未找到Qt安装"
    Write-Yellow "    请确保Qt已正确安装并设置环境变量"
}

# 检查windeployqt
$windeployqt = $null
if ($qtBinPath) {
    $windeployqtPath = Join-Path $qtBinPath "windeployqt.exe"
    if (Test-Path $windeployqtPath) {
        $windeployqt = $windeployqtPath
        Write-Green "  ✓ 找到windeployqt: $windeployqtPath"
    } else {
        Write-Red "  ✗ 未找到windeployqt.exe"
    }
}

# 生成诊断报告
Write-Yellow "[4/4] 生成诊断报告..."

Write-Blue "========================================"
Write-Blue "    诊断报告"
Write-Blue "========================================"

Write-Cyan "总体状态:"
Write-Output "  总DLL数量: $($dllErrors.Count)"
Write-Output "  存在DLL: $($existingDlls.Count)"
Write-Output "  缺失DLL: $($missingDlls.Count)"
Write-Output "  关键缺失: $($criticalMissing.Count)"

if ($criticalMissing.Count -eq 0) {
    Write-Green "✓ 所有关键DLL都存在"
    $overallStatus = "良好"
} elseif ($criticalMissing.Count -le 2) {
    Write-Yellow "⚠️  少量关键DLL缺失"
    $overallStatus = "需要修复"
} else {
    Write-Red "✗ 多个关键DLL缺失"
    $overallStatus = "严重问题"
}

Write-Cyan "整体评估: $overallStatus"

if ($missingDlls.Count -gt 0) {
    Write-Cyan ""
    Write-Cyan "缺失的DLL文件:"
    foreach ($dll in $missingDlls) {
        $info = $dllErrors[$dll]
        $priority = if ($info.critical) { "[关键]" } else { "[可选]" }
        Write-Output "  • $dll - $($info.description) $priority"
    }
}

# 提供修复建议
Write-Cyan ""
Write-Cyan "修复建议:"

if ($criticalMissing.Count -gt 0) {
    Write-Output "  1. 运行Qt依赖修复: .\fix-windows-webengine.ps1 -PackageDir '$PackageDir'"
    
    if ($windeployqt) {
        Write-Output "  2. 手动运行windeployqt:"
        Write-Output "     $windeployqt --webenginewidgets --compiler-runtime '$exePath'"
    }
    
    Write-Output "  3. 检查Qt安装完整性:"
    Write-Output "     - 确保安装了Qt WebEngine模块"
    Write-Output "     - 使用Qt Maintenance Tool验证安装"
    
    Write-Output "  4. 安装Visual C++ Redistributable:"
    Write-Output "     - 下载并安装Microsoft Visual C++ 2015-2022 Redistributable"
}

# 自动修复选项
if ($AutoFix -and $criticalMissing.Count -gt 0) {
    Write-Yellow ""
    Write-Yellow "执行自动修复..."
    
    if (Test-Path ".\fix-windows-webengine.ps1") {
        & ".\fix-windows-webengine.ps1" -PackageDir $PackageDir
    } else {
        Write-Red "✗ 未找到修复脚本: fix-windows-webengine.ps1"
    }
}

# 返回状态码
if ($criticalMissing.Count -eq 0) {
    Write-Green ""
    Write-Green "✓ 诊断完成，没有发现关键问题"
    exit 0
} else {
    Write-Red ""
    Write-Red "✗ 诊断完成，发现 $($criticalMissing.Count) 个关键问题"
    exit 1
}
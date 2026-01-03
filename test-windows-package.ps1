# ========================================
# Windows包测试脚本
# 验证打包的应用程序是否包含所有必需文件
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
Write-Blue "    Windows包测试"
Write-Blue "========================================"

# 如果没有指定包目录，自动查找
if (-not $PackageDir) {
    $possibleDirs = Get-ChildItem -Directory -Name "example-v*-Windows-*" | Sort-Object -Descending
    if ($possibleDirs) {
        $PackageDir = $possibleDirs[0]
        Write-Cyan "自动找到包目录: $PackageDir"
    } else {
        Write-Red "✗ 未找到Windows包目录"
        Write-Cyan "请指定包目录: .\test-windows-package.ps1 -PackageDir 'example-v1.0.1-Windows-qt6.6.1'"
        exit 1
    }
}

if (-not (Test-Path $PackageDir)) {
    Write-Red "✗ 包目录不存在: $PackageDir"
    exit 1
}

Write-Yellow "测试包目录: $PackageDir"

# 定义必需文件
$requiredFiles = @{
    "核心文件" = @(
        "example.exe"
    )
    "Qt核心库" = @(
        "Qt6Core.dll",
        "Qt6Gui.dll", 
        "Qt6Widgets.dll",
        "Qt6Network.dll",
        "Qt6PrintSupport.dll"
    )
    "WebEngine模块" = @(
        "Qt6WebEngineWidgets.dll",
        "Qt6WebEngineCore.dll",
        "Qt6WebEngine.dll",
        "Qt6WebChannel.dll",
        "Qt6Positioning.dll",
        "Qt6Quick.dll",
        "Qt6Qml.dll",
        "Qt6QmlModels.dll"
    )
    "运行时库" = @(
        "msvcp140.dll",
        "vcruntime140.dll",
        "vcruntime140_1.dll"
    )
}

$requiredDirs = @(
    "platforms",
    "imageformats",
    "styles"
)

# 检查文件
Write-Yellow "[1/3] 检查必需文件..."
$totalFiles = 0
$missingFiles = 0
$existingFiles = 0

foreach ($category in $requiredFiles.Keys) {
    Write-Cyan "检查 $category :"
    
    foreach ($file in $requiredFiles[$category]) {
        $totalFiles++
        $filePath = Join-Path $PackageDir $file
        
        if (Test-Path $filePath) {
            $existingFiles++
            if ($Verbose) {
                $fileInfo = Get-Item $filePath
                $size = [math]::Round($fileInfo.Length / 1KB, 1)
                Write-Green "  ✓ $file ($size KB)"
            } else {
                Write-Green "  ✓ $file"
            }
        } else {
            $missingFiles++
            Write-Red "  ✗ $file"
        }
    }
    Write-Output ""
}

# 检查目录
Write-Yellow "[2/3] 检查必需目录..."
$totalDirs = $requiredDirs.Count
$missingDirs = 0
$existingDirs = 0

foreach ($dir in $requiredDirs) {
    $dirPath = Join-Path $PackageDir $dir
    
    if (Test-Path $dirPath -PathType Container) {
        $existingDirs++
        $fileCount = (Get-ChildItem $dirPath -File -Recurse).Count
        Write-Green "  ✓ $dir ($fileCount 个文件)"
    } else {
        $missingDirs++
        Write-Red "  ✗ $dir"
    }
}

# 检查资源文件
Write-Yellow "[3/3] 检查资源文件..."
$resourceDirs = @("images")
$resourceFiles = 0

foreach ($dir in $resourceDirs) {
    $dirPath = Join-Path $PackageDir $dir
    
    if (Test-Path $dirPath -PathType Container) {
        $files = Get-ChildItem $dirPath -File -Recurse
        $resourceFiles += $files.Count
        Write-Green "  ✓ $dir ($($files.Count) 个文件)"
        
        if ($Verbose) {
            foreach ($file in $files) {
                Write-Cyan "    - $($file.Name)"
            }
        }
    } else {
        Write-Yellow "  ⚠️  $dir (可选)"
    }
}

# 显示统计结果
Write-Blue "========================================"
Write-Blue "    测试结果"
Write-Blue "========================================"

Write-Cyan "文件统计:"
Write-Output "  总文件数: $totalFiles"
Write-Output "  存在文件: $existingFiles"
Write-Output "  缺失文件: $missingFiles"

Write-Cyan "目录统计:"
Write-Output "  总目录数: $totalDirs"
Write-Output "  存在目录: $existingDirs"
Write-Output "  缺失目录: $missingDirs"

Write-Cyan "资源文件: $resourceFiles 个"

# 计算包大小
$packageSize = 0
if (Test-Path $PackageDir) {
    $packageSize = (Get-ChildItem $PackageDir -Recurse -File | Measure-Object -Property Length -Sum).Sum
    $packageSizeMB = [math]::Round($packageSize / 1MB, 2)
    Write-Cyan "包大小: $packageSizeMB MB"
}

# 最终评估
Write-Output ""
if ($missingFiles -eq 0 -and $missingDirs -eq 0) {
    Write-Green "✓ 包测试通过！所有必需文件和目录都存在"
    $exitCode = 0
} elseif ($missingFiles -gt 0) {
    Write-Red "✗ 包测试失败！缺少 $missingFiles 个必需文件"
    Write-Yellow "建议运行: .\fix-windows-webengine.ps1 -PackageDir '$PackageDir'"
    $exitCode = 1
} else {
    Write-Yellow "⚠️  包测试部分通过，缺少 $missingDirs 个目录"
    $exitCode = 2
}

# 提供下一步建议
Write-Cyan ""
Write-Cyan "下一步操作:"
if ($exitCode -eq 0) {
    Write-Output "  1. 测试应用程序: .\$PackageDir\example.exe"
    Write-Output "  2. 创建ZIP包: Compress-Archive -Path '$PackageDir' -DestinationPath '$PackageDir.zip'"
    Write-Output "  3. 上传到GitHub Release"
} else {
    Write-Output "  1. 修复缺失文件: .\fix-windows-webengine.ps1 -PackageDir '$PackageDir'"
    Write-Output "  2. 重新运行测试: .\test-windows-package.ps1 -PackageDir '$PackageDir'"
    Write-Output "  3. 如果问题持续，检查Qt安装是否完整"
}

exit $exitCode
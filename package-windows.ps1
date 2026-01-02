# ========================================
# Windows 应用程序打包脚本
# ========================================

param(
    [string]$Version = "v1.0.1",
    [string]$QtVersion = "qt6.6.1"
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
Write-Blue "    Windows 应用程序打包"
Write-Blue "========================================"
Write-Yellow "版本: $Version"
Write-Yellow "Qt版本: $QtVersion"

$ArchiveName = "example-$Version-Windows-$QtVersion"

# 检查构建目录
if (-not (Test-Path "build")) {
    Write-Red "✗ 错误: build目录不存在"
    Write-Yellow "请先运行构建脚本: .\test-build-windows.ps1"
    exit 1
}

# 查找可执行文件
$exePaths = @(
    "build\Release\example.exe",
    "build\Debug\example.exe",
    "build\example.exe"
)

$exePath = $null
foreach ($path in $exePaths) {
    if (Test-Path $path) {
        $exePath = $path
        break
    }
}

if (-not $exePath) {
    Write-Red "✗ 错误: 未找到可执行文件"
    Write-Yellow "可能的位置:"
    $exePaths | ForEach-Object { Write-Output "  $_" }
    exit 1
}

Write-Green "✓ 找到可执行文件: $exePath"

Write-Yellow "[1/4] 创建打包目录..."
if (Test-Path $ArchiveName) {
    Remove-Item -Recurse -Force $ArchiveName
}
New-Item -ItemType Directory -Path $ArchiveName -Force | Out-Null
Write-Green "✓ 打包目录创建成功"

Write-Yellow "[2/4] 复制应用程序文件..."
try {
    # 复制主可执行文件
    Copy-Item $exePath "$ArchiveName\example.exe"
    Write-Green "✓ 复制可执行文件成功"
    
    # 复制DLL文件
    $buildDir = Split-Path $exePath -Parent
    $dllFiles = Get-ChildItem "$buildDir\*.dll" -ErrorAction SilentlyContinue
    if ($dllFiles) {
        $dllFiles | ForEach-Object {
            Copy-Item $_.FullName "$ArchiveName\"
            Write-Cyan "  复制DLL: $($_.Name)"
        }
        Write-Green "✓ 复制DLL文件成功"
    } else {
        Write-Yellow "⚠️  未找到DLL文件"
    }
    
    # 复制资源文件
    if (Test-Path "images") {
        Copy-Item -Recurse "images" "$ArchiveName\"
        Write-Green "✓ 复制资源文件成功"
    } else {
        Write-Yellow "⚠️  未找到images目录"
    }
    
}
catch {
    Write-Red "✗ 复制文件失败: $($_.Exception.Message)"
    exit 1
}

Write-Yellow "[3/4] 检查Qt依赖..."
try {
    # 尝试使用windeployqt部署Qt依赖
    $windeployqt = $null
    
    # 常见的Qt安装路径
    $qtPaths = @(
        "${env:QTDIR}\bin\windeployqt.exe",
        "${env:Qt6_DIR}\bin\windeployqt.exe",
        "C:\Qt\6.6.1\msvc2019_64\bin\windeployqt.exe",
        "C:\Qt\6.6.1\msvc2022_64\bin\windeployqt.exe"
    )
    
    foreach ($qtPath in $qtPaths) {
        if ($qtPath -and (Test-Path $qtPath)) {
            $windeployqt = $qtPath
            break
        }
    }
    
    if ($windeployqt) {
        Write-Cyan "找到windeployqt: $windeployqt"
        Write-Cyan "正在部署Qt依赖..."
        
        & $windeployqt "$ArchiveName\example.exe" --dir $ArchiveName --compiler-runtime
        
        if ($LASTEXITCODE -eq 0) {
            Write-Green "✓ Qt依赖部署成功"
        } else {
            Write-Yellow "⚠️  Qt依赖部署可能不完整"
        }
    } else {
        Write-Yellow "⚠️  未找到windeployqt，需要手动复制Qt依赖"
        Write-Cyan "提示: 确保Qt的bin目录在PATH中，或设置QTDIR环境变量"
    }
}
catch {
    Write-Yellow "⚠️  Qt依赖部署失败: $($_.Exception.Message)"
}

Write-Yellow "[4/4] 创建ZIP压缩包..."
try {
    $zipPath = "$ArchiveName.zip"
    if (Test-Path $zipPath) {
        Remove-Item $zipPath -Force
    }
    
    # 使用PowerShell 5.0+的Compress-Archive
    if (Get-Command Compress-Archive -ErrorAction SilentlyContinue) {
        Compress-Archive -Path $ArchiveName -DestinationPath $zipPath -CompressionLevel Optimal
        Write-Green "✓ ZIP压缩包创建成功"
    } else {
        Write-Yellow "⚠️  PowerShell版本不支持Compress-Archive"
        Write-Cyan "请手动压缩目录: $ArchiveName"
    }
}
catch {
    Write-Red "✗ 创建ZIP压缩包失败: $($_.Exception.Message)"
    Write-Cyan "请手动压缩目录: $ArchiveName"
}

# 显示打包结果
Write-Blue "========================================"
Write-Green "✓ Windows打包完成！"
Write-Blue "========================================"

Write-Cyan "打包结果:"
if (Test-Path "$ArchiveName.zip") {
    $zipInfo = Get-Item "$ArchiveName.zip"
    Write-Output "  ZIP文件: $($zipInfo.Name)"
    Write-Output "  大小: $([math]::Round($zipInfo.Length / 1MB, 2)) MB"
    Write-Output "  路径: $($zipInfo.FullName)"
} else {
    Write-Output "  目录: $ArchiveName"
}

Write-Cyan "目录内容:"
Get-ChildItem $ArchiveName | Format-Table Name, Length, LastWriteTime

Write-Cyan "下一步操作:"
Write-Output "  1. 测试打包的应用程序: .\$ArchiveName\example.exe"
if (Test-Path "$ArchiveName.zip") {
    Write-Output "  2. 上传到GitHub Release: .\upload-release.ps1 $ArchiveName.zip"
} else {
    Write-Output "  2. 手动压缩并上传到GitHub Release"
}
Write-Output "  3. 分发给最终用户"
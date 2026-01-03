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

Write-Yellow "[3/4] 部署Qt依赖（包含WebEngine）..."
try {
    # 尝试使用windeployqt部署Qt依赖
    $windeployqt = $null
    
    # 常见的Qt安装路径
    $qtPaths = @(
        "${env:QTDIR}\bin\windeployqt.exe",
        "${env:Qt6_DIR}\bin\windeployqt.exe",
        "${env:QT_ROOT}\bin\windeployqt.exe",
        "C:\Qt\6.6.1\msvc2019_64\bin\windeployqt.exe",
        "C:\Qt\6.6.1\msvc2022_64\bin\windeployqt.exe",
        "C:\Qt\6.7.0\msvc2019_64\bin\windeployqt.exe",
        "C:\Qt\6.7.0\msvc2022_64\bin\windeployqt.exe"
    )
    
    # 也尝试从PATH中查找
    $pathWinDeploy = Get-Command windeployqt.exe -ErrorAction SilentlyContinue
    if ($pathWinDeploy) {
        $qtPaths = @($pathWinDeploy.Source) + $qtPaths
    }
    
    foreach ($qtPath in $qtPaths) {
        if ($qtPath -and (Test-Path $qtPath)) {
            $windeployqt = $qtPath
            break
        }
    }
    
    if ($windeployqt) {
        Write-Cyan "找到windeployqt: $windeployqt"
        Write-Cyan "正在部署Qt依赖（包含WebEngine模块）..."
        
        # 使用更完整的windeployqt参数，特别针对WebEngine
        $deployArgs = @(
            "$ArchiveName\example.exe",
            "--dir", $ArchiveName,
            "--compiler-runtime",
            "--webenginewidgets",  # 明确包含WebEngine
            "--force",             # 强制覆盖
            "--verbose", "2"       # 详细输出
        )
        
        Write-Cyan "执行命令: $windeployqt $($deployArgs -join ' ')"
        & $windeployqt @deployArgs
        
        if ($LASTEXITCODE -eq 0) {
            Write-Green "✓ Qt依赖部署成功"
            
            # 验证关键的WebEngine文件是否存在
            $webEngineFiles = @(
                "Qt6WebEngineWidgets.dll",
                "Qt6WebEngineCore.dll", 
                "Qt6WebEngine.dll"
            )
            
            $missingFiles = @()
            foreach ($file in $webEngineFiles) {
                if (-not (Test-Path "$ArchiveName\$file")) {
                    $missingFiles += $file
                }
            }
            
            if ($missingFiles.Count -gt 0) {
                Write-Yellow "⚠️  缺少WebEngine文件: $($missingFiles -join ', ')"
                Write-Cyan "尝试手动复制WebEngine文件..."
                
                # 尝试从Qt安装目录手动复制
                $qtBinDir = Split-Path $windeployqt -Parent
                foreach ($file in $missingFiles) {
                    $sourcePath = Join-Path $qtBinDir $file
                    if (Test-Path $sourcePath) {
                        Copy-Item $sourcePath "$ArchiveName\"
                        Write-Green "✓ 手动复制: $file"
                    } else {
                        Write-Red "✗ 未找到: $file"
                    }
                }
            } else {
                Write-Green "✓ WebEngine文件验证通过"
            }
            
        } else {
            Write-Yellow "⚠️  Qt依赖部署可能不完整 (退出代码: $LASTEXITCODE)"
        }
    } else {
        Write-Red "✗ 未找到windeployqt工具"
        Write-Cyan "请确保:"
        Write-Cyan "  1. Qt正确安装"
        Write-Cyan "  2. Qt的bin目录在PATH中"
        Write-Cyan "  3. 或设置QTDIR/Qt6_DIR环境变量"
        Write-Cyan ""
        Write-Cyan "手动部署步骤:"
        Write-Cyan "  1. 找到Qt安装目录的bin文件夹"
        Write-Cyan "  2. 运行: windeployqt.exe --webenginewidgets $ArchiveName\example.exe"
    }
}
catch {
    Write-Red "✗ Qt依赖部署失败: $($_.Exception.Message)"
    Write-Cyan "请尝试手动运行: windeployqt.exe --webenginewidgets $ArchiveName\example.exe"
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
Write-Output "  1. 运行WebEngine依赖修复: .\fix-windows-webengine.ps1 -PackageDir '$ArchiveName'"
Write-Output "  2. 测试打包的应用程序: .\$ArchiveName\example.exe"
if (Test-Path "$ArchiveName.zip") {
    Write-Output "  3. 上传到GitHub Release: .\upload-release-windows.ps1 $ArchiveName.zip"
} else {
    Write-Output "  3. 手动压缩并上传到GitHub Release"
}
Write-Output "  4. 分发给最终用户"

# 自动运行WebEngine修复
Write-Yellow ""
Write-Yellow "自动运行WebEngine依赖修复..."
if (Test-Path ".\fix-windows-webengine.ps1") {
    try {
        & ".\fix-windows-webengine.ps1" -PackageDir $ArchiveName
        if ($LASTEXITCODE -eq 0) {
            Write-Green "✓ WebEngine依赖修复完成"
        } else {
            Write-Yellow "⚠️  WebEngine依赖修复可能不完整"
        }
    }
    catch {
        Write-Yellow "⚠️  WebEngine依赖修复失败: $($_.Exception.Message)"
        Write-Cyan "请手动运行: .\fix-windows-webengine.ps1 -PackageDir '$ArchiveName'"
    }
} else {
    Write-Yellow "⚠️  未找到WebEngine修复脚本"
    Write-Cyan "请确保 fix-windows-webengine.ps1 存在于当前目录"
}
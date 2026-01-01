# Windows 本地打包脚本 - 根据 CI/CD 流程打包应用
# 使用方法: .\package-local.ps1 [版本号]

param(
    [string]$Version = "v1.0.0"
)

# 配置参数
$QtVersion = "6.6.1"
$BuildDir = "build-release"
$CMakeBuildType = "Release"
$Platform = "Windows"
$QtArch = "win64_msvc2019_64"

$ArchiveName = "example-${Version}-${Platform}-qt${QtVersion}"

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "开始打包应用" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "平台: $Platform" -ForegroundColor Green
Write-Host "Qt 版本: $QtVersion" -ForegroundColor Green
Write-Host "Qt 架构: $QtArch" -ForegroundColor Green
Write-Host "构建类型: $CMakeBuildType" -ForegroundColor Green
Write-Host "版本: $Version" -ForegroundColor Green
Write-Host "========================================" -ForegroundColor Cyan

# 步骤 1: 清理旧的构建目录
Write-Host "`n[1/6] 清理旧的构建目录..." -ForegroundColor Yellow
if (Test-Path $BuildDir) {
    Remove-Item -Path $BuildDir -Recurse -Force
}

# 步骤 2: 配置 CMake
Write-Host "`n[2/6] 配置 CMake..." -ForegroundColor Yellow
cmake -B $BuildDir -G "Ninja" `
    -DCMAKE_BUILD_TYPE=$CMakeBuildType `
    -DCMAKE_PREFIX_PATH="$env:Qt6_DIR" `
    -DCMAKE_CXX_STANDARD=17

if ($LASTEXITCODE -ne 0) {
    Write-Host "错误: CMake 配置失败" -ForegroundColor Red
    exit 1
}

# 步骤 3: 构建项目
Write-Host "`n[3/6] 构建项目..." -ForegroundColor Yellow
cmake --build $BuildDir --config $CMakeBuildType --parallel

if ($LASTEXITCODE -ne 0) {
    Write-Host "错误: 构建失败" -ForegroundColor Red
    exit 1
}

# 步骤 4: 清理旧的打包目录
Write-Host "`n[4/6] 清理旧的打包目录..." -ForegroundColor Yellow
if (Test-Path $ArchiveName) {
    Remove-Item -Path $ArchiveName -Recurse -Force
}
if (Test-Path "$ArchiveName.zip") {
    Remove-Item -Path "$ArchiveName.zip" -Force
}

# 步骤 5: 打包应用
Write-Host "`n[5/6] 打包应用..." -ForegroundColor Yellow
New-Item -ItemType Directory -Path $ArchiveName -Force | Out-Null

Write-Host "复制 example.exe..." -ForegroundColor Gray
Copy-Item "$BuildDir\example.exe" "$ArchiveName\"

Write-Host "复制 DLL 文件..." -ForegroundColor Gray
Get-ChildItem "$BuildDir\*.dll" -ErrorAction SilentlyContinue | Copy-Item -Destination "$ArchiveName\"

Write-Host "复制 images 文件夹..." -ForegroundColor Gray
Copy-Item "images" "$ArchiveName\" -Recurse -Force

Write-Host "创建 zip 压缩包..." -ForegroundColor Gray
Compress-Archive -Path $ArchiveName -DestinationPath "$ArchiveName.zip" -Force

# 步骤 6: 完成
Write-Host "`n[6/6] 打包完成！" -ForegroundColor Green
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "打包产物:" -ForegroundColor Green
Write-Host "  - $ArchiveName.zip" -ForegroundColor White
Get-Item "$ArchiveName.zip" | Select-Object Name, @{Name="Size";Expression={[math]::Round($_.Length/1MB, 2)}}, LastWriteTime | Format-Table -AutoSize
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "" -ForegroundColor White
Write-Host "提示:" -ForegroundColor Gray
Write-Host "  - 使用不同的版本号: .\package-local.ps1 v1.0.1" -ForegroundColor Gray
Write-Host "  - 清理构建产物: Remove-Item -Recurse -Force $BuildDir, $ArchiveName*" -ForegroundColor Gray
Write-Host "========================================" -ForegroundColor Cyan
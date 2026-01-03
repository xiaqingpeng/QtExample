# ========================================
# Windows 本地构建测试脚本
# ========================================

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
Write-Blue "    Windows 本地构建测试"
Write-Blue "========================================"

# 检查是否在正确的目录
if (-not (Test-Path "CMakeLists.txt")) {
    Write-Red "✗ 错误: 未找到CMakeLists.txt文件"
    Write-Yellow "请在项目根目录运行此脚本"
    exit 1
}

# 检查必需工具
$requiredTools = @(
    @{Name="cmake"; Description="CMake构建系统"},
    @{Name="git"; Description="Git版本控制"}
)

Write-Cyan "检查必需工具..."
$allToolsAvailable = $true

foreach ($tool in $requiredTools) {
    try {
        $null = Get-Command $tool.Name -ErrorAction Stop
        Write-Green "✓ $($tool.Name) ($($tool.Description))"
    }
    catch {
        Write-Red "✗ $($tool.Name) ($($tool.Description)) - 未找到"
        $allToolsAvailable = $false
    }
}

if (-not $allToolsAvailable) {
    Write-Red "✗ 部分必需工具缺失，请安装后重试"
    exit 1
}

# 检查Visual Studio构建工具
Write-Cyan "检查Visual Studio构建环境..."
try {
    $vsWhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
    if (Test-Path $vsWhere) {
        $vsInstallations = & $vsWhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
        if ($vsInstallations) {
            Write-Green "✓ 找到Visual Studio构建工具"
        } else {
            Write-Yellow "⚠️  未找到Visual Studio构建工具，可能需要手动配置"
        }
    } else {
        Write-Yellow "⚠️  未找到vswhere.exe，无法自动检测Visual Studio"
    }
}
catch {
    Write-Yellow "⚠️  Visual Studio检测失败，构建可能需要手动配置环境"
}

Write-Yellow "[1/4] 清理构建目录..."
if (Test-Path "build") {
    Remove-Item -Recurse -Force "build"
    Write-Green "✓ 构建目录已清理"
} else {
    Write-Cyan "构建目录不存在，跳过清理"
}

Write-Yellow "[2/4] 创建构建目录..."
New-Item -ItemType Directory -Path "build" -Force | Out-Null
Set-Location "build"

Write-Yellow "[3/4] 配置CMake..."
try {
    # 尝试使用Visual Studio生成器
    $generators = @(
        "Visual Studio 17 2022",
        "Visual Studio 16 2019", 
        "Visual Studio 15 2017",
        "Ninja"
    )
    
    $configSuccess = $false
    foreach ($generator in $generators) {
        Write-Cyan "尝试生成器: $generator"
        try {
            if ($generator -eq "Ninja") {
                cmake .. -G $generator -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_STANDARD=17
            } else {
                cmake .. -G $generator -A x64 -DCMAKE_CXX_STANDARD=17
            }
            
            if ($LASTEXITCODE -eq 0) {
                Write-Green "✓ CMake配置成功 (使用 $generator)"
                $configSuccess = $true
                break
            }
        }
        catch {
            Write-Yellow "⚠️  生成器 $generator 失败，尝试下一个..."
        }
    }
    
    if (-not $configSuccess) {
        Write-Red "✗ 所有CMake生成器都失败了"
        Set-Location ".."
        exit 1
    }
}
catch {
    Write-Red "✗ CMake配置失败: $($_.Exception.Message)"
    Set-Location ".."
    exit 1
}

Write-Yellow "[4/4] 构建项目..."
try {
    cmake --build . --config Release --parallel
    
    if ($LASTEXITCODE -eq 0) {
        Write-Green "✓ 项目构建成功"
    } else {
        Write-Red "✗ 项目构建失败"
        Set-Location ".."
        exit 1
    }
}
catch {
    Write-Red "✗ 项目构建失败: $($_.Exception.Message)"
    Set-Location ".."
    exit 1
}

# 检查构建结果
Write-Cyan "构建结果检查:"
Write-Cyan "构建目录内容:"
Get-ChildItem -Force | Format-Table Name, Length, LastWriteTime

# 检查可执行文件
$exePaths = @(
    "Release\example.exe",
    "Debug\example.exe", 
    "example.exe"
)

$foundExe = $false
foreach ($exePath in $exePaths) {
    if (Test-Path $exePath) {
        Write-Green "✓ 找到Windows可执行文件: $exePath"
        Write-Cyan "文件信息:"
        Get-Item $exePath | Format-List Name, Length, LastWriteTime, VersionInfo
        $foundExe = $true
        break
    }
}

if (-not $foundExe) {
    Write-Red "✗ 未找到Windows可执行文件"
    Write-Yellow "请检查构建配置和输出目录"
}

# 检查DLL文件
Write-Cyan "DLL文件检查:"
$dllFiles = Get-ChildItem -Recurse -Filter "*.dll" -ErrorAction SilentlyContinue
if ($dllFiles) {
    Write-Green "✓ 找到DLL文件:"
    $dllFiles | ForEach-Object { Write-Output "  $($_.FullName)" }
} else {
    Write-Yellow "⚠️  未找到DLL文件"
}

Set-Location ".."

Write-Blue "========================================"
Write-Green "✓ Windows构建测试完成！"
Write-Blue "========================================"

# 提供下一步建议
Write-Cyan "下一步操作建议:"
Write-Output "  1. 测试应用程序运行: .\build\Release\example.exe 或 .\build\example.exe"
Write-Output "  2. 如果构建成功，可以创建发布包"
Write-Output "  3. 使用打包脚本创建Windows发布包"
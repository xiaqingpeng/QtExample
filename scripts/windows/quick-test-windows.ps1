# ========================================
# Windows 快速脚本测试
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
Write-Blue "    Windows 快速脚本测试"
Write-Blue "========================================"

# 要测试的脚本列表
$scripts = @(
    "test-build-windows.ps1",
    "package-windows.ps1", 
    "upload-release-windows.ps1",
    "complete-release-windows.ps1"
)

Write-Cyan "检查脚本文件..."

$allGood = $true
foreach ($script in $scripts) {
    if (Test-Path $script) {
        Write-Green "  ✓ $script (存在)"
    } else {
        Write-Red "  ✗ $script (不存在)"
        $allGood = $false
    }
}

Write-Output ""

if ($allGood) {
    Write-Green "✓ 所有脚本文件检查通过"
} else {
    Write-Red "✗ 部分脚本文件缺失"
    exit 1
}

# 检查PowerShell语法
Write-Cyan "检查脚本语法..."

foreach ($script in $scripts) {
    if (Test-Path $script) {
        try {
            $null = [System.Management.Automation.PSParser]::Tokenize((Get-Content $script -Raw), [ref]$null)
            Write-Green "  ✓ $script (语法正确)"
        }
        catch {
            Write-Red "  ✗ $script (语法错误)"
            $allGood = $false
        }
    }
}

Write-Output ""

if ($allGood) {
    Write-Green "✓ 所有脚本语法检查通过"
} else {
    Write-Red "✗ 部分脚本存在语法错误"
    exit 1
}

# 检查依赖工具
Write-Cyan "检查依赖工具..."

$tools = @(
    @{Name="git"; Description="Git版本控制"; Required=$true},
    @{Name="cmake"; Description="CMake构建系统"; Required=$true}
)

$optionalTools = @(
    @{Name="gh"; Description="GitHub CLI (推荐)"; Required=$false}
)

foreach ($tool in $tools) {
    try {
        $null = Get-Command $tool.Name -ErrorAction Stop
        Write-Green "  ✓ $($tool.Name) ($($tool.Description))"
    }
    catch {
        Write-Red "  ✗ $($tool.Name) ($($tool.Description)) - 必需"
        $allGood = $false
    }
}

foreach ($tool in $optionalTools) {
    try {
        $null = Get-Command $tool.Name -ErrorAction Stop
        Write-Green "  ✓ $($tool.Name) ($($tool.Description))"
    }
    catch {
        Write-Yellow "  ⚠️  $($tool.Name) ($($tool.Description)) - 可选"
    }
}

Write-Output ""

# 检查Visual Studio构建工具
Write-Cyan "检查Visual Studio构建环境..."
try {
    $vsWhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
    if (Test-Path $vsWhere) {
        $vsInstallations = & $vsWhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
        if ($vsInstallations) {
            Write-Green "  ✓ 找到Visual Studio构建工具"
        } else {
            Write-Yellow "  ⚠️  未找到Visual Studio构建工具"
        }
    } else {
        Write-Yellow "  ⚠️  未找到vswhere.exe"
    }
}
catch {
    Write-Yellow "  ⚠️  Visual Studio检测失败"
}

# 检查Git仓库状态
Write-Cyan "检查Git仓库状态..."

try {
    git rev-parse --git-dir | Out-Null
    if ($LASTEXITCODE -eq 0) {
        Write-Green "  ✓ 当前目录是Git仓库"
        
        # 检查远程仓库
        try {
            $remoteUrl = git remote get-url origin 2>$null
            if ($LASTEXITCODE -eq 0) {
                Write-Green "  ✓ 远程仓库已配置: $remoteUrl"
                
                if ($remoteUrl -match "github\.com") {
                    Write-Green "  ✓ 这是一个GitHub仓库"
                } else {
                    Write-Yellow "  ⚠️  不是GitHub仓库，部分功能可能不可用"
                }
            } else {
                Write-Yellow "  ⚠️  未配置远程仓库"
            }
        }
        catch {
            Write-Yellow "  ⚠️  无法获取远程仓库信息"
        }
        
        # 检查当前分支
        try {
            $currentBranch = git branch --show-current 2>$null
            if ($LASTEXITCODE -eq 0) {
                Write-Cyan "  当前分支: $currentBranch"
            }
        }
        catch {
            Write-Yellow "  ⚠️  无法获取当前分支信息"
        }
        
        # 检查是否有未提交的更改
        try {
            git diff --quiet 2>$null
            $diffExit = $LASTEXITCODE
            git diff --staged --quiet 2>$null
            $stagedExit = $LASTEXITCODE
            
            if ($diffExit -eq 0 -and $stagedExit -eq 0) {
                Write-Green "  ✓ 工作目录干净"
            } else {
                Write-Yellow "  ⚠️  有未提交的更改"
            }
        }
        catch {
            Write-Yellow "  ⚠️  无法检查工作目录状态"
        }
    } else {
        Write-Red "  ✗ 当前目录不是Git仓库"
        $allGood = $false
    }
}
catch {
    Write-Red "  ✗ Git检查失败"
    $allGood = $false
}

Write-Output ""

# 检查项目文件
Write-Cyan "检查项目文件..."

$projectFiles = @(
    @{Name="CMakeLists.txt"; Description="CMake配置文件"},
    @{Name="main.cpp"; Description="主程序文件"},
    @{Name="mainuiwindow.h"; Description="主窗口头文件"},
    @{Name="mainuiwindow.cpp"; Description="主窗口实现文件"}
)

foreach ($file in $projectFiles) {
    if (Test-Path $file.Name) {
        Write-Green "  ✓ $($file.Name) ($($file.Description))"
    } else {
        Write-Yellow "  ⚠️  $($file.Name) ($($file.Description)) - 未找到"
    }
}

# 检查GitHub Actions配置
if (Test-Path ".github\workflows\release.yml") {
    Write-Green "  ✓ GitHub Actions配置文件存在"
} else {
    Write-Yellow "  ⚠️  GitHub Actions配置文件不存在"
}

Write-Output ""
Write-Blue "========================================"

if ($allGood) {
    Write-Green "✓ 所有检查通过！环境已准备就绪"
    Write-Output ""
    Write-Cyan "可以使用的命令:"
    Write-Output "  .\complete-release-windows.ps1 - 完整发布流程"
    Write-Output "  .\test-build-windows.ps1 - 本地构建测试"
    Write-Output "  .\package-windows.ps1 - 打包应用程序"
    Write-Output "  .\upload-release-windows.ps1 - 上传到GitHub Release"
} else {
    Write-Red "✗ 部分检查失败，请解决上述问题"
    exit 1
}

Write-Blue "========================================"
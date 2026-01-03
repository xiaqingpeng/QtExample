# ========================================
# Windows 完整发布流程脚本
# ========================================

param(
    [string]$Version = "v1.0.1",
    [string]$TagName = "v1.0.1-lite"
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
Write-Blue "    Windows 完整发布流程"
Write-Blue "========================================"

# 显示当前配置
Write-Cyan "配置信息:"
Write-Output "  版本: $Version"
Write-Output "  标签名称: $TagName"
Write-Output ""

# 步骤选择菜单
Write-Yellow "请选择要执行的步骤:"
Write-Cyan "  1. 本地构建测试 ${YELLOW}(单平台快速验证)${NC}"
Write-Cyan "  2. 打包应用程序 ${YELLOW}(创建Windows发布包)${NC}"
Write-Cyan "  3. WebEngine依赖修复 ${YELLOW}(修复WebEngine DLL)${NC}"
Write-Cyan "  4. 包完整性测试 ${YELLOW}(检查所有必需文件)${NC}"
Write-Cyan "  5. DLL问题诊断 ${YELLOW}(诊断DLL缺失问题)${NC}"
Write-Cyan "  6. 一键DLL修复 ${YELLOW}(修复所有DLL问题)${NC}"
Write-Cyan "  7. 上传到GitHub Release ${YELLOW}(发布到GitHub)${NC}"
Write-Cyan "  8. 完整流程 (1→2→6→4→7) ${YELLOW}(完整发布流程)${NC}"
Write-Cyan "  0. 退出"
Write-Output ""
Write-Blue "💡 推荐选择:${NC}"
Write-Green "  • 选项 6${NC} - 一键修复所有DLL问题（推荐）"
Write-Green "  • 选项 5${NC} - 诊断具体的DLL问题"
Write-Green "  • 选项 8${NC} - 完整发布流程"
Write-Output ""

$choice = Read-Host "请输入选择 [1-8,0]"

switch ($choice) {
    "1" {
        Write-Yellow "执行本地构建测试..."
        if (Test-Path ".\test-build-windows.ps1") {
            & ".\test-build-windows.ps1"
        } else {
            Write-Red "✗ 错误: test-build-windows.ps1 不存在"
            exit 1
        }
    }
    "2" {
        Write-Yellow "执行应用程序打包..."
        if (Test-Path ".\package-windows.ps1") {
            & ".\package-windows.ps1" -Version $Version
        } else {
            Write-Red "✗ 错误: package-windows.ps1 不存在"
            exit 1
        }
    }
    "3" {
        Write-Yellow "执行WebEngine依赖修复..."
        if (Test-Path ".\fix-windows-webengine.ps1") {
            & ".\fix-windows-webengine.ps1"
        } else {
            Write-Red "✗ 错误: fix-windows-webengine.ps1 不存在"
            exit 1
        }
    }
    "4" {
        Write-Yellow "执行包完整性测试..."
        if (Test-Path ".\test-windows-package.ps1") {
            & ".\test-windows-package.ps1"
        } else {
            Write-Red "✗ 错误: test-windows-package.ps1 不存在"
            exit 1
        }
    }
    "5" {
        Write-Yellow "执行DLL问题诊断..."
        if (Test-Path ".\diagnose-windows-dll.ps1") {
            & ".\diagnose-windows-dll.ps1"
        } else {
            Write-Red "✗ 错误: diagnose-windows-dll.ps1 不存在"
            exit 1
        }
    }
    "6" {
        Write-Yellow "执行一键DLL修复..."
        if (Test-Path ".\fix-all-windows-dll.ps1") {
            & ".\fix-all-windows-dll.ps1"
        } else {
            Write-Red "✗ 错误: fix-all-windows-dll.ps1 不存在"
            exit 1
        }
    }
    "7" {
        Write-Yellow "上传到GitHub Release..."
        Write-Output ""
        Write-Cyan "可用的打包文件:"
        
        # 查找可能的打包文件
        $buildFiles = @()
        $archiveFiles = Get-ChildItem -Filter "example-$Version-Windows-*.zip" -ErrorAction SilentlyContinue
        
        if ($archiveFiles) {
            Write-Cyan "已打包的文件:"
            for ($i = 0; $i -lt $archiveFiles.Count; $i++) {
                Write-Output "  $($i + 1). $($archiveFiles[$i].Name)"
                $buildFiles += $archiveFiles[$i].FullName
            }
        }
        
        if ($buildFiles.Count -eq 0) {
            Write-Yellow "⚠️  未找到可上传的文件"
            Write-Yellow "请先运行打包步骤"
            exit 1
        }
        
        Write-Output ""
        $fileChoice = Read-Host "请输入文件编号"
        
        if ($fileChoice -match "^\d+$" -and [int]$fileChoice -ge 1 -and [int]$fileChoice -le $buildFiles.Count) {
            $selectedFile = $buildFiles[[int]$fileChoice - 1]
            Write-Cyan "选择的文件: $selectedFile"
            
            if (Test-Path ".\upload-release-windows.ps1") {
                & ".\upload-release-windows.ps1" -AssetFile $selectedFile -TagName $TagName
            } else {
                Write-Red "✗ 错误: upload-release-windows.ps1 不存在"
                exit 1
            }
        } else {
            Write-Red "✗ 无效的选择"
            exit 1
        }
    }
    "8" {
        Write-Yellow "执行完整发布流程..."
        Write-Output ""
        
        # 步骤1: 本地构建测试
        Write-Blue "[1/5] 本地构建测试"
        if (Test-Path ".\test-build-windows.ps1") {
            & ".\test-build-windows.ps1"
            if ($LASTEXITCODE -ne 0) {
                Write-Red "✗ 本地构建测试失败"
                exit 1
            }
            Write-Green "✓ 本地构建测试成功"
        } else {
            Write-Red "✗ 错误: test-build-windows.ps1 不存在"
            exit 1
        }
        
        Write-Output ""
        $continuePack = Read-Host "构建测试成功，是否继续打包? [y/N]"
        if ($continuePack -notmatch "^[Yy]$") {
            Write-Yellow "用户取消操作"
            exit 0
        }
        
        # 步骤2: 打包应用程序
        Write-Output ""
        Write-Blue "[2/5] 打包应用程序"
        if (Test-Path ".\package-windows.ps1") {
            & ".\package-windows.ps1" -Version $Version
            if ($LASTEXITCODE -ne 0) {
                Write-Red "✗ 应用程序打包失败"
                exit 1
            }
            Write-Green "✓ 应用程序打包成功"
        } else {
            Write-Red "✗ 错误: package-windows.ps1 不存在"
            exit 1
        }
        
        # 步骤3: 一键DLL修复
        Write-Output ""
        Write-Blue "[3/5] 一键DLL修复"
        if (Test-Path ".\fix-all-windows-dll.ps1") {
            & ".\fix-all-windows-dll.ps1"
            if ($LASTEXITCODE -ne 0) {
                Write-Yellow "⚠️  DLL修复可能不完整，但继续流程"
            } else {
                Write-Green "✓ DLL修复成功"
            }
        } else {
            Write-Yellow "⚠️  未找到DLL修复脚本，跳过此步骤"
        }
        
        # 步骤4: 包完整性测试
        Write-Output ""
        Write-Blue "[4/5] 包完整性测试"
        if (Test-Path ".\test-windows-package.ps1") {
            & ".\test-windows-package.ps1"
            if ($LASTEXITCODE -ne 0) {
                Write-Yellow "⚠️  包测试发现问题，但继续流程"
                $continueUpload = Read-Host "是否继续上传? [y/N]"
                if ($continueUpload -notmatch "^[Yy]$") {
                    Write-Yellow "用户取消上传"
                    exit 0
                }
            } else {
                Write-Green "✓ 包完整性测试通过"
            }
        } else {
            Write-Yellow "⚠️  未找到包测试脚本，跳过此步骤"
        }
        
        # 步骤5: 上传到GitHub Release
        Write-Output ""
        Write-Blue "[5/5] 上传到GitHub Release"
        
        # 查找刚刚创建的打包文件
        $archiveFiles = Get-ChildItem -Filter "example-$Version-Windows-*.zip" -ErrorAction SilentlyContinue | Sort-Object LastWriteTime -Descending
        
        if ($archiveFiles) {
            $latestFile = $archiveFiles[0].FullName
            Write-Cyan "找到最新的打包文件: $($archiveFiles[0].Name)"
            
            if (Test-Path ".\upload-release-windows.ps1") {
                & ".\upload-release-windows.ps1" -AssetFile $latestFile -TagName $TagName
            } else {
                Write-Red "✗ 错误: upload-release-windows.ps1 不存在"
                exit 1
            }
        } else {
            Write-Red "✗ 未找到打包文件"
            exit 1
        }
    }
    "0" {
        Write-Yellow "退出"
        exit 0
    }
    default {
        Write-Red "✗ 无效的选择"
        exit 1
    }
}

Write-Output ""
Write-Blue "========================================"
Write-Green "✓ 操作完成！"
Write-Blue "========================================"

# 显示有用的链接
try {
    $remoteUrl = git remote get-url origin 2>$null
    if ($LASTEXITCODE -eq 0 -and $remoteUrl -match "github\.com[:/]([^/]+)/([^/]+)(\.git)?$") {
        $githubUser = $matches[1]
        $githubRepo = $matches[2] -replace "\.git$", ""
        
        Write-Output ""
        Write-Cyan "有用的链接:"
        Write-Output "  📦 Release页面: https://github.com/$githubUser/$githubRepo/releases/tag/$TagName"
        Write-Output "  🔄 Actions页面: https://github.com/$githubUser/$githubRepo/actions"
        Write-Output "  📊 仓库主页: https://github.com/$githubUser/$githubRepo"
    }
}
catch {
    # 忽略错误，不显示链接
}
# ========================================
# Windows GitHub Release 资产上传脚本
# ========================================

param(
    [Parameter(Mandatory=$true)]
    [string]$AssetFile,
    [string]$TagName = "v1.0.1-lite",
    [string]$RemoteName = "origin",
    [int]$MaxRetries = 3,
    [int]$RetryDelay = 10
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
Write-Blue "    GitHub Release 资产上传"
Write-Blue "========================================"

Write-Cyan "标签: $TagName"
Write-Cyan "文件: $AssetFile"

# 检查文件是否存在
if (-not (Test-Path $AssetFile)) {
    Write-Red "✗ 错误: 文件不存在: $AssetFile"
    exit 1
}

# 显示文件信息
Write-Cyan "文件信息:"
$fileInfo = Get-Item $AssetFile
Write-Output "  名称: $($fileInfo.Name)"
Write-Output "  大小: $([math]::Round($fileInfo.Length / 1MB, 2)) MB"
Write-Output "  路径: $($fileInfo.FullName)"
Write-Output ""

# 检查是否在Git仓库中
try {
    git rev-parse --git-dir | Out-Null
    if ($LASTEXITCODE -ne 0) {
        Write-Red "✗ 错误: 当前目录不是Git仓库"
        exit 1
    }
}
catch {
    Write-Red "✗ 错误: 当前目录不是Git仓库"
    exit 1
}

# 获取仓库信息
try {
    $remoteUrl = git remote get-url $RemoteName 2>$null
    if ($LASTEXITCODE -ne 0) {
        Write-Red "✗ 错误: 无法获取远程仓库信息"
        exit 1
    }
}
catch {
    Write-Red "✗ 错误: 无法获取远程仓库信息"
    exit 1
}

# 检查是否为GitHub仓库
if ($remoteUrl -notmatch "github\.com") {
    Write-Red "✗ 错误: 不是GitHub仓库"
    exit 1
}

# 提取GitHub仓库信息
if ($remoteUrl -match "github\.com[:/]([^/]+)/([^/]+)(\.git)?$") {
    $githubUser = $matches[1]
    $githubRepo = $matches[2] -replace "\.git$", ""
} else {
    Write-Red "✗ 错误: 无法解析GitHub仓库信息"
    exit 1
}

Write-Cyan "仓库: $githubUser/$githubRepo"
Write-Output ""

# 检查GitHub CLI
try {
    $null = Get-Command gh -ErrorAction Stop
    Write-Green "✓ 找到GitHub CLI"
}
catch {
    Write-Red "✗ 错误: 需要安装GitHub CLI"
    Write-Yellow "安装方法:"
    Write-Output "  1. 访问: https://cli.github.com/"
    Write-Output "  2. 或使用winget: winget install GitHub.cli"
    Write-Output "  3. 或使用Chocolatey: choco install gh"
    exit 1
}

# 检查GitHub CLI认证
try {
    gh auth status 2>$null | Out-Null
    if ($LASTEXITCODE -ne 0) {
        Write-Red "✗ 错误: GitHub CLI未认证"
        Write-Yellow "请运行: gh auth login"
        exit 1
    }
}
catch {
    Write-Red "✗ 错误: GitHub CLI未认证"
    Write-Yellow "请运行: gh auth login"
    exit 1
}

Write-Green "✓ GitHub CLI已准备就绪"

# 检查Release是否存在
Write-Yellow "检查Release状态..."
try {
    gh release view $TagName --repo "$githubUser/$githubRepo" 2>$null | Out-Null
    if ($LASTEXITCODE -eq 0) {
        Write-Green "✓ Release $TagName 已存在"
    } else {
        Write-Yellow "⚠️  Release $TagName 不存在，将创建新的Release"
        
        $createRelease = Read-Host "是否创建新的Release? [y/N]"
        if ($createRelease -match "^[Yy]$") {
            Write-Cyan "创建Release..."
            gh release create $TagName --repo "$githubUser/$githubRepo" --generate-notes
            
            if ($LASTEXITCODE -eq 0) {
                Write-Green "✓ Release创建成功"
            } else {
                Write-Red "✗ Release创建失败"
                exit 1
            }
        } else {
            Write-Yellow "取消操作"
            exit 0
        }
    }
}
catch {
    Write-Red "✗ 检查Release状态失败"
    exit 1
}

# 上传文件（带重试机制）
Write-Yellow "上传文件到Release..."

$uploadSuccess = $false
for ($i = 1; $i -le $MaxRetries; $i++) {
    Write-Cyan "尝试 $i/$MaxRetries..."
    
    try {
        gh release upload $TagName $AssetFile --repo "$githubUser/$githubRepo" --clobber
        
        if ($LASTEXITCODE -eq 0) {
            Write-Green "✓ 文件上传成功！"
            $uploadSuccess = $true
            break
        } else {
            Write-Red "✗ 上传失败"
        }
    }
    catch {
        Write-Red "✗ 上传失败: $($_.Exception.Message)"
    }
    
    if ($i -lt $MaxRetries) {
        Write-Yellow "等待 $RetryDelay 秒后重试..."
        Start-Sleep -Seconds $RetryDelay
    }
}

if (-not $uploadSuccess) {
    Write-Red "✗ 所有重试都失败了"
    Write-Output ""
    Write-Yellow "故障排除建议:"
    Write-Output "  1. 检查网络连接"
    Write-Output "  2. 验证GitHub token权限"
    Write-Output "  3. 检查文件大小限制 (GitHub限制100MB)"
    Write-Output "  4. 查看GitHub状态: https://www.githubstatus.com/"
    Write-Output "  5. 手动上传: 访问 https://github.com/$githubUser/$githubRepo/releases/tag/$TagName"
    exit 1
}

# 验证上传结果
Write-Output ""
Write-Cyan "验证上传结果..."
try {
    $releaseInfo = gh release view $TagName --repo "$githubUser/$githubRepo"
    $fileName = Split-Path $AssetFile -Leaf
    
    if ($releaseInfo -match [regex]::Escape($fileName)) {
        Write-Green "✓ 文件已成功添加到Release"
    } else {
        Write-Yellow "⚠️  无法验证文件是否成功上传"
    }
}
catch {
    Write-Yellow "⚠️  无法验证上传结果"
}

Write-Output ""
Write-Blue "========================================"
Write-Green "✓ 上传完成！"
Write-Blue "========================================"

# 显示Release信息
Write-Output ""
Write-Cyan "Release信息:"
Write-Output "  标签: $TagName"
Write-Output "  仓库: $githubUser/$githubRepo"
Write-Output "  链接: https://github.com/$githubUser/$githubRepo/releases/tag/$TagName"

Write-Output ""
Write-Cyan "上传的文件:"
Write-Output "  文件名: $(Split-Path $AssetFile -Leaf)"
Write-Output "  大小: $([math]::Round($fileInfo.Length / 1MB, 2)) MB"
@echo off
REM Windows平台qmake构建脚本
REM 此脚本会自动禁用WebEngine功能以确保Windows兼容性

echo ========================================
echo Qt项目 Windows 平台构建脚本 (qmake)
echo ========================================
echo.

REM 检查qmake
where qmake >nul 2>nul
if %ERRORLEVEL% neq 0 (
    echo 错误: 未找到qmake命令
    echo 请确保Qt的bin目录已添加到PATH环境变量中
    pause
    exit /b 1
)

echo qmake版本信息:
qmake -v
echo.

REM 创建构建目录
if not exist "build-windows-qmake" (
    mkdir build-windows-qmake
    echo 创建构建目录: build-windows-qmake
)

cd build-windows-qmake

echo 开始配置项目...
echo 注意: Windows平台将自动禁用WebEngine功能

REM 使用qmake配置项目
qmake CONFIG+=release ..\example.pro

if %ERRORLEVEL% neq 0 (
    echo qmake配置失败!
    pause
    exit /b 1
)

echo.
echo 开始构建项目...

REM 构建项目 (使用nmake或mingw32-make)
where nmake >nul 2>nul
if %ERRORLEVEL% equ 0 (
    echo 使用nmake构建...
    nmake
) else (
    where mingw32-make >nul 2>nul
    if %ERRORLEVEL% equ 0 (
        echo 使用mingw32-make构建...
        mingw32-make
    ) else (
        echo 错误: 未找到make工具 (nmake或mingw32-make)
        pause
        exit /b 1
    )
)

if %ERRORLEVEL% neq 0 (
    echo 构建失败!
    pause
    exit /b 1
)

echo.
echo ========================================
echo 构建完成!
echo ========================================
echo 可执行文件位置: build-windows-qmake\release\example.exe
echo.
echo 注意: 由于Windows平台限制，WebEngine相关功能已被禁用
echo 图表功能将显示替代界面
echo.
pause
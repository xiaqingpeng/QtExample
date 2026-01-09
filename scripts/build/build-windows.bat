@echo off
REM Windows平台构建脚本
REM 此脚本会自动禁用WebEngine功能以确保Windows兼容性

echo ========================================
echo Qt项目 Windows 平台构建脚本
echo ========================================
echo.

REM 检查Qt环境
if "%QTDIR%"=="" (
    echo 错误: 未设置QTDIR环境变量
    echo 请确保已安装Qt并设置了正确的环境变量
    pause
    exit /b 1
)

echo Qt安装路径: %QTDIR%
echo.

REM 创建构建目录
if not exist "build-windows" (
    mkdir build-windows
    echo 创建构建目录: build-windows
)

cd build-windows

echo 开始配置项目...
echo 注意: Windows平台将自动禁用WebEngine功能

REM 使用CMake配置项目
cmake -G "Visual Studio 17 2022" -A x64 ^
    -DCMAKE_PREFIX_PATH="%QTDIR%" ^
    -DCMAKE_BUILD_TYPE=Release ^
    ..

if %ERRORLEVEL% neq 0 (
    echo 配置失败!
    pause
    exit /b 1
)

echo.
echo 开始构建项目...

REM 构建项目
cmake --build . --config Release

if %ERRORLEVEL% neq 0 (
    echo 构建失败!
    pause
    exit /b 1
)

echo.
echo ========================================
echo 构建完成!
echo ========================================
echo 可执行文件位置: build-windows\Release\example.exe
echo.
echo 注意: 由于Windows平台限制，WebEngine相关功能已被禁用
echo 图表功能将显示替代界面
echo.
pause
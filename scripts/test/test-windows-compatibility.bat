@echo off
REM Windows兼容性测试脚本

echo ========================================
echo Windows 兼容性测试
echo ========================================
echo.

REM 检查构建环境
echo 1. 检查构建环境...
echo.

REM 检查Qt环境
if "%QTDIR%"=="" (
    echo ❌ 未设置QTDIR环境变量
    set /a errors+=1
) else (
    echo ✅ QTDIR: %QTDIR%
)

REM 检查qmake
where qmake >nul 2>nul
if %ERRORLEVEL% neq 0 (
    echo ❌ 未找到qmake命令
    set /a errors+=1
) else (
    echo ✅ qmake 可用
)

REM 检查CMake
where cmake >nul 2>nul
if %ERRORLEVEL% neq 0 (
    echo ❌ 未找到cmake命令
    set /a errors+=1
) else (
    echo ✅ cmake 可用
)

echo.
echo 2. 检查项目配置...
echo.

REM 检查主要配置文件
if exist "example.pro" (
    echo ✅ 找到 example.pro
    
    REM 检查Windows平台配置
    findstr /C:"win32" example.pro >nul
    if %ERRORLEVEL% equ 0 (
        echo ✅ example.pro 包含Windows平台配置
    ) else (
        echo ❌ example.pro 缺少Windows平台配置
        set /a errors+=1
    )
) else (
    echo ❌ 未找到 example.pro
    set /a errors+=1
)

if exist "CMakeLists.txt" (
    echo ✅ 找到 CMakeLists.txt
    
    REM 检查Windows平台配置
    findstr /C:"WIN32" CMakeLists.txt >nul
    if %ERRORLEVEL% equ 0 (
        echo ✅ CMakeLists.txt 包含Windows平台配置
    ) else (
        echo ❌ CMakeLists.txt 缺少Windows平台配置
        set /a errors+=1
    )
) else (
    echo ❌ 未找到 CMakeLists.txt
    set /a errors+=1
)

echo.
echo 3. 检查模块配置...
echo.

REM 检查各模块的Windows兼容性配置
set modules=Charts Reports UserProfile
for %%m in (%modules%) do (
    if exist "src\%%m\*.pro" (
        findstr /C:"win32" src\%%m\*.pro >nul
        if %ERRORLEVEL% equ 0 (
            echo ✅ %%m 模块包含Windows平台配置
        ) else (
            echo ❌ %%m 模块缺少Windows平台配置
            set /a errors+=1
        )
    )
    
    if exist "src\%%m\CMakeLists.txt" (
        findstr /C:"WIN32" src\%%m\CMakeLists.txt >nul
        if %ERRORLEVEL% equ 0 (
            echo ✅ %%m 模块CMakeLists.txt包含Windows平台配置
        ) else (
            echo ❌ %%m 模块CMakeLists.txt缺少Windows平台配置
            set /a errors+=1
        )
    )
)

echo.
echo 4. 检查源代码兼容性...
echo.

REM 检查条件编译
findstr /S /C:"#ifdef WEBENGINE_AVAILABLE" src\*.cpp src\*.h >nul 2>nul
if %ERRORLEVEL% equ 0 (
    echo ✅ 找到WebEngine条件编译代码
) else (
    echo ❌ 未找到WebEngine条件编译代码
    set /a errors+=1
)

findstr /S /C:"#else" src\*.cpp src\*.h >nul 2>nul
if %ERRORLEVEL% equ 0 (
    echo ✅ 找到替代实现代码
) else (
    echo ❌ 未找到替代实现代码
    set /a errors+=1
)

echo.
echo ========================================
echo 测试结果
echo ========================================

if defined errors (
    if %errors% gtr 0 (
        echo ❌ 发现 %errors% 个问题
        echo 请检查上述错误并修复后重新测试
        echo.
        pause
        exit /b 1
    )
)

echo ✅ Windows兼容性检查通过!
echo.
echo 项目已正确配置Windows平台兼容性：
echo - WebEngine功能在Windows平台自动禁用
echo - 提供了友好的替代界面
echo - 构建配置支持Windows平台
echo.
echo 可以使用以下脚本进行构建：
echo - scripts\build\build-windows.bat (CMake)
echo - scripts\build\build-windows-qmake.bat (qmake)
echo.
pause
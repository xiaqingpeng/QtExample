cmake_minimum_required(VERSION 3.16)

# 项目信息
project(example VERSION 1.0.0 LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# 查找Qt模块
find_package(Qt6 COMPONENTS Core Gui Widgets REQUIRED)

# 设置MacOS特定选项
if(APPLE)
    # 忽略SDK版本检查（与qmake中的sdk_no_version_check对应）
    set(CMAKE_OSX_DEPLOYMENT_TARGET "14.0" CACHE STRING "Minimum macOS version" FORCE)
    # 设置应用程序包类型
    set(MACOSX_BUNDLE_BUNDLE_NAME "example")
    set(MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION})
    set(MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR})
    set(MACOSX_BUNDLE_GUI_IDENTIFIER "com.example.example")
endif()

# 定义模块库

# BasicControls模块
qt_add_library(BasicControls SHARED
    basiccontrols/basiccontrolstab.cpp
    basiccontrols/basiccontrolstab.h
)
target_link_libraries(BasicControls PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

# DataDisplay模块
qt_add_library(DataDisplay SHARED
    datadisplay/datadisplaytab.cpp
    datadisplay/datadisplaytab.h
)
target_link_libraries(DataDisplay PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

# AdvancedControls模块
qt_add_library(AdvancedControls SHARED
    advancedcontrols/advancedcontrolstab.cpp
    advancedcontrols/advancedcontrolstab.h
)
target_link_libraries(AdvancedControls PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

# Dialogs模块
qt_add_library(Dialogs SHARED
    dialogs/dialogstab.cpp
    dialogs/dialogstab.h
)
target_link_libraries(Dialogs PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

# LayoutExamples模块
qt_add_library(LayoutExamples SHARED
    layoutexamples/layoutexamplestab.cpp
    layoutexamples/layoutexamplestab.h
)
target_link_libraries(LayoutExamples PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

# 主应用程序源文件
set(SOURCES
    main.cpp
    mainuiwindow.cpp
)

# 主应用程序头文件
set(HEADERS
    mainuiwindow.h
)

# UI文件列表
set(UIS
    mainuiwindow.ui
)

# 创建可执行文件
qt_add_executable(${PROJECT_NAME} MACOSX_BUNDLE ${SOURCES} ${HEADERS} ${UIS})

# 链接Qt库和自定义模块
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    BasicControls
    DataDisplay
    AdvancedControls
    Dialogs
    LayoutExamples
)

# 包含模块头文件目录
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/advancedcontrols
    ${CMAKE_CURRENT_SOURCE_DIR}/basiccontrols
    ${CMAKE_CURRENT_SOURCE_DIR}/datadisplay
    ${CMAKE_CURRENT_SOURCE_DIR}/dialogs
    ${CMAKE_CURRENT_SOURCE_DIR}/layoutexamples
)

# 安装规则
install(TARGETS ${PROJECT_NAME}
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# 安装模块库
install(TARGETS BasicControls DataDisplay AdvancedControls Dialogs LayoutExamples
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
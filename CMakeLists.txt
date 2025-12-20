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

# 源文件列表
set(SOURCES
    main.cpp
    mainuiwindow.cpp
    basiccontrolstab.cpp
    datadisplaytab.cpp
    advancedcontrolstab.cpp
    dialogstab.cpp
    layoutexamplestab.cpp
)

# 头文件列表
set(HEADERS
    mainuiwindow.h
    basiccontrolstab.h
    datadisplaytab.h
    advancedcontrolstab.h
    dialogstab.h
    layoutexamplestab.h
)

# UI文件列表
set(UIS
    mainuiwindow.ui
)

# 创建可执行文件
qt_add_executable(${PROJECT_NAME} MACOSX_BUNDLE ${SOURCES} ${HEADERS} ${UIS})

# 链接Qt库
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

# 安装规则
install(TARGETS ${PROJECT_NAME}
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# 如果使用资源文件，需要添加以下内容
# set(RESOURCES resources.qrc)
# qt_add_resources(${PROJECT_NAME} "resources" PREFIX "/" FILES ${RESOURCES})
cmake_minimum_required(VERSION 3.16)

# 项目信息
project("QT Example" VERSION 1.0.0 LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# 查找Qt模块
find_package(Qt6 COMPONENTS Core Gui Widgets WebEngineWidgets Network PrintSupport REQUIRED)

# 设置MacOS特定选项
if(APPLE)
    # 设置部署目标为14.0以匹配Qt库版本
    set(CMAKE_OSX_DEPLOYMENT_TARGET "14.0" CACHE STRING "Minimum macOS version" FORCE)
    # 设置应用程序包类型
    set(MACOSX_BUNDLE_BUNDLE_NAME "QT Example")
    set(MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION})
    set(MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR})
    set(MACOSX_BUNDLE_GUI_IDENTIFIER "com.example.qtexample")
    set(MACOSX_BUNDLE_ICON_FILE "icon")
endif()

# 定义模块库

# NetworkManager模块
add_subdirectory(networkmanager)

# Styles主题管理模块 - 需要在其他模块之前定义
qt_add_library(Styles STATIC
    styles/theme_manager.cpp
    styles/theme_manager.h
)
target_link_libraries(Styles PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)
target_include_directories(Styles PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# BasicControls模块
qt_add_library(BasicControls STATIC
    basiccontrols/basiccontrolstab.cpp
    basiccontrols/basiccontrolstab.h
)
target_link_libraries(BasicControls PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

# DataDisplay模块
qt_add_library(DataDisplay STATIC
    datadisplay/datadisplaytab.cpp
    datadisplay/datadisplaytab.h
)
target_link_libraries(DataDisplay PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

# AdvancedControls模块
qt_add_library(AdvancedControls STATIC
    advancedcontrols/advancedcontrolstab.cpp
    advancedcontrols/advancedcontrolstab.h
)
target_link_libraries(AdvancedControls PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

# Dialogs模块
qt_add_library(Dialogs STATIC
    dialogs/dialogstab.cpp
    dialogs/dialogstab.h
)
target_link_libraries(Dialogs PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

# LayoutExamples模块
if(NOT WIN32)
    set(LAYOUT_QRC_FILES layoutexamplestab.qrc)
else()
    set(LAYOUT_QRC_FILES "")
endif()

qt_add_library(LayoutExamples1 STATIC
    layoutexamples1/layoutexamplestab1.cpp
    layoutexamples1/layoutexamplestab1.h
    ${LAYOUT_QRC_FILES}
)
target_link_libraries(LayoutExamples1 PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

# LayoutExamplesTab2模块
qt_add_library(LayoutExamplesTab2 STATIC
    layoutexamples2/layoutexamplestab2.cpp
    layoutexamples2/layoutexamplestab2.h
)
target_link_libraries(LayoutExamplesTab2 PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

# LayoutExamplesTab3模块
qt_add_library(LayoutExamplesTab3 STATIC
    layoutexamples3/layoutexamplestab3.cpp
    layoutexamples3/layoutexamplestab3.h
)
target_link_libraries(LayoutExamplesTab3 PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Network
    NetworkManager
)
target_include_directories(LayoutExamplesTab3 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# ECharts模块
qt_add_library(ECharts STATIC
    echarts/echartstab.cpp
    echarts/echartstab.h
    echarts/logstatstab.cpp
    echarts/logstatstab.h
    layoutexamplestab.qrc
)
target_link_libraries(ECharts PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::WebEngineWidgets
    Qt6::Network
    NetworkManager
    Analytics
    Styles
)
target_include_directories(ECharts PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/styles
)

# UserProfileTab模块
qt_add_library(UserProfileTab STATIC
    userprofiletab/userprofiletab.cpp
    userprofiletab/userprofiletab.h
    layoutexamplestab.qrc
)
target_link_libraries(UserProfileTab PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::WebEngineWidgets
    Qt6::Network
    NetworkManager
    Styles
)
target_include_directories(UserProfileTab PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/styles
)

# ReportsTab模块
qt_add_library(ReportsTab STATIC
    reportstab/reportstab.cpp
    reportstab/reportstab.h
    layoutexamplestab.qrc
)
target_link_libraries(ReportsTab PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::WebEngineWidgets
    Qt6::Network
    Qt6::PrintSupport
    NetworkManager
    Styles
)
target_include_directories(ReportsTab PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/styles
)

# LoginPage模块
add_subdirectory(loginpage)

# ChangePasswordPage模块
add_subdirectory(changepasswordpage)

# UserInfo模块
add_subdirectory(userinfo)

# Analytics埋点SDK模块
add_subdirectory(analytics)

# 添加依赖关系，确保 NetworkManager 先于依赖它的模块构建
add_dependencies(ChangePasswordPage NetworkManager)
add_dependencies(LoginPage NetworkManager)
add_dependencies(UserInfo NetworkManager)
add_dependencies(LayoutExamplesTab3 NetworkManager)
add_dependencies(ECharts NetworkManager)
add_dependencies(UserProfileTab NetworkManager)
add_dependencies(ReportsTab NetworkManager)
add_dependencies(LoginPage Analytics)
add_dependencies(UserInfo Analytics)
add_dependencies(ChangePasswordPage Analytics)
add_dependencies(ECharts Analytics)

# 添加Styles库依赖关系
add_dependencies(ECharts Styles)
add_dependencies(UserProfileTab Styles)
add_dependencies(ReportsTab Styles)

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
qt_add_executable(QTExample MACOSX_BUNDLE ${SOURCES} ${HEADERS} ${UIS} layoutexamplestab.qrc)

# 设置应用程序显示名称和输出名称
set_target_properties(QTExample PROPERTIES
    OUTPUT_NAME "example"
    MACOSX_BUNDLE_BUNDLE_NAME "QT Example"
)

# 设置应用程序图标
if(APPLE)
    # 复制图标文件到应用程序包
    set(ICON_FILE "${CMAKE_CURRENT_SOURCE_DIR}/images/icon.png")
    set_source_files_properties(${ICON_FILE} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
    target_sources(QTExample PRIVATE ${ICON_FILE})
endif()

# 设置RPATH，确保应用程序能找到共享库
set_target_properties(QTExample PROPERTIES
    MACOSX_RPATH YES
    BUILD_WITH_INSTALL_RPATH YES
    INSTALL_RPATH @executable_path/../Frameworks
)

# 链接Qt库和自定义模块
target_link_libraries(QTExample PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::WebEngineWidgets
    Qt6::Network
    Qt6::PrintSupport
    NetworkManager
    BasicControls
    DataDisplay
    AdvancedControls
    Dialogs
    LayoutExamples1
    LayoutExamplesTab2
    LayoutExamplesTab3
    ECharts
    UserProfileTab
    ReportsTab
    LoginPage
    ChangePasswordPage
    UserInfo
    Analytics
    Styles
)

# 包含模块头文件目录
target_include_directories(QTExample PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/advancedcontrols
    ${CMAKE_CURRENT_SOURCE_DIR}/basiccontrols
    ${CMAKE_CURRENT_SOURCE_DIR}/datadisplay
    ${CMAKE_CURRENT_SOURCE_DIR}/dialogs
    ${CMAKE_CURRENT_SOURCE_DIR}/LayoutExamples1
    ${CMAKE_CURRENT_SOURCE_DIR}/layoutexamples2
    ${CMAKE_CURRENT_SOURCE_DIR}/layoutexamples3
    ${CMAKE_CURRENT_SOURCE_DIR}/echarts
    ${CMAKE_CURRENT_SOURCE_DIR}/userprofiletab
    ${CMAKE_CURRENT_SOURCE_DIR}/reportstab
    ${CMAKE_CURRENT_SOURCE_DIR}/loginpage
    ${CMAKE_CURRENT_SOURCE_DIR}/changepasswordpage
    ${CMAKE_CURRENT_SOURCE_DIR}/userinfo
    ${CMAKE_CURRENT_SOURCE_DIR}/analytics
    ${CMAKE_CURRENT_SOURCE_DIR}/styles
)

# 安装规则
install(TARGETS QTExample
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# 安装模块库
install(TARGETS NetworkManager BasicControls DataDisplay AdvancedControls Dialogs LayoutExamples1 LayoutExamplesTab2 LayoutExamplesTab3 ECharts UserProfileTab ReportsTab LoginPage ChangePasswordPage UserInfo Analytics Styles
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# 复制共享库到应用程序的Frameworks目录中
set_target_properties(QTExample PROPERTIES
    MACOSX_BUNDLE TRUE
    MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.in
    MACOSX_RPATH TRUE
    BUILD_WITH_INSTALL_RPATH TRUE
    INSTALL_RPATH @executable_path/../Frameworks
)

# 复制所有共享库到应用程序的Frameworks目录（仅 macOS）
function(copy_shared_library_to_bundle target_name)
    if(APPLE)
        add_custom_command(TARGET QTExample POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_BUNDLE_DIR:QTExample>/Contents/Frameworks
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_FILE:${target_name}>
                $<TARGET_BUNDLE_DIR:QTExample>/Contents/Frameworks/
            COMMENT "Copying ${target_name} to app bundle"
        )
    endif()
endfunction()

# 注意：NetworkManager、LoginPage、ChangePasswordPage和UserInfo已改为静态库，
# 不再需要复制到macOS应用包中


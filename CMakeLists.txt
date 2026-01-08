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

# 启用企业级架构作为标准
add_definitions(-DENTERPRISE_EDITION)

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

# 企业级核心架构模块
qt_add_library(EnterpriseCore STATIC
    src/Core/Application.cpp
    src/Core/Application.h
    src/Core/Application.inl
    src/Core/ServiceManager.cpp
    src/Core/ServiceManager.h
    src/Core/DependencyContainer.cpp
    src/Core/DependencyContainer.h
)
target_link_libraries(EnterpriseCore PUBLIC
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Network
    EnterpriseLocalization
    Styles
)
target_include_directories(EnterpriseCore PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# 企业级接口模块
qt_add_library(EnterpriseInterfaces STATIC
    src/Interfaces/INetworkService.h
    src/Interfaces/IAuthenticationService.h
)
target_link_libraries(EnterpriseInterfaces PRIVATE
    Qt6::Core
)
target_include_directories(EnterpriseInterfaces PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# 企业级服务模块
qt_add_library(EnterpriseServices STATIC
    src/Services/NetworkService.cpp
    src/Services/NetworkService.h
    src/Services/AuthenticationService.cpp
    src/Services/AuthenticationService.h
    src/Services/AnalyticsService.cpp
    src/Services/AnalyticsService.h
)
target_link_libraries(EnterpriseServices PUBLIC
    Qt6::Core
    Qt6::Network
    EnterpriseInterfaces
)
target_include_directories(EnterpriseServices PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# 企业级本地化模块
qt_add_library(EnterpriseLocalization STATIC
    src/Localization/LocalizationManager.cpp
    src/Localization/LocalizationManager.h
)
target_link_libraries(EnterpriseLocalization PRIVATE
    Qt6::Core
)
target_include_directories(EnterpriseLocalization PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# NetworkManager模块
add_subdirectory(src/Network)

# Styles主题管理模块 - 需要在其他模块之前定义
qt_add_library(Styles STATIC
    src/Styles/theme_manager.cpp
    src/Styles/theme_manager.h
)
target_link_libraries(Styles PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)
target_include_directories(Styles PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# BasicControls模块 - 已移除
# DataDisplay模块 - 已移除
# AdvancedControls模块 - 已移除
# Dialogs模块 - 已移除
# LayoutExamples模块 - 已移除

# ECharts模块
qt_add_library(ECharts STATIC
    src/Charts/echartstab.cpp
    src/Charts/echartstab.h
    src/Charts/logstatstab.cpp
    src/Charts/logstatstab.h
    layoutexamplestab.qrc
)
target_link_libraries(ECharts PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::WebEngineWidgets
    Qt6::Network
    network
    analytics
    Styles
    Device
)

target_include_directories(ECharts PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Styles
)

# UserProfileTab模块
qt_add_library(UserProfileTab STATIC
    src/UserProfile/userprofiletab.cpp
    src/UserProfile/userprofiletab.h
    layoutexamplestab.qrc
)
target_link_libraries(UserProfileTab PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::WebEngineWidgets
    Qt6::Network
    network
    Styles
)
target_include_directories(UserProfileTab PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Styles
)

# ReportsTab模块
qt_add_library(ReportsTab STATIC
    src/Reports/reportstab.cpp
    src/Reports/reportstab.h
    layoutexamplestab.qrc
)
target_link_libraries(ReportsTab PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::WebEngineWidgets
    Qt6::Network
    Qt6::PrintSupport
    network
    Styles
)
target_include_directories(ReportsTab PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Styles
)

# Auth模块（包含 LoginPage 和 ChangePasswordPage）
add_subdirectory(src/Auth)

# UserInfo模块
add_subdirectory(src/UserProfile)

# Analytics埋点SDK模块
add_subdirectory(src/Analytics)

# Device模块
add_subdirectory(src/Device)

# 创建库别名以保持向后兼容（在所有库创建后）
add_library(NetworkManager ALIAS network)
add_library(LoginPage ALIAS auth)
add_library(ChangePasswordPage ALIAS auth)
add_library(UserInfo ALIAS userprofile)
add_library(Analytics ALIAS analytics)

# 添加依赖关系，确保 network 先于依赖它的模块构建
# 注意：auth 包含 LoginPage 和 ChangePasswordPage，userprofile 包含 UserInfo
add_dependencies(auth network)
add_dependencies(userprofile network)
add_dependencies(ECharts network)
add_dependencies(UserProfileTab network)
add_dependencies(ReportsTab network)
add_dependencies(auth analytics)
add_dependencies(userprofile analytics)
add_dependencies(ECharts analytics)

# 添加Styles库依赖关系
add_dependencies(ECharts Styles)
add_dependencies(UserProfileTab Styles)
add_dependencies(ReportsTab Styles)

# 主应用程序源文件
set(SOURCES
    src/App/main.cpp
    src/UI/mainuiwindow.cpp
)

# 主应用程序头文件
set(HEADERS
    src/UI/mainuiwindow.h
)

# UI文件列表
set(UIS
    src/UI/mainuiwindow.ui
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
    EnterpriseServices
    EnterpriseCore
    EnterpriseInterfaces
    EnterpriseLocalization
    network
    ECharts
    UserProfileTab
    ReportsTab
    auth
    userprofile
    analytics
    Styles
    Device
)

# 包含模块头文件目录
target_include_directories(QTExample PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Core
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Interfaces
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Services
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Localization
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Charts
    ${CMAKE_CURRENT_SOURCE_DIR}/src/UserProfile
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Reports
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Auth
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Analytics
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Styles
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Device
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Network
    ${CMAKE_CURRENT_SOURCE_DIR}/src/UI
    ${CMAKE_CURRENT_SOURCE_DIR}/src/App
)

# 安装规则
install(TARGETS QTExample
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# 安装模块库
install(TARGETS network ECharts UserProfileTab ReportsTab auth userprofile analytics Styles Device
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


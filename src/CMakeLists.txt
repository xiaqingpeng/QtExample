cmake_minimum_required(VERSION 3.16)

# Qt大型项目结构
project(QtEnterpriseProject VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找Qt6组件
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Network WebEngineWidgets)

# 启用Qt的MOC、UIC和RCC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# 添加子目录（按依赖顺序）
add_subdirectory(Interfaces)
add_subdirectory(Localization)
add_subdirectory(Services)
add_subdirectory(Core)
add_subdirectory(Utils)
add_subdirectory(Styles)
add_subdirectory(Network)
add_subdirectory(Analytics)
add_subdirectory(Auth)
add_subdirectory(UserProfile)
add_subdirectory(Charts)
add_subdirectory(Reports)
add_subdirectory(UI)
add_subdirectory(App)
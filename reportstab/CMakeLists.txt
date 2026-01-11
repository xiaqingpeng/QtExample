# ReportsTab模块的CMakeLists.txt

cmake_minimum_required(VERSION 3.16)
project(ReportsTab)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找Qt6包
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Network)

# 定义源文件和头文件
set(REPORTSTAB_SOURCES
    reportstab.cpp
)

set(REPORTSTAB_HEADERS
    reportstab.h
)

# 创建静态库
add_library(${PROJECT_NAME} STATIC
    ${REPORTSTAB_SOURCES}
    ${REPORTSTAB_HEADERS}
)

# 链接Qt库
target_link_libraries(${PROJECT_NAME}
    Qt6::Core
    Qt6::Widgets
    
    Qt6::Network
    NetworkManager
)

# 设置包含目录
target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/../
)

# 启用MOC自动生成
set_target_properties(${PROJECT_NAME} PROPERTIES
    AUTOMOC ON
    AUTORCC ON
    AUTOUIC ON
)
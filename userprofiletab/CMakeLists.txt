cmake_minimum_required(VERSION 3.16)

project(userprofiletab)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找Qt6组件
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Network)

# 设置源文件
set(SOURCES
    userprofiletab.cpp
)

# 设置头文件
set(HEADERS
    userprofiletab.h
)

# 创建静态库
add_library(${PROJECT_NAME} STATIC ${SOURCES} ${HEADERS})

# 链接Qt6库
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Widgets
    
    Qt6::Network
    NetworkManager
)

# 包含目录
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

# 设置MOC自动生成
set_target_properties(${PROJECT_NAME} PROPERTIES
    AUTOMOC ON
    AUTORCC ON
    AUTOUIC ON
)
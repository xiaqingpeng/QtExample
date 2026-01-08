cmake_minimum_required(VERSION 3.16)

# 创建Device模块
qt_add_library(Device STATIC
    contenttab.cpp
    contenttab.h
    serverconfigtab.cpp
    serverconfigtab.h
)

# 定义WEBENGINE_AVAILABLE宏，因为Device模块使用了WebEngineWidgets
target_compile_definitions(Device PRIVATE WEBENGINE_AVAILABLE)

target_link_libraries(Device PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::WebEngineWidgets
    Qt6::Network
    network
    Styles
)

target_include_directories(Device PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../Core
    ${CMAKE_CURRENT_SOURCE_DIR}/../Network
    ${CMAKE_CURRENT_SOURCE_DIR}/../Services
    ${CMAKE_CURRENT_SOURCE_DIR}/../Styles
)
cmake_minimum_required(VERSION 3.16)

# 创建Device模块
qt_add_library(Device STATIC
    contenttab.cpp
    contenttab.h
    serverconfigtab.cpp
    serverconfigtab.h
)

target_link_libraries(Device PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::WebEngineWidgets
    Qt6::Network
    NetworkManager
    Styles
)

target_include_directories(Device PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../Core
    ${CMAKE_CURRENT_SOURCE_DIR}/../Network
    ${CMAKE_CURRENT_SOURCE_DIR}/../Services
    ${CMAKE_CURRENT_SOURCE_DIR}/../Styles
)
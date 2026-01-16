cmake_minimum_required(VERSION 3.16)

# 创建Device模块
qt_add_library(Device STATIC
    contenttab.cpp
    contenttab.h
    serverconfigtab.cpp
    serverconfigtab.h
)

# 检查WebEngine是否可用
if(WEBENGINE_AVAILABLE)
    target_compile_definitions(Device PRIVATE WEBENGINE_AVAILABLE)
    message(STATUS "Device module: WebEngine enabled")
else()
    message(STATUS "Device module: WebEngine disabled")
endif()

target_link_libraries(Device PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Network
    network
    Styles
)

# 只在 WebEngine 可用时链接
if(WEBENGINE_AVAILABLE)
    target_link_libraries(Device PRIVATE Qt6::WebEngineWidgets)
endif()

target_include_directories(Device PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../Core
    ${CMAKE_CURRENT_SOURCE_DIR}/../Network
    ${CMAKE_CURRENT_SOURCE_DIR}/../Services
    ${CMAKE_CURRENT_SOURCE_DIR}/../Styles
)
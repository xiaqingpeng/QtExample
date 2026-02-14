cmake_minimum_required(VERSION 3.16)

# 创建Device模块
qt_add_library(Device STATIC
    contenttab.cpp
    contenttab.h
    serverconfigtab.cpp
    serverconfigtab.h
)

if(Qt6Charts_FOUND)
    target_link_libraries(Device PUBLIC Qt6::Charts)
    target_compile_definitions(Device PRIVATE HAS_QT_CHARTS)
endif()

target_link_libraries(Device PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Network
    network
    Styles
    ECharts
)

target_include_directories(Device PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../Core
    ${CMAKE_CURRENT_SOURCE_DIR}/../Services
    ${CMAKE_CURRENT_SOURCE_DIR}/../Styles
)
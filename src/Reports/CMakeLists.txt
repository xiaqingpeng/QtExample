qt6_add_library(reports STATIC
    reportstab.h
    reportstab.cpp
)

# 定义WEBENGINE_AVAILABLE宏，因为reports模块使用了WebEngineWidgets
target_compile_definitions(reports PRIVATE WEBENGINE_AVAILABLE)

target_link_libraries(reports
    Qt6::Core
    Qt6::Widgets
    Qt6::WebEngineWidgets
    Qt6::Network
    EnterpriseCore
    EnterpriseServices
    network
    analytics
    Styles
)

target_include_directories(reports PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src/App
)
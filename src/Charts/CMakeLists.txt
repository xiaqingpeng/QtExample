qt6_add_library(charts STATIC
    echartstab.h
    echartstab.cpp
    logstatstab.h
    logstatstab.cpp
)

# 定义WEBENGINE_AVAILABLE宏，因为charts模块使用了WebEngineWidgets
target_compile_definitions(charts PRIVATE WEBENGINE_AVAILABLE)

target_link_libraries(charts
    Qt6::Core
    Qt6::Widgets
    Qt6::WebEngineWidgets
    Qt6::Network
    EnterpriseCore
    EnterpriseServices
    network
    Styles
)

target_include_directories(charts PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src/App
)
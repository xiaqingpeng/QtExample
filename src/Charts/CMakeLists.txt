qt6_add_library(charts STATIC
    echartstab.h
    echartstab.cpp
    echartstab.ui
    logstatstab.h
    logstatstab.cpp
    logstatstab.ui
)

target_link_libraries(charts
    Qt6::Core
    Qt6::Widgets
    Qt6::WebEngineWidgets
    Qt6::Network
    enterprisecore
    enterpriseservices
    network
    styles
)

target_include_directories(charts PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}
)
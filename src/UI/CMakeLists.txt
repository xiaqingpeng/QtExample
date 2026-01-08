qt6_add_library(ui STATIC
    mainuiwindow.h
    mainuiwindow.cpp
    mainuiwindow.ui
)

target_link_libraries(ui
    Qt6::Core
    Qt6::Widgets
    Qt6::WebEngineWidgets
    Qt6::Network
    EnterpriseCore
    EnterpriseServices
    network
    analytics
    auth
    userprofile
    ECharts
    ReportsTab
    Device
    Styles
)

target_include_directories(ui PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}
)
qt6_add_library(reports STATIC
    reportstab.h
    reportstab.cpp
)

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
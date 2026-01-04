qt6_add_library(reports STATIC
    reportstab.h
    reportstab.cpp
    reportstab.ui
)

target_link_libraries(reports
    Qt6::Core
    Qt6::Widgets
    Qt6::Network
    enterprisecore
    enterpriseservices
    network
    analytics
    styles
)

target_include_directories(reports PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}
)
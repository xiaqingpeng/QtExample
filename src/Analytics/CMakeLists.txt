qt6_add_library(analytics STATIC
    analytics.h
    analytics.cpp
)

target_link_libraries(analytics
    Qt6::Core
    Qt6::Network
    enterprisecore
    enterpriseservices
    network
)

target_include_directories(analytics PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}
)
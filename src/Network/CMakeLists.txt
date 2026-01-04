qt6_add_library(network STATIC
    networkmanager.h
    networkmanager.cpp
)

target_link_libraries(network
    Qt6::Core
    Qt6::Network
    Qt6::Widgets
    enterprisecore
    enterpriseservices
)

target_include_directories(network PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}
)
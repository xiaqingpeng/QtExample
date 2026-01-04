qt6_add_library(styles STATIC
    theme_manager.h
    theme_manager.cpp
)

target_link_libraries(styles
    Qt6::Core
    Qt6::Widgets
    enterprisecore
    utils
)

target_include_directories(styles PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}
)
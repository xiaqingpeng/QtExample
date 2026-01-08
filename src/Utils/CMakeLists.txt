qt6_add_library(utils STATIC
    logger.h
    logger.cpp
)

target_link_libraries(utils
    Qt6::Core
    EnterpriseCore
)

target_include_directories(utils PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}
)
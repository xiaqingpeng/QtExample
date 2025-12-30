# Analytics埋点SDK模块
qt_add_library(Analytics STATIC
    analytics.cpp
    analytics.h
)

target_link_libraries(Analytics PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Network
)

target_include_directories(Analytics PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
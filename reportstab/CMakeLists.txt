# ReportsTab模块
qt_add_library(ReportsTab STATIC
    reportstab.cpp
    reportstab.h
)

target_link_libraries(ReportsTab PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Network
    NetworkManager
)

target_include_directories(ReportsTab PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)
# UserProfileTab模块
qt_add_library(UserProfileTab STATIC
    userprofiletab.cpp
    userprofiletab.h
)

target_link_libraries(UserProfileTab PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Network
    NetworkManager
)

target_include_directories(UserProfileTab PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)
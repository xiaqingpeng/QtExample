qt6_add_library(userprofile STATIC
    userinfopage.h
    userinfopage.cpp
    userinfopage.ui
    userprofiletab.h
    userprofiletab.cpp
    userprofiletab.ui
)

target_link_libraries(userprofile
    Qt6::Core
    Qt6::Widgets
    Qt6::Network
    enterprisecore
    enterpriseservices
    network
    auth
    styles
)

target_include_directories(userprofile PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}
)
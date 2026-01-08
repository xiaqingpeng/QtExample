qt6_add_library(userprofile STATIC
    userinfopage.h
    userinfopage.cpp
    userprofiletab.h
    userprofiletab.cpp
)

target_link_libraries(userprofile
    Qt6::Core
    Qt6::Widgets
    Qt6::WebEngineWidgets
    Qt6::Network
    EnterpriseCore
    EnterpriseServices
    network
    auth
    Styles
)

target_include_directories(userprofile PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src/Network
    ${CMAKE_SOURCE_DIR}/src/Analytics
    ${CMAKE_SOURCE_DIR}/src/App
)
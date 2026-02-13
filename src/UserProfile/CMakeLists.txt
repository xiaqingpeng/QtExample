qt6_add_library(userprofile STATIC
    userinfopage.h
    userinfopage.cpp
    userprofiletab.h
    userprofiletab.cpp
)

target_link_libraries(userprofile PUBLIC
    Qt6::Charts
PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Network
    Qt6::PrintSupport
    EnterpriseCore
    EnterpriseServices
    network
    auth
    Styles
)

target_include_directories(userprofile PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src/Analytics
    ${CMAKE_SOURCE_DIR}/src/App
)
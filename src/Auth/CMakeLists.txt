qt6_add_library(auth STATIC
    loginpage.h
    loginpage.cpp
    changepasswordpage.h
    changepasswordpage.cpp
)

target_link_libraries(auth
    Qt6::Core
    Qt6::Widgets
    Qt6::Network
    EnterpriseCore
    EnterpriseServices
    network
    Styles
)

target_include_directories(auth PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src/Network
    ${CMAKE_SOURCE_DIR}/src/Analytics
    ${CMAKE_SOURCE_DIR}/src/App
)
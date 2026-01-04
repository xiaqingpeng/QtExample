qt6_add_library(auth STATIC
    loginpage.h
    loginpage.cpp
    loginpage.ui
    changepasswordpage.h
    changepasswordpage.cpp
    changepasswordpage.ui
)

target_link_libraries(auth
    Qt6::Core
    Qt6::Widgets
    Qt6::Network
    enterprisecore
    enterpriseservices
    network
    styles
)

target_include_directories(auth PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}
)
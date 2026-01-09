qt6_add_library(userprofile STATIC
    userinfopage.h
    userinfopage.cpp
    userprofiletab.h
    userprofiletab.cpp
)

# 检查WebEngine是否可用，特别处理Windows平台
if(Qt6WebEngineWidgets_FOUND AND NOT WIN32)
    target_compile_definitions(userprofile PRIVATE WEBENGINE_AVAILABLE)
    target_link_libraries(userprofile PRIVATE Qt6::WebEngineWidgets)
    message(STATUS "UserProfile module: WebEngine enabled")
elseif(WIN32)
    message(STATUS "UserProfile module: WebEngine disabled for Windows compatibility")
else()
    message(STATUS "UserProfile module: WebEngine not available")
endif()

target_link_libraries(userprofile PRIVATE
    Qt6::Core
    Qt6::Widgets
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
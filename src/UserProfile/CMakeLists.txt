qt6_add_library(userprofile STATIC
    userinfopage.h
    userinfopage.cpp
    userprofiletab.h
    userprofiletab.cpp
)

# 检查WebEngine是否可用（使用主 CMakeLists.txt 中的 WEBENGINE_AVAILABLE）
if(WEBENGINE_AVAILABLE)
    target_compile_definitions(userprofile PRIVATE WEBENGINE_AVAILABLE)
    target_link_libraries(userprofile PRIVATE Qt6::WebEngineWidgets)
    message(STATUS "UserProfile module: WebEngine enabled")
else()
    message(STATUS "UserProfile module: WebEngine disabled")
endif()

target_link_libraries(userprofile PRIVATE
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
    ${CMAKE_SOURCE_DIR}/src/Network
    ${CMAKE_SOURCE_DIR}/src/Analytics
    ${CMAKE_SOURCE_DIR}/src/App
)
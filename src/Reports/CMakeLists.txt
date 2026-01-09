qt6_add_library(reports STATIC
    reportstab.h
    reportstab.cpp
)

# 检查WebEngine是否可用，特别处理Windows平台
if(Qt6WebEngineWidgets_FOUND AND NOT WIN32)
    target_compile_definitions(reports PRIVATE WEBENGINE_AVAILABLE)
    target_link_libraries(reports PRIVATE Qt6::WebEngineWidgets)
    message(STATUS "Reports module: WebEngine enabled")
elseif(WIN32)
    message(STATUS "Reports module: WebEngine disabled for Windows compatibility")
else()
    message(STATUS "Reports module: WebEngine not available")
endif()

target_link_libraries(reports PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Network
    EnterpriseCore
    EnterpriseServices
    network
    analytics
    Styles
)

target_include_directories(reports PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src/App
)
qt6_add_library(reports STATIC
    reportstab.h
    reportstab.cpp
)

# 检查WebEngine是否可用（使用主 CMakeLists.txt 中的 WEBENGINE_AVAILABLE）
if(WEBENGINE_AVAILABLE)
    target_compile_definitions(reports PRIVATE WEBENGINE_AVAILABLE)
    target_link_libraries(reports PRIVATE Qt6::WebEngineWidgets)
    message(STATUS "Reports module: WebEngine enabled")
else()
    message(STATUS "Reports module: WebEngine disabled")
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
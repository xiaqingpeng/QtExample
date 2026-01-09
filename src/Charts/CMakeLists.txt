qt6_add_library(charts STATIC
    echartstab.h
    echartstab.cpp
    logstatstab.h
    logstatstab.cpp
)

# 检查WebEngine是否可用，特别处理Windows平台
if(Qt6WebEngineWidgets_FOUND AND NOT WIN32)
    target_compile_definitions(charts PRIVATE WEBENGINE_AVAILABLE)
    target_link_libraries(charts PRIVATE Qt6::WebEngineWidgets)
    message(STATUS "Charts module: WebEngine enabled")
elseif(WIN32)
    message(STATUS "Charts module: WebEngine disabled for Windows compatibility")
else()
    message(STATUS "Charts module: WebEngine not available")
endif()

target_link_libraries(charts PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Network
    EnterpriseCore
    EnterpriseServices
    network
    Styles
)

target_include_directories(charts PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src/App
)
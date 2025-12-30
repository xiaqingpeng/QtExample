# LoginPage模块的CMakeLists.txt

# 创建静态库
qt_add_library(LoginPage STATIC
    loginpage.cpp
    loginpage.h
)

# 链接Qt库
target_link_libraries(LoginPage PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Network
    NetworkManager
    Analytics
)

# 添加依赖关系
add_dependencies(LoginPage Analytics)

# 导出头文件
target_include_directories(LoginPage PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)
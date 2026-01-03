# UserInfo模块的CMakeLists.txt

# 创建静态库
qt_add_library(UserInfo STATIC
    userinfopage.cpp
    userinfopage.h
)

# 链接Qt库
target_link_libraries(UserInfo PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Network
    NetworkManager
    Analytics
    Styles
)

# 添加依赖关系
add_dependencies(UserInfo Analytics)
add_dependencies(UserInfo Styles)

# 导出头文件
target_include_directories(UserInfo PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../styles
)
# LoginPage模块的CMakeLists.txt

# 创建共享库
qt_add_library(LoginPage SHARED
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
)

# 导出头文件
target_include_directories(LoginPage PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)
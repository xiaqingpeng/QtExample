# ChangePasswordPage模块的CMakeLists.txt

# 创建静态库
qt_add_library(ChangePasswordPage STATIC
    changepasswordpage.cpp
    changepasswordpage.h
)

# 链接Qt库
target_link_libraries(ChangePasswordPage PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Network
    NetworkManager
)

# 导出头文件
target_include_directories(ChangePasswordPage PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)
# NetworkManager模块的CMakeLists.txt

# 创建静态库
qt_add_library(NetworkManager STATIC
    ../networkmanager.cpp
    ../networkmanager.h
)

# 链接Qt库
target_link_libraries(NetworkManager PRIVATE
    Qt6::Core
    Qt6::Network
)

# 导出头文件
target_include_directories(NetworkManager PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)
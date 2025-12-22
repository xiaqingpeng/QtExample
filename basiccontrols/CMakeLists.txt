# BasicControls模块的CMakeLists.txt

# 创建共享库
qt_add_library(BasicControls SHARED
    basiccontrolstab.cpp
    basiccontrolstab.h
)

# 链接Qt库
target_link_libraries(BasicControls PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

# 导出头文件
target_include_directories(BasicControls PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
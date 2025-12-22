# AdvancedControls模块的CMakeLists.txt

# 创建共享库
qt_add_library(AdvancedControls SHARED
    advancedcontrolstab.cpp
    advancedcontrolstab.h
)

# 链接Qt库
target_link_libraries(AdvancedControls PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

# 导出头文件
target_include_directories(AdvancedControls PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
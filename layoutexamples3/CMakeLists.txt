# LayoutExamples模块的CMakeLists.txt

# 创建共享库
# LayoutExamples模块
qt_add_library(LayoutExamples3 SHARED
    layoutexamplestab3.cpp
    layoutexamplestab3.h
)

# 链接Qt库
target_link_libraries(LayoutExamples3 PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

# 导出头文件
target_include_directories(LayoutExamples3 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)
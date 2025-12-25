# LayoutExamples模块的CMakeLists.txt

# 创建静态库
# LayoutExamples模块
qt_add_library(LayoutExamples1 STATIC
    layoutexamplestab.cpp
    layoutexamplestab.h
)

# 链接Qt库
target_link_libraries(LayoutExamples1 PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

# 导出头文件
target_include_directories(LayoutExamples1 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)
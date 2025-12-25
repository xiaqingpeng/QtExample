# LayoutExamples模块的CMakeLists.txt

# 创建静态库
# LayoutExamples模块
qt_add_library(LayoutExamples2 STATIC
    
    layoutexamplestab2.cpp
    layoutexamplestab2.h
    
)

# 链接Qt库
target_link_libraries(LayoutExamples2 PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

# 导出头文件
target_include_directories(LayoutExamples2 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)
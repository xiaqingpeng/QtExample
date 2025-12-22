# LayoutExamples模块的CMakeLists.txt

# 创建共享库
qt_add_library(LayoutExamples SHARED
    ${CMAKE_CURRENT_SOURCE_DIR}/../layoutexamplestab.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../layoutexamplestab.h
)

# 链接Qt库
target_link_libraries(LayoutExamples PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

# 导出头文件
target_include_directories(LayoutExamples PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)
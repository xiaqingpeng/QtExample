#!/usr/bin/env python3

# 读取当前CMakeLists.txt内容
with open('CMakeLists.txt', 'r') as f:
    content = f.read()

# 替换子目录引用为直接模块定义
content = content.replace(
    '# 添加模块子目录
add_subdirectory(basiccontrols)
add_subdirectory(datadisplay)
add_subdirectory(advancedcontrols)
add_subdirectory(dialogs)
add_subdirectory(layoutexamples)',
    '# 定义模块库

# BasicControls模块
qt_add_library(BasicControls SHARED
    basiccontrolstab.cpp
    basiccontrolstab.h
)
target_link_libraries(BasicControls PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

# DataDisplay模块
qt_add_library(DataDisplay SHARED
    datadisplaytab.cpp
    datadisplaytab.h
)
target_link_libraries(DataDisplay PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

# AdvancedControls模块
qt_add_library(AdvancedControls SHARED
    advancedcontrolstab.cpp
    advancedcontrolstab.h
)
target_link_libraries(AdvancedControls PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

# Dialogs模块
qt_add_library(Dialogs SHARED
    dialogstab.cpp
    dialogstab.h
)
target_link_libraries(Dialogs PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

# LayoutExamples模块
qt_add_library(LayoutExamples SHARED
    layoutexamplestab.cpp
    layoutexamplestab.h
)
target_link_libraries(LayoutExamples PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)#!/usr/bin/env python3

# 读取当前CMakeLists.txt内容
with open('CMakeLists.txt', 'r') as f:
    content = f.read()

# 替换子目录引用为直接模块定义
new_content = content.replace(
    '# 添加模块子目录
add_subdirectory(basiccontrols)
add_subdirectory(datadisplay)
add_subdirectory(advancedcontrols)
add_subdirectory(dialogs)
add_subdirectory(layoutexamples)',
    '# 定义模块库

# BasicControls模块
qt_add_library(BasicControls SHARED
    basiccontrolstab.cpp
    basiccontrolstab.h
)
target_link_libraries(BasicControls PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

# DataDisplay模块
qt_add_library(DataDisplay SHARED
    datadisplaytab.cpp
    datadisplaytab.h
)
target_link_libraries(DataDisplay PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

# AdvancedControls模块
qt_add_library(AdvancedControls SHARED
    advancedcontrolstab.cpp
    advancedcontrolstab.h
)
target_link_libraries(AdvancedControls PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

# Dialogs模块
qt_add_library(Dialogs SHARED
    dialogstab.cpp
    dialogstab.h
)
target_link_libraries(Dialogs PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

# LayoutExamples模块
qt_add_library(LayoutExamples SHARED
    layoutexamplestab.cpp
    layoutexamplestab.h
)
target_link_libraries(LayoutExamples PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

# 修改头文件包含目录
#!/usr/bin/env python3

# 读取当前CMakeLists.txt内容
with open('CMakeLists.txt', 'r') as f:
    content = f.read()

# 替换子目录引用为直接模块定义
new_content = content.replace(
    '# 添加模块子目录
add_subdirectory(basiccontrols)
add_subdirectory(datadisplay)
add_subdirectory(advancedcontrols)
add_subdirectory(dialogs)
add_subdirectory(layoutexamples)',
    '# 定义模块库

# BasicControls模块
qt_add_library(BasicControls SHARED
    basiccontrolstab.cpp
    basiccontrolstab.h
)
target_link_libraries(BasicControls PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

# DataDisplay模块
qt_add_library(DataDisplay SHARED
    datadisplaytab.cpp
    datadisplaytab.h
)
target_link_libraries(DataDisplay PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

# AdvancedControls模块
qt_add_library(AdvancedControls SHARED
    advancedcontrolstab.cpp
    advancedcontrolstab.h
)
target_link_libraries(AdvancedControls PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

# Dialogs模块
qt_add_library(Dialogs SHARED
    dialogstab.cpp
    dialogstab.h
)
target_link_libraries(Dialogs PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

# LayoutExamples模块
qt_add_library(LayoutExamples SHARED
    layoutexamplestab.cpp
    layoutexamplestab.h
)
target_link_libraries(LayoutExamples PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)#!/usr/bin/env python3

# 读取当前CMakeLists.txt内容
with open('CMakeLists.txt', 'r') as f:
    content = f.read()

# 替换子目录引用为直接模块定义
new_content = content.replace(
    '# 添加模块子目录
add_subdirectory(basiccontrols)
add_subdirectory(datadisplay)
add_subdirectory(advancedcontrols)
add_subdirectory(dialogs)
add_subdirectory(layoutexamples)',
    '# 定义模块库

# BasicControls模块
qt_add_library(BasicControls SHARED
    basiccontrolstab.cpp
    basiccontrolstab.h
)
target_link_libraries(BasicControls PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

# DataDisplay模块
qt_add_library(DataDisplay SHARED
    datadisplaytab.cpp
    datadisplaytab.h
)
target_link_libraries(DataDisplay PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

# AdvancedControls模块
qt_add_library(AdvancedControls SHARED
    advancedcontrolstab.cpp
    advancedcontrolstab.h
)
target_link_libraries(AdvancedControls PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

# Dialogs模块
qt_add_library(Dialogs SHARED
    dialogstab.cpp
    dialogstab.h
)
target_link_libraries(Dialogs PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

# LayoutExamples模块
qt_add_library(LayoutExamples SHARED
    layoutexamplestab.cpp
    layoutexamplestab.h
)
target_link_libraries(LayoutExamples PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)#!/usr/bin/env python3

# 读取当前CMakeLists.txt内容
with open('CMakeLists.txt', 'r') as f:
    content = f.read()

# 替换子目录引用为直接模块定义
new_content = content.replace(
    '# 添加模块子目录
add_subdirectory(basiccontrols)
add_subdirectory(datadisplay)
add_subdirectory(advancedcontrols)
add_subdirectory(dialogs)
add_subdirectory(layoutexamples)',
    '# 定义模块库

# BasicControls模块
qt_add_library(BasicControls SHARED
    basiccontrolstab.cpp
    basiccontrolstab.h
)
target_link_libraries(BasicControls PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

# DataDisplay模块
qt_add_library(DataDisplay SHARED
    datadisplaytab.cpp
    datadisplaytab.h
)
target_link_libraries(DataDisplay PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

# AdvancedControls模块
qt_add_library(AdvancedControls SHARED
    advancedcontrolstab.cpp
    advancedcontrolstab.h
)
target_link_libraries(AdvancedControls PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

# Dialogs模块
qt_add_library(Dialogs SHARED
    dialogstab.cpp
    dialogstab.h
)
target_link_libraries(Dialogs PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

# LayoutExamples模块
qt_add_library(LayoutExamples SHARED
    layoutexamplestab.cpp
    layoutexamplestab.h
)
target_link_libraries(LayoutExamples PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

# 修改头文件包含目录
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)#!/usr/bin/env python3

# 读取当前CMakeLists.txt内容
with open('CMakeLists.txt', 'r') as f:
    content = f.read()

# 替换子目录引用为直接模块定义
new_content = content.replace(
    '# 添加模块子目录
add_subdirectory(basiccontrols)
add_subdirectory(datadisplay)
add_subdirectory(advancedcontrols)
add_subdirectory(dialogs)
add_subdirectory(layoutexamples)',
    '# 定义模块库

# BasicControls模块
qt_add_library(BasicControls SHARED
    basiccontrolstab.cpp
    basiccontrolstab.h
)
target_link_libraries(BasicControls PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

# DataDisplay模块
qt_add_library(DataDisplay SHARED
    datadisplaytab.cpp
    datadisplaytab.h
)
target_link_libraries(DataDisplay PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

# AdvancedControls模块
qt_add_library(AdvancedControls SHARED
    advancedcontrolstab.cpp
    advancedcontrolstab.h
)
target_link_libraries(AdvancedControls PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

# Dialogs模块
qt_add_library(Dialogs SHARED
    dialogstab.cpp
    dialogstab.h
)
target_link_libraries(Dialogs PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

# LayoutExamples模块
qt_add_library(LayoutExamples SHARED
    layoutexamplestab.cpp
    layoutexamplestab.h
)
target_link_libraries(LayoutExamples PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)
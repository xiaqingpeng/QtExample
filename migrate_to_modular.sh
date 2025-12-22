#!/bin/bash

# 创建模块目录
mkdir -p basiccontrols datadisplay advancedcontrols dialogs layoutexamples

# 移动BasicControls模块文件
mv basiccontrolstab.cpp basiccontrols/
mv basiccontrolstab.h basiccontrols/

# 移动DataDisplay模块文件
mv datadisplaytab.cpp datadisplay/
mv datadisplaytab.h datadisplay/

# 移动AdvancedControls模块文件
mv advancedcontrolstab.cpp advancedcontrols/
mv advancedcontrolstab.h advancedcontrols/

# 移动Dialogs模块文件
mv dialogstab.cpp dialogs/
mv dialogstab.h dialogs/

# 移动LayoutExamples模块文件
mv layoutexamplestab.cpp layoutexamples/
mv layoutexamplestab.h layoutexamples/

echo "文件迁移完成！"
echo "请确保您已经更新了CMakeLists.txt和example.pro文件中的路径配置。"
echo "使用以下命令构建项目："
echo "cmake -B build && cmake --build build"
echo "或者使用qmake："
echo "qmake && make"
# LayoutExamples模块的qmake项目文件

QT += core gui widgets

CONFIG += c++17 sdk_no_version_check

# 禁用过时的single_module链接器标志
CONFIG -= single_module

# 创建共享库
TEMPLATE = lib
CONFIG += shared

TARGET = LayoutExamples1

SOURCES += \
    layoutexamplestab1.cpp

HEADERS += \
    layoutexamplestab1.h

# 添加资源文件
RESOURCES += \
    ../layoutexamplestab.qrc

# 安装配置
target.path = $$[QT_INSTALL_LIBS]
INSTALLS += target

# 头文件安装
headers.files = $$HEADERS
headers.path = $$[QT_INSTALL_HEADERS]/LayoutExamples1
INSTALLS += headers
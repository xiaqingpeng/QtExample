# LayoutExamplesTab2模块的qmake项目文件

QT += core gui widgets

CONFIG += c++17 sdk_no_version_check

# 创建共享库
TEMPLATE = lib
CONFIG += shared

TARGET = layoutexamplestab2

SOURCES += \
    layoutexamplestab2.cpp

HEADERS += \
    layoutexamplestab2.h

# 安装配置
target.path = $$[QT_INSTALL_LIBS]
INSTALLS += target

# 头文件安装
headers.files = $$HEADERS
headers.path = $$[QT_INSTALL_HEADERS]/layoutexamplestab2
INSTALLS += headers
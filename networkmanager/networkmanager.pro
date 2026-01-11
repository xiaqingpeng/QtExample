# NetworkManager模块的qmake项目文件

QT += core network

CONFIG += c++17 sdk_no_version_check

# 禁用过时的single_module链接器标志
CONFIG -= single_module

# 创建共享库
TEMPLATE = lib
CONFIG += shared

TARGET = networkmanager

SOURCES += \
    ../networkmanager.cpp

HEADERS += \
    ../networkmanager.h

# 安装配置
target.path = $$[QT_INSTALL_LIBS]
INSTALLS += target

# 头文件安装
headers.files = $$HEADERS
headers.path = $$[QT_INSTALL_HEADERS]/networkmanager
INSTALLS += headers
QT += core network

CONFIG += c++17 staticlib
CONFIG -= single_module

TEMPLATE = lib
TARGET = enterpriseinterfaces

HEADERS += \
    INetworkService.h \
    IAuthenticationService.h

# 添加一个空的源文件以满足静态库的要求
SOURCES += \
    interfaces_dummy.cpp

# 包含路径
INCLUDEPATH += ../..
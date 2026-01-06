QT += core gui widgets webenginewidgets network

CONFIG += c++17 staticlib sdk_no_version_check
CONFIG -= single_module

TEMPLATE = lib
TARGET = device

HEADERS += \
    serverconfigtab.h \
    contenttab.h

SOURCES += \
    serverconfigtab.cpp \
    contenttab.cpp

# 包含路径
INCLUDEPATH += ../..
INCLUDEPATH += ../Core
INCLUDEPATH += ../Services
INCLUDEPATH += ../Network
INCLUDEPATH += ../Styles
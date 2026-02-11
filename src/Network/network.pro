QT += core network widgets

CONFIG += c++17 staticlib
CONFIG -= single_module

TEMPLATE = lib
TARGET = network

HEADERS += \
    networkmanager.h \
    apiService.h \

SOURCES += \
    networkmanager.cpp \
     apiService.cpp \

# 包含路径
INCLUDEPATH += ../..
INCLUDEPATH += ../Core
INCLUDEPATH += ../Services
INCLUDEPATH += ../App

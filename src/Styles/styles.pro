QT += core gui widgets

CONFIG += c++17 staticlib
CONFIG -= single_module

TEMPLATE = lib
TARGET = styles

HEADERS += \
    theme_manager.h

SOURCES += \
    theme_manager.cpp

# 包含路径
INCLUDEPATH += ../..
INCLUDEPATH += ../Core
INCLUDEPATH += ../Utils
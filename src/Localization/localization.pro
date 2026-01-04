QT += core

CONFIG += c++17 staticlib
CONFIG -= single_module

TEMPLATE = lib
TARGET = enterpriselocalization

HEADERS += \
    LocalizationManager.h

SOURCES += \
    LocalizationManager.cpp

# 包含路径
INCLUDEPATH += ../..
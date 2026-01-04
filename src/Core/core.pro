QT += core gui widgets network

CONFIG += c++17 staticlib
CONFIG -= single_module

TEMPLATE = lib
TARGET = enterprisecore

HEADERS += \
    Application.h \
    Application.inl \
    ServiceManager.h \
    DependencyContainer.h

SOURCES += \
    Application.cpp \
    ServiceManager.cpp \
    DependencyContainer.cpp

# 包含路径
INCLUDEPATH += ../..
INCLUDEPATH += ../Localization
INCLUDEPATH += ../../styles
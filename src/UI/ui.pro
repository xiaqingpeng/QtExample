QT += core gui widgets webenginewidgets network

CONFIG += c++17 staticlib
CONFIG -= single_module

TEMPLATE = lib
TARGET = ui

HEADERS += \
    mainuiwindow.h

SOURCES += \
    mainuiwindow.cpp

# 包含路径
INCLUDEPATH += ../..
INCLUDEPATH += ../Core
INCLUDEPATH += ../Services
INCLUDEPATH += ../Network
INCLUDEPATH += ../Analytics
INCLUDEPATH += ../Auth
INCLUDEPATH += ../UserProfile
INCLUDEPATH += ../Charts
INCLUDEPATH += ../Reports
INCLUDEPATH += ../Styles
INCLUDEPATH += ../Utils
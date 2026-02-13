QT += core gui widgets network

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

INCLUDEPATH += ../Analytics
INCLUDEPATH += ../Auth
INCLUDEPATH += ../UserProfile
INCLUDEPATH += ../Charts
INCLUDEPATH += ../Reports
INCLUDEPATH += ../Device
INCLUDEPATH += ../Styles
INCLUDEPATH += ../Utils

# 链接库
LIBS += -L$$OUT_PWD/../Device -ldevice
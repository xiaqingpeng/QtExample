QT += core gui widgets network webenginewidgets

CONFIG += c++17 staticlib
CONFIG -= single_module

TEMPLATE = lib
TARGET = userprofile

HEADERS += \
    userinfopage.h \
    userprofiletab.h

SOURCES += \
    userinfopage.cpp \
    userprofiletab.cpp

# 包含路径
INCLUDEPATH += ../..
INCLUDEPATH += ../Core
INCLUDEPATH += ../Services
INCLUDEPATH += ../Network
INCLUDEPATH += ../Auth
INCLUDEPATH += ../Analytics
INCLUDEPATH += ../Styles
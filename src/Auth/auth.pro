QT += core gui widgets network

CONFIG += c++17 staticlib
CONFIG -= single_module

TEMPLATE = lib
TARGET = auth

HEADERS += \
    loginpage.h \
    LoginController.h \
    changepasswordpage.h

SOURCES += \
    loginpage.cpp \
    LoginController.cpp \
    changepasswordpage.cpp

# 包含路径
INCLUDEPATH += ../..
INCLUDEPATH += ../Core
INCLUDEPATH += ../Services
INCLUDEPATH += ../Network
INCLUDEPATH += ../Analytics
INCLUDEPATH += ../Styles
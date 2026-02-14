QT += core gui widgets network charts

CONFIG += c++17 staticlib
CONFIG -= single_module

defines += HAS_QT_CHARTS

TEMPLATE = lib
TARGET = userprofile

HEADERS += \
    userinfopage.h \
    userprofiletab.h \
    editprofilewidget.h

SOURCES += \
    userinfopage.cpp \
    userprofiletab.cpp \
    editprofilewidget.cpp

# 包含路径
INCLUDEPATH += ../..
INCLUDEPATH += ../Core
INCLUDEPATH += ../Services

INCLUDEPATH += ../Auth
INCLUDEPATH += ../Analytics
INCLUDEPATH += ../Styles
INCLUDEPATH += ../App

# 链接Services库
LIBS += -L$$OUT_PWD/../Services -lenterpriseservices
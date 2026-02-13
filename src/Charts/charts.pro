QT += core gui widgets network charts

CONFIG += c++17 staticlib
CONFIG -= single_module

TEMPLATE = lib
TARGET = charts

HEADERS += \
    echartstab.h \
    logstatstab.h \
    gaugewidget.h

SOURCES += \
    echartstab.cpp \
    logstatstab.cpp \
    gaugewidget.cpp

# 包含路径
INCLUDEPATH += ../..
INCLUDEPATH += ../Core
INCLUDEPATH += ../Services

INCLUDEPATH += ../Styles
INCLUDEPATH += ../App

# 链接Services库
LIBS += -L$$OUT_PWD/../Services -lenterpriseservices
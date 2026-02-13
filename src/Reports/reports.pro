QT += core gui widgets network charts

CONFIG += c++17 staticlib
CONFIG -= single_module

TEMPLATE = lib
TARGET = reports

HEADERS += \
    reportstab.h

SOURCES += \
    reportstab.cpp

# 包含路径
INCLUDEPATH += ../..
INCLUDEPATH += ../Core
INCLUDEPATH += ../Services

INCLUDEPATH += ../Analytics
INCLUDEPATH += ../Styles
INCLUDEPATH += ../App

# 链接Services库
LIBS += -L$$OUT_PWD/../Services -lenterpriseservices

# 包含资源文件
RESOURCES += ../../layoutexamplestab.qrc
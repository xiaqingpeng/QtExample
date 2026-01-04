QT += core gui widgets network webenginewidgets

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
INCLUDEPATH += ../Network
INCLUDEPATH += ../Analytics
INCLUDEPATH += ../Styles
# 包含资源文件
RESOURCES += ../../layoutexamplestab.qrc
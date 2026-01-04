QT += core gui widgets webenginewidgets network

CONFIG += c++17 staticlib
CONFIG -= single_module

TEMPLATE = lib
TARGET = charts

HEADERS += \
    echartstab.h \
    logstatstab.h

SOURCES += \
    echartstab.cpp \
    logstatstab.cpp

# 包含路径
INCLUDEPATH += ../..
INCLUDEPATH += ../Core
INCLUDEPATH += ../Services
INCLUDEPATH += ../Network
INCLUDEPATH += ../Styles
# 包含资源文件
RESOURCES += ../../layoutexamplestab.qrc
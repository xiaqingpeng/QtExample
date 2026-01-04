QT += core network

CONFIG += c++17 staticlib
CONFIG -= single_module

TEMPLATE = lib
TARGET = enterpriseservices

HEADERS += \
    NetworkService.h \
    AuthenticationService.h \
    AnalyticsService.h

SOURCES += \
    NetworkService.cpp \
    AuthenticationService.cpp \
    AnalyticsService.cpp

# 包含路径
INCLUDEPATH += ../..
INCLUDEPATH += ../Interfaces
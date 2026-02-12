QT += core network

CONFIG += c++17 staticlib
CONFIG -= single_module

TEMPLATE = lib
TARGET = enterpriseservices

HEADERS += \
    NetworkService.h \
    NetworkManagerAdapter.h \
    AuthenticationService.h \
    AnalyticsService.h \
    PersistentCookieJar.h \
    ApiService.h

SOURCES += \
    NetworkService.cpp \
    NetworkManagerAdapter.cpp \
    AuthenticationService.cpp \
    AnalyticsService.cpp \
    PersistentCookieJar.cpp \
    ApiService.cpp

# 包含路径
INCLUDEPATH += ../..
INCLUDEPATH += ../Interfaces
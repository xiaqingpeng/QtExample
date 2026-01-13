QT += core gui widgets network

# WebEngine 模块设为可选
# 如果系统中没有安装 Qt WebEngine，将禁用相关功能
qtHaveModule(webenginewidgets) {
    QT += webenginewidgets
    DEFINES += WEBENGINE_AVAILABLE
    message("WebEngine available - enabling web features")
} else {
    message("WebEngine not available - web features disabled")
}

CONFIG += c++17 staticlib
CONFIG -= single_module

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
INCLUDEPATH += ../Network
INCLUDEPATH += ../Auth
INCLUDEPATH += ../Analytics
INCLUDEPATH += ../Styles
INCLUDEPATH += ../App
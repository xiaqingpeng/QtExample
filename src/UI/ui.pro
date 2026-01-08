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
TARGET = ui

HEADERS += \
    mainuiwindow.h

SOURCES += \
    mainuiwindow.cpp

# 包含路径
INCLUDEPATH += ../..
INCLUDEPATH += ../Core
INCLUDEPATH += ../Services
INCLUDEPATH += ../Network
INCLUDEPATH += ../Analytics
INCLUDEPATH += ../Auth
INCLUDEPATH += ../UserProfile
INCLUDEPATH += ../Charts
INCLUDEPATH += ../Reports
INCLUDEPATH += ../Device
INCLUDEPATH += ../Styles
INCLUDEPATH += ../Utils

# 链接库
LIBS += -L$$OUT_PWD/../Device -ldevice
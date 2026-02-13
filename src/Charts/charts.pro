QT += core gui widgets network charts

# WebEngine 模块设为可选
# Windows平台禁用WebEngine以确保兼容性
win32 {
    message("Charts module: Windows platform detected - WebEngine features disabled for compatibility")
} else {
    # 非Windows平台检查WebEngine可用性
    qtHaveModule(webenginewidgets) {
        QT += webenginewidgets
        DEFINES += WEBENGINE_AVAILABLE
        message("Charts module: WebEngine available - enabling web features")
    } else {
        message("Charts module: WebEngine not available - web features disabled")
    }
}

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
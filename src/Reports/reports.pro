QT += core gui widgets network

# WebEngine 模块设为可选
# Windows平台禁用WebEngine以确保兼容性
win32 {
    message("Reports module: Windows platform detected - WebEngine features disabled for compatibility")
} else {
    # 非Windows平台检查WebEngine可用性
    qtHaveModule(webenginewidgets) {
        QT += webenginewidgets
        DEFINES += WEBENGINE_AVAILABLE
        message("Reports module: WebEngine available - enabling web features")
    } else {
        message("Reports module: WebEngine not available - web features disabled")
    }
}

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
INCLUDEPATH += ../App
# 包含资源文件
RESOURCES += ../../layoutexamplestab.qrc
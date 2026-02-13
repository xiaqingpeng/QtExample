QT += core gui widgets network charts

# WebEngine 模块设为可选
# 如果系统中没有安装 Qt WebEngine，将禁用相关功能
qtHaveModule(webenginewidgets) {
    QT += webenginewidgets
    DEFINES += WEBENGINE_AVAILABLE
    message("WebEngine available - enabling web features")
} else {
    message("WebEngine not available - web features disabled")
}

CONFIG += c++17 staticlib sdk_no_version_check
CONFIG -= single_module

TEMPLATE = lib
TARGET = device

HEADERS += \
    serverconfigtab.h \
    contenttab.h

SOURCES += \
    serverconfigtab.cpp \
    contenttab.cpp

# 包含路径
INCLUDEPATH += ../..
INCLUDEPATH += ../Core
INCLUDEPATH += ../Services
INCLUDEPATH += ../Styles

# 链接Services库
LIBS += -L$$OUT_PWD/../Services -lenterpriseservices

# 链接Charts库（包含GaugeWidget）
LIBS += -L$$OUT_PWD/../Charts -lcharts

# 资源文件
RESOURCES += ../../layoutexamplestab.qrc
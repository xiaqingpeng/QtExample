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

CONFIG += c++17 sdk_no_version_check

# 设置macOS部署目标版本为14.0，与Qt库版本兼容
QMAKE_MACOSX_DEPLOYMENT_TARGET = 14.0

# 创建共享库
TEMPLATE = lib
TARGET = echarts

# 添加包含路径
INCLUDEPATH += ..
INCLUDEPATH += ../styles
INCLUDEPATH += ../Services

# 链接Services库
LIBS += -L../Services -lenterpriseservices

# 链接Analytics库
LIBS += -L../Analytics -lanalytics

# 链接styles库
LIBS += -L../styles -lstyles

SOURCES += \
    echartstab.cpp \
    logstatstab.cpp

HEADERS += \
    echartstab.h \
    logstatstab.h

# 包含analytics头文件
INCLUDEPATH += ../analytics

RESOURCES += \
    ../layoutexamplestab.qrc

# 设置库文件安装路径
lib.path = $$[QT_INSTALL_PLUGINS]/../libs
lib.files = $$OUT_PWD/lib$$TARGET*.dylib
INSTALLS += lib

# 设置头文件安装路径
headers.path = $$[QT_INSTALL_PLUGINS]/../headers/echarts
headers.files = $$HEADERS
INSTALLS += headers
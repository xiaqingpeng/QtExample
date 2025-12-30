QT += core gui widgets webenginewidgets network

CONFIG += c++17 sdk_no_version_check

# 设置macOS部署目标版本为26.0，与Qt库版本兼容
QMAKE_MACOSX_DEPLOYMENT_TARGET = 26.0

# 创建共享库
TEMPLATE = lib
TARGET = echarts

# 添加networkmanager.h的绝对路径
INCLUDEPATH += /Applications/qingpengxia/qt/qt6/example

# 链接networkmanager库
LIBS += -L$$OUT_PWD/../networkmanager -lnetworkmanager

SOURCES += \
    echartstab.cpp \
    logstatstab.cpp

HEADERS += \
    echartstab.h \
    logstatstab.h

# 设置库文件安装路径
lib.path = $$[QT_INSTALL_PLUGINS]/../libs
lib.files = $$OUT_PWD/lib$$TARGET*.dylib
INSTALLS += lib

# 设置头文件安装路径
headers.path = $$[QT_INSTALL_PLUGINS]/../headers/echarts
headers.files = $$HEADERS
INSTALLS += headers
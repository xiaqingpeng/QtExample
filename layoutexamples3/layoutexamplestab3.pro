# LayoutExamplesTab3模块的qmake项目文件

QT += core gui widgets network

CONFIG += c++17 sdk_no_version_check

# 创建共享库
TEMPLATE = lib
CONFIG += shared

TARGET = layoutexamplestab3

# 添加networkmanager.h的绝对路径
INCLUDEPATH += /Applications/qingpengxia/qt/qt6/example

# 链接networkmanager库
LIBS += -L$$OUT_PWD/../networkmanager -lnetworkmanager

SOURCES += \
    layoutexamplestab3.cpp

HEADERS += \
    layoutexamplestab3.h

# 安装配置
target.path = $$[QT_INSTALL_LIBS]
INSTALLS += target

# 头文件安装
headers.files = $$HEADERS
headers.path = $$[QT_INSTALL_HEADERS]/layoutexamplestab3
INSTALLS += headers
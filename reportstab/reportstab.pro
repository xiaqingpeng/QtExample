QT += core gui widgets webenginewidgets network

CONFIG += c++17 sdk_no_version_check

# 禁用过时的single_module链接器标志
CONFIG -= single_module

# 设置macOS部署目标版本为14.0，与Qt库版本兼容
QMAKE_MACOSX_DEPLOYMENT_TARGET = 14.0

# 创建共享库
TEMPLATE = lib
TARGET = reportstab

# 添加networkmanager.h的相对路径
INCLUDEPATH += ..

# 链接networkmanager库
LIBS += -L$$OUT_PWD/../networkmanager -lnetworkmanager

SOURCES += \
    reportstab.cpp

# 添加styles库依赖
LIBS += -L$$OUT_PWD/../styles -lstyles

HEADERS += \
    reportstab.h

# 添加资源文件
RESOURCES += \
    ../layoutexamplestab.qrc

# 设置库文件安装路径
lib.path = $$[QT_INSTALL_PLUGINS]/../libs
lib.files = $$OUT_PWD/lib$$TARGET*.dylib
INSTALLS += lib

# 设置头文件安装路径
headers.path = $$[QT_INSTALL_PLUGINS]/../headers/reportstab
headers.files = $$HEADERS
INSTALLS += headers
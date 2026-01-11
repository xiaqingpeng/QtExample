QT += core gui widgets  network

CONFIG += c++17 sdk_no_version_check

# 禁用过时的single_module链接器标志
CONFIG -= single_module

# 设置macOS部署目标版本为14.0，与Qt库版本兼容
QMAKE_MACOSX_DEPLOYMENT_TARGET = 14.0

# 创建共享库
TEMPLATE = lib
TARGET = echarts

# 添加networkmanager.h和styles的相对路径
INCLUDEPATH += ..
INCLUDEPATH += ../styles

# 链接networkmanager库和styles库
LIBS += -L../networkmanager -lnetworkmanager
LIBS += -L../styles -lstyles

SOURCES += \
    logstatstab.cpp \
    ../analytics/analytics.cpp

HEADERS += \
    logstatstab.h \
    ../analytics/analytics.h

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